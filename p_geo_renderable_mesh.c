
#include "enough.h"
#include "p_geo_subdivide_internal.h"

extern void p_get_tri_tess_index(uint *index, uint base_tess);
extern void p_get_quad_tess_index(uint *index, uint base_tess);
extern PTessTableElement	*p_select_tri_tesselation(const SubMesh *mesh, const egreal *vertex, uint *index, uint polygon_times_size_times_three, egreal tess_factor, uint force_tess, egreal (*edge_tess_func)(egreal *v_0, egreal *v_1, egreal *e_0, egreal *eay), egreal *eay);
extern PTessTableElement	*p_select_quad_tesselation(const SubMesh *mesh, const egreal *vertex, uint *index, uint polygon_times_size_times_four, egreal tess_factor, uint force_tess, egreal (*edge_tess_func)(egreal *v_0, egreal *v_1, egreal *e_0, egreal *eay), egreal *eay);

extern egreal edge_tesselation_global_func(egreal *v_0, egreal *v_1, egreal *e_0, egreal *eay);
extern egreal edge_tesselation_local_func(egreal *v_0, egreal *v_1, egreal *e_0, egreal *eay);


PMesh *p_rm_create(ENode *node)
{
	PMesh *mesh;
	if(e_ns_get_node_type(node) != V_NT_GEOMETRY)
		return NULL;
	mesh = malloc(sizeof *mesh);
	mesh->geometry_id =	e_ns_get_node_id(node);
	mesh->geometry_version = e_ns_get_node_version_struct(node);
	mesh->stage = 0;
	mesh->sub_stage = 0;
	mesh->tess.force = 1;
	mesh->tess.factor = 50;
	mesh->tess.edge_tess_func = edge_tesselation_global_func;
	return mesh;
}

void p_rm_set_eay(PMesh *mesh, egreal *eay)
{
	mesh->tess.eay[0] = eay[0];
	mesh->tess.eay[1] = eay[1];
	mesh->tess.eay[2] = eay[2];
	mesh->tess.edge_tess_func = edge_tesselation_local_func;
}


void p_rm_destroy(PMesh *mesh)
{
	if(mesh->stage > 0)
	{
		free(mesh->tess.tri_tess);
		free(mesh->tess.quad_tess);
		free(mesh->normal.reference);
		free(mesh->normal.count);
		if(mesh->stage > 3)
		{
			free(mesh->render.vertex_array);
			free(mesh->render.normal_array);
			free(mesh->render.reference);
			free(mesh->depend.reference);
			free(mesh->depend.weight);
			free(mesh->depend.ref_count);
			free(mesh->normal.weight);
		}
	}
	free(mesh);
}

boolean p_rm_validate(PMesh *mesh)
{
	ENode *node;
	SubMesh *smesh;
	node = e_ns_get_node(0, mesh->geometry_id);
	if(node == NULL)
		return FALSE;
	smesh = p_geometry_get_sds(node);
	if(smesh == NULL || mesh->geometry_version != smesh->version)
		return FALSE;
	return TRUE;
}

boolean p_rm_drawable(PMesh *mesh)
{
	return mesh->stage == 9;
}

#define P_TRI_TESS_SELECT 10000
#define P_QUAD_TESS_SELECT 7500
#define P_TRI_TESS_REF 10000
#define P_QUAD_TESS_REF 7500

extern void p_rm_compute(PMesh *mesh, const egreal *vertex);

void p_rm_service(PMesh *mesh, const egreal *vertex)
{
	PTessTableElement *table;
	SubVertexDepend *dep;
	SubMesh *smesh;
	uint i, j, k, l, poly_size = 4;

	if(!p_rm_validate(mesh))
		return;

	smesh = p_geometry_get_sds(e_ns_get_node(0, mesh->geometry_id));

	for(i = 1 ; i < smesh->base_level; i++)
		poly_size *= 4;
	switch(mesh->stage)
	{
		case 0 : /* clearing and allocating */
//printf("mesh->stage %u\n", mesh->stage);
			mesh->tess.tri_count = smesh->tri_count / poly_size;
			mesh->tess.quad_count = smesh->quad_count / poly_size;
			mesh->tess.tri_tess = malloc((sizeof *mesh->tess.tri_tess) * mesh->tess.tri_count);
			mesh->tess.quad_tess = malloc((sizeof *mesh->tess.quad_tess) * mesh->tess.quad_count);
			mesh->normal.reference = malloc((sizeof *mesh->normal.reference) * (mesh->tess.quad_count * 4 + mesh->tess.tri_count * 3));
			mesh->normal.count = malloc((sizeof *mesh->normal.reference) * (mesh->tess.quad_count + mesh->tess.tri_count));
			mesh->render.element_count = 0;
			mesh->render.vertex_count = 0;
			mesh->depend.length = 0;
			mesh->sub_stage = 0;
			mesh->stage++;
			break;
		case 1 : /* choosing tri split tesselation */
//printf("mesh->stage %u\n", mesh->stage);
			{
				uint edge[4] = {2, 2, 2, 2}, index[6];
				p_get_tri_tess_index(index, smesh->base_level);
				for(i = 0; i < P_TRI_TESS_SELECT && mesh->sub_stage < mesh->tess.tri_count; i++)
				{
					table = p_select_tri_tesselation(smesh, vertex, index, mesh->sub_stage * poly_size * 3, mesh->tess.factor, mesh->tess.force, mesh->tess.edge_tess_func, mesh->tess.eay);
					mesh->normal.reference[mesh->sub_stage * 3 + 0] = smesh->tri_reference[mesh->sub_stage * poly_size * 3 + index[0]] * 3;
					mesh->normal.reference[mesh->sub_stage * 3 + 1] = smesh->tri_reference[mesh->sub_stage * poly_size * 3 + index[4]] * 3;
					mesh->normal.reference[mesh->sub_stage * 3 + 2] = smesh->tri_reference[mesh->sub_stage * poly_size * 3 + index[5]] * 3;					
					mesh->normal.count[mesh->sub_stage] = table->vertex_count;
					mesh->tess.tri_tess[mesh->sub_stage] = table;
					mesh->render.element_count += table->element_count;
					mesh->render.vertex_count += table->vertex_count;
					for(j = 0; j < table->vertex_count; j++)
						mesh->depend.length += smesh->vertex_depend_temp[smesh->tri_reference[table->reference[j] + poly_size * mesh->sub_stage * 3]].length;
					mesh->sub_stage++;
				}
				if(mesh->sub_stage == mesh->tess.tri_count)
				{
					mesh->sub_stage = 0;
					mesh->stage++;
				}
				break;
			}
		case 2 : /* choosing quad split tesselation */
//printf("mesh->stage %u %u %u\n", mesh->stage, mesh->sub_stage, mesh->tess.quad_count);
			{
				uint edge[4] = {2, 2, 2, 2}, index[9];
				p_get_quad_tess_index(index, smesh->base_level);
				for(i = 0; i < P_QUAD_TESS_SELECT && mesh->sub_stage < mesh->tess.quad_count; i++)
				{
					table = p_select_quad_tesselation(smesh, vertex, index, mesh->sub_stage * poly_size * 4, mesh->tess.factor, mesh->tess.force, mesh->tess.edge_tess_func, mesh->tess.eay);
					mesh->normal.reference[mesh->tess.tri_count * 3 + mesh->sub_stage * 4 + 0] = smesh->quad_reference[mesh->sub_stage * poly_size * 4 + index[0]] * 3;
					mesh->normal.reference[mesh->tess.tri_count * 3 + mesh->sub_stage * 4 + 1] = smesh->quad_reference[mesh->sub_stage * poly_size * 4 + index[5]] * 3;
					mesh->normal.reference[mesh->tess.tri_count * 3 + mesh->sub_stage * 4 + 2] = smesh->quad_reference[mesh->sub_stage * poly_size * 4 + index[7]] * 3;
					mesh->normal.reference[mesh->tess.tri_count * 3 + mesh->sub_stage * 4 + 3] = smesh->quad_reference[mesh->sub_stage * poly_size * 4 + index[3]] * 3;
					mesh->normal.count[mesh->tess.tri_count + mesh->sub_stage] = table->vertex_count;
					mesh->tess.quad_tess[mesh->sub_stage] = table;
					mesh->render.element_count += table->element_count;
					mesh->render.vertex_count += table->vertex_count;
					for(j = 0; j < table->vertex_count; j++)
						mesh->depend.length += smesh->vertex_depend_temp[smesh->quad_reference[table->reference[j] + poly_size * mesh->sub_stage * 4]].length;
					mesh->sub_stage++;
				}
				if(mesh->sub_stage == mesh->tess.quad_count)
				{
					mesh->sub_stage = 0;
					mesh->stage++;
				}
			}
			break;
		case 3 : /* allocating all the stuff */
//printf("mesh->stage %u\n", mesh->stage);
//printf("%u %u %u\n", mesh->render.vertex_count, mesh->render.element_count, mesh->depend.length);
	
			mesh->render.vertex_array = malloc((sizeof *mesh->render.vertex_array) * mesh->render.vertex_count * 3);
			mesh->render.normal_array = malloc((sizeof *mesh->render.normal_array) * mesh->render.vertex_count * 3);
			mesh->render.vertex_count;
			mesh->render.reference = malloc((sizeof *mesh->render.reference) * mesh->render.element_count * 3);

			mesh->depend.reference = malloc((sizeof *mesh->depend.reference) * mesh->depend.length);
			mesh->depend.weight = malloc((sizeof *mesh->depend.weight) * mesh->depend.length);
			mesh->depend.ref_count = malloc((sizeof *mesh->depend.ref_count) * mesh->render.vertex_count);

			mesh->normal.weight = malloc((sizeof *mesh->normal.weight) * mesh->render.vertex_count * 4);
				mesh->render.vertex_count = 0;
			mesh->depend.length = 0;
	//	exit(0);
			mesh->sub_stage = 0;
			mesh->stage++;
			break;
		case 4 : /* building tri reference */
//printf("mesh->stage %u %u\n", mesh->stage, mesh->tess.tri_count);
			l = 0;
			for(i = 0; i < mesh->tess.tri_count; i++)
			{
				table = mesh->tess.tri_tess[i];
				for(j = 0; j < table->element_count;)
				{
					mesh->render.reference[mesh->sub_stage++] = table->index[j++] + l;
					mesh->render.reference[mesh->sub_stage++] = table->index[j++] + l;
					mesh->render.reference[mesh->sub_stage++] = table->index[j++] + l;
				}
				l += table->vertex_count;
			}
			mesh->stage++;
			break;
		case 5 : /* building quad reference */
//printf("mesh->stage %u\n", mesh->stage);
			l = 0;
			k = 0;
			for(i = 0; i < mesh->tess.quad_count; i++)
			{
				table = mesh->tess.quad_tess[i];
				for(j = 0; j < table->element_count;)
				{
					mesh->render.reference[mesh->sub_stage++] = table->index[j++] + l;
					mesh->render.reference[mesh->sub_stage++] = table->index[j++] + l;
					mesh->render.reference[mesh->sub_stage++] = table->index[j++] + l;
				}
				l += table->vertex_count;
			}
			mesh->sub_stage = 0;
			mesh->stage++;
			break;
		case 6 : /* building tri depend */
//printf("mesh->stage %u\n", mesh->stage);
			for(i = 0; i < P_TRI_TESS_REF && mesh->sub_stage < mesh->tess.tri_count; mesh->sub_stage++)
			{
				table = mesh->tess.tri_tess[mesh->sub_stage];
				for(j = 0; j < table->vertex_count; j++)
				{
					dep = &smesh->vertex_depend_temp[smesh->tri_reference[table->reference[j] + mesh->sub_stage * poly_size * 3]];
					mesh->normal.weight[mesh->render.vertex_count * 3 + 0] = table->vertex_influence[j * 3 + 0];
					mesh->normal.weight[mesh->render.vertex_count * 3 + 1] = table->vertex_influence[j * 3 + 1];
					mesh->normal.weight[mesh->render.vertex_count * 3 + 2] = table->vertex_influence[j * 3 + 2];
					mesh->depend.ref_count[mesh->render.vertex_count++] = dep->length;
					for(k = 0; k < dep->length; k++)
					{
						mesh->depend.reference[mesh->depend.length] = dep->element[k].vertex_ref * 3;
						mesh->depend.weight[mesh->depend.length] = dep->element[k].value;
						
						mesh->depend.length++;
					}
				}
				i += j;
			}
			if(mesh->sub_stage == mesh->tess.tri_count)
			{
				mesh->sub_stage = 0;
				mesh->stage++;
			}
			break;
		case 7 : /* building quad depend */
//printf("mesh->stage %u\n", mesh->stage);
			for(i = 0; i < P_QUAD_TESS_REF && mesh->sub_stage < mesh->tess.quad_count; mesh->sub_stage++)
			{
				table = mesh->tess.quad_tess[mesh->sub_stage];
				for(j = 0; j < table->vertex_count; j++)
				{
					dep = &smesh->vertex_depend_temp[smesh->quad_reference[table->reference[j] + mesh->sub_stage * poly_size * 4]];
					mesh->normal.weight[mesh->render.vertex_count * 4 + 0] = table->vertex_influence[j * 4 + 0];
					mesh->normal.weight[mesh->render.vertex_count * 4 + 1] = table->vertex_influence[j * 4 + 1];
					mesh->normal.weight[mesh->render.vertex_count * 4 + 2] = table->vertex_influence[j * 4 + 2];
					mesh->normal.weight[mesh->render.vertex_count * 4 + 3] = table->vertex_influence[j * 4 + 3];
					mesh->depend.ref_count[mesh->render.vertex_count++] = dep->length;
					for(k = 0; k < dep->length; k++)
					{
						mesh->depend.reference[mesh->depend.length] = dep->element[k].vertex_ref * 3;
						mesh->depend.weight[mesh->depend.length] = dep->element[k].value;
						mesh->depend.length++;
					}
				}
				i += j;
			}
			if(mesh->sub_stage == mesh->tess.quad_count)
			{
				mesh->sub_stage = 0;
				mesh->stage++;
			}
			break;
		case 8 :
			p_rm_compute(mesh, vertex);
			mesh->stage++;
			break;
	}
}
/*
void p_rm_compute(PMesh *mesh, egreal *vertex, uint start, uint length)
{
	uint i, j, k = 0, l, count, *ref;
	egreal f;
	ref = mesh->depend.reference;

	for(i = 0; i < mesh->render.vertex_count; i++)
	{
		mesh->render.vertex_array[i * 3] = 0;
		mesh->render.vertex_array[i * 3 + 1] = 0;
		mesh->render.vertex_array[i * 3 + 2] = 0;
		count = mesh->depend.ref_count[i];
		for(j = 0; j < count; j++)
		{
			l = mesh->depend.reference[k];
			f = mesh->depend.weight[k];
			mesh->render.vertex_array[i * 3] += vertex[l++] * f;
			mesh->render.vertex_array[i * 3 + 1] += vertex[l++] * f;
			mesh->render.vertex_array[i * 3 + 2] += vertex[l] * f;
			k++;
		}
	}
}
*/

boolean p_rm_draw_ready(PMesh *mesh)
{
	return mesh->stage > 7;
}

#define NORMAL_ADD 0.001

void p_rm_compute(PMesh *mesh, const egreal *vertex)
{
	uint i, j, k = 0, l = 0, m = 0, v = 0, w = 0, count, inner;
	egreal f, *cv1, *cv2, *cv3, *cv4, weight[4], normal[3];
	for(i = 0; i < mesh->tess.tri_count; i++)
	{
		count = mesh->normal.count[i];
		cv1 = &vertex[mesh->normal.reference[i * 3 + 0]];
		cv2 = &vertex[mesh->normal.reference[i * 3 + 1]];
		cv3 = &vertex[mesh->normal.reference[i * 3 + 2]];

		normal[0] = ((cv3[1] - cv1[1]) * (cv2[2] - cv1[2]) - (cv3[2] - cv1[2]) * (cv2[1] - cv1[1]));
		normal[1] = ((cv3[2] - cv1[2]) * (cv2[0] - cv1[0]) - (cv3[0] - cv1[0]) * (cv2[2] - cv1[2]));
		normal[2] = ((cv3[0] - cv1[0]) * (cv2[1] - cv1[1]) - (cv3[1] - cv1[1]) * (cv2[0] - cv1[0]));

		for(j = 0; j < count; j++)
		{
			mesh->render.vertex_array[v + 0] = 0;
			mesh->render.vertex_array[v + 1] = 0;
			mesh->render.vertex_array[v + 2] = 0;
			inner = mesh->depend.ref_count[v / 3];
			for(k = 0; k < inner; k++)
			{
				m = mesh->depend.reference[l];
				f = mesh->depend.weight[l];
				mesh->render.vertex_array[v + 0] += vertex[m++] * f;
				mesh->render.vertex_array[v + 1] += vertex[m++] * f;
				mesh->render.vertex_array[v + 2] += vertex[m] * f;
				l++;
			}
			weight[0] = mesh->normal.weight[w++];
			weight[1] = mesh->normal.weight[w++];
			weight[2] = mesh->normal.weight[w++];
			mesh->render.normal_array[v + 0] = (cv1[0] * weight[0] + cv2[0] * weight[1] + cv3[0] * weight[2]) - mesh->render.vertex_array[v + 0];
			mesh->render.normal_array[v + 1] = (cv1[1] * weight[0] + cv2[1] * weight[1] + cv3[1] * weight[2]) - mesh->render.vertex_array[v + 1];
			mesh->render.normal_array[v + 2] = (cv1[2] * weight[0] + cv2[2] * weight[1] + cv3[2] * weight[2]) - mesh->render.vertex_array[v + 2];

			mesh->render.normal_array[v + 0] = normal[0];
			mesh->render.normal_array[v + 1] = normal[1];
			mesh->render.normal_array[v + 2] = normal[2];
			v += 3;
		}
	}
	for(i = 0; i < mesh->tess.quad_count; i++)
	{
		count = mesh->normal.count[i + mesh->tess.tri_count];
		cv1 = &vertex[mesh->normal.reference[mesh->tess.tri_count * 3 + i * 4 + 0]];
		cv2 = &vertex[mesh->normal.reference[mesh->tess.tri_count * 3 + i * 4 + 1]];
		cv3 = &vertex[mesh->normal.reference[mesh->tess.tri_count * 3 + i * 4 + 2]];
		cv4 = &vertex[mesh->normal.reference[mesh->tess.tri_count * 3 + i * 4 + 3]];

		normal[0] = ((cv3[1] - cv1[1]) * (cv2[2] - cv1[2]) - (cv3[2] - cv1[2]) * (cv2[1] - cv1[1]));
		normal[1] = ((cv3[2] - cv1[2]) * (cv2[0] - cv1[0]) - (cv3[0] - cv1[0]) * (cv2[2] - cv1[2]));
		normal[2] = ((cv3[0] - cv1[0]) * (cv2[1] - cv1[1]) - (cv3[1] - cv1[1]) * (cv2[0] - cv1[0]));

		for(j = 0; j < count; j++)
		{
			mesh->render.vertex_array[v + 0] = 0;
			mesh->render.vertex_array[v + 1] = 0;
			mesh->render.vertex_array[v + 2] = 0;
			inner = mesh->depend.ref_count[v / 3];
			for(k = 0; k < inner; k++)
			{
				m = mesh->depend.reference[l];
				f = mesh->depend.weight[l];
				mesh->render.vertex_array[v + 0] += vertex[m++] * f;
				mesh->render.vertex_array[v + 1] += vertex[m++] * f;
				mesh->render.vertex_array[v + 2] += vertex[m] * f;
				l++;
			}
			weight[0] = mesh->normal.weight[w++];
			weight[1] = mesh->normal.weight[w++];
			weight[2] = mesh->normal.weight[w++];
			weight[3] = mesh->normal.weight[w++];
			mesh->render.normal_array[v + 0] = (cv1[0] * weight[0] + cv2[0] * weight[1] + cv3[0] * weight[2] + cv4[0] * weight[3]) - mesh->render.vertex_array[v + 0];
			mesh->render.normal_array[v + 1] = (cv1[1] * weight[0] + cv2[1] * weight[1] + cv3[1] * weight[2] + cv4[1] * weight[3]) - mesh->render.vertex_array[v + 1];
			mesh->render.normal_array[v + 2] = (cv1[2] * weight[0] + cv2[2] * weight[1] + cv3[2] * weight[2] + cv4[2] * weight[3]) - mesh->render.vertex_array[v + 2];

			mesh->render.normal_array[v + 0] = normal[0];
			mesh->render.normal_array[v + 1] = normal[1];
			mesh->render.normal_array[v + 2] = normal[2];
			v += 3;
		}
	}
}


egreal *p_rm_get_vertex(PMesh *mesh)
{
	return mesh->render.vertex_array;
}

egreal *p_rm_get_normal(PMesh *mesh)
{
	return mesh->render.normal_array;
}

uint p_rm_get_vertex_length(PMesh *mesh)
{
	return mesh->render.vertex_count;
}

uint *p_rm_get_reference(PMesh *mesh)
{
	return mesh->render.reference;
}

uint p_rm_get_ref_length(PMesh *mesh)
{
	return mesh->render.element_count;
}

egreal p_rm_compute_bounding_box(ENode *node, egreal *vertex, egreal *center, egreal *scale)
{
	uint i, length;
	egreal box[6], size;
	length = e_nsg_get_vertex_legnth(node) * 3;
	for(i = 0; i < length; i += 3)
	{
		if(vertex[i] != E_REAL_MAX)
		{
			box[0] = vertex[i];
			box[1] = vertex[i];
			box[2] = vertex[i + 1];
			box[3] = vertex[i + 1];
			box[4] = vertex[i + 2];
			box[5] = vertex[i + 2];
			break;
		}
	}if(i == length)
		return 1;
	while(i < length)
	{
		if(vertex[i] != E_REAL_MAX)
		{
			if(box[0] < vertex[i])
				box[0] = vertex[i];
			if(box[1] > vertex[i])
				box[1] = vertex[i];
			i++;
			if(box[2] < vertex[i])
				box[2] = vertex[i];
			if(box[3] > vertex[i])
				box[3] = vertex[i];
			i++;
			if(box[4] < vertex[i])
				box[4] = vertex[i];
			if(box[5] > vertex[i])
				box[5] = vertex[i];
			i++;
	
		}else
			i += 3;
	}
	if(center != NULL)
	{
		center[0] = box[0] + (box[1] - box[0]) * 0.5;
		center[1] = box[2] + (box[3] - box[2]) * 0.5;
		center[2] = box[4] + (box[5] - box[4]) * 0.5;
	}
	if(scale != NULL)
	{
		scale[0] = size = (box[1] - box[0]);
		scale[1] = box[0] = (box[3] - box[2]);
		scale[2] = box[1] = (box[5] - box[4]);
	}
	if(box[0] > size);
		size = box[0];
	if(box[1] > size);
		size = box[1];
	return size;
}