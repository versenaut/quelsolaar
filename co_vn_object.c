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

uint32 rename_o_node_id = 0;

double new_transform[10];
double new_light[3];

void rename_method_group_func(void *user, char *text)
{
	uint16 group_id;
	ENode *node;
	if((node = e_ns_get_node(0, rename_o_node_id)) != NULL)
	{
		for(group_id = e_nso_get_next_method_group(node, 0); group_id != (uint16)-1 && user != e_nso_get_method_group(node, group_id); group_id = e_nso_get_next_method_group(node, group_id + 1));
		if(group_id != (uint16)-1)
		{
			verse_send_o_method_group_create(rename_o_node_id, group_id, text);
			return;
		}
	}
}

void rename_method_func(void *user, char *text)
{
	uint i, j;
	ENode *node;
	if((node = e_ns_get_node(0, rename_o_node_id)) != NULL)
	{
		for(i = e_nso_get_next_method_group(node, 0); i != (uint16)-1 ; i = e_nso_get_next_method_group(node, i + 1))
		{
			for(j = e_nso_get_next_method(node, i, 0); j != (uint16)-1 && user != e_nso_get_method(node, i, j); j = e_nso_get_next_method(node, i, j + 1))
			if(j != -1)
			{
				verse_send_o_method_create(rename_o_node_id, i, j, text, e_nso_get_method_param_count(node, i, j), e_nso_get_method_param_types(node, i, j), e_nso_get_method_param_names(node, i, j));
				return;
			}
		}
	}
}

void rename_param_func(void *user, char *text)
{
	uint i, j, k, count;
	ENode *node;
	char **names, *temp_name;
	VNOParamType *types;
	if((node = e_ns_get_node(0, rename_o_node_id)) != NULL)
	{
		for(i = e_nso_get_next_method_group(node, 0); i != (uint16)-1 ; i = e_nso_get_next_method_group(node, i + 1))
		{
			for(j = e_nso_get_next_method(node, i, 0); j != (uint16)-1; j = e_nso_get_next_method(node, i, j + 1))
			{
				names = e_nso_get_method_param_names(node, i, j);
				types = e_nso_get_method_param_types(node, i, j);
				count = e_nso_get_method_param_count(node, i, j);
				for(k = 0; k < count && names[k] != user; k++);

				if(k != count)
				{
					temp_name = names[k];
					names[k] = text;
					verse_send_o_method_create(rename_o_node_id, i, j, e_nso_get_method(node, i, j), count, types, names);
					names[k] = temp_name;
					return;
				}
			}
		}
	}
}

void rename_link_func(void *user, char *text)
{

	ENode *node;
	EObjLink *link;
	if((node = e_ns_get_node(0, rename_o_node_id)) != NULL)
	{
		EObjLink *link;
		for(link = e_nso_get_next_link(node, 0); link != NULL && link != user; link = e_nso_get_next_link(node, e_nso_get_link_id(link) + 1));
		if(link != NULL)
			verse_send_o_link_set(rename_o_node_id, e_nso_get_link_id(link), e_nso_get_link_node(link), text, e_nso_get_link_target_id(link));
	}
}

extern float co_handle_node_head(BInputState *input, ENode *node);

boolean co_handle_object(BInputState *input, ENode *node)
{
	float y, color, color_light, y_meth, y_group, pre_expander;
	static float rot_meth = 0, rot_light = 0, rot_trans = 0, rot_link = 0;
	static boolean show_meth = TRUE, show_light = TRUE, show_trans = TRUE, show_link = TRUE;
	static double light[3], transform[10], data = 0;
	uint16 m_group, method;
	char nr[64];
	uint i, count;
	y = co_handle_node_head(input, node);
	rename_o_node_id = e_ns_get_node_id(node);

	co_vng_divider(input, 0.2, y, &rot_light, &color, &color_light, &show_light, "LIGHT");

	if(rot_light > 0.001)
	{
		e_nso_get_light(node, light);

		sui_draw_text(0.0, y - 0.05, SUI_T_SIZE, SUI_T_SPACE, "Red:", color_light, color_light, color_light);  
		if(sui_type_number_double(input, 0.15, y - 0.05, 0.15, SUI_T_SIZE, &light[0], &light[0], color, color, color))
			verse_send_o_light_set(e_ns_get_node_id(node), light[0], light[1], light[2]);
		if(co_w_slider(input, 0.3, y - 0.05, 0.35, &light[0], color, color, color))
			verse_send_o_light_set(e_ns_get_node_id(node), light[0], light[1], light[2]);

		sui_draw_text(0.0, y - 0.10, SUI_T_SIZE, SUI_T_SPACE, "Green:", color_light, color_light, color_light);  
		if(sui_type_number_double(input, 0.15, y - 0.1, 0.15, SUI_T_SIZE, &light[1], &light[1], color, color, color))
			verse_send_o_light_set(e_ns_get_node_id(node), light[0], light[1], light[2]);
		if(co_w_slider(input, 0.3, y - 0.10, 0.35, &light[1], color, color, color))
			verse_send_o_light_set(e_ns_get_node_id(node), light[0], light[1], light[2]);

		sui_draw_text(0.0, y - 0.15, SUI_T_SIZE, SUI_T_SPACE, "Blue:", color_light, color_light, color_light);  
		if(sui_type_number_double(input, 0.15, y - 0.15, 0.15, SUI_T_SIZE, &light[2], &light[2], color, color, color))
			verse_send_o_light_set(e_ns_get_node_id(node), light[0], light[1], light[2]);
		if(co_w_slider(input, 0.3, y - 0.15, 0.35, &light[2], color, color, color))
			verse_send_o_light_set(e_ns_get_node_id(node), light[0], light[1], light[2]);
	}
	glPopMatrix();
	y -= 0.05 + 0.15 * rot_light;
	co_vng_divider(input, 0.2, y, &rot_meth, &color, &color_light, &show_meth, "METHODS");
	pre_expander = y;

	if(rot_meth > 0.001)
	{
		if(sw_text_button(input, -0.3, y - 0.05, 0, SUI_T_SIZE, SUI_T_SPACE, "Create new Method group", color, color, color))
		{
			i = 1;
			for(m_group = e_nso_get_next_method_group(node, 0); m_group != (uint16)-1 ; m_group = e_nso_get_next_method_group(node, m_group + 1))
				i++;
			sprintf(nr, "group_%u", i);
			verse_send_o_method_group_create(e_ns_get_node_id(node), ~0, nr);
		}
		if(NULL == e_nso_get_method_group(node, 0))
		{
			sui_draw_text(-0.3, y - 0.1, SUI_T_SIZE, SUI_T_SPACE, "NO METHOD GROUPS", color_light, color_light, color_light);
			y -= 0.05;
		}

		y -= 0.05;
		for(m_group = e_nso_get_next_method_group(node, 0); m_group != (uint16)-1 ; m_group = e_nso_get_next_method_group(node, m_group + 1))
		{
			y_group = y - 0.05;
			sui_draw_text(0.0, y_group, SUI_T_SIZE, SUI_T_SPACE, "Group name:", color_light, color_light, color_light);  
			co_w_type_in(input, 0.15, y_group, 0.5, SUI_T_SIZE, e_nso_get_method_group(node, m_group), 16, rename_method_group_func, e_nso_get_method_group(node, m_group), color, color_light);
			if(co_w_close_button(input, 0.635, y_group, color, color, color))
				verse_send_o_method_group_destroy(e_ns_get_node_id(node), m_group);
			if(sw_text_button(input, -0.27, y_group, 0, SUI_T_SIZE, SUI_T_SPACE, "Create new method", color, color, color))
			{
				i = 1;
				for(method = e_nso_get_next_method(node, m_group, 0); method != (uint16)-1; method = e_nso_get_next_method(node, m_group, method + 1))
					i++;
				sprintf(nr, "method_%u", i);
				verse_send_o_method_create(e_ns_get_node_id(node), m_group, -1, nr, 0, NULL, NULL);
			}
			y_meth = y_group;
			for(method = e_nso_get_next_method(node, m_group, 0); method != (uint16)-1; method = e_nso_get_next_method(node, m_group, method + 1))
			{
				char **names;
				VNOParamType *types;
				y_meth = y_group;
				y_meth -= 0.05;
				sui_draw_text(0.0, y_meth, SUI_T_SIZE, SUI_T_SPACE, "Method name:", color_light, color_light, color_light);  
				co_w_type_in(input, 0.15, y_meth, 0.5, SUI_T_SIZE, e_nso_get_method(node, m_group, method), 16, NULL, e_nso_get_method(node, m_group, method), color, color_light);
				if(co_w_close_button(input, 0.635, y_meth, color, color, color))
					verse_send_o_method_destroy(e_ns_get_node_id(node), m_group, method);

				names = e_nso_get_method_param_names(node, m_group, method);
				types = e_nso_get_method_param_types(node, m_group, method);
				count = e_nso_get_method_param_count(node, m_group, method);
			
				if(count < 255 && sw_text_button(input, -0.27, y_meth, 0, SUI_T_SIZE, SUI_T_SPACE, "Add parameter", color, color, color))
				{
					char *new_names[256];
					VNOParamType new_types[256];
					for(i = 0; i < count; i++)
					{
						new_names[i] = names[i];
						new_types[i] = types[i];
					}
					new_names[i] = "param";
					new_types[i] = VN_O_METHOD_PTYPE_INT32;
					verse_send_o_method_create(e_ns_get_node_id(node), m_group, method, e_nso_get_method(node, m_group, method), count + 1, new_types, new_names);
				}			
				for(i = 0; i < count; i++)
				{
					char *type_names[] = {"INT8", "INT16", "INT32", "UINT8", "UINT16", "UINT32", "REAL32", "REAL64", "REAL32_VEC2", "REAL32_VEC3", "REAL32_VEC4", "REAL64_VEC2", "REAL64_VEC3", "REAL64_VEC4", "REAL32_MAT4", "REAL32_MAT9", "REAL32_MAT16", "REAL64_MAT4", "REAL64_MAT9", "REAL64_MAT16", "STRING", "NODE", "LAYER"};
					static uint pu_g = -1, pu_m = -1, pu_p = 0;
					y_meth -= 0.05;
					sui_draw_2d_line_gl(0.05, y_meth + 0.03, 0.65, y_meth + 0.03, color_light, color_light, color_light);
					sui_draw_text(0.0, y_meth, SUI_T_SIZE, SUI_T_SPACE, "Param:", color_light, color_light, color_light);

					if(co_w_close_button(input, 0.635, y_meth, color, color, color))
					{
						char *new_names[256];
						VNOParamType new_types[256];
						uint j, k;
						k = 0;
						for(j = 0; j < count; j++)
						{
							if(j != i)
							{
								new_names[k] = names[j];
								new_types[k++] = types[j];
							}
						}
						verse_send_o_method_create(e_ns_get_node_id(node), m_group, method, e_nso_get_method(node, m_group, method), count - 1, new_types, new_names);

					}

					co_w_type_in(input, 0.15, y_meth, 0.5, SUI_T_SIZE, names[i], 16, rename_param_func, names[i], color, color_light);

					y_meth -= 0.05;
					sui_draw_text(0.0, y_meth, SUI_T_SIZE, SUI_T_SPACE, "Type:", color_light, color_light, color_light);
					if(sw_text_button(input, 0.15, y_meth, 0, SUI_T_SIZE, SUI_T_SPACE, type_names[types[i]], color, color, color))
					{
						pu_g = m_group;
						pu_m = method;
						pu_p = i;
					}
					if(pu_g == m_group && pu_m == method && pu_p == i)
					{
						SUIPUElement element[24];
						uint j, output;
						for(j = 0; j < 23; j++)
							element[j].text = type_names[j];
						for(j = 0; j <= VN_O_METHOD_PTYPE_UINT32; j++)
							element[j].type = PU_T_TOP;
						for(; j <= VN_O_METHOD_PTYPE_REAL64_VEC4; j++)
							element[j].type = PU_T_BOTTOM;
						for(; j <= VN_O_METHOD_PTYPE_REAL64_MAT16; j++)
						{
							element[j].type = PU_T_SQUARE;
							element[j].data.square.draw_func = NULL;
							element[j].data.square.square[0] = 0.2 + (float)((j - VN_O_METHOD_PTYPE_REAL64_VEC4 - 1) / 3) * 0.25;
							element[j].data.square.square[1] = 0.15 * ((float)((j - VN_O_METHOD_PTYPE_REAL64_VEC4 - 1) % 3) - 0.66666);
							element[j].data.square.square[2] = 0.2;
							element[j].data.square.square[3] = -0.1;
						}
						for(; j <= VN_O_METHOD_PTYPE_LAYER; j++)
						{
							element[j].type = PU_T_SQUARE;
							element[j].data.square.draw_func = NULL;
							element[j].data.square.square[0] = -0.4;
							element[j].data.square.square[1] = 0.15 * ((float)j - 1.66666 - VN_O_METHOD_PTYPE_REAL64_MAT16);
							element[j].data.square.square[2] = 0.2;
							element[j].data.square.square[3] = -0.1;
						}

						output = sui_draw_popup(input, 0.15, y_meth, element, VN_O_METHOD_PTYPE_LAYER + 1, 0);
						if(output <= VN_O_METHOD_PTYPE_REAL64_MAT16)
						{
							VNOParamType temp;
							temp = types[pu_p];
							types[pu_p] = output;
							verse_send_o_method_create(e_ns_get_node_id(node), m_group, method, e_nso_get_method(node, m_group, method), count, types, names);
							types[pu_p] = temp;
							pu_g = -1;
						}
						if(input->mode == BAM_EVENT && input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE)
							pu_g = -1;
					}
				}
				sui_draw_rounded_square(-0.01, y_group - 0.025, 0.7, y_meth - y_group, color_light, color_light, color_light);
				y_group = y_meth - 0.025;
			}
			sui_draw_rounded_square(-0.3, y - 0.025, 1, y_meth - y - 0.01, color_light, color_light, color_light);
			y = y_meth - 0.025;
			
		}
	}
	glPopMatrix();

	y -= 0.05 + (y - pre_expander) * (1 - rot_meth);
	co_vng_divider(input, 0.2, y, &rot_trans, &color, &color_light, &show_trans, "TRANSFORM");

	if(rot_trans > 0.001)
	{
		uint32 seconds, fractions;
		verse_session_get_time(&seconds, &fractions);
		e_nso_get_pos(node, transform, NULL, NULL, NULL, NULL, NULL);

		sui_draw_text(-0.3, y - 0.05, SUI_T_SIZE, SUI_T_SPACE, "POSITION", color_light, color_light, color_light);

		sui_draw_text(0.0, y - 0.05, SUI_T_SIZE, SUI_T_SPACE, "X:", color_light, color_light, color_light);  
		if(sui_type_number_double(input, 0.15, y - 0.05, 0.15, SUI_T_SIZE, &transform[0], &transform[0], color, color, color))
			verse_send_o_transform_pos_real64(e_ns_get_node_id(node), seconds, fractions, transform, NULL, NULL, NULL, 0);
		if(co_w_slider(input, 0.3, y - 0.05, 0.35, &transform[0], color, color, color))
			verse_send_o_transform_pos_real64(e_ns_get_node_id(node), seconds, fractions, transform, NULL, NULL, NULL, 0);

		sui_draw_text(0.0, y - 0.10, SUI_T_SIZE, SUI_T_SPACE, "Y:", color_light, color_light, color_light);  
		if(sui_type_number_double(input, 0.15, y - 0.1, 0.15, SUI_T_SIZE, &transform[1], &transform[1], color, color, color))
			verse_send_o_transform_pos_real64(e_ns_get_node_id(node), seconds, fractions, transform, NULL, NULL, NULL, 0);		
		if(co_w_slider(input, 0.3, y - 0.10, 0.35, &transform[1], color, color, color))
			verse_send_o_transform_pos_real64(e_ns_get_node_id(node), seconds, fractions, transform, NULL, NULL, NULL, 0);

		sui_draw_text(0.0, y - 0.15, SUI_T_SIZE, SUI_T_SPACE, "Z:", color_light, color_light, color_light);  
		if(sui_type_number_double(input, 0.15, y - 0.15, 0.15, SUI_T_SIZE, &transform[2], &transform[2], color, color, color))
			verse_send_o_transform_pos_real64(e_ns_get_node_id(node), seconds, fractions, transform, NULL, NULL, NULL, 0);
		if(co_w_slider(input, 0.3, y - 0.15, 0.35, &transform[2], color, color, color))
			verse_send_o_transform_pos_real64(e_ns_get_node_id(node), seconds, fractions, transform, NULL, NULL, NULL, 0);

		sui_draw_2d_line_gl(-0.25, y - 0.20, 0.65, y - 0.20, color_light, color_light, color_light);

		e_nso_get_rot(node, &transform[3], NULL, NULL, NULL, NULL, NULL);					
		sui_draw_text(-0.3, y - 0.25, SUI_T_SIZE, SUI_T_SPACE, "ROTATE", color_light, color_light, color_light);

		sui_draw_text(0.0, y - 0.25, SUI_T_SIZE, SUI_T_SPACE, "S:", color_light, color_light, color_light);  
		if(sui_type_number_double(input, 0.15, y - 0.25, 0.15, SUI_T_SIZE, &transform[3], &transform[3], color, color, color))
			verse_send_o_transform_rot_real64(e_ns_get_node_id(node), seconds, fractions, &transform[3], NULL, NULL, NULL, 0);
		if(co_w_slider(input, 0.3, y - 0.25, 0.35, &transform[3], color, color, color))
			verse_send_o_transform_rot_real64(e_ns_get_node_id(node), seconds, fractions, &transform[3], NULL, NULL, NULL, 0);

		sui_draw_text(0.0, y - 0.30, SUI_T_SIZE, SUI_T_SPACE, "T:", color_light, color_light, color_light);  
		if(sui_type_number_double(input, 0.15, y - 0.30, 0.15, SUI_T_SIZE, &transform[4], &transform[4], color, color, color))
			verse_send_o_transform_rot_real64(e_ns_get_node_id(node), seconds, fractions, &transform[3], NULL, NULL, NULL, 0);		
		if(co_w_slider(input, 0.3, y - 0.30, 0.35, &transform[4], color, color, color))
			verse_send_o_transform_rot_real64(e_ns_get_node_id(node), seconds, fractions, &transform[3], NULL, NULL, NULL, 0);

		sui_draw_text(0.0, y - 0.35, SUI_T_SIZE, SUI_T_SPACE, "U:", color_light, color_light, color_light);  
		if(sui_type_number_double(input, 0.15, y - 0.35, 0.15, SUI_T_SIZE, &transform[5], &transform[5], color, color, color))
			verse_send_o_transform_rot_real64(e_ns_get_node_id(node), seconds, fractions, &transform[3], NULL, NULL, NULL, 0);
		if(co_w_slider(input, 0.3, y - 0.35, 0.35, &transform[5], color, color, color))
			verse_send_o_transform_rot_real64(e_ns_get_node_id(node), seconds, fractions, &transform[3], NULL, NULL, NULL, 0);

		sui_draw_text(0.0, y - 0.40, SUI_T_SIZE, SUI_T_SPACE, "V:", color_light, color_light, color_light);  
		if(sui_type_number_double(input, 0.15, y - 0.40, 0.15, SUI_T_SIZE, &transform[6], &transform[6], color, color, color))
			verse_send_o_transform_rot_real64(e_ns_get_node_id(node), seconds, fractions, &transform[3], NULL, NULL, NULL, 0);
		if(co_w_slider(input, 0.3, y - 0.40, 0.35, &transform[6], color, color, color))
			verse_send_o_transform_rot_real64(e_ns_get_node_id(node), seconds, fractions, &transform[3], NULL, NULL, NULL, 0);

		sui_draw_2d_line_gl(-0.25, y - 0.45, 0.65, y - 0.45, color_light, color_light, color_light);

		e_nso_get_scale(node, &transform[7]);

		sui_draw_text(-0.3, y - 0.50, SUI_T_SIZE, SUI_T_SPACE, "SCALE", color_light, color_light, color_light);

		sui_draw_text(0.0, y - 0.50, SUI_T_SIZE, SUI_T_SPACE, "X:", color_light, color_light, color_light);  
		if(sui_type_number_double(input, 0.15, y - 0.50, 0.15, SUI_T_SIZE, &transform[7], &transform[7], color, color, color))
			verse_send_o_transform_scale_real64(e_ns_get_node_id(node), transform[7], transform[8], transform[9]);
		if(co_w_slider(input, 0.3, y - 0.50, 0.35, &transform[7], color, color, color))
			verse_send_o_transform_scale_real64(e_ns_get_node_id(node), transform[7], transform[8], transform[9]);

		sui_draw_text(0.0, y - 0.55, SUI_T_SIZE, SUI_T_SPACE, "Y:", color_light, color_light, color_light);  
		if(sui_type_number_double(input, 0.15, y - 0.55, 0.15, SUI_T_SIZE, &transform[8], &transform[8], color, color, color))
			verse_send_o_transform_scale_real64(e_ns_get_node_id(node), transform[7], transform[8], transform[9]);		
		if(co_w_slider(input, 0.3, y - 0.55, 0.35, &transform[8], color, color, color))
			verse_send_o_transform_scale_real64(e_ns_get_node_id(node), transform[7], transform[8], transform[9]);

		sui_draw_text(0.0, y - 0.60, SUI_T_SIZE, SUI_T_SPACE, "Z:", color_light, color_light, color_light);  
		if(sui_type_number_double(input, 0.15, y - 0.60, 0.15, SUI_T_SIZE, &transform[9], &transform[9], color, color, color))
			verse_send_o_transform_scale_real64(e_ns_get_node_id(node), transform[7], transform[8], transform[9]);
		if(co_w_slider(input, 0.3, y - 0.60, 0.35, &transform[9], color, color, color))
			verse_send_o_transform_scale_real64(e_ns_get_node_id(node), transform[7], transform[8], transform[9]);
	}
	glPopMatrix();
	y -= 0.05 + 0.6 * rot_trans;
	co_vng_divider(input, 0.2, y, &rot_link, &color, &color_light, &show_link, "LINKS");
	y -= 0.05;	
	if(rot_link > 0.001)
	{
		EObjLink *link;
		if(sw_text_button(input, -0.27, y, 0, SUI_T_SIZE, SUI_T_SPACE, "Create new link", color, color, color))
		{
			verse_send_o_link_set(rename_o_node_id, -1, -1, 0, "new_link");
		}
		y -= 0.05;
		for(link = e_nso_get_next_link(node, 0); link != NULL; link = e_nso_get_next_link(node, e_nso_get_link_id(link) + 1))
		{
			ENode *l_node;
			uint32 target;
			sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Link name:", color_light, color_light, color_light);  
			co_w_type_in(input, 0.15, y_meth, 0.5, SUI_T_SIZE, e_nso_get_link_name(link), 16, rename_link_func, e_nso_get_link_name(link), color, color_light);
			y -= 0.05;

			sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Node ID:", color_light, color_light, color_light);  
			target = e_nso_get_link_node(link);
			if(sui_type_number_uint(input, 0.15, y, 0.5, SUI_T_SIZE, &target, link, color, color, color));	
				verse_send_o_link_set(rename_o_node_id, e_nso_get_link_id(link), target, e_nso_get_link_name(link), e_nso_get_link_target_id(link));
			
			if(input->mode == BAM_DRAW && (l_node = e_ns_get_node(0, target)) != NULL)
			{
				nr[256];
				glPushMatrix();
				glTranslatef(-0.15 + (-1 + rot_link * rot_link) * 3, y, 0);
				glScalef(0.1, 0.1, 0.1);
				co_node_draw(l_node, e_ns_get_node_type(l_node), FALSE);
				glPopMatrix();
				sprintf(nr, "(%s)", e_ns_get_node_name(l_node));
				sui_draw_text(0.30, y, SUI_T_SIZE, SUI_T_SPACE, nr, color_light, color_light, color_light);  
			}
			y -= 0.05;

			sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Target ID:", color_light, color_light, color_light);  
			target = e_nso_get_link_target_id(link);
			if(sui_type_number_uint(input, 0.15, y, 0.5, SUI_T_SIZE, &target, link, color, color, color));
				verse_send_o_link_set(rename_o_node_id, e_nso_get_link_id(link), e_nso_get_link_node(link), e_nso_get_link_name(link), target);
			sui_draw_rounded_square(-0.3, y + 0.125, 1, -0.15, color_light, color_light, color_light);
			y -= 0.075;
		}
	}
	glPopMatrix();
	return co_handle_return(input);
}
