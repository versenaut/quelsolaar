
/*#include "st_matrix_operations.h"*/
#include "enough.h"
#include "seduce.h"
/*#include "persuade.h"*/
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

extern void co_input_handler(BInputState *input, void *user);
extern void co_intro_handler(BInputState *input, void *user);
extern void co_intro_draw(void *user);
extern void *se_symbol_editor_func(BInputState *input, void *user_pointer);
extern void *se_font_editor_func(BInputState *input, void *user_pointer);
extern void co_intro_init(void);

/*
static void place_and_fix(uint connection, uint id, VNodeType type, void *user)
{
	uint32 seconds, fractions;
	static uint i = 0; 
	real64 pos[3] = {0, 0, 0};
	ENode *node;
	pos[0] = (real64)(i % 20) * 0.1;
	pos[2] = (real64)(i / 20) * 0.1;
	i++;
	verse_session_get_time(&seconds, &fractions);
	verse_send_o_transform_pos_real64(id, seconds, fractions, pos, NULL, NULL, NULL, 0);
	node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY);
	verse_send_o_link_set(id, -1, e_ns_get_node_id(node), "geometry", 0);
	node = e_ns_get_node_next(0, 0, V_NT_MATERIAL);
	verse_send_o_link_set(id, -1, e_ns_get_node_id(node), "material", 0);
	if((i / 20) % 5 == 0 && i % 5 == 0)
		verse_send_o_light_set(id, sin((real64)i) * 0.5 + 0.5, sin((real64)i + 1) * 0.5 + 0.5, sin((real64)i + 2) * 0.5 + 0.5);
}
*/

int main(int argc, char **argv)
{
	betray_init(argc, argv, 1280, 1024, FALSE, "Connector");
	sui_init();

	glClearColor(1, 0, 1, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	sui_load_settings("co_config.cfg");
	co_init_handle_verse_node();
	enough_init();					/* initializing the Enough Lib, setting the max subdivision level to 3*/
#ifdef PERSUADE_H
	persuade_init(5, betray_get_gl_proc_address());
#endif
	co_vng_init();
	co_intro_init();
	e_nsm_set_custom_func(CO_ENOUGH_NODE_SLOT, material_func);

	deceive_set_intro_draw_func(co_intro_draw, NULL);
	betray_set_action_func(deceive_intro_handler, co_input_handler);

//	betray_set_action_func(co_intro_handler, NULL);
//	betray_set_action_func(sui_symbol_editor_func, NULL);
//	printf("entering main loop\n");
	betray_launch_main_loop();
	return 0;
}
