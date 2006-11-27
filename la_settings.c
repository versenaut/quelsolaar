
#include "la_includes.h"
#include "la_geometry_undo.h"
#include "la_projection.h"
#include "la_draw_overlay.h"
#include "la_particle_fx.h"


extern void parse_input(BInputState *input, void *user);

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
void la_edit_func(BInputState *input, void *user);
void la_draw_force_update_persuade(void);

void la_draw_settings_menu(BInputState *input, void *user)
{
	SUIViewElement element[10];
	char *t, text[36];
	uint i;
	if(input->mode == BAM_MAIN)
	{
		verse_callback_update(0);
#ifdef PERSUADE_H
		p_task_compute(1);
#endif
		return;
	}
	element[0].type = S_VET_BOOLEAN;
	element[0].text = "dispalay Flares";
	element[0].param.checkbox = sui_get_setting_int("DISPLAY_SILLY_FLARES", TRUE);

	element[1].type = S_VET_BOOLEAN;
	element[1].text = "display SDS";
	element[1].param.checkbox = sui_get_setting_int("RENDER_AS_SDS", TRUE);

	element[2].type = S_VET_SLIDER;
	element[2].text = "complexity";
	element[2].param.slider = sqrt(sui_get_setting_double("GEOMETRY_COMPLEXITY", 1) / 1000.0);

	element[3].type = S_VET_INTEGER;
	element[3].text = "maximum Tesselation";
	element[3].param.integer = sui_get_setting_int("MAX_TESS_LEVEL", 2);

	element[4].type = S_VET_INTEGER;
	element[4].text = "minimum Tesselation";
	element[4].param.integer = sui_get_setting_int("MIN_TESS_LEVEL", 0);

	element[5].type = S_VET_TEXT;
	element[5].text = "author";
	t = sui_get_setting_text("AUTHOR", "eskil");
	for(i = 0; i < 32 && t[i] != 0; i++)
		text[i] = t[i];
	text[i] = 0;
	element[5].param.text.text = text;
	element[5].param.text.length = 32;

	element[6].type = S_VET_INTEGER;
	element[6].text = "revole section";
	element[6].param.integer = sui_get_setting_int("REVOLVE_INTERSECTIONS", 8);

	element[7].type = S_VET_INTEGER;
	element[7].text = "undo buffer";
	element[7].param.integer = sui_get_setting_int("UNDO_MEMORY_FOOT_PRINT", 5000000);

	if(input->mode == BAM_DRAW)
	{
		glClearColor(0, 0, 0, 0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glPushMatrix();
		p_view_set();
		la_do_owerlay();
		glPushMatrix();
		la_pfx_select_vertex();
		glPopMatrix();
		glPopMatrix();
		la_pfx_draw(FALSE);

		glPushMatrix();
		glTranslatef(0, 0, -1);
		glDisable(GL_DEPTH_TEST);
	}
	if(sui_draw_setting_view(input, 0, 0, 0.5, element, 8, "settings", 0.0))
	{
		sui_save_settings("la_config.cfg");
		betray_set_action_func(la_edit_func, NULL);
	}
	if(input->mode == BAM_DRAW)
	{
		glPopMatrix();
		glEnable(GL_DEPTH_TEST);
	}
	sui_set_setting_int("DISPLAY_SILLY_FLARES",		element[0].param.checkbox);
	sui_set_setting_int("RENDER_AS_SDS",			element[1].param.checkbox);
	sui_set_setting_double("GEOMETRY_COMPLEXITY",	element[2].param.slider * element[2].param.slider * 1000.0);
	sui_set_setting_int("MAX_TESS_LEVEL",			element[3].param.integer);
	sui_set_setting_int("MIN_TESS_LEVEL",			element[4].param.integer);
	sui_set_setting_text("AUTHOR",					element[5].param.text.text);
	sui_set_setting_int("REVOLVE_INTERSECTIONS",	element[6].param.integer);
	sui_set_setting_int("UNDO_MEMORY_FOOT_PRINT",	element[7].param.integer);


	p_geo_set_sds_level(element[3].param.integer);
	p_geo_set_sds_force_level(element[4].param.integer);
	p_geo_set_sds_mesh_factor(element[2].param.slider * element[2].param.slider * 1000.0);
	la_draw_force_update_persuade();
}
