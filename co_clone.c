#include "st_matrix_operations.h"
#include "enough.h"
#include "seduce.h"
#include "persuade.h"

uint32 co_clone_from_node = -1;
uint32 co_clone_to_node = -1;

boolean co_clone_node_sub = FALSE;
boolean co_clone_tag_sub = FALSE;
boolean co_clone_method_sub = FALSE;
boolean co_clone_layer_sub = FALSE;


boolean co_compare_text(char *a, char *b)
{
	uint i;
	for(i = 0; a[i] != 0 && a[i] == b[i]; i++);
	return a[i] == b[i];
}

void co_clone_head(ENode *node, ENode *clone)
{
	uint16 i, j, k;
	VNodeID node_id;

	node_id = e_ns_get_node_id(clone);
	if(!co_clone_node_sub)
		for(i = e_ns_get_next_tag_group(node, 0); i != (uint16)-1; i = e_ns_get_next_tag_group(node, i + 1))
			verse_send_tag_group_create(node_id, -1, e_ns_get_tag_group(node, i));
	if(!co_clone_tag_sub)
	{
		for(i = e_ns_get_next_tag_group(node, 0); i != (uint16)-1; i = e_ns_get_next_tag_group(node, i + 1))
		{
			for(j = e_ns_get_next_tag_group(clone, 0); j != (uint16)-1; j = e_ns_get_next_tag_group(clone, j + 1))
				if(co_compare_text(e_ns_get_tag_group(node, i), e_ns_get_tag_group(clone, j)))
					break;
			if(j == (uint16)-1)
				break;
		}
		if(i == (uint16)-1)
		{
			for(i = e_ns_get_next_tag_group(node, 0); i != (uint16)-1; i = e_ns_get_next_tag_group(node, i + 1))
			{
				for(j = e_ns_get_next_tag_group(clone, 0); j != (uint16)-1; j = e_ns_get_next_tag_group(clone, j + 1))
					if(co_compare_text(e_ns_get_tag_group(node, i), e_ns_get_tag_group(clone, j)))
						break;
				if(j != (uint16)-1)
				for(k = e_ns_get_next_tag(node, i, 0); k != (uint16)-1; k = e_ns_get_next_tag(node, i, k + 1))
					verse_send_tag_create(node_id, j, -1, e_ns_get_tag_name(node, i, k), e_ns_get_tag_type(node, i, k), e_ns_get_tag(node, i, k));
			}
			co_clone_tag_sub = TRUE;
		}
	}
}


void co_clone_object(ENode *node, ENode *clone)
{
	double a[4], b[4], c[4], d[4], e[4];
	VNQuat64 qa, qb, qc, qd;
	EObjLink *link;
	uint16 i, j, k;
	uint time[2];
	VNodeID id;
	id = e_ns_get_node_id(clone);

	co_clone_head(node, clone);
	if(!co_clone_node_sub)
	{
		e_nso_get_pos(node, a, b, c, d, e, time);
		verse_send_o_transform_pos_real64(id, time[0], time[1], a, b, c, d, e[0]);
		e_nso_get_rot(node, &qa, &qb, &qc, &qd, e, time);
		verse_send_o_transform_rot_real64(id, time[0], time[1], &qa, &qb, &qc, &qd, e[0]);
		e_nso_get_scale(node, a);
		verse_send_o_transform_scale_real64(id, a[0], a[1], a[2]);
		e_nso_get_light(node, a);
		verse_send_o_light_set(id, a[0], a[1], a[2]);
		for(link = e_nso_get_next_link(node, 0); link != 0; link = e_nso_get_next_link(node, e_nso_get_link_id(link) + 1))
			verse_send_o_link_set(id, -1, e_nso_get_link_node(link), e_nso_get_link_name(link), e_nso_get_link_target_id(link));
		verse_send_o_hide(id, e_nso_get_hide(node));
		for(i = e_nso_get_next_method_group(node, 0); i != (uint16)-1; i = e_nso_get_next_method_group(node, i + 1))
			verse_send_o_method_group_create(id, -1, e_nso_get_method_group(node, i));
		co_clone_node_sub = TRUE;
	}
	if(!co_clone_method_sub)
	{
		for(i = e_nso_get_next_method_group(node, 0); i != (uint16)-1; i = e_nso_get_next_method_group(node, i + 1))
		{
			for(j = e_nso_get_next_method_group(clone, 0); j != (uint16)-1; j = e_nso_get_next_method_group(clone, j + 1))
				if(co_compare_text(e_nso_get_method_group(node, i), e_nso_get_method_group(clone, j)))
					break;
			if(j == (uint16)-1)
				break;
		}
		if(i == (uint16)-1)
		{
			for(i = e_nso_get_next_method_group(node, 0); i != (uint16)-1; i = e_nso_get_next_method_group(node, i + 1))
			{
				for(j = e_nso_get_next_method_group(clone, 0); j != (uint16)-1; j = e_nso_get_next_method_group(clone, j + 1))
					if(co_compare_text(e_nso_get_method_group(node, i), e_nso_get_method_group(clone, j)))
						break;
				if(j != (uint16) -1)
					for(k = e_nso_get_next_method(node, i, 0); k != (uint16)-1; k = e_nso_get_next_method(node, i, k + 1))
						verse_send_o_method_create(id, j, -1, e_nso_get_method(node, i, k), e_nso_get_method_param_count(node, i, k),
									   e_nso_get_method_param_types(node, i, k), e_nso_get_method_param_names(node, i, k));
			}
			co_clone_method_sub = TRUE;
		}
	}
}

void co_clone_geometry(ENode *node, ENode *clone)
{
	VNMFragmentID frag;
	VNodeID node_id;
	EGeoLayer *layer, *clayer;
	double pos[3];
	uint i, v_length, p_length, *ref;
	uint16 bone_id;
	void *data;
	egreal *vertex;
	node_id = e_ns_get_node_id(clone);
	co_clone_head(node, clone);
	if(!co_clone_node_sub)
	{
		for(layer = e_nsg_get_layer_next(node, 2); layer != NULL && layer != NULL; layer = e_nsg_get_layer_next(node, e_nsg_get_layer_id(layer) + 1))
			verse_send_g_layer_create(node_id, e_nsg_get_layer_id(layer), e_nsg_get_layer_name(layer), e_nsg_get_layer_type(layer), 0, 0);
		for(bone_id = e_nsg_get_bone_next(node, 0); bone_id != (uint16)-1; bone_id = e_nsg_get_bone_next(node, bone_id + 1))
		{
			e_nsg_get_bone_pos64(node, bone_id, pos);
		}
		verse_send_g_crease_set_vertex(node_id, e_nsg_get_layer_crease_vertex_name(node), e_nsg_get_layer_crease_vertex_value(node));
		verse_send_g_crease_set_edge(node_id, e_nsg_get_layer_crease_edge_name(node), e_nsg_get_layer_crease_edge_value(node));
		co_clone_node_sub = TRUE;
	}
	if(!co_clone_layer_sub)
	{
		for(layer = e_nsg_get_layer_next(node, 0); layer != NULL; layer = e_nsg_get_layer_next(node, e_nsg_get_layer_id(layer) + 1))
		{
			for(clayer = e_nsg_get_layer_next(clone, 0); clayer != NULL; clayer = e_nsg_get_layer_next(clone, e_nsg_get_layer_id(clayer) + 1))
				if(co_compare_text(e_nsg_get_layer_name(layer), e_nsg_get_layer_name(clayer)))
					break;
			if(clayer == NULL)
				break;
		}
		if(layer == NULL)
		{
			v_length = e_nsg_get_vertex_length(node);
			p_length = e_nsg_get_polygon_length(node);
			vertex = e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node, 0));
			ref = e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node, 1));
			for(layer = e_nsg_get_layer_next(node, 0); layer != NULL; layer = e_nsg_get_layer_next(node, e_nsg_get_layer_id(layer) + 1))
			{
				for(clayer = e_nsg_get_layer_next(clone, 0); clayer != NULL ; clayer = e_nsg_get_layer_next(clone, e_nsg_get_layer_id(clayer) + 1))
					if(co_compare_text(e_nsg_get_layer_name(layer), e_nsg_get_layer_name(clayer)))
						break;
				if(clone != NULL)
				{
					data = e_nsg_get_layer_data(node, layer);
					switch(e_nsg_get_layer_type(layer))
					{
						case VN_G_LAYER_VERTEX_XYZ :
							for(i = 0; i < v_length; i++)
								if(vertex[i * 3] != E_REAL_MAX)
									verse_send_g_vertex_set_xyz_real64(node_id, e_nsg_get_layer_id(clayer), i, ((egreal *)data)[i * 3 + 0], ((egreal *)data)[i * 3 + 1], ((egreal *)data)[i * 3 + 2]);
						break;
						case VN_G_LAYER_VERTEX_UINT32 :
							for(i = 0; i < v_length; i++)
								if(vertex[i * 3] != E_REAL_MAX)
									verse_send_g_vertex_set_uint32(node_id, e_nsg_get_layer_id(clayer), i, ((uint32 *)data)[i]);
						break;
						case VN_G_LAYER_VERTEX_REAL :
							for(i = 0; i < v_length; i++)
								if(vertex[i * 3] != E_REAL_MAX)
									verse_send_g_vertex_set_real64(node_id, e_nsg_get_layer_id(clayer), i, ((egreal *)data)[i]);
						break;
						case VN_G_LAYER_POLYGON_CORNER_UINT32 :
							for(i = 0; i < p_length; i++)
								if(ref[i * 4] != -1)
									verse_send_g_polygon_set_corner_uint32(node_id, e_nsg_get_layer_id(clayer), i, ((uint32 *)data)[i * 4 + 0], ((uint32 *)data)[i * 4 + 1], ((uint32 *)data)[i * 4 + 2], ((uint32 *)data)[i * 4 + 3]);
						break;
						case VN_G_LAYER_POLYGON_CORNER_REAL :
							for(i = 0; i < p_length; i++)
								if(ref[i * 4] != -1)
									verse_send_g_polygon_set_corner_real64(node_id, e_nsg_get_layer_id(clayer), i, ((egreal *)data)[i * 4 + 0], ((egreal *)data)[i * 4 + 1], ((egreal *)data)[i * 4 + 2], ((egreal *)data)[i * 4 + 3]);
						break;
						case VN_G_LAYER_POLYGON_FACE_UINT32 :
							for(i = 0; i < p_length; i++)
								if(ref[i * 4] != -1)
									verse_send_g_polygon_set_face_uint8(node_id, e_nsg_get_layer_id(clayer), i, ((uint32 *)data)[i]);
						break;
						case VN_G_LAYER_POLYGON_FACE_UINT8 :
							for(i = 0; i < p_length; i++)
								if(ref[i * 4] != -1)
									verse_send_g_polygon_set_face_uint32(node_id, e_nsg_get_layer_id(clayer), i, ((uint8 *)data)[i]);
						break;
						case VN_G_LAYER_POLYGON_FACE_REAL :
							for(i = 0; i < p_length; i++)
								if(ref[i * 4] != -1)
									verse_send_g_polygon_set_face_real64(node_id, e_nsg_get_layer_id(clayer), i, ((egreal *)data)[i]);
						break;
					}
				}
			}
			co_clone_layer_sub = TRUE;
		}
	}
}

void co_clone_material(ENode *node, ENode *clone)
{
	VNMFragmentID frag;
	VNodeID node_id;
	co_clone_head(node, clone);
	if(!co_clone_node_sub)
	{
		node_id = e_ns_get_node_id(clone);
		for(frag = e_nsm_get_fragment_next(node, 0); frag != (VNMFragmentID)-1; frag = e_nsm_get_fragment_next(node, frag + 1))
			verse_send_m_fragment_create(node_id, frag, e_nsm_get_fragment_type(node, frag), e_nsm_get_fragment(node, frag));
		co_clone_node_sub = TRUE;
	}
}


void co_clone_bitmap(ENode *node, ENode *clone)
{
	VNodeID node_id;
	EBitLayer *layer, *clayer;
	uint i, j, k, l, size[3];
	VNBTile tile;
	void *data;
	node_id = e_ns_get_node_id(clone);
	co_clone_head(node, clone);

	if(!co_clone_node_sub)
	{
		for(layer = e_nsb_get_layer_next(node, 0); layer != NULL && layer != NULL; layer = e_nsb_get_layer_next(node, e_nsb_get_layer_id(layer) + 1))
			verse_send_b_layer_create(node_id, e_nsb_get_layer_id(layer), e_nsb_get_layer_name(layer), e_nsb_get_layer_type(layer));
		e_nsb_get_size(node, size, &size[1], &size[2]);
		verse_send_b_dimensions_set(node_id, size[0], size[1], size[2]);
		co_clone_node_sub = TRUE;
	}
	if(!co_clone_layer_sub)
	{
		for(layer = e_nsb_get_layer_next(node, 0); layer != NULL; layer = e_nsb_get_layer_next(node, e_nsb_get_layer_id(layer) + 1))
		{
			for(clayer = e_nsb_get_layer_next(clone, 0); clayer != NULL; clayer = e_nsb_get_layer_next(clone, e_nsb_get_layer_id(clayer) + 1))
				if(co_compare_text(e_nsb_get_layer_name(layer), e_nsb_get_layer_name(clayer)))
					break;
			if(clayer == NULL)
				break;
			printf("Got A\n");
		}
		if(layer == NULL)
		{
			printf("Got B\n");
			e_nsb_get_size(node, size, &size[1], &size[2]);
			for(layer = e_nsb_get_layer_next(node, 0); layer != NULL; layer = e_nsb_get_layer_next(node, e_nsb_get_layer_id(layer) + 1))
			{
				for(clayer = e_nsb_get_layer_next(clone, 0); clayer != NULL ; clayer = e_nsb_get_layer_next(clone, e_nsb_get_layer_id(clayer) + 1))
					if(co_compare_text(e_nsb_get_layer_name(layer), e_nsb_get_layer_name(clayer)))
						break;
				if(clone != NULL)
				{
					printf("Got C\n");
					data = e_nsb_get_layer_data(node, layer);
					switch(e_nsb_get_layer_type(layer))
					{
						case VN_B_LAYER_UINT8 :
							for(i = 0; i < size[0]; i += VN_B_TILE_SIZE)
							{
								for(j = 0; j < size[1]; j += VN_B_TILE_SIZE)
								{
									for(k = 0; k < size[2]; k++)
									{
										for(l = 0; l < VN_B_TILE_SIZE * VN_B_TILE_SIZE; l++)
											if(i + l % VN_B_TILE_SIZE < size[0] && ((l / VN_B_TILE_SIZE) + j) < size[1])
												tile.vuint8[l] = ((uint8 *)data)[i + j * size[0] + k * size[0] * size[1] + l % VN_B_TILE_SIZE + (l / VN_B_TILE_SIZE) * size[0]];
										verse_send_b_tile_set(node_id, e_nsb_get_layer_id(clayer), i / VN_B_TILE_SIZE, j / VN_B_TILE_SIZE, k, VN_B_LAYER_UINT8, &tile);
									}
								}
							}
						break;
						case VN_B_LAYER_UINT16 :
							for(i = 0; i < size[0]; i += VN_B_TILE_SIZE)
							{
								for(j = 0; j < size[1]; j += VN_B_TILE_SIZE)
								{
									for(k = 0; k < size[2]; k++)
									{
										for(l = 0; l < VN_B_TILE_SIZE * VN_B_TILE_SIZE; l++)
											if(i + l % VN_B_TILE_SIZE < size[0] && ((l / VN_B_TILE_SIZE) + j) < size[1])
												tile.vuint8[l] = ((uint16 *)data)[i + j * size[0] + k * size[0] * size[1] + l % VN_B_TILE_SIZE + (l / VN_B_TILE_SIZE) * size[0]];
										verse_send_b_tile_set(node_id, e_nsb_get_layer_id(clayer), i / VN_B_TILE_SIZE, j / VN_B_TILE_SIZE, k, VN_B_LAYER_UINT16, &tile);
									}
								}
							}
						break;
						case VN_B_LAYER_REAL32 :
							for(i = 0; i < size[0]; i += VN_B_TILE_SIZE)
							{
								for(j = 0; j < size[1]; j += VN_B_TILE_SIZE)
								{
									for(k = 0; k < size[2]; k++)
									{
										for(l = 0; l < VN_B_TILE_SIZE * VN_B_TILE_SIZE; l++)
											if(i + l % VN_B_TILE_SIZE < size[0] && ((l / VN_B_TILE_SIZE) + j) < size[1])
												tile.vuint8[l] = ((float *)data)[i + j * size[0] + k * size[0] * size[1] + l % VN_B_TILE_SIZE + (l / VN_B_TILE_SIZE) * size[0]];
										verse_send_b_tile_set(node_id, e_nsb_get_layer_id(clayer), i / VN_B_TILE_SIZE, j / VN_B_TILE_SIZE, k, VN_B_LAYER_REAL32, &tile);
									}
								}
							}
						break;
						case VN_B_LAYER_REAL64 :
							for(i = 0; i < size[0]; i += VN_B_TILE_SIZE)
							{
								for(j = 0; j < size[1]; j += VN_B_TILE_SIZE)
								{
									for(k = 0; k < size[2]; k++)
									{
										for(l = 0; l < VN_B_TILE_SIZE * VN_B_TILE_SIZE; l++)
											if(i + l % VN_B_TILE_SIZE < size[0] && ((l / VN_B_TILE_SIZE) + j) < size[1])
												tile.vuint8[l] = ((double *)data)[i + j * size[0] + k * size[0] * size[1] + l % VN_B_TILE_SIZE + (l / VN_B_TILE_SIZE) * size[0]];
										verse_send_b_tile_set(node_id, e_nsb_get_layer_id(clayer), i / VN_B_TILE_SIZE, j / VN_B_TILE_SIZE, k, VN_B_LAYER_REAL64, &tile);
									}
								}
							}
						break;
					}
				}
			}
			co_clone_layer_sub = TRUE;
		}
	}
}

void co_clone_node(ENode *node)
{
	void *layer, *buffer;
	co_clone_from_node = e_ns_get_node_id(node);
	co_clone_node_sub = FALSE;
	co_clone_tag_sub = FALSE;
	co_clone_method_sub = FALSE;
	co_clone_layer_sub = FALSE;
	
	if(e_ns_get_node_type(node) == V_NT_GEOMETRY)
		for(layer = e_nsg_get_layer_next(node, 0); layer != NULL; layer = e_nsg_get_layer_next(node, e_nsg_get_layer_id(layer) + 1))
			e_nsg_get_layer_data(node, layer);
	if(e_ns_get_node_type(node) == V_NT_BITMAP)
		for(layer = e_nsb_get_layer_next(node, 0); layer != NULL; layer = e_nsb_get_layer_next(node, e_nsb_get_layer_id(layer) + 1))
			e_nsb_get_layer_data(node, layer);
	if(e_ns_get_node_type(node) == V_NT_TEXT)
		for(buffer = e_nst_get_buffer_next(node, 0); buffer != NULL; buffer = e_nst_get_buffer_next(node, e_nst_get_buffer_id(buffer) + 1))
			e_nst_get_buffer_data(node, buffer);

	verse_send_node_create(-1, e_ns_get_node_type(node), VN_OWNER_MINE);
}

void co_clone_node_update(void)
{
	static version, timer = 0;
	ENode *node, *clone;
	if((node = e_ns_get_node(0, co_clone_from_node)) != NULL && (clone = e_ns_get_node(0, co_clone_to_node)) != NULL && e_ns_get_node_type(clone) == e_ns_get_node_type(node))
	{
	/*	if(version != e_ns_get_node_version_data(node))
		{
			timer = 0;
			version = e_ns_get_node_version_data(node);
		}
		if(timer < 40)
		{
			timer++;
			return;
		}*/
		switch(e_ns_get_node_type(clone))
		{
			case V_NT_OBJECT :
				co_clone_object(node, clone);
			break;
			case V_NT_GEOMETRY :
				co_clone_geometry(node, clone);
			break;
			case V_NT_MATERIAL :
				co_clone_material(node, clone);
			break;
			case V_NT_BITMAP :
				co_clone_bitmap(node, clone);
			break;
		}
	}
}

void co_node_create_func(uint connection, uint id, VNodeType type, void *user)
{
	ENode *node;
	if((node = e_ns_get_node(0, co_clone_from_node)) != NULL && type == e_ns_get_node_type(node))
		co_clone_to_node = id;
}
