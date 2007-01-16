
#include <stdio.h>

#include "enough.h"

#include "p_sds_geo.h"
#include "p_sds_table.h"
#include "p_sds_obj.h"
#include "p_task.h"

#include <math.h>

extern void p_pre_load_material_select(ENode *node);
extern void p_get_tri_tess_index(uint *index, uint base_tess);
extern void p_get_quad_tess_index(uint *index, uint base_tess);
extern boolean p_lod_displacement_update_test(PMesh *mesh);

static uint p_sds_force_level = 0;
static float p_sds_mesh_factor = 100;

void p_geo_set_sds_force_level(uint level)
{
	p_sds_force_level = level;
}

uint p_geo_get_sds_force_level(void)
{
	return p_sds_force_level;
}

void p_geo_set_sds_mesh_factor(float factor)
{
	p_sds_mesh_factor = factor;
}

float p_geo_get_sds_mesh_factor(void)
{
	return p_sds_mesh_factor;
}

PMesh *p_rm_create(ENode *node)
{
	PPolyStore *smesh;
	PMesh *mesh;
	smesh = p_sds_get_mesh(node);
	if(e_ns_get_node_type(node) != V_NT_GEOMETRY || smesh == NULL)
		return NULL;
	mesh = malloc(sizeof *mesh);
	mesh->geometry_id =	e_ns_get_node_id(node);
	mesh->geometry_version = smesh->geometry_version;
	mesh->stage = 0;
	mesh->sub_stages[0] = 0;
	mesh->sub_stages[1] = 0;
	mesh->sub_stages[2] = 0;
	mesh->sub_stages[3] = 0;
	mesh->tess.force = p_sds_force_level;
	mesh->tess.factor = p_sds_mesh_factor;
	mesh->tess.edge_tess_func = p_sds_edge_tesselation_global_func;
	mesh->temp = NULL;
	mesh->next = NULL;
	mesh->tess.tess = NULL;
	mesh->tess.order_node = NULL;
	mesh->tess.order_temp_mesh = NULL;
	mesh->tess.order_temp_mesh_rev = NULL;
	mesh->tess.order_temp_poly_start = NULL;
	mesh->render.vertex_array = NULL;
	mesh->render.normal_array = NULL;
	mesh->render.reference = NULL;
	mesh->render.mat = NULL;
	mesh->render.shadows = TRUE;
	mesh->depend.reference = NULL;
	mesh->depend.weight = NULL;
	mesh->depend.ref_count = NULL;
	mesh->normal.normal_ref = NULL;	
	mesh->normal.normals = NULL;
	mesh->displacement.displacement = NULL;
	mesh->displacement.live = TRUE;
	mesh->displacement.node_version = 0;
	mesh->displacement.tree_version = 0;
	mesh->normal.draw_normals = NULL;
	mesh->param.array = NULL;
	mesh->param.version = 0;
	mesh->param.data_version = 0;
	mesh->anim.layers.layers = NULL;
	mesh->anim.bones.bonereference = NULL;
	mesh->anim.bones.ref_count = NULL;
	mesh->anim.bones.boneweight = NULL;
	mesh->anim.cvs = NULL; 
	return mesh;
}

void p_rm_set_eay(PMesh *mesh, egreal *eay)
{
	mesh->tess.eay[0] = eay[0];
	mesh->tess.eay[1] = eay[1];
	mesh->tess.eay[2] = eay[2];
	mesh->tess.edge_tess_func = p_sds_edge_tesselation_global_func;
}


void p_rm_destroy(PMesh *mesh)
{
	uint i;
	if(mesh->temp != NULL)
		free(mesh->temp);
	if(mesh->tess.tess != NULL)
		free(mesh->tess.tess);
	if(mesh->tess.order_node != NULL)
		free(mesh->tess.order_node);
	if(mesh->tess.order_temp_mesh != NULL)
		free(mesh->tess.order_temp_mesh);
	if(mesh->tess.order_temp_mesh_rev != NULL)
		free(mesh->tess.order_temp_mesh_rev);
	if(mesh->tess.order_temp_poly_start != NULL)
		free(mesh->tess.order_temp_poly_start);

	if(mesh->render.vertex_array != NULL)
		free(mesh->render.vertex_array);
	if(mesh->render.normal_array != NULL)
		free(mesh->render.normal_array);
	if(mesh->render.reference != NULL)
		free(mesh->render.reference);
	if(mesh->render.mat != NULL)
		free(mesh->render.mat);

	if(mesh->depend.reference != NULL)
		free(mesh->depend.reference);
	if(mesh->depend.weight != NULL)
		free(mesh->depend.weight);
	if(mesh->depend.ref_count != NULL)
		free(mesh->depend.ref_count);

	if(mesh->normal.normal_ref != NULL)
		free(mesh->normal.normal_ref);	
	if(mesh->normal.normals != NULL)
		free(mesh->normal.normals);
	if(mesh->normal.draw_normals != NULL)
		free(mesh->normal.draw_normals);

	if(mesh->displacement.displacement != NULL)
		free(mesh->displacement.displacement);

	if(mesh->param.array != NULL)
	{
		for(i = 0; i < mesh->param.array_count; i++)
			p_ra_free_array(&mesh->param.array[i]);
		free(mesh->param.array);
	}
	if(mesh->param.version != NULL)
		free(mesh->param.version);

	if(mesh->anim.layers.layers != NULL)
		free(mesh->anim.layers.layers);
	if(mesh->anim.bones.bonereference != NULL)
		free(mesh->anim.bones.bonereference);
	if(mesh->anim.bones.ref_count != NULL)
		free(mesh->anim.bones.ref_count);
	if(mesh->anim.bones.boneweight != NULL)
		free(mesh->anim.bones.boneweight);
	if(mesh->anim.cvs != NULL)
		free(mesh->anim.cvs); 
	free(mesh);
}

boolean p_rm_validate(PMesh *mesh)
{
	PPolyStore *smesh;
	smesh = p_sds_get_mesh(e_ns_get_node(0, mesh->geometry_id));
	if(smesh == NULL || mesh->geometry_version != smesh->version)
		return FALSE;
	return TRUE;
}

void p_sds_compute_eye_pos(ENode *node, PMesh *mesh, double *eay)
{
	double matrix[16], tmp[3];
	static double t = 0;
	mesh->tess.eay[0] = eay[0];
	mesh->tess.eay[1] = eay[1];
	mesh->tess.eay[2] = eay[2];
	if(node != NULL)
	{
		e_nso_get_matrix(node, matrix, 0, 0);
		e_nso_get_pos_time(node, tmp, 0, 0);
		tmp[0] = -tmp[0] + mesh->tess.eay[0];
		tmp[1] = -tmp[1] + mesh->tess.eay[1];
		tmp[2] = -tmp[2] + mesh->tess.eay[2];

		mesh->tess.eay[0] = tmp[0];
		mesh->tess.eay[1] = tmp[1];
		mesh->tess.eay[2] = tmp[2];		

		mesh->tess.eay[0] = matrix[0] * tmp[0] + matrix[1] * tmp[1] + matrix[2] * tmp[2];
		mesh->tess.eay[1] = matrix[4] * tmp[0] + matrix[5] * tmp[1] + matrix[6] * tmp[2];
		mesh->tess.eay[2] = matrix[8] * tmp[0] + matrix[9] * tmp[1] + matrix[10] * tmp[2];
	}
}


typedef enum{
	POS_ALLOCATE,
	POS_FIND_HOLES_MAT_SORT,
	POS_TESS_SELECT,
	POS_SECOND_ALLOCATE,
	POS_CREATE_REF,
	POS_CREATE_DEPEND,
	POS_CREATE_VERTEX_NORMAL,
	POS_CREATE_NORMAL_REF,
	POS_CREATE_LAYER_PARAMS,
	POS_CREATE_DISPLACEMENT,
	POS_CREATE_ANIM,
	POS_ANIMATE,
	POS_CREATE_VERTICES,
	POS_DONE
}PObjStage;

boolean p_rm_drawable(PMesh *mesh)
{
	return mesh->stage == POS_DONE;
}

void p_rm_unvalidate(PMesh *mesh)
{
	mesh->geometry_version--;
}

uint p_rm_get_geometry_node(PMesh *mesh)
{
	return mesh->geometry_id;
}

void *p_rm_get_param(PMesh *mesh)
{
	return mesh->param.array;
}


egreal *p_rm_get_param_data(PMesh *mesh, uint id)
{
	return ((PRenderArray *)mesh->param.array)[id].array.real;
}


uint p_rm_get_param_count(PMesh *mesh)
{
	return mesh->param.array_count;
}


#define P_TRI_TESS_SELECT 10000
#define P_QUAD_TESS_SELECT 7500
#define P_TRI_TESS_REF 10000
#define P_QUAD_TESS_REF 7500

boolean p_lod_material_test(PMesh *mesh, ENode *o_node);
double *p_lod_get_view_pos(void);


				

void p_rm_test(PMesh *mesh, PPolyStore *smesh)
{
	PDepend *dep;
	ENode *g_node;
	PMesh *store = NULL;
	PTessTableElement *table;
	PTimer timer;
	uint i, j, k, poly, temp, temp2, length;
	double *eye;
	uint32 seconds, fractions;
return;
	i = 0;
	j = 0;
	k = 0;
	temp = 0;
	temp2 = 0;

	for(; i < mesh->tess.tri_count + mesh->tess.quad_count; i++)
	{
		table = mesh->tess.tess[i];

		if(i == mesh->render.mat[j].tri_end)
			j++;
		if(i < mesh->render.mat[j].quad_end)
			poly = mesh->tess.order_temp_mesh[i] * smesh->poly_per_base * 4;
		else
			poly = smesh->base_quad_count * smesh->poly_per_base * 4 + (mesh->tess.order_temp_mesh[i] - smesh->base_quad_count) * smesh->poly_per_base * 3;
		for(j = 0; j < table->vertex_count; j++)
		{
			temp++;
			dep = &smesh->vertex_dependency[smesh->ref[table->reference[j] + poly]];
			length += dep->length;
		}
		k += table->vertex_count;
	}
	printf("p_rm_test = %u %u %u\n", temp, length, k);
	printf("p_rm_test = %u %u %u\n", mesh->render.vertex_count, mesh->depend.length, mesh->render.element_count);

	if(mesh->render.mat_count > 4)
		exit(0);
	 
}

PMesh *p_rm_service(PMesh *mesh, ENode *o_node, /*const*/ egreal *vertex)
{
	ENode *g_node;
	PMesh *store = NULL;
	PTessTableElement *table;
	PTimer timer;
	PPolyStore *smesh;
	uint i, j, k, poly_size = 4;
	double *eye;
	uint32 seconds, fractions;
	g_node = e_ns_get_node(0, mesh->geometry_id);
	smesh = p_sds_get_mesh(g_node);

	if(vertex == NULL)
		vertex = e_nsg_get_layer_data(g_node, e_nsg_get_layer_by_id(g_node, 0));

	if(smesh == NULL || e_ns_get_node_version_struct(g_node) != smesh->geometry_version)
	{
		p_pre_load_material_select(o_node);
		return mesh;
	}
	verse_session_get_time(&seconds, &fractions);
	if(mesh->stage == POS_DONE && mesh->next == NULL && (p_lod_compute_lod_update(o_node, g_node, seconds, fractions, mesh->tess.factor / p_sds_mesh_factor) || p_lod_material_test(mesh, o_node)))
		mesh->geometry_version--;
/*	if(smesh->geometry_version != mesh->geometry_version && mesh->next == NULL)
	{
		printf("recomputing node = %u, (%u %u)\n", mesh->geometry_id, smesh->geometry_version, mesh->geometry_version);
		if(mesh->stage != POS_DONE)
		{
			p_rm_destroy(mesh);
			return p_rm_create(g_node);
		}else
		{
			mesh->next = p_rm_create(g_node);
		}
	}*/

	if(mesh->next != NULL)
	{
		if(smesh->geometry_version != ((PMesh *)mesh->next)->geometry_version)
		{
			p_rm_destroy(mesh->next);
			mesh->next = NULL;
			return mesh;
		}
		store = mesh;
		mesh = mesh->next;
	}

	for(i = 1 ; i < smesh->level; i++)
		poly_size *= 4;
	p_timer_start(&timer);
	while(p_timer_elapsed(&timer) < 0.025)
	{
		switch(mesh->stage)
		{
			case POS_ALLOCATE : /* clearing and allocating */
				mesh->tess.tri_count = smesh->base_tri_count;
				mesh->tess.quad_count = smesh->base_quad_count;
				mesh->tess.tess = malloc((sizeof *mesh->tess.tess) * (mesh->tess.tri_count + mesh->tess.quad_count));
				for(i = 0; i < (mesh->tess.tri_count + mesh->tess.quad_count); i++)
					mesh->tess.tess[i] = NULL;
				mesh->tess.factor = p_lod_compute_lod_level(o_node, g_node, seconds, fractions) * p_sds_mesh_factor;
				p_sds_compute_eye_pos(o_node, mesh, p_lod_get_view_pos());
				mesh->temp = NULL;
				mesh->render.element_count = 0;
				mesh->render.vertex_count = 0;
				mesh->render.open_edges = smesh->open_edges != 0;
				mesh->render.shadows = smesh->open_edges == 0;
				mesh->depend.length = 0;
				mesh->anim.cv_count = e_nsg_get_vertex_length(g_node);
				mesh->anim.cvs = malloc((sizeof *mesh->anim.cvs) * mesh->anim.cv_count * 3); 
				mesh->anim.play.layers = TRUE;
				mesh->anim.play.bones = TRUE;
				mesh->anim.layers.data_version = 0;
				mesh->anim.layers.layers = NULL;
				mesh->anim.layers.layer_count = 0;
				mesh->anim.bones.bonereference = NULL;
				mesh->anim.bones.ref_count = NULL;
				mesh->anim.bones.boneweight = NULL;
				mesh->anim.scale[0] = 1;
				mesh->anim.scale[1] = 1;
				mesh->anim.scale[2] = 1;
				if(o_node != NULL)
					p_lod_anim_scale_update_test(mesh, o_node);
				mesh->sub_stages[0] = 0;
				mesh->sub_stages[1] = 0;
				mesh->sub_stages[2] = 0;
				mesh->sub_stages[3] = 0;
				mesh->stage++;
				break;
			case POS_FIND_HOLES_MAT_SORT : /* handle_materials */
				p_lod_gap_count(g_node, smesh, mesh, o_node);
				break;
			case POS_TESS_SELECT : /* choosing quad split tesselation */

				p_lod_select_tesselation(mesh, smesh, vertex);
				p_rm_test(mesh, smesh);
/*				for(i = 0; i < mesh->tess.tri_count + mesh->tess.quad_count; i++)
					fprintf(stderr, "%p \n", mesh->tess.tess[i]);
*/				break;
			case POS_SECOND_ALLOCATE : /* allocating all the stuff */
				mesh->render.vertex_array = malloc((sizeof *mesh->render.vertex_array) * mesh->render.vertex_count * 3);
				mesh->render.normal_array = malloc((sizeof *mesh->render.normal_array) * mesh->render.vertex_count * 3);
				mesh->normal.normal_ref = malloc((sizeof *mesh->normal.normal_ref) * mesh->render.vertex_count * 4);
				mesh->render.reference = malloc((sizeof *mesh->render.reference) * mesh->render.element_count * 3);

				mesh->displacement.displacement = NULL;

				mesh->depend.reference = malloc((sizeof *mesh->depend.reference) * mesh->depend.length);
				mesh->depend.weight = malloc((sizeof *mesh->depend.weight) * mesh->depend.length);
				mesh->depend.length_temp = mesh->depend.length;
				mesh->depend.length_temp2 = mesh->render.vertex_count;
				mesh->depend.length_temp3 = mesh->render.element_count;
				mesh->depend.ref_count = malloc((sizeof *mesh->depend.ref_count) * mesh->render.vertex_count);
				mesh->render.element_count = 0;
				mesh->render.vertex_count = 0;
				mesh->depend.length = 0;
				mesh->stage++;
				break;
			case POS_CREATE_REF : /* building reference */
				for(; mesh->sub_stages[0] < mesh->tess.tri_count + mesh->tess.quad_count; mesh->sub_stages[0]++)
				{
					if(mesh->sub_stages[0] == mesh->render.mat[mesh->sub_stages[3]].tri_end)
					{
						mesh->render.mat[mesh->sub_stages[3]].render_end = mesh->render.element_count;
						mesh->sub_stages[3]++;
					}
					table = mesh->tess.tess[mesh->sub_stages[0]];
					for(j = 0; j < table->element_count;)
					{
						mesh->render.reference[mesh->render.element_count++] = table->index[j++] + mesh->sub_stages[1];
						mesh->render.reference[mesh->render.element_count++] = table->index[j++] + mesh->sub_stages[1];
						mesh->render.reference[mesh->render.element_count++] = table->index[j++] + mesh->sub_stages[1];
					}
					mesh->sub_stages[1] += table->vertex_count;
				}
				
				if(mesh->sub_stages[0] == mesh->tess.tri_count + mesh->tess.quad_count)
				{
					mesh->render.mat[mesh->sub_stages[3]].render_end = mesh->render.element_count;
					mesh->stage++;
					mesh->sub_stages[0] = 0;
					mesh->sub_stages[1] = 0;
					mesh->sub_stages[2] = 0;
					mesh->sub_stages[3] = 0;
				}
				break;
			case POS_CREATE_DEPEND : /* building depend */
				{
					PDepend *dep;
					uint poly, temp = 0;
					for(; mesh->sub_stages[0] < mesh->tess.tri_count + mesh->tess.quad_count; mesh->sub_stages[0]++)
					{
						table = mesh->tess.tess[mesh->sub_stages[0]];

						if(mesh->sub_stages[0] == mesh->render.mat[mesh->sub_stages[1]].tri_end)
							mesh->sub_stages[1]++;
						if(mesh->sub_stages[0] < mesh->render.mat[mesh->sub_stages[1]].quad_end)
							poly = mesh->tess.order_temp_mesh[mesh->sub_stages[0]] * smesh->poly_per_base * 4;
						else
					//		poly = smesh->quad_length / 4 + mesh->tess.order_temp_mesh[mesh->sub_stages[0]] * smesh->poly_per_base * 3;
							poly = smesh->base_quad_count * smesh->poly_per_base * 4 + (mesh->tess.order_temp_mesh[mesh->sub_stages[0]] - smesh->base_quad_count) * smesh->poly_per_base * 3;
//							poly = smesh->quad_length / 4 + mesh->tess.order_temp_mesh[mesh->sub_stages[0]] * smesh->poly_per_base * 3;
//mesh->depend.length += smesh->vertex_dependency[smesh->ref[poly + table->reference[j]]].length;
						for(j = 0; j < table->vertex_count; j++)
						{
							temp++;
							dep = &smesh->vertex_dependency[smesh->ref[table->reference[j] + poly]];
							mesh->depend.ref_count[mesh->render.vertex_count++] = dep->length;
							for(k = 0; k < dep->length; k++)
							{
								mesh->depend.reference[mesh->depend.length] = dep->element[k].vertex * 3;
								mesh->depend.weight[mesh->depend.length] = dep->element[k].value;
								mesh->depend.length++;
							}
						}
						mesh->tess.order_temp_poly_start[mesh->sub_stages[0]] = mesh->sub_stages[2];
						mesh->sub_stages[2] += table->vertex_count;
					}
					if(mesh->sub_stages[0] == mesh->tess.tri_count + mesh->tess.quad_count)
					{
						mesh->stage++;
						mesh->sub_stages[0] = 0;
						mesh->sub_stages[1] = 0;
						mesh->sub_stages[2] = 0;
						mesh->sub_stages[3] = 0;
					}
					break;
				}
			case POS_CREATE_VERTEX_NORMAL :
				p_lod_compute_vertex_normals(smesh, mesh);
				mesh->stage++;
				break;
			case POS_CREATE_NORMAL_REF :
				p_lod_create_normal_ref_and_shadow_skirts(smesh, mesh);
				break;
			case POS_CREATE_LAYER_PARAMS :
				p_lod_create_layer_param(g_node, mesh);
				break;
			case POS_CREATE_DISPLACEMENT :
				if(o_node != NULL)	
				{
					if(p_lod_displacement_update_test(mesh))
					{
						uint ii;
						mesh->displacement.displacement = malloc((sizeof *mesh->displacement.displacement) * mesh->render.vertex_count);
						p_lod_create_displacement_array(g_node, o_node, mesh, smesh->level);
					//	for(ii = 0; ii < mesh->render.vertex_count; ii++)
					//		mesh->displacement.displacement[ii] = 0;
					}
				}
				else
					mesh->stage++;
				break;
			case POS_CREATE_ANIM :
				if(o_node != NULL)	
				{
					p_lod_anim_bones_update_test(mesh, o_node, g_node);
					p_lod_anim_scale_update_test(mesh, o_node);
					p_lod_anim_layer_update_test(mesh, o_node, g_node);
				}
				mesh->stage++;
				break;
			case POS_ANIMATE :
		
				if(o_node != NULL)	
					p_lod_anim_vertex_array(mesh->anim.cvs, mesh->anim.cv_count, mesh, g_node);
				mesh->stage++;
				break;
			case POS_CREATE_VERTICES :
				j = 0;
				for(i = 0; i < mesh->render.vertex_count; i++)
					j += mesh->depend.ref_count[i];
				if(o_node != NULL)	
					p_lod_compute_vertex_array(mesh->render.vertex_array, mesh->render.vertex_count, mesh->depend.ref_count, mesh->depend.reference, mesh->depend.weight, mesh->anim.cvs);
				else
					p_lod_compute_vertex_array(mesh->render.vertex_array, mesh->render.vertex_count, mesh->depend.ref_count, mesh->depend.reference, mesh->depend.weight, vertex);
				p_lod_compute_normal_array(mesh->render.normal_array, mesh->render.vertex_count, mesh->normal.normal_ref, mesh->render.vertex_array);
			//	if(o_node != NULL)	
			//		p_lod_create_displacement_array(g_node, o_node, mesh, smesh->level);
				if(mesh->displacement.displacement != NULL)
				{
					p_lod_compute_displacement_array(mesh->render.vertex_array, mesh->render.vertex_count, mesh->render.normal_array, mesh->displacement.displacement);
					p_lod_compute_normal_array(mesh->render.normal_array, mesh->render.vertex_count, mesh->normal.normal_ref, mesh->render.vertex_array);
				}
				mesh->stage++;
				if(store != NULL)
					p_rm_destroy(store);
				store = NULL;
				break;
			case POS_DONE :
				if(o_node != NULL)
				{
					boolean update = FALSE;
					static double timer = 0;
					timer += 0.1;
					p_lod_update_shadow(g_node, mesh);
					p_lod_update_layer_param(g_node, mesh);
					if(p_lod_anim_bones_update_test(mesh, o_node, g_node))
						update = TRUE;
					if(p_lod_anim_scale_update_test(mesh, o_node))
						update = TRUE;
					if(p_lod_anim_layer_update_test(mesh, o_node, g_node))
						update = TRUE;
					if(p_lod_displacement_update_test(mesh))
					{
						uint ii;
						p_lod_update_displacement_array(g_node, o_node, mesh, smesh->level);
						update = TRUE;
					}
					if(update)
					{
						p_lod_anim_vertex_array(mesh->anim.cvs, mesh->anim.cv_count, mesh, g_node);
						p_lod_compute_vertex_array(mesh->render.vertex_array, mesh->render.vertex_count, mesh->depend.ref_count, mesh->depend.reference, mesh->depend.weight, mesh->anim.cvs);
						p_lod_compute_normal_array(mesh->render.normal_array, mesh->render.vertex_count, mesh->normal.normal_ref, mesh->render.vertex_array);
						if(mesh->displacement.displacement != NULL)
						{
							p_lod_compute_displacement_array(mesh->render.vertex_array, mesh->render.vertex_count, mesh->render.normal_array, mesh->displacement.displacement);
							p_lod_compute_normal_array(mesh->render.normal_array, mesh->render.vertex_count, mesh->normal.normal_ref, mesh->render.vertex_array);
						}
					}
				}
				return mesh;	
		}
	}
	if(store != NULL)
		return store;
	return mesh;
}

void p_rm_update_shape(PMesh *mesh, egreal *vertex)
{
	p_lod_compute_vertex_array(mesh->render.vertex_array, mesh->render.vertex_count, mesh->depend.ref_count, mesh->depend.reference, mesh->depend.weight, vertex);
	p_lod_compute_normal_array(mesh->render.normal_array, mesh->render.vertex_count, mesh->normal.normal_ref, mesh->render.vertex_array);
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

void p_lod_compute_vertex_array(egreal *vertex, uint vertex_count, const uint *ref_count, const uint *reference,  const egreal *weight, const egreal *cvs)
{
	uint i, j, k = 0, v = 0, count, r;
	egreal f;
//	printf("vertex_count %u\n", vertex_count);
	for(i = 0; i < vertex_count; i++)
	{
		vertex[v + 0] = 0;
		vertex[v + 1] = 0;
		vertex[v + 2] = 0;
		count = ref_count[i];
		for(j = 0; j < count; j++)
		{
			r = reference[k];
			f = weight[k++];
	//		printf("cvs %u %f %f %f\n", r / 3, cvs[r], cvs[r + 1], cvs[r + 2]);
			vertex[v + 0] += cvs[r++] * f;
			vertex[v + 1] += cvs[r++] * f;
			vertex[v + 2] += cvs[r] * f;
		}
	//	printf("float %f %f %f\n", vertex[v + 0], vertex[v + 1], vertex[v + 2]);
		v += 3;
	}

}

void p_lod_compute_displacement_array(egreal *vertex, uint vertex_count, const egreal *normals, const egreal *displacement)
{
	uint i;
	for(i = 0; i < vertex_count; i++)
	{
		*vertex++ += *normals++ * *displacement * 100;
		*vertex++ += *normals++ * *displacement * 100;
		*vertex++ += *normals++ * *displacement++ * 100;
	}
}

void p_lod_compute_normal_array(egreal *normals, uint vertex_count, const uint *normal_ref, const egreal *vertex)
{
	uint i = 0, j = 0;
	egreal x, y, z, f, vec0[3], vec1[3], vec2[3], vec3[3];
	vertex_count *= 3;
	if(TRUE)
	{
		while(i < vertex_count)
		{
			x = vertex[i];
			y = vertex[i + 1];
			z = vertex[i + 2];

			vec0[0] = vertex[normal_ref[j] * 3] - x;
			vec0[1] = vertex[normal_ref[j] * 3 + 1] - y;
			vec0[2] = vertex[normal_ref[j] * 3 + 2] - z;
			j++;
			vec1[0] = vertex[normal_ref[j] * 3] - x;
			vec1[1] = vertex[normal_ref[j] * 3 + 1] - y;
			vec1[2] = vertex[normal_ref[j] * 3 + 2] - z;
			j++;
			vec2[0] = vertex[normal_ref[j] * 3] - x;
			vec2[1] = vertex[normal_ref[j] * 3 + 1] - y;
			vec2[2] = vertex[normal_ref[j] * 3 + 2] - z;
			j++;
			vec3[0] = vertex[normal_ref[j] * 3] - x;
			vec3[1] = vertex[normal_ref[j] * 3 + 1] - y;
			vec3[2] = vertex[normal_ref[j] * 3 + 2] - z;
			j++;

			x = (vec0[1] * vec1[2] - vec0[2] * vec1[1]) + (vec2[1] * vec3[2] - vec2[2] * vec3[1]);
			y = (vec0[2] * vec1[0] - vec0[0] * vec1[2]) + (vec2[2] * vec3[0] - vec2[0] * vec3[2]);
			z = (vec0[0] * vec1[1] - vec0[1] * vec1[0]) + (vec2[0] * vec3[1] - vec2[1] * vec3[0]);

			f = sqrt(x * x + y * y + z * z);
			normals[i++] = x / f;
			normals[i++] = y / f;
			normals[i++] = z / f;
		}
	}else
	{
		while(i < vertex_count)
		{
			x = vertex[i];
			y = vertex[i + 1];
			z = vertex[i + 2];

			vec0[0] = vertex[normal_ref[j] * 3] - x;
			vec0[1] = vertex[normal_ref[j] * 3 + 1] - y;
			vec0[2] = vertex[normal_ref[j] * 3 + 2] - z;
			f = sqrt(vec0[0] * vec0[0] + vec0[1] * vec0[1] + vec0[2] * vec0[2]);
			vec0[0] /= f;
			vec0[1] /= f;
			vec0[2] /= f;
			j++;
			vec1[0] = vertex[normal_ref[j] * 3] - x;
			vec1[1] = vertex[normal_ref[j] * 3 + 1] - y;
			vec1[2] = vertex[normal_ref[j] * 3 + 2] - z;
			f = sqrt(vec1[0] * vec1[0] + vec1[1] * vec1[1] + vec1[2] * vec1[2]);
			vec1[0] /= f;
			vec1[1] /= f;
			vec1[2] /= f;
			j++;
			vec2[0] = vertex[normal_ref[j] * 3] - x;
			vec2[1] = vertex[normal_ref[j] * 3 + 1] - y;
			vec2[2] = vertex[normal_ref[j] * 3 + 2] - z;
			f = sqrt(vec2[0] * vec2[0] + vec2[1] * vec2[1] + vec2[2] * vec2[2]);
			vec2[0] /= f;
			vec2[1] /= f;
			vec2[2] /= f;
			j++;
			vec3[0] = vertex[normal_ref[j] * 3] - x;
			vec3[1] = vertex[normal_ref[j] * 3 + 1] - y;
			vec3[2] = vertex[normal_ref[j] * 3 + 2] - z;
			f = sqrt(vec3[0] * vec3[0] + vec3[1] * vec3[1] + vec3[2] * vec3[2]);
			vec3[0] /= f;
			vec3[1] /= f;
			vec3[2] /= f;
			j++;

			x = (vec0[1] * vec1[2] - vec0[2] * vec1[1]) + (vec2[1] * vec3[2] - vec2[2] * vec3[1]);
			y = (vec0[2] * vec1[0] - vec0[0] * vec1[2]) + (vec2[2] * vec3[0] - vec2[0] * vec3[2]);
			z = (vec0[0] * vec1[1] - vec0[1] * vec1[0]) + (vec2[0] * vec3[1] - vec2[1] * vec3[0]);

			f = sqrt(x * x + y * y + z * z);
			normals[i++] = x / f;
			normals[i++] = y / f;
			normals[i++] = z / f;
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

uint p_rm_get_mat_count(PMesh *mesh)
{
	return mesh->render.mat_count;
}

uint p_rm_get_material_range(PMesh *mesh, uint mat)
{
	return mesh->render.mat[mat].render_end;
}

uint p_rm_get_material(PMesh *mesh, uint mat)
{
	return mesh->render.mat[mat].material;
}

boolean p_rm_get_shadow(PMesh *mesh)
{
	return mesh->render.shadows;
}
