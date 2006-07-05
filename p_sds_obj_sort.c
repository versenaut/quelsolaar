#include "enough.h"
#include "p_sds_geo.h"
#include "p_sds_table.h"
#include "p_sds_obj.h"

extern boolean p_shader_transparancy(uint32 node_id);
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

void p_pre_load_material_select(ENode *node)
{
	ENode *m_node,  *g_node;
	EObjLink *g_link = NULL, *m_link = NULL;
	EGeoLayer *layer;
	if(node != NULL)
	{
		for(g_link = e_nso_get_next_link(node, 0); g_link != 0; g_link = e_nso_get_next_link(node, e_nso_get_link_id(g_link) + 1))
		{
			if((g_node = e_ns_get_node(0, e_nso_get_link_node(g_link))) != NULL && V_NT_GEOMETRY == e_ns_get_node_type(g_node))
			{
				for(m_link = e_nso_get_next_link(node, 0); m_link != 0; m_link = e_nso_get_next_link(node, e_nso_get_link_id(m_link) + 1))
				{
					if((m_node = e_ns_get_node(0, e_nso_get_link_node(m_link))) != NULL && V_NT_MATERIAL == e_ns_get_node_type(m_node))
					{
						layer = e_nsg_get_layer_by_name(g_node, e_nso_get_link_name(m_link));
						if(layer != NULL && (VN_G_LAYER_POLYGON_FACE_UINT8 == e_nsg_get_layer_type(layer) || VN_G_LAYER_POLYGON_FACE_UINT32 == e_nsg_get_layer_type(layer)))
							e_nsg_get_layer_data(node, layer);
					}
				}
			}
		}
	}
}

void p_lod_gap_count(ENode *node, PPolyStore *geometry, PMesh *mesh, ENode *o_node)
{

	ENode *m_node;
	EObjLink *link = NULL, *def = NULL;
	EGeoLayer *layer;
	PMeshMaterial *mat;
	egreal *v;
	uint32 i, j, k, *ref, stage, ref_count, vertex_count, mat_count = 0, param_count = 0;

	if(mesh->sub_stages[0] == 0)
	{
		mesh->render.mat_count = 0;
		if(o_node != NULL)
		{
			for(link = e_nso_get_next_link(o_node, 0); link != 0; link = e_nso_get_next_link(o_node, e_nso_get_link_id(link) + 1))
				if((m_node = e_ns_get_node(0, e_nso_get_link_node(link))) != NULL && V_NT_MATERIAL == e_ns_get_node_type(m_node))
					mat_count++;
			if(mat_count > 0)
			{
				mesh->render.mat = mat = malloc((sizeof *mat) * mat_count);
				for(link = e_nso_get_next_link(o_node, 0); link != 0; link = e_nso_get_next_link(o_node, e_nso_get_link_id(link) + 1))
				{
					if((m_node = e_ns_get_node(0, e_nso_get_link_node(link))) != NULL && V_NT_MATERIAL == e_ns_get_node_type(m_node))
					{
						if(p_shader_get_param_count(m_node) > param_count)
							param_count = p_shader_get_param_count(m_node);
						layer = e_nsg_get_layer_by_name(node, e_nso_get_link_name(link));
						if(mesh->render.mat_count == 0 || (layer != NULL && (VN_G_LAYER_POLYGON_FACE_UINT8 == e_nsg_get_layer_type(layer) || VN_G_LAYER_POLYGON_FACE_UINT32 == e_nsg_get_layer_type(layer))))
						{
							mat[mesh->render.mat_count].material = e_nso_get_link_node(link);
							mat[mesh->render.mat_count].material_version = 0;
							mat[mesh->render.mat_count].tri_end = 0;
							mat[mesh->render.mat_count].quad_end = 0;
							mat[mesh->render.mat_count].id = e_nso_get_link_target_id(link);
							if(layer != NULL)
								mat[mesh->render.mat_count].layer = e_nsg_get_layer_id(layer);
							else
								mat[mesh->render.mat_count].layer = -1;
							if(p_shader_transparancy(mat[mesh->render.mat_count].material))
								mesh->render.shadows = FALSE;
							mesh->render.mat_count++;
						}
					}
				}
			}
		}
		if(mesh->render.mat_count == 0)
		{
			mesh->render.mat = mat = malloc(sizeof *mat);
			mesh->render.mat_count = 0;
			mat[mesh->render.mat_count].tri_end = 0;
			mat[mesh->render.mat_count].quad_end = 0;
			mat[mesh->render.mat_count].id = 0;
			mat[mesh->render.mat_count].layer = -1;
			mat[mesh->render.mat_count].material = -1;
			mat[mesh->render.mat_count].material_version = 0;
			mesh->render.mat_count++;
		}



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
		mesh->sub_stages[0] = 1;
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
		uint32 *buf;
		v = e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node, 0));
		ref = e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node, 1));
		ref_count = e_nsg_get_polygon_length(node);
		vertex_count = e_nsg_get_vertex_length(node);
		buf = malloc((sizeof *buf) * ref_count);
		for(i = 0; i < ref_count; i++)
			buf[i] = mat[0].id;
		for(i = mesh->render.mat_count - 1; i < mesh->render.mat_count; i--)
		{
			layer = e_nsg_get_layer_by_id(node, mesh->render.mat[i].layer);
			if(layer != NULL)
			{
				VNGLayerType type;
				void *data;
				k = mat[i].id;
				type = e_nsg_get_layer_type(layer);
				data = e_nsg_get_layer_data(node, layer);
				if(VN_G_LAYER_POLYGON_FACE_UINT8 == type && k < 256)
					for(j = 0; j < ref_count; j++)
						if(((uint8 *)data)[j] == k)
							buf[j] = k;

				if(VN_G_LAYER_POLYGON_FACE_UINT32 == type)
					for(j = 0; j < ref_count; j++)
						if(((uint32 *)data)[j] == k)
							buf[j] = k;
			}
		}
/*
		uint32 *order_temp_mesh;  how the PMesh polys refers to the geometry mesh polygs (deleted once used)
		uint32 *order_temp_mesh_rev;  how the geometry mesh polys refers to the PMesh polys (deleted once used)	
*/
		ref_count *= 4;
		mesh->sub_stages[2] = 0;
		for(i = 0; i < mesh->render.mat_count; i++)
		{
			j = 0;
		//	k = geometry->base_quad_count;
			for(stage = mesh->sub_stages[1]; stage < ref_count /*&& stage < MAX_COUNT_STAGE_LOOPS*/ ; stage += 4)
			{
				if(ref[stage] < vertex_count && ref[stage + 1] < vertex_count &&  ref[stage + 2] < vertex_count && v[ref[stage] * 3] != E_REAL_MAX && v[ref[stage + 1] * 3] != E_REAL_MAX && v[ref[stage + 2] * 3] != E_REAL_MAX)
				{
					if(ref[stage + 3] < vertex_count && v[ref[stage + 3] * 3] != E_REAL_MAX)
					{
						if(mesh->render.mat[i].id == buf[stage / 4])
						{
							mesh->tess.order_temp_mesh[mesh->sub_stages[2]] = j;
							mesh->tess.order_temp_mesh_rev[j] = mesh->sub_stages[2];
							mesh->tess.order_node[mesh->sub_stages[2]++] = stage / 4;
						}
						j++;
					}
				}
			}
			mesh->render.mat[i].quad_end = mesh->sub_stages[2];
			for(stage = mesh->sub_stages[1]; stage < ref_count /*&& stage < MAX_COUNT_STAGE_LOOPS*/ ; stage += 4)
			{
				if(ref[stage] < vertex_count && ref[stage + 1] < vertex_count &&  ref[stage + 2] < vertex_count && v[ref[stage] * 3] != E_REAL_MAX && v[ref[stage + 1] * 3] != E_REAL_MAX && v[ref[stage + 2] * 3] != E_REAL_MAX)
				{
					if(ref[stage + 3] >= vertex_count || v[ref[stage + 3] * 3] == E_REAL_MAX)
					{
						if(mesh->render.mat[i].id == buf[stage / 4])
						{
							mesh->tess.order_temp_mesh[mesh->sub_stages[2]] = j;
							mesh->tess.order_temp_mesh_rev[j] = mesh->sub_stages[2];
							mesh->tess.order_node[mesh->sub_stages[2]++] = stage / 4;
						}
						j++;
					}
				}
			}
			mesh->render.mat[i].tri_end = mesh->sub_stages[2];
		}
		if(mesh->render.mat[i - 1].tri_end != mesh->tess.tri_count + mesh->tess.quad_count)
		{
//			printf("error: %u %u - %u %u\n", mesh->render.mat[i - 1].tri_end, mesh->tess.tri_count + mesh->tess.quad_count, mesh->tess.tri_count, mesh->tess.quad_count);
		//	error: 7 10 - 0 10
				   {
					   int *a = NULL;
					   a[0] = 0;
				   }
			exit(0);
		}
	//	if(mesh->render.mat_count > 1)
	//		exit(0);
/*
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
		if(stage == ref_count)*/
		{
			mesh->sub_stages[0] = 0;
			mesh->sub_stages[1] = 0;
			mesh->sub_stages[2] = 0;
			mesh->sub_stages[3] = 0;
			mesh->stage++;
		}
		free(buf);
	}
}
