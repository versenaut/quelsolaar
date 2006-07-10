
#include "st_matrix_operations.h"
#include "enough.h"
#include "seduce.h"
#include "deceive.h"

#include "lp_projection.h"

/* Disable hijacking of main() by SDL. */
#if defined UNDEF_MAIN
#undef main
#endif

extern void lp_input_handler(BInputState *input, void *user);

int main(int argc, char **argv)
{
	betray_init(argc, argv, 1280, 1024, FALSE, "LayerPainter");
	sui_init();
	p_init();
	glClearColor(1, 1, 1, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	sui_load_settings("lp_config.cfg");
	enough_init();					/* initializing the Enough Lib, setting the max subdivision level to 3*/
//	deceive_set_intro_draw_func(NULL, NULL);
	betray_set_action_func(deceive_intro_handler, lp_input_handler);
	betray_launch_main_loop();
	return 0;
}
