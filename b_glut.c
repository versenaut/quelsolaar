
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "betray.h"

#ifdef BETRAY_GLUT_SYSTEM_WRAPPER

/*
	SUIAM_DRAW,
	SUIAM_EVENT,
	SUIAM_MAIN
*/

void b_glut_idle_func(void)
{
	betray_action(BAM_MAIN);
    glutPostRedisplay();
}

void b_glut_reshape_func(int width, int height)
{
	betray_reshape_view(width, height);
}

void b_glut_passive_mouse_func(int x, int y)
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


void b_glut_mouse_func(int button, int state, int x, int y)
{
	BInputState *input;
	input = betray_get_input_state();
	b_glut_passive_mouse_func(x, y);
	if(button == GLUT_MIDDLE_BUTTON)
		input->mouse_button[2] = !state;
	else if(button == GLUT_RIGHT_BUTTON)
		input->mouse_button[1] = !state;
	else
		input->mouse_button[button] = !state;

}
//glutSpecialFunc(void (GLUTCALLBACK *func)(int key, int x, int y));

void betray_insert_caracter_b(char caracter);

void b_glut_special_keyboard_func(int key, int x, int y)
{
	BInputState *input;
	input = betray_get_input_state();
	b_glut_passive_mouse_func(x, y);
	if(betray_is_type_in())
	{
/*		betray_insert_caracter_b(key);
		return;
*/		if(key == GLUT_KEY_LEFT)
			betray_move_cursor(-1);
		else if(key == GLUT_KEY_RIGHT)
			betray_move_cursor(1);
		else if(key == GLUT_KEY_HOME)
			betray_move_cursor(-32000);
		else if(key == GLUT_KEY_END)
			betray_move_cursor(32000);
	}
}

void b_glut_keyboard_func(unsigned char key, int x, int y)
{
	BInputState *input;
	input = betray_get_input_state();
	b_glut_passive_mouse_func(x, y);

	if(betray_is_type_in())
	{
/*		betray_insert_caracter(key);
		return;
*/		if(key == 13) /* RETURN */
			betray_end_type_in_mode(FALSE);
		else if(key == 27) /* ESCAPE */
			betray_end_type_in_mode(TRUE);
		else if(key == 127) /* DELETE */
		{
			betray_move_cursor(1);
			betray_delete_caracter();
		}
		else if(key == 8) /* MACKSPACE */
		{
			betray_delete_caracter();
		}
		else if(key == 12) /* CLEAR */
		{
			betray_move_cursor(-32000);
			betray_insert_caracter(0);
		}else
			betray_insert_caracter(key);
	}else
	{
		if(input->event_count >= MAX_EVENT_COUNT)
			return;
		input->event[input->event_count].button = key;
		input->event[input->event_count].state = TRUE;
		input->event_count++;
	}
}

void b_glut_keyboard_up_func(unsigned char key, int x, int y)
{
	BInputState *input;
	input = betray_get_input_state();
	b_glut_passive_mouse_func(x, y);
	if(input->event_count >= MAX_EVENT_COUNT)
		return;
	input->event[input->event_count].button = key;
	input->event[input->event_count].state = FALSE;
	input->event_count++;
}

/*
if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 13)
GLUTAPI void APIENTRY glutKeyboardUpFunc(void (GLUTCALLBACK *func)(unsigned char key, int x, int y));
GLUTAPI void APIENTRY glutSpecialUpFunc(void (GLUTCALLBACK *func)(int key, int x, int y));
GLUTAPI void APIENTRY glutJoystickFunc(void (GLUTCALLBACK *func)(unsigned int buttonMask, int x, int y, int z), int pollInterval);
*/

extern void betray_time_update();

void b_glut_display_func(void)
{
	BInputState *input;
	input = betray_get_input_state();
	if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == FALSE)
	{
		input->click_pointer_x = input->pointer_x;
		input->click_pointer_y = input->pointer_y;
	}
	betray_time_update();
	betray_action(BAM_EVENT);
	betray_action(BAM_DRAW);
	input->event_count = 0;
	glutSwapBuffers();
	input->delta_pointer_x = 0;
	input->delta_pointer_y = 0;
	input->last_mouse_button[0] = input->mouse_button[0];
	input->last_mouse_button[1] = input->mouse_button[1];
	input->last_mouse_button[2] = input->mouse_button[2];
}

/*
glutWarpPointer(int x, int y);
*/

boolean b_glut_system_wrapper_set_display(uint x_size, uint y_size, boolean fullscreen)
{
	glutReshapeWindow(x_size, y_size);
	return TRUE;
}

boolean b_glut_init_display(int argc, char **argv, uint size_x, uint size_y, boolean full_screen, char *caption)
{
	 glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
//	glutInitDisplayString("stencil>=1 rgb depth double");
	glutInitWindowPosition(10, 10);
	glutInitWindowSize(size_x, size_y);
//	glutInitWindowSize(100, 100);
	glutCreateWindow(caption);
	glutDisplayFunc(b_glut_display_func);
	glutReshapeFunc(b_glut_reshape_func);
	glutIdleFunc(b_glut_idle_func);
	glutKeyboardFunc(b_glut_keyboard_func);
	glutSpecialFunc(b_glut_special_keyboard_func);
	glutPassiveMotionFunc(b_glut_passive_mouse_func);
	glutMotionFunc(b_glut_passive_mouse_func);
	glutMouseFunc(b_glut_mouse_func);
	glutKeyboardUpFunc(b_glut_keyboard_up_func);
	return TRUE;
}

void betray_launch_main_loop()
{

	glutMainLoop();
}

#endif
