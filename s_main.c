#include "seduce.h"

extern void sui_init_background(void);

void sui_init(void)
{
	sui_init_background();
}

boolean sui_box_click_test(float x_pos, float y_pos, float x_size, float y_size)
{
	BInputState *input;
	input = betray_get_input_state();
	if(input->pointer_x < x_pos)
		return FALSE;
	if(input->pointer_y < y_pos)
		return FALSE;
	if(input->pointer_x > x_pos + x_size)
		return FALSE;
	if(input->pointer_y > y_pos + y_size)
		return FALSE;
	return TRUE;
}

boolean sui_box_down_click_test(float x_pos, float y_pos, float x_size, float y_size)
{
	BInputState *input;
	input = betray_get_input_state();
	if(input->click_pointer_x < x_pos)
		return FALSE;
	if(input->click_pointer_y < y_pos)
		return FALSE;
	if(input->click_pointer_x > x_pos + x_size)
		return FALSE;
	if(input->click_pointer_y > y_pos + y_size)
		return FALSE;
	return TRUE;
}

void sui_draw_pointer(float x, float y)
{
/*	glPushMatrix();
	glTranslatef(x - 0.004, y - 0.004, -1);
	glDisable(GL_DEPTH_TEST);
	nglDrawArray(NGL_QUADS_FILLED, SUIGlobal.pointer_shadow, &SUIGlobal.pointer_color, 1, SUIGlobal.material[SUIM_COLOR_PARAM], 0);
	nglDrawArray(NGL_TRIANGLES_FILLED, SUIGlobal.pointer, NULL, 0, SUIGlobal.material[SUIM_WHITE_LIGHT], 0);
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	glPushMatrix();*/
}

