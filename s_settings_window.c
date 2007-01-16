#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "seduce.h"


double sui_draw_slider(BInputState *input, double x_pos, double y_pos, double width, double value, float *start_color, float *end_color, float *color)
{
	float line[16], col[8 * 3];
	uint i;

	if(input->mode == BAM_DRAW)
	{
		for(i = 1; i < 7; i++)
		{
			col[i * 3 + 0] = color[0];
			col[i * 3 + 1] = color[1];
			col[i * 3 + 2] = color[2];
		}
		col[0] = start_color[0];
		col[1] = start_color[1];
		col[2] = start_color[2];
		col[21] = end_color[0];
		col[22] = end_color[1];
		col[23] = end_color[2];
		width -= 0.02;
		line[0] = x_pos;
		line[1] = y_pos;
		line[2] = x_pos - 0.01 + width * value;
		line[3] = y_pos;

		line[4] = x_pos - 0.01 + width * value;
		line[5] = y_pos + 0.01;
		line[6] = x_pos - 0.00 + width * value;
		line[7] = y_pos - 0.01;

		line[8] = x_pos + 0.01 + width * value;
		line[9] = y_pos + 0.01;
		line[10] = x_pos + 0.00 + width * value;
		line[11] = y_pos - 0.01;

		line[12] = x_pos + 0.01 + width * value;
		line[13] = y_pos;
		line[14] = x_pos + width + 0.02;
		line[15] = y_pos;

		sui_set_color_array_gl(col, 8, 3);
		sui_draw_gl(GL_LINES, line, 8, 2, 0, 0, 0, 1);
	}else
	{
		if(input->mouse_button[0] && input->click_pointer_x > x_pos && input->click_pointer_x < x_pos + width && y_pos - 0.025 < input->click_pointer_y && y_pos + 0.025 > input->click_pointer_y)
		{
			value = (input->pointer_x - x_pos) / width;
			if(value > 1)
				value = 1;
			if(value < 0)
				value = 0;
		}

	}
	return value;
}

float sui_setting_view_size(SUIViewElement *element)
{
	if(element->type == S_VET_COLOR)
		return 0.15;
	if(element->type == S_VET_CUSTOM)
		return element->param.custom.length + 0.05;
	return 0.05;
}

#define BACKGROUND_SHADOW_SIZE 1.6

void compute_background(float *buffer, float *shadow, float *shadow_color, float size, float top, float bottom, float left, float right)
{
	float a, b, a2 = 0, b2 = size, f;
	uint i, j = 0, k = 0;
	for(i = 0; i < 10; i++)
	{
		f = (float)(i + 1) * PI * 0.5 / 10.0;
		a =	sin(f) * size;
		b =	cos(f) * size;
		buffer[j++] = right + a2;
		buffer[j++] = top + b2;

		buffer[j++] = right + a;
		buffer[j++] = top + b;

		buffer[j++] = left - a;
		buffer[j++] = top + b;

		buffer[j++] = left - a2;
		buffer[j++] = top + b2;

		shadow[k++] = right + a2;
		shadow[k++] = top + b2;

		shadow[k++] = right + a;
		shadow[k++] = top + b;

		shadow[k++] = right + a * BACKGROUND_SHADOW_SIZE;
		shadow[k++] = top + b * BACKGROUND_SHADOW_SIZE;

		shadow[k++] = right + a2 * BACKGROUND_SHADOW_SIZE;
		shadow[k++] = top + b2 * BACKGROUND_SHADOW_SIZE;


		shadow[k++] = left + -a2;
		shadow[k++] = top + b2;

		shadow[k++] = left + -a;
		shadow[k++] = top + b;

		shadow[k++] = left + -a * BACKGROUND_SHADOW_SIZE;
		shadow[k++] = top + b * BACKGROUND_SHADOW_SIZE;

		shadow[k++] = left + -a2 * BACKGROUND_SHADOW_SIZE;
		shadow[k++] = top + b2 * BACKGROUND_SHADOW_SIZE;

		a2 = a;
		b2 = b;
	}
	buffer[j++] = left - size;
	buffer[j++] = top;

	buffer[j++] = right + size;
	buffer[j++] = top;

	buffer[j++] = right + size;
	buffer[j++] = bottom;

	buffer[j++] = left - size;
	buffer[j++] = bottom;

	shadow[k++] = right;
	shadow[k++] = top + size;

	shadow[k++] = left;
	shadow[k++] = top + size;

	shadow[k++] = left;
	shadow[k++] = top + size * BACKGROUND_SHADOW_SIZE;

	shadow[k++] = right;
	shadow[k++] = top + size * BACKGROUND_SHADOW_SIZE;


	shadow[k++] = right;
	shadow[k++] = bottom - size;

	shadow[k++] = left;
	shadow[k++] = bottom - size;

	shadow[k++] = left;
	shadow[k++] = bottom - size * BACKGROUND_SHADOW_SIZE;

	shadow[k++] = right;
	shadow[k++] = bottom - size * BACKGROUND_SHADOW_SIZE;


	shadow[k++] = right + size;
	shadow[k++] = bottom;

	shadow[k++] = right + size;
	shadow[k++] = top;

	shadow[k++] = right + size * BACKGROUND_SHADOW_SIZE;
	shadow[k++] = top;

	shadow[k++] = right + size * BACKGROUND_SHADOW_SIZE;
	shadow[k++] = bottom;

	shadow[k++] = left - size;
	shadow[k++] = bottom;

	shadow[k++] = left - size;
	shadow[k++] = top;

	shadow[k++] = left - size * BACKGROUND_SHADOW_SIZE;
	shadow[k++] = top;

	shadow[k++] = left - size * BACKGROUND_SHADOW_SIZE;
	shadow[k++] = bottom;

	for(; i < 20; i++)
	{
		f = (float)(i + 1) * PI * 0.5 / 10.0;
		a =	sin(f) * size;
		b =	cos(f) * size;
		buffer[j++] = right + a2;
		buffer[j++] = bottom + b2;

		buffer[j++] = right + a;
		buffer[j++] = bottom + b;

		buffer[j++] = left - a;
		buffer[j++] = bottom + b;

		buffer[j++] = left - a2;
		buffer[j++] = bottom + b2;


		shadow[k++] = right + a2;
		shadow[k++] = bottom + b2;

		shadow[k++] = right + a;
		shadow[k++] = bottom + b;

		shadow[k++] = right + a * BACKGROUND_SHADOW_SIZE;
		shadow[k++] = bottom + b * BACKGROUND_SHADOW_SIZE;

		shadow[k++] = right + a2 * BACKGROUND_SHADOW_SIZE;
		shadow[k++] = bottom + b2 * BACKGROUND_SHADOW_SIZE;

		shadow[k++] = left + -a2;
		shadow[k++] = bottom + b2;

		shadow[k++] = left + -a;
		shadow[k++] = bottom + b;

		shadow[k++] = left + -a * BACKGROUND_SHADOW_SIZE;
		shadow[k++] = bottom + b * BACKGROUND_SHADOW_SIZE;

		shadow[k++] = left + -a2 * BACKGROUND_SHADOW_SIZE;
		shadow[k++] = bottom + b2 * BACKGROUND_SHADOW_SIZE;

		a2 = a;
		b2 = b;
	}
	for(j = 0; j < k / 2; j++)
	{

		if((j / 2) % 2)
		{
			shadow_color[j * 4 + 0] = 0;
			shadow_color[j * 4 + 1] = 0;
			shadow_color[j * 4 + 2] = 0;
			shadow_color[j * 4 + 3] = 0;
		}
		else
		{
			shadow_color[j * 4 + 0] = 0;
			shadow_color[j * 4 + 1] = 0;
			shadow_color[j * 4 + 2] = 0;
			shadow_color[j * 4 + 3] = 0.1;
		}
	}
}

void sui_draw_window(double x_pos, double y_pos, double width, double height, float color, char *title)
{
	float back_color, f, draw[168], shadow[(40 * 4 + 16) * 2], shadow_color[(40 * 4 + 16) * 4];
	back_color = 1.0 - color;
	width *= 0.5;
	compute_background(draw, shadow, shadow_color, 0.03, y_pos, y_pos - height, x_pos - width, x_pos + width);
	sui_set_color_array_gl(shadow_color, 40 * 4 + 16, 4);
	sui_draw_gl(GL_QUADS, shadow, 40 * 4 + 16, 2, 0.2, 0.5, 0.7, 1);
	sui_draw_gl(GL_QUADS, draw, 84, 2, color, color, color, 0.5);
	f = -0.5 * sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE * 8, title);
	sui_draw_text(x_pos + f, y_pos, SUI_T_SIZE, SUI_T_SPACE * 8, title, back_color, back_color, back_color, 1.0);
	sui_draw_2d_line_gl(x_pos - f + 0.02, y_pos - 0.005, x_pos + f - 0.02, y_pos - 0.005, back_color, back_color, back_color, 1.0);
}

boolean sui_draw_setting_view(BInputState *input, double x_pos, double y_pos, double width, SUIViewElement *element, uint element_count, char *title, float back_color)
{
	float draw[168], shadow[(40 * 4 + 16) * 2], shadow_color[(40 * 4 + 16) * 4];
	float color, f, f2, y_pos_start = y_pos;
	uint i, j;
	width *= 0.5;
	color = 1.0 - back_color;
	f = 0;
	for(i = 0; i < element_count; i++)
		f += sui_setting_view_size(&element[i]);

	if(input->mode == BAM_DRAW)
	{
		compute_background(draw, shadow, shadow_color, 0.03, y_pos, y_pos - f, x_pos - width, x_pos + width);
		sui_set_blend_gl(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		sui_set_color_array_gl(shadow_color, 40 * 4 + 16, 4);
		sui_draw_gl(GL_QUADS, shadow, 40 * 4 + 16, 2, 0.2, 0.5, 0.7, 1);
		sui_set_blend_gl(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		sui_draw_gl(GL_QUADS, draw, 84, 2, back_color, back_color, back_color, 0.5);

		f = -0.5 * sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE * 8, title);
		sui_draw_text(x_pos + f, y_pos, SUI_T_SIZE, SUI_T_SPACE * 8, title, color, color, color, 1);
		sui_draw_2d_line_gl(x_pos - f + 0.02, y_pos - 0.005, x_pos + f - 0.02, y_pos - 0.005, color, color, color, 1);
	}

	y_pos -= 0.05;

	for(i = 0; i < element_count; i++)
	{
		if(input->mode == BAM_DRAW)
			sui_draw_text(x_pos - width, y_pos, SUI_T_SIZE, SUI_T_SPACE, element[i].text, color, color, color, 1);
		switch(element[i].type)
		{
			case S_VET_SPLIT :
				if(input->mode == BAM_DRAW)
					sui_draw_2d_line_gl(x_pos - width + 0.01 + sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, element[i].text), y_pos + 0.007, x_pos + width, y_pos + 0.007, color, color, color, 1);
			break;
			case S_VET_SPLIT_MULTI :
				{
					char nr[64];
					sprintf(nr, "<%u/%u>", element[i].param.split_multi.current + 1, element[i].param.split_multi.count);
					f = sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, element[i].text) + 0.03 - width;
					if(input->mode == BAM_DRAW)
					{
						sui_draw_2d_line_gl(x_pos + f - 0.01, y_pos + 0.007, x_pos + f - 0.02, y_pos + 0.007, color, color, color, 1);
						sui_draw_text(x_pos + f, y_pos, SUI_T_SIZE, SUI_T_SPACE, nr, color, color, color, 1);
						f += sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, nr) + 0.03;
					}else
					{
						f2 = sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, nr);
						if(input->mouse_button[0] && !input->last_mouse_button[0])
						{
							if(input->pointer_x > x_pos + f && input->pointer_x < x_pos + f + f2 * 0.5 &&
								y_pos - 0.025 < input->click_pointer_y && y_pos + 0.025 > input->click_pointer_y && element[i].param.split_multi.current != 0)
								element[i].param.split_multi.current--;
							if(input->pointer_x > x_pos + f + f2 * 0.5 && input->pointer_x < x_pos + f + f2 &&
								y_pos - 0.025 < input->click_pointer_y && y_pos + 0.025 > input->click_pointer_y && element[i].param.split_multi.current != element[i].param.split_multi.count - 1)
								element[i].param.split_multi.current++;
						}
						f += f2 + 0.03; 
					}

					if(input->mode == BAM_DRAW)
					{
						sui_draw_2d_line_gl(x_pos + f - 0.01, y_pos + 0.007, x_pos + f - 0.02, y_pos + 0.007, color, color, color, 1);
						sui_draw_text(x_pos + f, y_pos, SUI_T_SIZE, SUI_T_SPACE, "Add Del", color, color, color, 1);
						f += sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, "Add Del") + 0.01;
					}else
					{
						f2 = sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, "Add Del");
						if(input->mouse_button[0] && !input->last_mouse_button[0])
						{
							if(input->pointer_x > x_pos + f && input->pointer_x < x_pos + f + f2 * 0.5 &&
								y_pos - 0.025 < input->click_pointer_y && y_pos + 0.025 > input->click_pointer_y)
								element[i].param.split_multi.add = TRUE;
							if(input->pointer_x > x_pos + f + f2 * 0.5 && input->pointer_x < x_pos + f + f2 &&
								y_pos - 0.025 < input->click_pointer_y && y_pos + 0.025 > input->click_pointer_y)
								element[i].param.split_multi.del = TRUE;
						}
						f += f2 + 0.01; 
					}
					if(input->mode == BAM_DRAW)
						sui_draw_2d_line_gl(x_pos + f, y_pos + 0.007, x_pos + width, y_pos + 0.007, color, color, color, 1);
				}
			break;
			case S_VET_BOOLEAN :
				if(input->mode == BAM_DRAW)
				{
					if(element[i].param.checkbox)
					{
						sui_draw_2d_line_gl(x_pos + width - 0.02, y_pos + 0.01, x_pos + width, y_pos - 0.01, color, color, color, 1);
						sui_draw_2d_line_gl(x_pos + width - 0.02, y_pos - 0.01, x_pos + width, y_pos + 0.01, color, color, color, 1);
					}
					sui_draw_2d_line_gl(x_pos + width - 0.02, y_pos + 0.01, x_pos + width - 0.00, y_pos + 0.01, color, color, color, 1);
					sui_draw_2d_line_gl(x_pos + width - 0.02, y_pos - 0.01, x_pos + width - 0.00, y_pos - 0.01, color, color, color, 1);
					sui_draw_2d_line_gl(x_pos + width - 0.02, y_pos + 0.01, x_pos + width - 0.02, y_pos - 0.01, color, color, color, 1);
					sui_draw_2d_line_gl(x_pos + width - 0.00, y_pos + 0.01, x_pos + width - 0.00, y_pos - 0.01, color, color, color, 1);
				}else if(input->mouse_button[0] && !input->last_mouse_button[0])
				{
					if(input->pointer_x - x_pos < width && input->pointer_x - x_pos > -width)
						if(input->pointer_y - y_pos > -0.025 && input->pointer_y - y_pos < 0.025)
 							element[i].param.checkbox = !element[i].param.checkbox;
				}
			break;
			case S_VET_INTEGER :
				sui_type_number_uint(input, x_pos + width, y_pos, 1, 0.1, SUI_T_SIZE, &element[i].param.integer, &element[i].param.integer, color, color, color);
			break;
			case S_VET_REAL :
				sui_type_number_double(input, x_pos + width, y_pos, 1, 0.1, SUI_T_SIZE, &element[i].param.real, &element[i].param.real, color, color, color, 1.0);
			break;
			case S_VET_SLIDER :
				{				
					float col[3] = {color, color, color};
					element[i].param.slider = sui_draw_slider(input, x_pos + width * -0.5, y_pos, width * 1.5, element[i].param.slider, col, col, col);
				}
			break;
			case S_VET_TEXT :
			//	
				if(element[i].param.text.text[0] == 0)
				{
					sui_type_in(input, x_pos, y_pos, width, SUI_T_SIZE, element[i].param.text.text, element[i].param.text.length, NULL, NULL, color, color, color, 1.0);
				}else
				{
					f = sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, element[i].param.text.text);
					sui_type_in(input, x_pos + width - f, y_pos, f, SUI_T_SIZE, element[i].param.text.text, element[i].param.text.length, NULL, NULL, color, color, color, 1.0);
				}
			break;
			case S_VET_COLOR :
				{				
					float col[3] = {0, 0, 0}, start[3], end[3];
					col[0] = start[0] = end[0] = element[i].param.color[0];
					col[1] = start[1] = end[1] = element[i].param.color[1];
					col[2] = start[2] = end[2] = element[i].param.color[2];
					start[0] = 0;
					end[0] = 1;
					element[i].param.color[0] = sui_draw_slider(input, x_pos + width * -0.5, y_pos, width * 1.5, element[i].param.color[0], start, end, col);
					start[0] = element[i].param.color[0];
					end[0] = element[i].param.color[0];
					start[1] = 0;
					end[1] = 1;
					y_pos -= 0.05;
					element[i].param.color[1] = sui_draw_slider(input, x_pos + width * -0.5, y_pos, width * 1.5, element[i].param.color[1], start, end, col);
					start[1] = element[i].param.color[1];
					end[1] = element[i].param.color[1];
					start[2] = 0;
					end[2] = 1;
					y_pos -= 0.05;
					element[i].param.color[2] = sui_draw_slider(input, x_pos + width * -0.5, y_pos, width * 1.5, element[i].param.color[2], start, end, col);
					
				}
			break;
			case S_VET_SELECT :
				for(j = 0; element[i].param.select.text[j] != NULL; j++);
				f = width;
				while(j != 0)
				{
					f2 = f;
					f -= sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, element[i].param.select.text[j - 1]);
					if(input->mode == BAM_DRAW)
					{
						sui_draw_text(x_pos + f, y_pos, SUI_T_SIZE, SUI_T_SPACE, element[i].param.select.text[j - 1], color, color, color, 1);
						if(element[i].param.select.select == j - 1)
							sui_draw_2d_line_gl(x_pos + f - 0.005, y_pos - 0.02, x_pos + f2 + 0.005, y_pos - 0.02, color, color, color, 1);
					}else
					{
						if(input->mouse_button[0] && !input->last_mouse_button[0] && input->click_pointer_x > f - 0.02 && input->click_pointer_x < f2 + 0.02  && y_pos - 0.025 < input->click_pointer_y && y_pos + 0.025 > input->click_pointer_y)
							element[i].param.select.select = j - 1;
					}
					f -= 0.04;
					j--;
				}
			break;
			case S_VET_CUSTOM :
				if(element[i].param.custom.func != NULL)
					element[i].param.custom.func(input, element[i].param.custom.user, x_pos - width, y_pos, width * 2, element[i].param.custom.length);
				y_pos -= element[i].param.custom.length + 0.05;
			break;
		}
		y_pos -= 0.05;
	}
	if(input->mode != BAM_DRAW && input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE)
		if((input->click_pointer_x - 0.03 > x_pos + width ||
			input->click_pointer_x + 0.03 < x_pos - width ||
			input->click_pointer_y - 0.03 > y_pos_start ||
 			input->click_pointer_y - 0.03 < y_pos) && (
			input->pointer_x - 0.03 > x_pos + width ||
			input->pointer_x + 0.03 < x_pos - width ||
			input->pointer_y - 0.03 > y_pos_start ||
 			input->pointer_y - 0.03 < y_pos))
			return TRUE;
	return FALSE;
}

void draw_settings_test(BInputState *input)
{
	static SUIViewElement element[10];
	static boolean init = FALSE;
	static float time = 0;

	if(init == FALSE)
	{
		init = TRUE;
		element[0].type = S_VET_SPLIT;
		element[0].text = "split";

		element[1].type = S_VET_BOOLEAN;
		element[1].text = "boolean";
		element[1].param.checkbox = FALSE;

		element[2].type = S_VET_INTEGER;
		element[2].text = "integer";
		element[2].param.integer = 23;

		element[3].type = S_VET_REAL;
		element[3].text = "real";
		element[3].param.real = 23.3;

		element[4].type = S_VET_SLIDER;
		element[4].text = "slider";
		element[4].param.slider = 0.3;

		element[5].type = S_VET_TEXT;
		element[5].text = "text";
		element[5].param.text.text = "nisse";
		element[5].param.text.length = 6;

		element[6].type = S_VET_COLOR;
		element[6].text = "color";
		element[6].param.color[0] = 0.2;
		element[6].param.color[1] = 0.8;
		element[6].param.color[2] = 1;

		element[7].type = S_VET_SELECT;
		element[7].text = "select";
		element[7].param.select.text[0] = "oj";
		element[7].param.select.text[1] = "uff";
		element[7].param.select.text[2] = "paff";
		element[7].param.select.text[3] = "potjong";
		element[7].param.select.text[4] = NULL;
		element[7].param.select.select = 0;

		element[8].type = S_VET_BOOLEAN;
		element[8].text = "boolean";
		element[8].param.checkbox = TRUE;

		element[9].type = S_VET_SPLIT_MULTI;
		element[9].text = "split_multi";
		element[9].param.split_multi.current = 0;
		element[9].param.split_multi.count = 10;
		element[9].param.split_multi.add = FALSE;
		element[9].param.split_multi.del = FALSE;

/*		element[1].type = S_VET_SPLIT;
		element[1].text = "texttext";
		element[2].type = S_VET_SPLIT;
		element[2].text = "texttexttext";
		element[3].type = S_VET_SPLIT;
		element[3].text = "texttexttexttext";*/
	}

//	time += 0.1;

	sui_draw_setting_view(input, sin(time) * 0.1, cos(time) * 0.1, 0.5, element, 10, "TITLE", 1.0);

	if(element[9].param.split_multi.add == TRUE)
	{
		element[9].param.split_multi.count++;
		element[9].param.split_multi.add = FALSE;
	}
	if(element[9].param.split_multi.del == TRUE)
	{
		element[9].param.split_multi.count--;
		element[9].param.split_multi.del = FALSE;
	}
}
