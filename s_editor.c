#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include "seduce.h"

typedef enum{
	SE_EM_SYMBOL,
	SE_EM_FONT
}SEEdtorMode;

typedef enum{
	SE_LT_LINE,
	SE_LT_CURVE,
	SE_LT_DRAG
}SELineMode;

typedef enum{
	SE_FT_ICON,
	SE_FT_LINE,
	SE_FT_SQUARE,
}SEFuncType;


typedef struct{
	SELineMode	type;
	float		pos_one[2];
	float		pos_two[2];
}SEditorLine;

typedef struct{
	char		name[64];
	SEditorLine *lines;
	uint		line_allocate;
	uint		line_count;
}SDrawing;

struct{
	SDrawing	*drawings;
	uint		drawing_allocate;
	uint		drawing_count;
	uint		current_drawing;
}SEditor;

#define SE_GRID_POINTS 100

float se_grid_compute(float pos)
{	
	return (float)((int)((pos - 2.0) * SE_GRID_POINTS * 0.5 - 0.5)) / (SE_GRID_POINTS * 0.5) + 2;
}

extern void sui_draw_gl(uint draw_type, float *array, uint length, uint dimensions, float red, float green, float blue);

void sui_draw_symb_trach(float pos_x, float pos_y, float red, float green, float blue)
{
	static float array[] = {0.025000, 0.030000, 0.030000, 0.030000, 0.030000, 0.030000, 0.030000, 0.025000, 0.030000, 0.025000, 0.025000, 0.025000, 0.025000, 0.025000, 0.020000, -0.030000, 0.020000, -0.030000, -0.020000, -0.030000, -0.020000, -0.030000, -0.025000, 0.025000, -0.025000, 0.025000, -0.030000, 0.025000, -0.030000, 0.025000, -0.030000, 0.030000, -0.030000, 0.030000, -0.025000, 0.030000, -0.025000, 0.030000, -0.000000, 0.040000, -0.000000, 0.040000, 0.025000, 0.030000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	sui_draw_gl(GL_LINES, array, 22, 2, red, green, blue);
	glPopMatrix();
}

void sui_draw_symb_lines(float pos_x, float pos_y, float red, float green, float blue)
{
	static float array[] = {0.020000, 0.020000, 0.020000, -0.020000, 0.020000, -0.020000, -0.020000, -0.020000, 0.015000, 0.015000, -0.015000, -0.015000, -0.020000, 0.020000, 0.020000, 0.020000, -0.020000, 0.020000, -0.020000, -0.020000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	sui_draw_gl(GL_LINES, array, 10, 2, red, green, blue);
	glPopMatrix();
}


void sui_draw_symb_arcs(float pos_x, float pos_y, float red, float green, float blue)
{
	static float array[] = {0.020000, 0.020000, 0.020000, -0.020000, 0.020000, -0.020000, -0.020000, -0.020000, -0.020000, 0.020000, -0.020000, -0.020000, -0.020000, 0.020000, 0.020000, 0.020000, -0.015000, -0.015000, -0.012059, -0.014856, -0.012059, -0.014856, -0.009147, -0.014424, -0.009147, -0.014424, -0.006291, -0.013708, -0.006291, -0.013708, -0.003519, -0.012716, -0.003519, -0.012716, -0.000858, -0.011458, -0.000858, -0.011458, 0.001667, -0.009944, 0.001667, -0.009944, 0.004032, -0.008190, 0.004032, -0.008190, 0.006213, -0.006213, 0.006213, -0.006213, 0.008190, -0.004032, 0.008190, -0.004032, 0.009944, -0.001667, 0.009944, -0.001667, 0.011458, 0.000858, 0.011458, 0.000858, 0.012716, 0.003519, 0.012716, 0.003519, 0.013708, 0.006291, 0.013708, 0.006291, 0.014424, 0.009147, 0.014424, 0.009147, 0.014856, 0.012059, 0.014856, 0.012059, 0.015000, 0.015000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	sui_draw_gl(GL_LINES, array, 40, 2, red, green, blue);
	glPopMatrix();
}



void sui_draw_symb_next(float pos_x, float pos_y, float red, float green, float blue)
{
	static float array[] = {0.020000, 0.020000, 0.020000, -0.020000, 0.020000, -0.020000, -0.020000, -0.020000, -0.020000, 0.020000, -0.020000, -0.020000, -0.020000, 0.020000, 0.020000, 0.020000, 0.015000, 0.000000, -0.000000, -0.015000, -0.000000, -0.015000, -0.000000, -0.010000, -0.000000, -0.010000, -0.015000, -0.010000, -0.015000, -0.010000, -0.015000, 0.010000, -0.015000, 0.010000, -0.000000, 0.010000, -0.000000, 0.010000, -0.000000, 0.015000, -0.000000, 0.015000, 0.015000, 0.000000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	sui_draw_gl(GL_LINES, array, 22, 2, red, green, blue);
	glPopMatrix();
}

void sui_draw_symb_prev(float pos_x, float pos_y, float red, float green, float blue)
{
	static float array[] = {0.020000, 0.020000, 0.020000, -0.020000, 0.020000, -0.020000, -0.020000, -0.020000, -0.020000, 0.020000, -0.020000, -0.020000, -0.020000, 0.020000, 0.020000, 0.020000, -0.000000, 0.015000, -0.015000, 0.000000, -0.015000, 0.000000, -0.000000, -0.015000, -0.000000, -0.015000, -0.000000, -0.010000, -0.000000, -0.010000, 0.015000, -0.010000, 0.015000, -0.010000, 0.015000, 0.010000, 0.015000, 0.010000, -0.000000, 0.010000, -0.000000, 0.010000, -0.000000, 0.015000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	sui_draw_gl(GL_LINES, array, 22, 2, red, green, blue);
	glPopMatrix();
}

void sui_draw_symb_del(float pos_x, float pos_y, float red, float green, float blue)
{
	static float array[] = {0.020000, 0.020000, 0.020000, -0.020000, 0.020000, -0.020000, -0.020000, -0.020000, -0.020000, 0.020000, -0.020000, -0.020000, -0.020000, 0.020000, 0.020000, 0.020000, -0.010000, 0.015000, -0.015000, 0.010000, -0.015000, 0.010000, -0.005000, 0.000000, -0.005000, 0.000000, -0.015000, -0.010000, -0.015000, -0.010000, -0.010000, -0.015000, -0.010000, -0.015000, -0.000000, -0.005000, -0.000000, -0.005000, 0.010000, -0.015000, 0.010000, -0.015000, 0.015000, -0.010000, 0.015000, -0.010000, 0.005000, 0.000000, 0.005000, 0.000000, 0.015000, 0.010000, 0.015000, 0.010000, 0.010000, 0.015000, 0.010000, 0.015000, -0.000000, 0.005000, -0.000000, 0.005000, -0.010000, 0.015000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	sui_draw_gl(GL_LINES, array, 32, 2, red, green, blue);
	glPopMatrix();
}

void sui_draw_symb_font(float pos_x, float pos_y, float red, float green, float blue)
{
	static float array[] = {0.200000, 0.420000, 0.208901, 0.418997, 0.208901, 0.418997, 0.217355, 0.416039, 0.217355, 0.416039, 0.224940, 0.411273, 0.224940, 0.411273, 0.231273, 0.404940, 0.231273, 0.404940, 0.236039, 0.397355, 0.236039, 0.397355, 0.238997, 0.388901, 0.238997, 0.388901, 0.240000, 0.380000, 0.040000, -0.320000, 0.031099, -0.318997, 0.031099, -0.318997, 0.022645, -0.316039, 0.022645, -0.316039, 0.015060, -0.311273, 0.015060, -0.311273, 0.008727, -0.304940, 0.008727, -0.304940, 0.003961, -0.297355, 0.003961, -0.297355, 0.001003, -0.288901, 0.001003, -0.288901, -0.000000, -0.280000, 0.020000, 0.000000, 0.220000, 0.000000, 0.200000, -0.320000, 0.208901, -0.318997, 0.208901, -0.318997, 0.217355, -0.316039, 0.217355, -0.316039, 0.224940, -0.311273, 0.224940, -0.311273, 0.231273, -0.304940, 0.231273, -0.304940, 0.236039, -0.297355, 0.236039, -0.297355, 0.238997, -0.288901, 0.238997, -0.288901, 0.240000, -0.280000, 0.040000, 0.420000, 0.031099, 0.418997, 0.031099, 0.418997, 0.022645, 0.416039, 0.022645, 0.416039, 0.015060, 0.411273, 0.015060, 0.411273, 0.008727, 0.404940, 0.008727, 0.404940, 0.003961, 0.397355, 0.003961, 0.397355, 0.001003, 0.388901, 0.001003, 0.388901, -0.000000, 0.380000, 0.040000, -0.320000, 0.200000, -0.320000, 0.040000, 0.420000, 0.200000, 0.420000, 0.240000, 0.380000, 0.240000, -0.280000, -0.000000, -0.280000, -0.000000, 0.380000, 0.020000, 0.240000, 0.220000, 0.240000, 0.260000, 0.000000, 0.280000, 0.000000, -0.020000, 0.000000, -0.040000, 0.000000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	sui_draw_gl(GL_LINES, array, 72, 2, red, green, blue);
	glPopMatrix();
}








void	sui_draw_line_empty(float pos_x, float pos_y, float size_x, float size_y, float red, float green, float blue)
{
	float array[100];
	size_x += pos_x;
	size_y += pos_y;
	array[0] = pos_x + -0.000000;
	array[1] = pos_y + 0.000000;
	array[2] = size_x + 0.000000;
	array[3] = pos_y + 0.000000;
	array[4] = size_x + 0.040000;
	array[5] = pos_y + 0.000000;
	array[6] = size_x + 0.000000;
	array[7] = pos_y + 0.040000;
	array[8] = size_x + 0.040000;
	array[9] = pos_y + 0.000000;
	array[10] = size_x + 0.000000;
	array[11] = pos_y + -0.040000;
	array[12] = size_x + -0.040000;
	array[13] = pos_y + 0.040000;
	array[14] = pos_x + 0.040000;
	array[15] = pos_y + 0.040000;
	array[16] = pos_x + 0.040000;
	array[17] = pos_y + -0.040000;
	array[18] = size_x + -0.040000;
	array[19] = pos_y + -0.040000;
	array[20] = pos_x + -0.000000;
	array[21] = pos_y + 0.040000;
	array[22] = pos_x + -0.040000;
	array[23] = pos_y + 0.000000;
	array[24] = pos_x + -0.040000;
	array[25] = pos_y + 0.000000;
	array[26] = pos_x + -0.000000;
	array[27] = pos_y + -0.040000;
	array[28] = pos_x + 0.040000;
	array[29] = pos_y + 0.040000;
	array[30] = pos_x + 0.040000;
	array[31] = pos_y + 0.060000;
	array[32] = pos_x + 0.040000;
	array[33] = pos_y + 0.060000;
	array[34] = pos_x + -0.000000;
	array[35] = pos_y + 0.060000;
	array[36] = pos_x + -0.000000;
	array[37] = pos_y + 0.060000;
	array[38] = pos_x + -0.000000;
	array[39] = pos_y + 0.040000;
	array[40] = pos_x + -0.000000;
	array[41] = pos_y + -0.040000;
	array[42] = pos_x + -0.000000;
	array[43] = pos_y + -0.060000;
	array[44] = pos_x + -0.000000;
	array[45] = pos_y + -0.060000;
	array[46] = pos_x + 0.040000;
	array[47] = pos_y + -0.060000;
	array[48] = pos_x + 0.040000;
	array[49] = pos_y + -0.060000;
	array[50] = pos_x + 0.040000;
	array[51] = pos_y + -0.040000;
	array[52] = size_x + -0.040000;
	array[53] = pos_y + 0.040000;
	array[54] = size_x + -0.040000;
	array[55] = pos_y + 0.060000;
	array[56] = size_x + -0.040000;
	array[57] = pos_y + 0.060000;
	array[58] = size_x + 0.000000;
	array[59] = pos_y + 0.060000;
	array[60] = size_x + 0.000000;
	array[61] = pos_y + 0.060000;
	array[62] = size_x + 0.000000;
	array[63] = pos_y + 0.040000;
	array[64] = size_x + -0.040000;
	array[65] = pos_y + -0.040000;
	array[66] = size_x + -0.040000;
	array[67] = pos_y + -0.060000;
	array[68] = size_x + -0.040000;
	array[69] = pos_y + -0.060000;
	array[70] = size_x + 0.000000;
	array[71] = pos_y + -0.060000;
	array[72] = size_x + 0.000000;
	array[73] = pos_y + -0.060000;
	array[74] = size_x + 0.000000;
	array[75] = pos_y + -0.040000;
	array[76] = size_x + 0.000000;
	array[77] = pos_y + -0.080000;
	array[78] = size_x + 0.000000;
	array[79] = size_y + 0.020000;
	array[80] = size_x + 0.000000;
	array[81] = size_y + 0.020000;
	array[82] = size_x + -0.020000;
	array[83] = size_y + -0.000000;
	array[84] = size_x + -0.020000;
	array[85] = size_y + -0.000000;
	array[86] = pos_x + 0.020000;
	array[87] = size_y + -0.000000;
	array[88] = pos_x + 0.020000;
	array[89] = size_y + -0.000000;
	array[90] = pos_x + -0.000000;
	array[91] = size_y + 0.020000;
	array[92] = pos_x + -0.000000;
	array[93] = size_y + 0.020000;
	array[94] = pos_x + -0.000000;
	array[95] = pos_y + -0.080000;
	array[96] = size_x + -0.020000;
	array[97] = pos_y + 0.120000;
	array[98] = size_x + -0.020000;
	array[99] = pos_y + 0.120000;
	sui_draw_gl(GL_LINES, array, 50, 2, red, green, blue);
}

void se_draw(SDrawing *e, boolean points, boolean mid)
{
	static float curve[128], circle[128] = {10000};
	SEditorLine *l;
	uint i;
	if(circle[0] > 9999)
	{
		for(i = 0; i < 32; i++)
		{
			sui_draw_set_vec2(circle, i * 2, sin(((double)i) * 2 * PI / 32) / SE_GRID_POINTS * 0.8, cos(((double)i) * 2 * PI / 32) / SE_GRID_POINTS * 0.8);
			sui_draw_set_vec2(circle, i * 2 + 1, sin(((double)i + 1) * 2 * PI / 32) / SE_GRID_POINTS * 0.8, cos(((double)i + 1) * 2 * PI / 32) / SE_GRID_POINTS * 0.8);
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
			glTranslatef(l->pos_one[0], l->pos_one[1], 0);
			sui_draw_gl(GL_LINES, circle, 64, 2, 0.2, 0.8, 1);
			glPopMatrix();
			glPushMatrix();
			glTranslatef(l->pos_two[0], l->pos_two[1], 0);
			sui_draw_gl(GL_LINES, circle, 64, 2, 0.2, 0.8, 1);
			glPopMatrix();
		}
		if(mid)
		{
			glPushMatrix();
			glTranslatef((l->pos_one[0] + l->pos_two[0]) * 0.5, (l->pos_one[1] + l->pos_two[1]) * 0.5, 0);
			sui_draw_gl(GL_LINES, circle, 64, 2, 1, 0.6, 0.2);
			glPopMatrix();
		}
		switch(l->type)
		{
			case SE_LT_LINE :
				sui_draw_2d_line_gl(l->pos_one[0], l->pos_one[1], l->pos_two[0], l->pos_two[1], 1, 1, 1);
			break;
			case SE_LT_CURVE : 
				glPushMatrix();
				glTranslatef(l->pos_one[0], l->pos_one[1], 0);
				glScalef(l->pos_two[0] - l->pos_one[0], l->pos_two[1] - l->pos_one[1], 0);
				sui_draw_gl(GL_LINES, curve, 64, 2, 1, 1, 1);
				glPopMatrix();
			break;
		}
	}
}

void se_draw_grid(boolean grid, boolean line)
{
	static float *points = NULL, lines[12];
	if(points == NULL)
	{
		uint i, j;
		points = malloc((sizeof *points) * SE_GRID_POINTS * SE_GRID_POINTS * 2);
		for(i = 0; i < SE_GRID_POINTS; i++)
			for(j = 0; j < SE_GRID_POINTS; j++)
				sui_draw_set_vec2(points, i * SE_GRID_POINTS + j, (float)i / SE_GRID_POINTS * 2 - 1, (float)j / SE_GRID_POINTS * 2 - 1);
		sui_draw_set_vec2(lines, 0, 0.4, -1);
		sui_draw_set_vec2(lines, 1, 0.4, 1);
		sui_draw_set_vec2(lines, 2, -1, -0.4);
		sui_draw_set_vec2(lines, 3, 1, -0.4);

		sui_draw_set_vec2(lines, 4, -0.2, 0.2);
		sui_draw_set_vec2(lines, 5, -0.2, -0.6);
		sui_draw_set_vec2(lines, 6, 0.6, 0.2);
		sui_draw_set_vec2(lines, 7, 0.6, -0.6);
		sui_draw_set_vec2(lines, 8, -0.2, -0.6);
		sui_draw_set_vec2(lines, 9, 0.6, -0.6);
		sui_draw_set_vec2(lines, 10, -0.2, 0.2);
		sui_draw_set_vec2(lines, 11, 0.6, 0.2);
	}
	if(line)
		sui_draw_gl(GL_LINES, lines, 12, 2, 0.2, 0.6, 1);
	if(grid)
		sui_draw_gl(GL_POINTS, points, SE_GRID_POINTS * SE_GRID_POINTS, 2, 0.2, 0.6, 1);
}

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

}

#define SE_ARCH_SPLITS 16

uint se_compute_splits(SEditorLine *line)
{
	float f;
	f = 10 * sqrt((line->pos_one[0] - line->pos_two[0]) * (line->pos_one[0] - line->pos_two[0]) + (line->pos_one[0] - line->pos_two[0]) * (line->pos_one[0] - line->pos_two[0]));
	return 3;//(uint)f + 2;
}

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
}
/*

typedef enum{
	SE_FT_ICON,
	SE_FT_LINE,
	SE_FT_SQUADE,
}SEFuncType;

*/

#define SE_OUTPUT_SCALE 1
/*#define SE_OUTPUT_SCALE 0.25*/

void se_convert_flat_arcs(void)
{
	uint i, j;
	for(i = 0; i < SEditor.drawing_count; i++)
		for(j = 0; j < SEditor.drawings[i].line_count; j++)
			if(SEditor.drawings[i].lines[j].type == SE_LT_CURVE)
				if((SEditor.drawings[i].lines[j].pos_one[0] + 0.001 > SEditor.drawings[i].lines[j].pos_two[0] && SEditor.drawings[i].lines[j].pos_one[0] - 0.001 < SEditor.drawings[i].lines[j].pos_two[0]) ||
					(SEditor.drawings[i].lines[j].pos_one[1] + 0.001 > SEditor.drawings[i].lines[j].pos_two[1] && SEditor.drawings[i].lines[j].pos_one[1] - 0.001 < SEditor.drawings[i].lines[j].pos_two[1]))
						SEditor.drawings[i].lines[j].type = SE_LT_LINE;
}

void se_save(char *file_name)
{
	uint i, j;
	FILE *file;
	file = fopen(file_name, "w");
	for(i = 0; i < SEditor.drawing_count; i++)
	{
		fprintf(file, "%s\n", SEditor.drawings[i].name);
		for(j = 0; j < SEditor.drawings[i].line_count; j++)
		fprintf(file, "%u %f %f %f %f\n", SEditor.drawings[i].lines[j].type,
			SEditor.drawings[i].lines[j].pos_one[0],
			SEditor.drawings[i].lines[j].pos_one[1],
			SEditor.drawings[i].lines[j].pos_two[0],
			SEditor.drawings[i].lines[j].pos_two[1]);
	}
	fclose(file);
}

void se_save_symbols(char *file_name)
{
	uint i, j, k, line_count;
	float *buf;
	SEFuncType type;
	FILE *file;
	file = fopen(file_name, "w");
	for(i = 0; i < SEditor.drawing_count; i++)
	{
		buf = se_create_array(&SEditor.drawings[i], &line_count);
		type = SE_FT_ICON;
		for(j = 0; j < line_count && type == SE_FT_ICON; j += 2)
			if(buf[j] > 0.4)
				type = SE_FT_LINE;
		for(j = 1; j < line_count && type == SE_FT_LINE; j += 2)
			if(buf[j] < -0.4)
				type = SE_FT_SQUARE;

		switch(type)
		{
		case SE_FT_ICON :
			fprintf(file, "void	sui_draw_symb_%s(float pos_x, float pos_y, float red, float green, float blue)\n{\n", SEditor.drawings[i].name);
			fprintf(file, "\tstatic float array[] = {%f, %f", (buf[0] + 0.2) * SE_OUTPUT_SCALE, (buf[1] - 0.2) * SE_OUTPUT_SCALE);
			for(j = 1; j < line_count / 2; j++)
				fprintf(file, ", %f, %f", (buf[j * 2] + 0.2) * SE_OUTPUT_SCALE, (buf[j * 2 + 1] - 0.2) * SE_OUTPUT_SCALE);
			fprintf(file, "};\n");
			fprintf(file, "\tglPushMatrix();\n");
			fprintf(file, "\tglTranslatef(pos_x, pos_y, 0);\n");
			fprintf(file, "\tsui_draw_gl(GL_LINES, array, %u, 2, red, green, blue);\n", line_count / 2);
			fprintf(file, "\tglPopMatrix();\n");
			fprintf(file, "}\n\n");
			break;
		case SE_FT_LINE :
			fprintf(file, "void	sui_draw_line_%s(float pos_x, float pos_y, float dest_x, float dest_y, float red, float green, float blue)\n{\n", SEditor.drawings[i].name);
			fprintf(file, "\tfloat array[%u], vec_x, vec_y, r;\n", line_count);
			fprintf(file, "\tvec_x = pos_x - dest_x;\n");
			fprintf(file, "\tvec_y = dest_y - pos_y;\n");
			fprintf(file, "\tr = sqrt(vec_x * vec_x + vec_y * vec_y);\n");
			fprintf(file, "\tvec_x /= r;\n");
			fprintf(file, "\tvec_y /= r;\n");
			for(j = 0; j < line_count; j += 2)
			{
				if(buf[j] < 0.4)
				{
					fprintf(file, "\tarray[%u] = pos_x - %f * vec_x - %f * vec_y;\n", j, (buf[j] + 0.2) * SE_OUTPUT_SCALE, (buf[j + 1] - 0.2) * SE_OUTPUT_SCALE);
					fprintf(file, "\tarray[%u] = pos_y + %f * vec_y - %f * vec_x;\n", j + 1, (buf[j] + 0.2) * SE_OUTPUT_SCALE, (buf[j + 1] - 0.2) * SE_OUTPUT_SCALE);
				}else
				{
					fprintf(file, "\tarray[%u] = dest_x - %f * vec_x - %f * vec_y;\n", j, (buf[j] - 0.6) * SE_OUTPUT_SCALE, (buf[j + 1] - 0.2) * SE_OUTPUT_SCALE);
					fprintf(file, "\tarray[%u] = dest_y + %f * vec_y - %f * vec_x;\n", j + 1, (buf[j] - 0.6) * SE_OUTPUT_SCALE, (buf[j + 1] - 0.2) * SE_OUTPUT_SCALE);
				}
			}
			fprintf(file, "\tsui_draw_gl(GL_LINES, array, %u, 2, red, green, blue);\n", line_count / 2);
			fprintf(file, "}\n\n");
			break;
		case SE_FT_SQUARE :
			fprintf(file, "void	sui_draw_square_%s(float pos_x, float pos_y, float size_x, float size_y, float red, float green, float blue)\n{\n", SEditor.drawings[i].name);
			fprintf(file, "\tfloat array[%u];\n", line_count);
			fprintf(file, "\tsize_x += pos_x;\n");
			fprintf(file, "\tsize_y += pos_y;\n");
			for(j = 0; j < line_count; j++)
			{
				if(buf[j] < 0.4)
					fprintf(file, "\tarray[%u] = pos_x + %f;\n", j, (buf[j] + 0.2) * SE_OUTPUT_SCALE);
				else
					fprintf(file, "\tarray[%u] = size_x + %f;\n", j, (buf[j] - 0.6) * SE_OUTPUT_SCALE);
				if(buf[++j] > -0.4)
					fprintf(file, "\tarray[%u] = pos_y + %f;\n", j, (buf[j] - 0.2) * SE_OUTPUT_SCALE);
				else
					fprintf(file, "\tarray[%u] = size_y + %f;\n", j, (buf[j] + 0.6) * SE_OUTPUT_SCALE);

			}
			fprintf(file, "\tsui_draw_gl(GL_LINES, array, %u, 2, red, green, blue);\n", line_count / 2);
			fprintf(file, "}\n\n");
			break;
		}
	}
	fclose(file);
}
void se_save_font(char *file_name)
{
	uint i, j, k, line_count, size[256];
	float *buf, width[256];
	SEFuncType type;
	FILE *file;
	file = fopen(file_name, "w");
	fprintf(file, "#include \"seduce.h\"\n\n");
	fprintf(file, "void sui_draw_letter(uint8 letter, float red, float green, float blue)\n{\n");
	
	for(i = 0; i < 256; i++)
		size[i] = 0;

	for(i = 0; i < SEditor.drawing_count && i < 256; i++)
	{
		size[i] = 0;
		width[i] = 0;
		if(SEditor.drawings[i].line_count > 0)
		{
			buf = se_create_array(&SEditor.drawings[i], &line_count);


			fprintf(file, "\tstatic float font_%u[] = {(float)%f, (float)%f", i, buf[0] * SE_OUTPUT_SCALE, buf[1] * SE_OUTPUT_SCALE);
			for(j = 2; j < line_count; j += 2)
			{
				if(j % 16 == 0)
					fprintf(file, "\n\t\t\t");
				fprintf(file, ", (float)%f, (float)%f", buf[j] * SE_OUTPUT_SCALE, buf[j + 1] * SE_OUTPUT_SCALE);
				if(buf[j] > width[i])
					width[i] = buf[j];
			}
			size[i] = line_count / 2;
			fprintf(file, "};\n");
		}
		if(width[i] < 0.000001)
			width[i] = 0.1 * SE_OUTPUT_SCALE;
	}
	fprintf(file, "\tstatic uint font_size[] = {%u", size[0]);
	for(i = 1; i < 256; i++)
	{
		if(i % 16 == 0)
			fprintf(file, "\n\t\t\t");
		fprintf(file, ", %u", size[i]);
	}
	fprintf(file, "};\n");

	fprintf(file, "\tstatic float *font_array[256];\n");
	fprintf(file, "\tstatic boolean init = FALSE;\n");
	fprintf(file, "\tif(init == FALSE)\n\t{\n\t\tinit = TRUE;\n");
	for(i = 0; i < SEditor.drawing_count && i < 256; i++)
		if(SEditor.drawings[i].line_count > 0)
			fprintf(file, "\t\tfont_array[%u] = font_%u;\n", i, i);
	fprintf(file, "\t}\n\tif(font_size[letter] != 0)\n");
	fprintf(file, "\t\tsui_draw_gl(GL_LINES, font_array[letter], font_size[letter], 2, red, green, blue);\n}\n\n");
	fprintf(file, "float sui_get_letter_size(char letter)\n{\n");
		fprintf(file, "\tstatic float font_width[] = {%f", width[0]);
	for(i = 1; i < 256; i++)
	{
		if(i % 16 == 0)
			fprintf(file, "\n\t\t\t");
		fprintf(file, ", (float)%f", width[i]);
	}
	fprintf(file, "};\n");
	fprintf(file, "\treturn font_width[letter];\n}\n");
	fclose(file);
}



void se_load(char *file_name, SEEdtorMode ed_mode)
{
	char	line[512];
	uint	i, j;
	FILE	*drawing;
	if((drawing = fopen(file_name, "r")) != NULL)
	{
		for(i = 0; i < 512; i++)
			line[i] = 0;
		while((fgets(line, sizeof line, drawing)) != NULL)
		{
			if(sscanf(line, "%u %f %f %f %f", &SEditor.drawings[SEditor.current_drawing].lines[SEditor.drawings[SEditor.current_drawing].line_count].type,
				&SEditor.drawings[SEditor.current_drawing].lines[SEditor.drawings[SEditor.current_drawing].line_count].pos_one[0],
				&SEditor.drawings[SEditor.current_drawing].lines[SEditor.drawings[SEditor.current_drawing].line_count].pos_one[1],
				&SEditor.drawings[SEditor.current_drawing].lines[SEditor.drawings[SEditor.current_drawing].line_count].pos_two[0],
				&SEditor.drawings[SEditor.current_drawing].lines[SEditor.drawings[SEditor.current_drawing].line_count].pos_two[1]) == 5)
			{
				SEditor.drawings[SEditor.current_drawing].line_count++;
				if(SEditor.drawings[SEditor.current_drawing].line_count == SEditor.drawings[SEditor.current_drawing].line_allocate - 2)
				{

					SEditor.drawings[SEditor.current_drawing].line_allocate *= 2;
					SEditor.drawings[SEditor.current_drawing].lines = realloc(SEditor.drawings[SEditor.current_drawing].lines, (sizeof *SEditor.drawings[SEditor.current_drawing].lines) * SEditor.drawings[SEditor.current_drawing].line_allocate);
				}
			}else
			{
				for(i = 0; line[i] != 0; i++);
				line[i - 1] = 0;
				se_new_drawing(line);
/*				sscanf(line, "char %u", &SEditor.drawings[SEditor.current_drawing].character);*/
			}
		}
		fclose(drawing);
	}
	while(SEditor.drawing_count < 256)
	{
		char name[64];
		sprintf(name, "char_%u", SEditor.drawing_count);
		se_new_drawing(name);
	}
}

void se_draw_button(BInputState *input, SELineMode *mode, SEEdtorMode ed_mode)
{
	if(input->mode == BAM_DRAW)
	{
		if(*mode == SE_LT_LINE)
			sui_draw_symb_lines(-0.9, 0.2,  0.1, 0.4, 0.1);
		else
			sui_draw_symb_lines(-0.9, 0.2,  0.4, 0.1, 0.1);
	
		if(*mode == SE_LT_CURVE)
			sui_draw_symb_arcs(-0.9, 0.1, 0.1, 0.4, 0.1);
		else
			sui_draw_symb_arcs(-0.9, 0.1, 0.4, 0.1, 0.1);



		sui_draw_symb_next(-0.9, 0.0, 0.3, 0.3, 0.3);

		sui_draw_symb_prev(-0.9, -0.1, 0.3, 0.3, 0.3);
		sui_draw_symb_del(-0.9, -0.2, 0.3, 0.3, 0.3);
	}else if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
	{
/*		exit(0);*/
		if(sui_box_click_test(-0.95, 0.15, 0.1, 0.1))
			*mode = SE_LT_LINE;
		if(sui_box_click_test(-0.95, 0.05, 0.1, 0.1))
			*mode = SE_LT_CURVE;

		if(sui_box_click_test(-0.95, -0.05, 0.1, 0.1))
		{
			if(SEditor.current_drawing == SEditor.drawing_count - 1)
			{
				if(SEditor.drawings[SEditor.drawing_count - 1].line_count == 0 || ed_mode == SE_EM_FONT)					
					SEditor.current_drawing = 0;
				else
					se_new_drawing("empty");
			}else
			{
				SEditor.current_drawing++;
			}
		}
		if(sui_box_click_test(-0.95, -0.15, 0.1, 0.1))
		{
			if(SEditor.current_drawing == 0)
			{
				if(SEditor.drawings[SEditor.drawing_count - 1].line_count == 0 || ed_mode == SE_EM_FONT)					
					SEditor.current_drawing = SEditor.drawing_count - 1;
				else
					se_new_drawing("empty");
			}else
			{
				SEditor.current_drawing--;
			}
		}
/*		if(sui_box_click_test(-0.95, -0.05, 0.1, 0.1))
		if(sui_box_click_test(-0.95, -0.15, 0.1, 0.1))
		if(sui_box_click_test(-0.95, -0.25, 0.1, 0.1))
*/	}
}



void se_editor(BInputState *input, SEEdtorMode ed_mode)
{
	static uint mode = SE_LT_LINE;
	static boolean init = FALSE, active = FALSE;
	SDrawing *d;
	char *text[] = {"LINE", "ARC"};

	if(init == FALSE)
	{
		init = TRUE;
		SEditor.drawings = malloc((sizeof *SEditor.drawings) * 256);
		SEditor.drawing_allocate = 256;
		SEditor.drawing_count = 0;
		SEditor.current_drawing = 0;
		if(ed_mode == SE_EM_SYMBOL)
		{
			se_load("symbol.lin", SE_EM_SYMBOL);
			if(SEditor.drawing_count == 0)
				se_new_drawing("empty");
		}else
			se_load("font.lin", SE_EM_FONT);
	}
	d = &SEditor.drawings[SEditor.current_drawing];

	if(input->mode == BAM_EVENT && input->mouse_button[1] == TRUE && input->last_mouse_button[1] == FALSE)
	{
		se_convert_flat_arcs();
		if(ed_mode == SE_EM_SYMBOL)
		{
			se_save_symbols("s_symbol.c");
			se_save("symbol.lin");
		}
		else
		{
			se_save_font("s_line_font.c");
			se_save("font.lin");
		}
	}
	if(input->mode == BAM_MAIN)
		return;
	if(input->mode == BAM_DRAW)
	{
		glPushMatrix();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glClearColor(0.1, 0.1, 0.1, 0);
		glTranslatef(0, 0, -1);	
/*		glTranslatef(4, 4, 0);*/
		se_draw_grid(TRUE, ed_mode == SE_EM_SYMBOL);
/*		se_draw_grid(TRUE, TRUE);
		glTranslatef(-4, -4, 0);
*/		if(ed_mode == SE_EM_FONT)
		{
			sui_draw_symb_font(0, 0, 0.4, 0.4, 0.4);
			sui_draw_text(-0.9, -0.5, SUI_T_SIZE, SUI_T_SPACE, d->name, 1, 1, 1);
		}
		se_draw(d, TRUE, TRUE);
		if(SE_OUTPUT_SCALE < 0.8)
		{
			glPushMatrix();
			glTranslatef(0.8, 0.4, 0);
			glScalef(SE_OUTPUT_SCALE, SE_OUTPUT_SCALE, SE_OUTPUT_SCALE);
			se_draw_grid(FALSE, ed_mode == SE_EM_SYMBOL);
			se_draw(d, FALSE, FALSE);
			glPopMatrix();
		}
	}else
	{
		if(mode == SE_LT_LINE || mode == SE_LT_CURVE) /* DRAW */
		{
			if(input->click_pointer_x > -0.8)
			{
				if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
				{
					d->lines[d->line_count].type = mode;
					d->lines[d->line_count].pos_one[0] = se_grid_compute(input->pointer_x);
					d->lines[d->line_count].pos_one[1] = se_grid_compute(input->pointer_y);
					d->lines[d->line_count].pos_two[0] = se_grid_compute(input->pointer_x);
					d->lines[d->line_count].pos_two[1] = se_grid_compute(input->pointer_y);
					d->line_count++;
				}
				if(input->mouse_button[0] == TRUE)
				{
					d->lines[d->line_count - 1].pos_two[0] = se_grid_compute(input->pointer_x);
					d->lines[d->line_count - 1].pos_two[1] = se_grid_compute(input->pointer_y);
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
							x = (d->lines[i].pos_one[0] + d->lines[i].pos_two[0]) / 2 - input->pointer_x;
							y = (d->lines[i].pos_one[1] + d->lines[i].pos_two[1]) / 2 - input->pointer_y;
							if(x * x + y * y < (0.5 / SE_GRID_POINTS) * (0.5 / SE_GRID_POINTS))
								d->lines[i] = d->lines[--d->line_count];
						}
					}
				}
			}
		}
	}
	se_draw_button(input, &mode, ed_mode);
	if(input->mode == BAM_DRAW)
		glPopMatrix();
}


void *sui_symbol_editor_func(BInputState *input, void *user_pointer)
{
	se_editor(input, SE_EM_SYMBOL);
	return NULL;
}

void *sui_font_editor_func(BInputState *input, void *user_pointer)
{
	se_editor(input, SE_EM_FONT);
	return NULL;
}
