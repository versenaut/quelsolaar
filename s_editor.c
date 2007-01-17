#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include "seduce.h"

typedef enum{
	SE_EM_SYMBOL,
	SE_EM_FONT
}SEEdtorMode;

#define SE_GRID_POINTS 50

float se_grid_compute(float pos)
{	
	return (float)((int)(pos * SE_GRID_POINTS + 0.5 + SE_GRID_POINTS * 10)) / (float)SE_GRID_POINTS - 10.0;
}

void se_draw(SDrawing *e, boolean points, boolean mid, float center_x, float center_y, float *color_drawing, float *color_interface)
{
	static float curve[128], circle[128] = {10000};
	SEditorLine *l;
	uint i;
	if(circle[0] > 9999)
	{
		for(i = 0; i < 32; i++)
		{
			sui_draw_set_vec2(circle, i * 2, sin(((double)i) * 2 * PI / 32) / SE_GRID_POINTS * 0.3, cos(((double)i) * 2 * PI / 32) / SE_GRID_POINTS * 0.3);
			sui_draw_set_vec2(circle, i * 2 + 1, sin(((double)i + 1) * 2 * PI / 32) / SE_GRID_POINTS * 0.3, cos(((double)i + 1) * 2 * PI / 32) / SE_GRID_POINTS * 0.3);
			sui_draw_set_vec2(curve, i * 2, sin(((double)i) * 0.5 * PI / 32), cos(((double)i) * 0.5 * PI / 32));
			sui_draw_set_vec2(curve, i * 2 + 1, sin(((double)i + 1) * 0.5 * PI / 32), cos(((double)i + 1) * 0.5 * PI / 32));
		}
	}
	for(i = 0; i < e->line_count; i++)
	{
		l = &e->lines[i];
		if(points)
		{
			glPushMatrix();
			glTranslatef(l->pos_one[0] - center_x, l->pos_one[1] - center_y, 0);
			sui_draw_gl(GL_LINES, circle, 64, 2, color_interface[0], color_interface[1], color_interface[2], 1.0);
			glPopMatrix();
			glPushMatrix();
			glTranslatef(l->pos_two[0] - center_x, l->pos_two[1] - center_y, 0);
			sui_draw_gl(GL_LINES, circle, 64, 2, color_interface[0], color_interface[1], color_interface[2], 1.0);
			glPopMatrix();
		}
		if(mid)
		{
			glPushMatrix();
			glTranslatef((l->pos_one[0] + l->pos_two[0]) * 0.5 - center_x, (l->pos_one[1] + l->pos_two[1]) * 0.5 - center_y, 0);
			sui_draw_gl(GL_LINES, circle, 64, 2, 1, 0.6, 0.2, 1.0f);
			glPopMatrix();
		}
		switch(l->type)
		{
			case SE_LT_LINE :
				sui_draw_2d_line_gl(l->pos_one[0] - center_x, l->pos_one[1] - center_y, l->pos_two[0] - center_x, l->pos_two[1] - center_y, color_drawing[0], color_drawing[1], color_drawing[2], 1);
			break;
			case SE_LT_CURVE : 
				glPushMatrix();
				glTranslatef(l->pos_one[0] - center_x, l->pos_one[1] - center_y, 0);
				glScalef(l->pos_two[0] - l->pos_one[0], l->pos_two[1] - l->pos_one[1], 0);
				sui_draw_gl(GL_LINES, curve, 64, 2, color_drawing[0], color_drawing[1], color_drawing[2], 1.0f);
				glPopMatrix();
			break;
			default:
			break;
		}
	}
}

void se_draw_grid(float pos_x, float pos_y, float size_x, float size_y, float *color)
{
	static float *points = NULL;
	static uint point_count;
	uint x, y, i, j;
	
	x = size_x * SE_GRID_POINTS;
	y = size_y * SE_GRID_POINTS + 1;
	if(x * y != point_count || points == NULL)
	{
		point_count = x * y;
		if(points != NULL)
			free(points);
		points = malloc((sizeof *points) * point_count * 2);
		for(i = 0; i < x; i++)
			for(j = 0; j < y; j++)
				sui_draw_set_vec2(points, i * y + j,  ((float)i + 0.5) / SE_GRID_POINTS, (float)j / SE_GRID_POINTS);
	}
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	sui_draw_gl(GL_POINTS, points, point_count, 2, color[0], color[1], color[2], 0.5f);
	glPopMatrix();
}
/*
void se_new_drawing(char *name)
{
	uint i;
	SEditor.drawing_count++;
	if(SEditor.drawing_count == SEditor.drawing_allocate)
	{
		SEditor.drawing_allocate += 256;
		SEditor.drawings = realloc(SEditor.drawings, (sizeof *SEditor.drawings) * SEditor.drawing_allocate);
	}
	SEditor.current_drawing = SEditor.drawing_count - 1;
	SEditor.drawings[SEditor.current_drawing].line_count = 0;
	for(i = 0; name[i] != 0; i++)
		SEditor.drawings[SEditor.current_drawing].name[i] = name[i];
	SEditor.drawings[SEditor.current_drawing].name[i] = 0;
	SEditor.drawings[SEditor.current_drawing].line_count = 0;
	SEditor.drawings[SEditor.current_drawing].line_allocate = 256;
	SEditor.drawings[SEditor.current_drawing].lines = malloc((sizeof *SEditor.drawings[SEditor.current_drawing].lines) * SEditor.drawings[SEditor.current_drawing].line_allocate);

}*/

#define SE_ARCH_SPLITS 16

uint se_compute_splits(SEditorLine *line)
{
	float f;
	f = 10 * sqrt((line->pos_one[0] - line->pos_two[0]) * (line->pos_one[0] - line->pos_two[0]) + (line->pos_one[0] - line->pos_two[0]) * (line->pos_one[0] - line->pos_two[0]));
	return 3;//(uint)f + 2;
}
/*
float *se_create_array(SDrawing *draw, uint *length)
{
	uint i, j, line_count, split;
	float *buf;
	line_count = 0;
	for(i = 0; i < draw->line_count; i++)
	{
		if(draw->lines[i].type == SE_LT_CURVE)
			line_count += se_compute_splits(&draw->lines[i]) - 1;
		line_count++;
	}
	buf = malloc((sizeof *buf) * line_count * 4);
	line_count = 0;
	for(i = 0; i < draw->line_count; i++)
	{
		switch(draw->lines[i].type)
		{
			case SE_LT_LINE :
				buf[line_count++] = draw->lines[i].pos_one[0];
				buf[line_count++] = draw->lines[i].pos_one[1];
				buf[line_count++] = draw->lines[i].pos_two[0];
				buf[line_count++] = draw->lines[i].pos_two[1];
			break;
			case SE_LT_CURVE : 
				split = se_compute_splits(&draw->lines[i]);
				for(j = 0; j < split; j++)
				{
					buf[line_count++] = draw->lines[i].pos_one[0] + sin(((double)j) * 0.5 * PI / split) * (draw->lines[i].pos_two[0] - draw->lines[i].pos_one[0]); 
					buf[line_count++] = draw->lines[i].pos_one[1] + cos(((double)j) * 0.5 * PI / split) * (draw->lines[i].pos_two[1] - draw->lines[i].pos_one[1]);
					buf[line_count++] = draw->lines[i].pos_one[0] + sin(((double)j + 1) * 0.5 * PI / split) * (draw->lines[i].pos_two[0] - draw->lines[i].pos_one[0]); 
					buf[line_count++] = draw->lines[i].pos_one[1] + cos(((double)j + 1) * 0.5 * PI / split) * (draw->lines[i].pos_two[1] - draw->lines[i].pos_one[1]);
				}
			break;
		}
	}
	*length = line_count;
	return buf;
}*/
/*

typedef enum{
	SE_FT_ICON,
	SE_FT_LINE,
	SE_FT_SQUADE,
}SEFuncType;

*/

#define SE_OUTPUT_SCALE 1
/*#define SE_OUTPUT_SCALE 0.25*/
/*
void se_convert_flat_arcs(void)
{
	uint i, j;
	for(i = 0; i < SEditor.drawing_count; i++)
		for(j = 0; j < SEditor.drawings[i].line_count; j++)
			if(SEditor.drawings[i].lines[j].type == SE_LT_CURVE)
				if((SEditor.drawings[i].lines[j].pos_one[0] + 0.001 > SEditor.drawings[i].lines[j].pos_two[0] && SEditor.drawings[i].lines[j].pos_one[0] - 0.001 < SEditor.drawings[i].lines[j].pos_two[0]) ||
					(SEditor.drawings[i].lines[j].pos_one[1] + 0.001 > SEditor.drawings[i].lines[j].pos_two[1] && SEditor.drawings[i].lines[j].pos_one[1] - 0.001 < SEditor.drawings[i].lines[j].pos_two[1]))
						SEditor.drawings[i].lines[j].type = SE_LT_LINE;
						
}*/

void se_save_drawing_asci(FILE *file, SDrawing *d, uint draw_count)
{
	uint i, j;
	for(i = 0; i < draw_count; i++)
	{
		fprintf(file, "%s\n", d[i].name);
		for(j = 0; j < d[i].line_count; j++)
			fprintf(file, "%u %f %f %f %f\n",
				d[i].lines[j].type,
				d[i].lines[j].pos_one[0],
				d[i].lines[j].pos_one[1],
				d[i].lines[j].pos_two[0],
				d[i].lines[j].pos_two[1]);
	}
}


SDrawing *se_load_drawing_asci(FILE *drawing, uint *count)
{
	char	line[512];
	SEditorLine l;
	uint	i;
	SDrawing *d;
	d = malloc((sizeof *d) * 32);
	*count = -1;
	for(i = 0; i < 512; i++)
		line[i] = 0;
	while((fgets(line, sizeof line, drawing)) != NULL)
	{
		if(sscanf(line, "%u %f %f %f %f", &l.type, &l.pos_one[0], &l.pos_one[1], &l.pos_two[0], &l.pos_two[1]) == 5 && *count != -1)
		{
			if(d[*count].line_count == d[*count].line_allocate)
			{
				d[*count].line_allocate *= 2;
				d[*count].lines = realloc(d[*count].lines, (sizeof *d[*count].lines) * d[*count].line_allocate);
			}
			d[*count].lines[d[*count].line_count++] = l;
		}else
		{
			(*count)++;
			for(i = 0; line[i] != 0 && i < 64; i++)
				d[*count].name[i] = line[i];
			d[*count].name[i] = 0;
			d[*count].line_allocate = 32;
			d[*count].line_count = 0;
			d[*count].lines = malloc((sizeof *d[*count].lines) * d[*count].line_allocate);
		}
	}
	return d;
}

void se_pack_binary_uint32(FILE *f, const uint32 data)
{
	fputc((data >> 24) & 0xFF, f);
	fputc((data >> 16) & 0xFF, f);
	fputc((data >> 8)  & 0xFF, f);
	fputc(data & 0xFF, f);
}

void se_pack_binary_real32(FILE *f, float data)
{
	union { uint32 integer; float real; } punt;
	punt.real = data;
	se_pack_binary_uint32(f, punt.integer);
}

void se_save_drawing_binary(FILE *file, SDrawing *d, uint draw_count)
{
	uint i, j;
	se_pack_binary_uint32(file, draw_count);
	for(i = 0; i < draw_count; i++)
	{
		for(j = 0; d[i].name[j] != 0; j++)
			fputc(d[i].name[j], file);
		fputc(0, file);
		se_pack_binary_uint32(file, d[i].line_count);
		for(j = 0; j < d[i].line_count; j++)
		{
			fputc(d[i].lines[j].type, file);
			se_pack_binary_real32(file, d[i].lines[j].pos_one[0]),
			se_pack_binary_real32(file, d[i].lines[j].pos_one[1]),
			se_pack_binary_real32(file, d[i].lines[j].pos_two[0]),
			se_pack_binary_real32(file, d[i].lines[j].pos_two[1]);
		}
	}
}

uint32 se_unpack_binary_uint32(FILE *f)
{
	uint32 data;
	data = ((uint32) fgetc(f)) << 24;
	data |= ((uint32) fgetc(f)) << 16;
	data |= ((uint32) fgetc(f)) << 8;
	data |= ((uint32) fgetc(f));
	return data;
}

float se_unpack_binary_real32(FILE *f)
{
	union { uint integer; float real; } data;
	data.integer = ((uint32) fgetc(f)) << 24;
	data.integer |= ((uint32) fgetc(f)) << 16;
	data.integer |= ((uint32) fgetc(f)) << 8;
	data.integer |= ((uint32) fgetc(f));
	return data.real;
}

SDrawing *se_load_drawing_binary(FILE *file, uint *draw_count)
{
	SDrawing *d;
	uint i, j;
	*draw_count = se_unpack_binary_uint32(file);
	d = malloc((sizeof *d) * *draw_count);
	for(i = 0; i < *draw_count; i++)
	{
		for(j = 0; (d[i].name[j] = fgetc(file)) != 0; j++);
		d[i].line_count = se_unpack_binary_uint32(file);
		d[i].line_allocate = d[i].line_count + 16;
		d[i].lines = malloc((sizeof *d[i].lines) * d[i].line_allocate);
		for(j = 0; j < d[i].line_count; j++)
		{
			d[i].lines[j].type = fgetc(file);
			d[i].lines[j].pos_one[0] = se_unpack_binary_real32(file);
			d[i].lines[j].pos_one[1] = se_unpack_binary_real32(file);
			d[i].lines[j].pos_two[0] = se_unpack_binary_real32(file);
			d[i].lines[j].pos_two[1] = se_unpack_binary_real32(file);
		}
	}
	return d;
}

void se_save_drawing_code(FILE *file, SDrawing *d, uint draw_count)
{
	uint i, j = 0;

	for(i = 0; i < draw_count; i++)
	{
		if(d[i].line_count != 0)
		{
			fprintf(file, "void	sui_draw_symb_%s(float pos_x, float pos_y, float red, float green, float blue)\n{\n", d[i].name);
			fprintf(file, "\tstatic float array[] = {%f, %f", d[i].lines[j].pos_one[0] * SE_OUTPUT_SCALE, d[i].lines[j].pos_one[1] * SE_OUTPUT_SCALE, d[i].lines[j].pos_two[0] * SE_OUTPUT_SCALE, d[i].lines[j].pos_two[1] * SE_OUTPUT_SCALE);
			for(j = 1; j < d[i].line_count; j++)
				fprintf(file, ", %f, %f", d[i].lines[j].pos_one[0] * SE_OUTPUT_SCALE, d[i].lines[j].pos_one[1] * SE_OUTPUT_SCALE, d[i].lines[j].pos_two[0] * SE_OUTPUT_SCALE, d[i].lines[j].pos_two[1] * SE_OUTPUT_SCALE);
			fprintf(file, "};\n");
			fprintf(file, "\tglPushMatrix();\n");
			fprintf(file, "\tglTranslatef(pos_x, pos_y, 0);\n");
			fprintf(file, "\tsui_draw_gl(GL_LINES, array, %u, 2, red, green, blue, 1.0f);\n", d[i].line_count * 2);
			fprintf(file, "\tglPopMatrix();\n");
			fprintf(file, "}\n\n");
		}

	}
	fclose(file);
}

void se_editor(BInputState *input, SDrawing *d, float pos_x, float pos_y, float size_x, float size_y, float *color_drawing, float *color_interface, SELineMode mode)
{
	float center_x, center_y, f;
	center_x = -(pos_x + size_x / 2);
	center_y = -(pos_y + size_y / 2); 
	if(input->mode == BAM_MAIN)
		return;
	if(input->mode == BAM_DRAW)
	{
		se_draw_grid(pos_x, pos_y, size_x, size_y, color_interface);
		se_draw(d, TRUE, TRUE, center_x, center_y, color_drawing, color_interface);
	}else
	{
		if(mode == SE_LT_LINE || mode == SE_LT_CURVE) /* DRAW */
		{
			if(input->click_pointer_x > pos_x && input->click_pointer_x < pos_x + size_x && input->click_pointer_y > pos_y && input->click_pointer_y < pos_y + size_y)
			{
				if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
				{
					d->lines[d->line_count].type = mode;
					d->lines[d->line_count].pos_one[0] = se_grid_compute(input->pointer_x + center_x);
					d->lines[d->line_count].pos_one[1] = se_grid_compute(input->pointer_y + center_y);
					d->lines[d->line_count].pos_two[0] = se_grid_compute(input->pointer_x + center_x);
					d->lines[d->line_count].pos_two[1] = se_grid_compute(input->pointer_y + center_y);
				/*	d->lines[d->line_count].pos_one[0] = input->pointer_x + center_x;
					d->lines[d->line_count].pos_one[1] = input->pointer_y + center_y;
					d->lines[d->line_count].pos_two[0] = input->pointer_x + center_x;
					d->lines[d->line_count].pos_two[1] = input->pointer_y + center_y;*/
					d->line_count++;
				}
				if(input->mouse_button[0] == TRUE)
				{
					d->lines[d->line_count - 1].pos_two[0] = se_grid_compute(input->pointer_x + center_x);
					d->lines[d->line_count - 1].pos_two[1] = se_grid_compute(input->pointer_y + center_y);
					f = se_grid_compute(input->pointer_x + center_x);
					if(f > size_x * 0.5)
						f = size_x * 0.5;
					else if(f < size_x * -0.5)
						f = size_x * -0.5;
					d->lines[d->line_count - 1].pos_two[0] = f;
					f = se_grid_compute(input->pointer_y + center_y);
					if(f > size_y * 0.5)
						f = size_y * 0.5;
					else if(f < size_y * -0.5)
						f = size_y * -0.5;
					d->lines[d->line_count - 1].pos_two[1] = f;
				}
				if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE)
				{		
					uint i;
					float x, y;
					x = d->lines[d->line_count - 1].pos_one[0] - d->lines[d->line_count - 1].pos_two[0];
					y = d->lines[d->line_count - 1].pos_one[1] - d->lines[d->line_count - 1].pos_two[1];
					if(x * x + y * y < 0.0001 || d->lines[d->line_count - 1].pos_one[0] < -0.85)
					{
						d->line_count--;
						for(i = 0; i < d->line_count; i++)
						{
							x = (d->lines[i].pos_one[0] + d->lines[i].pos_two[0]) / 2 - center_x - input->pointer_x;
							y = (d->lines[i].pos_one[1] + d->lines[i].pos_two[1]) / 2 - center_y - input->pointer_y;
							if(x * x + y * y < (0.5 / SE_GRID_POINTS) * (0.5 / SE_GRID_POINTS))
								d->lines[i] = d->lines[--d->line_count];
						}
					}
				}
			}
		}
	}
}
