#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"

#include "enough.h"
#include "seduce.h"
#include "uv.h"
#include "uv_geometry.h"

/*
void uv_handle_view(BInputState *input, float *pos, boolean active);
void uvo_draw_square();
void uvo_draw_overlay(BInputState *input);
boolean uvg_update();
*/

typedef enum{
	IM_IDLE,
	IM_DRAW,
	IM_POLY,
	IM_VERTEX,
	IM_EDGE,
	IM_MANIP,
	IM_VIEW,
	IM_MENU,
	IM_PU_EMPTY,
	IM_PU_POLY,
	IM_PU_CORNER,
	IM_PU_EDGE,
	IM_PU_MANIP
}UVIMode;

void draw_a_polygon(uint id, float x, float y); /* debug draw */

void uv_input_handler(BInputState *input, void *user_pointer)
{
	static UVIMode mode = IM_IDLE;
	static uint	polygon = -1, part = -1;
	float view[3], x, y;
	x = uv_get_pos_x(input->pointer_x);
	y = uv_get_pos_y(input->pointer_y);


	if(input->mode == BAM_MAIN)
	{
		verse_callback_update(0);
		return;
	}
	uvg_update();
	uv_handle_view(input, view, mode == IM_VIEW);
	if(input->mode == BAM_DRAW)
	{
		glClearColor(0, 0, 0, 0);
		glDisable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		uvo_draw_square();
		uvo_draw_overlay(input);
		uve_update();
	}

	if(mode == IM_IDLE)
		polygon = uv_test_all_polygons(x, y);

	draw_a_polygon(polygon, x, y);
//		uve_test(input->pointer_x, input->pointer_y);
	if(mode == IM_DRAW)
		uv_draw_line_add(input->pointer_x, input->pointer_y, TRUE);
	if(input->mode == BAM_EVENT)
	{
		if(input->mouse_button[2] == TRUE && input->last_mouse_button[2] == FALSE && mode == IM_IDLE)
		{

			if(uv_trans_test_manip(input))
				mode = IM_PU_MANIP;
			else if(polygon == -1)
				mode = IM_PU_EMPTY;
			else if((part = uv_test_corner(polygon, x, y)) != -1)
				mode = IM_PU_CORNER;
			else if((part = uv_test_edge(polygon, x, y)) != -1)
				mode = IM_PU_EDGE;
			else
				mode = IM_PU_POLY;
		}
		if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE && mode == IM_IDLE)
		{
			if(uv_test_menu(input))
				mode = IM_MENU;
			else if(uv_trans_begin(input))
			{
				mode = IM_MANIP;
				uvo_set_overlay_motion(0.5);
			}else
			{
				if(polygon != -1)
				{	
					if((part = uv_test_corner(polygon, x, y)) != -1)
					{
						uvo_set_overlay_motion(0.5);
						mode = IM_VERTEX;
					}
					else if((part = uv_test_edge(polygon, x, y)) != -1)
					{
						mode = IM_EDGE;
					}else
					{
						uv_start_polygon_unfold(polygon, x, y);
						mode = IM_POLY;
					}
				}else
				{
					uv_new_draw_line();
					mode = IM_DRAW;
				}
			}
		}
		if(input->mouse_button[1] == TRUE && input->last_mouse_button[1] == FALSE && mode == IM_IDLE)
			mode = IM_VIEW;

		if(mode == IM_VERTEX && polygon != -1 && polygon == uvg_get_next_polygon(polygon))
			uv_trans_move_corner(input, polygon, part);
		if(mode == IM_EDGE && polygon != -1 && polygon == uvg_get_next_polygon(polygon))
			uv_trans_move_edge(input, polygon, part);
		if(mode == IM_MANIP)
			uv_trans_continiue(input);
		if(mode == IM_POLY)
			polygon = uv_polygon_unfold(polygon, x, y);

		
		if(input->mouse_button[1] == FALSE && input->last_mouse_button[1] == TRUE && mode == IM_VIEW)
			mode = IM_IDLE;
		if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE && mode != IM_VIEW)
		{
			
			if(mode == IM_POLY)
			{
				if(uv_test_polygon(polygon, x, y))
				{
					if(polygon != -1 && polygon == uvg_get_next_polygon(polygon) && (0.025 * 0.025 > (input->pointer_x - input->click_pointer_x) * (input->pointer_x - input->click_pointer_x) + (input->pointer_y - input->click_pointer_y) * (input->pointer_y - input->click_pointer_y)))
					{
						egreal *s;
						s = uvg_get_corner_select(polygon);
						uvo_set_overlay_motion(0.02);
						
	
						{
							if(s[0] > 0.5 && s[1] > 0.5 && s[2] > 0.5 && (!uvg_is_quad(polygon) || s[3] > 0.5))
							{							
								uvg_set_all_corner_select(polygon, 0, 0, 0, 0);
								uv_trans_hide();
							}
							else
							{
								uvg_set_all_corner_select(polygon, 1, 1, 1, 1);
								uv_trans_unhide();
							}
						}
					}
				}
			}if(mode == IM_EDGE)
			{
				egreal *s;
				s = uvg_get_corner_select(polygon);
				uvo_set_overlay_motion(0.02);
				if(0.001 > (input->pointer_x - input->click_pointer_x) * (input->pointer_x - input->click_pointer_x) + (input->pointer_y - input->click_pointer_y) * (input->pointer_y - input->click_pointer_y))
				{
					if(mode == IM_EDGE && polygon != -1 && polygon == uvg_get_next_polygon(polygon))
					{
						if(s[part] > 0.5 && s[(part + uvg_get_sides(polygon) - 1) % uvg_get_sides(polygon)])
						{
							uvg_set_one_corner_select(polygon, part, 0);
							uvg_set_one_corner_select(polygon, (part + uvg_get_sides(polygon) - 1) % uvg_get_sides(polygon), 0);
							uv_trans_hide();
						}
						else
						{
							uvg_set_one_corner_select(polygon, part, 1);
							uvg_set_one_corner_select(polygon, (part + uvg_get_sides(polygon) - 1) % uvg_get_sides(polygon), 1);
							uv_trans_unhide();
						}
					}
				}
			}
			else
				uve_test(input->pointer_x, input->pointer_y);
			if(mode == IM_MANIP)
				uv_trans_end();
			if(mode == IM_DRAW)
			{
				uvo_set_overlay_motion(0.02);
				if(0.001 > (input->pointer_x - input->click_pointer_x) * (input->pointer_x - input->click_pointer_x) + (input->pointer_y - input->click_pointer_y) * (input->pointer_y - input->click_pointer_y))
				{
					uvg_set_corner_select_all(0);
					uv_trans_hide();
				}
				else
					uv_draw_line_test_select();
			}
			if(mode == IM_VERTEX && polygon != -1 && polygon == uvg_get_next_polygon(polygon))
			{
				uvg_set_one_corner_select(polygon, part, 1);
				uv_trans_unhide();
			}

			uvg_action_end();
			mode = IM_IDLE;
		}		
	}
	if(mode == IM_PU_EMPTY)
		uv_pu_empty(input);
	if(mode == IM_PU_POLY && polygon != -1 && polygon == uvg_get_next_polygon(polygon))
		uv_pu_polygon(input, polygon);
	if(mode == IM_PU_CORNER && polygon != -1 && polygon == uvg_get_next_polygon(polygon))
		uv_pu_corner(input, polygon, part);
	if(mode == IM_PU_EDGE && polygon != -1 && polygon == uvg_get_next_polygon(polygon))
		uv_pu_edge(input, polygon, part);
	if(mode == IM_PU_MANIP && polygon != -1 && polygon == uvg_get_next_polygon(polygon))
		uv_pu_manip(input);


	if((input->mouse_button[2] == FALSE && input->last_mouse_button[2] == FALSE) && (mode == IM_PU_EMPTY || mode == IM_PU_POLY || mode == IM_PU_EDGE || mode == IM_PU_CORNER || mode == IM_PU_MANIP || mode == IM_MENU))
	{
		mode = IM_IDLE;
		uvg_action_end();
	}
	if(input->mode == BAM_DRAW)
	{

		uv_trans_draw();

		glPopMatrix();
			uve_draw();	
	}
	uv_draw_menu(input);
	glEnable(GL_DEPTH_TEST);
}
