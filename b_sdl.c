#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "st_types.h"
#include "betray.h"

#ifdef BETRAY_SDL_SYSTEM_WRAPPER

static boolean	input_focus = TRUE;
static uint	screen_size_x = 800;
static uint	screen_size_y = 600;
static boolean	mouse_warp = FALSE;

extern void sui_get_input(BInputState *data);

boolean b_sdl_system_wrapper_set_display(uint size_x, uint size_y, boolean full_screen) 
{  
	uint video_flags;
	video_flags = SDL_OPENGL;

	if(full_screen)
		video_flags |= SDL_FULLSCREEN;
	video_flags |= SDL_RESIZABLE;
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8) ;
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	screen_size_x = size_x;
	screen_size_y = size_y;

	return SDL_SetVideoMode(size_x, size_y, 0, video_flags) == NULL;
}

void (*sdl_context_func)(void) = NULL;

void betray_set_context_update_func(void (*context_func)(void))
{
	sdl_context_func = context_func;
}

void b_sdl_init_display(uint size_x, uint size_y, boolean full_screen, const char *caption)
{  
	if(SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	if(b_sdl_system_wrapper_set_display(size_x, size_y, full_screen))
	{
		size_x /= 2;
		size_y /= 2;
		if(b_sdl_system_wrapper_set_display(size_x, size_y, FALSE))
		{
			fprintf(stderr, "Unable to open %ux%u display: %s\n", size_x, size_y, SDL_GetError());
			exit(EXIT_FAILURE);
		}
	}
	betray_reshape_view(size_x, size_y);
	SDL_WM_SetCaption(caption, NULL);
	SDL_EnableUNICODE(TRUE); 
	SDL_ShowCursor(TRUE);
}

void system_wrapper_endframe(void)
{ 
	static uint32 timer = 0;
	uint32 update;
	SDL_GL_SwapBuffers();
	update = SDL_GetTicks();
	if ((timer - update) < 10)
	{
	  SDL_Delay(5);
	}
	timer = update;
}

void system_wrapper_lose_focus(void)
{
	input_focus = FALSE;
	SDL_ShowCursor(TRUE);
}

void betray_set_mouse_warp(boolean warp)
{
	mouse_warp = warp;
}

static uint system_wrapper_eventloop(BInputState *input)
{
	SDL_Event	event;
	uint		i;

/*	if(out_going_data.clear == TRUE)
	{
		for(i = 0; i < AXIS_COUNT; i++)
		{
			out_going_data.axis_update_flag[i] = FALSE;
		}
		out_going_data.event_count = 0;
		out_going_data.clear = FALSE;
	}
	else	
		return 0;
*/
	if(mouse_warp)
	{
		input->pointer_x = 0;
		input->pointer_y = 0;
	}

	while(SDL_PollEvent(&event)) 
	{
		switch(event.type)
		{
			case SDL_ACTIVEEVENT:
			if(event.active.state & SDL_APPACTIVE)
			{
				if(event.active.gain) 
				{
					input_focus = TRUE;
	/*				SDL_ShowCursor(FALSE);*/
				}
				else
				{
					input_focus = FALSE;
	/*				SDL_ShowCursor(TRUE);*/
				}
			}
			break;
 
			case SDL_MOUSEBUTTONUP: 
			{
				if(event.button.button == SDL_BUTTON_LEFT)
					input->mouse_button[0] = FALSE;
				else if(event.button.button == SDL_BUTTON_MIDDLE)
					input->mouse_button[2] = FALSE;
				else if(event.button.button == SDL_BUTTON_RIGHT)
					input->mouse_button[1] = FALSE;
			}
			break;

			case SDL_MOUSEBUTTONDOWN: 
			{				
				if(event.button.button == SDL_BUTTON_LEFT)
					input->mouse_button[0] = TRUE;
				else if(event.button.button == SDL_BUTTON_MIDDLE)
					input->mouse_button[2] = TRUE;
				else if(event.button.button == SDL_BUTTON_RIGHT)
					input->mouse_button[1] = TRUE;
			}
			break;
			case SDL_MOUSEMOTION: 
			{
				uint x_size, y_size;
				betray_get_screen_mode(&x_size, &y_size, NULL);
				input->delta_pointer_x = -input->pointer_x;
				input->pointer_x = (double)event.motion.x / (double)x_size * 2.0 - 1.0;
				input->delta_pointer_x += input->pointer_x;
				input->delta_pointer_y = -input->pointer_y;
				input->pointer_y = ((double)y_size / (double)x_size) + (double)-event.motion.y / (double)x_size * 2.0;
				input->delta_pointer_y += input->pointer_y;
			}
			break;
			case SDL_QUIT:
				return FALSE;
			break;
			case SDL_VIDEORESIZE :
				b_sdl_system_wrapper_set_display(event.resize.w, event.resize.h, FALSE);
				betray_reshape_view(event.resize.w, event.resize.h);
				if(sdl_context_func != NULL)
					sdl_context_func();
			break;
			case SDL_KEYDOWN: 
			{
				if(betray_is_type_in())
				{
					if(event.key.keysym.sym == SDLK_RETURN)
						betray_end_type_in_mode(FALSE);
					else if(event.key.keysym.sym == SDLK_ESCAPE)
						betray_end_type_in_mode(TRUE);
					else if(event.key.keysym.sym == SDLK_DELETE)
					{
						betray_move_cursor(1);
						betray_delete_character();
					}
					else if(event.key.keysym.sym == SDLK_RIGHT)
						betray_move_cursor(1);
					else if(event.key.keysym.sym == SDLK_LEFT)
						betray_move_cursor(-1);
					else if(event.key.keysym.sym == SDLK_END)
						betray_move_cursor(32000);
					else if(event.key.keysym.sym == SDLK_HOME)
						betray_move_cursor(-32000);
					else if(event.key.keysym.sym == SDLK_BACKSPACE)
					{
						betray_delete_character();
					}
					else if(event.key.keysym.sym == SDLK_CLEAR)
					{
						betray_move_cursor(-32000);
						betray_insert_character(0);
					}
					else if(event.key.keysym.unicode > 0 && event.key.keysym.unicode < 128)
					{
						betray_insert_character(event.key.keysym.unicode);
					}
				}
				else
				{
					if(input->event_count < BETRAY_MAX_EVENT_COUNT)
					{
						input->event[input->event_count].state = TRUE;
						input->event[input->event_count++].button = event.key.keysym.sym;
					}
				}
			}
			break;
			case SDL_KEYUP: 
			{
				if(!betray_is_type_in() && input->event_count < BETRAY_MAX_EVENT_COUNT)
				{
					input->event[input->event_count].state = FALSE;
					input->event[input->event_count++].button = event.key.keysym.sym;
				}
			}
			break;
		}
	}
	if(mouse_warp)
		SDL_WarpMouse(screen_size_x / 2, screen_size_y / 2);
	return TRUE;
}

void *betray_get_gl_proc_address()
{
	return &SDL_GL_GetProcAddress;
}

extern void betray_time_update(void);

void betray_launch_main_loop(void)
{
	BInputState *input;
	input = betray_get_input_state();
	while(system_wrapper_eventloop(input))
	{
		if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == FALSE)
		{
			input->click_pointer_x = input->pointer_x;
			input->click_pointer_y = input->pointer_y;
		}
		betray_time_update();
		betray_action(BAM_EVENT);	
		betray_action(BAM_DRAW);
		input->delta_pointer_x = 0;
		input->delta_pointer_y = 0;	
		input->event_count = 0;
		SDL_GL_SwapBuffers();
		input->time++;
		input->last_mouse_button[0] = input->mouse_button[0];
		input->last_mouse_button[1] = input->mouse_button[1];
		input->last_mouse_button[2] = input->mouse_button[2];
		betray_action(BAM_MAIN);
	}
}

#endif
