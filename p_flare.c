#include <stdio.h>

#ifdef _WIN32
	#include <windows.h>
	#include <GL/gl.h>
#else
#if defined(__APPLE__) || defined(MACOSX)
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif
#endif

#include "enough.h"

extern uint p_th_get_hdri_token(boolean alpha);

uint flare_texture = 0;
uint flare_size = 128;

boolean p_display_flare = TRUE;

void p_render_set_flare(boolean draw)
{
	p_display_flare = draw;
}

boolean	p_render_get_flare(void)
{
	return p_display_flare;
}

void p_create_flare(void)
{
	int i, j, k, temp;
	float x, y, r, half, best;
	float *data;
	half = (float)flare_size * 0.5;

	data = malloc((sizeof *data) * flare_size * flare_size * 3);
	for(i = 0; i < flare_size; i++)
	{
		for(j = 0; j < flare_size; j++)
		{
			x = ((i - half) / half) * 8;
			y = ((j - half) / half) * 8;
			r = 1 / (1 + x * x + y * y) - 0.015; 
			data[(i * flare_size + j) * 3] = r * 2;
			data[(i * flare_size + j) * 3 + 1] = r * 2;
			data[(i * flare_size + j) * 3 + 2] = r * 2;
		}
	}
	glGenTextures(1, &flare_texture);
	glBindTexture(GL_TEXTURE_2D, flare_texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, p_th_get_hdri_token(FALSE), flare_size, flare_size, 0, GL_RGB, GL_FLOAT, data);
	free(data);
}

void p_draw_flare(double *pos, double *color)
{
	double tmp[3] = {1.0, 2.0, 1.0};

	float color_array[8] = {1, 1, 1, 0, 0, 0, 0, 1};
	float vertex_array[12] = {-1, -1, 0, 1, -1, 0, 1, 1, 0, -1, 1, 0};
	double matrix[16], scale;

	if(pos == NULL)
		pos = tmp;

	glGetDoublev(GL_MODELVIEW_MATRIX, matrix);
/*
	vertex_array[0] = matrix[8] + matrix[4];
	vertex_array[1] = matrix[9] + matrix[5];
	vertex_array[2] = matrix[10] + matrix[6];
	vertex_array[3] = matrix[8] - matrix[4];
	vertex_array[4] = matrix[9] - matrix[5];
	vertex_array[5] = matrix[10] - matrix[6];
	vertex_array[6] = vertex_array[0];
	vertex_array[7] = vertex_array[1];
	vertex_array[8] = vertex_array[2];
	vertex_array[9] = vertex_array[3];
	vertex_array[10] = vertex_array[4];
	vertex_array[11] = vertex_array[5];
*/
	vertex_array[0] = matrix[0];
	vertex_array[1] = matrix[4];
	vertex_array[2] = matrix[8];
	vertex_array[3] = matrix[1];
	vertex_array[4] = matrix[5];
	vertex_array[5] = matrix[9];
	vertex_array[6] = -vertex_array[0];
	vertex_array[7] = -vertex_array[1];
	vertex_array[8] = -vertex_array[2];
	vertex_array[9] = -vertex_array[3];
	vertex_array[10] = -vertex_array[4];
	vertex_array[11] = -vertex_array[5];

	scale = color[0] + color[1] + color[2];
	glPushMatrix();
	glTranslated(pos[0], pos[1], pos[2]);
	glScaled(scale / 100.0, scale / 100.0, scale / 100.0);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0 , color_array);
	glBindTexture(GL_TEXTURE_2D, flare_texture);
	glColor4f(color[0] / scale * 3, color[1] / scale * 3, color[2] / scale * 3, 1);
	glVertexPointer(3, GL_FLOAT , 0, vertex_array);
	glDrawArrays(GL_QUADS, 0, 4);
//	glColor4f(1, 1, 1, 0);
//	glScaled(0.5, 0.5, 0.5);
//	glDrawArrays(GL_QUADS, 0, 4);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glPopMatrix();
}

void p_draw_flares(void)
{
	if(p_display_flare)
	{
		ENode *node;
		double light[3], pos[3];
		for(node = e_ns_get_node_next(0, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
		{
			e_nso_get_light(node, light);
			if(light[0] + light[1] + light[2] > 0)
			{
				e_nso_get_pos_time(node, pos, 0, 0);
				p_draw_flare(pos, light);
			}
		}
	}
}

void p_draw_reflection_flare(double *view, double *pos, double *color)
{
	double tmp[3] = {1.0, 2.0, 1.0}, nomal[3] = {1.1, 1.1, 1.1};
	float color_array[8] = {1, 1, 1, 0, 0, 0, 0, 1};
	float vertex_array[12] = {-1, -1, 0, 1, -1, 0, 1, 1, 0, -1, 1, 0};
	double matrix[16], scale;

	if(pos == NULL)
		pos = tmp;

	glGetDoublev(GL_MODELVIEW_MATRIX, matrix);
	create_matrix_normalized(matrix, pos, view, nomal);
	vertex_array[0] = matrix[0];
	vertex_array[1] = matrix[1];
	vertex_array[2] = matrix[2];
	vertex_array[3] = matrix[4];
	vertex_array[4] = matrix[5];
	vertex_array[5] = matrix[6];
	vertex_array[6] = -vertex_array[0];
	vertex_array[7] = -vertex_array[1];
	vertex_array[8] = -vertex_array[2];
	vertex_array[9] = -vertex_array[3];
	vertex_array[10] = -vertex_array[4];
	vertex_array[11] = -vertex_array[5];

	scale = color[0] + color[1] + color[2];
	glPushMatrix();
	glTranslated(pos[0], pos[1], pos[2]);
	glScaled(scale * 0.3, scale * 0.3, scale * 0.3);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0 , color_array);
	glBindTexture(GL_TEXTURE_2D, flare_texture);
	glColor4f(color[0] / (scale * 3), color[1] / (scale * 3), color[2] / (scale * 3), 1);
	glVertexPointer(3, GL_FLOAT , 0, vertex_array);
	glDrawArrays(GL_QUADS, 0, 4);
	glColor4f(1, 1, 1, 0);
	glScaled(0.5, 0.5, 0.5);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//	glEnableClientState(GL_NORMAL_ARRAY);
	glPopMatrix();
}

void p_draw_reflection_flares(double *view)
{
	ENode *node;
	double light[3], pos[3];
	for(node = e_ns_get_node_next(0, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
	{
		e_nso_get_light(node, light);
		if(light[0] + light[1] + light[2] > 0)
		{
			e_nso_get_pos_time(node, pos, 0, 0);
			p_draw_reflection_flare(view, pos, light);
		}
	}
}
