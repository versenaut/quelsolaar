/*
 * This is the header for the Betray windowing system abstraction layer. 
*/

#if !defined(BETRAY_H)
#define	BETRAY_H

#include "st_types.h"

/* ----------------------------------------------------------------------------------------- */

/* These symbols control which back-end wrapper we use for Betray.
 * Only one can be defined at a time.
*/
#undef BETRAY_GLUT_SYSTEM_WRAPPER
#undef BETRAY_GLFW_SYSTEM_WRAPPER
#undef BETRAY_SDL_SYSTEM_WRAPPER
#undef BETRAY_WIN32_SYSTEM_WRAPPER
#undef BETRAY_X11_SYSTEM_WRAPPER

/* On Windows, we really want to use the native (Win32) wrapper. On Linux,
 * we use the native X11 wrapper. On other platforms, SDL is the default.
*/
#if defined _WIN32
#define	BETRAY_WIN32_SYSTEM_WRAPPER
#elif defined __linux__
#define	BETRAY_X11_SYSTEM_WRAPPER
#elif defined __APPLE_CC__
#define	BETRAY_GLUT_SYSTEM_WRAPPER
#else
#define	BETRAY_SDL_SYSTEM_WRAPPER
#endif



/* On Windows, we also need the main platform header file. */
#if defined _WIN32
#include <windows.h>
#endif

/* We need OpenGL, which is a bit harder to locate on Macintosh systems. */
#if defined __APPLE_CC__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif


#if defined BETRAY_SDL_SYSTEM_WRAPPER
#include <SDL/SDL.h>

/* FIXME: This shouldn't be necessary. We should just link with sdlmain.lib and
 * be happy. Unfortunately, I can't seem to be able to get it to work. So, the
 * work-around here is to undefine SDL's wrapping of main().
*/
#ifdef _WIN32
#ifdef UNDEF_MAIN
#undef main
#endif
#endif

#elif defined BETRAY_GLUT_SYSTEM_WRAPPER
/* Get hold of glut.h include file. Need to be Mac-specific, here. */
#if defined __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#elif defined BETRAY_GLFW_SYSTEM_WRAPPER
#include <GL/glfw.h>
#elif defined BETRAY_WIN32_SYSTEM_WRAPPER
#include <GL/gl.h>

#elif defined BETRAY_X11_SYSTEM_WRAPPER
/* do nothing */
#else
#error "Please choose a system wrapper to use."
#endif

/* ----------------------------------------------------------------------------------------- */

#define BETRAY_DEVICE_BUTTONS_COUNT	16
#define BETRAY_MAX_EVENT_COUNT		64
#define BETRAY_BUTTONS_COUNT		(256 + 16) 

typedef enum {
	BAM_DRAW,
	BAM_EVENT,
	BAM_MAIN
} BActionMode;

typedef struct {
	uint	button;
	boolean state;
} BButtonEvent;

typedef struct {
	boolean		mouse_button[BETRAY_DEVICE_BUTTONS_COUNT];
	boolean		last_mouse_button[BETRAY_DEVICE_BUTTONS_COUNT];
	float		pointer_x;
	float		pointer_y;
	float		click_pointer_x;
	float		click_pointer_y;
	float		delta_pointer_x;
	float		delta_pointer_y;
	BButtonEvent	event[16];
	uint		event_count;
	double		time;
	BActionMode	mode;
} BInputState;

/* Internal functions, not to be used directly by applications. */
extern boolean		betray_internal_init_display(int argc, char **argv, uint size_x, uint size_y, boolean full_screen, const char *caption);
extern boolean		betray_internal_set_display(uint x_size, uint y_size, boolean fullscreen);
extern boolean		betray_internal_key_add(BInputState *input, uint key, boolean pressed);
/* End of internal functions, below is public API. */

extern void		betray_init(int argc, char **argv, uint window_size_x, uint window_size_y, boolean window_fullscreen, const char *name);
extern void		betray_action(BActionMode mode);
extern void		betray_set_action_func(void (*input_compute_func)(BInputState *data, void *user_pointer), void *user_pointer);
extern void		betray_set_context_update_func(void (*context_func)(void));
extern void		betray_set_mouse_warp(boolean warp);
extern void		betray_execute(const char *command);
extern float		betray_get_time(void);
extern void *		betray_get_gl_proc_address(void);

extern boolean		betray_get_key(uint key);
extern void		betray_get_key_up_down(boolean *press, boolean *last_press, uint key);

extern BInputState *	betray_get_input_state(void);

/* To set and get the screen mode, please note that sui_get_screen_mode returns the aspect (width * aspect == height). */

extern void		betray_reshape_view(uint x_size, uint y_size);

extern boolean		betray_set_screen_mode(uint x_size, uint y_size, boolean fullscreen);
extern double		betray_get_screen_mode(uint *x_size, uint *y_size, boolean *fullscreen);

extern void		betray_start_type_in(char *text, uint size, void (*done_func)(void *user, boolean cancel), uint *cursor, void *user_pointer);
extern void		betray_end_type_in_mode(boolean cancel);
extern boolean		betray_is_type_in(void);
extern void		betray_move_cursor(int move);
extern void		betray_insert_character(char character);
extern void		betray_delete_character(void);

extern char *		betray_debug(void);
extern void		betray_launch_main_loop(void);

extern void		betray_get_current_time(uint32 *seconds, uint32 *fractions);
extern double		betray_get_delta_time(void);
extern void		betray_time_update(void);

#endif
