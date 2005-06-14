
#include "enough.h"
#include "p_sds_geo.h"
#include "p_sds_table.h"
#include "p_sds_obj.h"
#include "p_task.h"

uint p_lod_get_base_corner_next(PPolyStore *mesh, uint corner, uint move)
{
	if(corner < mesh->base_quad_count * 4)
		return ((corner / 4) * 4) + (corner + (uint)(4 + move)) % 4;
	else
	{
		uint a;
		a = corner - (mesh->base_quad_count * 4);
		return (mesh->base_quad_count * 4) + ((a / 3) * 3) + (a + (uint)(3 + move)) % 3;
	}
}

void p_lod_compute_vertex_normals(PPolyStore *smesh, PMesh *mesh)
{
	uint i, j, corner, other_corner, tmp, length;
	uint *corner_normals;
	length = (smesh->base_tri_count * 3 + smesh->base_quad_count * 4);

	mesh->temp = corner_normals = malloc((sizeof *corner_normals) * length * 2);

	for(i = 0; i < length * 2; i += 2)
		corner_normals[i] = -1;

	for(i = 0; i < length; i++)
	{
		if(corner_normals[i * 2] == -1)
		{
			corner = other_corner = i;

			for(j = 0; j < 100; j++)
			{
				tmp = smesh->base_neighbor[corner];
				if(tmp != -1 && smesh->crease[tmp] > 0.5)
					corner = p_lod_get_base_corner_next(smesh, tmp, 1);
				else
				{
					tmp = -1;
					break;
				}
				if(j % 2 == 1)
				{
					other_corner = smesh->base_neighbor[other_corner];
					other_corner = p_lod_get_base_corner_next(smesh, other_corner, 1);
				}
				if(corner == i)
					break;
			}
			if(corner != i)
			{
				other_corner = i;
				tmp = p_lod_get_base_corner_next(smesh, i, -1);
				for(j = 0; other_corner != -1 && j < 100; j++)
				{
					tmp = smesh->base_neighbor[tmp];
					if(tmp != -1 && smesh->crease[tmp] > 0.5)
					{
						other_corner = tmp;
						tmp = p_lod_get_base_corner_next(smesh, tmp, -1);
					}else
						break;
					if(tmp == i)
						break;
				}
			}

			corner_normals[i * 2] = corner;
			corner_normals[i * 2 + 1] = other_corner;
			tmp = i;
			for(j = 0; j < 100; j++)
			{
				tmp = smesh->base_neighbor[tmp];
				if(tmp != -1 && smesh->crease[tmp] > 0.5)
					tmp = p_lod_get_base_corner_next(smesh, tmp, 1);
				else
					break;
				corner_normals[tmp * 2] = corner;
				corner_normals[tmp * 2 + 1] = other_corner;
				if(tmp == i)
					break;
			}

			tmp = p_lod_get_base_corner_next(smesh, i, -1);
			for(j = 0; j < 100; j++)
			{
				tmp = smesh->base_neighbor[tmp];
				if(tmp == -1 || smesh->crease[tmp] < 0.5)
					break;
				corner_normals[tmp * 2] = corner;
				corner_normals[tmp * 2 + 1] = other_corner;
				tmp = p_lod_get_base_corner_next(smesh, tmp, -1);
				if(tmp == i)
					break;
			}
		}
	}
}

uint p_lod_edge_shadow_length_quad(PPolyStore *smesh, PMesh *mesh, uint poly, uint edge, uint *crease, egreal def)
{
	poly = mesh->tess.order_temp_mesh[poly];
	if(smesh->base_neighbor[poly * 4 + edge] == -1)
		return FALSE;
	if(crease != NULL)
		def = crease[poly * 4 + edge];
	if(smesh->base_neighbor[poly * 4 + edge] > poly * 4 + edge)
		return FALSE;
	if(def > (uint)-10)
		return FALSE;
	else
		return TRUE;
}


boolean p_lod_handle_edge(PPolyStore *smesh, PMesh *mesh, uint current_poly, uint current_edge)
{
	PTessTableElement *current_table, *table;
	uint poly, edge, i, j, wrap, current_wrap;
	boolean crease;

	if(mesh->tess.order_temp_mesh[current_poly] > smesh->base_quad_count)
		poly = smesh->base_neighbor[smesh->base_quad_count * 4 + (mesh->tess.order_temp_mesh[current_poly] - smesh->base_quad_count) * 3 + current_edge];
	else
		poly = smesh->base_neighbor[mesh->tess.order_temp_mesh[current_poly] * 4 + current_edge];
	if(poly == -1)
		return FALSE;
	if(poly / 4 < smesh->base_quad_count)
	{
		crease = smesh->crease[poly] > 0.1;
		edge = poly % 4;
		poly /= 4;
	}else
	{
		crease = smesh->crease[poly] > 0.1;
		edge = (poly - smesh->base_quad_count * 4) % 3;
		poly = smesh->base_quad_count + (poly - smesh->base_quad_count * 4) / 3;
	}
	poly = mesh->tess.order_temp_mesh_rev[poly];
	table = mesh->tess.tess[poly];
	current_table = mesh->tess.tess[current_poly];
	wrap = table->edges[4];
	current_wrap = current_table->edges[4];
	j = table->edges[edge + 1];

	if(crease)
	{
		for(i = current_table->edges[current_edge] + 1; i < current_table->edges[current_edge + 1]; i++)
		{
			j--;
			mesh->normal.normal_ref[mesh->sub_stages[1]++] = mesh->tess.order_temp_poly_start[current_poly] + current_table->normals[i * 4 + 2];
			mesh->normal.normal_ref[mesh->sub_stages[1]++] = mesh->tess.order_temp_poly_start[current_poly] + current_table->normals[i * 4 + 3];
			mesh->normal.normal_ref[mesh->sub_stages[1]++] = mesh->tess.order_temp_poly_start[poly] + table->normals[j * 4 + 2];
			mesh->normal.normal_ref[mesh->sub_stages[1]++] = mesh->tess.order_temp_poly_start[poly] + table->normals[j * 4 + 3];
		}
		return TRUE;
	}else
	{
		if(poly > current_poly)
		{
			for(i = current_table->edges[current_edge]; i < current_table->edges[current_edge + 1]; i++)
			{
				mesh->render.reference[mesh->render.element_count++] = mesh->tess.order_temp_poly_start[current_poly] + (i + 1) % current_wrap;
				mesh->render.reference[mesh->render.element_count++] = mesh->tess.order_temp_poly_start[current_poly] + i % current_wrap;
				mesh->render.reference[mesh->render.element_count++] = mesh->tess.order_temp_poly_start[poly] + j % wrap;

				mesh->render.reference[mesh->render.element_count++] = mesh->tess.order_temp_poly_start[current_poly] + (i + 1) % current_wrap;
				mesh->render.reference[mesh->render.element_count++] = mesh->tess.order_temp_poly_start[poly] + j % wrap;
				mesh->render.reference[mesh->render.element_count++] = mesh->tess.order_temp_poly_start[poly] + (j + wrap - 1) % wrap;
				j--;
			}
		}
		return FALSE;
	}
	return FALSE;
}


boolean crease_edge(PPolyStore *smesh, PMesh *mesh, uint edge)
{
	return FALSE;
}


void p_rm_create_vertex_normals(uint *output, PMesh *mesh, uint poly, uint corner)
{
	PTessTableElement *corner_table;
	uint *vertex_normals, corner_poly, corner_corner, corner_start;
	vertex_normals = &((uint32 *)mesh->temp)[mesh->tess.order_temp_mesh[poly] * 8];

	corner_poly = (vertex_normals[corner * 2] / 4);
	corner_corner = (vertex_normals[corner * 2] % 4);
	corner_table = mesh->tess.tess[mesh->tess.order_temp_mesh_rev[corner_poly]];
	corner_start = mesh->tess.order_temp_poly_start[corner_poly];
	output[0] = corner_start + corner_table->normals[corner_table->edges[corner_corner] * 4];
	output[1] = corner_start + corner_table->normals[corner_table->edges[corner_corner] * 4 + 1];
	
	corner_poly = (vertex_normals[corner * 2 + 1] / 4);
	corner_corner = (vertex_normals[corner * 2 + 1] % 4);
	corner_table = mesh->tess.tess[mesh->tess.order_temp_mesh_rev[corner_poly]];
	corner_start = mesh->tess.order_temp_poly_start[corner_poly];
	output[2] = corner_start + corner_table->normals[corner_table->edges[corner_corner] * 4 + 2];
	output[3] = corner_start + corner_table->normals[corner_table->edges[corner_corner] * 4 + 1];
}

void p_lod_create_normal_ref_and_shadow_skirts(PPolyStore *smesh, PMesh *mesh)
{
	PTessTableElement *table;
	uint i, j, start, corners, *vertex_normals;
	vertex_normals = mesh->temp;
	for(; mesh->sub_stages[0] < mesh->tess.tri_count + mesh->tess.quad_count; mesh->sub_stages[0]++)
	{
		if(mesh->sub_stages[0] == mesh->render.mat[mesh->sub_stages[2]].tri_end)
			mesh->sub_stages[2]++;
		if(mesh->sub_stages[0] < mesh->render.mat[mesh->sub_stages[2]].quad_end)
			corners = 4;
		else
			corners = 3;

		table = mesh->tess.tess[mesh->sub_stages[0]];
		start = mesh->tess.order_temp_poly_start[mesh->sub_stages[0]];
		if(mesh->sub_stages[0] < mesh->render.mat[mesh->sub_stages[2]].quad_end)
		{
			for(i = 0; i < 4; i++) /* quads */
			{
			//	printf("poly %u corner %u start %u table %p\n", mesh->sub_stages[0], i, start, table);

				p_rm_create_vertex_normals(&mesh->normal.normal_ref[mesh->sub_stages[1]], mesh, mesh->sub_stages[0], i);
				mesh->sub_stages[1] += 4;

/*				printf("is = %u %u %u %u \n",
					mesh->normal.normal_ref[mesh->sub_stages[1]],
					mesh->normal.normal_ref[mesh->sub_stages[1] + 1],
					mesh->normal.normal_ref[mesh->sub_stages[1] + 2],
					mesh->normal.normal_ref[mesh->sub_stages[1] + 3]);

				mesh->normal.normal_ref[mesh->sub_stages[1]++] = start + table->normals[table->edges[i] * 4];
				mesh->normal.normal_ref[mesh->sub_stages[1]++] = start + table->normals[table->edges[i] * 4 + 1];
				mesh->normal.normal_ref[mesh->sub_stages[1]++] = start + table->normals[table->edges[i] * 4 + 2];
				mesh->normal.normal_ref[mesh->sub_stages[1]++] = start + table->normals[table->edges[i] * 4 + 3];
*/
//				mesh->sub_stages[1] += 4;

				if(!p_lod_handle_edge(smesh, mesh, mesh->sub_stages[0], i))
				{
					for(j = table->edges[i] + 1; j < table->edges[i + 1]; j++)
					{
						mesh->normal.normal_ref[mesh->sub_stages[1]++] = start + table->normals[j * 4];
						mesh->normal.normal_ref[mesh->sub_stages[1]++] = start + table->normals[j * 4 + 1];
						mesh->normal.normal_ref[mesh->sub_stages[1]++] = start + table->normals[j * 4 + 2];
						mesh->normal.normal_ref[mesh->sub_stages[1]++] = start + table->normals[j * 4 + 3];
					}
				}
			}
		}else
		{
			for(i = 0; i < 3; i++) /* triangles */
			{
				mesh->normal.normal_ref[mesh->sub_stages[1]++] = start + table->normals[table->edges[i] * 4];
				mesh->normal.normal_ref[mesh->sub_stages[1]++] = start + table->normals[table->edges[i] * 4 + 1];
				mesh->normal.normal_ref[mesh->sub_stages[1]++] = start + table->normals[table->edges[i] * 4 + 2];
				mesh->normal.normal_ref[mesh->sub_stages[1]++] = start + table->normals[table->edges[i] * 4 + 3];
				if(!p_lod_handle_edge(smesh, mesh, mesh->sub_stages[0], i))
				{
					for(j = table->edges[i] + 1; j < table->edges[i + 1]; j++)
					{
						mesh->normal.normal_ref[mesh->sub_stages[1]++] = start + table->normals[j * 4];
						mesh->normal.normal_ref[mesh->sub_stages[1]++] = start + table->normals[j * 4 + 1];
						mesh->normal.normal_ref[mesh->sub_stages[1]++] = start + table->normals[j * 4 + 2];
						mesh->normal.normal_ref[mesh->sub_stages[1]++] = start + table->normals[j * 4 + 3];
					}
				}
			}
		}
		for(j = table->edges[i]; j < table->vertex_count; j++)
		{
			mesh->normal.normal_ref[mesh->sub_stages[1]++] = start + table->normals[j * 4];
			mesh->normal.normal_ref[mesh->sub_stages[1]++] = start + table->normals[j * 4 + 1];
			mesh->normal.normal_ref[mesh->sub_stages[1]++] = start + table->normals[j * 4 + 2];
			mesh->normal.normal_ref[mesh->sub_stages[1]++] = start + table->normals[j * 4 + 3];
		}
	}
	if(mesh->sub_stages[0] == mesh->tess.tri_count + mesh->tess.quad_count)
	{
		mesh->stage++;
		free(mesh->temp);
		mesh->temp = NULL;
		mesh->sub_stages[0] = 0;
		mesh->sub_stages[1] = 0;
		mesh->sub_stages[2] = 0;
		mesh->sub_stages[3] = 0;
	}
}
