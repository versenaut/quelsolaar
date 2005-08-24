
#include "enough.h"
#include "p_sds_geo.h"
#include "p_sds_table.h"
#include "p_sds_obj.h"

extern uint p_shader_get_param_count(ENode *node);

boolean p_lod_material_test(PMesh *mesh, ENode *o_node)
{
	EObjLink *link = NULL;
	ENode *m_node;
	uint material = -1;
	if(o_node == NULL)
		return FALSE;
	for(link = e_nso_get_next_link(o_node, 0); link != 0; link = e_nso_get_next_link(o_node, e_nso_get_link_id(link) + 1))
		if((m_node = e_ns_get_node(0, e_nso_get_link_node(link))) != NULL && V_NT_MATERIAL == e_ns_get_node_type(m_node))
			material = e_nso_get_link_node(link);
	return mesh->render.mat[0].material != material;
}

void p_lod_gap_count(ENode *node, PPolyStore *geometry, PMesh *mesh, ENode *o_node)
{

	ENode *m_node;
	EObjLink *link = NULL, *def = NULL;
	EGeoLayer *layer;
	PMeshMaterial *mat;
	egreal *v;
	uint32 i, *ref, stage, ref_count, vertex_count, mat_count = 1, param_count = 0;

	if(mesh->sub_stages[0] == 0)
	{
		if(o_node != NULL)
		{
			for(link = e_nso_get_next_link(o_node, 0); link != 0; link = e_nso_get_next_link(o_node, e_nso_get_link_id(link) + 1))
				if((m_node = e_ns_get_node(0, e_nso_get_link_node(link))) != NULL && V_NT_MATERIAL == e_ns_get_node_type(m_node))
					mat_count++;
			mesh->render.mat = mat = malloc((sizeof *mat) * mat_count);
			mesh->render.mat_count = 0;

			for(link = e_nso_get_next_link(o_node, 0); link != 0; link = e_nso_get_next_link(o_node, e_nso_get_link_id(link) + 1))
			{
				if((m_node = e_ns_get_node(0, e_nso_get_link_node(link))) != NULL && V_NT_MATERIAL == e_ns_get_node_type(m_node))
				{
					if(p_shader_get_param_count(m_node) > param_count)
						param_count = p_shader_get_param_count(m_node);
				//	if(def == NULL && strcmp("default", e_nso_get_link_name(link)) == 0)
				//		def = link; FIXME
					else if((layer = e_nsg_get_layer_by_name(node, e_nso_get_link_name(link))) != NULL && (VN_G_LAYER_POLYGON_FACE_UINT8 == e_nsg_get_layer_type(layer) || VN_G_LAYER_POLYGON_FACE_UINT32 == e_nsg_get_layer_type(layer)))
					{
						mat[mesh->render.mat_count].material = e_nso_get_link_node(link);
						mat[mesh->render.mat_count].tri_end = 0;
						mat[mesh->render.mat_count].quad_end = 0;
						mat[mesh->render.mat_count].id = e_nso_get_link_target_id(link);
						mat[mesh->render.mat_count].layer = e_nsg_get_layer_id(layer);
						mesh->render.mat_count++;
					}
				}
			}

			for(link = e_nso_get_next_link(o_node, 0); link != 0; link = e_nso_get_next_link(o_node, e_nso_get_link_id(link) + 1))
				if((m_node = e_ns_get_node(0, e_nso_get_link_node(link))) != NULL && V_NT_MATERIAL == e_ns_get_node_type(m_node))
					break;
			if(def != NULL)	
				mat[mesh->render.mat_count].material = e_nso_get_link_node(def);
			else if(mesh->render.mat_count == 0 && link != 0)
				mat[mesh->render.mat_count].material = e_nso_get_link_node(link);
			else
				mat[mesh->render.mat_count].material = -1;
		}else
		{
			mesh->render.mat = mat = malloc(sizeof *mat);
			mesh->render.mat_count = 0;
			mat[mesh->render.mat_count].material = -1;
		}

		mat[mesh->render.mat_count].tri_end = 0;
		mat[mesh->render.mat_count].quad_end = 0;
		mat[mesh->render.mat_count].id = -1;
		mat[mesh->render.mat_count].layer = -1;
		mesh->render.mat_count++;
		mesh->sub_stages[0] = 1;

		if(param_count != 0)
		{
			mesh->param.array_count = param_count;
			mesh->param.array = malloc((sizeof *mesh->param.array) * mesh->param.array_count);
			for(i = 0; i < mesh->param.array_count; i++)
				p_ra_clear_array(&mesh->param.array[i]);
		}else
		{
			mesh->param.array = 0;
			mesh->param.array_count = 0;
		}
	}
	if(mesh->sub_stages[0] == 1)
	{
		mesh->tess.order_node = malloc((sizeof *mesh->tess.order_node) * (geometry->base_quad_count + geometry->base_tri_count));
		mesh->tess.order_temp_mesh = malloc((sizeof *mesh->tess.order_temp_mesh) * (geometry->base_quad_count + geometry->base_tri_count));
		mesh->tess.order_temp_mesh_rev = malloc((sizeof *mesh->tess.order_temp_mesh_rev) * (geometry->base_quad_count + geometry->base_tri_count));
		mesh->tess.order_temp_poly_start = malloc((sizeof *mesh->tess.order_temp_poly_start) * (geometry->base_quad_count + geometry->base_tri_count));
		mesh->sub_stages[0] = 2;
		mesh->sub_stages[1] = 0;
		mesh->sub_stages[2] = 0;
		mesh->sub_stages[3] = geometry->base_quad_count;
	}
	if(mesh->sub_stages[0] == 2)
	{
		for(stage = mesh->sub_stages[1]; stage < geometry->base_quad_count + geometry->base_tri_count && stage < MAX_COUNT_STAGE_LOOPS ; stage++)
		{
			mesh->tess.order_temp_mesh[stage] = stage;
			mesh->tess.order_temp_mesh_rev[stage] = stage;
		}
		mesh->sub_stages[1] = stage;
		if(stage == geometry->base_quad_count + geometry->base_tri_count)
		{
			mesh->sub_stages[0] = 3;
			mesh->sub_stages[1] = 0;
		}
	}
	if(mesh->sub_stages[0] == 3)
	{
		v = e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node, 0));
		ref = e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node, 1));
		ref_count = e_nsg_get_polygon_length(node) * 4;
		vertex_count = e_nsg_get_vertex_length(node);
		for(stage = mesh->sub_stages[1]; stage < ref_count && stage < MAX_COUNT_STAGE_LOOPS ; stage += 4)
		{
			if(ref[stage] < vertex_count && ref[stage + 1] < vertex_count &&  ref[stage + 2] < vertex_count && v[ref[stage] * 3] != E_REAL_MAX && v[ref[stage + 1] * 3] != E_REAL_MAX && v[ref[stage + 2] * 3] != E_REAL_MAX)
			{
				if(ref[stage + 3] < vertex_count && v[ref[stage + 3] * 3] != E_REAL_MAX)
					mesh->tess.order_node[mesh->sub_stages[2]++] = stage / 4;
				else
					mesh->tess.order_node[mesh->sub_stages[3]++] = stage / 4;
			}
		}
		mesh->sub_stages[1] = stage;
		if(stage == ref_count)
		{
			mesh->render.mat->quad_end = mesh->sub_stages[2];
			mesh->render.mat->tri_end = mesh->sub_stages[3];
			mesh->sub_stages[0] = 0;
			mesh->sub_stages[1] = 0;
			mesh->sub_stages[2] = 0;
			mesh->sub_stages[3] = 0;
			mesh->stage++;
		}
	}
}

#if 0

void p_sds_gap_count(ENode *node, PPolyStore *geometry, PMesh *mesh)
{
	PMeshMaterial *mat;
	egreal *v;
	uint32 *ref, stage, ref_count, vertex_count;
	v = e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node, 0));
	ref = e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node, 1));
	ref_count = e_nsg_get_polygon_length(node);
	vertex_count = e_nsg_get_vertex_length(node);

	if(mesh->sub_stages[0] == 0)
	{
		mesh->tess.order = malloc((sizeof *mesh->tess.order) * (geometry->base_quad_count + geometry->base_tri_count));
		mesh->tess.order_geo = malloc((sizeof *mesh->tess.order_geo) * ref_count);
		mesh->tess.quad_count = 0;
		mesh->tess.tri_count = geometry->base_quad_count;
		mesh->sub_stages[0]++;
		mesh->sub_stages[1] = 0;
		mesh->sub_stages[2] = 0;
		mesh->sub_stages[3] = geometry->base_quad_count;
		mesh->render.mat = malloc(sizeof *mesh->render.mat);
		mesh->render.mat_count = 1;
		mesh->render.mat->material;
		mesh->render.mat->end;
		mesh->render.mat->id;
		mesh->render.mat->layer;
	}
	if(mesh->sub_stages[0] == 1) 
	{
		for(stage = mesh->sub_stages[1]; stage < ref_count && stage < MAX_COUNT_STAGE_LOOPS ; stage += 4)
		{
			if(ref[stage] < vertex_count && ref[stage + 1] < vertex_count &&  ref[stage + 2] < vertex_count && v[ref[stage] * 3] != E_REAL_MAX && v[ref[stage + 1] * 3] != E_REAL_MAX && v[ref[stage + 2] * 3] != E_REAL_MAX)
			{
				if(ref[stage + 3] < vertex_count && v[ref[stage + 3] * 3] != E_REAL_MAX)
				{
					mesh->tess.order[mesh->sub_stages[2]] = stage / 4;
					mesh->tess.order_geo[stage / 4] = mesh->sub_stages[2]++;
				}else
				{
					mesh->tess.order[mesh->sub_stages[3]] = stage / 4;
					mesh->tess.order_geo[stage / 4] = mesh->sub_stages[3]++;
				}
			}else
				mesh->tess.order_geo[stage / 4] = -1;
		}
		mesh->sub_stages[1] = stage;
		if(stage == ref_count)
		{
			mesh->sub_stages[0]++;
			mesh->sub_stages[1] = 0;
			mesh->sub_stages[2] = 0;
			mesh->sub_stages[3] = 0;
			mesh->stage++;
		}
	}
	
	if(mesh->sub_stages[0] == 1)
	{
		for(stage = mesh->sub_stages[1]; stage < ref_count && stage < MAX_COUNT_STAGE_LOOPS ; stage += 4)
		{
			if(ref[stage] < vertex_count && ref[stage + 1] < vertex_count &&  ref[stage + 2] < vertex_count && v[ref[stage] * 3] != E_REAL_MAX && v[ref[stage + 1] * 3] != E_REAL_MAX && v[ref[stage + 2] * 3] != E_REAL_MAX)
			{
				if(ref[stage + 3] < vertex_count && v[ref[stage + 3] * 3] != E_REAL_MAX)
					sort[mesh->tess.quad_count++] = stage / 4;
				else
					sort[mesh->tess.tri_count++] = stage / 4;	
			}
		}
		mesh->sub_stages[1] = stage
		if(stage == ref_count)
		{
		}

	}
	if(stage < MAX_COUNT_STAGE_LOOPS)
	{
		mesh->tess.tri_count -= mesh->tess.quad_count;
		mesh->stage++;
		mesh->sub_stages[0] = 0;
	}
}
#endif



void p_sort_materials(ENode *g_node, ENode *o_node, PMesh *mesh)
{
	EObjLink *link, *def = NULL;
	EGeoLayer *layer;
	PMeshMaterial *mat;
	ENode *m_node;
	uint i, j, k, mat_count = 1, *holes, *sort, sort_pos = 0, poly_length;
	char *t;

//	holes = mesh->tess.order;

	mat = malloc((sizeof *mat) * mat_count);
	mat_count = 0;

	for(link = e_nso_get_next_link(o_node, 0); link != 0; link = e_nso_get_next_link(o_node, e_nso_get_link_id(link) + 1))
	{
		if((m_node = e_ns_get_node(0, e_nso_get_link_node(link))) != NULL && V_NT_MATERIAL == e_ns_get_node_type(m_node))
		{
	/*		if(def == NULL && strcmp("default", e_nso_get_link_name(link)) == 0)
				def = link; FIXME
			else*/ if((layer = e_nsg_get_layer_by_name(g_node, e_nso_get_link_name(link))) != NULL && (VN_G_LAYER_POLYGON_FACE_UINT8 == e_nsg_get_layer_type(layer) || VN_G_LAYER_POLYGON_FACE_UINT32 == e_nsg_get_layer_type(layer)));
			{
				mat[mat_count].material = e_nso_get_link_node(link);
				mat[mat_count].tri_end = 0;
				mat[mat_count].id = e_nso_get_link_target_id(link);
				mat[mat_count].layer = e_nsg_get_layer_id(layer);
				mat_count++;
			}
		}
	}

	if(mat_count == 0)
	{
		free(mat);
		return;
	}
	sort = malloc((sizeof *sort) * (mesh->tess.tri_count + mesh->tess.quad_count));

	poly_length = e_nsg_get_polygon_length(g_node);
	for(i = 1; i < mat_count - 1 && mat[0].layer == mat[i].layer ;i++);
	if(i < mat_count - 1)
	{
		uint8 **data8;
		uint32 **data32;
		boolean *type;
		data8 = malloc((sizeof *data8) * mat_count);
		data32 = malloc((sizeof *data32) * mat_count);
		type = malloc((sizeof *type) * mat_count);
		for(i = 0; i < mat_count - 1; i++)
		{
			layer = e_nsg_get_layer_by_id(g_node, mat[i].layer);
			type[i] = (VN_G_LAYER_POLYGON_FACE_UINT8 == e_nsg_get_layer_type(layer));
			if(type[i])
				data8[i] = e_nsg_get_layer_data(g_node, layer);
			else
				data32[i] = e_nsg_get_layer_data(g_node, layer);
		}
		for(i = 0; i < mat_count - 1; i++)
		{
			for(j = 0; j < poly_length; j++)
			{
				if(data8[j] == mat[i].id)
				{
					for(k = 0; k < i; k++)
					{
						if(type[k])
						{
							if(data8[holes[j]] == mat[i].id)
								break;
						}else
						{
							if(data32[holes[j]] == mat[i].id)
								break;
						}
					}
					if(k == i)
						sort[sort_pos++] = j;
				}
			}
			mat[i].tri_end = sort_pos;
		}
		for(j = 0; j < poly_length; j++)
		{
			for(k = 0; k < i; k++)
			{
				if(type[k])
				{
					if(data8[holes[j]] == mat[i].id)
						break;
				}else if(data32[holes[j]] == mat[i].id)
					break;
			}
			if(k == i)
				sort[sort_pos++] = j;
		}
		mat[i].tri_end = sort_pos;
		free(data8);
		free(data32);
		free(type);
	}else
	{
		layer = e_nsg_get_layer_by_id(g_node, mat[i].layer);
		if(VN_G_LAYER_POLYGON_FACE_UINT8 == e_nsg_get_layer_type(layer))
		{
			uint8 *data;
			data = e_nsg_get_layer_data(g_node, layer);
			for(i = 0; i < mat_count - 1; i++)
			{
				for(j = 0; j < poly_length; j++)
					if(data[holes[j]] == mat[i].id)
						sort[sort_pos++] = j;
					mat[i].tri_end = j;
			}
			for(j = 0; j < poly_length; j++)
			{
				for(k = 0; k < mat_count - 1 && data[holes[j]] != mat[i].id; k++);
				if(k == mat_count)
				{
					sort[sort_pos++] = j;

				}
			}
			mat[i].tri_end = j;
		}else
		{
			uint32 *data;
			data = e_nsg_get_layer_data(g_node, layer);
			for(i = 0; i < mat_count - 1; i++)
			{
				for(j = 0; j < poly_length; j++)
					if(data[holes[j]] == mat[i].id)
						sort[sort_pos++] = j;
				mat[i].tri_end = j;
			}
			for(j = 0; j < poly_length; j++)
			{
				for(k = 0; k < mat_count - 1 && data[holes[j]] != mat[i].id; k++);
				if(k == mat_count)
					sort[sort_pos++] = j;
			}
			mat[i].tri_end = j;
		}
	}
//	mesh->tess.order = sort;
}
