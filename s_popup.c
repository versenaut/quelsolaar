#include <math.h>
#include <stdlib.h>

#include "seduce.h"

void sui_draw_popup_top_bottom(float pos_x, float pos_y, float scale, float red, float green, float blue)
{
	static float array[] = {0.060000, 0.040000, 0.075307, 0.035433, 0.075307, 0.035433, 0.088284, 0.022426, 0.088284, 0.022426, 0.096955, 0.002961, 0.096955, 0.002961, 0.100000, -0.020000, -0.060000, 0.040000, -0.075307, 0.035433, -0.075307, 0.035433, -0.088284, 0.022426, -0.088284, 0.022426, -0.096955, 0.002961, -0.096955, 0.002961, -0.100000, -0.020000, -0.000000, -0.600000, -0.022252, -0.585458, -0.022252, -0.585458, -0.043388, -0.542562, -0.043388, -0.542562, -0.062349, -0.473462, -0.062349, -0.473462, -0.078183, -0.381624, -0.078183, -0.381624, -0.090097, -0.271653, -0.090097, -0.271653, -0.097493, -0.149062, -0.097493, -0.149062, -0.100000, -0.020000, -0.000000, -0.600000, 0.022252, -0.585458, 0.022252, -0.585458, 0.043388, -0.542562, 0.043388, -0.542562, 0.062349, -0.473462, 0.062349, -0.473462, 0.078183, -0.381624, 0.078183, -0.381624, 0.090097, -0.271653, 0.090097, -0.271653, 0.097493, -0.149062, 0.097493, -0.149062, 0.100000, -0.020000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	glScalef(1, scale, 1);
	sui_draw_gl(GL_LINES, array, 44, 2, red, green, blue);
	glPopMatrix();
}




void sui_draw_popup_line(float pos_x, float pos_y, float dest_x, float dest_y, float red, float green, float blue)
{
	float array[48], vec_x, vec_y, r;
	vec_x = dest_x - pos_x;
	vec_y = dest_y - pos_y;
	r = sqrt(vec_x * vec_x + vec_y * vec_y);
	vec_x /= r;
	vec_y /= r;
	array[0] = dest_x + -0.002500 * vec_x + -0.005000 * vec_y;
	array[1] = dest_y + -0.002500 * vec_y + -0.005000 * -vec_x;
	array[2] = dest_x + -0.000732 * vec_x + -0.004268 * vec_y;
	array[3] = dest_y + -0.000732 * vec_y + -0.004268 * -vec_x;
	array[4] = dest_x + -0.000732 * vec_x + -0.004268 * vec_y;
	array[5] = dest_y + -0.000732 * vec_y + -0.004268 * -vec_x;
	array[6] = dest_x + 0.000000 * vec_x + -0.002500 * vec_y;
	array[7] = dest_y + 0.000000 * vec_y + -0.002500 * -vec_x;
	array[8] = dest_x + -0.002500 * vec_x + 0.005000 * vec_y;
	array[9] = dest_y + -0.002500 * vec_y + 0.005000 * -vec_x;
	array[10] = dest_x + -0.000732 * vec_x + 0.004268 * vec_y;
	array[11] = dest_y + -0.000732 * vec_y + 0.004268 * -vec_x;
	array[12] = dest_x + -0.000732 * vec_x + 0.004268 * vec_y;
	array[13] = dest_y + -0.000732 * vec_y + 0.004268 * -vec_x;
	array[14] = dest_x + 0.000000 * vec_x + 0.002500 * vec_y;
	array[15] = dest_y + 0.000000 * vec_y + 0.002500 * -vec_x;
	array[16] = pos_x + 0.002500 * vec_x + 0.005000 * vec_y;
	array[17] = pos_y + 0.002500 * vec_y + 0.005000 * -vec_x;
	array[18] = pos_x + 0.000732 * vec_x + 0.004268 * vec_y;
	array[19] = pos_y + 0.000732 * vec_y + 0.004268 * -vec_x;
	array[20] = pos_x + 0.000732 * vec_x + 0.004268 * vec_y;
	array[21] = pos_y + 0.000732 * vec_y + 0.004268 * -vec_x;
	array[22] = pos_x + -0.000000 * vec_x + 0.002500 * vec_y;
	array[23] = pos_y + -0.000000 * vec_y + 0.002500 * -vec_x;
	array[24] = pos_x + 0.002500 * vec_x + -0.005000 * vec_y;
	array[25] = pos_y + 0.002500 * vec_y + -0.005000 * -vec_x;
	array[26] = pos_x + 0.000732 * vec_x + -0.004268 * vec_y;
	array[27] = pos_y + 0.000732 * vec_y + -0.004268 * -vec_x;
	array[28] = pos_x + 0.000732 * vec_x + -0.004268 * vec_y;
	array[29] = pos_y + 0.000732 * vec_y + -0.004268 * -vec_x;
	array[30] = pos_x + -0.000000 * vec_x + -0.002500 * vec_y;
	array[31] = pos_y + -0.000000 * vec_y + -0.002500 * -vec_x;
	array[32] = pos_x + -0.000000 * vec_x + 0.002500 * vec_y;
	array[33] = pos_y + -0.000000 * vec_y + 0.002500 * -vec_x;
	array[34] = pos_x + -0.000000 * vec_x + -0.002500 * vec_y;
	array[35] = pos_y + -0.000000 * vec_y + -0.002500 * -vec_x;
	array[36] = pos_x + 0.002500 * vec_x + -0.005000 * vec_y;
	array[37] = pos_y + 0.002500 * vec_y + -0.005000 * -vec_x;
	array[38] = dest_x + -0.002500 * vec_x + -0.005000 * vec_y;
	array[39] = dest_y + -0.002500 * vec_y + -0.005000 * -vec_x;
	array[40] = dest_x + 0.000000 * vec_x + -0.002500 * vec_y;
	array[41] = dest_y + 0.000000 * vec_y + -0.002500 * -vec_x;
	array[42] = dest_x + 0.000000 * vec_x + 0.002500 * vec_y;
	array[43] = dest_y + 0.000000 * vec_y + 0.002500 * -vec_x;
	array[44] = dest_x + -0.002500 * vec_x + 0.005000 * vec_y;
	array[45] = dest_y + -0.002500 * vec_y + 0.005000 * -vec_x;
	array[46] = pos_x + 0.002500 * vec_x + 0.005000 * vec_y;
	array[47] = pos_y + 0.002500 * vec_y + 0.005000 * -vec_x;
	sui_draw_gl(GL_LINES, array, 24, 2, red, green, blue);
}

void sui_draw_popup_square(float pos_x, float pos_y, float size_x, float size_y, float red, float green, float blue)
{
	float array[64];
	size_x += pos_x;
	size_y += pos_y;
	array[0] = size_x + -0.005000;
	array[1] = pos_y + 0.000000;
	array[2] = pos_x + 0.005000;
	array[3] = pos_y + 0.000000;
	array[4] = pos_x + -0.000000;
	array[5] = pos_y + -0.005000;
	array[6] = pos_x + -0.000000;
	array[7] = size_y + 0.005000;
	array[8] = pos_x + 0.005000;
	array[9] = size_y + -0.000000;
	array[10] = size_x + -0.005000;
	array[11] = size_y + -0.000000;
	array[12] = size_x + 0.000000;
	array[13] = size_y + 0.005000;
	array[14] = size_x + 0.000000;
	array[15] = pos_y + -0.005000;
	array[16] = pos_x + 0.005000;
	array[17] = pos_y + 0.000000;
	array[18] = pos_x + 0.002500;
	array[19] = pos_y + -0.000670;
	array[20] = pos_x + 0.002500;
	array[21] = pos_y + -0.000670;
	array[22] = pos_x + 0.000670;
	array[23] = pos_y + -0.002500;
	array[24] = pos_x + 0.000670;
	array[25] = pos_y + -0.002500;
	array[26] = pos_x + -0.000000;
	array[27] = pos_y + -0.005000;
	array[28] = size_x + -0.005000;
	array[29] = pos_y + 0.000000;
	array[30] = size_x + -0.002500;
	array[31] = pos_y + -0.000670;
	array[32] = size_x + -0.002500;
	array[33] = pos_y + -0.000670;
	array[34] = size_x + -0.000670;
	array[35] = pos_y + -0.002500;
	array[36] = size_x + -0.000670;
	array[37] = pos_y + -0.002500;
	array[38] = size_x + 0.000000;
	array[39] = pos_y + -0.005000;
	array[40] = size_x + -0.005000;
	array[41] = size_y + -0.000000;
	array[42] = size_x + -0.002500;
	array[43] = size_y + 0.000670;
	array[44] = size_x + -0.002500;
	array[45] = size_y + 0.000670;
	array[46] = size_x + -0.000670;
	array[47] = size_y + 0.002500;
	array[48] = size_x + -0.000670;
	array[49] = size_y + 0.002500;
	array[50] = size_x + 0.000000;
	array[51] = size_y + 0.005000;
	array[52] = pos_x + 0.005000;
	array[53] = size_y + -0.000000;
	array[54] = pos_x + 0.002500;
	array[55] = size_y + 0.000670;
	array[56] = pos_x + 0.002500;
	array[57] = size_y + 0.000670;
	array[58] = pos_x + 0.000670;
	array[59] = size_y + 0.002500;
	array[60] = pos_x + 0.000670;
	array[61] = size_y + 0.002500;
	array[62] = pos_x + -0.000000;
	array[63] = size_y + 0.005000;
	sui_draw_gl(GL_LINES, array, 32, 2, red, green, blue);
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
float icon_color[3] = {0.7, 0.7, 0.7};
float text_color[3] = {0, 0, 0};

uint sui_draw_popup(BInputState *input, float pos_x, float pos_y, SUIPUElement *element, uint element_count, uint button)
{
	uint i, j, top = 0, bottom = 0;
	float x, y;
	if(input->mode == BAM_DRAW)
	{
	/*	for(i = 0; i < element_count; i++)
		{
			if(element[i].type == PU_T_TOP)
				top++;
			if(element[i].type == PU_T_BOTTOM)
				bottom++;
		}*/
		sw_draw_bakground_ring(pos_x, pos_y);
		for(i = 0; i < element_count; i++)
		{
			switch(element[i].type)
			{
				case PU_T_TOP :
					if(top == 0)
						sui_draw_popup_top_bottom(pos_x, pos_y + 0.15, -1, icon_color[0], icon_color[1], icon_color[2]);
					if(sui_box_click_test(pos_x - 0.125, pos_y + (float)(top) * 0.05 + 0.075, 0.25, 0.05))
						sui_draw_popup_square(pos_x - 0.125, pos_y + (float)(top) * 0.05 + 0.125, 0.25, -0.05, icon_color[0], icon_color[1], icon_color[2]);
					sui_draw_text(pos_x + sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, element[i].text) * -0.5, pos_y + (float)(top++) * 0.05 + 0.1 - SUI_T_SIZE, SUI_T_SIZE, SUI_T_SPACE, element[i].text, text_color[0], text_color[1], text_color[2]);
				break;
				case PU_T_BOTTOM :
					if(bottom == 0)
						sui_draw_popup_top_bottom(pos_x, pos_y - 0.15, 1, icon_color[0], icon_color[1], icon_color[2]);
					if(sui_box_click_test(pos_x - 0.125, pos_y + (float)(bottom) * -0.05 - 0.125, 0.25, 0.05))
						sui_draw_popup_square(pos_x - 0.125, pos_y + (float)(bottom) * -0.05 - 0.075, 0.25, -0.05, icon_color[0], icon_color[1], icon_color[2]);
					sui_draw_text(pos_x + sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, element[i].text) * -0.5, pos_y + (float)(bottom++) * -0.05 - 0.1 - SUI_T_SIZE, SUI_T_SIZE, SUI_T_SPACE, element[i].text, text_color[0], text_color[1], text_color[2]);
				break;
				case PU_T_ANGLE :
					for(j = 0; j < i; j++)
						if(element[i].type == PU_T_ANGLE && ((element[i].data.angle[0] - 1 < element[j].data.angle[0] && element[i].data.angle[0] + 1 > element[j].data.angle[0]) || (element[i].data.angle[0] - 1 < element[j].data.angle[1] && element[i].data.angle[0] + 1 > element[j].data.angle[1])))
							break;
					if(j == i)
					{
						x = sin(element[i].data.angle[0] * PI / 180);
						y = cos(element[i].data.angle[0] * PI / 180);
						sui_draw_popup_line(pos_x + x * 0.1, pos_y + y * 0.1, pos_x + x * 0.5, pos_y + y * 0.5, icon_color[0], icon_color[1], icon_color[2]);
					}
					for(j = 0; j < i; j++)
						if(element[i].type == PU_T_ANGLE && ((element[i].data.angle[1] - 1 < element[j].data.angle[0] && element[i].data.angle[1] + 1 > element[j].data.angle[0]) || (element[i].data.angle[1] - 1 < element[j].data.angle[1] && element[i].data.angle[1] + 1 > element[j].data.angle[1])))
							break;
					if(j == i)
					{
						x = sin(element[i].data.angle[1] * PI / 180);
						y = cos(element[i].data.angle[1] * PI / 180);
						sui_draw_popup_line(pos_x + x * 0.1, pos_y + y * 0.1, pos_x + x * 0.5, pos_y + y * 0.5, icon_color[0], icon_color[1], icon_color[2]);
					}

					if(element[i].data.angle[0] < 0)
					{
						sui_draw_text(pos_x + sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, element[i].text) * -0.5, pos_y + 0.45, SUI_T_SIZE, SUI_T_SPACE, element[i].text, text_color[0], text_color[1], text_color[2]);
					}else if(element[i].data.angle[0] < 180 && element[i].data.angle[1] > 180)
					{
						sui_draw_text(pos_x + sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, element[i].text) * -0.5, pos_y - 0.45, SUI_T_SIZE, SUI_T_SPACE, element[i].text, text_color[0], text_color[1], text_color[2]);
					}else if(element[i].data.angle[0] < 180)
					{
						glPushMatrix();
						glTranslatef(pos_x, pos_y, 0);
						glRotatef(90 - (element[i].data.angle[0] + element[i].data.angle[1]) * 0.5, 0, 0, 1);
						sui_draw_text(0.25, -SUI_T_SIZE, SUI_T_SIZE, SUI_T_SPACE, element[i].text, text_color[0], text_color[1], text_color[2]);
						glPopMatrix();
					}else if(element[i].data.angle[0] > 180)
					{
						glPushMatrix();
						glTranslatef(pos_x, pos_y, 0);
						glRotatef(270 - (element[i].data.angle[0] + element[i].data.angle[1]) * 0.5, 0, 0, 1);
						sui_draw_text(-0.25 - sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, element[i].text), -SUI_T_SIZE, SUI_T_SIZE, SUI_T_SPACE, element[i].text, text_color[0], text_color[1], text_color[2]);
						glPopMatrix();
					}
					break;
				case PU_T_SQUARE :
					if(element[i].data.square.draw_func == NULL)
						sui_draw_popup_square(pos_x + element[i].data.square.square[0], pos_y + element[i].data.square.square[1], element[i].data.square.square[2], element[i].data.square.square[3], icon_color[0], icon_color[1], icon_color[2]);
					else
						element[i].data.square.draw_func(pos_x + element[i].data.square.square[0], pos_y + element[i].data.square.square[1], element[i].data.square.square[2], element[i].data.square.square[3], icon_color[0], icon_color[1], icon_color[2]);		

					if(sui_box_click_test(pos_x + element[i].data.square.square[0], pos_y + element[i].data.square.square[1] + element[i].data.square.square[3], element[i].data.square.square[2], -element[i].data.square.square[3]))
						sui_draw_popup_square(pos_x + element[i].data.square.square[0] - 0.005, pos_y + element[i].data.square.square[1] + 0.005, element[i].data.square.square[2] + 0.01, element[i].data.square.square[3] - 0.01, icon_color[0], icon_color[1], icon_color[2]);

					sui_draw_text(pos_x + element[i].data.square.square[0] + element[i].data.square.square[2] * 0.5 - sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, element[i].text) * 0.5, -SUI_T_SIZE + pos_y + element[i].data.square.square[1] + element[i].data.square.square[3] * 0.5, SUI_T_SIZE, SUI_T_SPACE, element[i].text, text_color[0], text_color[1], text_color[2]);
					break;
				case PU_T_ICON :
					element[i].data.icon.draw_func(element[i].data.icon.pos[0], element[i].data.icon.pos[1], icon_color[0], icon_color[1], icon_color[2]);
				break;
			}
		}
	}else if(input->mouse_button[button] == FALSE && input->last_mouse_button[button] == TRUE)
	{
		float angle = 1000;
		if(0.01 < (pos_x - input->pointer_x) * (pos_x - input->pointer_x) + (pos_y - input->pointer_y) * (pos_y - input->pointer_y))
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
