
#include <math.h>
#include "st_matrix_operations.h"
#include "enough.h"
#include "seduce.h"
#include "persuade.h"

struct{
	float		*star_id;
	float		*star_color;
	float		*star_fade;
	uint		*star_ref;
//	NGLArray	square;
	double		intro;
	float		intro_t;
}GlobalIntro;



void qs_intro_init(void)
{
	float a, b, c, d;
	int i, j;


	a = sqrt(2.0);
	b = 1.0 / a;
	c = sin((22.5 * 2.0 * 3.14) / 360.0);
	d = cos((22.5 * 2.0 * 3.14) / 360.0);

	GlobalIntro.star_id = malloc((sizeof *GlobalIntro.star_id) * 35 * 2);

	sui_draw_set_vec2(GlobalIntro.star_id, 0, 32.0, 5.0);
	sui_draw_set_vec2(GlobalIntro.star_id, 1, 32.0 + c, 4.0 + d);
	sui_draw_set_vec2(GlobalIntro.star_id, 2, 32.0 + b, 4.0 + b);
	sui_draw_set_vec2(GlobalIntro.star_id, 3, 36.0 + b, b);
	sui_draw_set_vec2(GlobalIntro.star_id, 4, 36.0 + d, c);
	sui_draw_set_vec2(GlobalIntro.star_id, 5, 37.0, 0);
	sui_draw_set_vec2(GlobalIntro.star_id, 6, 36.0 + d, -c);
	sui_draw_set_vec2(GlobalIntro.star_id, 7, 36.0 + b, -b);
	sui_draw_set_vec2(GlobalIntro.star_id, 8, 32.0 + b, -4.0 - b);
	sui_draw_set_vec2(GlobalIntro.star_id, 9, 32.0 + c, -4.0 - d);
	sui_draw_set_vec2(GlobalIntro.star_id, 10, 32.0, -5.0);

	sui_draw_set_vec2(GlobalIntro.star_id, 11, 12.0, -5.0);
	sui_draw_set_vec2(GlobalIntro.star_id, 12, 12.0 - c, -4.0 - d);
	sui_draw_set_vec2(GlobalIntro.star_id, 13, 12.0 - b, -4.0 - b);
	sui_draw_set_vec2(GlobalIntro.star_id, 14, 8.0 - b, -b);
	sui_draw_set_vec2(GlobalIntro.star_id, 15, 8.0 - d, -c);
	sui_draw_set_vec2(GlobalIntro.star_id, 16, 7.0, 0.0);
	sui_draw_set_vec2(GlobalIntro.star_id, 17, 8.0 - d, c);
	sui_draw_set_vec2(GlobalIntro.star_id, 18, 8.0 - b, b);
	sui_draw_set_vec2(GlobalIntro.star_id, 19, 12.0 - b, 4.0 + b);
	sui_draw_set_vec2(GlobalIntro.star_id, 20, 12.0 - c, 4.0 + d);
	sui_draw_set_vec2(GlobalIntro.star_id, 21, 12.0, 5.0);

	sui_draw_set_vec2(GlobalIntro.star_id, 22, 30.0 + a, 3.0);
	sui_draw_set_vec2(GlobalIntro.star_id, 23, 33.0 + a, 0);
	sui_draw_set_vec2(GlobalIntro.star_id, 24, 30.0 + a, -3.0);
	sui_draw_set_vec2(GlobalIntro.star_id, 25, 14.0 - a, -3.0);
	sui_draw_set_vec2(GlobalIntro.star_id, 26, 11.0 - a, 0);
	sui_draw_set_vec2(GlobalIntro.star_id, 27, 14.0 - a, 3.0);

	sui_draw_set_vec2(GlobalIntro.star_id, 28, 30.0 + a, 3.0);
	sui_draw_set_vec2(GlobalIntro.star_id, 29, 33.0 + a, 0);
	sui_draw_set_vec2(GlobalIntro.star_id, 30, 30.0 + a, -3.0);
	sui_draw_set_vec2(GlobalIntro.star_id, 31, 14.0 - a, -3.0);
	sui_draw_set_vec2(GlobalIntro.star_id, 32, 11.0 - a, 0);
	sui_draw_set_vec2(GlobalIntro.star_id, 33, 14.0 - a, 3.0);
	sui_draw_set_vec2(GlobalIntro.star_id, 34, 22.0, 0.0);	

	GlobalIntro.star_color = malloc((sizeof *GlobalIntro.star_color) * 35 * 4);
	GlobalIntro.star_fade = malloc((sizeof *GlobalIntro.star_fade) * 35 * 4);


	for(i = 0; i < 28; i++)
		sui_draw_set_vec4(GlobalIntro.star_color, i, 0, 0, 0, 1);
	sui_draw_set_vec4(GlobalIntro.star_color, 28, 0.8, 0, 0, 1);
	sui_draw_set_vec4(GlobalIntro.star_color, 29, 0.9, 0, 0, 1);
	sui_draw_set_vec4(GlobalIntro.star_color, 30, 0.8, 0, 0, 1);
	sui_draw_set_vec4(GlobalIntro.star_color, 31, 0.8, 0, 0, 1);
	sui_draw_set_vec4(GlobalIntro.star_color, 32, 0.9, 0, 0, 1);
	sui_draw_set_vec4(GlobalIntro.star_color, 33, 0.8, 0, 0, 1);
	sui_draw_set_vec4(GlobalIntro.star_color, 34, 1, 0.05, 0.05, 1);
	GlobalIntro.star_ref = malloc((sizeof *GlobalIntro.star_ref) * 34 * 3);

	sui_draw_set_ivec3(GlobalIntro.star_ref, 0, 0, 1, 22);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 1, 1, 2, 22);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 2, 2, 3, 22);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 3, 3, 23, 22);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 4, 3, 4, 23);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 5, 23, 4, 5);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 6, 23, 5, 6);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 7, 23, 6, 7);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 8, 23, 7, 8);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 9, 8, 24, 23);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 10, 24, 8, 9);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 11, 24, 9, 10);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 12, 24, 10, 11);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 13, 24, 11, 25);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 14, 25, 11, 12);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 15, 25, 12, 13);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 16, 25, 13, 14);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 17, 25, 14, 26);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 18, 26, 14, 15);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 19, 26, 15, 16);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 20, 26, 16, 17);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 21, 26, 17, 18);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 22, 26, 18, 19);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 23, 26, 19, 27);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 24, 27, 19, 20);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 25, 27, 20, 21);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 26, 27, 21, 0);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 27, 27, 0, 22);

	sui_draw_set_ivec3(GlobalIntro.star_ref, 28, 34, 28, 29);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 29, 34, 29, 30);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 30, 34, 30, 31);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 31, 34, 31, 32);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 32, 34, 32, 33);
	sui_draw_set_ivec3(GlobalIntro.star_ref, 33, 34, 33, 28);
/*
	GlobalIntro.square = malloc((sizeof *GlobalIntro.square) * 4 * 2);

	sui_draw_set_vec2(GlobalIntro.square, 0, 150, 150);
	sui_draw_set_vec2(GlobalIntro.square, 1, 150, -150);
	sui_draw_set_vec2(GlobalIntro.square, 2, -150, -150);
	sui_draw_set_vec2(GlobalIntro.square, 3, -150, 150);
*/
	GlobalIntro.intro_t = 0;
	GlobalIntro.intro = TRUE;
}

void draw_star(float time, float *color)
{
	uint i;
	for(i = 0; i < 6; i++)
	{
		glPushMatrix();
		glRotatef(60 * i, 0, 0, 1);
		glRotatef(50 * (1 - time), 0, 1, 0);
		glRotatef(50 * (1 - time), 1, 0, 0);
//		nglDrawArray(NGL_TRIANGLES_FILLED, GlobalIntro.star_id, &color, 1, GlobalIntro.star_m_id, GlobalIntro.star_ref);
		sui_set_color_array_gl(color, 35, 4);
		sui_draw_ellements_gl(GL_TRIANGLES, GlobalIntro.star_id, GlobalIntro.star_ref, 34 * 3, 2, 0, 0, 0);
		glPopMatrix();
	}
}
/*

extern void sui_draw_gl(uint draw_type, float *array, uint length, uint dimentions, float red, float green, float blue);
extern void sui_draw_2d_line_gl(float start_x, float start_y, float end_x, float end_y, float red, float green, float blue);
extern void sui_draw_3d_line_gl(float start_x, float start_y,  float start_z, float end_x, float end_y, float end_z, float red, float green, float blue);
extern void sui_draw_ellements_gl(uint draw_type, float *array, uint *reference, uint length, uint dimentions, float red, float green, float blue);

extern void sui_set_blend_gl(uint source, uint destination);

extern void sui_set_color_array_gl(float *array, uint length, uint channels);
extern void sui_set_normal_array_gl(float *array, uint length);
extern void sui_set_texture2D_array_gl(float *array, uint length, uint dimentions, uint texture);

*/
void draw_pointer(float time, float *color)
{
	uint i;
	glPushMatrix();

	glTranslatef(0, 0,-10);
	glScalef (0.005, 0.005, 0.005);
	glRotatef(time * 0.1, 0, 0, 1);
	for(i = 0; i < 6; i++)
	{
		glPushMatrix();
		glRotatef(60 * i, 0, 0, 1);
	//	nglDrawArray(NGL_TRIANGLES_FILLED, GlobalIntro.star_id, &GlobalIntro.star_color, 1, GlobalIntro.star_add_id, GlobalIntro.star_ref);
		sui_set_color_array_gl(color, 35, 4);
		sui_draw_ellements_gl(GL_TRIANGLES, GlobalIntro.star_id, GlobalIntro.star_ref, 34 * 3, 2, 0, 0, 0);
		glPopMatrix();
	}
	glPopMatrix();
}

#define INTRO_STAR_COUNT 200

void set_intro_mode(double run)
{
	if(run)
		GlobalIntro.intro_t = 0;
}

boolean qs_intro_draw(void)
{
	uint i;
	float t = 0, border[16] = {-1, 1, 1, 1, 1, 0.45, -1, 0.45, -1, -1, 1, -1, 1, -0.45, -1, -0.45};
//	Point position;
	if(GlobalIntro.intro != TRUE)
		return FALSE;
	glDisable(GL_CULL_FACE);
	glPushMatrix();
	glScalef (0.08, 0.08, 0.04);
	glPushMatrix();

	glTranslatef(0,0,-1000);
	glScalef(1 + GlobalIntro.intro_t * 2.8, 1, 1);

//	nglDrawArray(NGL_QUADS_FILLED, GlobalIntro.square, NULL, 0, mui_get_material(3), 0);
	glPopMatrix();

	if(GlobalIntro.intro_t > 0.6)
	{
		t = (GlobalIntro.intro_t - 0.6) * 2.5;
		t = (1 - (1 - t) * (1 - t));
	}
	for(i = 0; i < 28; i++)
		sui_draw_set_vec4(GlobalIntro.star_fade, i, t * (0.5 * t + 0.5), t * (0.7 * t + 0.3), t, 1);
	sui_draw_set_vec4(GlobalIntro.star_fade, 28, 0.8 + t, t * 2, t * 2, 1);
	sui_draw_set_vec4(GlobalIntro.star_fade, 29, 0.9 + t, t * 2, t * 2, 1);
	sui_draw_set_vec4(GlobalIntro.star_fade, 30, 0.8 + t, t * 2, t * 2, 1);
	sui_draw_set_vec4(GlobalIntro.star_fade, 31, 0.8 + t, t * 2, t * 2, 1);
	sui_draw_set_vec4(GlobalIntro.star_fade, 32, 0.9 + t, t * 2, t * 2, 1);
	sui_draw_set_vec4(GlobalIntro.star_fade, 33, 0.8 + t, t * 2, t * 2, 1);
	sui_draw_set_vec4(GlobalIntro.star_fade, 34, 1 + t, 0.05 + t * 2, 0.05 + t * 2, 1);

	for(i = 0; i < INTRO_STAR_COUNT; i++)
	{
		glPushMatrix();
		glTranslatef(sin(i * 0.7) * 1000 * cos(i * 0.5) + sin(GlobalIntro.intro_t * 72 + i) * 20, cos(i * 0.4) * 200 * sin(i * 0.9) + sin(GlobalIntro.intro_t * 54 + i) * 20, 5000 * (1 - GlobalIntro.intro_t) - 5300 + (INTRO_STAR_COUNT - i) * (5000 / INTRO_STAR_COUNT));
		glRotatef(4000 * (i - GlobalIntro.intro_t) + 180, 0, 1, 0);
		glRotatef(4000 * (i - GlobalIntro.intro_t), 1, 0, 0);
		draw_star(0, GlobalIntro.star_fade);
		glPopMatrix();
	}
	if(GlobalIntro.intro_t > 0.5)
	{
		if(GlobalIntro.intro_t < 0.8)
			t = (GlobalIntro.intro_t - 0.5) / 0.3;
		else
			t = 1;
		glPushMatrix();
		glTranslatef(0, 0, 2000 * (1 - t) * (1 - t) * (1 - t) - 1200);
		glRotatef(400 * (1 - t) * (1 - t) * (1 - t), 0, 1, 0);
		glRotatef(400 * (1 - t) * (1 - t) * (1 - t), 1, 0, 0);
		glScalef(3, 3, 3);
		draw_star((1 - (1 - t) * (1 - t)), GlobalIntro.star_color);
		glPopMatrix();
	}
	glPopMatrix();	
	t = GlobalIntro.intro_t;
	glPushMatrix();
		glTranslatef(-1 + (1 - t) * (1 - t) * 4 , 0.3, -1);
		glScalef(3, 3, 3);
	//	mui_draw_text("QUEL", UITM_INTRO, NULL, NULL, 1 - (1 - t) * (1 - t) * (1 - t), position);
		sui_draw_text(0, 0, 0.02, 2, "QUEL", 0, 0, 0);   
	glPopMatrix();
		glPushMatrix();
		glTranslatef(0.8 - (1 - t) * (1 - t) * 8 , -0.25, -1);
		glScalef(3, 3, 3);
	//	mui_draw_text("SOLAAR", UITM_INTRO, NULL, NULL, 1 - (1 - t) * (1 - t) * (1 - t), position);	
		sui_draw_text(0, 0, 0.02, 2, "SOLAAR", 0, 0, 0);  
	glPopMatrix();	

	if(GlobalIntro.intro_t < 1)
		GlobalIntro.intro_t +=  0.025 * betray_get_delta_time();
	glPushMatrix();
//	glTranslatef(0, 0, -1);
	sui_draw_gl(GL_QUADS, border, 8, 2, 0, 0, 0);
	glPopMatrix();
	return TRUE;
}

