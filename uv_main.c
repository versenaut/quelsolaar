

#include "enough.h"
#include "seduce.h"
#include "persuade.h"
#include "st_matrix_operations.h"
#include "uv_geometry.h"
#include "uv.h"

/* Disable hijacking of main() by SDL. */
#if defined _WIN32 && defined BETRAY_SDL_SYSTEM_WRAPPER
#undef main
#endif

void connect_type_in_func(void *user, char *text)
{
	e_vc_connect(text, "unknown", "none", NULL);
}

extern void uv_input_handler(BInputState *input, void *user);
extern void uv_intro_handler(BInputState *input, void *user);

void uv_intro_handler(BInputState *input, void *user)
{
	static boolean active = TRUE;
	static char connect_type_in[48];
	connect_type_in[0] = 0;

	if(e_vc_check_connected() && e_vc_check_accepted_slot(0))
	{
		ENode *node;
		node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY);
//		printf("node %p\n", node);
		if(node != NULL)
			uvg_set_node(e_ns_get_node_id(node));
		if(uvg_update())
		{
			betray_set_action_func(uv_input_handler, NULL);
			return;
		}
	}
	if(input->mode == BAM_MAIN)
	{
		verse_callback_update(0);
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
			sui_draw_2d_line_gl(-0.2, -0.45, 0.2, -0.45, 0, 0, 0, 1.0f);
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
		glEnable(GL_DEPTH_TEST);
		glPopMatrix();
		glPushMatrix();
		glPushMatrix();
	}
}


int main(int argc, char **argv)
{
	betray_init(argc, argv,1280, 1024, FALSE, "UV");
	sui_init();
	uvg_init();
	uve_init();
	uv_init_draw_line();
	uv_trans_init();
#ifdef PERSUADE_H
	persuade_init(1, betray_get_gl_proc_address());
#endif
	glClearColor(1, 1, 1, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	enough_init();
	betray_set_action_func(uv_intro_handler, NULL);
//	betray_set_action_func(sui_symbol_editor_func, NULL);
	betray_launch_main_loop();
	return 0;
}
