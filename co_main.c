
#include "st_matrix_operations.h"
#include "enough.h"
#include "seduce.h"
#include "persuade.h"
#include "deceive.h"
//#include "co_func_repository.h"
//#include "co_storage.h"

/* Disable hijacking of main() by SDL. */
#if defined _WIN32 && defined BETRAY_SDL_SYSTEM_WRAPPER
#undef main
#endif

#include "co_vn_handle.h"
#include "co_vn_graphics.h"
#include "co_widgets.h"
#include "co_projection.h"

extern void co_input_handler(BInputState *input, void *user);
extern void co_intro_handler(BInputState *input, void *user);
extern void co_intro_draw(void *user);
extern void *se_symbol_editor_func(BInputState *input, void *user_pointer);
extern void *se_font_editor_func(BInputState *input, void *user_pointer);
extern void co_intro_init(void);

int main(int argc, char **argv)
{
	betray_init(argc, argv, 1280, 1024, FALSE, "Connector");
	sui_init();

	p_init();
	glClearColor(1, 1, 1, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	sui_load_settings("co_config.cfg");
	co_init_handle_verse_node();
	enough_init();					/* initializing the Enough Lib, setting the max subdivision level to 3*/
#ifdef PERSUADE_H
	persuade_init(3, betray_get_gl_proc_address());
	p_geo_set_sds_level(1);
#endif
	co_vng_init();
	co_intro_init();
	e_nsm_set_custom_func(CO_ENOUGH_NODE_SLOT, material_func);
	e_ns_set_node_create_func(NULL, NULL);

	deceive_set_intro_draw_func(co_intro_draw, NULL);
	betray_set_action_func(deceive_intro_handler, co_input_handler);

//	betray_set_action_func(co_intro_handler, NULL);
//	betray_set_action_func(sui_symbol_editor_func, NULL);
//	printf("entering main loop\n");
	betray_launch_main_loop();
	return 0;
}
