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

#define CONNECTOR_ENOUGH_SLOTT 1

typedef struct{
	uint		node_id;
	float		pos_x;
	float		pos_y;
	boolean		hidden;
}COVerseNode;

float view_cam_pos[3] = {0, 0, 4};
float view_cam_pos_goal[3] = {0, 0, 4};
float view_cam_move_start[2];
float view_cam_speed = 0.1;

void co_handle_view(BInputState *input, float *pos, boolean active)
{
	float light[4] = {1, 1, 1, 1};

	if(active && input->mouse_button[1])
	{
		if(input->mouse_button[0])
		{
			view_cam_pos_goal[2] *= 1 + 2 * ((input->pointer_y - view_cam_move_start[1]) + (input->pointer_x - view_cam_move_start[0]));
			if(view_cam_pos_goal[2] < 0.5)
				view_cam_pos_goal[2] = 0.5;
			view_cam_speed = 0.1;
		}else
		{
			view_cam_pos_goal[0] += (input->pointer_x - view_cam_move_start[0]) * view_cam_pos[2];
			view_cam_pos_goal[1] += (input->pointer_y - view_cam_move_start[1]) * view_cam_pos[2];
			view_cam_speed = 0.3;
		}
	}else if(active && input->mouse_button[0])
	if(0.025 * 0.025 < (input->pointer_x - input->click_pointer_x) * (input->pointer_x - input->click_pointer_x) + (input->pointer_y - input->click_pointer_y) * (input->pointer_y - input->click_pointer_y))
	{
		if(input->click_pointer_x > 0.9 || input->click_pointer_x < -0.9)
		{
			view_cam_pos_goal[2] *= 1 + 2 * ((input->pointer_y - view_cam_move_start[1]) + (input->pointer_x - view_cam_move_start[0]));
			if(view_cam_pos_goal[2] < 0.5)
				view_cam_pos_goal[2] = 0.5;
			view_cam_speed = 0.1;
		}else
		{
			view_cam_pos_goal[0] += (input->pointer_x - view_cam_move_start[0]) * view_cam_pos[2];
			view_cam_pos_goal[1] += (input->pointer_y - view_cam_move_start[1]) * view_cam_pos[2];
			view_cam_speed = 0.3;
		}
	}	
	view_cam_move_start[0] = input->pointer_x;
	view_cam_move_start[1] = input->pointer_y;
	pos[0] = view_cam_pos[0] = (view_cam_pos_goal[0] * view_cam_speed) + (view_cam_pos[0] * (1.0 - view_cam_speed));
	pos[1] = view_cam_pos[1] = (view_cam_pos_goal[1] * view_cam_speed) + (view_cam_pos[1] * (1.0 - view_cam_speed));
	pos[2] = view_cam_pos[2] = (view_cam_pos_goal[2] * view_cam_speed) + (view_cam_pos[2] * (1.0 - view_cam_speed));
} 

float co_get_pos_x(float x)
{
	return x * view_cam_pos[2] - view_cam_pos[0];
}

float co_get_pos_y(float y)
{
	return y * view_cam_pos[2] - view_cam_pos[1];
}

float co_get_view_x(float x)
{
	return (x + view_cam_pos[0]) / view_cam_pos[2];
}

float co_get_view_y(float y)
{
	return (y + view_cam_pos[1]) / view_cam_pos[2];
}

COVerseNode *create_verse_node(ENode *node)
{
	COVerseNode *co_node;
	co_node = malloc(sizeof *co_node);
	co_node->node_id = e_ns_get_node_id(node);
	co_node->pos_x = co_get_pos_x(((float)e_ns_get_node_type(node) - ((float)(V_NT_NUM_TYPES) / 2) + 0.5) / V_NT_NUM_TYPES * 1.8);
	co_node->pos_y = co_get_pos_y(0);
	co_node->hidden = TRUE /*e_ns_get_node_owner(node) == VN_OWNER_MINE*/;
	e_ns_set_custom_data(node, CONNECTOR_ENOUGH_SLOTT, co_node);
	printf("creating node");
	return co_node;
}


void verse_node_create_func(ENode *node, ECustomDataCommand command)
{
	if(command == E_CDC_CREATE)
		create_verse_node(node);
	if(command == E_CDC_DESTROY)
		free(e_ns_get_custom_data(node, CONNECTOR_ENOUGH_SLOTT));
}


void co_draw_bitmap(ENode *node);

void p_render_object(ENode *node);

void co_node_draw(ENode *node, VNodeType type, boolean hidden)
{
	glDisable(GL_DEPTH_TEST);
	co_vng_ring();
	if(!hidden)
	{
		switch(type)
		{
			case V_NT_OBJECT :

				co_vng_sun();
				co_vng_sunrays();
//				co_vng_color(245.0 / 255.0, 243.0 / 255.0, 170.0 / 255.0);
#ifdef PERSUADE_H
				if(node != NULL)	
					p_render_object(node);
#endif
			break;
			case V_NT_GEOMETRY :
				co_vng_saturn_ring();
				co_vng_gas_planet();
//				co_vng_color(254.0 / 255.0, 214.0 / 255.0, 161.0 / 255.0);
#ifdef PERSUADE_H
				{

					static double t = 0;
					PMesh	*mesh;
					egreal center[3], scale;
					mesh = p_rm_create(node);

					if(mesh != 0)
					{
						while(p_rm_validate(mesh) && p_rm_drawable(mesh) == FALSE)
							p_rm_service(mesh, e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node,  0)));
					//	scale = p_rm_compute_bounding_box(node, e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node,  0)), center, NULL);
						scale = e_nsg_get_size(node);
						e_nsg_get_center(node, center);
						glPushMatrix();
				/*		glScaled(0.15 / scale, 0.15 / scale, 0.15 / scale);
						glTranslated(-center[0], -center[1], -center[2]);
						glRotated(t++, 1, 1, 1);
				*/		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
						glEnableClientState(GL_NORMAL_ARRAY);
						glEnableClientState(GL_VERTEX_ARRAY);
		#ifdef E_GEOMETRY_REAL_PRESISSION_64_BIT
						glVertexPointer(3, GL_DOUBLE, 0, p_rm_get_vertex(mesh));
						glNormalPointer(GL_DOUBLE, 0 , p_rm_get_normal(mesh));
		#endif
		#ifdef E_GEOMETRY_REAL_PRESISSION_32_BIT
						glVertexPointer(3, GL_FLOAT, 0, p_rm_get_vertex(mesh));
						glNormalPointer(GL_FLOAT, 0 , p_rm_get_normal(mesh));
		#endif
						glEnable(GL_LIGHTING);
					//	glColor3f(red, green, blue);

						glDrawElements(GL_TRIANGLES, p_rm_get_ref_length(mesh), GL_UNSIGNED_INT, p_rm_get_reference(mesh));
						glDisableClientState(GL_COLOR_ARRAY);
						glDisableClientState(GL_NORMAL_ARRAY);
						glDisableClientState(GL_TEXTURE_COORD_ARRAY);
						glDisable(GL_LIGHTING);
						glDisable(GL_BLEND);
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						glPopMatrix();
						p_rm_destroy(mesh);
					}
				}
#endif
			break;
			case V_NT_MATERIAL :
				if(node != NULL)
					co_draw_material(node);
			break;
			case V_NT_BITMAP :

				if(node != NULL)
				{
					glPushMatrix();
					glScalef(0.5, 0.5, 0.5);
					co_draw_bitmap(node);
					glPopMatrix();
				}
				co_vng_dust_planet();
			break;
			case V_NT_TEXT :
				co_vng_geo_planet();
			break;
			case V_NT_CURVE :
				co_vng_dust_planet();

			break;
		}
	}
}



void co_center(float x, float y)
{
	view_cam_pos[0] = x;
	view_cam_pos[1] = y;
	view_cam_pos[2] = 4;
	view_cam_speed = 0.9;
}

extern boolean co_serch_node(ENode *node, char *search);


typedef enum{
	COIM_NONE,
	COIM_MOVE,
	COIM_POPUP,
	COIM_CREATE,
	COIM_LINK
}COInteractMode;

extern boolean co_handle_head(BInputState *input, ENode *node, float *length);
extern void co_material_compute(uint lines);

boolean get_stop_sign(void);

extern void co_init_game(uint count);
extern void co_end_game(void);
extern void co_create_pebel(uint type, float pos_x, float pos_y, float scale);
extern boolean co_is_game_active(void);
extern void co_play_game(BInputState *input);

extern boolean co_draw_ships(BInputState *input, boolean clickable);

void co_input_handler(BInputState *input, void *user_pointer)
{
	static COInteractMode mode = COIM_NONE;
	static VNodeID active = -1;
	static VNodeType link_id;
	static float clear_color = 1, create_move = 0, popup_move = 0, type_count, create_scroll = 0;
//	char *names[7] = {"OBJECT", "GEOMETRY", "MATERIAL", "BITMAP", "TEXT", "PARTICLE", "CURVE"};
	ENode *node;
	COVerseNode *co_node;
	VNodeType type;
	float pos[3];

	if(input->mode == BAM_MAIN)
	{
		verse_callback_update(3000);
		co_material_compute(1);
		co_vng_update_time();
#ifdef PERSUADE_H
		p_task_compute(2);
#endif
		return;
	}

	if(input->mode == BAM_DRAW)
	{
		glDisable(GL_DEPTH_TEST);
//		glEnable(GL_LINE_SMOOTH);		
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	}

	if(co_is_game_active())
	{
		co_play_game(input);
		return;
	}else if(co_draw_ships(input, mode == COIM_NONE))
	{
		uint i = 0;
		for(type = 0; type < V_NT_NUM_TYPES; type++)
			for(node = e_ns_get_node_next(0, 0, type); node != 0; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, type))
				if((co_node = e_ns_get_custom_data(node, CONNECTOR_ENOUGH_SLOTT)) != NULL && co_node->hidden != TRUE)
					i++;
		co_init_game(i);
		for(type = 0; type < V_NT_NUM_TYPES; type++)
			for(node = e_ns_get_node_next(0, 0, type); node != 0; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, type))
				if((co_node = e_ns_get_custom_data(node, CONNECTOR_ENOUGH_SLOTT)) != NULL && co_node->hidden != TRUE)
					co_create_pebel(type, co_get_view_x(co_node->pos_x), co_get_view_y(co_node->pos_y), 1 / view_cam_pos[2]);
		view_cam_pos[2] = 0;
		view_cam_speed = 0.01;
	}
	co_handle_view(input, pos, mode == COIM_NONE);
	

	if(input->mode == BAM_DRAW)
	{
		glPushMatrix();

		if((node = e_ns_get_node(0, active)) != NULL && (co_node = e_ns_get_custom_data(node, CONNECTOR_ENOUGH_SLOTT)) != NULL)
			glTranslatef(pos[0] * (1 - popup_move) - popup_move * (co_node->pos_x + 1), pos[1] * (1 - popup_move) -  popup_move * co_node->pos_y, -pos[2] * (1 - popup_move) - popup_move);
		else
			glTranslatef(pos[0], pos[1], -pos[2]);
#ifdef PERSUADE_H
		p_draw_scene();
#endif
	}


	for(type = 0; type < V_NT_NUM_TYPES; type++)
	{
		type_count = create_scroll - 1;
		for(node = e_ns_get_node_next(0, 0, type); node != 0; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, type))
		{
			type_count++;
			co_node = e_ns_get_custom_data(node, CONNECTOR_ENOUGH_SLOTT);
			if(co_node != NULL && co_node->hidden != TRUE)
			{
				if(input->mode == BAM_DRAW)
				{
					glPushMatrix();
					glTranslatef(co_get_pos_x(((float)type - ((float)(V_NT_NUM_TYPES) / 2) + 0.5) / V_NT_NUM_TYPES * 1.8) * create_move + co_node->pos_x * (1 - create_move),
						co_get_pos_y((float)type_count * -0.2) * create_move + co_node->pos_y * (1 - create_move), 0);
					glScalef(view_cam_pos[2] / 5 * create_move + (1 - create_move), view_cam_pos[2] / 5 * create_move + (1 - create_move), view_cam_pos[2] / 5 * create_move + (1 - create_move));
					if(mode == COIM_CREATE)
					{
						glPushMatrix();
						glScalef(4, 4, 4);
						sui_draw_text(sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, e_ns_get_node_name(node)) * -0.5, 0, SUI_T_SIZE, SUI_T_SPACE, e_ns_get_node_name(node), 0, 0, 0);
						glPopMatrix();
					}
					co_node_draw(node, type, FALSE);
				}else
				{
					if(mode == COIM_NONE)
					{
						if(input->last_mouse_button[0] == FALSE && input->mouse_button[0] == TRUE)
						{
							if((co_get_pos_x(input->pointer_x) - co_node->pos_x) * (co_get_pos_x(input->pointer_x) - co_node->pos_x) + (co_get_pos_y(input->pointer_y) - co_node->pos_y) * (co_get_pos_y(input->pointer_y) - co_node->pos_y) < 0.4 * 0.4)
							{
								active = e_ns_get_node_id(node);
								mode = COIM_MOVE;
							}
						}
						if(input->last_mouse_button[2] == FALSE && input->mouse_button[2] == TRUE)
						{
							if((co_get_pos_x(input->pointer_x) - co_node->pos_x) * (co_get_pos_x(input->pointer_x) - co_node->pos_x) + (co_get_pos_y(input->pointer_y) - co_node->pos_y) * (co_get_pos_y(input->pointer_y) - co_node->pos_y) < 0.4 * 0.4)
							{
								active = e_ns_get_node_id(node);
								mode = COIM_POPUP;
							}
						}
						if(input->last_mouse_button[1] == FALSE && input->mouse_button[1] == TRUE)
						{
							if((co_get_pos_x(input->pointer_x) - co_node->pos_x) * (co_get_pos_x(input->pointer_x) - co_node->pos_x) + (co_get_pos_y(input->pointer_y) - co_node->pos_y) * (co_get_pos_y(input->pointer_y) - co_node->pos_y) < 0.4 * 0.4)
							{
								co_center(co_node->pos_x, co_node->pos_y);
							}
						}
					}
					if(mode == COIM_MOVE && active == e_ns_get_node_id(node))
					{
						if(0.025 * 0.025 < (input->pointer_x - input->click_pointer_x) * (input->pointer_x - input->click_pointer_x) + (input->pointer_y - input->click_pointer_y) * (input->pointer_y - input->click_pointer_y))
						{
							co_node->pos_x = co_get_pos_x(input->pointer_x);
							co_node->pos_y = co_get_pos_y(input->pointer_y);
						}
						if(input->last_mouse_button[0] == TRUE && input->mouse_button[0] == FALSE)
						{
							mode = COIM_NONE;
							if(0.025 * 0.025 > (input->pointer_x - input->click_pointer_x) * (input->pointer_x - input->click_pointer_x) + (input->pointer_y - input->click_pointer_y) * (input->pointer_y - input->click_pointer_y))
								mode = COIM_POPUP;
						}
					}
				}
				if(type == V_NT_OBJECT && mode != COIM_CREATE && mode != COIM_POPUP)
				{
					uint i = 0;
					EObjLink *link;
					for(link = e_nso_get_next_link(node, 0); link != NULL; link = e_nso_get_next_link(node, e_nso_get_link_id(link) + 1))
					{
						COVerseNode *co_link = NULL;
						ENode *link_node;
						if((link_node = e_ns_get_node(0, e_nso_get_link_node(link))) != NULL)
							co_link = e_ns_get_custom_data(link_node, CONNECTOR_ENOUGH_SLOTT);

						if(co_link != NULL && co_link->hidden == TRUE)
						{
							co_link->hidden = FALSE;
							co_link->pos_x = co_node->pos_x + 2;
							co_link->pos_y = co_node->pos_y + 2 - i;
						}

						if((co_link != NULL && co_vng_render_link(input, i, TRUE, -co_node->pos_x + co_get_pos_x(input->pointer_x), -co_node->pos_y + co_get_pos_y(input->pointer_y), -co_node->pos_x + co_link->pos_x, -co_node->pos_y + co_link->pos_y, e_nso_get_link_name(link)))
						||	(co_link == NULL && co_vng_render_link(input, i, FALSE, -co_node->pos_x + co_get_pos_x(input->pointer_x), -co_node->pos_y + co_get_pos_y(input->pointer_y), 0, 0, e_nso_get_link_name(link))))
						{
							mode = COIM_LINK;
							link_id = e_nso_get_link_id(link);
							active = e_ns_get_node_id(node);
						}
						i++;
					}
					if(co_vng_render_link(input, i, FALSE, -co_node->pos_x + co_get_pos_x(input->pointer_x), -co_node->pos_y + co_get_pos_y(input->pointer_y), 0, 0, "New"))
					{
						mode = COIM_LINK;
						link_id = -1;
						active = e_ns_get_node_id(node);
					}
				}
				if(type == V_NT_MATERIAL && mode != COIM_CREATE && mode != COIM_POPUP)
				{
					VNMFragmentID i;
					uint j = 0;
					VMatFrag *frag, f;
	
					for(i = e_nsm_get_fragment_next(node, 0); i != (VNMFragmentID)-1; i = e_nsm_get_fragment_next(node, i + 1))
					{
						if(VN_M_FT_TEXTURE == e_nsm_get_fragment_type(node, i))
						{
							COVerseNode *co_link = NULL;
							ENode *link_node;
							frag = e_nsm_get_fragment(node, i);
							if((link_node = e_ns_get_node(0, frag->texture.bitmap)) != NULL)
								co_link = e_ns_get_custom_data(link_node, CONNECTOR_ENOUGH_SLOTT);

							if(co_link != NULL && co_link->hidden == TRUE)
							{
								co_link->hidden = FALSE;
								co_link->pos_x = co_node->pos_x + 2;
								co_link->pos_y = co_node->pos_y + 2 - i;
							}

							if((co_link != NULL && co_vng_render_link(input, j, TRUE, -co_node->pos_x + co_get_pos_x(input->pointer_x), -co_node->pos_y + co_get_pos_y(input->pointer_y), -co_node->pos_x + co_link->pos_x, -co_node->pos_y + co_link->pos_y, "Texture"))
							||	(co_link == NULL && co_vng_render_link(input, j, FALSE, -co_node->pos_x + co_get_pos_x(input->pointer_x), -co_node->pos_y + co_get_pos_y(input->pointer_y), 0, 0, "Texture")))
							{
								mode = COIM_LINK;
								link_id = i;
								active = e_ns_get_node_id(node);
							}
							j++;
						}
					}
				}
				if(mode != COIM_POPUP)
				{
					switch(co_vng_render_name(input, e_ns_get_node_name(node), co_node->pos_x, co_node->pos_y, co_get_pos_x(input->pointer_x), co_get_pos_y(input->pointer_y)))
					{
						case 1:
							verse_send_node_destroy(e_ns_get_node_id(node));
						break;
						case 2:
							co_node->hidden = TRUE;
						break;
						case 3:
						{
						/*	uint i;
							ENode *link;
							for(i = 0; i < V_NT_NUM_TYPES; i++)
							{
								e_nso_get_link(node, i);
								link = e_ns_get_node(0, e_nso_get_link(node, i));
								if(link != NULL && (co_node = e_ns_get_custom_data(link, CONNECTOR_ENOUGH_SLOTT)) != NULL)
									co_node->hidden = TRUE;
							}*/
						}
						break;
						case 4:
							co_node->hidden = TRUE;
						break;
					}
				}
				if(input->mode == BAM_DRAW)
					glPopMatrix();
			}else if(input->mode == BAM_DRAW && mode == COIM_CREATE)
			{
				glPushMatrix();
				glTranslatef(co_get_pos_x(((float)type - ((float)(V_NT_NUM_TYPES) / 2) + 0.5) / V_NT_NUM_TYPES * 1.8),
					co_get_pos_y((float)type_count * -0.2), 0);
				glScalef(view_cam_pos[2] / 5 * create_move + (1 - create_move), view_cam_pos[2] / 5 * create_move + (1 - create_move), view_cam_pos[2] / 5 * create_move + (1 - create_move));
				co_node_draw(node, type, FALSE);
				glScalef(4, 4, 4);
				sui_draw_text(sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, e_ns_get_node_name(node)) * -0.5, 0, SUI_T_SIZE, SUI_T_SPACE, e_ns_get_node_name(node), 0.5, 0.5, 0.5);
				glPopMatrix();
			}
		}
	}


	if(mode == COIM_CREATE)
	{
		clear_color = clear_color - 0.001;
		create_move = create_move * 0.95 + 0.05;
	}else
		create_move = create_move * 0.95;
	if(mode == COIM_CREATE)
	{
		float x, y;
		y = (1 - create_scroll) / 5;
		for(type = 0; type < V_NT_NUM_TYPES; type++)
		{
			x = ((float)type - ((float)(V_NT_NUM_TYPES) / 2) + 0.5) / V_NT_NUM_TYPES * 1.8;
			if(input->mode == BAM_DRAW)
			{
				char *names[8] = {"OBJECT", "GEOMETRY", "MATERIAL", "BITMAP", "TEXT", "CURVE", "AUDIO"};
				glPushMatrix();
				glTranslatef(co_get_pos_x(x), co_get_pos_y(y), 0);
				glScalef(view_cam_pos[2], view_cam_pos[2] , view_cam_pos[2]);
				sui_draw_text(sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, "NEW") * -0.5, 0, SUI_T_SIZE, SUI_T_SPACE, "NEW", 0, 0, 0);
				sui_draw_text(sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, names[type]) * -0.5, 0.15, SUI_T_SIZE, SUI_T_SPACE, names[type], 0, 0, 0);
				glScalef(0.2, 0.2, 0.2);
				co_node_draw(NULL, type, FALSE);
				glPopMatrix();
			}else
			{
				if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE)
				{
					if((input->pointer_x - x) * (input->pointer_x - x) + (input->pointer_y - y) * (input->pointer_y - y) < 0.2 * 0.2 * 0.4 * 0.4)
					{
						verse_send_node_create(-1, type, 0);
						mode = COIM_NONE;
					}
				}
			}
		}

		if(input->mode != BAM_DRAW) /*&& co_node_create_draw(input)*/
		{
			if(input->last_mouse_button[0] == TRUE)
				if(0.05 * 0.05 < (input->pointer_x - input->click_pointer_x) * (input->pointer_x - input->click_pointer_x) + (input->pointer_y - input->click_pointer_y) * (input->pointer_y - input->click_pointer_y))
					create_scroll -= input->delta_pointer_y * 5;
			if(0.05 * 0.05 > (input->pointer_x - input->click_pointer_x) * (input->pointer_x - input->click_pointer_x) + (input->pointer_y - input->click_pointer_y) * (input->pointer_y - input->click_pointer_y) && input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE)
			{
				for(type = 0; type < V_NT_NUM_TYPES; type++)
				{
					type_count = create_scroll;
					for(node = e_ns_get_node_next(0, 0, type); node != 0; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, type))
					{
						co_node = e_ns_get_custom_data(node, CONNECTOR_ENOUGH_SLOTT);
						if(co_node != NULL /*&& co_search_node(node, search)*/)
						{
							x = ((float)type - ((float)(V_NT_NUM_TYPES) / 2) + 0.5) / V_NT_NUM_TYPES * 1.8;
							if((input->pointer_x - x) * (input->pointer_x - x) + (input->pointer_y - (type_count * -0.2)) * (input->pointer_y - (type_count * -0.2)) < 0.2 * 0.2 * 0.4 * 0.4)
							{
								co_node->hidden = FALSE;
								co_node->pos_x = co_get_pos_x(x);
								co_node->pos_y = co_get_pos_y(type_count * -0.2);				
							}
						}
						type_count++;
					}
				}
				mode = COIM_NONE;
			}
		}
	}
	else if(input->mode != BAM_DRAW)
	{
		if(input->mouse_button[2] == FALSE && input->last_mouse_button[2] == TRUE && mode == COIM_NONE)
			mode = COIM_CREATE;
		if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE && mode == COIM_NONE)
			if(0.025 * 0.025 > (input->pointer_x - input->click_pointer_x) * (input->pointer_x - input->click_pointer_x) + (input->pointer_y - input->click_pointer_y) * (input->pointer_y - input->click_pointer_y))
				mode = COIM_CREATE;
		if(mode == COIM_CREATE)
			create_scroll = -5 * betray_get_screen_mode(NULL, NULL, NULL) + 2;
	}
	if(mode == COIM_LINK)
	{
		if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE)
		{
			uint i;
			EObjLink *link;
			ENode *linker;
			uint32 found = -1;
			for(i = 0; i < V_NT_NUM_TYPES; i++)
			{
				for(node = e_ns_get_node_next(0, 0, i); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, i))
				{
					co_node = e_ns_get_custom_data(node, CONNECTOR_ENOUGH_SLOTT);
					if(co_node != NULL && co_node->hidden != TRUE && ((co_get_pos_x(input->pointer_x) - co_node->pos_x) * (co_get_pos_x(input->pointer_x) - co_node->pos_x) + (co_get_pos_y(input->pointer_y) - co_node->pos_y) * (co_get_pos_y(input->pointer_y) - co_node->pos_y) < 0.4 * 0.4))					
						found = co_node->node_id;
				}	
			}

			linker = e_ns_get_node(0, active);
			if(linker != NULL)
			{
				if(V_NT_OBJECT == e_ns_get_node_type(linker))
				{
					if((link = e_nso_get_link(linker, link_id)) != NULL)
						verse_send_o_link_set(active, link_id, found, e_nso_get_link_name(link), e_nso_get_link_target_id(link));
					else
						verse_send_o_link_set(active, link_id, found, "my_link", 0);
				}
				if(V_NT_MATERIAL == e_ns_get_node_type(linker))
				{
					VMatFrag *frag, f;
					frag = e_nsm_get_fragment(linker, link_id);
					if(frag != NULL)
					{
						f = *frag;
						f.texture.bitmap = found;
						verse_send_m_fragment_create(active, link_id, VN_M_FT_TEXTURE, &f);
					}
				}
			}
			mode = COIM_NONE;
		}
	}

	if(mode == COIM_POPUP)
	{
		clear_color = clear_color - 0.001;
		popup_move = popup_move * 0.95 + 0.05;
		if(input->mode == BAM_DRAW && (node = e_ns_get_node(0, active)) != NULL && (co_node = e_ns_get_custom_data(node, CONNECTOR_ENOUGH_SLOTT)) != NULL)
			glTranslatef(co_node->pos_x + 1, co_node->pos_y, 0);

		if((node = e_ns_get_node(0, active)) != NULL)
		{
			switch(e_ns_get_node_type(node))
			{
				case V_NT_OBJECT :
					if(!co_handle_object(input, node))
						mode = COIM_NONE;
					break;
				case V_NT_GEOMETRY :
					if(!co_handle_geometry(input, node))
						mode = COIM_NONE;
					break;
				case V_NT_MATERIAL :
					if(!co_handle_material(input, node))
						mode = COIM_NONE;
					break;
				case V_NT_BITMAP :
					if(!co_handle_bitmap(input, node))
						mode = COIM_NONE;
					break;
				case V_NT_TEXT :
					if(!co_handle_text(input, node))
						mode = COIM_NONE;
					break;
				case V_NT_CURVE :
					if(!co_handle_curve(input, node))
						mode = COIM_NONE;
					break;
			}
		}
	}else
		popup_move = popup_move * 0.95;
	if(input->mode == BAM_DRAW)
		glPopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void co_init_handle_verse_node(void)
{
	e_ns_set_custom_func(1, V_NT_OBJECT, verse_node_create_func);
	e_ns_set_custom_func(1, V_NT_GEOMETRY, verse_node_create_func);
	e_ns_set_custom_func(1, V_NT_MATERIAL, verse_node_create_func);
	e_ns_set_custom_func(1, V_NT_BITMAP, verse_node_create_func);
	e_ns_set_custom_func(1, V_NT_TEXT, verse_node_create_func);
	e_ns_set_custom_func(1, V_NT_CURVE, verse_node_create_func);
	e_ns_set_custom_func(1, V_NT_AUDIO, verse_node_create_func);
}
