
#include "la_includes.h"
#include "la_geometry_undo.h"
#include "la_projection.h"

extern void parce_input(BInputState *input, void *user);

void layer_name_func(void *user, char *text)
{
	ENode *node;
	if((node = e_ns_get_node(0, udg_get_modeling_node())) != NULL &&
	   (node = e_ns_get_node(0, e_nso_get_link_id(e_nso_get_link(node, V_NT_GEOMETRY)))) != NULL) 
	{
		EGeoLayer *layer;
		if((layer = e_nsg_get_layer_by_name(node, text)) != NULL)
		{
			if(VN_G_LAYER_VERTEX_XYZ == e_nsg_get_layer_type(layer))
				udg_set_modeling_layer(text);
		}else
		{
			verse_send_g_layer_create(e_ns_get_node_id(node), -1, text, VN_G_LAYER_VERTEX_XYZ, 0, 0);
			udg_set_modeling_layer(text);
		}
	}
}

void geometry_load_obj(void *user, char *file);
void geometry_save_obj(void *user, char *file);

void draw_settings_menu(BInputState *input, void *user)
{
    double a;
	float pos[] = {-0.25, 0, 0};
	static boolean complexity = FALSE, min = FALSE, max = FALSE, screen_mode = FALSE;
	static uint choosen = 0, old = 0;
	boolean active;

	if(input->mode == BAM_MAIN)
	{
		verse_callback_update(0);
		return;
	}

	if(input->mode == BAM_DRAW)
	{
		glPopMatrix();
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0, 0, -1);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST); 
	    sw_draw_window(0, 0, 0.4, 0.6, 0.02);
//		sui_draw_text("SETTINGS", SUITM_CENTER_SPACED, NULL, NULL, 0, 0, 0.5 , 0, sui_get_material(SUIM_BLACK_FONT));
		sui_draw_text(0 - 0.5 * sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, "SETTINGS"), 0, SUI_T_SIZE, SUI_T_SPACE, "SETTINGS", 0, 0, 0);
    }
/*
	sui_set_setting_int("DISPLAY_SILLY_FLARES", sw_symb_check_box(input, -0.25, 0.4, sui_get_setting_int("DISPLAY_SILLY_FLARES", TRUE)));
	sui_draw_text("Dispaly Flares", SUITM_NORMAL, NULL, NULL, 0, -0.15, 0.4, 0, sui_get_material(SUIM_BLACK_FONT));

	sui_set_setting_int("RENDER_AS_SDS",sw_symb_check_box(input, -0.25, 0.3, sui_get_setting_int("RENDER_AS_SDS", TRUE)));
	sui_draw_text("Dispaly Subdivision", SUITM_NORMAL, NULL, NULL, 0, -0.15, 0.3, 0, sui_get_material(SUIM_BLACK_FONT));

	sui_set_setting_int("WINDOW_FULLSCREEN",sw_symb_check_box(input, -0.25, 0.2, sui_get_setting_int("WINDOW_FULLSCREEN", FALSE)));
	sui_draw_text("Fullscreen", SUITM_NORMAL, NULL, NULL, 0, -0.15, 0.2, 0, sui_get_material(SUIM_BLACK_FONT));

	sui_set_setting_int("RENDER_AS_SDS",sw_symb_check_box(input, -0.25, 0.0, sui_get_setting_int("RENDER_AS_SDS", TRUE)));
	sui_draw_text("Dispaly Subdivision", SUITM_NORMAL, NULL, NULL, 0, -0.15, 0.0, 0, sui_get_material(SUIM_BLACK_FONT));

*/

//	boolean udg_update_geometry(void)

//	if(sw_text_button_center(input, SUIMC_BLACK, 0, -0.5, "OK"))
//		betray_set_action_func(la_edit_func, NULL);

//	if(udg_update_geometry())
//		betray_set_action_func(parce_input, NULL);
/*
	sui_draw_text("Geometry complexity", SUITM_NORMAL, NULL, NULL, 0, pos[0] + 0.1, -0.1 , 0, sui_get_material(SUIM_BLACK_FONT));
	sui_draw_text("Min Tesselation", SUITM_NORMAL, NULL, NULL, 0, pos[0] + 0.1, -0.2 , 0, sui_get_material(SUIM_BLACK_FONT));
	sui_draw_text("Max Tesselation", SUITM_NORMAL, NULL, NULL, 0, pos[0] + 0.1, -0.3 , 0, sui_get_material(SUIM_BLACK_FONT));

	a = sui_get_setting_double("GEOMETRY_COMPLEXITY", 40) / 100;
	pos[1] = -0.1;
	sw_draw_background_point(pos);
	if(complexity = sw_dir_slider_double_alt(input, complexity, 8, pos, &a))
		sui_set_setting_double("GEOMETRY_COMPLEXITY", a * 100);
	pos[1] = -0.2;
	a = ((double)sui_get_setting_int("MIN_TESS_LEVEL", 1) - 1.0) / 5.0;
	sw_draw_background_point(pos);
	if(min = sw_dir_slider_double_alt(input, min, 5, pos, &a))
		sui_set_setting_int("MIN_TESS_LEVEL", (double)(a * 5.0 + 1.0));
	pos[1] = -0.3;
	a = ((double)sui_get_setting_int("MAX_TESS_LEVEL", 1) - 1.0) / 5.0;
	sw_draw_background_point(pos);
	if(max = sw_dir_slider_double_alt(input, max, 5, pos, &a))
		sui_set_setting_int("MAX_TESS_LEVEL", (double)(a * 5.0 + 1.0));
*/
//	void done_func(void *user)
/*
		static boolean layer_mode = FALSE;
		EGeoLayer *layer;
		char **names;
		uint count = 0;
		ENode *node;
		node = e_ns_get_node(0, udg_get_modeling_node());

		for(layer = e_nsg_get_layer_next(node, 0); layer != NULL; layer = e_nsg_get_layer_next(node, e_nsg_get_layer_id(layer) + 1))
			if(VN_G_LAYER_VERTEX_XYZ == e_nsg_get_layer_type(layer))
				count++;
		names = malloc((sizeof *names) * count);
		count = 0;
		for(layer = e_nsg_get_layer_next(node, 0);layer != NULL; layer = e_nsg_get_layer_next(node, e_nsg_get_layer_id(layer) + 1))
			if(VN_G_LAYER_VERTEX_XYZ == e_nsg_get_layer_type(layer))
				names[count++] = e_nsg_get_layer_name(layer);
		layer_mode = sw_type_in_pulldown(input, -0.25, 0.1, 0.5, "Nisse", 15, names, count, layer_name_func, NULL, layer_mode);
	}*/

//	sui_type_in(input, -0.25, 0.2, 0.5, "load.obj", 32, geometry_load_obj, NULL);
//	sui_type_in(input, -0.25, 0.3, 0.5, "save.obj", 32, geometry_save_obj, NULL);


	if(input->mode == BAM_DRAW)
	{
		glEnable(GL_DEPTH_TEST); 
		glPushMatrix();
		glPushMatrix();
		glPopMatrix();  
    }	
}
