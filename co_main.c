
#include "st_matrix_operations.h"
#include "enough.h"
#include "seduce.h"
#include "persuade.h"

//#include "co_func_repository.h"
//#include "co_storage.h"

#include "co_vn_handle.h"
#include "co_vn_graphics.h"
#include "co_widgets.h"

void connect_type_in_func(void *user, char *text)
{
	e_vc_connect(text, "unknown", "none", NULL);
}

extern void co_input_handler(BInputState *input, void *user);
extern void co_intro_handler(BInputState *input, void *user);
extern void co_intro_draw(void);

void co_intro_handler(BInputState *input, void *user)
{
	static boolean active = FALSE;
	static char connect_type_in[48];
	connect_type_in[0] = 0;

	if(e_vc_check_connected() && e_vc_check_accepted_slot(0))
	{
		betray_set_action_func(co_input_handler, NULL);
		return;
	}

	if(input->mode == BAM_MAIN)
	{
		verse_callback_update(30000);
		co_vng_update_time();
		return;
	}

	if(input->mode == BAM_DRAW)
	{
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glPopMatrix();
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0, 0, -1);
		glDisable(GL_DEPTH_TEST);
	}
	co_intro_draw();
	if((input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE) ||
	   (input->mouse_button[1] == FALSE && input->last_mouse_button[1] == TRUE) ||
	   (input->mouse_button[2] == FALSE && input->last_mouse_button[2] == TRUE))
		active = TRUE;

/*	if(active)
	{
//		if(input->mode == BAM_DRAW)
//	        sw_draw_window(-0.35, -0.2, 0.7, 0.4, 0.02);
		if(e_vc_check_connected_slot(0))
		{
			if(input->mode == BAM_DRAW)
				sui_draw_text(sui_compute_text_length(0.02, 2, "CONNECTING") * -0.5, 0, 0.02, 2, "CONNECTING", 0, 0, 0);    
		}
		else
		{
			co_w_type_in(input, -0.2, -0.1, 0.4, SUI_T_SIZE, connect_type_in, 48, connect_type_in_func, NULL, 0, 0);
		//	sw_type_in(input, -0.2, -0.1, 0.4, connect_type_in, 48, connect_type_in_func, NULL);
			if(input->mode == BAM_DRAW)
				sui_draw_text(sui_compute_text_length(0.02, 2, "CONNECT") * -0.5, 0.1, 0.02, 2, "CONNECT", 0, 0, 0);

			if(sw_text_button(input, -0.2, 0, 0, 0.02, 0.3, "Localhost", 0, 0, 0))
				e_vc_connect("localhost", "somepass", "somepass", NULL);
			if(sw_text_button(input, 0.2, 0, 1, 0.02, 0.3, "Exit", 0, 0, 0))
			{
				sui_save_settings("co_config.cfg");
				exit(0);
			}
		}
	}*/

	if(active)
	{
//		if(input->mode == BAM_DRAW)
//	        sw_draw_window(-0.35, -0.2, 0.7, 0.4, 0.02);
		if(e_vc_check_connected_slot(0))
		{
			if(input->mode == BAM_DRAW)
				sui_draw_text(sui_compute_text_length(0.02, 2, "CONNECTING...") * -0.5, -0.3, 0.02, 2, "CONNECTING...", 0, 0, 0);
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
				sui_draw_text(sui_compute_text_length(0.02, 2, "CONNECT") * -0.5, 0.35, 0.02, 2, "CONNECT", 0, 0, 0);

			if(sw_text_button(input, -0.2, 0.25 - 2 * 0.02, 0, 0.02, 0.3, "Localhost", 0, 0, 0))
				e_vc_connect("localhost", "somepass", "somepass", NULL);
			if(sw_text_button(input, 0.2, 0.25 - 2 * 0.02, 1, 0.02, 0.3, "Exit", 0, 0, 0))
			{
				sui_save_settings("co_config.cfg");
				exit(0);
			}
			
			sui_type_in(input, -0.2, -0.2, 0.4, SUI_T_SIZE, address, 64, NULL, NULL, 0, 0, 0);
			sui_draw_2d_line_gl(-0.2, -0.2, 0.2, -0.2, 0, 0, 0);
			sui_type_in(input, -0.2, -0.25, 0.4, SUI_T_SIZE, name, 64, NULL, NULL, 0, 0, 0);
			sui_draw_2d_line_gl(-0.2, -0.25, 0.2, -0.25, 0, 0, 0);
			sui_type_in(input, -0.2, -0.3, 0.4, SUI_T_SIZE, pass, 64, NULL, NULL, 0, 0, 0);
			sui_draw_2d_line_gl(-0.2, -0.3, 0.2, -0.3, 0, 0, 0);


			if(sw_text_button(input, -0.2, -0.325 - 2 * 0.02, 0, 0.02, 0.3, "OK", 0, 0, 0))
			{
				sui_set_setting_text("address", address);
				sui_set_setting_text("name", name);
				sui_set_setting_text("pass", pass);
				sui_save_settings("co_config.cfg");
				e_vc_connect(address, name, pass, NULL);
			}
			if(sw_text_button(input, 0.2, -0.325 - 2 * 0.02, 1, 0.02, 0.3, "Cancel", 0, 0, 0))
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
extern void co_intro_init(void);

int main(int argc, char **argv)
{
	betray_init(argc, argv, 1280, 1024, FALSE, "Connector");
	sui_init();

	glClearColor(1, 1, 1, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	sui_init_settings("co_config.cfg");
	co_init_handle_verse_node();
	enough_init();					/* initializing the Enough Lib, setting the max subdivision level to 3*/
#ifdef PERSUADE_H
	persuade_init(3, betray_get_gl_proc_address());
#endif
	co_vng_init();
	co_intro_init();
	e_nsm_set_custom_func(CO_ENOUGH_NODE_SLOT, material_func);

	betray_set_action_func(co_intro_handler, NULL);
//	betray_set_action_func(sui_symbol_editor_func, NULL);
//	printf("entering main loop\n");
	betray_launch_main_loop();
	return 0;
}
