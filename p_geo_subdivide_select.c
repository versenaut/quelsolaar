#include "e_includes.h"

//#include <pthread.h>
#include <math.h>
#include "verse.h"
#include "p_geo_subdivide_internal.h"


egreal edge_tesselation_global_func(egreal *v_0, egreal *v_1, egreal *e_0, egreal *eay)
{
	egreal length, edge, vector[3];
	length = ((v_0[0] - v_1[0]) * (v_0[0] - v_1[0])) + ((v_0[1] - v_1[1]) * (v_0[1] - v_1[1])) + ((v_0[2] - v_1[2]) * (v_0[2] - v_1[2]));

	vector[0] = (e_0[0] - ((v_0[0] + v_1[0]) * 0.5));
	vector[1] = (e_0[1] - ((v_0[1] + v_1[1]) * 0.5));
	vector[2] = (e_0[2] - ((v_0[2] + v_1[2]) * 0.5));
	edge = (vector[0] * vector[0]) + (vector[1] * vector[1]) + (vector[2] * vector[2]);
	return sqrt(edge / length);
}

egreal edge_tesselation_local_func(egreal *v_0, egreal *v_1, egreal *e_0, egreal *eay)
{
	egreal length, edge, dist, vector[3];
	length = ((v_0[0] - v_1[0]) * (v_0[0] - v_1[0])) + ((v_0[1] - v_1[1]) * (v_0[1] - v_1[1])) + ((v_0[2] - v_1[2]) * (v_0[2] - v_1[2]));

	vector[0] = (e_0[0] - ((v_0[0] + v_1[0]) * 0.5));
	vector[1] = (e_0[1] - ((v_0[1] + v_1[1]) * 0.5));
	vector[2] = (e_0[2] - ((v_0[2] + v_1[2]) * 0.5));
	edge = (vector[0] * vector[0]) + (vector[1] * vector[1]) + (vector[2] * vector[2]);
	vector[0] = (e_0[0] + eay[0]);
	vector[1] = (e_0[1] + eay[1]);
	vector[2] = (e_0[2] + eay[2]);
	dist = 1 / ((vector[0] * vector[0]) + (vector[1] * vector[1]) + (vector[2] * vector[2]));
	return sqrt((edge / length) * dist);
}

/* this function selects how a polygon shuld be tesselated */


uint p_polygon_split_count(uint level)
{
	uint i,	polygon_size = 4;
	for(i = 1 ; i < level; i++)
		polygon_size *= 4;
	return polygon_size;
}/*

static boolean p_seclect_mesh_tesselation(PMesh *output, const SubMesh *mesh, preal *vertex, preal tess_factor, uint force_tess, preal *eay)
{
	PTessTableElement	*tess_element;
	uint i, j, k, l, polygon_size, vertex_size, *index, poly_count, vertex_count = 0, element_count = 0, edge[4], ref;
	SubVertexDepend *dep;
	Point edge_data[9];
	SubPrecision value, *vertex_data; 
	preal (*edge_tess_func)(preal *v_0, preal *v_1, preal *e_0, preal *eay);

	if(eay == NULL)
		edge_tess_func = edge_tesselation_global_func;
	else
		edge_tess_func = edge_tesselation_local_func;

	polygon_size = 4;
	vertex_size = 2;
	for(i = 1 ; i < mesh->base_level; i++)
	{
		polygon_size *= 4;
		vertex_size *= 2;
	}
	vertex_size = ((vertex_size + 1) * (vertex_size * 2)) / 2;
	vertex_size = 15;
	poly_count = mesh->tri_count / polygon_size;
 	output->tess.tri_count = mesh->tri_count / polygon_size;
 	output->tess.quad_count = mesh->quad_count / polygon_size;

	output->tess.tri_tess = malloc((sizeof *output->tri_tess) * poly_count);
	edge[0] = 1;
	edge[1] = 1;
	edge[2] = 1;
	edge[3] = 1;	
	index = get_dynamic_table_tri(mesh->base_level, edge)->reference;
	for(i = 0; i < poly_count; i++) 
	{
		for(j = 0; j < 6; j++)
		{
			ref = mesh->tri_reference[index[j] + i * polygon_size * 3];
			dep = &mesh->vertex_depend_temp[ref];
			edge_data[j].x = 0;
			edge_data[j].y = 0;
			edge_data[j].z = 0;
			for(k = 0; k < dep->length; k++)
			{
				value = dep->element[k].value;
				vertex_data = vertex[dep->element[k].vertex_ref * 3];
				edge_data[j].x += value * vertex_data[0];
				edge_data[j].y += value * vertex_data[1];
				edge_data[j].z += value * vertex_data[2];
			}
		}

		edge[0] = edge_tesselation(&edge_data[0], &edge_data[5], &edge_data[1]) * tess_factor;
		edge[1] = edge_tesselation(&edge_data[5], &edge_data[4], &edge_data[3]) * tess_factor;
		edge[2] = edge_tesselation(&edge_data[4], &edge_data[0], &edge_data[2]) * tess_factor;
		if(edge[0] < force_tess)
			edge[0] = force_tess;
		if(edge[1] < force_tess)
			edge[1] = force_tess;
		if(edge[2] < force_tess)
			edge[2] = force_tess;
		if(edge[0] > mesh->base_level)
			edge[0] = mesh->base_level;
		if(edge[1] > mesh->base_level)
			edge[1] = mesh->base_level;
		if(edge[2] > mesh->base_level)
			edge[2] = mesh->base_level;
		tess_element = get_dynamic_table_tri(mesh->base_level, &edge[0]);
		element_count += tess_element->element_count;
		vertex_count += tess_element->vertex_count;
		output->tri_tess[i] = tess_element;
	}

	poly_count = mesh->quad_count / polygon_size;
	output->quad_tess = malloc((sizeof *output->quad_tess) * poly_count);

	edge[0] = 1;
	edge[1] = 1;
	edge[2] = 1;
	edge[3] = 1;
	index = get_dynamic_table_quad(mesh->base_level, &edge[0])->reference;
	for(i = 0; i < poly_count; i++) 
	{
		for(j = 0; j < 9; j++)
		{
			ref = mesh->quad_reference[index[j] + i * polygon_size * 4];
			dep = &mesh->vertex_depend_temp[ref];
			edge_data[j].x = 0;
			edge_data[j].y = 0;
			edge_data[j].z = 0;
			for(k = 0; k < dep->length; k++)
			{
				value = dep->element[k].value;
				vertex_data = nglGetVertexPointer(vertex_array, dep->element[k].vertex_ref);
				edge_data[j].x += value * vertex_data[0];
				edge_data[j].y += value * vertex_data[1];
				edge_data[j].z += value * vertex_data[2];
			}
		}
		edge[0] = edge_tesselation(&edge_data[0], &edge_data[4], &edge_data[2]) * tess_factor;
		edge[1] = edge_tesselation(&edge_data[4], &edge_data[8], &edge_data[5]) * tess_factor;
		edge[2] = edge_tesselation(&edge_data[8], &edge_data[6], &edge_data[7]) * tess_factor;
		edge[3] = edge_tesselation(&edge_data[6], &edge_data[0], &edge_data[1]) * tess_factor;
		
		if(edge[0] < force_tess)
			edge[0] = force_tess;
		if(edge[1] < force_tess)
			edge[1] = force_tess;
		if(edge[2] < force_tess)
			edge[2] = force_tess;
		if(edge[3] < force_tess)
			edge[3] = force_tess;
		
		if(edge[0] > mesh->base_level)
			edge[0] = mesh->base_level;
		if(edge[1] > mesh->base_level)
			edge[1] = mesh->base_level;
		if(edge[2] > mesh->base_level)
			edge[2] = mesh->base_level;
		if(edge[3] > mesh->base_level)
			edge[3] = mesh->base_level;


		tess_element = get_dynamic_table_quad(mesh->base_level, &edge[0]);
		element_count += tess_element->element_count;
		vertex_count += tess_element->vertex_count;
		output->quad_tess[i] = tess_element;
	}

	output->element_count = element_count;
	output->vertex_count = vertex_count;
	return TRUE;

}
*/
void p_get_tri_tess_index(uint *index, uint base_tess)
{
	PTessTableElement	*table;
	uint edge[3] = {1, 1, 1};
	table = get_dynamic_table_tri(base_tess, edge);
	index[0] = table->reference[0];
	index[1] = table->reference[1];
	index[2] = table->reference[2];
	index[3] = table->reference[3];
	index[4] = table->reference[4];
	index[5] = table->reference[5];
}

PTessTableElement	*p_select_tri_tesselation(const SubMesh *mesh, const egreal *vertex, uint *index, uint polygon_times_size_times_three, egreal tess_factor, uint force_tess, egreal (*edge_tess_func)(egreal *v_0, egreal *v_1, egreal *e_0, egreal *eay), egreal *eay)
{
	SubVertexDepend *dep;
	uint i, j, pos, edge[4];
	egreal value, edge_data[3 * 6];
	for(i = 0; i < 6; i++)
	{
		dep = &mesh->vertex_depend_temp[mesh->tri_reference[index[i] + polygon_times_size_times_three]];
		pos = dep->element[0].vertex_ref * 3;
		edge_data[i * 3] = value * vertex[pos + 0];
		edge_data[i * 3 + 1] = value * vertex[pos + 1];
		edge_data[i * 3 + 2] = value * vertex[pos + 2];
		for(j = 1; j < dep->length; j++)
		{
			value = dep->element[j].value;
			pos = dep->element[j].vertex_ref * 3;
			edge_data[i * 3] += value * vertex[pos + 0];
			edge_data[i * 3 + 1] += value * vertex[pos + 1];
			edge_data[i * 3 + 2] += value * vertex[pos + 2];
		}
	}
		edge[0] = edge_tess_func(&edge_data[0], &edge_data[5], &edge_data[1], eay) * tess_factor;
		edge[1] = edge_tess_func(&edge_data[5], &edge_data[4], &edge_data[3], eay) * tess_factor;
		edge[2] = edge_tess_func(&edge_data[4], &edge_data[0], &edge_data[2], eay) * tess_factor;
	if(edge[0] < force_tess)
		edge[0] = force_tess;
	if(edge[1] < force_tess)
		edge[1] = force_tess;
	if(edge[2] < force_tess)
		edge[2] = force_tess;
	if(edge[0] > mesh->base_level)
		edge[0] = mesh->base_level;
	if(edge[1] > mesh->base_level)
		edge[1] = mesh->base_level;
	if(edge[2] > mesh->base_level)
		edge[2] = mesh->base_level;
	return get_dynamic_table_tri(mesh->base_level, edge);
}

void p_get_quad_tess_index(uint *index, uint base_tess)
{
	PTessTableElement	*table;
	uint edge[4] = {1, 1, 1, 1};
	table = get_dynamic_table_quad(base_tess, edge);
	index[0] = table->reference[0];
	index[1] = table->reference[1];
	index[2] = table->reference[2];
	index[3] = table->reference[4];
	index[4] = table->reference[5];
	index[5] = table->reference[6];
	index[6] = table->reference[7];
	index[7] = table->reference[8];
}

PTessTableElement	*p_select_quad_tesselation(const SubMesh *mesh, const egreal *vertex, uint *index, uint polygon_times_size_times_four, egreal tess_factor, uint force_tess, egreal (*edge_tess_func)(egreal *v_0, egreal *v_1, egreal *e_0, egreal *eay), egreal *eay)
{
	SubVertexDepend *dep;
	uint i, j, pos, edge[4];
	egreal value, edge_data[3 * 9];
//	printf("looking for an edge\n");
	for(i = 0; i < 8; i++)
	{
		dep = &mesh->vertex_depend_temp[mesh->quad_reference[polygon_times_size_times_four + index[i]]];
		value = dep->element[0].value;
		pos = dep->element[0].vertex_ref * 3;
		edge_data[i * 3] = value * vertex[pos + 0];
		edge_data[i * 3 + 1] = value * vertex[pos + 1];
		edge_data[i * 3 + 2] = value * vertex[pos + 2];
//		printf("ref = %u %u %u ", polygon_times_size_times_four, index[i], mesh->quad_reference[polygon_times_size_times_four + index[i]]);
		for(j = 1; j < dep->length; j++)
		{
			value = dep->element[j].value;
			pos = dep->element[j].vertex_ref * 3;
			edge_data[i * 3] += value * vertex[pos + 0];
			edge_data[i * 3 + 1] += value * vertex[pos + 1];
			edge_data[i * 3 + 2] += value * vertex[pos + 2];

		}
//		printf("pos = %i, %f %f %f\n", i, edge_data[i * 3], edge_data[i * 3 + 1], edge_data[i * 3 + 2]);
	}
	edge[0] = edge_tess_func(&edge_data[0 * 3], &edge_data[3 * 3], &edge_data[2 * 3], eay) * tess_factor;
	edge[1] = edge_tess_func(&edge_data[3 * 3], &edge_data[7 * 3], &edge_data[4 * 3], eay) * tess_factor;
	edge[2] = edge_tess_func(&edge_data[7 * 3], &edge_data[5 * 3], &edge_data[6 * 3], eay) * tess_factor;
	edge[3] = edge_tess_func(&edge_data[5 * 3], &edge_data[0 * 3], &edge_data[1 * 3], eay) * tess_factor;
/*	edge[0] = (uint)(edge_tess_func(&edge_data[0 * 3], &edge_data[4 * 3], &edge_data[2 * 3], eay) * tess_factor);
	edge[1] = (uint)(edge_tess_func(&edge_data[4 * 3], &edge_data[8 * 3], &edge_data[5 * 3], eay) * tess_factor);
	edge[2] = (uint)(edge_tess_func(&edge_data[8 * 3], &edge_data[6 * 3], &edge_data[7 * 3], eay) * tess_factor);
	edge[3] = (uint)(edge_tess_func(&edge_data[6 * 3], &edge_data[0 * 3], &edge_data[1 * 3], eay) * tess_factor);
*/
	if(edge[0] < force_tess)
		edge[0] = force_tess;
	if(edge[1] < force_tess)
		edge[1] = force_tess;
	if(edge[2] < force_tess)
		edge[2] = force_tess;
	if(edge[3] < force_tess)
		edge[3] = force_tess;
	if(edge[0] > mesh->base_level)
		edge[0] = mesh->base_level;
	if(edge[1] > mesh->base_level)
		edge[1] = mesh->base_level;
	if(edge[2] > mesh->base_level)
		edge[2] = mesh->base_level;
	if(edge[3] > mesh->base_level)
		edge[3] = mesh->base_level;
	return get_dynamic_table_quad(mesh->base_level, edge);
}
