#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"
#include "seduce.h"
#include "co_vn_graphics.h"
#include "co_vn_handle.h"
#include "co_widgets.h"

extern boolean co_handle_head(BInputState *input, ENode *node, float *length);

static uint change_g_node_id = 0;

void rename_g_layer_func(void *user, char *text)
{
	EBitLayer *layer;
	ENode *node;
	uint16 bone_id;
	if((node = e_ns_get_node(0, change_g_node_id)) != NULL)
	{
		for(layer = e_nsg_get_layer_next(node, 0); layer != NULL && layer != user ; layer = e_nsg_get_layer_next(node, e_nsg_get_layer_id(layer) + 1));
		if(layer == user)
			verse_send_g_layer_create(change_g_node_id, e_nsg_get_layer_id(layer), text, e_nsg_get_layer_type(layer), 0, 0);

		for(bone_id = e_nsg_get_bone_next(node, 0); bone_id != (uint16)-1; bone_id = e_nsg_get_bone_next(node, bone_id + 1))
		{
			double 		pos[3];
			VNQuat64	rot;
			e_nsg_get_bone_pos64(node, bone_id, pos);
			e_nsg_get_bone_rot64(node, bone_id, &rot);

			if(layer == e_nsg_get_bone_weight(node, bone_id))
				verse_send_g_bone_create(change_g_node_id, bone_id, text, e_nsg_get_bone_reference(node, bone_id), e_nsg_get_bone_parent(node, bone_id), pos[0], pos[1], pos[2], &rot);
			if(layer == e_nsg_get_bone_weight(node, bone_id))
				verse_send_g_bone_create(change_g_node_id, bone_id, e_nsg_get_bone_weight(node, bone_id), text, e_nsg_get_bone_parent(node, bone_id), pos[0], pos[1], pos[2], &rot);
		}
	}
}

extern float co_handle_node_head(BInputState *input, ENode *node);

boolean co_handle_geometry(BInputState *input, ENode *node)
{
	float y, color, color_light, pre_expander;
	static float rot_layer = 0, rot_crease = 0, rot_bone = 0;
	static boolean show_layer = TRUE, show_crease = TRUE, show_bone = TRUE;

	change_g_node_id = e_ns_get_node_id(node);

	y = co_handle_node_head(input, node);

	co_vng_divider(input, 0.2, y, &rot_layer, &color, &color_light, &show_layer, "Layers");

	pre_expander = y;
	if(rot_layer > 0.001)
	{
		EGeoLayer *layer;
		y -= 0.05;
		if(sw_text_button(input, -0.27, y, 0, SUI_T_SIZE, SUI_T_SPACE, "Create new Layer", color, color, color))
		{
			EGeoLayer *l;
			uint i;
			char nr[32];
			i = 0;
			for(l = e_nsg_get_layer_next(node, 0); l != NULL ; l = e_nsg_get_layer_next(node, e_nsg_get_layer_id(l) + 1))
				i++;
			sprintf(nr, "layer_%u", i);
			verse_send_g_layer_create(change_g_node_id, -1, nr, VN_G_LAYER_VERTEX_XYZ, 0, 0);
		}
		if(sw_text_button(input, 0, y, 0, SUI_T_SIZE, SUI_T_SPACE, "Vertex Color", color, color, color))
		{
			if(e_nsg_get_layer_by_name(node, "col_r") == NULL)
				verse_send_g_layer_create(change_g_node_id, -1, "col_r", VN_G_LAYER_VERTEX_REAL, 0, 0);
			if(e_nsg_get_layer_by_name(node, "col_g") == NULL)
				verse_send_g_layer_create(change_g_node_id, -2, "col_g", VN_G_LAYER_VERTEX_REAL, 0, 0);
			if(e_nsg_get_layer_by_name(node, "col_b") == NULL)
				verse_send_g_layer_create(change_g_node_id, -3, "col_b", VN_G_LAYER_VERTEX_REAL, 0, 0);
		}
		if(sw_text_button(input, 0.27, y, 0, SUI_T_SIZE, SUI_T_SPACE, "Texture Mapping", color, color, color))
		{
			if(e_nsg_get_layer_by_name(node, "map_u") == NULL)
				verse_send_g_layer_create(change_g_node_id, -1, "map_u", VN_G_LAYER_POLYGON_CORNER_REAL, 0, 0);
			if(e_nsg_get_layer_by_name(node, "map_v") == NULL)
				verse_send_g_layer_create(change_g_node_id, -2, "map_v", VN_G_LAYER_POLYGON_CORNER_REAL, 0, 0);
		}
		if(sw_text_button(input, 0.54, y, 0, SUI_T_SIZE, SUI_T_SPACE, "Vertex Select", color, color, color))
		{
			if(e_nsg_get_layer_by_name(node, "select") == NULL)
				verse_send_g_layer_create(change_g_node_id, -1, "select", VN_G_LAYER_VERTEX_REAL, 0, 0);
		}
		y -= 0.05;
		for(layer = e_nsg_get_layer_next(node, 0); layer != NULL ; layer = e_nsg_get_layer_next(node, e_nsg_get_layer_id(layer) + 1))
		{
			char *names[] = {"VERTEX_XYZ", "VERTEX_UINT32", "VERTEX_REAL", "POLYGON_CORNER_UINT32", "POLYGON_CORNER_REAL", "POLYGON_FACE_UINT8", "POLYGON_FACE_UINT32", "POLYGON_FACE_REAL"};
			static popup = -1;
			VNGLayerType type;

			sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Layer name:", color_light, color_light, color_light);  
			co_w_type_in(input, 0.15, y, 0.5, SUI_T_SIZE, e_nsg_get_layer_name(layer), 16, rename_g_layer_func, e_nsg_get_layer_name(layer), color, color_light);

			if(e_nsg_get_layer_id(layer) > 1 && co_w_close_button(input, 0.635, y, color, color, color))
				verse_send_g_layer_destroy(change_g_node_id, e_nsg_get_layer_id(layer));
			y -= 0.05;

			type = e_nsg_get_layer_type(layer);
			if(type > VN_G_LAYER_VERTEX_REAL)
				type -= VN_G_LAYER_POLYGON_CORNER_UINT32 - 1 - VN_G_LAYER_VERTEX_REAL;
			if(type > 7)
				type = 7;
			
			sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Layer type:", color_light, color_light, color_light);  
			if(sw_text_button(input, 0.15, y, 0, SUI_T_SIZE, SUI_T_SPACE, names[type], color, color, color))
				popup = e_nsg_get_layer_id(layer);

			if(popup == e_nsg_get_layer_id(layer))
			{
				SUIPUElement e[7];
				uint i, output;
				for(i = 0; i < 7; i++)
					e[i].text = names[i]; 
				for(i = 0; i < 3; i++)
				{
					e[i].type = PU_T_ANGLE; 
					e[i].data.angle[0] = 45 + (float)i * 30;
					e[i].data.angle[1] = 45 + (float)(i + 1) * 30;
				}
				for(; i < 7; i++)
				{
					e[i].type = PU_T_ANGLE; 
					e[i].data.angle[0] = 180 + 45 + (float)(i - 3) * 22.5;
					e[i].data.angle[1] = 180 + 45 + (float)(i - 2) * 22.5;
				}
				output = sui_draw_popup(input, 0.15, y, e, 7, 0);
				if(output != -1 && output != type)
				{
					if(output > 3)
						output -=  VN_G_LAYER_POLYGON_CORNER_UINT32 - VN_G_LAYER_VERTEX_REAL - 1; 
					verse_send_g_layer_create(e_ns_get_node_id(node), popup, e_nsg_get_layer_name(layer), output, 0, 0);
				}
			}
			if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == FALSE)
				popup = -1;
			y -= 0.05;
			sui_draw_rounded_square(-0.3, y + 0.125, 1, -0.09, color_light, color_light, color_light);
		}
	}
	glPopMatrix();
	y -= 0.05 + (y - pre_expander) * (1 - rot_layer);

	co_vng_divider(input, 0.2, y, &rot_crease, &color, &color_light, &show_crease, "Crease");
	pre_expander = y;
	if(rot_crease > 0.001)
	{
		uint bone;
		y -= 0.05;
		sui_draw_text(-0.27, y, SUI_T_SIZE, SUI_T_SPACE, "vertex crease", color_light, color_light, color_light); 
		sui_draw_text(0.0, y - 0.05, SUI_T_SIZE, SUI_T_SPACE, "Vertex", color_light, color_light, color_light);  
/*		if(sui_type_number_double(input, 0.15, y - 0.05, 0.15, SUI_T_SIZE, &transform[0], &transform[0], color, color, color))
			verse_send_o_transform_pos_real64(e_ns_get_node_id(node), 0, transform, NULL, NULL, NULL, 0);
		if(co_w_slider(input, 0.3, y - 0.05, 0.35, &transform[0], color, color, color))
			verse_send_o_transform_pos_real64(e_ns_get_node_id(node), 0, transform, NULL, NULL, NULL, 0);
*/
	}
	glPopMatrix();
	y -= 0.05 + (y - pre_expander) * (1 - rot_layer);

	co_vng_divider(input, 0.2, y, &rot_bone, &color, &color_light, &show_bone, "Bones");
	pre_expander = y;
	if(rot_bone > 0.001)
	{
		uint16 bone;
		y -= 0.05;
		if(sw_text_button(input, -0.27, y, 0, SUI_T_SIZE, SUI_T_SPACE, "Create new Bone", color, color, color))
		{
			VNQuat64	identity = { 0.0, 0.0, 0.0, 1.0 };
			uint i;
			char nr[32];
			i = 3;
			for(bone = e_nsg_get_bone_next(node, 0); bone != (uint16)-1; bone = e_nsg_get_bone_next(node, bone + 1))
				i++;
			sprintf(nr, "weight_%u", i);
			verse_send_g_bone_create(change_g_node_id, -1, nr, "reference", 0, 0, 0, 0, &identity);
		}
		y -= 0.05;
		for(bone = e_nsg_get_bone_next(node, 0); bone != (uint16)-1; bone = e_nsg_get_bone_next(node, bone + 1))
		{
			double t[3];
			VNQuat64	rot;
			char *ref, *text[] = {"X", "Y", "Z", "X", "Y", "Z", "W"};
			uint i;
			uint32 parent;

			sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Bone Weight:", color_light, color_light, color_light);
			co_w_type_in(input, 0.15, y, 0.5, SUI_T_SIZE, e_nsg_get_bone_weight(node, bone), 16, rename_g_layer_func, e_nsg_get_bone_weight(node, bone), color, color_light);

			if(co_w_close_button(input, 0.645, y, color, color, color))
				verse_send_g_bone_destroy(change_g_node_id, bone);
			y -= 0.05;
			ref = e_nsg_get_bone_reference(node, bone);
			sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Bone Ref:", color_light, color_light, color_light);  
			co_w_type_in(input, 0.15, y, 0.5, SUI_T_SIZE, e_nsg_get_bone_reference(node, bone), 16, rename_g_layer_func, e_nsg_get_bone_reference(node, bone), color, color_light);
			y -= 0.05;
			e_nsg_get_bone_pos64(node, bone, t);
			e_nsg_get_bone_rot64(node, bone, &rot);
			parent = e_nsg_get_bone_parent(node, bone);
			sui_draw_text(-0.25, y, SUI_T_SIZE, SUI_T_SPACE, "PARENT", color_light, color_light, color_light);
			if(sui_type_number_uint(input,0.15, y, 0.15, SUI_T_SIZE, &parent, e_nsg_get_bone_weight(node, bone), color, color, color))
				verse_send_g_bone_create(change_g_node_id, bone, e_nsg_get_bone_weight(node, bone), ref, parent, t[0], t[1], t[2], &rot);
			y -= 0.05;
			sui_draw_text(-0.25, y, SUI_T_SIZE, SUI_T_SPACE, "POSITION", color_light, color_light, color_light);
			sui_draw_text(-0.25, y - 0.15, SUI_T_SIZE, SUI_T_SPACE, "ROTATION", color_light, color_light, color_light);

			for(i = 0; i < 7; i++)
			{	
				double	*edit;

				/* Hack: since the code assumes position and rotation are in a sequence of 7 reals (3 for pos, 4 for rot),
				 * but they no longer are since the introduction of quaternions, we use the variable 'edit' to point at
				 * one of the reals, and the code below to "pick apart" the quaternion as needed. How pretty.
				*/
				if(i < 3)
					edit = t + i;
				else
				{
					switch(i)
					{
					case 3:	edit = &rot.x;	break;
					case 4:	edit = &rot.y;	break;
					case 5:	edit = &rot.z;	break;
					case 6:	edit = &rot.w;	break;
					}
				}
				sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, text[i], color_light, color_light, color_light);  
				if(sui_type_number_double(input, 0.15, y, 0.15, SUI_T_SIZE, edit, edit, color, color, color))
					verse_send_g_bone_create(change_g_node_id, bone, e_nsg_get_bone_weight(node, bone), ref, parent, t[0], t[1], t[2], &rot);
				if(co_w_slider(input, 0.3, y, 0.35, edit, color, color, color))
					/*verse_send_g_bone_create(change_g_node_id, bone, e_nsg_get_bone_weight(node, bone), ref, parent, t[0], t[1], t[2], &rot)*/;
				y -= 0.05;
			}
			sui_draw_rounded_square(-0.3, y + 0.525, 1, -0.49, color_light, color_light, color_light);
		}
	}
	glPopMatrix();
	y -= 0.05 + (y - pre_expander) * (1 - rot_layer);
	return co_handle_return(input);
}
