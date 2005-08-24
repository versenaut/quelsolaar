#include <math.h>
#include <stdlib.h>

#include "seduce.h"

#define S_POPUP_ANGLE_END_SECTIONS 32
#define S_POPUP_INNER 0.006
#define S_POPUP_OUTER 0.01
#define S_POPUP_SELECT 0.001
#define S_POPUP_SLIDE 0.005
#define S_POPUP_SLIDE2 (S_POPUP_INNER * 2 - S_POPUP_OUTER)

extern void sw_draw_background_ring(float pos_x, float pos_y, float color);

void sui_draw_popup_top_bottom(float pos_x, float pos_y, float scale, float color)
{
	static float array[] = {0.060000, 0.040000, 0.075307, 0.035433, 0.075307, 0.035433, 0.088284, 0.022426, 0.088284, 0.022426, 0.096955, 0.002961, 0.096955, 0.002961, 0.100000, -0.020000, -0.060000, 0.040000, -0.075307, 0.035433, -0.075307, 0.035433, -0.088284, 0.022426, -0.088284, 0.022426, -0.096955, 0.002961, -0.096955, 0.002961, -0.100000, -0.020000, -0.000000, -0.600000, -0.022252, -0.585458, -0.022252, -0.585458, -0.043388, -0.542562, -0.043388, -0.542562, -0.062349, -0.473462, -0.062349, -0.473462, -0.078183, -0.381624, -0.078183, -0.381624, -0.090097, -0.271653, -0.090097, -0.271653, -0.097493, -0.149062, -0.097493, -0.149062, -0.100000, -0.020000, -0.000000, -0.600000, 0.022252, -0.585458, 0.022252, -0.585458, 0.043388, -0.542562, 0.043388, -0.542562, 0.062349, -0.473462, 0.062349, -0.473462, 0.078183, -0.381624, 0.078183, -0.381624, 0.090097, -0.271653, 0.090097, -0.271653, 0.097493, -0.149062, 0.097493, -0.149062, 0.100000, -0.020000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	glScalef(1, scale, 1);
	sui_draw_gl(GL_LINES, array, 44, 2, color, color, color);
	glPopMatrix();
}


void sui_draw_popup_angle_line(float x, float y, float angle)
{
	static float length_sides[16] = {0, S_POPUP_OUTER, 1, S_POPUP_OUTER, 1, S_POPUP_INNER, 0, S_POPUP_INNER, 0, -S_POPUP_INNER, 1, -S_POPUP_INNER, 1, -S_POPUP_OUTER, 0, -S_POPUP_OUTER};
	static float length_mid[8] = {0, S_POPUP_INNER, 1, S_POPUP_INNER, 1, -S_POPUP_INNER, 0, -S_POPUP_INNER};
	static boolean init = FALSE;
	static float end_sides[S_POPUP_ANGLE_END_SECTIONS * 8];
	static float end_mid[S_POPUP_ANGLE_END_SECTIONS * 6];
	
	if(init == FALSE)
	{
		float x, y, next_x, next_y;
		uint i;
		x = 0;
		y = 1;
		init = TRUE;
		for(i = 0; i < S_POPUP_ANGLE_END_SECTIONS; i++)
		{
			next_x = -sin((double)(i + 1) / S_POPUP_ANGLE_END_SECTIONS * PI);
			next_y = cos((double)(i + 1) / S_POPUP_ANGLE_END_SECTIONS * PI);
			end_sides[i * 8 + 0] = x * S_POPUP_OUTER;
			end_sides[i * 8 + 1] = y * S_POPUP_OUTER;
			end_sides[i * 8 + 2] = x * S_POPUP_INNER;
			end_sides[i * 8 + 3] = y * S_POPUP_INNER;
			end_sides[i * 8 + 4] = next_x * S_POPUP_INNER;
			end_sides[i * 8 + 5] = next_y * S_POPUP_INNER;
			end_sides[i * 8 + 6] = next_x * S_POPUP_OUTER;
			end_sides[i * 8 + 7] = next_y * S_POPUP_OUTER;
			end_mid[i * 6 + 0] = x * S_POPUP_INNER;
			end_mid[i * 6 + 1] = y * S_POPUP_INNER;
			end_mid[i * 6 + 2] = 0;
			end_mid[i * 6 + 3] = 0;
			end_mid[i * 6 + 4] = next_x * S_POPUP_INNER;
			end_mid[i * 6 + 5] = next_y * S_POPUP_INNER;
			x = next_x;
			y = next_y;
		}
	}

	glPushMatrix();
	glTranslatef(x, y, 0);
	glRotatef(angle - 90, 0, 0, 1);
	glTranslatef(0.1, 0, 0);
	sui_draw_gl(GL_QUADS, end_sides, S_POPUP_ANGLE_END_SECTIONS * 4, 2, 0, 0, 0);
	sui_draw_gl(GL_TRIANGLES, end_mid, S_POPUP_ANGLE_END_SECTIONS * 3, 2, 1, 1, 1);
	glPushMatrix();

	
	glScalef(0.15, 1, 1);
	sui_draw_gl(GL_QUADS, length_sides, 8, 2, 0, 0, 0);
	sui_draw_gl(GL_QUADS, length_mid, 4, 2, 1, 1, 1);
	glPopMatrix();
	glTranslatef(0.15, 0, 0);
	glScalef(-1, 1, 1);
	sui_draw_gl(GL_QUADS, end_sides, S_POPUP_ANGLE_END_SECTIONS * 4, 2, 0, 0, 0);
	sui_draw_gl(GL_TRIANGLES, end_mid, S_POPUP_ANGLE_END_SECTIONS * 3, 2, 1, 1, 1);

	glPopMatrix();

}

void sui_draw_popup_square(float pos_x, float pos_y, float size_x, float size_y, float color)
{
	static float color_array[4 * 4] = {0, 0, 0, 0.4, 0, 0, 0, 0.4, 0, 0, 0, 0.4, 0, 0, 0, 0.4};
	float array[8];

	if(color_array[3] != color)
	{
		uint i;
		for(i = 0; i < 4; i++)
			sui_draw_set_vec4(color_array, i, color, color, color, 0.4);
	}
	array[0] = pos_x;
	array[1] = pos_y;
	array[2] = pos_x + size_x;
	array[3] = pos_y;
	array[4] = pos_x + size_x;
	array[5] = pos_y + size_y;
	array[6] = pos_x;
	array[7] = pos_y + size_y;
	sui_set_blend_gl(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	sui_set_color_array_gl(color_array, 8, 4);
	sui_draw_gl(GL_QUADS, array, 4, 2, color, color, color);
}



/*

typedef enum{
	PU_T_TOP,
	PU_T_BOTTOM,
	PU_T_ANGLE,
	PU_T_VSLIDER,
	PU_T_HSLIDER,
	PU_T_SQUARE,
	PU_T_ICON
}SUIPUType;

typedef struct{
	SUIPUType	type;
	char		*name;
	union{
		float angle[2];
		struct{
			float pos[2];
			float length;
			double *value;
		}slider;
		struct{
			float square[4];
			void (*draw_func)(float pos_x, float pos_y, float size_x, float size_y);
		}square;
		struct{
			float pos[2];
			float size;
			void (*draw_func)(float pos_x, float pos_y);
		}icon;
	}data;
}SUIPUElement;
*/

uint sui_draw_popup(BInputState *input, float pos_x, float pos_y, SUIPUElement *element, uint element_count, uint button, float back_color)
{
	uint i, j, top = 0, bottom = 0;

	if(input->mode == BAM_DRAW)
	{
		back_color = 1 - back_color;
		for(i = 0; i < element_count && element[i].type != PU_T_ANGLE; i++);
		if(i < element_count)
			sw_draw_background_ring(pos_x, pos_y, 0);

		for(i = 0; i < element_count; i++)
			if(element[i].type == PU_T_TOP)
				top++;
		sui_draw_popup_square(pos_x - 0.125, pos_y + 0.075, 0.25, 0.05 * (float)(top), 1 - back_color);
		top = 0;

		for(i = 0; i < element_count; i++)
			if(element[i].type == PU_T_BOTTOM)
				bottom++;
		sui_draw_popup_square(pos_x - 0.125, pos_y - 0.075, 0.25, -0.05 * (float)(bottom), 1 - back_color);
		bottom = 0;

		for(i = 0; i < element_count; i++)
		{
			switch(element[i].type)
			{
				case PU_T_TOP :
					if(sui_box_click_test(pos_x - 0.125, pos_y + (float)(top) * 0.05 + 0.075, 0.25, 0.05))
						sui_draw_popup_square(pos_x - 0.125, pos_y + (float)(top) * 0.05 + 0.125, 0.25, -0.05, 1 - back_color);
					sui_draw_text(pos_x + sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, element[i].text) * -0.5, pos_y + (float)(top++) * 0.05 + 0.1 - SUI_T_SIZE, SUI_T_SIZE, SUI_T_SPACE, element[i].text, back_color, back_color, back_color);
				break;
				case PU_T_BOTTOM :
					if(sui_box_click_test(pos_x - 0.125, pos_y + (float)(bottom) * -0.05 - 0.125, 0.25, 0.05))
						sui_draw_popup_square(pos_x - 0.125, pos_y + (float)(bottom) * -0.05 - 0.075, 0.25, -0.05, 1 - back_color);
					sui_draw_text(pos_x + sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, element[i].text) * -0.5, pos_y + (float)(bottom++) * -0.05 - 0.1 - SUI_T_SIZE, SUI_T_SIZE, SUI_T_SPACE, element[i].text, back_color, back_color, back_color);
				break;
				case PU_T_ANGLE :
					for(j = 0; j < i; j++)
						if(element[i].type == PU_T_ANGLE && ((element[i].data.angle[0] - 1 < element[j].data.angle[0] && element[i].data.angle[0] + 1 > element[j].data.angle[0]) || (element[i].data.angle[0] - 1 < element[j].data.angle[1] && element[i].data.angle[0] + 1 > element[j].data.angle[1])))
							break;
					if(j == i)
						sui_draw_popup_angle_line(pos_x, pos_y, element[i].data.angle[0]);

					for(j = 0; j < i; j++)
						if(element[i].type == PU_T_ANGLE && ((element[i].data.angle[1] - 1 < element[j].data.angle[0] && element[i].data.angle[1] + 1 > element[j].data.angle[0]) || (element[i].data.angle[1] - 1 < element[j].data.angle[1] && element[i].data.angle[1] + 1 > element[j].data.angle[1])))
							break;
					if(j == i)
						sui_draw_popup_angle_line(pos_x, pos_y, element[i].data.angle[1]);

					if(element[i].data.angle[0] < 0)
					{
						sui_draw_text(pos_x + sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, element[i].text) * -0.5, pos_y + 0.15, SUI_T_SIZE, SUI_T_SPACE, element[i].text, back_color, back_color, back_color);
					}else if(element[i].data.angle[0] < 180 && element[i].data.angle[1] > 180)
					{
						sui_draw_text(pos_x + sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, element[i].text) * -0.5, pos_y - 0.15, SUI_T_SIZE, SUI_T_SPACE, element[i].text, back_color, back_color, back_color);
					}else if(element[i].data.angle[0] < 180)
					{
						glPushMatrix();
						glTranslatef(pos_x, pos_y, 0);
						glRotatef(90 - (element[i].data.angle[0] + element[i].data.angle[1]) * 0.5, 0, 0, 1);
						sui_draw_text(0.15, -SUI_T_SIZE, SUI_T_SIZE, SUI_T_SPACE, element[i].text, back_color, back_color, back_color);
						glPopMatrix();
					}else if(element[i].data.angle[0] > 180)
					{
						glPushMatrix();
						glTranslatef(pos_x, pos_y, 0);
						glRotatef(270 - (element[i].data.angle[0] + element[i].data.angle[1]) * 0.5, 0, 0, 1);
						sui_draw_text(-0.15 - sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, element[i].text), -SUI_T_SIZE, SUI_T_SIZE, SUI_T_SPACE, element[i].text, back_color, back_color, back_color);
						glPopMatrix();
					}
					break;
				case PU_T_SQUARE :
					if(element[i].data.square.draw_func == NULL)
						sui_draw_popup_square(pos_x + element[i].data.square.square[0], pos_y + element[i].data.square.square[1], element[i].data.square.square[2], element[i].data.square.square[3], 1 - back_color);
					else
						element[i].data.square.draw_func(pos_x + element[i].data.square.square[0], pos_y + element[i].data.square.square[1], element[i].data.square.square[2], element[i].data.square.square[3], 1 - back_color);		

					if(sui_box_click_test(pos_x + element[i].data.square.square[0], pos_y + element[i].data.square.square[1] + element[i].data.square.square[3], element[i].data.square.square[2], -element[i].data.square.square[3]))
						sui_draw_popup_square(pos_x + element[i].data.square.square[0] - 0.005, pos_y + element[i].data.square.square[1] + 0.005, element[i].data.square.square[2] + 0.01, element[i].data.square.square[3] - 0.01, 1 - back_color);

					sui_draw_text(pos_x + element[i].data.square.square[0] + element[i].data.square.square[2] * 0.5 - sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, element[i].text) * 0.5, -SUI_T_SIZE + pos_y + element[i].data.square.square[1] + element[i].data.square.square[3] * 0.5, SUI_T_SIZE, SUI_T_SPACE, element[i].text, back_color, back_color, back_color);
					break;
				case PU_T_ICON :
					element[i].data.icon.draw_func(element[i].data.icon.pos[0], element[i].data.icon.pos[1], 1 - back_color);
				break;
			}
		}
	}else if(input->mouse_button[button] == FALSE && input->last_mouse_button[button] == TRUE)
	{
		float angle = 1000;
		if(0.001 < (pos_x - input->pointer_x) * (pos_x - input->pointer_x) + (pos_y - input->pointer_y) * (pos_y - input->pointer_y))
			angle = (atan2(pos_x - input->pointer_x, pos_y - input->pointer_y) / PI + 1) * 180;
		for(i = 0; i < element_count; i++)
		{
			switch(element[i].type)
			{
				case PU_T_TOP :
					if(sui_box_click_test(pos_x - 0.125, pos_y + (float)(top++) * 0.05 + 0.075, 0.25, 0.05))
						return i;
					break;
				case PU_T_BOTTOM :
					if(sui_box_click_test(pos_x - 0.125, pos_y + (float)(bottom++) * -0.05 - 0.125, 0.25, 0.05))
						return i;
					break;
				case PU_T_ANGLE :
					if((element[i].data.angle[0] < angle && element[i].data.angle[1] > angle) || (element[i].data.angle[0] - 360 < angle && element[i].data.angle[1] - 360 > angle))
						return i;
					break;
				case PU_T_SQUARE :
					if(sui_box_click_test(pos_x + element[i].data.square.square[0], pos_y + element[i].data.square.square[1] + element[i].data.square.square[3], element[i].data.square.square[2], -element[i].data.square.square[3]))
						return i;
					break;
				case PU_T_ICON :
					if(element[i].data.icon.size * element[i].data.icon.size > (element[i].data.icon.pos[0] + pos_x - input->pointer_x) * (element[i].data.icon.pos[0] + pos_x - input->pointer_x) + (element[i].data.icon.pos[1] + pos_y - input->pointer_y) * (element[i].data.icon.pos[1] + pos_y - input->pointer_y))
						return i;
				break;
			}
		}
	}
	return -1;
}
