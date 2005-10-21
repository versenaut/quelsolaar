#include "la_includes.h"
#include "la_geometry_undo.h"
#include "la_projection.h"
#include "la_draw_overlay.h"
#include "la_tool.h"
#include "la_pop_up.h"
#include "la_particle_fx.h"
#include "persuade.h"
#include "deceive.h"

/* Disable hijacking of main() by SDL. */
#if defined _WIN32 && defined BETRAY_SDL_SYSTEM_WRAPPER
#undef main
#endif

#define THREADED FALSE

uint connection;

extern void set_defult_settings(void);


extern void p_view_set(void);
extern void draw_qube(void);
extern void init_owerlay(void);
extern void draw_owerlay(uint connection);
extern void init_draw_settings(void);
extern void p_init(void);
extern void init_draw_line(void);
extern void init_transform_manipulator(void);
extern void	la_intro(void);
extern void init_key_input(void);
extern void parse_input(BInputState *input, void *user);
extern void la_intro_draw(void *user);
extern void la_edit_func(BInputState *input, void *user);


int main(int argc, char **argv)
{
	betray_init(argc, argv, 1000, 750, FALSE, "Loq Airou");
	sui_load_settings("settngs.cfg");
	sui_init();
	glClearColor(0, 0, 0, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);	
	enough_init();

	la_do_init();
	p_init();
	init_key_input();
	init_undo();
	la_intro();
	la_t_init_draw_line();
	la_t_init_edge_connector();
	la_t_tm_init();
	la_pfx_init(sui_get_setting_int("PARTICLE_COUNT", 512), sui_get_setting_int("FLARE_TEXTURE_SIZE", 256));

/*	p_op_lod_settings(sui_get_setting_int("MIN_TESS_LEVEL", 1),	sui_get_setting_double("GEOMETRY_COMPLEXITY", 40), sui_get_setting_double("LOD_TRESHOLD", 1.5), FALSE);
	p_op_lod_set_tess_levels(limit, 5);
	p_op_service_set_auto(TRUE); /* choosing to run enough lib in auto service mode */
#ifdef PERSUADE_H
	persuade_init(3, betray_get_gl_proc_address());
#endif
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);/*GL_AMBIENT_AND_DIFFUSE*/

	deceive_set_intro_draw_func(la_intro_draw, NULL);
	betray_set_action_func(deceive_intro_handler, la_edit_func);
	betray_launch_main_loop();	
	return 0;
}
