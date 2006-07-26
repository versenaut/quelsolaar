#include "la_includes.h"

#include "la_geometry_undo.h"
#include "la_tool.h"
#include "la_projection.h"
/*
struct{
	void	*ring;
	void	*empty_star;
	void	*manipulator_star;
	void	*edge_star;
	void	*polygon_star;
	void	*browser_star;
	void	*select_star;
	char	text[48];
	void	*text_button; 
	void	*slider;
	void	*complexity_slider;
	void	*treshold_slider;
	char	*mouse[3];
	float	mouse_pos[3];
	float	popup_pos[3];
}GlobalPopupData;*/

void la_intro_draw(void *user);
void geometry_load_obj(void *user, char *file);
void geometry_save_obj(void *user, char *file);
extern void la_draw_settings_menu(BInputState *input, void *user);


void connect_type_in_func(void *user, char *text)
{
	e_vc_connect(text, "unknown", "none", NULL);
}

extern void la_edit_func(BInputState *input, void *user);

void la_pu_connect(BInputState *input, void *user)
{
	static boolean active = FALSE;
	static char connect_type_in[48];
	connect_type_in[0] = 0;

	if(e_vc_check_connected() && e_vc_check_accepted_slot(0))
	{
		betray_set_action_func(la_edit_func, NULL);
		return;
	}

	if(input->mode == BAM_MAIN)
	{
		verse_callback_update(3000);
		return;
	}

	if(input->mode == BAM_DRAW)
	{
		glClearColor(1, 1, 1, 1);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glPopMatrix();
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0, 0, -1);
		la_intro_draw(NULL);
		glDisable(GL_DEPTH_TEST);
	}
	
	if((input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE) || (input->mouse_button[1] == FALSE && input->last_mouse_button[1] == TRUE) || (input->mouse_button[2] == FALSE && input->last_mouse_button[2] == TRUE))
		active = TRUE;

	if(active)
	{
//		if(input->mode == BAM_DRAW)
//	        sw_draw_window(-0.35, -0.2, 0.7, 0.4, 0.02);
		if(e_vc_check_connected_slot(0))
		{
			if(input->mode == BAM_DRAW)
				sui_draw_text(sui_compute_text_length(0.02, 2, "CONNECTING") * -0.5, -0.35, 0.02, 2, "CONNECTING", 0, 0, 0);
		}
		else
		{
			sui_type_in(input, -0.2, -0.45, 0.4, SUI_T_SIZE, connect_type_in, 48, connect_type_in_func, NULL, 0, 0, 0);
			sui_draw_2d_line_gl(-0.2, -0.45, 0.2, -0.45, 0, 0, 0, 0.0);
			if(input->mode == BAM_DRAW)
				sui_draw_text(sui_compute_text_length(0.02, 2, "CONNECT") * -0.5, 0.35, 0.02, 2, "CONNECT", 0, 0, 0);

			if(sw_text_button(input, -0.2, -0.35 - 2 * 0.02, 0, 0.02, 0.3, "Localhost", 0, 0, 0))
				e_vc_connect("localhost", "somepass", "somepass", NULL);
			if(sw_text_button(input, 0.2, -0.35 - 2 * 0.02, 1, 0.02, 0.3, "Exit", 0, 0, 0))
			{
				sui_save_settings("co_config.cfg");
				exit(0);
			}
		}
	}

	if(input->mode == BAM_DRAW)
	{
		glPopMatrix();
		glPushMatrix();
		glPushMatrix();
		glEnable(GL_DEPTH_TEST);
	}
}

extern void geometry_save(void);
extern void geometry_load(char *file);

extern void start_macro_record(char *file);
extern void start_macro_play_back(char *file);
extern void stop_macro(void);

extern void save_node_vnf(ENode *node, uint *node_count, FILE *file);
extern void test_ref_creator(void);
extern void draw_settings_menu(BInputState *input, void *user);

/*
typedef enum{
	PU_T_TOP,
	PU_T_BOTTOM,
	PU_T_ANGLE,
	PU_T_VSLIDER,
	PU_T_HSLIDER,
	PU_T_SQUARE,
	PU_T_ICON
}SUIPUType;

typedef struct{
	SUIPUType	type;
	char		*text;
	union{
		float angle[2];
		struct{
			float pos[2];
			float length;
			double *value;
		}slider;
		struct{
			float square[4];
			void (*draw_func)(float pos_x, float pos_y, float size_x, float size_y, float red, float green, float blue);
		}square;
		struct{
			float pos[2];
			float size;
			void (*draw_func)(float pos_x, float pos_y, float red, float green, float blue);
		}icon;
	}data;
}SUIPUElement;
*/


#define MAX_NODE_DISPLAY_COUNT 16 
extern void la_close_all_holes(void);

boolean la_pu_empty(BInputState *input)
{
	SUIPUElement element[4 + MAX_NODE_DISPLAY_COUNT];
	static float x, y, static_start = 0;
	uint start = 0, ring, count = 4, i;
	ENode *node;

	start = static_start;
	element[0].type = PU_T_ANGLE;
	element[0].text = "Settings";
	element[0].data.angle[0] = -45;
	element[0].data.angle[1] = 45;
	element[1].type = PU_T_ANGLE;
	element[1].text = "Undo";
	element[1].data.angle[0] = 45;
	element[1].data.angle[1] = 135;
	element[2].type = PU_T_ANGLE;
	element[2].text = "Redo";
	element[2].data.angle[0] = 225;
	element[2].data.angle[1] = 315;
	element[3].type = PU_T_BOTTOM;
	element[3].text = "NEW GEOMETRY";
	count = 4;

	
	node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY);
	for(i = 1; node != NULL && i < start; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
		i++;

	if(start != 0)
	{
		if(y - 0.175 < input->pointer_y)
			static_start -= betray_get_delta_time() * 10 * (0.2 + input->pointer_y + (y - 0.175));
		element[count].type = PU_T_BOTTOM;
		element[count].text = "More...";
		count++;
	}


	for(; node != NULL && count < MAX_NODE_DISPLAY_COUNT + 3; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
	{
		element[count].type = PU_T_BOTTOM;
		element[count].text = e_ns_get_node_name(node);
		count++;
	}
	if(count == MAX_NODE_DISPLAY_COUNT + 3 && node != NULL)
	{
		if(y - 0.05 * MAX_NODE_DISPLAY_COUNT - 0.075 > input->pointer_y)
			static_start += betray_get_delta_time() * 10 * (0.2 + (y - 0.05 * MAX_NODE_DISPLAY_COUNT - 0.075) - input->pointer_y);
		element[count].type = PU_T_BOTTOM;
		element[count].text = "More...";
		count++;
	}
	if(input->mode == BAM_DRAW)
	{
		glDisable(GL_DEPTH_TEST);
		glPopMatrix();
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0, 0, -1);
	}
	if(input->mouse_button[2] == TRUE && input->last_mouse_button[2] == FALSE)
	{
		x = input->pointer_x;
		y = input->pointer_y;
	}
	ring = sui_draw_popup(input, x, y, element, count, 2, 0);
	switch(ring)
	{
		case 0 :
			betray_set_action_func(la_draw_settings_menu, NULL);
/*			geometry_load_obj(NULL, "./girl_hi.obj");
			la_close_all_holes();
			geometry_save_obj(NULL,"cafe.obj");
*/		break;
		case 1 :
			udg_undo_geometry();
		break;
		case 2 :
			udg_redo_geometry();
		break;
		case 3 :
			udg_create_new_modeling_node();
		break;
	}


	node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY);
	for(i = 1; node != NULL && i < start; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
		i++;
	count = 4;
	if(ring > 3 && ring != -1)		
		for(; node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
			if(ring == count++)
				udg_set_modeling_node(e_ns_get_node_id(node));

//	sw_drawbackground();
	if(input->mode == BAM_DRAW)
	{
		glPopMatrix();
		glPushMatrix();
		glPushMatrix();
		glEnable(GL_DEPTH_TEST);
	}
	if(ring == 3)
		return TRUE;
	else
		return FALSE;

}

void la_pu_vertex(BInputState *input, uint vertex)
{	
/*	static double value;
	double pos[3], output[3];
	glDisable(GL_DEPTH_TEST);
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();
	udg_get_vertex_pos(pos, vertex);
	p_get_projection_screen(output, pos[0], pos[1], pos[2]);
	glTranslatef(-output[0], -output[1], -1);
	input->pointer_x += output[0];
	input->pointer_y += output[1];
	input->click_pointer_x += output[0];
	input->click_pointer_y += output[2];
	input->pointer_x -= output[0];
	input->pointer_y -= output[1];
	input->click_pointer_x -= output[0];
	input->click_pointer_y -= output[2];
	udg_set_select(vertex, value);
	glPopMatrix();
	glPushMatrix();
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);*/
}


void la_pu_manipulator(BInputState *input)
{
	SUIPUElement element[23];
	static float x, y;
	uint ring;
	element[0].type = PU_T_ANGLE;
	element[0].text = "Reverse";
	element[0].data.angle[0] = 45;
	element[0].data.angle[1] = 75;
	element[1].type = PU_T_ANGLE;
	element[1].text = "Duplicate";
	element[1].data.angle[0] = 75;
	element[1].data.angle[1] = 105;
	element[2].type = PU_T_ANGLE;
	element[2].text = "Detach";
	element[2].data.angle[0] = 105;
	element[2].data.angle[1] = 135;
	element[3].type = PU_T_ANGLE;
	element[3].text = "Collapse";
	element[3].data.angle[0] = 45 + 180;
	element[3].data.angle[1] = 75 + 180;
	element[4].type = PU_T_ANGLE;
	element[4].text = "Full Crease";
	element[4].data.angle[0] = 75 + 180;
	element[4].data.angle[1] = 105 + 180;
	element[5].type = PU_T_ANGLE;
	element[5].text = "No Crease";
	element[5].data.angle[0] = 105 + 180;
	element[5].data.angle[1] = 135 + 180;

	element[6].type = PU_T_TOP;
	element[6].text = "Flip";
	element[7].type = PU_T_TOP;
	element[7].text = "Smooth Selection";
	element[8].type = PU_T_TOP;
	element[8].text = "Delete";
	element[9].type = PU_T_TOP;
	element[9].text = "Mirror";

	element[10].type = PU_T_TOP;
	element[10].text = "Flatten";
	element[11].type = PU_T_BOTTOM;
	element[11].text = "Slice";
	element[12].type = PU_T_BOTTOM;
	element[12].text = "Slice Off";
	element[13].type = PU_T_BOTTOM;
	element[13].text = "Weld";
	element[14].type = PU_T_BOTTOM;
	element[14].text = "Center Geometry";
	element[15].type = PU_T_BOTTOM;
	element[15].text = "Center Manipulator";
	element[16].type = PU_T_BOTTOM;
	element[16].text = "Create Tag";
	element[17].type = PU_T_BOTTOM;
	element[17].text = "Triangulate";
	element[18].type = PU_T_BOTTOM;
	element[18].text = "Auto Crease";
	element[19].type = PU_T_BOTTOM;
	element[19].text = "Find Quads";

	element[20].type = PU_T_BOTTOM;
	element[20].text = "Cut";
	element[21].type = PU_T_BOTTOM;
	element[21].text = "Copy";
	element[22].type = PU_T_BOTTOM;
	element[22].text = "Paste";

	if(input->mode == BAM_DRAW)
	{
		glDisable(GL_DEPTH_TEST);
		glPopMatrix();
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0, 0, -1);
	}
	if(input->mouse_button[2] == TRUE && input->last_mouse_button[2] == FALSE)
	{
		x = input->pointer_x;
		y = input->pointer_y;
	}
	ring = sui_draw_popup(input, x, y, element, 23, 2, 0);
	switch(ring)
	{
		case 0 :
		{
			uint32 i, vertex_count;
			double *vertex;
			udg_get_geometry(&vertex_count, NULL, &vertex, NULL, NULL);
			for(i = 0; i < vertex_count; i++)
				if(vertex[i * 3] != V_REAL64_MAX)
					udg_set_select(i, 1 - udg_get_select(i));
		}
		break;
		case 1 :
			la_t_duplicate_selected_polygons();
		break;
		case 2 :
			la_t_detach_selected_polygons();
		break;
		case 3 :
			la_t_collapse_selected_vertexes();
		break;
		case 4 :
			la_t_crease_selected(-1);
		break;
		case 5 :
			la_t_crease_selected(0);
		break;
		case 6 :
			la_t_flip_selected_polygons();
		break;
		case 7 :
			la_t_smooth_select();
		break;
		case 8 :
			la_t_delete_selection();
		break;
		case 9 :
		{
			double pos[3], vector[3];
			la_t_tm_get_pos(pos);
			la_t_tm_get_vector(vector);
			la_t_mirror(pos, vector);
			la_t_flip_selected_polygons();
		}
		break;
		case 10 :
		{
			double pos[3], vector[3];
			la_t_tm_get_pos(pos);
			la_t_tm_get_vector(vector);
			la_t_flatten(pos, vector);
		}
		break;
		case 11 :
		{
			double pos[3], vector[3];
			la_t_tm_get_pos(pos);
			la_t_tm_get_vector(vector);
			la_t_slice(pos, vector, FALSE);
		}
		break;
		case 12 :
		{
			double pos[3], vector[3];
			la_t_tm_get_pos(pos);
			la_t_tm_get_vector(vector);
			la_t_slice(pos, vector, TRUE);
		}
		break;
		case 13 :
		{
			la_t_weld_selected_vertexes();
		}
		break;
		case 14 :
		{
			la_t_center_geometry();
		}
		break;
		case 15 :
		{
			la_t_center_manipulator();
		}
		break;
		case 16 :
		{
			double pos[3];
			la_t_tm_get_pos(pos);
			udg_create_tag(pos);
		}
		break;
		case 17 :
		{
			la_t_poly_triangulate();
		}
		break;
		case 18 :
		{
			la_t_poly_auto_crease();
		}
		break;
		case 19 :
		{
			la_t_poly_find_quads();
		}
		break;
		case 20 :
		{
			double pos[3];
			la_t_tm_get_pos(pos);
			la_t_copy(pos);
			la_t_delete_selection();
		}
		break;
		case 21 :
		{
			double pos[3];
			la_t_tm_get_pos(pos);
			la_t_copy(pos);

		}
		break;
		case 22 :
		{
			double pos[3];
			la_t_tm_get_pos(pos);
			la_t_paste(pos);
		}
		break;
	}
	if(input->mode == BAM_DRAW)
	{
		glPopMatrix();
		glPushMatrix();
		glPushMatrix();
		glEnable(GL_DEPTH_TEST);
	}
}

void la_pu_edge(BInputState *input, uint *edge)
{
	SUIPUElement element[6];
	static float x, y;
	uint ring;
	element[0].type = PU_T_ANGLE;
	element[0].text = "Select Border";
	element[0].data.angle[0] = 0;
	element[0].data.angle[1] = 60;
	element[1].type = PU_T_ANGLE;
	element[1].text = "Revolve";
	element[1].data.angle[0] = 60;
	element[1].data.angle[1] = 120;
	element[2].type = PU_T_ANGLE;
	element[2].text = "Tube";
	element[2].data.angle[0] = 120;
	element[2].data.angle[1] = 180;
	element[3].type = PU_T_ANGLE;
	element[3].text = "Select Hull";
	element[3].data.angle[0] = 180;
	element[3].data.angle[1] = 240;
	element[4].type = PU_T_ANGLE;
	element[4].text = "Collapse";
	element[4].data.angle[0] = 240;
	element[4].data.angle[1] = 300;
	element[5].type = PU_T_ANGLE;
	element[5].text = "Measure Grid";
	element[5].data.angle[0] = 300;
	element[5].data.angle[1] = 360;

	if(input->mode == BAM_DRAW)
	{
		glDisable(GL_DEPTH_TEST);
		glPopMatrix();
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0, 0, -1);
	}
	if(input->mouse_button[2] == TRUE && input->last_mouse_button[2] == FALSE)
	{
		x = input->pointer_x;
		y = input->pointer_y;
	}
	ring = sui_draw_popup(input, x, y, element, 6, 2, 0);
	switch(ring)
	{
		case 0 :
			la_t_select_open_edge();
		break;
		case 1 :
			la_t_revolve(edge, sui_get_setting_int("REVOLVE_INTERSECTIONS", 8));
		break;
		case 2 :
			la_t_tube(edge, sui_get_setting_int("REVOLVE_INTERSECTIONS", 8));
		break;
		case 3 :
			la_t_select_hull(edge);
		break;
		case 4 :
			la_t_collapse_two_vertexes(edge[0], edge[1]);
		break;
		case 5 :
		{
			double a[3], b[3];
			udg_get_vertex_pos(a, edge[0]);
			udg_get_vertex_pos(b, edge[1]);
			a[0] -= b[0];
			a[1] -= b[1];
			a[2] -= b[2];
			p_set_grid_size(sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]));
            
            la_t_tm_get_pos(b);
			udg_set_grid_snap(b, sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]));
		}
		break;
	}
	if(input->mode == BAM_DRAW)
	{
		glPopMatrix();
		glPushMatrix();
		glPushMatrix();
		glEnable(GL_DEPTH_TEST);
	}
}




void la_pu_polygon(BInputState *input, uint polygon)
{
	SUIPUElement element[8];
	static float x, y;
	uint ring;
	element[0].type = PU_T_ANGLE;
	element[0].text = "Mirror";
	element[0].data.angle[0] = 0;
	element[0].data.angle[1] = 360.0 / 7.0;
	element[1].type = PU_T_ANGLE;
	element[1].text = "Flatten";
	element[1].data.angle[0] = 360.0 / 7.0;
	element[1].data.angle[1] = 360.0 / 7.0 * 2.0;
	element[2].type = PU_T_ANGLE;
	element[2].text = "Fill Selection";
	element[2].data.angle[0] = 360.0 / 7.0 * 2.0;
	element[2].data.angle[1] = 360.0 / 7.0 * 3.0;
	element[3].type = PU_T_ANGLE;
	element[3].text = "Deploy";
	element[3].data.angle[0] = 360.0 / 7.0 * 3.0;
	element[3].data.angle[1] = 360.0 / 7.0 * 4.0;
	element[4].type = PU_T_ANGLE;
	element[4].text = "Slice";
	element[4].data.angle[0] = 360.0 / 7.0 * 4.0;
	element[4].data.angle[1] = 360.0 / 7.0 * 5.0;
	element[5].type = PU_T_ANGLE;
	element[5].text = "Slice Off";
	element[5].data.angle[0] = 360.0 / 7.0 * 5.0;
	element[5].data.angle[1] = 360.0 / 7.0 * 6.0;
	element[6].type = PU_T_ANGLE;
	element[6].text = "Split";
	element[6].data.angle[0] = 360.0 / 7.0 * 6.0;
	element[6].data.angle[1] = 360;

	element[7].type = PU_T_ANGLE;
	element[7].text = "Splt";
	element[7].data.angle[0] = 360.0 - 1;
	element[7].data.angle[1] = 360;




	if(input->mode == BAM_DRAW)
	{
		glDisable(GL_DEPTH_TEST);
		glPopMatrix();
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0, 0, -1);
	}
	if(input->mouse_button[2] == TRUE && input->last_mouse_button[2] == FALSE)
	{
		x = input->pointer_x;
		y = input->pointer_y;
	}
	ring = sui_draw_popup(input, x, y, element, 7, 2, 0);
	switch(ring)
	{
		case 0 :
		{
			uint *ref;
			double origo[3], vector[3];
			udg_get_geometry(NULL, NULL, NULL, &ref, NULL);
			la_t_face_vector(origo, vector, ref[polygon * 4], ref[polygon * 4 + 1], ref[polygon * 4 + 2]);
			la_t_mirror(origo, vector);
			la_t_flip_selected_polygons();
		}
		break;
		case 1 :
		{
			uint *ref;
			double origo[3], vector[3];
			udg_get_geometry(NULL, NULL, NULL, &ref, NULL);
			la_t_face_vector(origo, vector, ref[polygon * 4], ref[polygon * 4 + 1], ref[polygon * 4 + 2]);
			la_t_flatten(origo, vector);
		}
		break;
		case 2 :
			la_t_polygon_select_fill(polygon);
		break;
		case 3 :
			la_t_deploy(polygon);
		break;
		case 4 :
		{
			uint *ref;
			double origo[3], vector[3];
			udg_get_geometry(NULL, NULL, NULL, &ref, NULL);
			la_t_face_vector(origo, vector, ref[polygon * 4], ref[polygon * 4 + 1], ref[polygon * 4 + 2]);
			la_t_slice(origo, vector, FALSE);
		}
		break;
		case 5 :
		{
			uint *ref;
			double origo[3], vector[3];
			udg_get_geometry(NULL, NULL, NULL, &ref, NULL);
			la_t_face_vector(origo, vector, ref[polygon * 4], ref[polygon * 4 + 1], ref[polygon * 4 + 2]);
			la_t_slice(origo, vector, TRUE);
		}
		break;
		case 6 :
			la_t_poly_spliter(polygon);
		break;


		
	}
//	sw_drawbackground();
	if(input->mode == BAM_DRAW)
	{
		glPopMatrix();
		glPushMatrix();
		glPushMatrix();
		glEnable(GL_DEPTH_TEST);
	}
}



uint la_pu_select(BInputState *input)
{
	SUIPUElement element[4];
	uint ring;
	element[0].type = PU_T_ANGLE;
	element[0].text = "Select";
	element[0].data.angle[0] = 0;
	element[0].data.angle[1] = 90;
	element[1].type = PU_T_ANGLE;
	element[1].text = "Deselect";
	element[1].data.angle[0] = 90;
	element[1].data.angle[1] = 180;
	element[2].type = PU_T_ANGLE;
	element[2].text = "Sub";
	element[2].data.angle[0] = 180;
	element[2].data.angle[1] = 270;
	element[3].type = PU_T_ANGLE;
	element[3].text = "Add";
	element[3].data.angle[0] = 270;
	element[3].data.angle[1] = 360;
	if(input->mode == BAM_DRAW)
	{
		glDisable(GL_DEPTH_TEST);
		glPopMatrix();
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0, 0, -1);
	}
	ring = sui_draw_popup(input, input->click_pointer_x, input->click_pointer_y, element, 4, 0, 0);
	if(input->mode == BAM_DRAW)
	{
		glPopMatrix();
		glPushMatrix();
		glPushMatrix();
		glEnable(GL_DEPTH_TEST);
	}
	return ring;
}
