#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*#include "st_types.h"
#include "st_text.h"
#include "verse.h"*/
#include "enough.h"
#include "seduce.h"

#define LOGGO_RING_SPLIT 48
#define LOGGO_SMALL_RING_SPLIT 48
#define CO_INIT_DUST_COUNT 2000


struct{
	float	*ring;
	float	*small_rings;
	float	*larger_ring;
	float	*lens_vertex;
	float	*lens_color;
	float	*dust;
}COIntroData;

void create_ring_section(uint start, uint sections, float start_angle, float angle)
{
	uint i, temp;
	double x, y;
	for(i = 0; i < sections; i++)
	{
		x = sin((double)i * angle * 3.14 / 180 / (double)sections + start_angle * 3.14 / 180);
		y = cos((double)i * angle * 3.14 / 180 / (double)sections + start_angle * 3.14 / 180);
		sui_draw_set_vec2(COIntroData.ring, 4 * (i + start) + 0, x * 0.20, y * 0.20);
		sui_draw_set_vec2(COIntroData.ring, 4 * (i + start) + 1, x * 0.16, y * 0.16);
		x = sin((double)(i + 1) * angle * 3.14 / 180 / (double)sections + start_angle * 3.14 / 180);
		y = cos((double)(i + 1) * angle * 3.14 / 180 / (double)sections + start_angle * 3.14 / 180);
		sui_draw_set_vec2(COIntroData.ring, 4 * (i + start) + 2, x * 0.16, y * 0.16);
		sui_draw_set_vec2(COIntroData.ring, 4 * (i + start) + 3, x * 0.20, y * 0.20);

	}
}

void co_intro_init()
{
	uint i, temp;
	double x, y, z, size;

	COIntroData.ring = malloc((sizeof * COIntroData.ring) * LOGGO_RING_SPLIT * 4 * 2);
	COIntroData.small_rings = malloc((sizeof * COIntroData.small_rings) * LOGGO_SMALL_RING_SPLIT * 4 * 2);
	COIntroData.larger_ring = malloc((sizeof * COIntroData.larger_ring) * LOGGO_SMALL_RING_SPLIT * 4 * 2);
	COIntroData.lens_vertex = malloc((sizeof * COIntroData.lens_vertex) * LOGGO_SMALL_RING_SPLIT * 3 * 2);
	COIntroData.lens_color = malloc((sizeof * COIntroData.lens_color) * LOGGO_SMALL_RING_SPLIT * 3 * 4);
	COIntroData.dust = malloc((sizeof * COIntroData.dust) * CO_INIT_DUST_COUNT * 3);
	create_ring_section(0, 4, -69, 15);
	create_ring_section(4, (LOGGO_RING_SPLIT - 4) / 2, -5.7, 120);
	create_ring_section((LOGGO_RING_SPLIT - 4) / 2 + 4, (LOGGO_RING_SPLIT - 4) / 2, 156.5, 93);

	for(i = 0; i < CO_INIT_DUST_COUNT * 3;)
	{
		x = get_rand(i * 3) * 2 - 1;
		y = get_rand(i * 3 + 1) * 4 * 3.14;
		z = 0.2 + get_rand(i * 3 + 2);

		COIntroData.dust[i++] = 0.7 * sin(y) * z;
		COIntroData.dust[i++] = 0.4 * x / z;
		COIntroData.dust[i++] = 0.7 * cos(y) * z;
		y += 0.04;
		COIntroData.dust[i++] = 0.7 * sin(y) * z;
		COIntroData.dust[i++] = 0.4 * x / z;
		COIntroData.dust[i++] = 0.7 * cos(y) * z;

	}
	for(i = 0; i < LOGGO_SMALL_RING_SPLIT; i++)
	{
		x = sin((double)i * 2.0 * 3.14 / LOGGO_SMALL_RING_SPLIT);
		y = cos((double)i * 2.0 * 3.14 / LOGGO_SMALL_RING_SPLIT);
		temp = ((i + LOGGO_SMALL_RING_SPLIT - 1) % LOGGO_SMALL_RING_SPLIT);
		sui_draw_set_vec2(COIntroData.small_rings, 4 * i + 2, -0.19 + y * 0.04, x * 0.04);
		sui_draw_set_vec2(COIntroData.small_rings, 4 * i +  3, -0.19 + y * 0.06, x * 0.06);
		sui_draw_set_vec2(COIntroData.small_rings, 4 * temp + 0, -0.19 + y * 0.06, x * 0.06);
		sui_draw_set_vec2(COIntroData.small_rings, 4 * temp + 1, -0.19 + y * 0.04, x * 0.04);
		sui_draw_set_vec2(COIntroData.larger_ring, 4 * i + 2, -0.19 + y * 0.05, x * 0.05);
		sui_draw_set_vec2(COIntroData.larger_ring, 4 * i +  3, -0.19 + y * 0.07, x * 0.07);
		sui_draw_set_vec2(COIntroData.larger_ring, 4 * temp + 0, -0.19 + y * 0.07, x * 0.07);
		sui_draw_set_vec2(COIntroData.larger_ring, 4 * temp + 1, -0.19 + y * 0.05, x * 0.05);

		sui_draw_set_vec2(COIntroData.lens_vertex, 3 * i + 0, -0.19 + x * 0.05, y * 0.05);
		sui_draw_set_vec2(COIntroData.lens_vertex, 3 * i + 1, -0.19 + 0, 0);
		sui_draw_set_vec2(COIntroData.lens_vertex, 3 * temp + 2, -0.19 + x * 0.05, y * 0.05);
		sui_draw_set_vec4(COIntroData.lens_color, 3 * i, 0.9, 1, 0.1, 1);
		sui_draw_set_vec4(COIntroData.lens_color, 3 * i + 1, 1, 1, 0.5, 0.4);
		sui_draw_set_vec4(COIntroData.lens_color, 3 * i + 2, 0.9, 1, 0.1, 1);

	}
}
void co_vng_ring(void);
void co_vng_sun();
void co_pfx_sun_stars(void);

void co_intro_symbol(float color)
{
	glPushMatrix();

sui_draw_gl(GL_QUADS, COIntroData.ring, LOGGO_RING_SPLIT * 4, 2, color, color, color);


sui_set_color_array_gl(COIntroData.lens_color, LOGGO_SMALL_RING_SPLIT * 3, 4);
sui_draw_gl(GL_TRIANGLES, COIntroData.lens_vertex, LOGGO_SMALL_RING_SPLIT * 3, 2, color, color, color);
sui_draw_gl(GL_QUADS, COIntroData.small_rings, LOGGO_SMALL_RING_SPLIT * 4, 2, color, color, color);

	glRotated(135, 0, 0, 1);

sui_set_color_array_gl(COIntroData.lens_color, LOGGO_SMALL_RING_SPLIT * 3, 4);
sui_draw_gl(GL_TRIANGLES, COIntroData.lens_vertex, LOGGO_SMALL_RING_SPLIT * 3, 2, color, color, color);
sui_draw_gl(GL_QUADS, COIntroData.small_rings, LOGGO_SMALL_RING_SPLIT * 4, 2, color, color, color);

	glRotated(-135 - 60, 0, 0, 1);

sui_set_color_array_gl(COIntroData.lens_color, LOGGO_SMALL_RING_SPLIT * 3, 4);
sui_draw_gl(GL_TRIANGLES, COIntroData.lens_vertex, LOGGO_SMALL_RING_SPLIT * 3, 2, color, color, color);
sui_draw_gl(GL_QUADS, COIntroData.larger_ring, LOGGO_SMALL_RING_SPLIT * 4, 2, color, color, color);
	glPopMatrix();


}

void co_intro_draw()
{
	static float timer = 2;
	float color, cam = 1, x, y, z, aspect, border[16] = {-1, 1, 1, 1, 1, 0.45, -1, 0.45, -1, -1, 1, -1, 1, -0.45, -1, -0.45};
	uint i;

	aspect = betray_get_screen_mode(NULL, NULL, NULL);

	if(timer > 0)
		timer -= 0.008;
	if(timer > 1)
		cam = 1 - (timer - 1) * (timer - 1);
	glPushMatrix();
	glTranslatef(-((cam - 1) * (cam - 1)), 0, 0.8 - cam);
	glRotatef(180 * (cam - 1), 0, 1, 0);

	for(i = 0; i < CO_INIT_DUST_COUNT * 3;)
	{
		x = get_rand(i * 3) * 2 - 1;
		z = 0.2 + get_rand(i * 3 + 2);
		y = get_rand(i * 3 + 1) * 4 * 3.14 + timer * (float)i / (CO_INIT_DUST_COUNT * z) ;


		COIntroData.dust[i++] = 0.7 * sin(y) * z;
		COIntroData.dust[i++] = 0.15 * x / z;
		COIntroData.dust[i++] = 0.7 * cos(y) * z;
		y += 0.02;
		COIntroData.dust[i++] = 0.7 * sin(y) * z;
		COIntroData.dust[i++] = 0.15 * x / z;
		COIntroData.dust[i++] = 0.7 * cos(y) * z;

	}

	if(timer > 0.5)
	{
		glPushMatrix();
		glRotatef(-180 * (cam - 1), 0, 1, 0);
		glScaled(0.4, 0.4, 0.4);
		co_vng_sun();
		co_vng_sunrays();
		glPopMatrix();

		glPushMatrix();
		glRotatef(-180 * (cam - 1), 0, 1, 0);
		glTranslatef(((1 - cam) + 0.19) * sin(3.14 / 180 * 135), 0.19 * cos(3.14 / 180 * 135), 0);
		glScaled(0.1, 0.1, 0.1);
		co_vng_geo_planet();
		co_vng_saturn_ring();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(((1 - cam) + 0.19) * sin(3.14 / 180 * -90), 0.19 * cos(3.14 / 180 * -90), 0);
		glScaled(0.1, 0.1, 0.1);
		glRotatef(-180 * (cam - 1), 0, 1, 0);
		co_vng_dust_planet();
		co_vng_sunrays();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(((1 - cam) + 0.19) * sin(3.14 / 180 * -30), 0.19 * cos(3.14 / 180 * -30), 0);
		glScaled(0.12, 0.12, 0.12);
		glRotatef(-180 * (cam - 1), 0, 1, 0);
		co_vng_gas_planet();
		co_vng_ring();
		glPopMatrix();
	}

	color = (1 - timer) * (1 - timer);
	if(timer < 1)
		co_intro_symbol(timer * timer);
	sui_set_blend_gl(GL_ZERO, GL_SRC_COLOR);
	sui_draw_gl(GL_LINES, COIntroData.dust, CO_INIT_DUST_COUNT, 3, color, color, color);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-180 * timer, 0, 1, 0);
	glTranslatef(0.3 + 0.4 * (2 - timer), 0.1 * timer, 0);
	glScaled(0.2, 0.2, 0.2);
	glRotatef(180 * timer, 0, 1, 0);
	co_vng_gas_planet();
	co_vng_ring();
	glPopMatrix();

	glPushMatrix();
	glRotatef(230 - 180 * timer, 0, 1, 0);
	glTranslatef(0.3 + 0.4 * (2 - timer), -0.2 * timer, 0);
	glScaled(0.2, 0.2, 0.2);
	glRotatef(-230 + 180 * timer, 0, 1, 0);
	co_vng_gas_planet();
	co_vng_ring();
	glPopMatrix();

	glPushMatrix();
	glRotatef(190 - 180 * timer, 0, 1, 0);
	glTranslatef(0.3 + 0.4 * (2 - timer), 0.3 * timer, 0);
	glScaled(0.2, 0.2, 0.2);
	glRotatef(-190 + 180 * timer, 0, 1, 0);
	co_vng_gas_planet();
	co_vng_ring();
	glPopMatrix();
	sui_draw_gl(GL_QUADS, border, 8, 2, 0, 0, 0);
}
