

#include "enough.h"
#include "seduce.h"
#include "persuade.h"
#include "deceive.h"
#include "st_matrix_operations.h"
#include "uv_geometry.h"
#include "uv.h"

/* Disable hijacking of main() by SDL. */
#if defined _WIN32 && defined BETRAY_SDL_SYSTEM_WRAPPER
#undef main
#endif

extern void uv_input_handler(BInputState *input, void *user);

int main(int argc, char **argv)
{
	betray_init(argc, argv,1280, 1024, FALSE, "UV");
	sui_init();
	uvg_init();
	uve_init();
	uv_init_draw_line();
	uv_trans_init();
	glClearColor(1, 1, 1, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	enough_init();
#ifdef PERSUADE_H
	persuade_init(2, betray_get_gl_proc_address());
#endif
//	deceive_set_inrto_draw_func(la_intro_draw, NULL);
	betray_set_action_func(deceive_intro_handler, uv_input_handler);
//	betray_set_action_func(uv_intro_handler, NULL);
//	betray_set_action_func(sui_symbol_editor_func, NULL);
	betray_launch_main_loop();
	return 0;
}
