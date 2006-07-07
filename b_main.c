
/*#include "opengl3.0.h"*/

#include <stdlib.h>

#include "betray.h"
#include <time.h>

extern boolean b_sdl_system_wrapper_set_display(uint size_x, uint size_y, boolean full_screen);
extern boolean b_glut_system_wrapper_set_display(uint size_x, uint size_y, boolean full_screen);

typedef struct{
	uint		x_size;
	uint		y_size;
	boolean	fullscreen;
}GraphicsMode;

struct{
	BInputState		input;
	boolean			input_clear;
	BActionMode		action_mode;
	GraphicsMode	screen_mode;
	void			(*action_func)(BInputState *input, void *user);
	void			*action_func_data;
	uint32			time[2];
	double			delta_time;
}BGlobal;

void betray_reshape_view(uint x_size, uint y_size)
{
	float w, h, fov, aspect;
	BGlobal.screen_mode.x_size = x_size;
	BGlobal.screen_mode.y_size = y_size;
	w = BGlobal.screen_mode.x_size;
	h = BGlobal.screen_mode.y_size;
//	fov = atan(90 / 180 * 3.14 );
	fov = 1;
	glMatrixMode(GL_PROJECTION);
	aspect = w / h;
	glLoadIdentity();
	glFrustum(-fov * 0.005, fov * 0.005, (-fov / aspect) * 0.005, (fov / aspect) * 0.005, 0.005, 10000.0);
	glViewport(0, 0, x_size, y_size);
	glMatrixMode(GL_MODELVIEW);	
}

boolean betray_set_screen_mode(uint x_size, uint y_size, boolean fullscreen)
{
	#ifdef BETRAY_SDL_SYSTEM_WRAPPER
	if(b_sdl_system_wrapper_set_display(x_size, y_size, fullscreen) != TRUE)
		return FALSE;
	#endif 

	#ifdef BETRAY_GLUT_SYSTEM_WRAPPER
	if(b_glut_system_wrapper_set_display(x_size, y_size, fullscreen) != TRUE)
		return FALSE;
	#endif

	#ifdef BETRAY_GLFW_SYSTEM_WRAPPER
	if(b_glfw_system_wrapper_set_display(x_size, y_size, fullscreen) != TRUE)
		return FALSE;
	#endif

	BGlobal.screen_mode.x_size = x_size;
	BGlobal.screen_mode.y_size = y_size;
	BGlobal.screen_mode.fullscreen = fullscreen;
	betray_reshape_view(x_size, y_size);
	return TRUE;
}

double betray_get_screen_mode(uint *x_size, uint *y_size, boolean *fullscreen)
{
	if(x_size != NULL)
		*x_size = BGlobal.screen_mode.x_size;
	if(y_size != NULL)
		*y_size = BGlobal.screen_mode.y_size;
	if(fullscreen != NULL)
		*fullscreen = BGlobal.screen_mode.fullscreen;
	return (double)BGlobal.screen_mode.y_size / (double)BGlobal.screen_mode.x_size;
}

extern void b_sdl_init_display(uint size_x, uint size_y, boolean full_screen, char *caption);
extern void b_glut_init_display(int argc, char **argv, uint size_x, uint size_y, boolean full_screen, char *caption);

void betray_init(int argc, char **argv, uint window_size_x, uint window_size_y, boolean window_fullscreen, char *name)
{
	#ifdef BETRAY_SDL_SYSTEM_WRAPPER
	b_sdl_init_display(window_size_x, window_size_y, window_fullscreen, name);
	#endif 

	#ifdef BETRAY_GLUT_SYSTEM_WRAPPER
	b_glut_init_display(argc, argv, window_size_x, window_size_y, window_fullscreen, name);
	#endif

	#ifdef BETRAY_GLFW_SYSTEM_WRAPPER
	b_glfw_init_display(argc, argv, window_size_x, window_size_y, window_fullscreen, name);
	#endif

	betray_get_current_time(&BGlobal.time[0], &BGlobal.time[1]);
	BGlobal.screen_mode.x_size = window_size_x;
	BGlobal.screen_mode.y_size = window_size_y;
	BGlobal.screen_mode.fullscreen = window_fullscreen;
}

BInputState *betray_get_input_state(void)
{
	return &BGlobal.input;
}

float betray_get_time(void)
{
	return BGlobal.input.time;
}

void betray_set_action_func(void (*action_func)(BInputState *data, void *user_pointer), void *user_pointer)
{
	BGlobal.action_func = action_func;
	BGlobal.action_func_data = user_pointer;
}

void betray_action(BActionMode mode)
{
	BGlobal.input.mode = mode;
	if(mode == BAM_MAIN)
	{
		if(BGlobal.action_func != NULL)
			BGlobal.action_func(&BGlobal.input, BGlobal.action_func_data);
		return;
	}
	if(mode == BAM_DRAW)
		glPushMatrix();
	if(BGlobal.action_func != NULL)
		BGlobal.action_func(&BGlobal.input, BGlobal.action_func_data);
	if(mode == BAM_DRAW)
		glPopMatrix();
}

boolean betray_get_key(uint key)
{
	uint i;
	for(i = 0; i < BGlobal.input.event_count; i++)
		if(BGlobal.input.event[i].button == key && BGlobal.input.event[i].state)
			return TRUE;
	return FALSE;
}

void betray_get_key_up_down(boolean *press, boolean *last_press, uint key)
{
	uint i;
	for(i = 0; i < BGlobal.input.event_count; i++)
	{
		if(BGlobal.input.event[i].button == key)
		{
			*last_press = *press;
			*press = BGlobal.input.event[i].state;
		}
	}
}

extern void b_glut_main_loop(void);


static char *type_in_string = NULL;
static uint type_in_alocated = 0;
static int cursor_pos = 0;
static int *cursor_pos_pointer = NULL;
static uint type_in_length = 0;
static void (*type_in_done_func)(void *user, boolean cansle) = NULL;
static void *func_param;

void betray_start_type_in(char *string, uint length, void (*done_func)(void *user, boolean cancel), uint *cursor, void *user_pointer)
{
	type_in_string = string;
	type_in_alocated = length;
	type_in_done_func = done_func;
	func_param = user_pointer;
	if(cursor == NULL)
		cursor_pos_pointer = &cursor_pos;
	else
		cursor_pos_pointer = cursor;
	cursor_pos = 0;
	for(type_in_length = 0; string[type_in_length] != 0; type_in_length++)
		;
/*	for(i = AXIS_BUTTON_VECTOR_1_X; i <= AXIS_BUTTON_VECTOR_2_Z; i++)
		out_going_data.axis_state[i] = 0;*/

}

void betray_end_type_in_mode(boolean cancel)
{
	if(type_in_done_func != NULL)
	{
		char *string;
		void (*func)(void *p, boolean c);
		string = type_in_string;
		func = type_in_done_func;
		if(func != NULL)
			func(func_param, cancel);
	}
	type_in_string = NULL;
	type_in_done_func = NULL;
	if(cursor_pos_pointer != 0)
		*cursor_pos_pointer = 0;
	cursor_pos_pointer = NULL;
}

void betray_insert_character(char character)
{
	int i;
	char temp;
/*	sprintf(type_in_string, "%u", character);
	return;
*/	if(type_in_length + 1 == type_in_alocated)
		return;
	for(i = (*cursor_pos_pointer)++; i < type_in_alocated; i++)
	{
		temp = type_in_string[i];
		type_in_string[i] = character;
		character = temp;
	}
	type_in_length++;
}

void betray_delete_character(void)
{
	int i;
	char temp , temp2 = 0;
	if(*cursor_pos_pointer == 0)
		return;
	(*cursor_pos_pointer)--;
	for(i = type_in_alocated ; i > *cursor_pos_pointer; i--)
	{
		temp = type_in_string[i - 1];
		type_in_string[i - 1] = temp2;
		temp2 = temp;
	}
	type_in_length--;
}

void betray_move_cursor(int move)
{
	(*cursor_pos_pointer) += move;
	if(*cursor_pos_pointer < 0)
		*cursor_pos_pointer = 0;

	if((*cursor_pos_pointer) > type_in_length)
		(*cursor_pos_pointer) = type_in_length;
}

static char betray_debug_text[256];

char *betray_debug(void)
{
	if(type_in_string != NULL)
		sprintf(betray_debug_text, "pos %u length %u alloc %u", *cursor_pos_pointer, type_in_length, type_in_alocated);
	else
		betray_debug_text[0] = 0;
	return betray_debug_text;
}

boolean betray_is_type_in(void)
{
	return type_in_string != NULL;
}



#if defined _WIN32

void betray_get_current_time(uint32 *seconds, uint32 *fractions)
{
	static LARGE_INTEGER frequency;
	static boolean init = FALSE;
	LARGE_INTEGER counter;

	if(!init)
	{
		init = TRUE;
		QueryPerformanceFrequency(&frequency);
	}

	QueryPerformanceCounter(&counter);
	if(seconds != NULL)
		*seconds = counter.QuadPart / frequency.QuadPart;
	if(fractions != NULL)
		*fractions = (uint32)((0xffffffffULL * (counter.QuadPart % frequency.QuadPart)) / frequency.QuadPart);
}

#else

#include <sys/time.h>

void betray_get_current_time(uint32 *seconds, uint32 *fractions)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	if(seconds != NULL)
	    *seconds = tv.tv_sec;
	if(fractions != NULL)
		*fractions = tv.tv_usec * 1E-6 * (double) (uint32)~0;
}

#endif

double betray_get_delta_time(void)
{
	return BGlobal.delta_time;
}

void betray_time_update(void)
{
	uint32 seconds, fractions;
	betray_get_current_time(&seconds, &fractions);
	BGlobal.delta_time = (double)seconds - (double)BGlobal.time[0] + ((double)fractions - (double)BGlobal.time[1]) / (double)(0xffffffff);
	BGlobal.time[0] = seconds;
	BGlobal.time[1] = fractions;
}
