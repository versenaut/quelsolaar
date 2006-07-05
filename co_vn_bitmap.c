#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"
#include "seduce.h"
#include "persuade.h"
#include "co_vn_graphics.h"
#include "co_vn_handle.h"
#include "co_widgets.h"

extern boolean co_handle_head(BInputState *input, ENode *node, float *length);

void co_draw_bitmap(ENode *node)
{
#ifdef PERSUADE_H
	float vertex[8] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
	float uv[8] = {1, 1, 0, 1, 0, 0, 1, 0};
	PTextureHandle *h;
	h = p_th_create_texture_handle(e_ns_get_node_id(node), "color_r", "color_g", "color_b");
	sui_set_texture2D_array_gl(uv, 4, 2, p_th_get_texture_id(h));
	sui_draw_gl(GL_QUADS, vertex, 4, 2, 1, 1, 1, 1);
	p_th_destroy_texture_handle(h);
#endif
}
uint change_b_node_id = 0;

void rename_b_layer_func(void *user, char *text)
{
	EBitLayer *layer;
	ENode *node;
	if((node = e_ns_get_node(0, change_b_node_id)) != NULL)
	{
		for(layer = e_nsb_get_layer_next(node, 0); layer != NULL && layer != user ; layer = e_nsb_get_layer_next(node, e_nsb_get_layer_id(layer) + 1));
		if(layer == user)
			verse_send_b_layer_create(change_b_node_id, e_nsb_get_layer_id(layer), text, e_nsb_get_layer_type(layer));
	}
}

extern float co_handle_node_head(BInputState *input, ENode *node, boolean reset);

boolean co_handle_bitmap(BInputState *input, ENode *node)
{
	float y, color, color_light, pre_expander;
	static float rot_layer = 0, rot_size = 0;
	static boolean show_layer = TRUE, show_size = TRUE;
	static uint create_size[3] = {0, 0, 0}, s[3];
	if(change_b_node_id != e_ns_get_node_id(node))
	{
		create_size[0] = 0;
		create_size[1] = 0;
		create_size[2] = 0;
	}
	e_nsb_get_size(node, &s[0], &s[1], &s[2]);
	if(s[0] * s[1] * s[2] != 0)
		e_nsb_get_size(node, &create_size[0], &create_size[1], &create_size[2]);

	y = co_handle_node_head(input, node, change_b_node_id != e_ns_get_node_id(node));
	change_b_node_id = e_ns_get_node_id(node);

	co_vng_divider(input, 0.2, y, &rot_size, &color, &color_light, &show_size, "Size");
	pre_expander = y;
	if(rot_size > 0.001)
	{	
		y -= 0.05;
		sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Width:", color_light, color_light, color_light);  
		if(sui_type_number_uint(input, 0.15, y, 0, 0.5, SUI_T_SIZE, &create_size[0], &create_size[0], color, color, color) && create_size[0] * create_size[1] * create_size[2] != 0)
			verse_send_b_dimensions_set(change_b_node_id, create_size[0], create_size[1], create_size[2]);
		y -= 0.05;
		sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Height:", color_light, color_light, color_light);  
		if(sui_type_number_uint(input, 0.15, y, 0, 0.5, SUI_T_SIZE, &create_size[1], &create_size[1], color, color, color) && create_size[0] * create_size[1] * create_size[2] != 0)
			verse_send_b_dimensions_set(change_b_node_id, create_size[0], create_size[1], create_size[2]);
		y -= 0.05;
		sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Depth:", color_light, color_light, color_light);  
		if(sui_type_number_uint(input, 0.15, y, 0, 0.5, SUI_T_SIZE, &create_size[2], &create_size[2], color, color, color) && create_size[0] * create_size[1] * create_size[2] != 0)
			verse_send_b_dimensions_set(change_b_node_id, create_size[0], create_size[1], create_size[2]);	
	}
	glPopMatrix(); 
	y -= 0.05 + (y - pre_expander) * (1 - rot_size);

	co_vng_divider(input, 0.2, y, &rot_layer, &color, &color_light, &show_layer, "Layers");
	pre_expander = y;
	if(rot_layer > 0.001)
	{
		EBitLayer *layer;
		y -= 0.05;
		if(sw_text_button(input, -0.27, y, 0, SUI_T_SIZE, SUI_T_SPACE, "Create new layer", color, color, color))
		{
			EBitLayer *l;
			uint i;
			char nr[32];
			i = 0;
			for(l = e_nsb_get_layer_next(node, 0); l != NULL ; l = e_nsb_get_layer_next(node, e_nsb_get_layer_id(l) + 1))
				i++;
			sprintf(nr, "layer_%u", i);
			verse_send_b_layer_create(change_b_node_id, -1, nr, VN_B_LAYER_UINT8);
		}
		if(sw_text_button(input, 0, y, 0, SUI_T_SIZE, SUI_T_SPACE, "RGB 8bit", color, color, color))
		{
			if(e_nsb_get_layer_by_name(node, "color_r") == NULL)
				verse_send_b_layer_create(change_b_node_id, -1, "color_r", VN_B_LAYER_UINT8);
			if(e_nsb_get_layer_by_name(node, "color_g") == NULL)
				verse_send_b_layer_create(change_b_node_id, -2, "color_g", VN_B_LAYER_UINT8);
			if(e_nsb_get_layer_by_name(node, "color_b") == NULL)
				verse_send_b_layer_create(change_b_node_id, -3, "color_b", VN_B_LAYER_UINT8);
		}
		if(sw_text_button(input, 0.27, y, 0, SUI_T_SIZE, SUI_T_SPACE, "RGB 16bit", color, color, color))
		{
			if(e_nsb_get_layer_by_name(node, "color_r") == NULL)
				verse_send_b_layer_create(change_b_node_id, -1, "color_r", VN_B_LAYER_UINT16);
			if(e_nsb_get_layer_by_name(node, "color_g") == NULL)
				verse_send_b_layer_create(change_b_node_id, -2, "color_g", VN_B_LAYER_UINT16);
			if(e_nsb_get_layer_by_name(node, "color_b") == NULL)
				verse_send_b_layer_create(change_b_node_id, -3, "color_b", VN_B_LAYER_UINT16);
		}
		if(sw_text_button(input, 0.54, y, 0, SUI_T_SIZE, SUI_T_SPACE, "RGB 32bit", color, color, color))
		{
			if(e_nsb_get_layer_by_name(node, "color_r") == NULL)
				verse_send_b_layer_create(change_b_node_id, -1, "color_r", VN_B_LAYER_REAL32);
			if(e_nsb_get_layer_by_name(node, "color_g") == NULL)
				verse_send_b_layer_create(change_b_node_id, -2, "color_g", VN_B_LAYER_REAL32);
			if(e_nsb_get_layer_by_name(node, "color_b") == NULL)
				verse_send_b_layer_create(change_b_node_id, -3, "color_b", VN_B_LAYER_REAL32);
		}
		y -= 0.05;
		for(layer = e_nsb_get_layer_next(node, 0); layer != NULL ; layer = e_nsb_get_layer_next(node, e_nsb_get_layer_id(layer) + 1))
		{
			char *names[] = {"UINT1", "UINT8", "UINT16", "REAL32", "REAL64"};
			static uint32 popup = ~0;
			VNBLayerType type;

			sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Layer name:", color_light, color_light, color_light);  
			co_w_type_in(input, 0.15, y, 0.5, SUI_T_SIZE, e_nsb_get_layer_name(layer), 16, rename_b_layer_func, layer, color, color_light);

			if(co_w_close_button(input, 0.635, y, color, color, color))
				verse_send_b_layer_destroy(change_b_node_id, e_nsb_get_layer_id(layer));
			y -= 0.05;

			type = e_nsb_get_layer_type(layer);
			
			sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Layer type:", color_light, color_light, color_light);  
			if(sw_text_button(input, 0.15, y, 0, SUI_T_SIZE, SUI_T_SPACE, names[type], color, color, color))
				popup = e_nsb_get_layer_id(layer);

			if(popup == e_nsb_get_layer_id(layer))
			{
				SUIPUElement e[5];
				uint i, output;
				for(i = 0; i < 5; i++)
					e[i].text = names[i]; 
				for(i = 0; i < 3; i++)
				{
					e[i].type = PU_T_ANGLE; 
					e[i].data.angle[0] = 45 + (float)i * 30;
					e[i].data.angle[1] = 45 + (float)(i + 1) * 30;
				}
				for(; i < 5; i++)
				{
					e[i].type = PU_T_ANGLE; 
					e[i].data.angle[0] = 180 + 45 + (float)(i - 3) * 45;
					e[i].data.angle[1] = 180 + 45 + (float)(i - 2) * 45;
				}
				output = sui_draw_popup(input, 0.15, y, e, 5, 0, 0.9);
				if(output != -1 && output != type)
				{
					verse_send_b_layer_create(e_ns_get_node_id(node), popup, e_nsb_get_layer_name(layer), output);
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
	return co_handle_return(input);
}
