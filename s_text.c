#include <stdlib.h>

#include "seduce.h"

void sui_draw_text(float pos_x, float pos_y, float size, float spacing, const char *text, float red, float green, float blue)
{
	uint i;
//	glEnable(GL_LINE_SMOOTH);
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	size = size / sui_get_letter_size(97);
	spacing = spacing * sui_get_letter_size(97);
	glScalef(size, size, 1);
	for(i = 0; text[i] != 0; i++)
	{
		sui_draw_letter(text[i], red, green, blue);
		glTranslatef(sui_get_letter_size(text[i]) + spacing, 0, 0);
	}
	glPopMatrix();
//	glDisable(GL_LINE_SMOOTH);
}

float sui_compute_text_length(float size, float spacing, const char *text)
{
	uint i;
	float sum;
	size = size / sui_get_letter_size(97);
	spacing = spacing * sui_get_letter_size(97) * size;
	sum = -spacing;
	for(i = 0; text[i] != 0; i++)
		sum += sui_get_letter_size(text[i]) * size + spacing;
	return sum;
}
/*
void sui_draw_code(float pos_x, float pos_y, float size, float spacing, char *text)
{

	float f;
	uint i;
	glEnable(GL_LINE_SMOOTH);
	glPushMatrix();
	glTranslatef(-0.4, 0.4, 0);
//	glScalef(0.1, 0.1, 0.1);
	for(i = 0; text[i] != 0; i++)
	{
		f = sui_get_letter_size(text[i]);
		if(f > 0.240001)
		{
			glPushMatrix();
			glScalef(0.24000 / f, 1, 1);
			sui_draw_letter(text[i], 0.5, 0.5, 0.5);
			glPopMatrix();
		}else if(f < 0.23999)
		{
			glPushMatrix();
			glTranslatef((0.240000 - f) * 0.5, 0, 0);
			sui_draw_letter(text[i], 0.5, 0.5, 0.5);
			glPopMatrix();
		}else
			sui_draw_letter(text[i], 0.5, 0.5, 0.5);
		glTranslatef(0.240000 + 0.05, 0, 0);
	}
	glPopMatrix();
	printf("size of avrage text = %f\n", sui_get_letter_size(97));
	glDisable(GL_LINE_SMOOTH);
}*/

boolean sw_text_button(BInputState *input, float pos_x, float pos_y, float center, float size, float spacing, const char *text, float red, float green, float blue)
{
	if(input->mode == BAM_DRAW)
	{
		if(center > 0.0001)
			center *= sui_compute_text_length(size, spacing, text);
		sui_draw_text(pos_x - center, pos_y, size, spacing, text, red, green, blue);
	}else if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
	{
		spacing	= sui_compute_text_length(size, spacing, text);
		if(sui_box_click_test(pos_x, pos_y - size, spacing, size * 3) && sui_box_down_click_test(pos_x, pos_y - size, spacing, size * 3))
			return TRUE;
	}
	return FALSE;
}

uint sui_type_in_cursor = 0;
char *sui_type_in_text = 0;
char *sui_type_in_copy = 0;
char *sui_return_text = 0;
void (* sui_type_in_done_func)(void *user, char *text); 

void sui_end_type_func(void *user, boolean cancle)
{
	if(cancle == FALSE)
		if(sui_type_in_done_func != NULL)
			sui_type_in_done_func(user, sui_type_in_copy);
	sui_return_text = sui_type_in_text;
	if(sui_type_in_done_func != NULL)
	{
		free(sui_type_in_copy);
		sui_type_in_copy = NULL;
		sui_type_in_done_func = NULL;
	}
	sui_type_in_text = NULL;
}


boolean sui_type_in(BInputState *input, float pos_x, float pos_y, float length, float size, char *text, uint buffer_size, void (*done_func)(void *user, char *text), void* user, float red, float green, float blue)
{
	int i;
	float pos;
	if(input->mode == BAM_DRAW)
	{
		char *t;
		if(sui_type_in_text == text && sui_type_in_done_func != NULL)
			t = sui_type_in_copy;
		else
			t = text;

		sui_draw_text(pos_x, pos_y, size, SUI_T_SPACE, t, red, green, blue);
		if(sui_type_in_text == text)
		{
			pos = pos_x;
			for(i = 0; i < sui_type_in_cursor && t[i] != 0; i++)
				pos += (sui_get_letter_size(t[i]) / sui_get_letter_size(97) + SUI_T_SPACE) * size;
			sui_draw_text(pos + SUI_T_SPACE * 0.5 * size, pos_y, size, SUI_T_SPACE, "|", red, green, blue);
		}
	}
	else
	{
		if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
		{
			if(sui_box_click_test(pos_x, pos_y - size, length, size * 3.0))
			{
				pos = pos_x;
				sui_type_in_text = text;
				sui_type_in_done_func = done_func;
				if(done_func != NULL)
				{
					sui_type_in_copy = malloc((sizeof *sui_type_in_copy) * buffer_size);
					for(i = 0; i < buffer_size; i++)
						sui_type_in_copy[i] = text[i];
					betray_start_type_in(sui_type_in_copy, buffer_size, sui_end_type_func, &sui_type_in_cursor, user);
				}else
				{
					betray_start_type_in(text, buffer_size, sui_end_type_func, &sui_type_in_cursor, user);
					sui_type_in_copy = text;
				}
				for(i = 0; text[i] != 0 && i < buffer_size && pos < input->pointer_x; i++);
					pos = sui_get_letter_size(text[i]) * size;
				sui_type_in_cursor = i;
			}
		}
		if(sui_return_text = text)
		{
			sui_return_text = NULL;
			return TRUE;
		}
	}
	return FALSE;
}



#define SUI_ILLEGAL_NUMBER         1.7976931348623158e+308 /* max value */

char *sui_type_in_number_text;
double sui_type_in_number_output = SUI_ILLEGAL_NUMBER;
void *sui_type_in_number_id;

void sui_end_type_number_func(void *user, boolean cancle)
{
	if(cancle != TRUE)
		sscanf(sui_type_in_number_text, "%lf", &sui_type_in_number_output);
	free(sui_type_in_number_text);
	sui_type_in_number_text = NULL;
}


boolean sui_type_number_double(BInputState *input, float pos_x, float pos_y, float length, float size, double *number, void *id, float red, float green, float blue)
{
	int i;
	float pos;
	if(input->mode == BAM_DRAW)
	{
		if(sui_type_in_number_text != NULL && sui_type_in_number_id == id)
		{
			sui_draw_text(pos_x, pos_y, size, SUI_T_SPACE, sui_type_in_number_text, red, green, blue);
			pos = pos_x;
			for(i = 0; i < sui_type_in_cursor && sui_type_in_number_text[i] != 0; i++)
				pos += (sui_get_letter_size(sui_type_in_number_text[i]) / sui_get_letter_size(97) + SUI_T_SPACE) * size;
			sui_draw_text(pos + SUI_T_SPACE * 0.5 * size, pos_y, size, SUI_T_SPACE, "|", red, green, blue);
		}else
		{
			char nr[64];
			sprintf(nr, "%.3f", *number);
			sui_draw_text(pos_x, pos_y, size, SUI_T_SPACE, nr, red, green, blue);
		}
	}
	else
	{
		if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
		{
			if(sui_box_click_test(pos_x, pos_y - size, length, size * 3.0))
			{
				pos = pos_x;
				sui_type_in_number_text = malloc((sizeof *sui_type_in_number_text) * 64);
				sprintf(sui_type_in_number_text, "%.3f", *number);
				betray_start_type_in(sui_type_in_number_text, 64, sui_end_type_number_func, &sui_type_in_cursor, NULL);
				sui_type_in_number_id = id;
				sui_type_in_number_output = SUI_ILLEGAL_NUMBER;
				for(i = 0; sui_type_in_number_text[i] != 0 && i < 64 && pos < input->pointer_x; i++);
					pos = sui_get_letter_size(sui_type_in_number_text[i]) * size;
				sui_type_in_cursor = i;
			}
		}
		if(sui_type_in_number_id == id && sui_type_in_number_output != SUI_ILLEGAL_NUMBER)
		{
			sui_type_in_number_id = NULL;
			*number = sui_type_in_number_output;
			return TRUE;
		}
	}
	return FALSE;
}

boolean sui_type_number_uint(BInputState *input, float pos_x, float pos_y, float length, float size, uint32 *number, void *id, float red, float green, float blue)
{
	int i;
	float pos;
	if(input->mode == BAM_DRAW)
	{
		if(sui_type_in_number_text != NULL && sui_type_in_number_id == id)
		{
			sui_draw_text(pos_x, pos_y, size, SUI_T_SPACE, sui_type_in_number_text, red, green, blue);
			pos = pos_x;
			for(i = 0; i < sui_type_in_cursor && sui_type_in_number_text[i] != 0; i++)
				pos += (sui_get_letter_size(sui_type_in_number_text[i]) / sui_get_letter_size(97) + SUI_T_SPACE) * size;
			sui_draw_text(pos + SUI_T_SPACE * 0.5 * size, pos_y, size, SUI_T_SPACE, "|", red, green, blue);
		}else
		{
			char nr[64];
			sprintf(nr, "%u", *number);
			sui_draw_text(pos_x, pos_y, size, SUI_T_SPACE, nr, red, green, blue);
		}
	}
	else
	{
		if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
		{
			if(sui_box_click_test(pos_x, pos_y - size, length, size * 3.0))
			{
				pos = pos_x;
				sui_type_in_number_text = malloc((sizeof *sui_type_in_number_text) * 64);
				sprintf(sui_type_in_number_text, "%u", *number);
				betray_start_type_in(sui_type_in_number_text, 64, sui_end_type_number_func, &sui_type_in_cursor, NULL);
				sui_type_in_number_id = id;
				sui_type_in_number_output = SUI_ILLEGAL_NUMBER;
				for(i = 0; sui_type_in_number_text[i] != 0 && i < 64 && pos < input->pointer_x; i++);
					pos = sui_get_letter_size(sui_type_in_number_text[i]) * size;
				sui_type_in_cursor = i;
			}
		}
		if(sui_type_in_number_id == id && sui_type_in_number_output != SUI_ILLEGAL_NUMBER)
		{
			sui_type_in_number_id = NULL;
			*number = (uint32)sui_type_in_number_output;
			return TRUE;
		}
	}
	return FALSE;
}

