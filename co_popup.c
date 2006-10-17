 #include "st_matrix_operations.h"
#include "enough.h"
#include "seduce.h"
#include "deceive.h"

#include "co_vn_handle.h"

void co_pu_hide_all(void)
{

	ENode *node;
	COVerseNode *co_node;
	uint type;
	for(type = 0; type < V_NT_NUM_TYPES; type++)
	{
		for(node = e_ns_get_node_next(0, 0, type); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, type))
		{
			co_node = e_ns_get_custom_data(node, CONNECTOR_ENOUGH_SLOT);
			co_node->hidden = TRUE;
		}
	}
}

static boolean co_settings = FALSE;

void sp_settings_pre(SUIViewElement *element);
void sp_settings_post(SUIViewElement *element);

float co_background_color[3] = {1.0, 1.0, 1.0};
float co_line_color[3] = {0.0, 0.0, 0.0};

boolean co_draw_settings(BInputState *input)
{
	if(co_settings == TRUE)
	{
		SUIViewElement element[15];
		glPushMatrix();
		glTranslatef(0, 0, -1);
		sp_settings_pre(element);
		element[13].type = S_VET_COLOR;
		element[13].text = "background";
		element[13].param.color[0] = co_background_color[0];
		element[13].param.color[1] = co_background_color[1];
		element[13].param.color[2] = co_background_color[2];
		element[14].type = S_VET_COLOR;
		element[14].text = "lines";
		element[14].param.color[0] = co_line_color[0];
		element[14].param.color[1] = co_line_color[1];
		element[14].param.color[2] = co_line_color[2];
		glDisable(GL_DEPTH_TEST);
		if(co_background_color[0] + co_background_color[1] + co_background_color[2] > 1.5)
			co_settings = !sui_draw_setting_view(input, 0, 0.3, 0.5, element, 15, "SETTINGS", 1.0);
		else
			co_settings = !sui_draw_setting_view(input, 0, 0.3, 0.5, element, 15, "SETTINGS", 0.0);
		glEnable(GL_DEPTH_TEST);
		sp_settings_post(element);
		co_background_color[0] = element[13].param.color[0];
		co_background_color[1] = element[13].param.color[1];
		co_background_color[2] = element[13].param.color[2];
		co_line_color[0] = element[14].param.color[0];
		co_line_color[1] = element[14].param.color[1];
		co_line_color[2] = element[14].param.color[2];
		if(!co_settings)
		{
			sui_set_setting_double("BACKGROUND_COLOR_R", element[13].param.color[0]);
			sui_set_setting_double("BACKGROUND_COLOR_G", element[13].param.color[1]);
			sui_set_setting_double("BACKGROUND_COLOR_B", element[13].param.color[2]);
			sui_set_setting_double("LINE_COLOR_R", element[14].param.color[0]);
			sui_set_setting_double("LINE_COLOR_G", element[14].param.color[1]);
			sui_set_setting_double("LINE_COLOR_B", element[14].param.color[2]);
			sui_save_settings("co_config.cfg");
		}
		glPopMatrix();
		return TRUE;
	}else
	{
		co_background_color[0] = sui_get_setting_double("BACKGROUND_COLOR_R", 1.0);
		co_background_color[1] = sui_get_setting_double("BACKGROUND_COLOR_G", 1.0);
		co_background_color[2] = sui_get_setting_double("BACKGROUND_COLOR_B", 1.0);
		co_line_color[0] = sui_get_setting_double("LINE_COLOR_R", 0.0);
		co_line_color[1] = sui_get_setting_double("LINE_COLOR_G", 0.0);
		co_line_color[2] = sui_get_setting_double("LINE_COLOR_B", 0.0);
	}
	return FALSE;
}

boolean co_get_settings(void)
{
	return co_settings;
}

extern boolean co_draw_3d_view_get(void);
extern boolean co_draw_3d_persuade_get(void);
extern void co_draw_3d_view_set(boolean set);
extern void co_draw_3d_persuade_set(boolean set);

void co_pu_empty(BInputState *input)
{
	SUIPUElement element[4];
	static float x, y;
	uint ring;
	element[0].type = PU_T_ANGLE;
	element[0].text = "Settings";
	element[0].data.angle[0] = -45;
	element[0].data.angle[1] = +45;

	element[1].type = PU_T_ANGLE;
	element[1].text = "Hide All";
	element[1].data.angle[0] = 45;
	element[1].data.angle[1] = 135;

	element[2].type = PU_T_ANGLE;
	if(co_draw_3d_persuade_get())
		element[2].text = "Wireframe";
	else
		element[2].text = "Shaded";
	element[2].data.angle[0] = 135;
	element[2].data.angle[1] = 225;

	element[3].type = PU_T_ANGLE;
	if(co_draw_3d_view_get())
		element[3].text = "Hide scene";
	else
		element[3].text = "Show scene";
	element[3].data.angle[0] = 225;
	element[3].data.angle[1] = 315;

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
	if(co_background_color[0] + co_background_color[1] + co_background_color[2] > 1.5)
		ring = sui_draw_popup(input, x, y, element, 4, 2, 1.0);
	else
		ring = sui_draw_popup(input, x, y, element, 4, 2, 0.0);
	switch(ring)
	{
		case 0 :
	/*		co_pu_hide_all();*/
			co_settings = TRUE;
		break;
		case 1 :
			co_pu_hide_all();
	/*		la_t_revolve(edge, 16);*/
		break;
		case 2 :
			co_draw_3d_persuade_set(!co_draw_3d_persuade_get());
	/*		la_t_tube(edge, 8);*/
		break;
		case 3 :
			co_draw_3d_view_set(!co_draw_3d_view_get());
	/*		la_t_select_hull(edge);*/
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
