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


void compute_spline_pos(double *out, double f, double *pos_0, double *pos_1, double *pos_2, double *pos_3)
{
	double fi, t[2];
	fi = 1 - f;
	t[0] = (pos_1[0] * f) + (pos_2[0] * fi);
	t[1] = (pos_1[1] * f) + (pos_2[1] * fi);

	out[0] = (((pos_0[0] * f) + (pos_1[0] * fi)) * f + t[0] * fi) * f + (((pos_2[0] * f) + (pos_3[0] * fi)) * fi + t[0] * f) * fi;
	out[1] = (((pos_0[1] * f) + (pos_1[1] * fi)) * f + t[1] * fi) * f + (((pos_2[1] * f) + (pos_3[1] * fi)) * fi + t[1] * f) * fi;
}
void co_c_draw_section(ECurve *curve, float scroll, float stretch, float y_pos, float scale, float pan, float color, float color_light)
{
	uint i, j, k;
	double out[2], out_old[2], pos[8], aspect;
	float c[3];
	aspect = betray_get_screen_mode(NULL, NULL, NULL);
	if(aspect < 0.1)
		aspect = 0.1;
	scale /= (aspect - 0.1 + y_pos);
	pan -= (y_pos - aspect + 0.1 - y_pos) * scale;

	for(i = 0; i < e_nsc_get_curve_dimensions(curve); i++)
	{
		for(j = 0; j < 3; j++)
		{
			if(i == j + 1)
				c[j] = color_light;
			else
				c[j] = color;
		}

		for(j = 1; j < e_nsc_get_curve_point_count(curve); j++)
		{

			e_nsc_get_segment(curve, j - 1, i, pos, &pos[2], &pos[4], &pos[6]);
			for(k = 0; k < 8; k += 2)
			{
				pos[k] = (pos[k] - scroll) / stretch;
				pos[k + 1] = (pos[k + 1] - pan) / scale;
			} 
			out_old[0] = pos[6];
			out_old[1] = pos[7];
			for(k = 0; k < 24; k++)
			{
				compute_spline_pos(out, (double)(k + 1) / 24.0, pos, &pos[2], &pos[4], &pos[6]);
				sui_draw_2d_line_gl(out[0], out[1], out_old[0], out_old[1], c[0], c[1], c[2]);
				out_old[0] = out[0];
				out_old[1] = out[1];
			}
		}
	}
}

void co_c_space(ECurve *curve, float *scale, float *pan)
{
	uint i, j, k, dimensions = 1;
	double pos[8];
	if(2 > e_nsc_get_curve_point_count(curve))
	{
		*scale = 2;
		*pan = -1;
		return;
	}
	dimensions = e_nsc_get_curve_dimensions(curve);
	e_nsc_get_segment(curve, 0, 0, pos, &pos[2], &pos[4], &pos[6]);
	*scale = pos[1];
	*pan = pos[1];
	for(i = 1; i < e_nsc_get_curve_point_count(curve); i++)
	{
		for(j = 0; j < dimensions; j++)
		{
			e_nsc_get_segment(curve, i - 1, j, pos, &pos[2], &pos[4], &pos[6]);
			for(k = 0; k < 4; k++)
			{
				if(pos[k * 2 + 1] > *scale)
					*scale = pos[k * 2 + 1];
				if(pos[k * 2 + 1] < *pan)
					*pan = pos[k * 2 + 1];
			}
		}
	}
	*scale -= *pan;
/*	*pan;*/
}

void sui_draw_symb_curve_point(float pos_x, float pos_y, float red, float green, float blue)
{
	static float array[] = {0.008000, -0.004000, 0.006000, -0.004536, 0.006000, -0.004536, 0.004536, -0.006000, 0.004536, -0.006000, 0.004000, -0.008000, 0.008000, -0.004000, 0.010000, -0.003464, 0.010000, -0.003464, 0.011464, -0.002000, 0.011464, -0.002000, 0.012000, 0.000000, 0.008000, 0.004000, 0.010000, 0.003464, 0.010000, 0.003464,
	 0.011464, 0.002000, 0.011464, 0.002000, 0.012000, 0.000000, 0.008000, 0.004000, 0.006000, 0.004536, 0.006000, 0.004536, 0.004536, 0.006000, 0.004536, 0.006000, 0.004000, 0.008000, -0.000000, 0.012000, 0.002000, 0.011464, 0.002000, 0.011464, 0.003464, 0.010000, 0.003464, 0.010000, 0.004000, 0.008000, -0.000000, 0.012000, -0.002000, 
	0.011464, -0.002000, 0.011464, -0.003464, 0.010000, -0.003464, 0.010000, -0.004000, 0.008000, -0.008000, 0.004000, -0.006000, 0.004536, -0.006000, 0.004536, -0.004536, 0.006000, -0.004536, 0.006000, -0.004000, 0.008000, -0.008000, 0.004000, -0.010000, 0.003464, -0.010000, 0.003464, -0.011464, 0.002000, -0.011464, 0.002000, -0.012000, 
	0.000000, -0.008000, -0.004000, -0.010000, -0.003464, -0.010000, -0.003464, -0.011464, -0.002000, -0.011464, -0.002000, -0.012000, 0.000000, -0.008000, -0.004000, -0.006000, -0.004536, -0.006000, -0.004536, -0.004536, -0.006000, -0.004536, -0.006000, -0.004000, -0.008000, -0.000000, -0.012000, -0.002000, -0.011464, -0.002000, -0.011464, 
	-0.003464, -0.010000, -0.003464, -0.010000, -0.004000, -0.008000, -0.000000, -0.012000, 0.002000, -0.011464, 0.002000, -0.011464, 0.003464, -0.010000, 0.003464, -0.010000, 0.004000, -0.008000, -0.014000, 0.020000, -0.017000, 0.019196, -0.017000, 0.019196, -0.019196, 0.017000, -0.019196, 0.017000, -0.020000, 0.014000, 0.014000, -0.008000, 
	0.017000, -0.008804, 0.017000, -0.008804, 0.019196, -0.011000, 0.019196, -0.011000, 0.020000, -0.014000, 0.014000, -0.020000, 0.017000, -0.019196, 0.017000, -0.019196, 0.019196, -0.017000, 0.019196, -0.017000, 0.020000, -0.014000, 0.014000, -0.008000, 0.011000, -0.008804, 0.011000, -0.008804, 0.008804, -0.011000, 0.008804, -0.011000,
	0.008000, -0.014000, 0.014000, -0.020000, 0.011000, -0.019196, 0.011000, -0.019196, 0.008804, -0.017000, 0.008804, -0.017000, 0.008000, -0.014000, -0.014000, 0.008000, -0.017000, 0.008804, -0.017000, 0.008804, -0.019196, 0.011000, -0.019196, 0.011000, -0.020000, 0.014000, -0.014000, 0.008000, -0.011000, 0.008804, -0.011000, 0.008804,
	-0.008804, 0.011000, -0.008804, 0.011000, -0.008000, 0.014000, -0.014000, 0.020000, -0.011000, 0.019196, -0.011000, 0.019196, -0.008804, 0.017000, -0.008804, 0.017000, -0.008000, 0.014000, -0.018000, 0.014000, -0.010000, 0.014000, 0.016000, -0.012000, 0.012000, -0.016000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	sui_draw_gl(GL_LINES, array, 124, 2, red, green, blue);
	glPopMatrix();
}

void	sui_draw_symb_empty(float point_x, float point_y, float pos_x, float pos_y, float color, float color_light)
{
	static float array[] = {-0.000000, 0.008000, 0.008000, 0.000000, 0.008000, 0.000000, -0.000000, -0.008000, -0.000000, -0.008000, -0.008000, 0.000000, -0.008000, 0.000000, -0.000000, 0.008000};
	float f, vec[2];

	vec[0] = point_x - pos_x;
	vec[1] = point_y - pos_y;
	f = sqrt(vec[0] * vec[0] + vec[1] * vec[1]) * 60;
	sui_draw_2d_line_gl(point_x - vec[0] / f, point_y - vec[1] / f, pos_x + vec[0] / f, pos_y + vec[1] / f, color_light, color_light, color_light);
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	sui_draw_gl(GL_LINES, array, 8, 2, color, color, color);
	glPopMatrix();
}


void co_c_draw_point(BInputState *input, ENode *node, ECurve *curve, float *scroll, float *stretch, float y_pos, float scale, float pan, float color, float color_light)
{
	uint i, dimensions, point;
	static uint grab_dim, grab_control, grab_point = -1;
	double pre_x[4], pre_y[4], pos_x, pos_y[4], post_x[4], post_y[4], x, y, aspect;
	aspect = betray_get_screen_mode(NULL, NULL, NULL);
	if(aspect < 0.1)
		aspect = 0.1;
	dimensions = e_nsc_get_curve_dimensions(curve);
	x = input->pointer_x;
	y = input->pointer_y;

	scale /= (aspect - 0.1 + y_pos);
	pan -= (y_pos - aspect + 0.1 - y_pos) * scale;
				
	sui_draw_2d_line_gl(-0.9, y_pos + 0.02, 0.9, y_pos + 0.02, color_light, color_light, color_light);
	sui_draw_2d_line_gl(-0.9, -aspect + 0.08, 0.9, -aspect + 0.08, color_light, color_light, color_light);

	if(input->mode == BAM_EVENT && input->mouse_button[0] != TRUE && input->last_mouse_button[0] == TRUE && grab_point == -1 && y_pos > y)
	{
		if(0.0001 > (x - input->click_pointer_x) * (x - input->click_pointer_x) + (y - input->click_pointer_y) * (y - input->click_pointer_y))
		{
			real64 pre_value[4] = {0, 0, 0, 0};
			uint32 pre_pos[4] = {0x8fffffff, 0x8fffffff, 0x8fffffff, 0x8fffffff};
			real64 value[4];
			real64 pos;
			real64 post_value[4] = {0, 0, 0, 0};
			uint32 post_pos[4] = {0x8fffffff, 0x8fffffff, 0x8fffffff, 0x8fffffff};
			for(i = 0; i < dimensions; i++)
				value[i] = y * scale + pan;
			pos = x * *stretch + *scroll;
			verse_send_c_key_set(e_ns_get_node_id(node), e_nsc_get_curve_id(curve), -1, dimensions, pre_value, pre_pos, value, pos, post_value, post_pos);
		}
	}
	if(input->mode == BAM_EVENT && input->mouse_button[0] == TRUE && input->last_mouse_button[0] == TRUE && grab_point == -1 && y_pos > input->click_pointer_y)
	{
		*scroll -= input->delta_pointer_x;
		*stretch *= 1 + input->delta_pointer_y * 4;
	}
	for(point = e_nsc_get_point_next(curve, 0); point != -1; point = e_nsc_get_point_next(curve, point + 1))
	{
		e_nsc_get_point_double(curve, point, pre_y, pre_x, pos_y, &pos_x, post_y, post_x);
		for(i = 0; i < dimensions; i++)
		{
			pre_x[i] = (pre_x[i] - *scroll) / *stretch;
			post_x[i] = (post_x[i] - *scroll) / *stretch;
			pre_y[i] = (pre_y[i] - pan) / scale;
			pos_y[i] = (pos_y[i] - pan) / scale;
			post_y[i] = (post_y[i] - pan) / scale;
		}
		pos_x = (pos_x - *scroll) / *stretch;
		for(i = 0; i < dimensions; i++)
		{
			if(input->mode == BAM_DRAW)
			{
				sui_draw_symb_empty(pos_x, pos_y[i], pre_x[i], pre_y[i], color, color_light);
				sui_draw_symb_empty(pos_x, pos_y[i], post_x[i], post_y[i], color, color_light);
				sui_draw_symb_curve_point(pos_x, pos_y[i], color, color, color);

				if(i == grab_dim && grab_point == point)
				{
					sui_draw_2d_line_gl(pos_x - 0.004, pos_y[i] + 0.024, pos_x - 0.024, pos_y[i] + 0.004, color, color, 1);
					sui_draw_2d_line_gl(pos_x - 0.004, pos_y[i] + 0.004, pos_x - 0.024, pos_y[i] + 0.024, color, color, 1);
				}
			}else
			{
				if(input->mouse_button[0] == TRUE)
				{
					if(input->last_mouse_button[0] != TRUE && y_pos > y)
					{
						if(0.0001 > (pre_x[i] - x) * (pre_x[i] - x) + (pre_y[i] - y) * (pre_y[i] - y))
						{
							grab_dim = i;
							grab_control = 0;
							grab_point = point;
						}
						if(0.0001 > (pos_x - x) * (pos_x - x) + (pos_y[i] - y) * (pos_y[i] - y))
						{
							grab_dim = i;
							grab_control = 1;
							grab_point = point;
						}
						if(0.0001 > (post_x[i] - x) * (post_x[i] - x) + (post_y[i] - y) * (post_y[i] - y))
						{
							grab_dim = i;
							grab_control = 2;
							grab_point = point;
						}
						if(0.0001 > (pos_x - x - 0.014) * (pos_x - x - 0.014) + (pos_y[i] - y + 0.014) * (pos_y[i] - y + 0.014))
						{
							grab_point = i;
							grab_control = -1;
							printf("del %u %u %u\n", e_ns_get_node_id(node), e_nsc_get_curve_id(curve), point);

							verse_send_c_key_destroy(e_ns_get_node_id(node), e_nsc_get_curve_id(curve), point);
						}
					}else
					{
						if(i == grab_dim && grab_point == point)
						{
							real64 pre_value[4]; 
							real64 pre_pos[4]; 
							real64 value[4]; 
							real64 pos;
							real64 post_value[4]; 
							real64 post_pos[4];
							e_nsc_get_point_double(curve, point, pre_value, pre_pos, value, &pos, post_value, post_pos);
							if(grab_control == 0)
							{
								pre_pos[i] = x * *stretch + *scroll;
								pre_value[i] = y * scale + pan;
								e_nsc_send_c_key_set(node, curve, point, pre_value, pre_pos, NULL, NULL, NULL, NULL);

							}
							if(grab_control == 1)
							{
								pos = x * *stretch + *scroll;
								value[i] = y * scale + pan;
								e_nsc_send_c_key_set(node, curve, point, NULL, NULL, value, &pos, NULL, NULL);
							}
							if(grab_control == 2)
							{
								post_pos[i] = x * *stretch + *scroll;
								post_value[i] = y * scale + pan;
								e_nsc_send_c_key_set(node, curve, point, NULL, NULL, NULL, NULL, post_value, post_pos);
							}
						}
					}
				}else
					grab_point = -1;
			}
		}
	}
}


uint change_c_node_id = 0;

void rename_c_layer_func(void *user, char *text)
{
	ECurve *curve;
	ENode *node;
	if((node = e_ns_get_node(0, change_c_node_id)) != NULL)
	{
		for(curve = e_nsc_get_curve_next(node, 0); curve != NULL && curve != user; curve = e_nsc_get_curve_next(node, e_nsc_get_curve_id(curve) + 1));
		if(curve == user)
			verse_send_c_curve_create(change_c_node_id, e_nsc_get_curve_id(curve), text, e_nsc_get_curve_dimensions(curve));
	}
}

extern float co_handle_node_head(BInputState *input, ENode *node);

boolean co_handle_curve(BInputState *input, ENode *node)
{
	ECurve *curve;
	float y, color, color_light, pre_expander;
	static float rot_curve = 0, rot_editor = 0;
	static float scroll = 0, stretch = 1;
	static boolean show_curve = TRUE, show_editor = TRUE;
	static uint curve_edit_id = 0;
	uint i, j = 0;

	if(change_c_node_id != e_ns_get_node_id(node))
	{
		scroll = 0;
		stretch = 1;
	}
	change_c_node_id = e_ns_get_node_id(node);

	y = co_handle_node_head(input, node);

	co_vng_divider(input, 0.2, y, &rot_curve, &color, &color_light, &show_curve, "Curves");

	pre_expander = y;
	if(rot_curve > 0.001)
	{
	
		y -= 0.05;
		sui_draw_text(-0.27, y, SUI_T_SIZE, SUI_T_SPACE, "Create new Curve", color_light, color_light, color_light); 
		for(i = 0; i < 4; i++)
		{
			char *dim[4] = {"1D", "2D", "3D", "4D"};
			if(sw_text_button(input, -0.0 + (float)i * 0.1, y, 0, SUI_T_SIZE, SUI_T_SPACE, dim[i], color, color, color))
			{
				char nr[32];
				j = 0;
				for(curve = e_nsc_get_curve_by_id(node, 0); curve != NULL; curve = e_nsc_get_curve_by_id(node, ++j));
				sprintf(nr, "curve_%u", j);
				verse_send_c_curve_create(change_c_node_id, -1, nr, i + 1);
			}
		}
		y -= 0.05;
		for(curve = e_nsc_get_curve_next(node, 0); curve != NULL ; curve = e_nsc_get_curve_next(node, e_nsc_get_curve_id(curve) + 1))
		{
			boolean e;
			uint dim;
			sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Curve name:", color_light, color_light, color_light);  
			co_w_type_in(input, 0.15, y, 0.5, SUI_T_SIZE, e_nsc_get_curve_name(curve), 16, rename_c_layer_func, e_nsc_get_curve_name(curve), color, color_light);

			if(co_w_close_button(input, 0.645, y, color, color, color))
				verse_send_c_curve_destroy(change_c_node_id, e_nsc_get_curve_id(curve));
			y -= 0.05;
			dim = e_nsc_get_curve_dimensions(curve);
			sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Dimensions:", color_light, color_light, color_light); 
			if(sui_type_number_uint(input, 0.15, y, 0.5, SUI_T_SIZE, &dim, curve, color, color, color))
				if(dim > 0 && dim < 5)
					verse_send_c_curve_create(change_c_node_id, e_nsc_get_curve_id(curve), e_nsc_get_curve_name(curve), dim);
			y -= 0.05;
			sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Edit:", color_light, color_light, color_light);  
			e = curve_edit_id == e_nsc_get_curve_id(curve);
			if(co_w_checkbox(input, 0.17, y, &e, color, color, color) && e)
				curve_edit_id = e_nsc_get_curve_id(curve);
			y -= 0.05;
			sui_draw_rounded_square(-0.3, y + 0.175, 1, -0.14, color_light, color_light, color_light);
		}
	}
	glPopMatrix();
	y -= 0.05 + (y - pre_expander) * (1 - rot_curve);
	co_vng_divider(input, 0.2, y, &rot_editor, &color, &color_light, &show_editor, "Editor");
	pre_expander = y;
	if(rot_editor > 0.001)
	{
		y -= 0.05;
		if((curve = e_nsc_get_curve_by_id(node, curve_edit_id)) != NULL)
		{
			char nr[32];
			j = 0;
			j = e_nsc_get_curve_point_count(curve);
			sprintf(nr, "curve count %u", j);
			sui_draw_text(-0.27, y, SUI_T_SIZE, SUI_T_SPACE, nr, color_light, color_light, color_light); 
		}
		sui_draw_text(0.0, y - 0.05, SUI_T_SIZE, SUI_T_SPACE, "Vertex", color_light, color_light, color_light);  
		if((curve = e_nsc_get_curve_by_id(node, curve_edit_id)) != NULL)
		{
			static float scale, pan;
			co_c_space(curve, &scale, &pan);
				
			co_c_draw_section(curve, scroll, stretch, y, scale, pan, color, color_light);
			co_c_draw_point(input, node, curve, &scroll, &stretch, y, scale, pan, color, color_light);
		}
	}
	glPopMatrix();
	y -= 0.05 + (y - pre_expander) * (1 - rot_editor);
/*
	co_vng_divider(input, 0.2, y, &rot_bone, &color, &color_light, &show_bone, "Bones");
	pre_expander = y;
	if(rot_bone > 0.001)
	{
		uint16 bone;
		y -= 0.05;
		if(sw_text_button(input, -0.27, y, 0, SUI_T_SIZE, SUI_T_SPACE, "Create new Bone", color, color, color))
		{
			uint i;
			char nr[32];
			i = 3;
			for(bone = e_nsg_get_bone_next(node, 0); bone != (uint16)-1; bone = e_nsg_get_bone_next(node, bone + 1))
				i++;
			sprintf(nr, "weight_%u", i);
			verse_send_g_bone_create(change_g_node_id, -1, nr, "reference", 0, 0, 0, 0, 0, 0, 0, 1);
			verse_send_g_bone_create(change_g_node_id, -2, "kalla", "rference", 0, 0, 0, 0, 0, 0, 0, 1);

		}
		y -= 0.05;
		for(bone = e_nsg_get_bone_next(node, 0); bone != (uint16)-1; bone = e_nsg_get_bone_next(node, bone + 1))
		{

			static double t[7];
			char *ref, *text[] = {"X", "Y", "Z", "X", "Y", "Z", "W"};
			uint i;
			uint32 *parent;
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
			e_nsg_get_bone_rot64(node, bone, &t[3]);
			parent = e_nsg_get_bone_parent(node, bone);
			sui_draw_text(-0.25, y, SUI_T_SIZE, SUI_T_SPACE, "RARENT", color_light, color_light, color_light);
			if(sui_type_number_uint(input,0.15, y, 0.15, SUI_T_SIZE, &parent, e_nsg_get_bone_weight(node, bone), color, color, color);
				verse_send_g_bone_create(change_g_node_id, bone, e_nsg_get_bone_weight(node, bone), ref, parent, t[0], t[1], t[2], t[3], t[4], t[5], t[6]);
			y -= 0.05;
			sui_draw_text(-0.25, y, SUI_T_SIZE, SUI_T_SPACE, "POSITION", color_light, color_light, color_light);
			sui_draw_text(-0.25, y - 0.15, SUI_T_SIZE, SUI_T_SPACE, "ROTATION", color_light, color_light, color_light);



			for(i = 0; i < 7; i++)
			{	
				sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, text[i], color_light, color_light, color_light);  
				if(sui_type_number_double(input, 0.15, y, 0.15, SUI_T_SIZE, &t[i], &ref[i], color, color, color))
					verse_send_g_bone_create(change_g_node_id, bone, e_nsg_get_bone_weight(node, bone), ref, parent, t[0], t[1], t[2], t[3], t[4], t[5], t[6]);
				if(co_w_slider(input, 0.3, y, 0.35, &t[i], color, color, color))
					verse_send_g_bone_create(change_g_node_id, bone, e_nsg_get_bone_weight(node, bone), ref, parent, t[0], t[1], t[2], t[3], t[4], t[5], t[6]);
				y -= 0.05;
			}
			sui_draw_rounded_square(-0.3, y + 0.525, 1, -0.49, color_light, color_light, color_light);
		}
	}*/
	glPopMatrix();
	y -= 0.05 + (y - pre_expander) * (1 - rot_curve);
	return co_handle_return(input);
}
