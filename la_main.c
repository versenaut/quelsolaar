#include "la_includes.h"
#include "la_geometry_undo.h"
#include "la_projection.h"
#include "la_draw_overlay.h"
#include "la_tool.h"
#include "la_pop_up.h"
#include "la_particle_fx.h"

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
extern void init_transform_manupulator(void);
extern void	la_intro(void);
extern void init_key_input(void);
extern void parce_input(BInputState *input, void *user);
extern void draw_browser_menu(BInputState *input, void *user);


int main(int argc, char **argv)
{
	betray_init(argc, argv, 800, 600, FALSE, "Loq Airou");
	sui_init_settings("settngs.cfg");
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
	la_pfx_init(sui_get_setting_int("PARTICLE_COUNT", 512), sui_get_setting_int("FLARE_TEXTURE_SIZE", 128));

/*	p_op_lod_settings(sui_get_setting_int("MIN_TESS_LEVEL", 1),	sui_get_setting_double("GEOMETRY_COMPLEXITY", 40), sui_get_setting_double("LOD_TRESHOLD", 1.5), FALSE);
	p_op_lod_set_tess_levels(limit, 5);
	p_op_service_set_auto(TRUE); /* choosing to run enough lib in auto service mode */

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);/*GL_AMBIENT_AND_DIFFUSE*/

	betray_set_action_func(la_pu_connect, NULL);
	betray_launch_main_loop();	
	return 0;
}
