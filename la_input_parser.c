
#include "la_includes.h"

#include "la_geometry_undo.h"
#include "la_projection.h"
#include "la_tool.h"
#include "la_pop_up.h"
#include "la_draw_overlay.h"
#include "la_particle_fx.h"
 
#define VERTEX_SNAP_DISTANCE 0.0003
#define MAX_POPUP_TIME 100

typedef enum { 
	PIM_IDLE,
	PIM_DRAW,
	PIM_DRAW_SELECT,
	PIM_RESHAPE,
	PIM_SPLIT,
	PIM_DRAG_MANIPULATOR,
	PIM_DRAG_ONE_VERTEX,
	PIM_DRAG_ONE_TAG,
	PIM_SHOW_EMPTY_MENU,
	PIM_SHOW_VERTEX_MENU,
	PIM_SHOW_EDGE_MENU,
	PIM_SHOW_MANIPULATOR_MENU,
	PIM_SHOW_POLY_MENU,
	PIM_CHANGE_VIEW
}ParseInputMode;

struct{
	ParseInputMode	mode;
	uint			start_vertex;
	uint			closest_vertex;
	double			depth[3];
	uint			click_time;
}ParseInputData;

boolean draw_view_cage(void)
{
	return ParseInputData.mode != PIM_CHANGE_VIEW;
}

void la_parse_input(BInputState *input)
{
	double output[3]; 
	static double snap[6], distance, selected_distance;
	static uint closest, select_closest, edge[2] = {-1, -1}, polygon, tag;
	static LASnapType snap_type = LA_ST_VERTEX;
	if(input->mode == BAM_EVENT)
	{
		distance = 1E100;
		selected_distance = 1E100;
//		printf("Asnaps %u %f\n", closest, selected_distance);
		p_find_closest_vertex(&closest, &select_closest, &distance, &selected_distance, input->pointer_x, input->pointer_y);
//		printf("Bsnaps %u %f\n", closest, selected_distance);
//		select_closest = 0;
		udg_get_vertex_pos(snap, select_closest);
		snap_type = LA_ST_VERTEX;
//		printf("Csnaps %u %f\n", closest, selected_distance);
		if(selected_distance > VERTEX_SNAP_DISTANCE - (0.1 * VERTEX_SNAP_DISTANCE))
		{
			if(ParseInputData.mode != PIM_SHOW_POLY_MENU)
			{
				polygon = la_t_poly_test(input, snap, &snap[3]);
				if(polygon != -1)
					snap_type = LA_ST_SURFACE;
			}
			if(ParseInputData.mode != PIM_SHOW_EDGE_MENU)
			{
				if(p_find_closest_edge(edge, snap, input->pointer_x, input->pointer_y))
				{
					printf("snap[3] = %f\n", snap[3]);
					snap_type = LA_ST_EDGE;
				}
			}
		}
	}
	switch(ParseInputData.mode)
	{
		case PIM_IDLE :
			if(input->mode == BAM_EVENT)
			{
				if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
				{
					ParseInputData.click_time = 0;
					if(la_t_tm_grab(input))
					{
						ParseInputData.mode = PIM_DRAG_MANIPULATOR;
						return;
					}
					if(p_find_click_tag_lable(input->pointer_x, input->pointer_y))
						return;

					tag = p_find_click_tag(input->pointer_x, input->pointer_y);
					if(tag != -1)
					{
						udg_get_tag_pos(tag, ParseInputData.depth);
						ParseInputData.mode = PIM_DRAG_ONE_TAG;
						return;
					}
					if(selected_distance < VERTEX_SNAP_DISTANCE - (0.1 * VERTEX_SNAP_DISTANCE))
					{
						udg_get_vertex_pos(ParseInputData.depth, select_closest);
						ParseInputData.start_vertex = select_closest;
					}
					else
					{
						if(edge[0] != -1 && edge[1] != -1)
						{
							if(udg_get_select(edge[0]) > 0.01 && udg_get_select(edge[1]) > 0.01)
							{
								ParseInputData.mode = PIM_RESHAPE;								
								la_t_reshape_hull_start(input, edge);
							}else
							{
								ParseInputData.mode = PIM_SPLIT;
								la_t_edge_splitter_start(input, edge);
							}
							return;
						}
						p_get_projection_vertex(ParseInputData.depth, ParseInputData.depth, input->pointer_x, input->pointer_y);
						ParseInputData.start_vertex = -1;
					}
					if(ParseInputData.start_vertex != -1 && udg_get_select(ParseInputData.start_vertex) > 0.5)
					{
                    	ParseInputData.mode = PIM_DRAG_ONE_VERTEX;
                        grab_one_vertex(input, ParseInputData.start_vertex, ParseInputData.depth);
					}
                    else
						ParseInputData.mode = PIM_DRAW;
					la_t_new_draw_line();
				}else if(input->mouse_button[2] == TRUE && input->last_mouse_button[2] == FALSE)
				{
					ParseInputData.start_vertex = select_closest;
					if(la_t_tm_test_center(input))
						ParseInputData.mode = PIM_SHOW_MANIPULATOR_MENU;
					else if(selected_distance < VERTEX_SNAP_DISTANCE - (0.1 * VERTEX_SNAP_DISTANCE))
					{
						ParseInputData.mode = PIM_SHOW_VERTEX_MENU;
						la_pu_vertex(input, ParseInputData.start_vertex);
					}
					else if(edge[0] != -1 && edge[1] != -1)
						ParseInputData.mode = PIM_SHOW_EDGE_MENU;
					else if((polygon = la_t_poly_test(input, NULL, NULL)) != -1)
						ParseInputData.mode = PIM_SHOW_POLY_MENU;
					else
						ParseInputData.mode = PIM_SHOW_EMPTY_MENU;
				}
				if(input->mouse_button[1] == TRUE && input->last_mouse_button[1] == FALSE)
				{
					if(la_t_tm_test_center(input))
						la_t_tm_view_center();
					else
					{
						ParseInputData.mode = PIM_CHANGE_VIEW;
						p_view_change_start(input);
					}
				}
			}else
			{
				if(la_t_tm_test_center(input) == FALSE)
				{
					if((ParseInputData.mode == PIM_IDLE && selected_distance < VERTEX_SNAP_DISTANCE - (0.1 * VERTEX_SNAP_DISTANCE)) || (ParseInputData.mode == PIM_DRAW || ParseInputData.mode == PIM_DRAG_MANIPULATOR || ParseInputData.mode == PIM_DRAG_ONE_VERTEX) && input->mouse_button[1] == TRUE)
					{
						double closest[3];
						udg_get_vertex_pos(closest, select_closest);
						la_do_active_vertex(closest, 0.01 < udg_get_select(select_closest));
					}else
						la_do_draw_closest_edge(edge, input->pointer_x, input->pointer_y, FALSE);
				}
				la_t_tm_draw(input, ParseInputData.mode == PIM_DRAG_MANIPULATOR);
			}
			if(ParseInputData.mode != PIM_IDLE)
				la_parse_input(input);
		break;
		case PIM_DRAW :
			if(input->mode == BAM_DRAW)
			{
				glDisable(GL_DEPTH_TEST);
				if(selected_distance > VERTEX_SNAP_DISTANCE - (0.1 * VERTEX_SNAP_DISTANCE) && ParseInputData.start_vertex == -1 && input->mode == BAM_DRAW)
					la_t_draw_line_draw_delete_overlay();
				glEnable(GL_DEPTH_TEST);
			}
			p_get_projection_vertex_with_axis(output, ParseInputData.depth, input->pointer_x, input->pointer_y, input->mouse_button[1], snap, snap_type);
			if(ParseInputData.start_vertex == -1)
				if(la_t_draw_select_menu_test())
					ParseInputData.mode = PIM_DRAW_SELECT;
			if(input->mode == BAM_EVENT)
			{			
				if((input->mouse_button[2] == FALSE && input->last_mouse_button[2] == TRUE) || (input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE))
				{
					double pos[3];
					uint vertex[2];
					if(selected_distance < VERTEX_SNAP_DISTANCE - (0.1 * VERTEX_SNAP_DISTANCE) && input->mouse_button[1] != TRUE)
					{
						vertex[1] = select_closest;
						if(ParseInputData.start_vertex == select_closest)
						{
							udg_set_select(select_closest, 1);
							break;
						}else if(ParseInputData.start_vertex != -1)
						{
							vertex[0] = ParseInputData.start_vertex;
							if(la_t_edge_connector(vertex) == TRUE)
								udg_create_edge(ParseInputData.start_vertex, vertex[1]);
							ParseInputData.start_vertex = vertex[1];
							udg_get_vertex_pos(ParseInputData.depth, ParseInputData.start_vertex);
							break;
						}
					}else
					{
						if(ParseInputData.start_vertex == -1 && input->mouse_button[1] != TRUE && input->last_mouse_button[2] != TRUE)
						{
							if(la_t_draw_line_test_delete() || la_t_draw_line_test_select(SM_SELECT))
								break;
							if(0.01 > (input->pointer_x - input->click_pointer_x) * (input->pointer_x - input->click_pointer_x) + (input->pointer_y - input->click_pointer_y) * (input->pointer_y - input->click_pointer_y))
							{
								polygon = la_t_poly_test(input, NULL, NULL);
								if(polygon == -1)
								{
									la_pfx_create_dust_selected_vertexes(ParseInputData.depth);
									udg_clear_select(0);
								}								
								else
									la_t_poly_select(polygon);
								break;
							}
						}
						p_get_projection_vertex_with_axis(pos, ParseInputData.depth, input->pointer_x, input->pointer_y, input->mouse_button[1], snap, snap_type);
						vertex[1] = udg_find_empty_slot_vertex();
						udg_vertex_set(vertex[1], NULL, pos[0], pos[1], pos[2]);
						ParseInputData.depth[0] = pos[0];
						ParseInputData.depth[1] = pos[1];
						ParseInputData.depth[2] = pos[2];
					}
					if(ParseInputData.start_vertex == -1)
					{
						p_get_projection_vertex_with_axis(pos, ParseInputData.depth, input->click_pointer_x, input->click_pointer_y, FALSE, snap, snap_type);
						ParseInputData.start_vertex = udg_find_empty_slot_vertex();
						udg_vertex_set(ParseInputData.start_vertex, NULL, pos[0], pos[1], pos[2]);				
					}
					udg_create_edge(ParseInputData.start_vertex, vertex[1]);
					ParseInputData.start_vertex = vertex[1];
					if(input->mouse_button[2] == FALSE && input->last_mouse_button[2] == TRUE)
						undo_event_done();
				}
			}else
			{
				la_do_draw(ParseInputData.depth, output, input->mouse_button[1], snap);
				if(ParseInputData.start_vertex == -1)
					la_t_draw_line_add(input->pointer_x, input->pointer_y, TRUE);

				if(input->mouse_button[1] && snap_type != LA_ST_VERTEX)
				{
					if(snap_type == LA_ST_EDGE)
						la_do_draw_snap_edge(edge);	
					if(snap_type == LA_ST_SURFACE)
						la_do_active_polygon(snap);
				}
				if(selected_distance < VERTEX_SNAP_DISTANCE - (0.1 * VERTEX_SNAP_DISTANCE) || (input->mouse_button[1] && snap_type == LA_ST_VERTEX))
				{
					double closest[3];
					udg_get_vertex_pos(closest, select_closest);
					la_do_active_vertex(closest, FALSE);
				}
			}
		break;
		case PIM_RESHAPE :
			if(input->mode == BAM_EVENT)
				la_t_reshape_hull(input);
			else
				la_t_reshape_hull_draw();
		break;
		case PIM_SPLIT :
			if(input->mode == BAM_EVENT)
				la_t_edge_splitter(input);
		break;
		case PIM_DRAG_MANIPULATOR :
		//	udg_get_vertex_pos(snap, select_closest);
			if(input->mode == BAM_EVENT)
				la_t_tm_manipulate(input, snap, snap_type);
			else
				la_t_tm_draw(input, ParseInputData.mode == PIM_DRAG_MANIPULATOR);
		break;
		case PIM_DRAG_ONE_VERTEX :
			{
				double vertex[3];
				static uint collapse = 0;
				udg_get_vertex_pos(snap, select_closest);

				if(input->mode == BAM_EVENT)
				{
                    p_get_projection_vertex_with_axis(vertex, ParseInputData.depth, input->pointer_x, input->pointer_y, input->mouse_button[1], snap, snap_type);
                    grab_one_vertex(input, ParseInputData.start_vertex, vertex);
//                  udg_vertex_move(ParseInputData.start_vertex, vertex[0], vertex[1], vertex[2]);
                    if(input->mouse_button[0] == FALSE)
						ParseInputData.mode = PIM_IDLE;
                    
/*					if(input->last_mouse_button[0] != TRUE)
						collapse = ParseInputData.start_vertex;
					if(input->mouse_button[2] != TRUE)
						p_get_projection_vertex_with_axis(vertex, ParseInputData.depth, input->pointer_x, input->pointer_y, input->mouse_button[1], snap, snap_type);
					else
					{
						if(closest != ParseInputData.start_vertex)
							collapse = select_closest;
						vertex[0] = snap[0];
						vertex[1] = snap[1];
						vertex[2] = snap[2];
					}                    
					if(input->mouse_button[0] == FALSE)
					{
						if(input->mouse_button[2] != TRUE)
							udg_vertex_set(ParseInputData.start_vertex, ParseInputData.depth, vertex[0], vertex[1], vertex[2]);
						else
							la_t_collapse_two_vertexes(ParseInputData.start_vertex, select_closest);
						ParseInputData.mode = PIM_IDLE;
					}else
						udg_vertex_move(ParseInputData.start_vertex, vertex[0], vertex[1], vertex[2]);*/
				}else
				{
					la_do_xyz_lines(ParseInputData.depth, input->mouse_button[1]);
					if(input->mouse_button[1] && snap_type != LA_ST_VERTEX)
					{
						if(snap_type == LA_ST_EDGE)
							la_do_draw_snap_edge(edge);	
					}
					if(selected_distance < VERTEX_SNAP_DISTANCE - (0.1 * VERTEX_SNAP_DISTANCE) || (input->mouse_button[1] && snap_type == LA_ST_VERTEX))
					{
						double closest[3];
						udg_get_vertex_pos(closest, select_closest);
						la_do_active_vertex(closest, FALSE);
					}
				}
			}
		break;
		case PIM_DRAG_ONE_TAG :
			{
				double vec[3];
				static uint collapse = 0;
				udg_get_vertex_pos(snap, select_closest);
				if(input->mode == BAM_EVENT)
				{
					p_get_projection_vertex_with_axis(vec, ParseInputData.depth, input->pointer_x, input->pointer_y, input->mouse_button[1], snap, snap_type);
					udg_move_tag(tag, vec);
					if(input->mouse_button[0] == FALSE)
					{
						if(VERTEX_SNAP_DISTANCE > (input->pointer_x - input->click_pointer_x) * (input->pointer_x - input->click_pointer_x) + (input->pointer_y - input->click_pointer_y) * (input->pointer_y - input->click_pointer_y))
						{
							udg_move_tag(tag, ParseInputData.depth);
							udg_select_tag(tag, 1);
						}
						ParseInputData.mode = PIM_IDLE;
					}
				}else
				{
					la_do_xyz_lines(ParseInputData.depth, input->mouse_button[1]);
					if(select_closest != ParseInputData.start_vertex && selected_distance < VERTEX_SNAP_DISTANCE - (0.1 * VERTEX_SNAP_DISTANCE) && input->mouse_button[1] == TRUE)
						la_do_active_vertex(snap, FALSE);
				}
			}
		break;
		case PIM_DRAW_SELECT :
			{
				uint i;
				i = la_pu_select(input);
				if(input->mode == BAM_EVENT)
				{
					if(i != -1)
					la_t_draw_line_test_select(i);
				}else
					la_t_draw_line_add(input->pointer_x, input->pointer_y, FALSE);
			}
		break;
		case PIM_SHOW_EMPTY_MENU :
			if(la_pu_empty(input))
			{
				ParseInputData.depth[0] = 0;
				ParseInputData.depth[1] = 0;
				ParseInputData.depth[2] = 0;
			}
			if((input->mouse_button[2] == FALSE && input->last_mouse_button[2] == TRUE) || (input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE))
				ParseInputData.mode = PIM_IDLE;
		break;
		case PIM_SHOW_VERTEX_MENU :
			la_pu_vertex(input, ParseInputData.start_vertex);
		break;
		case PIM_SHOW_EDGE_MENU :
			la_pu_edge(input, edge);
		break;
		case PIM_SHOW_MANIPULATOR_MENU :
			la_pu_manipulator(input);
		break;
		case PIM_SHOW_POLY_MENU :
			la_pu_polygon(input, polygon);
		break;
		case PIM_CHANGE_VIEW :
			if(input->mode != BAM_EVENT)
				la_t_tm_draw(input, FALSE);
			else
				p_view_change(input);
		break;
	}
	if(input->mode != BAM_EVENT)
/*	{
		la_t_tm_draw(input, ParseInputData.mode == PIM_DRAG_MANIPULATOR);
		la_pu_type(input);
	}*/
	if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == FALSE && input->mouse_button[1] == FALSE && input->last_mouse_button[1] == FALSE && input->mouse_button[2] == FALSE && input->last_mouse_button[2] == FALSE)
	{
		undo_event_done();
		ParseInputData.mode = PIM_IDLE;
		ParseInputData.click_time = 0;
	}
//	ParseInputData.mode = PIM_CHANGE_VIEW;
}

extern void la_pu_browser(BInputState *input, uint node);

void la_edit_func(BInputState *input, void *user)
{
	if(input->mode == BAM_MAIN)
	{
		verse_callback_update(0);
#if defined PERSUADE_H
		p_task_compute(1);
#endif
		return;
	}
	if(input->mode == BAM_DRAW)
	{
		glClearColor(0, 0, 0, 0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glPushMatrix();
		p_view_set();
		la_do_owerlay();
		la_pfx_select_vertex();
	}
	if(udg_update_geometry())
	{
		la_parse_input(input);
	}else
	{
		ENode *node;
		if(input->mode == BAM_DRAW)
		{
			glPopMatrix();
			glPushMatrix();
			glTranslatef(0, 0, -1);
			sui_draw_text(-0.5 * sui_compute_text_length(SUI_T_SIZE * 2, 3, "NO GEOMETRY"), 0, SUI_T_SIZE * 2, 3, "NO GEOMETRY", 1, 1, 1);
			sui_draw_text(-0.5 * sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, "Click to create a geometry node"), -0.1, SUI_T_SIZE, SUI_T_SPACE, "Click to create a geometry node", 1, 1, 1);
		}else
		{
			if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
				udg_create_new_modeling_node();
			node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY);
			if(node != NULL)
				udg_set_modeling_node(e_ns_get_node_id(node));
		}
	}
	if(input->mode == BAM_DRAW)
	{
		glPopMatrix();
		la_pfx_draw(FALSE);
	}
}
