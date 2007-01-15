
#include "st_matrix_operations.h"
#include "enough.h"
#include "seduce.h"
#include "persuade.h"
#include "deceive.h"

//#include "co_func_repository.h"
//#include "co_storage.h"

void connect_type_in_func(void *user, char *text)
{
	e_vc_connect(text, "unknown", "none", NULL);
}

extern void qs_input_handler(BInputState *input, void *user);
extern void qs_intro_draw(void *user);
extern void g_set_ship_camera(BInputState *input, float delta_time);


extern void sds_2_test(void);
void draw_table_debuging(void);
void p_status_print(void);

extern void		qs_camera_init(void);
extern float	*qs_get_cam_matrix(void);
extern float	*qs_get_cam_pos(void);
extern void		qs_set_camera(void);
extern void		qs_compute_camera(BInputState *input, float delta_time);
extern void		qs_settings_get_background_color(float *color);
extern void		p_context_update(void);
extern boolean	qs_draw_settings(BInputState *input);

void qs_draw_handler(BInputState *input, void *user)
{
	static boolean settings = FALSE;

	if(input->mode == BAM_MAIN)
	{
		verse_callback_update(30);
		p_task_compute(8);
		return;
	}
	if(input->mode == BAM_DRAW)
	{
		double pos[3] = {0, 0, 0};
		float color[3];
		qs_settings_get_background_color(color);
		glEnable(GL_DEPTH_TEST);
		glClearColor(color[0], color[1], color[2], 0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glPushMatrix();
		qs_set_camera();
		p_draw_scene();
		{
			uint x, y;
			betray_get_screen_mode(&x, &y, NULL);
			betray_reshape_view(x, y);
		}
		glPushMatrix();
		glTranslatef(0, 0, -1);
		glDisable(GL_DEPTH_TEST);
	}else
	{
		if(input->mouse_button[2] == TRUE && input->last_mouse_button[2] != TRUE)
			settings = !settings;
		if(!settings)
			qs_compute_camera(input, betray_get_delta_time());
	}
	if(settings)
		settings = qs_draw_settings(input);
	if(input->mode == BAM_DRAW)
		glPopMatrix();
}


void qs_intro_handler(BInputState *input, void *user)
{
	static boolean active = FALSE;
	static char connect_type_in[48];
	connect_type_in[0] = 0;

	if(e_vc_check_connected() && e_vc_check_accepted_slot(0))
	{
		betray_set_action_func(qs_draw_handler, NULL);
		return;
	}

	if(input->mode == BAM_MAIN)
	{
		verse_callback_update(30000);
		return;
	}

	if(input->mode == BAM_DRAW)
	{
		glClearColor(1, 1, 1, 0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glPopMatrix();
		glPopMatrix();
		glPushMatrix();

		glTranslatef(0, 0, -1);
		glDisable(GL_DEPTH_TEST);
	}
//	qs_intro_draw(NULL);
	if((input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE) || (input->mouse_button[1] == FALSE && input->last_mouse_button[1] == TRUE) || (input->mouse_button[2] == FALSE && input->last_mouse_button[2] == TRUE))
		active = TRUE;

	if(active)
	{
		if(e_vc_check_connected_slot(0))
		{
			if(input->mode == BAM_DRAW)
				sui_draw_text(sui_compute_text_length(0.02, 2, "CONNECTING...") * -0.5, -0.3, 0.02, 2, "CONNECTING...", 0, 0, 0, 1.0);    
		}
		else
		{
			static boolean text_init = FALSE;
			static char address[64] = {255}, name[64] = {255}, pass[64] = {255};
			if(!text_init)
			{
				char *t;
				uint i;
				t = sui_get_setting_text("address", "localhost");
				for(i = 0; i < 64 && t[i] != 0 ; i++)
					address[i] = t[i];
				address[i] = 0;
				t = sui_get_setting_text("name", "name");
				for(i = 0; i < 64 && t[i] != 0 ; i++)
					name[i] = t[i];
				name[i] = 0;
				t = sui_get_setting_text("pass", "pass");
				for(i = 0; i < 64 && t[i] != 0 ; i++)
					pass[i] = t[i];
				pass[i] = 0;
				text_init = TRUE;
			}
			if(input->mode == BAM_DRAW)
				sui_draw_text(sui_compute_text_length(0.02, 2, "CONNECT") * -0.5, 0.35, 0.02, 2, "CONNECT", 0, 0, 0, 1.0);

			if(sw_text_button(input, -0.2, 0.25 - 2 * 0.02, 0, 0.02, 0.3, "Localhost", 0, 0, 0, 1.0))
				e_vc_connect("localhost", "somepass", "somepass", NULL);
			if(sw_text_button(input, 0.2, 0.25 - 2 * 0.02, 1, 0.02, 0.3, "Exit", 0, 0, 0, 1.0))
			{
				sui_save_settings("qs_config.cfg");
				exit(0);
			}
			
			sui_type_in(input, -0.2, -0.2, 0.4, SUI_T_SIZE, address, 64, NULL, NULL, 0, 0, 0, 1.0);
			sui_draw_2d_line_gl(-0.2, -0.2, 0.2, -0.2, 0, 0, 0, 0);
			sui_type_in(input, -0.2, -0.25, 0.4, SUI_T_SIZE, name, 64, NULL, NULL, 0, 0, 0, 1.0);
			sui_draw_2d_line_gl(-0.2, -0.25, 0.2, -0.25, 0, 0, 0, 0);
			sui_type_in(input, -0.2, -0.3, 0.4, SUI_T_SIZE, pass, 64, NULL, NULL, 0, 0, 0, 1.0);
			sui_draw_2d_line_gl(-0.2, -0.3, 0.2, -0.3, 0, 0, 0, 0);


			if(sw_text_button(input, -0.2, -0.325 - 2 * 0.02, 0, 0.02, 0.3, "OK", 0, 0, 0, 1.0))
			{
				sui_set_setting_text("address", address);
				sui_set_setting_text("name", name);
				sui_set_setting_text("pass", pass);
				sui_save_settings("qs_config.cfg");
				e_vc_connect(address, name, pass, NULL);
			}
			if(sw_text_button(input, 0.2, -0.325 - 2 * 0.02, 1, 0.02, 0.3, "Cancel", 0, 0, 0, 1.0))
			{
				address[0] = 0;
				name[0] = 0;
				pass[0] = 0;
			}
		}
	}

	if(input->mode == BAM_DRAW)
	{
		glEnable(GL_DEPTH_TEST);
		glPopMatrix();
		glPushMatrix();
		glPushMatrix();
	}
}


extern void *se_symbol_editor_func(BInputState *input, void *user_pointer);
extern void *se_font_editor_func(BInputState *input, void *user_pointer);
extern void qs_intro_init(void);
extern void qs_settings_init(void);
extern void sp_settings_init(void);
extern void p_init_render_to_texture(void);

int main(int argc, char **argv)
{
	betray_init(argc, argv, 800, 600, FALSE, "Quel Solaar");
	deceive_set_arg(argc, argv);
	sui_load_settings("qs_config.cfg");
	qs_settings_init();
	sui_init();
	qs_camera_init();
	glClearColor(1, 1, 1, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	enough_init();					/* initializing the Enough Lib, setting the max subdivision level to 3*/
#ifdef PERSUADE_H
	persuade_init(4, betray_get_gl_proc_address());
	p_geo_set_sds_level(4);
	sp_settings_init();
#endif
	qs_intro_init();
//	betray_set_mouse_warp(TRUE);

	deceive_set_intro_draw_func(qs_intro_draw, NULL);
	betray_set_action_func(deceive_intro_handler, qs_draw_handler);
//	betray_set_action_func(sui_symbol_editor_func, NULL);
	betray_set_context_update_func(p_context_update);
	betray_launch_main_loop();
	return 0;
}
