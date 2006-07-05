 
#include <stdlib.h>

#define WIDGET_DEFENITION
#include "seduce.h"
#include <math.h>

#define CIRCLE_SEGMENTS 32
#define IRIS_SEGMENTS 64
#define BACK_SEGMENTS 16

#define BUTTON_SEGMENTS 32

#define L_COL 0.2
#define S_COL 0.0

#define IN_SIZE 0.015
#define OUT_SIZE 0.023

static struct{
	float	*square;
	float	*square_color;
	float	*window_color;
	float	*transparency_ring;
	float	*transparency_ring_color;
	float	*button;
	float	*button_color;
	float	*point_color;
	float	*line;
	float	*line_color;
}SBackgroundData;

static void get_randomized_color(float *color, uint32 index)
{
	uint32 temp;
	temp = index * 4 + 0;
	temp = (temp<<13) ^ temp;
	color[0] = (((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) * 0.5;
	color[2] = color[0];
	color[1] = color[0] * color[0];
	color[0] = color[0] * color[0] * color[0];
	temp = index * 4 + 5;
	temp = (temp<<13) ^ temp;
	color[0] += (((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) * 0.1;
	temp = index * 4 + 6;
	temp = (temp<<13) ^ temp;
	color[1] += (((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) * 0.1;
	temp = index * 4 + 7;
	temp = (temp<<13) ^ temp;
	color[2] += (((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) * 0.1;
	color[0] *= 0.6;
	color[1] *= 0.6;
	color[2] *= 0.6;
}

void sui_init_background(void)
{
	uint i;
	float color_a[3], a, b;
	SBackgroundData.button = malloc((sizeof *SBackgroundData.button) * BUTTON_SEGMENTS * 9 * 3);
	SBackgroundData.button_color = malloc((sizeof *SBackgroundData.button_color) * BUTTON_SEGMENTS * 9 * 4);
	SBackgroundData.point_color = malloc((sizeof *SBackgroundData.point_color) * BUTTON_SEGMENTS * 9 * 4);
	for(i = 0; i < BUTTON_SEGMENTS; i++)
	{
		sui_draw_set_vec3(SBackgroundData.button, i * 9 + 0, -OUT_SIZE * sin(PI * (float)i / BUTTON_SEGMENTS), OUT_SIZE * cos(PI * (float)i / BUTTON_SEGMENTS), 0);
		sui_draw_set_vec3(SBackgroundData.button, i * 9 + 1, -OUT_SIZE * sin(PI * (float)(i + 1) / BUTTON_SEGMENTS), OUT_SIZE * cos(PI * (float)(i + 1) / BUTTON_SEGMENTS), 0);
		sui_draw_set_vec3(SBackgroundData.button, i * 9 + 2, -IN_SIZE * sin(PI * (float)i / BUTTON_SEGMENTS), IN_SIZE * cos(PI * (float)i / BUTTON_SEGMENTS), 0);
		sui_draw_set_vec3(SBackgroundData.button, i * 9 + 3, -OUT_SIZE * sin(PI * (float)(i + 1) / BUTTON_SEGMENTS), OUT_SIZE * cos(PI * (float)(i + 1) / BUTTON_SEGMENTS), 0);
		sui_draw_set_vec3(SBackgroundData.button, i * 9 + 4, -IN_SIZE * sin(PI * (float)i / BUTTON_SEGMENTS), IN_SIZE * cos(PI * (float)i / BUTTON_SEGMENTS), 0);
		sui_draw_set_vec3(SBackgroundData.button, i * 9 + 5, -IN_SIZE * sin(PI * (float)(i + 1) / BUTTON_SEGMENTS), IN_SIZE * cos(PI * (float)(i + 1) / BUTTON_SEGMENTS), 0);
		sui_draw_set_vec3(SBackgroundData.button, i * 9 + 6, -IN_SIZE * sin(PI * (float)(i + 1) / BUTTON_SEGMENTS), IN_SIZE * cos(PI * (float)(i + 1) / BUTTON_SEGMENTS), 0);
		sui_draw_set_vec3(SBackgroundData.button, i * 9 + 7, -IN_SIZE * sin(PI * (float)i / BUTTON_SEGMENTS), IN_SIZE * cos(PI * (float)i / BUTTON_SEGMENTS), 0);
		sui_draw_set_vec3(SBackgroundData.button, i * 9 + 8, 0, 0, 0);
		sui_draw_set_vec4(SBackgroundData.button_color, i * 9 + 0, S_COL, S_COL, S_COL, 1);
		sui_draw_set_vec4(SBackgroundData.button_color, i * 9 + 1, S_COL, S_COL, S_COL, 1);
		sui_draw_set_vec4(SBackgroundData.button_color, i * 9 + 2, L_COL, L_COL, L_COL, 1);
		sui_draw_set_vec4(SBackgroundData.button_color, i * 9 + 3, S_COL, S_COL, S_COL, 1);
		sui_draw_set_vec4(SBackgroundData.button_color, i * 9 + 4, L_COL, L_COL, L_COL, 1);
		sui_draw_set_vec4(SBackgroundData.button_color, i * 9 + 5, L_COL, L_COL, L_COL, 1);
		sui_draw_set_vec4(SBackgroundData.button_color, i * 9 + 6, L_COL, L_COL, L_COL, 1);
		sui_draw_set_vec4(SBackgroundData.button_color, i * 9 + 7, L_COL, L_COL, L_COL, 1);
		sui_draw_set_vec4(SBackgroundData.button_color, i * 9 + 8, L_COL, L_COL, L_COL, 1);

		sui_draw_set_vec4(SBackgroundData.point_color, i * 9 + 0, L_COL, L_COL, L_COL, 1);
		sui_draw_set_vec4(SBackgroundData.point_color, i * 9 + 1, L_COL, L_COL, L_COL, 1);
		sui_draw_set_vec4(SBackgroundData.point_color, i * 9 + 2, S_COL, S_COL, S_COL, 1);
		sui_draw_set_vec4(SBackgroundData.point_color, i * 9 + 3, L_COL, L_COL, L_COL, 1);
		sui_draw_set_vec4(SBackgroundData.point_color, i * 9 + 4, S_COL, S_COL, S_COL, 1);
		sui_draw_set_vec4(SBackgroundData.point_color, i * 9 + 5, S_COL, S_COL, S_COL, 1);
		sui_draw_set_vec4(SBackgroundData.point_color, i * 9 + 6, 1, 1, 1, 1);
		sui_draw_set_vec4(SBackgroundData.point_color, i * 9 + 7, 1, 1, 1, 1);
		sui_draw_set_vec4(SBackgroundData.point_color, i * 9 + 8, 1, 1, 1, 1);
	}
	SBackgroundData.line = malloc((sizeof *SBackgroundData.line) * 12 * 2);
	SBackgroundData.line_color = malloc((sizeof *SBackgroundData.line_color) * 12 * 4);
	sui_draw_set_vec2(SBackgroundData.line, 0, 0, OUT_SIZE);
	sui_draw_set_vec2(SBackgroundData.line, 1, 1, OUT_SIZE);
	sui_draw_set_vec2(SBackgroundData.line, 2, 1, IN_SIZE);
	sui_draw_set_vec2(SBackgroundData.line, 3, 0, IN_SIZE);
	sui_draw_set_vec2(SBackgroundData.line, 4, 0, IN_SIZE);
	sui_draw_set_vec2(SBackgroundData.line, 5, 1, IN_SIZE);
	sui_draw_set_vec2(SBackgroundData.line, 6, 1, -IN_SIZE);
	sui_draw_set_vec2(SBackgroundData.line, 7, 0, -IN_SIZE);
	sui_draw_set_vec2(SBackgroundData.line, 8, 0, -IN_SIZE);
	sui_draw_set_vec2(SBackgroundData.line, 9, 1, -IN_SIZE);
	sui_draw_set_vec2(SBackgroundData.line, 10, 1, -OUT_SIZE);
	sui_draw_set_vec2(SBackgroundData.line, 11, 0, -OUT_SIZE);
	sui_draw_set_vec4(SBackgroundData.line_color, 0, S_COL, S_COL, S_COL, 1);
	sui_draw_set_vec4(SBackgroundData.line_color, 1, S_COL, S_COL, S_COL, 1);
	sui_draw_set_vec4(SBackgroundData.line_color, 2, L_COL, L_COL, L_COL, 1);
	sui_draw_set_vec4(SBackgroundData.line_color, 3, L_COL, L_COL, L_COL, 1);
	sui_draw_set_vec4(SBackgroundData.line_color, 4, L_COL, L_COL, L_COL, 1);
	sui_draw_set_vec4(SBackgroundData.line_color, 5, L_COL, L_COL, L_COL, 1);
	sui_draw_set_vec4(SBackgroundData.line_color, 6, L_COL, L_COL, L_COL, 1);
	sui_draw_set_vec4(SBackgroundData.line_color, 7, L_COL, L_COL, L_COL, 1);
	sui_draw_set_vec4(SBackgroundData.line_color, 8, L_COL, L_COL, L_COL, 1);
	sui_draw_set_vec4(SBackgroundData.line_color, 9, L_COL, L_COL, L_COL, 1);
	sui_draw_set_vec4(SBackgroundData.line_color, 10, S_COL, S_COL, S_COL, 1);
	sui_draw_set_vec4(SBackgroundData.line_color, 11, S_COL, S_COL, S_COL, 1);


	SBackgroundData.square = malloc((sizeof *SBackgroundData.square) * BACK_SEGMENTS * 9 * 3);
	SBackgroundData.square_color = malloc((sizeof *SBackgroundData.square_color) * BACK_SEGMENTS * 9 * 4);
	for(i = 0; i < BACK_SEGMENTS; i++)
	{
		a = (float)i / BACK_SEGMENTS * 2.0 - 1;
		b = (float)(i + 1) / BACK_SEGMENTS * 2.0 - 1;
		sui_draw_set_vec2(SBackgroundData.square, i * 6 + 0, a, 2.0 / 2.34);
		sui_draw_set_vec2(SBackgroundData.square, i * 6 + 1, b, 2.0 / 2.34);
		sui_draw_set_vec2(SBackgroundData.square, i * 6 + 2, 0, 0);
		sui_draw_set_vec2(SBackgroundData.square, i * 6 + 3, a, -2.0 / 2.34);
		sui_draw_set_vec2(SBackgroundData.square, i * 6 + 4, b, -2.0 / 2.34);
		sui_draw_set_vec2(SBackgroundData.square, i * 6 + 5, 0, 0);
		a = 1.0 - sqrt(a * a + (2.0 / 2.34) * (2.0 / 2.34)) / 3;
		b = 1.0 - sqrt(b * b + (2.0 / 2.34) * (2.0 / 2.34)) / 3;
		sui_draw_set_vec4(SBackgroundData.square_color, i * 6 + 0, a, a, a, 1);
		sui_draw_set_vec4(SBackgroundData.square_color, i * 6 + 1, b, b, b, 1);
		sui_draw_set_vec4(SBackgroundData.square_color, i * 6 + 2, 1, 1, 1, 1);
		sui_draw_set_vec4(SBackgroundData.square_color, i * 6 + 3, a, a, a, 1);
		sui_draw_set_vec4(SBackgroundData.square_color, i * 6 + 4, b, b, b, 1);
		sui_draw_set_vec4(SBackgroundData.square_color, i * 6 + 5, 1, 1, 1, 1);
	}
	for(; i < (BACK_SEGMENTS * 3) / 2; i++)
	{
		a = ((float)(i - BACK_SEGMENTS) / (BACK_SEGMENTS * 0.25)- 1) * 2.0 / 2.34;
		b = ((float)(i - BACK_SEGMENTS + 1) / (BACK_SEGMENTS * 0.25) - 1) * 2.0 / 2.34;
		sui_draw_set_vec2(SBackgroundData.square, i * 6 + 0, 1, a);
		sui_draw_set_vec2(SBackgroundData.square, i * 6 + 1, 1, b);
		sui_draw_set_vec2(SBackgroundData.square, i * 6 + 2, 0, 0);
		sui_draw_set_vec2(SBackgroundData.square, i * 6 + 3, -1, a);
		sui_draw_set_vec2(SBackgroundData.square, i * 6 + 4, -1, b);
		sui_draw_set_vec2(SBackgroundData.square, i * 6 + 5, 0, 0);
		a = 1.0 - sqrt(a * a + 1) / 3;
		b = 1.0 - sqrt(b * b + 1) / 3;
		sui_draw_set_vec4(SBackgroundData.square_color, i * 6 + 0, 1, 1, 1, a);
		sui_draw_set_vec4(SBackgroundData.square_color, i * 6 + 1, 1, 1, 1, b);
		sui_draw_set_vec4(SBackgroundData.square_color, i * 6 + 2, 1, 1, 1, 1);
		sui_draw_set_vec4(SBackgroundData.square_color, i * 6 + 3, 1, 1, 1, a);
		sui_draw_set_vec4(SBackgroundData.square_color, i * 6 + 4, 1, 1, 1, b);
		sui_draw_set_vec4(SBackgroundData.square_color, i * 6 + 5, 1, 1, 1, 1);
	}
	SBackgroundData.transparency_ring = malloc((sizeof *SBackgroundData.transparency_ring) * IRIS_SEGMENTS * 4 * 3);
	SBackgroundData.transparency_ring_color = malloc((sizeof *SBackgroundData.transparency_ring_color) * IRIS_SEGMENTS * 4 * 4);
	get_randomized_color(color_a, IRIS_SEGMENTS - 1);
	for(i = 0; i < IRIS_SEGMENTS; i++)
	{
		sui_draw_set_vec3(SBackgroundData.transparency_ring, i * 4 + 0, 0.4 * sin(2.0 * PI * (float)i / IRIS_SEGMENTS), 0.4 * cos(2.0 * PI * (float)i / IRIS_SEGMENTS), 0);
		sui_draw_set_vec4(SBackgroundData.transparency_ring_color, i * 4 + 0, 1, 1, 1, 0);
		sui_draw_set_vec3(SBackgroundData.transparency_ring, i * 4 + 1, 0.07 * sin(2.0 * PI * (float)i / IRIS_SEGMENTS), 0.07 * cos(2.0 * PI * (float)i / IRIS_SEGMENTS), 0);
		sui_draw_set_vec4(SBackgroundData.transparency_ring_color, i * 4 + 1, 1, 1, 1, 0.6);
		sui_draw_set_vec3(SBackgroundData.transparency_ring, i * 4 + 2, 0.07 * sin(2.0 * PI * (float)(i + 1) / IRIS_SEGMENTS), 0.07 * cos(2.0 * PI * (float)(i + 1) / IRIS_SEGMENTS), 0);
		sui_draw_set_vec4(SBackgroundData.transparency_ring_color, i * 4 + 2, 1, 1, 1, 0.6);
		sui_draw_set_vec3(SBackgroundData.transparency_ring, i * 4 + 3, 0.4 * sin(2.0 * PI * (float)(i + 1) / IRIS_SEGMENTS), 0.4 * cos(2.0 * PI * (float)(i + 1) / IRIS_SEGMENTS), 0);
		sui_draw_set_vec4(SBackgroundData.transparency_ring_color, i * 4 + 3, 1, 1, 1, 0);
	}
	SBackgroundData.window_color = malloc((sizeof *SBackgroundData.window_color) * 20 * 4);
	sui_draw_set_vec4(SBackgroundData.window_color, 0, 1, 1, 1, 0.7);
	sui_draw_set_vec4(SBackgroundData.window_color, 1, 1, 1, 1, 0.7);
	sui_draw_set_vec4(SBackgroundData.window_color, 2, 1, 1, 1, 0.7);
	sui_draw_set_vec4(SBackgroundData.window_color, 3, 1, 1, 1, 0.7);
	sui_draw_set_vec4(SBackgroundData.window_color, 4, 0, 0, 0, 0.1);
	sui_draw_set_vec4(SBackgroundData.window_color, 5, 0, 0, 0, 0);
	sui_draw_set_vec4(SBackgroundData.window_color, 6, 0, 0, 0, 0);
	sui_draw_set_vec4(SBackgroundData.window_color, 7, 0, 0, 0, 0.1);
	sui_draw_set_vec4(SBackgroundData.window_color, 8, 0, 0, 0, 0.1);
	sui_draw_set_vec4(SBackgroundData.window_color, 9, 0, 0, 0, 0);
	sui_draw_set_vec4(SBackgroundData.window_color, 10, 0, 0, 0, 0);
	sui_draw_set_vec4(SBackgroundData.window_color, 11, 0, 0, 0, 0.1);
	sui_draw_set_vec4(SBackgroundData.window_color, 12, 0, 0, 0, 0.1);
	sui_draw_set_vec4(SBackgroundData.window_color, 13, 0, 0, 0, 0);
	sui_draw_set_vec4(SBackgroundData.window_color, 14, 0, 0, 0, 0);
	sui_draw_set_vec4(SBackgroundData.window_color, 15, 0, 0, 0, 0.1);
	sui_draw_set_vec4(SBackgroundData.window_color, 16, 0, 0, 0, 0.1);
	sui_draw_set_vec4(SBackgroundData.window_color, 17, 0, 0, 0, 0);
	sui_draw_set_vec4(SBackgroundData.window_color, 18, 0, 0, 0, 0);
	sui_draw_set_vec4(SBackgroundData.window_color, 19, 0, 0, 0, 0.1);
}

#define SUI_SHADOW 0.22

void sui_create_shadow_edge(float size, uint count, float *shadow, float *color, float *square)
{
	uint i;
	float pos_a[2], pos_b[2], temp[2], r;
	pos_a[1] = -(square[(count - 1) * 2] - square[0]);
	pos_a[0] = (square[count * 2 - 1] - square[1]);
	r  = sqrt(pos_a[0] * pos_a[0] + pos_a[1] * pos_a[1]);
	pos_a[1] /= r;
	pos_a[0] /= r;
	for(i = 0; i < count; i++)
	{
		pos_b[1] = -(square[i * 2] - square[((i + 1) % count) * 2]);
		pos_b[0] = (square[i * 2 + 1] - square[((i + 1) % count) * 2 + 1]);
		r = sqrt(pos_b[0] * pos_b[0] + pos_b[1] * pos_b[1]);
		pos_b[1] /= r;
		pos_b[0] /= r;
		r = (pos_a[0] + pos_b[0]) * pos_b[0] + (pos_a[1] + pos_b[1]) * pos_b[1];
		temp[0] = size * (pos_a[0] + pos_b[0]) / r;
		temp[1] = size * (pos_a[1] + pos_b[1]) / r;
		sui_draw_set_vec2(shadow, i * 4 + 2, square[i * 2], square[i * 2 + 1]);
		sui_draw_set_vec4(color, i * 4 + 2, 0, 0, 0, SUI_SHADOW);
		sui_draw_set_vec2(shadow, i * 4 + 3, square[i * 2] + temp[0], square[i * 2 + 1] + temp[1]);
		sui_draw_set_vec4(color, i * 4 + 3, 0.3, 0.3, 0.3, 0);
		sui_draw_set_vec2(shadow, ((i + 1) % count) * 4 + 0, square[i * 2] + temp[0], square[i * 2 + 1] + temp[1]);
		sui_draw_set_vec4(color, ((i + 1) % count) * 4 + 0, 0.3, 0.3, 0.3, 0);
		sui_draw_set_vec2(shadow, ((i + 1) % count) * 4 + 1, square[i * 2], square[i * 2 + 1]);
		sui_draw_set_vec4(color, ((i + 1) % count) * 4 + 1, 0, 0, 0, SUI_SHADOW);
		pos_a[0] = pos_b[0];
		pos_a[1] = pos_b[1];
	}
}

void sw_draw_background_line(float length);

void sw_draw_background_square(void)
{
	glPushMatrix();
	glScaled(1, 1.0 / 2.34, 1);
	sui_set_color_array_gl(SBackgroundData.square_color, BACK_SEGMENTS * 6, 4);
	sui_draw_gl(GL_TRIANGLES, SBackgroundData.square, BACK_SEGMENTS * 6, 3, 1, 1, 1, 1);
	glPopMatrix();
}

void sui_draw_background_ring(float pos_x, float pos_y, float color)
{
	static float last_color = 1;

	if(last_color != color)
	{
		uint i;
		for(i = 0; i < IRIS_SEGMENTS; i++)
		{
			sui_draw_set_vec4(SBackgroundData.transparency_ring_color, i * 4 + 0, color, color, color, 0);
			sui_draw_set_vec4(SBackgroundData.transparency_ring_color, i * 4 + 1, color, color, color, 0.6);
			sui_draw_set_vec4(SBackgroundData.transparency_ring_color, i * 4 + 2, color, color, color, 0.6);
			sui_draw_set_vec4(SBackgroundData.transparency_ring_color, i * 4 + 3, color, color, color, 0);
		}
		last_color = color;
	}
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	sui_set_blend_gl(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	sui_set_color_array_gl(SBackgroundData.transparency_ring_color, IRIS_SEGMENTS * 4, 4);
	sui_draw_gl(GL_QUADS, SBackgroundData.transparency_ring, IRIS_SEGMENTS * 4, 3, 1, 1, 1, 1);
	glPopMatrix();
}

void sw_draw_background_point(float *pos)
{
	glPushMatrix();
	glTranslatef(pos[0], pos[1], pos[2]);
//	glScaled(0.8, 0.8, 1);
	sui_set_color_array_gl(SBackgroundData.button_color, BUTTON_SEGMENTS * 9, 4);
	sui_draw_gl(GL_TRIANGLES, SBackgroundData.button, BUTTON_SEGMENTS * 9, 3, 1, 1, 1, 1);
	glScaled(-1, 1, 1);
	sui_set_color_array_gl(SBackgroundData.button_color, BUTTON_SEGMENTS * 9, 4);
	sui_draw_gl(GL_TRIANGLES, SBackgroundData.button, BUTTON_SEGMENTS * 9, 3, 1, 1, 1, 1);
	glPopMatrix();

}

void sw_draw_background_dot(float *pos)
{
	glPushMatrix();
	glTranslatef(pos[0], pos[1], pos[2]);
	glScaled(0.6, 0.6, 1);
	sui_set_color_array_gl(SBackgroundData.point_color, BUTTON_SEGMENTS * 9, 4);
	sui_draw_gl(GL_TRIANGLES, SBackgroundData.button, BUTTON_SEGMENTS * 9, 3, 1, 1, 1, 1);
	glScaled(-1, 1, 1);
	sui_set_color_array_gl(SBackgroundData.point_color, BUTTON_SEGMENTS * 9, 4);
	sui_draw_gl(GL_TRIANGLES, SBackgroundData.button, BUTTON_SEGMENTS * 9, 3, 1, 1, 1, 1);
	glPopMatrix();

}

void sw_draw_background_line(float length)
{
	glPushMatrix();
	sui_set_color_array_gl(SBackgroundData.button_color, BUTTON_SEGMENTS * 9, 4);
	sui_draw_gl(GL_TRIANGLES, SBackgroundData.button, BUTTON_SEGMENTS * 9, 3, 1, 1, 1, 1);
	glTranslatef(length, 0, 0);
	glScalef(-1, 1, 1);
	sui_set_color_array_gl(SBackgroundData.button_color, BUTTON_SEGMENTS * 9, 4);
	sui_draw_gl(GL_TRIANGLES, SBackgroundData.button, BUTTON_SEGMENTS * 9, 3, 1, 1, 1, 1);
	glScalef(length, 1, 1);
	sui_set_color_array_gl(SBackgroundData.line_color, 12, 4);
	sui_draw_gl(GL_QUADS, SBackgroundData.line, 12, 2, 1, 1, 1, 1);
	glPopMatrix();
}

void sw_draw_window(float x_pos, float y_pos, float x_size, float y_size, float shadow_size)
{
	float vertex[40];
	sui_draw_set_vec2(vertex, 0, x_pos + x_size, y_pos + y_size);
	sui_draw_set_vec2(vertex, 1, x_pos + x_size, y_pos);
	sui_draw_set_vec2(vertex, 2, x_pos, y_pos);
	sui_draw_set_vec2(vertex, 3, x_pos, y_pos + y_size);
	sui_draw_set_vec2(vertex, 4, x_pos + x_size, y_pos + y_size);
	sui_draw_set_vec2(vertex, 5, x_pos + x_size + shadow_size, y_pos + y_size + shadow_size);
	sui_draw_set_vec2(vertex, 6, x_pos - shadow_size, y_pos + y_size + shadow_size);
	sui_draw_set_vec2(vertex, 7, x_pos, y_pos + y_size);
	sui_draw_set_vec2(vertex, 8, x_pos + x_size, y_pos);
	sui_draw_set_vec2(vertex, 9, x_pos + x_size + shadow_size, y_pos - shadow_size);
	sui_draw_set_vec2(vertex, 10, x_pos - shadow_size, y_pos - shadow_size);
	sui_draw_set_vec2(vertex, 11, x_pos, y_pos);
	sui_draw_set_vec2(vertex, 12, x_pos + x_size, y_pos + y_size);
	sui_draw_set_vec2(vertex, 13, x_pos + x_size + shadow_size, y_pos + y_size + shadow_size);
	sui_draw_set_vec2(vertex, 14, x_pos + x_size + shadow_size, y_pos - shadow_size);
	sui_draw_set_vec2(vertex, 15, x_pos + x_size, y_pos);
	sui_draw_set_vec2(vertex, 16, x_pos, y_pos + y_size);
	sui_draw_set_vec2(vertex, 17, x_pos - shadow_size, y_pos + y_size + shadow_size);
	sui_draw_set_vec2(vertex, 18, x_pos - shadow_size, y_pos - shadow_size);
	sui_draw_set_vec2(vertex, 19, x_pos, y_pos);
	sui_set_blend_gl(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	sui_set_color_array_gl(SBackgroundData.window_color, 20, 4);
	sui_draw_gl(GL_QUADS, vertex, 20, 2, 1, 0.1, 1, 1);
}


void sw_draw_window2(float x_pos, float y_pos, float x_size, float y_size, float shadow_size)
{
	float vertex[40];
	sui_draw_set_vec2(vertex, 0, x_pos + x_size, y_pos + y_size);
	sui_draw_set_vec2(vertex, 1, x_pos + x_size, y_pos - y_size);
	sui_draw_set_vec2(vertex, 2, x_pos - x_size, y_pos - y_size);
	sui_draw_set_vec2(vertex, 3, x_pos - x_size, y_pos + y_size);
	sui_draw_set_vec2(vertex, 4, x_pos + x_size, y_pos + y_size);
	sui_draw_set_vec2(vertex, 5, x_pos + x_size + shadow_size, y_pos + y_size + shadow_size);
	sui_draw_set_vec2(vertex, 6, x_pos - x_size - shadow_size, y_pos + y_size + shadow_size);
	sui_draw_set_vec2(vertex, 7, x_pos - x_size, y_pos + y_size);
	sui_draw_set_vec2(vertex, 8, x_pos + x_size, y_pos - y_size);
	sui_draw_set_vec2(vertex, 9, x_pos + x_size + shadow_size, y_pos - y_size - shadow_size);
	sui_draw_set_vec2(vertex, 10, x_pos - x_size - shadow_size, y_pos - y_size - shadow_size);
	sui_draw_set_vec2(vertex, 11, x_pos - x_size, y_pos - y_size);
	sui_draw_set_vec2(vertex, 12, x_pos + x_size, y_pos + y_size);
	sui_draw_set_vec2(vertex, 13, x_pos + x_size + shadow_size, y_pos + y_size + shadow_size);
	sui_draw_set_vec2(vertex, 14, x_pos + x_size + shadow_size, y_pos - y_size - shadow_size);
	sui_draw_set_vec2(vertex, 15, x_pos + x_size, y_pos - y_size);
	sui_draw_set_vec2(vertex, 16, x_pos - x_size, y_pos + y_size);
	sui_draw_set_vec2(vertex, 17, x_pos - x_size - shadow_size, y_pos + y_size + shadow_size);
	sui_draw_set_vec2(vertex, 18, x_pos - x_size - shadow_size, y_pos - y_size - shadow_size);
	sui_draw_set_vec2(vertex, 19, x_pos - x_size, y_pos - y_size);

	sui_set_color_array_gl(SBackgroundData.window_color, 20, 4);
	sui_draw_gl(GL_TRIANGLES, vertex, 20, 2, 1, 1, 1, 1);
}
