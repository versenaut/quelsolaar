#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "enough.h"
#include "seduce.h"
#include "co_vn_graphics.h"
#include "co_vn_handle.h"
#include "co_widgets.h"

#define SUN_RAYS 13
#define SUN_RING_SPLIT 49
#define SUN_PARTICLES 6000
#define DUST_PARTICLES 200
#define MOON_SPLIT 8
#define CONNECT_SPLIT 24
#define SATURN_RINGS 32
#define PLANET_SPLITS 8
#define GAS_PLANET_SPLITS 8
#define BLACK_HOLE_SPLITS 40
#define FRAG_BASE_SPLIT 30
#define FRAG_TOP_SPLIT 10
#define FRAG_RADIUS 0.15
#define FRAG_TOP_RADIUS 0.02
#define COLOR_RING_SPLITS 32


struct{
	float	*ring;
	float	*qubes;
	float	*rays;
	float	*color_vertex;
	float	*color_ref;
	float	*color_color;
	float	*name;
	float	*dust;
	float	*moon;
	float	*moon_line;
	float	*connector_one;
	float	*connector_two;
	float	*frag_top;
	float	*frag_base;
	float	*particles_array;
	double	time;
}COVNGraphicsData;

void co_vng_init()
{
	uint i, temp;
	double x, y, z, size;

	/* Ring */

	COVNGraphicsData.ring = malloc((sizeof *COVNGraphicsData.ring) * SUN_RING_SPLIT * 4);
	for(i = 0; i < SUN_RING_SPLIT; i++)
	{
		sui_draw_set_vec2(COVNGraphicsData.ring, 2 * i, sin((double)i * 2.0 * 3.14 / SUN_RING_SPLIT), cos((double)i * 2.0 * 3.14 / SUN_RING_SPLIT));
		sui_draw_set_vec2(COVNGraphicsData.ring, 2 * i + 1, sin((double)(i + 1) * 2.0 * 3.14 / SUN_RING_SPLIT), cos((double)(i + 1) * 2.0 * 3.14 / SUN_RING_SPLIT));
	}

	COVNGraphicsData.color_vertex = malloc((sizeof *COVNGraphicsData.color_vertex) * COLOR_RING_SPLITS * 2 + 2);
	COVNGraphicsData.color_ref = malloc((sizeof *COVNGraphicsData.color_ref) * COLOR_RING_SPLITS * 3);
	COVNGraphicsData.color_color = malloc((sizeof *COVNGraphicsData.color_color) * COLOR_RING_SPLITS * 3 + 3);

	for(i = 0; i < COLOR_RING_SPLITS; i++)
	{
		x = sin(((double)i) * 2.0 * 3.14 / COLOR_RING_SPLITS);
		y = cos(((double)i) * 2.0 * 3.14 / COLOR_RING_SPLITS);
		sui_draw_set_vec2(COVNGraphicsData.color_vertex, i, x, y);
		sui_draw_set_vec3(COVNGraphicsData.color_color, i, 1, 1, 1);
		sui_draw_set_ivec3(COVNGraphicsData.color_ref, i, i, (i + 1) % COLOR_RING_SPLITS, COLOR_RING_SPLITS);
	}
	sui_draw_set_vec2(COVNGraphicsData.color_vertex, i, 0, 0);

	/* Saturn rings */

	COVNGraphicsData.qubes = malloc((sizeof *COVNGraphicsData.qubes) * SATURN_RINGS * 8 * 2);
	for(i = 0; i < SATURN_RINGS; i++)
	{
		size = 0.2;
		if(i % 16 == 0)
			size = 0.4;
		else if(i % 3 == 0)
			size = 0.3;
		else if(i % 4 == 0)
			size = 0.05;
		x = sin(((double)i - size) * 2.0 * 3.14 / SATURN_RINGS);
		y = cos(((double)i - size) * 2.0 * 3.14 / SATURN_RINGS);
		sui_draw_set_vec2(COVNGraphicsData.qubes, 8 * i + 0, x * 1.02, y * 1.02);
		sui_draw_set_vec2(COVNGraphicsData.qubes, 8 * i + 1, x * 1.26, y * 1.26);
		sui_draw_set_vec2(COVNGraphicsData.qubes, 8 * i + 4, x * 0.98, y * 0.98);
		sui_draw_set_vec2(COVNGraphicsData.qubes, 8 * i + 5, x * 0.94, y * 0.94);
		x = sin(((double)i + size) * 2.0 * 3.14 / SATURN_RINGS);
		y = cos(((double)i + size) * 2.0 * 3.14 / SATURN_RINGS);
		sui_draw_set_vec2(COVNGraphicsData.qubes, 8 * i + 2, x * 1.26, y * 1.26);
		sui_draw_set_vec2(COVNGraphicsData.qubes, 8 * i + 3, x * 1.02, y * 1.02);
		sui_draw_set_vec2(COVNGraphicsData.qubes, 8 * i + 6, x * 0.94, y * 0.94);
		sui_draw_set_vec2(COVNGraphicsData.qubes, 8 * i + 7, x * 0.98, y * 0.98);
	}

	/* Sun rays */

	COVNGraphicsData.rays = malloc((sizeof *COVNGraphicsData.rays) * SUN_RAYS * 4);
	for(i = 0; i < SUN_RAYS; i++)
	{
		sui_draw_set_vec2(COVNGraphicsData.rays, 2 * i, 0.7 * sin((double)i * 2.0 * 3.14 / SUN_RAYS), 0.7 * cos((double)i * 2.0 * 3.14 / SUN_RAYS));
		sui_draw_set_vec2(COVNGraphicsData.rays, 2 * i + 1, sin((double)i * 2.0 * 3.14 / SUN_RAYS), cos((double)i * 2.0 * 3.14 / SUN_RAYS));
	}

	COVNGraphicsData.particles_array = malloc((sizeof *COVNGraphicsData.particles_array) * SUN_PARTICLES * 4);
	for(i = 0; i < SUN_PARTICLES; i++)
	{
		temp = i * 4;
		temp = (temp<<13) ^ temp;
		x = (((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) - 1;
		temp = i * 4 + 1;
		temp = (temp<<13) ^ temp;
		y = (((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) - 1;

		COVNGraphicsData.particles_array[i * 4] = sin(((double)i / SUN_PARTICLES) * 2 * 3.14) * /*1.0*/2;
		COVNGraphicsData.particles_array[i * 4 + 1] = cos(((double)i / SUN_PARTICLES) * 2 * 3.14) * /*1.0*/2;
		COVNGraphicsData.particles_array[i * 4 + 2] = 0;
		COVNGraphicsData.particles_array[i * 4 + 3] = 0;
	}

	COVNGraphicsData.dust = malloc((sizeof *COVNGraphicsData.dust) * 3 * DUST_PARTICLES);

	for(i = 0; i < DUST_PARTICLES; i++)
	{
		temp = i * 3;
		temp = (temp<<13) ^ temp;
		x = (((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) - 1;
		temp = i * 3 + 1;
		temp = (temp<<13) ^ temp;
		y = (((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) - 1;
		temp = i * 3 + 2;
		temp = (temp<<13) ^ temp;
		z = (((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) - 1;
		size = sqrt(x * x + y * y + z * z);
		sui_draw_set_vec3(COVNGraphicsData.dust, i, x / size, y / size, z / size);
	}
	COVNGraphicsData.time = 0;
	COVNGraphicsData.name = malloc((sizeof *COVNGraphicsData.name) * 8);
	sui_draw_set_vec2(COVNGraphicsData.name, 0, 0.33, 0.33);
	sui_draw_set_vec2(COVNGraphicsData.name, 1, 0.6, 0.6);
	sui_draw_set_vec2(COVNGraphicsData.name, 2, 0.6, 0.6);
	sui_draw_set_vec2(COVNGraphicsData.name, 3, 0.9, 0.6);
	COVNGraphicsData.moon = malloc((sizeof *COVNGraphicsData.name) * 2 * (MOON_SPLIT * 14 + 2));
	for(i = 0; i < MOON_SPLIT * 2; i++)
	{
		sui_draw_set_vec2(COVNGraphicsData.moon, 2 * i, 0.04 * sin((double)i * 3.14 / MOON_SPLIT), 0.04 * cos((double)i * 3.14 / MOON_SPLIT));
		sui_draw_set_vec2(COVNGraphicsData.moon, 2 * i + 1, 0.04 * sin((double)(i + 1) * 3.14 / MOON_SPLIT), 0.04 * cos((double)(i + 1) * 3.14 / MOON_SPLIT));
	}
	for(; i < MOON_SPLIT * 3; i++)
	{
		sui_draw_set_vec2(COVNGraphicsData.moon, 2 * i, 0.08 + 0.04 * sin((double)i * 3.14 / MOON_SPLIT), 0.04 * cos((double)i * 3.14 / MOON_SPLIT));
		sui_draw_set_vec2(COVNGraphicsData.moon, 2 * i + 1, 0.08 + 0.04 * sin((double)(i + 1) * 3.14 / MOON_SPLIT), 0.04 * cos((double)(i + 1) * 3.14 / MOON_SPLIT));
	}
	for(; i < MOON_SPLIT * 4; i++)
	{
		sui_draw_set_vec2(COVNGraphicsData.moon, 2 * i, 0.08 + 0.02 * sin((double)i * 3.14 / MOON_SPLIT), 0.04 * cos((double)i * 3.14 / MOON_SPLIT));
		sui_draw_set_vec2(COVNGraphicsData.moon, 2 * i + 1, 0.08 + 0.02 * sin((double)(i + 1) * 3.14 / MOON_SPLIT), 0.04 * cos((double)(i + 1) * 3.14 / MOON_SPLIT));
	}
	for(; i < MOON_SPLIT * 5; i++)
	{
		sui_draw_set_vec2(COVNGraphicsData.moon, 2 * i, 0.16 + 0.04 * sin((double)i * 3.14 / MOON_SPLIT), 0.04 * cos((double)i * 3.14 / MOON_SPLIT));
		sui_draw_set_vec2(COVNGraphicsData.moon, 2 * i + 1, 0.16 + 0.04 * sin((double)(i + 1) * 3.14 / MOON_SPLIT), 0.04 * cos((double)(i + 1) * 3.14 / MOON_SPLIT));
	}
	for(; i < MOON_SPLIT * 6; i++)
	{
		sui_draw_set_vec2(COVNGraphicsData.moon, 2 * i, 0.24 + -0.04 * sin((double)i * 3.14 / MOON_SPLIT), 0.04 * cos((double)i * 3.14 / MOON_SPLIT));
		sui_draw_set_vec2(COVNGraphicsData.moon, 2 * i + 1, 0.24 + -0.04 * sin((double)(i + 1) * 3.14 / MOON_SPLIT), 0.04 * cos((double)(i + 1) * 3.14 / MOON_SPLIT));
	}
	for(; i < MOON_SPLIT * 7; i++)
	{
		sui_draw_set_vec2(COVNGraphicsData.moon, 2 * i, 0.24 + 0.02 * sin((double)i * 3.14 / MOON_SPLIT), 0.04 * cos((double)i * 3.14 / MOON_SPLIT));
		sui_draw_set_vec2(COVNGraphicsData.moon, 2 * i + 1, 0.24 + 0.02 * sin((double)(i + 1) * 3.14 / MOON_SPLIT), 0.04 * cos((double)(i + 1) * 3.14 / MOON_SPLIT));
	}
	sui_draw_set_vec2(COVNGraphicsData.moon, 2 * i + 0, 0.16, 0.04);
	sui_draw_set_vec2(COVNGraphicsData.moon, 2 * i + 1, 0.16, -0.04);
	COVNGraphicsData.connector_one = malloc((sizeof *COVNGraphicsData.connector_one) * CONNECT_SPLIT * 4);
	COVNGraphicsData.connector_two = malloc((sizeof *COVNGraphicsData.connector_two) * CONNECT_SPLIT * 4);
	for(i = 0; i < CONNECT_SPLIT; i++)
	{
		x = -0.015 * cos((double)i * 2.0 * 3.14 / CONNECT_SPLIT);
		y =	0.015 * sin((double)i * 2.0 * 3.14 / CONNECT_SPLIT);
		sui_draw_set_vec2(COVNGraphicsData.connector_one, 2 * i, x * 4, y * 4);
		sui_draw_set_vec2(COVNGraphicsData.connector_one,  (((2 * i + CONNECT_SPLIT * 2) - 1) % (CONNECT_SPLIT * 2)), x * 4, y * 4);
		x -= 0.02 * cos(((double)((i * 3) / CONNECT_SPLIT) + 0.5) * 2.0 * 3.14 / 3.0);
		y += 0.02 * sin(((double)((i * 3) / CONNECT_SPLIT) + 0.5) * 2.0 * 3.14 / 3.0);
		sui_draw_set_vec2(COVNGraphicsData.connector_two, 2 * i, x, y);
		sui_draw_set_vec2(COVNGraphicsData.connector_two, ((2 * i + CONNECT_SPLIT * 2) - 1) % (CONNECT_SPLIT * 2), x, y);
	}
	COVNGraphicsData.moon_line = malloc((sizeof *COVNGraphicsData.moon_line) * 8);
	sui_draw_set_vec2(COVNGraphicsData.moon_line, 0, -1.13, 0.21);
	sui_draw_set_vec2(COVNGraphicsData.moon_line, 1, -0.8, 0.21);
	sui_draw_set_vec2(COVNGraphicsData.moon_line, 2, -0.8, 0.21);
	sui_draw_set_vec2(COVNGraphicsData.moon_line, 3, -1.0, 0.04);

	COVNGraphicsData.frag_top = malloc((sizeof *COVNGraphicsData.frag_top) * (FRAG_TOP_SPLIT * 2 + 8) * 2);
	for(i = 0; i < FRAG_TOP_SPLIT / 2; i++)
	{
		x = -FRAG_TOP_RADIUS * cos((double)i * 3.14 / FRAG_TOP_SPLIT);
		y =	-FRAG_TOP_RADIUS * sin((double)i * 3.14 / FRAG_TOP_SPLIT);
		sui_draw_set_vec2(COVNGraphicsData.frag_top, 2 * i, x + FRAG_TOP_RADIUS - FRAG_RADIUS, -y - FRAG_TOP_RADIUS);
		x = -FRAG_TOP_RADIUS * cos((double)(i + 1) * 3.14 / FRAG_TOP_SPLIT);
		y =	-FRAG_TOP_RADIUS * sin((double)(i + 1) * 3.14 / FRAG_TOP_SPLIT);
		sui_draw_set_vec2(COVNGraphicsData.frag_top, 2 * i + 1, x + FRAG_TOP_RADIUS - FRAG_RADIUS, -y - FRAG_TOP_RADIUS);
	}
	for(; i < FRAG_TOP_SPLIT; i++)
	{
		x = -FRAG_TOP_RADIUS * cos((double)i * 3.14 / FRAG_TOP_SPLIT);
		y =	-FRAG_TOP_RADIUS * sin((double)i * 3.14 / FRAG_TOP_SPLIT);
		sui_draw_set_vec2(COVNGraphicsData.frag_top, 2 * i, x - FRAG_TOP_RADIUS + FRAG_RADIUS, -y - FRAG_TOP_RADIUS);
		x = -FRAG_TOP_RADIUS * cos((double)(i + 1) * 3.14 / FRAG_TOP_SPLIT);
		y =	-FRAG_TOP_RADIUS * sin((double)(i + 1) * 3.14 / FRAG_TOP_SPLIT);
		sui_draw_set_vec2(COVNGraphicsData.frag_top, 2 * i + 1, x - FRAG_TOP_RADIUS + FRAG_RADIUS, -y - FRAG_TOP_RADIUS);
	}
	sui_draw_set_vec2(COVNGraphicsData.frag_top, FRAG_TOP_SPLIT * 2 + 0, FRAG_TOP_RADIUS - FRAG_RADIUS, -FRAG_TOP_RADIUS * 2);
	sui_draw_set_vec2(COVNGraphicsData.frag_top, FRAG_TOP_SPLIT * 2 + 1, -FRAG_TOP_RADIUS + FRAG_RADIUS, -FRAG_TOP_RADIUS * 2);
	sui_draw_set_vec2(COVNGraphicsData.frag_top, FRAG_TOP_SPLIT * 2 + 2, FRAG_TOP_RADIUS - FRAG_RADIUS, 0);
	sui_draw_set_vec2(COVNGraphicsData.frag_top, FRAG_TOP_SPLIT * 2 + 3, -FRAG_TOP_RADIUS + FRAG_RADIUS, 0);
	sui_draw_set_vec2(COVNGraphicsData.frag_top, FRAG_TOP_SPLIT * 2 + 4, FRAG_RADIUS, -FRAG_TOP_RADIUS);
	sui_draw_set_vec2(COVNGraphicsData.frag_top, FRAG_TOP_SPLIT * 2 + 6, -FRAG_RADIUS, -FRAG_TOP_RADIUS);

	COVNGraphicsData.frag_base = malloc((sizeof *COVNGraphicsData.frag_base) * (FRAG_BASE_SPLIT * 2) * 2);
	for(i = 0; i < FRAG_BASE_SPLIT; i++)
	{
		x = -FRAG_RADIUS * cos((double)i * 3.14 / FRAG_BASE_SPLIT);
		y =	FRAG_RADIUS * sin((double)i * 3.14 / FRAG_BASE_SPLIT);
		sui_draw_set_vec2(COVNGraphicsData.frag_base, 2 * i, x, -y);
		x = -FRAG_RADIUS * cos((double)(i + 1) * 3.14 / FRAG_BASE_SPLIT);
		y =	FRAG_RADIUS * sin((double)(i + 1) * 3.14 / FRAG_BASE_SPLIT);
		sui_draw_set_vec2(COVNGraphicsData.frag_base, 2 * i + 1, x, -y);
	}
}

void co_vng_update_time()
{
	uint i, temp;
	float outbreak, length, x, y;
	COVNGraphicsData.time = betray_get_time();
	temp = COVNGraphicsData.time;
	temp = (temp<<13) ^ temp;
	outbreak = (float)((temp * (temp * temp * (uint)15731 + (uint)789221) + (uint)1376312589) & 0x7fffffff) / 1073741824.0;
	x = COVNGraphicsData.particles_array[(uint)(outbreak * SUN_PARTICLES / 2) * 2];
	y = COVNGraphicsData.particles_array[(uint)(outbreak * SUN_PARTICLES / 2) * 2 + 1];
	for(i = 0; i < SUN_PARTICLES; i++)
	{
		length = (COVNGraphicsData.particles_array[i * 2] - x) * (COVNGraphicsData.particles_array[i * 2] - x) + (COVNGraphicsData.particles_array[i * 2 + 1] - y) * (COVNGraphicsData.particles_array[i * 2 + 1] - y);
		length++;
		COVNGraphicsData.particles_array[(SUN_PARTICLES + i) * 2 + 0] += ((cos(length * 6) * y * 12 / length) + (COVNGraphicsData.particles_array[i * 2] - x) * -0.4) / 20 + x * 0.01;
		COVNGraphicsData.particles_array[(SUN_PARTICLES + i) * 2 + 1] += ((sin(length * 6) * x * 12 / length) + (COVNGraphicsData.particles_array[i * 2 + 1] - y) * -0.4) / 20 + y * 0.01;
	}
	for(i = 0; i < SUN_PARTICLES; i++)
	{
		temp = i * 4;
		temp = (temp<<13) ^ temp;
		x = (float)((temp * (temp * temp * (uint)15731 + (uint)789221) + (uint)1376312589) & 0x7fffffff)  - 1;
		COVNGraphicsData.particles_array[(SUN_PARTICLES + i) * 2 + 0] *= 0.98;
		COVNGraphicsData.particles_array[(SUN_PARTICLES + i) * 2 + 1] *= 0.98;
		COVNGraphicsData.particles_array[(SUN_PARTICLES + i) * 2 + 0] -= COVNGraphicsData.particles_array[i * 2] * 0.02;
		COVNGraphicsData.particles_array[(SUN_PARTICLES + i) * 2 + 1] -= COVNGraphicsData.particles_array[i * 2 + 1] * 0.02;
		if(COVNGraphicsData.particles_array[i * 2] * COVNGraphicsData.particles_array[i * 2] + COVNGraphicsData.particles_array[i * 2 + 1] * COVNGraphicsData.particles_array[i * 2 + 1] < 1.0)
		{
			COVNGraphicsData.particles_array[(SUN_PARTICLES + i) * 2 + 0] = COVNGraphicsData.particles_array[(SUN_PARTICLES + i) * 2 + 0] * 0.8;
			COVNGraphicsData.particles_array[(SUN_PARTICLES + i) * 2 + 1] = COVNGraphicsData.particles_array[(SUN_PARTICLES + i) * 2 + 1] * 0.8;
			COVNGraphicsData.particles_array[i * 2] = -COVNGraphicsData.particles_array[i * 2];
			COVNGraphicsData.particles_array[i * 2 + 1] = -COVNGraphicsData.particles_array[i * 2 + 1];
		}
		COVNGraphicsData.particles_array[i * 2] += COVNGraphicsData.particles_array[(SUN_PARTICLES + i) * 2] * 0.002;
		COVNGraphicsData.particles_array[i * 2 + 1] += COVNGraphicsData.particles_array[(SUN_PARTICLES + i) * 2 + 1] * 0.002;
	}
}


void co_vng_fragment(float pos_x, float pos_y, float length, float color)
{
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	sui_draw_set_vec2(COVNGraphicsData.frag_top, FRAG_TOP_SPLIT * 2 + 5, FRAG_RADIUS, -length);
	sui_draw_set_vec2(COVNGraphicsData.frag_top, FRAG_TOP_SPLIT * 2 + 7, -FRAG_RADIUS, -length);
	sui_draw_gl(GL_LINES, COVNGraphicsData.frag_top, (FRAG_TOP_SPLIT * 2 + 8), 2, color, color, color);
	glTranslatef(0, -length, 0);
	sui_draw_gl(GL_LINES, COVNGraphicsData.frag_base, FRAG_BASE_SPLIT * 2, 2, color, color, color);
	glPopMatrix();
}


void co_vng_color(float red, float green, float blue)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_COLOR, GL_ZERO);
	sui_draw_set_vec3(COVNGraphicsData.color_color, COLOR_RING_SPLITS, red, green, blue);
	sui_set_color_array_gl(COVNGraphicsData.color_color, COLOR_RING_SPLITS, 3);	sui_draw_ellements_gl(GL_TRIANGLES, COVNGraphicsData.color_vertex, COVNGraphicsData.color_ref, COLOR_RING_SPLITS * 3, 2, red, green, blue);
}

void co_vng_sunrays()
{
	float color = 0.9;
	glPushMatrix();
	glRotatef(COVNGraphicsData.time * -0.01, 0, 0, 1);
	sui_draw_gl(GL_LINES, COVNGraphicsData.rays, SUN_RAYS * 2, 2, color, color, color);
	glPopMatrix();
}

void co_vng_dust_planet()
{
	float color = 0, color_light = 0.9;
	glPushMatrix();
	glScalef(0.4, 0.4, 0.4);
	sui_draw_gl(GL_LINES, COVNGraphicsData.ring, SUN_RING_SPLIT * 2, 2, color, color, color);
	glRotatef(COVNGraphicsData.time * -0.145, 0.6, 0.1, 1);
	sui_draw_gl(GL_POINTS, COVNGraphicsData.dust, DUST_PARTICLES, 3, color, color, color);
	glPopMatrix();
}



void co_vng_geo_planet()
{
	float color = 0, color_light = 0.9;
	uint i;
	float hight;
	glPushMatrix();
	glRotatef(-60, 1, 0.3, 0);
	glRotatef(COVNGraphicsData.time * -0.045, 0.6, 0.1, 1);
	glScalef(0.4, 0.4, 0.4);
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glPopMatrix();
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	for(i = 0; i < PLANET_SPLITS; i++)
	{
		glRotatef((180.0 / PLANET_SPLITS), 0, 1, 0);
		sui_draw_gl(GL_LINES, COVNGraphicsData.ring, SUN_RING_SPLIT * 2, 2, color_light, color_light, color_light);
	}
	glPopMatrix();
	for(i = 0; i < PLANET_SPLITS; i++)
	{
		glPushMatrix();
		hight = (float)i * (2.0 / PLANET_SPLITS) - 1.0;
		glTranslatef(0, 0, sin(hight * 3.14 / 2.0));
		glScalef(cos(hight * 3.14 / 2.0), cos(hight * 3.14 / 2.0), 1);
		sui_draw_gl(GL_LINES, COVNGraphicsData.ring, SUN_RING_SPLIT * 2, 2, color_light, color_light, color_light);
		glPopMatrix();
	}
	glPopMatrix();
	glPushMatrix();
	glScalef(0.4, 0.4, 0.4);
	sui_draw_gl(GL_LINES, COVNGraphicsData.ring, SUN_RING_SPLIT * 2, 2, color, color, color);
	glPopMatrix();
}



void co_vng_gas_planet()
{
	float color = 0, color_light = 0.9;
	float time;
	uint i;
	float hight;
	glPushMatrix();
	glRotatef(-60, 1, 0.3, 0);
	glScalef(0.4, 0.4, 0.4);
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glPopMatrix();

	time = (COVNGraphicsData.time / 300.0) - (double)((int)(COVNGraphicsData.time / 300.0));
	for(i = 0; i < GAS_PLANET_SPLITS; i++)
	{
		glPushMatrix();
		hight = ((float)i + time) * (2.0 / GAS_PLANET_SPLITS) - 1.0;
		glTranslatef(0, 0, sin(hight * 3.14 / 2.0));
		glScalef(cos(hight * 3.14 / 2.0), cos(hight * 3.14 / 2.0), 1);
		sui_draw_gl(GL_LINES, COVNGraphicsData.ring, SUN_RING_SPLIT * 2, 2, color_light, color_light, color_light);
		glPopMatrix();
	}
	glRotatef(80, 1, 0.4, 1);

	for(i = 0; i < GAS_PLANET_SPLITS; i++)
	{
		glPushMatrix();
		hight = ((float)i + time) * (2.0 / GAS_PLANET_SPLITS) - 1.0;
		glTranslatef(0, 0, sin(hight * 3.14 / 2.0));
		glScalef(cos(hight * 3.14 / 2.0), cos(hight * 3.14 / 2.0), 1);
		sui_draw_gl(GL_LINES, COVNGraphicsData.ring, SUN_RING_SPLIT * 2, 2, color_light, color_light, color_light);
		glPopMatrix();
	}
	glPopMatrix();
	glPushMatrix();
	glScalef(0.4, 0.4, 0.4);
	sui_draw_gl(GL_LINES, COVNGraphicsData.ring, SUN_RING_SPLIT * 2, 2, color, color, color);
	glPopMatrix();
}


void co_vng_black_hole()
{
	float color = 0, color_light = 0.9;
	uint i;
	float hight;
	glPushMatrix();
	glScalef(0.4, 0.4, 0.4);
	glRotatef(-60, 1, 0.3, 0);
	for(i = 0; i < BLACK_HOLE_SPLITS; i++)
	{
		glPushMatrix();
		hight = (float)i * (2.0 / BLACK_HOLE_SPLITS) - 1.0;
		glTranslatef(0, 0, hight);
		glScalef(1 / sin(hight * 3.14 / 2.0) - 1, 1 / sin(hight * 3.14 / 2.0) - 1, 1);

		sui_draw_gl(GL_LINES, COVNGraphicsData.ring, SUN_RING_SPLIT * 2, 2, color, color, color);;
		glPopMatrix();
	}

	glPopMatrix();
}

void co_vng_sun()
{
	float color = 0, color_light = 0.4;
	static uint time = 0;
	uint i, temp;
	float outbreak, length, x, y;

	time++;
	glPushMatrix();
	glScalef(0.41, 0.41, 0.41);
	sui_draw_gl(GL_POINTS, COVNGraphicsData.particles_array, SUN_PARTICLES, 2, color_light, color_light, color_light);
	glPopMatrix();
	glPushMatrix();
	glScalef(0.4, 0.4, 0.4);
	sui_draw_gl(GL_LINES, COVNGraphicsData.ring, SUN_RING_SPLIT * 2, 2, color, color, color);
	glPopMatrix();
}

float *co_vng_get_sparks(uint *length)
{
	*length = SUN_PARTICLES;
	return COVNGraphicsData.particles_array;
}

void co_vng_ring(void)
{
	float color = 0, color_light = 0.9;
	glPushMatrix();
	glScalef(0.4, 0.4, 0.4);
	sui_draw_gl(GL_LINES, COVNGraphicsData.ring, SUN_RING_SPLIT * 2, 2, color, color, color);
	glPopMatrix();
}

void co_vng_saturn_ring()
{
	float color = 0, color_light = 0.9;
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	glRotatef(COVNGraphicsData.time * 0.01, 0, 0.1, 1);
	glRotatef(COVNGraphicsData.time * 0.09, 0.1, 0.1, 1);
	sui_draw_gl(GL_QUADS, COVNGraphicsData.qubes, SATURN_RINGS * 8, 2, color_light, color_light, color_light);
	sui_draw_gl(GL_LINES, COVNGraphicsData.ring, SUN_RING_SPLIT * 2, 2, color_light, color_light, color_light);
	glPushMatrix();
	glScalef(0.9, 0.9, 0.9);
	sui_draw_gl(GL_LINES, COVNGraphicsData.ring, SUN_RING_SPLIT * 2, 2, color_light, color_light, color_light);
	glPopMatrix();
	glPushMatrix();
	glScalef(1.38, 1.38, 1.38);
	sui_draw_gl(GL_LINES, COVNGraphicsData.ring, SUN_RING_SPLIT * 2, 2, color_light, color_light, color_light);
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();
	glScalef(0.4, 0.4, 0.4);
	sui_draw_gl(GL_LINES, COVNGraphicsData.ring, SUN_RING_SPLIT * 2, 2, color, color, color);
	glPopMatrix();
}

uint co_vng_render_name(BInputState *input, char *name, float pos_x, float pos_y, float pointer_x, float pointer_y)
{
	float color = 0;
	if(input->mode == BAM_DRAW)
	{
		if((pos_x - 0.80 - pointer_x) * (pos_x - 0.80 - pointer_x) + (pos_y - pointer_y) * (pos_y - pointer_y) < 0.04 * 0.04)
		{
			sui_draw_text(-1.1, 0.21 + 0.5 * SUI_T_SIZE, SUI_T_SIZE, SUI_T_SPACE, "Delete", color, color, color);
			sui_draw_set_vec2(COVNGraphicsData.moon_line, 3, pointer_x - pos_x, 0.06);
			sui_draw_gl(GL_LINES, COVNGraphicsData.moon_line, 4, 2, color, color, color);
		}
		if((pos_x - 0.72 - pointer_x) * (pos_x - 0.72 - pointer_x) + (pos_y - pointer_y) * (pos_y - pointer_y) < 0.04 * 0.04)
		{
			sui_draw_text(-1.1, 0.21 + 0.5 * SUI_T_SIZE, SUI_T_SIZE, SUI_T_SPACE, "Hide", color, color, color);
			sui_draw_set_vec2(COVNGraphicsData.moon_line, 3, pointer_x - pos_x, 0.06);
			sui_draw_gl(GL_LINES, COVNGraphicsData.moon_line, 4, 2, color, color, color);
		}
		if((pos_x - 0.64 - pointer_x) * (pos_x - 0.64 - pointer_x) + (pos_y - pointer_y) * (pos_y - pointer_y) < 0.04 * 0.04)
		{
			sui_draw_text(-1.1, 0.21 + 0.5 * SUI_T_SIZE, SUI_T_SIZE, SUI_T_SPACE, "Hide recursevly", color, color, color);
			sui_draw_set_vec2(COVNGraphicsData.moon_line, 3, pointer_x - pos_x, 0.06);
			sui_draw_gl(GL_LINES, COVNGraphicsData.moon_line, 4, 2, color, color, color);
		}
		if((pos_x - 0.56 - pointer_x) * (pos_x - 0.56 - pointer_x) + (pos_y - pointer_y) * (pos_y - pointer_y) < 0.04 * 0.04)
		{
			sui_draw_text(-1.1, 0.21 + 0.5 * SUI_T_SIZE, SUI_T_SIZE, SUI_T_SPACE, "Clone", color, color, color);
			sui_draw_set_vec2(COVNGraphicsData.moon_line, 3, pointer_x - pos_x, 0.06);
			sui_draw_gl(GL_LINES, COVNGraphicsData.moon_line, 4, 2, color, color, color);
		}
		sui_draw_text(0.6, 0.6 + 0.5 * SUI_T_SIZE, SUI_T_SIZE, SUI_T_SPACE, name, color, color, color);
		sui_draw_gl(GL_LINES, COVNGraphicsData.name, 4, 2, color, color, color);
		glPushMatrix();
		glTranslatef(-0.8, 0, 0);
		sui_draw_gl(GL_LINES, COVNGraphicsData.moon, (MOON_SPLIT * 14 + 2), 2, color, color, color);
		glPopMatrix();
	}else
	{
		if(input->last_mouse_button[0] == TRUE && input->mouse_button[0] == FALSE)
		{	
			if((pos_x - 0.80 - pointer_x) * (pos_x - 0.80 - pointer_x) + (pos_y - pointer_y) * (pos_y - pointer_y) < 0.04 * 0.04)
				return 1;
			if((pos_x - 0.72 - pointer_x) * (pos_x - 0.72 - pointer_x) + (pos_y - pointer_y) * (pos_y - pointer_y) < 0.04 * 0.04)
				return 2;
			if((pos_x - 0.64 - pointer_x) * (pos_x - 0.64 - pointer_x) + (pos_y - pointer_y) * (pos_y - pointer_y) < 0.04 * 0.04)
				return 3;
			if((pos_x - 0.56 - pointer_x) * (pos_x - 0.56 - pointer_x) + (pos_y - pointer_y) * (pos_y - pointer_y) < 0.04 * 0.04)
				return 4;
		}
	}
	return 0;
}

boolean co_vng_render_link(BInputState *input, uint id, boolean connected, float pos_x, float pos_y, float link_x, float link_y, char *name)
{
	float x, y, r, d_x, d_y;
	if(input->mode == BAM_DRAW)
	{
		glPushMatrix();
		glRotatef(-20 * (float)id + 30, 0, 0, 1);
		glTranslatef(0.5, 0, 0);
		sui_draw_gl(GL_LINES, COVNGraphicsData.connector_one, CONNECT_SPLIT * 2, 2, 0, 0, 0);
		if(!connected)
			sui_draw_gl(GL_LINES, COVNGraphicsData.connector_two, CONNECT_SPLIT * 2, 2, 0, 0, 0);
		glPopMatrix();
		x = cos(((float)id * 2 - 3) * 3.14 * 2.0 / (360 / 10));
		y = -sin(((float)id * 2 - 3) * 3.14 * 2.0 / (360 / 10));
		if(connected)
		{
			d_x = link_x - x * 0.5;
			d_y = link_y - y * 0.5;
			r = sqrt(d_x * d_x + d_y * d_y);

			sui_draw_2d_line_gl((d_x / r) * 0.1 + x * 0.5, (d_y / r) * 0.1 + y * 0.5, link_x - ((d_x / r) * 0.45), link_y - ((d_y / r) * 0.45), 0, 0, 0);
		}
		else
		{
			sui_draw_2d_line_gl(x * 0.6, y * 0.6, x * 0.75, y * 0.75, 0, 0, 0);
			sui_draw_2d_line_gl(x * 0.75 + 0.17, y * 0.75, x * 0.75, y * 0.75, 0, 0, 0);
			sui_draw_text(x * 0.75 + 0.02, y * 0.75 + 0.5 * SUI_T_SIZE, SUI_T_SIZE, SUI_T_SPACE, name, 0, 0, 0);
		}
	}else
	{
		if((input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE) || (input->mouse_button[1] == TRUE && input->last_mouse_button[1] == FALSE))
		{
			x = cos(((float)id * 2 - 3) * 3.14 * 2.0 / (360 / 10)) * 0.5 - pos_x;
			y = -sin(((float)id * 2 - 3) * 3.14 * 2.0 / (360 / 10)) * 0.5 - pos_y;
			if(x * x + y * y < 0.06 * 0.06)				
				return TRUE;
		}
	}
	return FALSE;

}

void co_vng_divider(BInputState *input, float pos_x, float pos_y, float *rotate, float *color, float *soft_color, boolean *on, char *text)
{
	if(input->mode == BAM_DRAW)
	{
		static float array[] = {0.010000, 0.002500, 0.010000, -0.002500, 0.010000, -0.002500, -0.010000, -0.007500, -0.010000, -0.007500, -0.010000, 0.007500, -0.010000, 0.007500, 0.010000, 0.002500, -0.000000, 0.015000, -0.004635, 0.014266, -0.004635, 0.014266, -0.008817, 0.012135, -0.008817, 0.012135, -0.012135, 0.008817, -0.012135, 0.008817, -0.014266, 0.004635, -0.014266, 0.004635, -0.015000, 0.000000, -0.000000, 0.015000, 0.004635, 0.014266, 0.004635, 0.014266, 0.008817, 0.012135, 0.008817, 0.012135, 0.012135, 0.008817, 0.012135, 0.008817, 0.014266, 0.004635, 0.014266, 0.004635, 0.015000, 0.000000, -0.000000, -0.015000, 0.004635, -0.014266, 0.004635, -0.014266, 0.008817, -0.012135, 0.008817, -0.012135, 0.012135, -0.008817, 0.012135, -0.008817, 0.014266, -0.004635, 0.014266, -0.004635, 0.015000, 0.000000, -0.000000, -0.015000, -0.004635, -0.014266, -0.004635, -0.014266, -0.008817, -0.012135, -0.008817, -0.012135, -0.012135, -0.008817, -0.012135, -0.008817, -0.014266, -0.004635, -0.014266, -0.004635, -0.015000, 0.000000};
		static float line[] = {-0.03 , 0, -0.1, 0, 0.03, 0, 0.9, 0}, r = 0;
		glPushMatrix();
		glTranslatef(pos_x - 0.4, pos_y, 0);
		sui_draw_text(0.03, 0 - SUI_T_SIZE, SUI_T_SIZE, SUI_T_SPACE * 2, text, 0.4, 0.4, 0.4);
		line[4] = 0.05 + sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE * 2, text);
		sui_draw_gl(GL_LINES, line, 4, 2, 0.4, 0.4, 0.4);	
		glRotatef(*rotate * -90, 0, 0, 1);
		sui_draw_gl(GL_LINES, array, 48, 2, 0, 0, 0);
		glPopMatrix();
		if(*on)
			*rotate = *rotate * 0.8 + 0.2;
		else
			*rotate *= 0.8;

		glPushMatrix();
		glTranslatef(0, pos_y, 0);
		glRotatef((1 - *rotate) * 90, 1, 0, 0);
		glTranslatef(0, -pos_y, 0);
		if(color != NULL)
			*color = (1 - *rotate);
		if(soft_color != NULL)
			*soft_color = (1 - *rotate) / 4 + 1.0 - 1.0 / 4.0;
	}else
	{
		if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE && sui_box_click_test(-0.5 + pos_x, pos_y - SUI_T_SIZE, 1, SUI_T_SIZE + SUI_T_SIZE) && sui_box_down_click_test(-0.5 + pos_x, pos_y - SUI_T_SIZE, 1, SUI_T_SIZE + SUI_T_SIZE))
		{
			if(*on)
				*on = FALSE;
			else
				*on = TRUE;
		}
	}
}

/*
sui_set_color_array_gl(float *array, uint length, uint channels);
sui_draw_gl(uint draw_type, float *array, uint length, uint dimentions, float red, float green, float blue);

void sui_set_normal_array_gl(float *array, uint length);
void sui_set_texture2D_array_gl(float *array, uint length, uint dimentions, uint texture);
*/

