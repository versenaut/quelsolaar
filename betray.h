
#if !defined(BETRAY_H)
#define	BETRAY_H


#include "st_types.h"

/* Basic functionality */

/*#define BETRAY_SDL_SYSTEM_WRAPPER*/
#if !defined(BETRAY_SDL_SYSTEM_WRAPPER)
#define BETRAY_GLUT_SYSTEM_WRAPPER
#include <GLUT/glut.h>
#else
#include <SDL/SDL.h>
#ifdef WIN32
	#include <windows.h>
#endif
#include <GL/gl.h>
#endif

#define DEVICE_BUTTONS_COUNT 16
#define MAX_EVENT_COUNT 64
#define BUTTONS_COUNT (256 + 16) 

typedef enum{
	BAM_DRAW,
	BAM_EVENT,
	BAM_MAIN
}BActionMode;

typedef struct{
	uint	button;
	boolean state;
}BButtonEvent;

typedef struct{
	boolean			mouse_button[DEVICE_BUTTONS_COUNT];
	boolean			last_mouse_button[DEVICE_BUTTONS_COUNT];
	float			pointer_x;
	float			pointer_y;
	float			click_pointer_x;
	float			click_pointer_y;
	float			delta_pointer_x;
	float			delta_pointer_y;
	BButtonEvent	event[16];
	uint			event_count;
	double			time;
	BActionMode		mode;
}BInputState;

extern void		betray_init(int argc, char **argv, uint window_size_x, uint window_size_y, boolean window_fullscreen, char *name);
extern void		betray_action(BActionMode mode);
extern void		betray_set_action_func(void (*input_compute_func)(BInputState *data, void *user_pointer), void *user_pointer);
extern void		betray_set_mouse_warp(boolean warp);
extern void		betray_execute(char *command);
extern float	betray_get_time();
extern void		*betray_get_gl_proc_address();

extern boolean	betray_get_key(uint key);
extern void		betray_get_key_up_down(boolean *press, boolean *last_press, uint key);



extern BInputState *betray_get_input_state();

/*  to set and get the screan mode, please note that sui_get_screen_mode returns the aspect (whith * aspect == hight)*/

extern void		betray_reshape_view(uint x_size, uint y_size);

extern boolean	betray_set_screen_mode(uint x_size, uint y_size, boolean fullscreen);
extern double	betray_get_screen_mode(uint *x_size, uint *y_size, boolean *fullscreen);


extern void		betray_start_type_in(char *text, uint size, void (*done_func)(void *user, boolean cancle), uint *curser, void *user_pointer);
extern void		betray_end_type_in_mode(boolean cancle);

extern boolean	betray_is_type_in();
extern void		betray_move_cursor(int move);
extern void		betray_insert_caracter(char caracter);
extern void		betray_delete_caracter(void);

extern char		*betray_debug(void);
extern void		betray_launch_main_loop(void);

extern void		betray_get_current_time(uint32 *seconds, uint32 *fractions);
extern double	betray_get_delta_time();

#endif
