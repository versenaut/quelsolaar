
#include "betray.h"

#ifdef BETRAY_GLFW_SYSTEM_WRAPPER

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static uint screen_size_x = 0;
static uint screen_size_y = 0;
static boolean mouse_warp = FALSE;

void b_glfw_reshape_func(int width, int height)
{
  screen_size_x = width;
  screen_size_y = height;
  betray_reshape_view(width, height);
}

void b_glfw_mouse_pos_func(int x, int y)
{
  BInputState *input;
  uint x_size, y_size;
  input = betray_get_input_state();
  betray_get_screen_mode(&x_size, &y_size, NULL);
  input->delta_pointer_x = -input->pointer_x;
  input->pointer_x = (double)x / (double)x_size * 2.0 - 1.0;
  input->delta_pointer_x += input->pointer_x;
  input->delta_pointer_y = -input->pointer_y;
  input->pointer_y = ((double)y_size / (double)x_size) + (double)-y / (double)x_size * 2.0;
  input->delta_pointer_y += input->pointer_y;
}

void b_glfw_mouse_button_func(int button, int state)
{
  BInputState *input;
  input = betray_get_input_state();
  if (state == GLFW_PRESS)
    input->mouse_button[button] = 1;
  else
    input->mouse_button[button] = 0;
}

void *betray_get_gl_proc_address()
{
  return glfwGetProcAddress;
}

void b_glfw_keyboard_func(int key, int state)
{
  BInputState *input;
  input = betray_get_input_state();

  if (state == GLFW_RELEASE)
  {
    if (input->event_count >= BETRAY_MAX_EVENT_COUNT)
      return;
    input->event[input->event_count].button = key;
    input->event[input->event_count].state = FALSE;
    input->event_count++;
  }
  else
  {
    if (betray_is_type_in())
    {
      switch (key)
      {
	case GLFW_KEY_LEFT:
	  betray_move_cursor(-1);
	  break;
	case GLFW_KEY_RIGHT:
	  betray_move_cursor(1);
	  break;
	case GLFW_KEY_HOME:
	  betray_move_cursor(-32000);
	  break;
	case GLFW_KEY_END:
	  betray_move_cursor(32000);
	  break;
	case GLFW_KEY_ENTER:
	  betray_end_type_in_mode(FALSE);
	  break;
	case GLFW_KEY_ESC:
	  betray_end_type_in_mode(TRUE);
	  break;
	case GLFW_KEY_DEL:
	  betray_move_cursor(1);
	  betray_delete_character();
	  break;
	case GLFW_KEY_BACKSPACE:
	  betray_delete_character();
	  break;
	case 12: /* CLEAR */
	  betray_move_cursor(-32000);
	  betray_insert_character(0);
	  break;
      }
    }
    else
    {
      if(input->event_count >= BETRAY_MAX_EVENT_COUNT)
	return;
      input->event[input->event_count].button = key;
      input->event[input->event_count].state = TRUE;
      input->event_count++;
    }
  }
}

void b_glfw_char_func(int character, int state)
{
  if (betray_is_type_in() && state == GLFW_PRESS)
    betray_insert_character(character);
}

extern void betray_time_update(void);

boolean b_glfw_system_wrapper_set_display(uint x_size, uint y_size, boolean fullscreen)
{
  glfwSetWindowSize(x_size, y_size);
  return TRUE;
}

boolean b_glfw_init_display(int argc, char **argv, uint size_x, uint size_y, boolean full_screen, char *caption)
{
  if (!glfwInit())
    return FALSE;

  if (!glfwOpenWindow(size_x, size_y, 8, 8, 8, 0, 32, 8, full_screen ? GLFW_FULLSCREEN : GLFW_WINDOW))
    return FALSE;

  glfwSetWindowTitle(caption);
  glfwSwapInterval(1);
  glfwEnable(GLFW_KEY_REPEAT);
  glfwSetMousePosCallback(b_glfw_mouse_pos_func);
  glfwSetMouseButtonCallback(b_glfw_mouse_button_func);
  glfwSetWindowSizeCallback(b_glfw_reshape_func);
  glfwSetKeyCallback(b_glfw_keyboard_func);
  glfwSetCharCallback(b_glfw_char_func);

  return TRUE;
}

void betray_set_mouse_warp(boolean warp)
{
	mouse_warp = warp;
}

void betray_launch_main_loop(void)
{
  BInputState *input;
  input = betray_get_input_state();

  while (glfwGetWindowParam(GLFW_OPENED))
  {
    if(mouse_warp)
    {
      input->pointer_x = 0;
      input->pointer_y = 0;
    }

    glfwPollEvents();

    if(mouse_warp)
      glfwSetMousePos(screen_size_x / 2, screen_size_y / 2);

    if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == FALSE)
    {
      input->click_pointer_x = input->pointer_x;
      input->click_pointer_y = input->pointer_y;
    }

    betray_time_update();
    betray_action(BAM_EVENT);
    betray_action(BAM_DRAW);
    input->event_count = 0;
    input->delta_pointer_x = 0;
    input->delta_pointer_y = 0;
    input->last_mouse_button[0] = input->mouse_button[0];
    input->last_mouse_button[1] = input->mouse_button[1];
    input->last_mouse_button[2] = input->mouse_button[2];
    glfwSwapBuffers();
    betray_action(BAM_MAIN);
  }
}

#endif /*BETRAY_GLFW_SYSTEM_WRAPPER*/

