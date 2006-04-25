#include "la_includes.h"
#include "la_projection.h"
#include "la_geometry_undo.h"
#include "la_tool.h"

//#include "presuade.h"

extern boolean	la_load_targa(char *file_name, uint *texture_id);
extern uint	la_save_targa(char *file_name, float *data, unsigned int size);

#define TEX_SPLIT 8

#define BRIGHT_COUNT 12

typedef struct {
	double	age;
	double	size;
	double	pos[3];
	double	vector[3];
}DustParticle;

typedef struct {
	double	age;
	double	size;
	double	pos[3];
	double	vector[3];
}SparkParticle;

typedef struct {
	double	age;
	double	pos[3];
	double	size;
}BrightParticle;

struct{
	DustParticle	*dust;
	uint			dust_length;
	uint			dust_count;
	uint			dust_start;
	float			*dust_pos;
	float			*dust_col;
	BrightParticle	*bright;
	uint			bright_count;
	uint			bright_start;
	SparkParticle	*spark;
	uint			spark_length;
	uint			spark_count;
	uint			spark_start;
	uint			point_material;
	uint			star_material;
	uint			flare_material;
	uint			intro_material;
	uint			video_material;
	uint			soft_material;
	uint			soft_material2;
	uint			surface_material;
	float			*select_pos;
	float			*select_uv;
}GlobalParticleData;


void add_video(int size, float *data)
{
	int i, j, k, temp;
	float x, y, r, half, best;
	half = (float)size * 0.5;
	for(i = 0; i < size; i++)
	{
		for(j = 0; j < size; j++)
		{
			temp = size / 8;
			x = (temp / 2) - (i % temp);
			y = 0 - (j % (temp * 2));
			best = ((x * x) / (temp * temp)) + ((y * y) / (temp * temp));
			k = 0;
			x = temp - (i % temp);
			y = temp - (j % (temp * 2));
			r = ((x * x) / (temp * temp)) + ((y * y) / (temp * temp));
			if(r < best)
			{
				best = r;
				k = 2;
			}
			x = 0 - (i % temp);
			y = temp - (j % (temp * 2));
			r = ((x * x) / (temp * temp)) + ((y * y) / (temp * temp));
			if(r < best)
			{
				best = r;
				k = 1;
				data[(i * size + j) * 3 + ((1 + (i / temp) + 0) % 3)] += 1 - r;
			}
			x = (temp / 2) - (i % temp);
			y = (temp * 2) - (j % (temp * 2));
			r = ((x * x) / (temp * temp)) + ((y * y) / (temp * temp));
			if(r < best)
			{
				best = r;
				k = 0;
			}
			x = (i - half) / half;
			y = (j - half) / half;
			r = (x * x + y * y);
			best *= 2;
			data[(i * size + j) * 3 + ((k + (i / temp) + 0) % 3)] = 1.6 - best + r * 0.8 + 0.2;
			data[(i * size + j) * 3 + ((k + (i / temp) + 1) % 3)] = 1 - best + r * 0.8 + 0.2;
	//		data[(i * size + j) * 3 + ((k + (i / temp) + 2) % 3)] = r * 0.6 + 0.4;
			data[(i * size + j) * 3 + ((k + (i / temp) + 2) % 3)] = r * 0.4 + 0.6;
		}
	}
}

void add_star(int size, float *data)
{
	int i, j, k, temp;
	float x, y, r, half, best;
	half = (float)size * 0.5;
	for(i = 0; i < size; i++)
	{
		for(j = 0; j < size; j++)
		{
			best = 0;
			for(k = 0; k < 240; k++)
			{
				temp = k * 2;
				temp = (temp<<13) ^ temp;
				x = (1.0 - ((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
				x = (((i - half) / half) + x) * 4;
				temp = k * 2 + 1;
				temp = (temp<<13) ^ temp;
				y = (1.0 - ((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
				y = (((j - half) / half) + y) * 4;
				r = 1 - (x * x + y * y);
				if(r > best)
					best = r;
			}
			best = 0.5 + ((1 - best) * 0.8);
			x = (i - half) / half;
			y = (j - half) / half;
			r = (1 - (x * x + y * y)) * best;
			if(r < 0)
				r = 0;
			data[(i * size + j) * 3] = r * r;
			data[(i * size + j) * 3 + 1] = r * r * r;
			data[(i * size + j) * 3 + 2] = r;
		}
	}
}


void add_point2(int jump, int size, float *data, float *color)
{
	int i, j, k, l, temp;
	float x, y, r, half, best, noise;
	half = (float)size * 0.5;
	for(i = 0; i < size; i++)
	{
		for(j = 0; j < size; j++)
		{
			temp = j * 4546 + i;
			temp = (temp<<13) ^ temp;
			noise = (1.0 - ((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) * 0.005;
			for(l = 0; l < 3; l++)
			{
				best = 0;
				for(k = 0; k < 60; k++)
				{
					temp = k * 2;
					temp = (temp<<13) ^ temp;
					x = (1.0 - ((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
					x = (((i - half) / half) + x) * (8 - l);
					temp = k * 2 + 1;
					temp = (temp<<13) ^ temp;
					y = (1.0 - ((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
					y = (((j - half) / half) + y) * (8 - l);
					r = 1 - (x * x + y * y);
					if(r > best)
						best = r;
				}
				best = 1 - ((1 - best) * 0.2);
				x = (i - half) / (half - l);
				y = (j - half) / (half - l);
				r = (1 - (x * x + y * y));
				r *= 5;
				if(r > 0.3)
					r = 0.3 - (r - 0.3) * 0.04 * best;
				r *= 2;
				data[(i * jump + j) * 3 + l] = r * color[l] + noise;
			}
		}
	}
}

void add_point(int size, float *data, float *color)
{
	int i, j, k, temp;
	float x, y, r, r2, half;
	half = (float)size * 0.5;
	for(i = 0; i < size; i++)
	{
		for(j = 0; j < size; j++)
		{
			x = (i - half) / half;
			y = (j - half) / half;
			r = (1 - sqrt(sqrt(x * x + y * y)) * 0.5) - 0.5;
			r2 = sin(r * 3.14);
			data[(i * size + j) * 3 + 0] = r2 * color[0];
			data[(i * size + j) * 3 + 1] = r2 * color[1];
			data[(i * size + j) * 3 + 2] = r2 * color[2];
		}
	}
}

void add_flare(int size, float *data)
{
	int i, j, k, temp;
	float x, y, r, half, best;
	half = (float)size * 0.5;
	for(i = 0; i < size; i++)
	{
		for(j = 0; j < size; j++)
		{
			x = ((i - half) / half) * 8;
			y = ((j - half) / half) * 8;
			r = 1 / (x * x + y * y) - 0.01; 
			data[(i * size + j) * 3] = r * 0.8;
			data[(i * size + j) * 3 + 1] = r * 0.7;
			data[(i * size + j) * 3 + 2] = r * 1;
		}
	}
}



void add_intro_flare(int size, float *data)
{
	int i, j, k, temp;
	float x, y, r, half, best;
	half = (float)size * 0.5;
	for(i = 0; i < size; i++)
	{
		for(j = 0; j < size; j++)
		{
			x = ((i - half) / half) * 1.4;
			y = ((j - half) / half) * 1.4;
			r = 1 / (x * x + y * y) - 0.8; 
			if(r > 1)
				r = 1;
			data[(i * size + j) * 3] = 1 - r * 0.75;
			data[(i * size + j) * 3 + 1] = 1 - r * 0.45;
			data[(i * size + j) * 3 + 2] = 1 - r * 0.15;
		}
	}
}


float get_random(uint temp)
{
	temp = (temp<<13) ^ temp;
	return (1.0 - (float)((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

void add_shade(int size, float *data, float *pos, float *color, float color_rand, float brightness)
{
	int i, j, k, temp;
	float x, y, z, r, half, nex_color[3], nex_pos[3];
	half = (float)size * 0.5;
	for(i = 0; i < size; i++)
	{
		for(j = 0; j < size; j++)
		{
			x = ((i - half) / half);
			y = ((j - half) / half);
			z = (x * x + y * y);
			if(z < 1)
				z = sqrt(1 - z);
			else
				z = 0;
			for(k = 0; k < 50; k++)
			{
				nex_pos[0] = pos[0] + (get_random(k * 6 + 0) * 0.8);
				nex_pos[1] = pos[1] + (get_random(k * 6 + 1) * 0.8);
				nex_pos[2] = pos[2] + (get_random(k * 6 + 2) * 0.8);
				r = x * nex_pos[0] + y * nex_pos[1] + z * nex_pos[2];
				if(r > 0)
				{
					nex_color[0] = color[0] + get_random(k * 6 + 3) * color_rand;
					nex_color[1] = color[1] + get_random(k * 6 + 4) * color_rand;
					nex_color[2] = color[2] + get_random(k * 6 + 5) * color_rand;
			/*		data[(i * size + j) * 3] += (r * nex_color[0]) * (r * nex_color[0]) * brightness;
					data[(i * size + j) * 3 + 1] += (r * nex_color[1]) * (r * nex_color[1]) * brightness;
					data[(i * size + j) * 3 + 2] += (r * nex_color[2]) * (r * nex_color[2]) * brightness;*/
					data[(i * size + j) * 3] += (r * nex_color[0]) * brightness;
					data[(i * size + j) * 3 + 1] += (r * nex_color[1]) * brightness;
					data[(i * size + j) * 3 + 2] += (r * nex_color[2]) * brightness;
				}
			}
		}
	}
}


uint create_def_material(void)
{
	uint texture_id, size = 256, i;
	float *data, color[3] = {1, 1, 1};
	glEnable(GL_TEXTURE_2D);
	data = malloc((sizeof *data) * size * size * 3);
	color[0] = 2;
	color[1] = 1.6;
	color[2] = 1.2;
	add_point2(size, size, data, color);
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGB, GL_FLOAT, data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glDisable(GL_TEXTURE_2D);
	free(data);
	return texture_id;
}

#define SELECT_FLARE_SPITT 4

extern void get_randomized_color(float *color, uint32 index);

void la_pfx_init(uint particle_count, uint texture_size)
{
	float *data, color[3];
	uint i, j, temp;
	GlobalParticleData.dust = NULL;
	GlobalParticleData.dust_length = particle_count * 4;
	GlobalParticleData.dust_count = 0;
	GlobalParticleData.dust_start = 0;
	GlobalParticleData.dust_pos = 0;
	GlobalParticleData.dust_col = 0;
	GlobalParticleData.spark = NULL;
	GlobalParticleData.spark_length = particle_count / 2;
	GlobalParticleData.spark_count = 0;
	GlobalParticleData.spark_start = 0;
	GlobalParticleData.bright = malloc((sizeof *GlobalParticleData.bright) * BRIGHT_COUNT);
	for(i = 0; i < BRIGHT_COUNT; i++)
		GlobalParticleData.bright[i].age = -2.0;
	GlobalParticleData.bright_count = 0;
	GlobalParticleData.bright_start = 0;
	data = malloc((sizeof *data) * texture_size * texture_size * 3);


	if(!la_load_targa("la_tmp_star.tga", &GlobalParticleData.star_material))
	{
		add_star(texture_size / 2, data);
		GlobalParticleData.star_material = la_save_targa("la_tmp_star.tga", data, texture_size / 2);
	}
	if(!la_load_targa("la_tmp_intro.tga", &GlobalParticleData.intro_material))
	{
		add_intro_flare(texture_size / 2, data);
		GlobalParticleData.intro_material = la_save_targa("la_tmp_intro.tga", data, texture_size / 2);
	}
	if(!la_load_targa("la_tmp_flare.tga", &GlobalParticleData.flare_material))
	{
		add_flare(texture_size / 2, data);
		GlobalParticleData.flare_material = la_save_targa("la_tmp_flare.tga", data, texture_size / 2);
	}

	GlobalParticleData.select_pos = malloc((sizeof *GlobalParticleData.select_pos) * 4 * SELECT_FLARE_SPITT * SELECT_FLARE_SPITT * 3);
	GlobalParticleData.select_uv = malloc((sizeof *GlobalParticleData.select_pos) * 4 * SELECT_FLARE_SPITT * SELECT_FLARE_SPITT * 2);
	for(i = 0; i < SELECT_FLARE_SPITT * SELECT_FLARE_SPITT; i++)
	{
		sui_draw_set_vec2(GlobalParticleData.select_uv, i * 4 + 0, (float)(i % SELECT_FLARE_SPITT) / SELECT_FLARE_SPITT, (float)(i / SELECT_FLARE_SPITT) / SELECT_FLARE_SPITT);
		sui_draw_set_vec2(GlobalParticleData.select_uv, i * 4 + 1, (float)(i % SELECT_FLARE_SPITT) / SELECT_FLARE_SPITT, (float)(i / SELECT_FLARE_SPITT + 1) / SELECT_FLARE_SPITT);
		sui_draw_set_vec2(GlobalParticleData.select_uv, i * 4 + 2, (float)(i % SELECT_FLARE_SPITT + 1) / SELECT_FLARE_SPITT, (float)(i / SELECT_FLARE_SPITT + 1) / SELECT_FLARE_SPITT);
		sui_draw_set_vec2(GlobalParticleData.select_uv, i * 4 + 3, (float)(i % SELECT_FLARE_SPITT + 1) / SELECT_FLARE_SPITT, (float)(i / SELECT_FLARE_SPITT) / SELECT_FLARE_SPITT);
	}

	if(!la_load_targa("la_tmp_points.tga", &GlobalParticleData.point_material))
	{
		for(i = 0; i < SELECT_FLARE_SPITT * SELECT_FLARE_SPITT; i++)
		{
			temp = i * 4 + 0;
			temp = (temp<<13) ^ temp;
			color[0] = (((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) * 0.5;
			color[2] = color[0];
			color[1] = color[0] * color[0];
			color[0] = color[0] * color[0] * color[0];
			temp = i * 4 + 5;
			temp = (temp<<13) ^ temp;
			color[0] += (((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) * 0.1;
			temp = i * 4 + 6;
			temp = (temp<<13) ^ temp;
			color[1] += (((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) * 0.1;
			temp = i * 4 + 7;
			temp = (temp<<13) ^ temp;
			color[2] += (((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) * 0.1;
		/*	color[0] *= 0.3;
			color[1] *= 0.3;
			color[2] *= 0.3;*/
			add_point2(texture_size, texture_size / SELECT_FLARE_SPITT, &data[(((texture_size / SELECT_FLARE_SPITT) * (i % SELECT_FLARE_SPITT)) + ((texture_size * texture_size / SELECT_FLARE_SPITT) * (i / SELECT_FLARE_SPITT))) * 3], color);
		}
		GlobalParticleData.point_material = la_save_targa("la_tmp_points.tga", data, texture_size);
	}

	if(!la_load_targa("la_tmp_surface.tga", &GlobalParticleData.surface_material))	
	{
		uint texture_id;
		float light_color[3] = {0.2, 0.6, 1};
		float light_pos[3] = {-0.2, 0.2, 0} , r;
		for(i = 0; i < texture_size * texture_size * 3; i++)
			data[i] = 0;
		add_shade(texture_size / 2, data, light_pos, light_color, -0.0, 0.03);
		light_color[2] = 1;
		light_color[1] = 0.5;
		light_color[0] = 0.3;
		light_pos[0] = -0.3;
		light_pos[1] = 0.3;
		light_pos[2] = -1;
		add_shade(texture_size / 2, data, light_pos, light_color, -0.06, 0.02);

		light_color[0] = 0.2;
		light_color[1] = -0.1;
		light_color[2] = -0.1;
		light_pos[0] = 0.3;
		light_pos[1] = -0.3;
		light_pos[2] = -0.3;
		add_shade(texture_size / 2, data, light_pos, light_color, -0.06, 0.01);
		GlobalParticleData.surface_material = la_save_targa("la_tmp_surface.tga", data, texture_size / 2);
	}
	if(!la_load_targa("la_tmp_video.tga", &GlobalParticleData.video_material))
	{
		add_video(texture_size, data);
		GlobalParticleData.video_material = la_save_targa("la_tmp_video.tga", data, texture_size);
	}
	if(!la_load_targa("la_tmp_soft.tga", &GlobalParticleData.soft_material))
	{
		color[0] = 0.163;
		color[1] = 0.179;
		color[2] = 0.185;
		add_point(texture_size / 4, data, color);
		GlobalParticleData.soft_material = la_save_targa("la_tmp_soft.tga", data, texture_size / 4);
	}
	if(!la_load_targa("la_tmp_soft2.tga", &GlobalParticleData.soft_material2))	
	{
		color[0] = 0.085;
		color[1] = 0.063;
		color[2] = 0.179;
		add_point(texture_size / 4, data, color);
		GlobalParticleData.soft_material2 = la_save_targa("la_tmp_soft2.tga", data, texture_size / 4);
	}
	free(data);
}

void la_pfx_create_dust(double *pos, double size)
{
	DustParticle *p;
	if(GlobalParticleData.dust_count == 0)
	{
		GlobalParticleData.dust = malloc((sizeof *GlobalParticleData.dust) * GlobalParticleData.dust_length);
		GlobalParticleData.dust_pos = malloc((sizeof *GlobalParticleData.dust_pos) * GlobalParticleData.dust_length * 2 * 3);
		GlobalParticleData.dust_col = malloc((sizeof *GlobalParticleData.dust_col) * GlobalParticleData.dust_length * 2 * 3);
	}
	if(GlobalParticleData.dust_length > GlobalParticleData.dust_count)
		GlobalParticleData.dust_count++;
	p = &GlobalParticleData.dust[GlobalParticleData.dust_start++ % GlobalParticleData.dust_length];
	p->pos[0] = pos[0];
	p->pos[1] = pos[1];
	p->pos[2] = pos[2];
	p->vector[0] = 0;
	p->vector[1] = 0;
	p->vector[2] = 0;
	p->size = size;
	p->age = 1;
}

void la_pfx_create_spark(double *pos)
{
	SparkParticle *p;
	double camera[3];
	static uint counter;
	uint temp;
	if(GlobalParticleData.spark_count == 0)
	{
		GlobalParticleData.spark = malloc((sizeof *GlobalParticleData.spark) * GlobalParticleData.spark_length);
	}
	if(GlobalParticleData.spark_length > GlobalParticleData.spark_count)
		GlobalParticleData.spark_count++;
	p = &GlobalParticleData.spark[GlobalParticleData.spark_start++ % GlobalParticleData.spark_length];
	p->pos[0] = pos[0];
	p->pos[1] = pos[1];
	p->pos[2] = pos[2];
	p_get_view_camera(camera);
	p->size = 0.02 * sqrt((camera[0] - pos[0]) * (camera[0] - pos[0]) + (camera[1] - pos[1]) * (camera[1] - pos[1]) + (camera[2] - pos[2]) * (camera[2] - pos[2]));
	counter++;
	temp = (counter<<13) ^ counter;
	p->vector[0] = (1.0 - ((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) * p->size;
	counter++;
	temp = (counter<<13) ^ counter;
	p->vector[1] = (1.0 - ((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) * p->size;
	counter++;
	temp = (counter<<13) ^ counter;
	p->vector[2] = (1.0 - ((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) * p->size;
	p->age = 1;
}

void la_pfx_create_intro_spark(double *pos)
{
	SparkParticle *p;
	double camera[3];
	static uint counter;
	uint temp;
	if(GlobalParticleData.spark_count == 0)
	{
		GlobalParticleData.spark = malloc((sizeof *GlobalParticleData.spark) * GlobalParticleData.spark_length);
	}
	if(GlobalParticleData.spark_length > GlobalParticleData.spark_count)
		GlobalParticleData.spark_count++;
	p = &GlobalParticleData.spark[GlobalParticleData.spark_start++ % GlobalParticleData.spark_length];
	p->pos[0] = pos[0];
	p->pos[1] = pos[1];
	p->pos[2] = pos[2];
	p_get_view_camera(camera);
//	p->size = 0.02 * sqrt((camera[0] - pos[0]) * (camera[0] - pos[0]) + (camera[1] - pos[1]) * (camera[1] - pos[1]) + (camera[2] - pos[2]) * (camera[2] - pos[2]));
	p->size = 0.006;
	counter++;
	temp = (counter<<13) ^ counter;
	p->vector[0] = (1.0 - ((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) * p->size;
	counter++;
	temp = (counter<<13) ^ counter;
	p->vector[1] = (1.0 - ((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) * p->size;
	counter++;
	temp = (counter<<13) ^ counter;
	p->vector[2] = (1.0 - ((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) * p->size;
	p->age = 1;
	p->size = 0.06;
}


void la_pfx_create_bright(double *pos)
{
	BrightParticle *p;
	double camera[3];
	if(BRIGHT_COUNT > GlobalParticleData.bright_count)
		GlobalParticleData.bright_count++;
	p = &GlobalParticleData.bright[GlobalParticleData.bright_start++ % BRIGHT_COUNT];
	p_get_view_camera(camera);
	p->size = sqrt((camera[0] - pos[0]) * (camera[0] - pos[0]) + (camera[1] - pos[1]) * (camera[1] - pos[1]) + (camera[2] - pos[2]) * (camera[2] - pos[2]));
	p->pos[0] = pos[0];
	p->pos[1] = pos[1];
	p->pos[2] = pos[2];
	p->age = 1;
	
}

void la_pfx_create_dust_line(double *pos_a, double *pos_b)
{
	double pos[3], size, step, count, i;
	count = sqrt((pos_a[0] - pos_b[0]) * (pos_a[0] - pos_b[0]) + (pos_a[1] - pos_b[1]) * (pos_a[1] - pos_b[1]) + (pos_a[2] - pos_b[2]) * (pos_a[2] - pos_b[2]));
	p_get_view_camera(pos);
	pos[0] -= (pos_a[0] + pos_b[0]) * 0.5;
	pos[1] -= (pos_a[1] + pos_b[1]) * 0.5;
	pos[2] -= (pos_a[2] + pos_b[2]) * 0.5;
	size = sqrt(pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2]);
	step = size / (count * GlobalParticleData.dust_length * 0.2);
	if(step  < 0.01)
			step  = 0.01;
	for(i = 0.1; i < 0.9; i += step)
	{
		pos[0] = pos_a[0] * i + pos_b[0] * (1.0 - i);
		pos[1] = pos_a[1] * i + pos_b[1] * (1.0 - i);
		pos[2] = pos_a[2] * i + pos_b[2] * (1.0 - i);
		la_pfx_create_dust(pos, size);
	}
}

void la_pfx_create_dust_selected_vertexes(double *mid)
{
	double pos[3], size;
	uint32 vertex_count, i, j;
	double *vertex;
	DustParticle *p;

	udg_get_geometry(&vertex_count, NULL, &vertex, NULL, NULL);
	p_get_view_camera(pos);
	size = sqrt((mid[0] - pos[0]) * (mid[0] - pos[0]) + (mid[1] - pos[1]) * (mid[1] - pos[1]) + (mid[2] - pos[2]) * (mid[2] - pos[2]));
	for(i = 0; i < vertex_count; i++)
	{
		if(vertex[i * 3] != E_REAL_MAX && udg_get_select(i) > 0.1)
		{
			udg_get_vertex_pos(pos, i);
			for(j = 0; j < 10; j++)
			{
				la_pfx_create_dust(pos, size);
				p = &GlobalParticleData.dust[(GlobalParticleData.dust_start - 1) % GlobalParticleData.dust_length];
				p->vector[0] = (pos[0] - mid[0]) * 0.02;
				p->vector[1] = (pos[1] - mid[1]) * 0.02;
				p->vector[2] = (pos[2] - mid[2]) * 0.02;
			}
		}
	}
}

void la_pfx_draw(boolean intro)
{
	double matrix[16], size;
	float vertex[12], uv[8] = {0, 0, 1, 0, 1, 1, 0, 1};
	uint i = 0;
	p_get_model_matrix(matrix);
	if(GlobalParticleData.dust_count > 0)
	{
		DustParticle *p;
		for(i = 0; i < GlobalParticleData.dust_length; i++)
		{
			p = &GlobalParticleData.dust[i];
			if(p->age > -1.0)
			{
				static uint32 counter = 0; 
				uint32 temp;
				sui_draw_set_vec3(GlobalParticleData.dust_pos, i * 2 + 1, p->pos[0], p->pos[1], p->pos[2]);
				size = p->size * 0.03 * (1 - p->age) * (1 - p->age);
				counter++;
				temp = (counter<<13) ^ counter;
				p->vector[0] += (1.0 - ((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) * size;
				counter++;
				temp = (counter<<13) ^ counter;
				p->vector[1] += (1.0 - ((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) * size;
				p->vector[1] -= p->size * 0.0004;
				counter++;
				temp = (counter<<13) ^ counter;
				p->vector[2] += (1.0 - ((temp * (temp * temp * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) * size;
				p->pos[0] += p->vector[0] * betray_get_delta_time() * 100;
				p->pos[1] += p->vector[1] * betray_get_delta_time() * 100;
				p->pos[2] += p->vector[2] * betray_get_delta_time() * 100;
				p->age -= betray_get_delta_time();
				sui_draw_set_vec3(GlobalParticleData.dust_pos, i * 2, p->pos[0], p->pos[1], p->pos[2]);
				sui_draw_set_vec3(GlobalParticleData.dust_col, i * 2, p->age * 1.2, p->age * 1.1, p->age * 1);
				if(p->age < 0)
				{
					p->age = -2.0;
					GlobalParticleData.dust_count--;
				}
			}
		}
		if(GlobalParticleData.dust_count == 0)
		{
			free(GlobalParticleData.dust);
			free(GlobalParticleData.dust_pos);
			free(GlobalParticleData.dust_col);
		}else
		{
			glDepthMask(0);
			sui_set_blend_gl(GL_ONE, GL_ONE);
			sui_set_color_array_gl(GlobalParticleData.dust_col, GlobalParticleData.dust_length * 2, 3);
			sui_draw_gl(GL_LINES, GlobalParticleData.dust_pos, GlobalParticleData.dust_length * 2, 3, 0, 0, 0);
			glDepthMask(1);
		}
	}

	if(GlobalParticleData.spark_count > 0)
	{
		SparkParticle *p;
		double matrix[16];
		p_get_model_matrix(matrix);
		for(i = 0; i < GlobalParticleData.spark_length; i++)
		{
			p = &GlobalParticleData.spark[i];
			if(p->age > -1.0)
			{
				p->vector[0] *= 0.98;
				p->vector[1] *= 0.98;
				p->vector[2] *= 0.98;
				p->pos[0] += p->vector[0];
				p->pos[1] += p->vector[1]/* - 0.025 * p->size*/;
				p->pos[2] += p->vector[2];
				p->age -= 0.005;
				if(intro)
					size = p->size * p->age * 10;
				else
					size = p->size * p->age;
				sui_draw_set_vec3(vertex, 0, p->pos[0] - matrix[0] * size + matrix[1] * size, p->pos[1] - matrix[4] * size + matrix[5] * size, p->pos[2] - matrix[8] * size + matrix[9] * size);
				sui_draw_set_vec3(vertex, 1, p->pos[0] - matrix[0] * size - matrix[1] * size, p->pos[1] - matrix[4] * size - matrix[5] * size, p->pos[2] - matrix[8] * size - matrix[9] * size);
				sui_draw_set_vec3(vertex, 2, p->pos[0] + matrix[0] * size - matrix[1] * size, p->pos[1] + matrix[4] * size - matrix[5] * size, p->pos[2] + matrix[8] * size - matrix[9] * size);
				sui_draw_set_vec3(vertex, 3, p->pos[0] + matrix[0] * size + matrix[1] * size, p->pos[1] + matrix[4] * size + matrix[5] * size, p->pos[2] + matrix[8] * size + matrix[9] * size);
				glDepthMask(0);
				if(intro)
				{
					sui_set_blend_gl(GL_ZERO, GL_SRC_COLOR);
					sui_set_texture2D_array_gl(uv, 4, 2, GlobalParticleData.intro_material);
				}else
				{
					sui_set_blend_gl(GL_ONE, GL_ONE);
					sui_set_texture2D_array_gl(uv, 4, 2, GlobalParticleData.flare_material /**/);
				}
				sui_draw_gl(GL_QUADS, vertex, 4, 3, 1, 1, 1);
				glDepthMask(1);
				if(p->age < 0)
				{
					p->age = -2.0;
					GlobalParticleData.spark_count--;
				}
			}
		}
		if(GlobalParticleData.spark_count == 0)
		{
			free(GlobalParticleData.spark);
		}
	}
	if(GlobalParticleData.bright_count > 0)
	{
		BrightParticle *p;
		double matrix[16], size, size2;
		p_get_model_matrix(matrix);
		glDepthMask(0);
		glDisable(GL_DEPTH_TEST);
		for(i = 0; i < BRIGHT_COUNT; i++)
		{
			p = &GlobalParticleData.bright[i];
			if(p->age > -1.0)
			{
				p->age -= betray_get_delta_time() * 2;
				if(p->age < 0)
				{
				//	glDisable(GL_LIGHT2 + i);
					p->age = -2.0;
					GlobalParticleData.bright_count--;
					break;
				}
				size = p->size * 1.6 * p->age;
				sui_draw_set_vec3(vertex, 0, p->pos[0] - matrix[0] * size + matrix[1] * size, p->pos[1] - matrix[4] * size + matrix[5] * size, p->pos[2] - matrix[8] * size + matrix[9] * size);
				sui_draw_set_vec3(vertex, 1, p->pos[0] - matrix[0] * size - matrix[1] * size, p->pos[1] - matrix[4] * size - matrix[5] * size, p->pos[2] - matrix[8] * size - matrix[9] * size);
				sui_draw_set_vec3(vertex, 2, p->pos[0] + matrix[0] * size - matrix[1] * size, p->pos[1] + matrix[4] * size - matrix[5] * size, p->pos[2] + matrix[8] * size - matrix[9] * size);
				sui_draw_set_vec3(vertex, 3, p->pos[0] + matrix[0] * size + matrix[1] * size, p->pos[1] + matrix[4] * size + matrix[5] * size, p->pos[2] + matrix[8] * size + matrix[9] * size);
				sui_set_blend_gl(GL_ONE, GL_ONE);
				sui_set_texture2D_array_gl(uv, 4, 2, GlobalParticleData.flare_material);
				sui_draw_gl(GL_QUADS, vertex, 4, 3, 1, 1, 1);

				size = p->size * 3 * p->age;
				size2 = p->size * 0.1 * p->age * p->age * p->age;
				sui_draw_set_vec3(vertex, 0, p->pos[0] - matrix[0] * size + matrix[1] * size2, p->pos[1] - matrix[4] * size + matrix[5] * size2, p->pos[2] - matrix[8] * size + matrix[9] * size2);
				sui_draw_set_vec3(vertex, 1, p->pos[0] - matrix[0] * size - matrix[1] * size2, p->pos[1] - matrix[4] * size - matrix[5] * size2, p->pos[2] - matrix[8] * size - matrix[9] * size2);
				sui_draw_set_vec3(vertex, 2, p->pos[0] + matrix[0] * size - matrix[1] * size2, p->pos[1] + matrix[4] * size - matrix[5] * size2, p->pos[2] + matrix[8] * size - matrix[9] * size2);
				sui_draw_set_vec3(vertex, 3, p->pos[0] + matrix[0] * size + matrix[1] * size2, p->pos[1] + matrix[4] * size + matrix[5] * size2, p->pos[2] + matrix[8] * size + matrix[9] * size2);
				sui_set_blend_gl(GL_ONE, GL_ONE);
				sui_set_texture2D_array_gl(uv, 4, 2, GlobalParticleData.star_material);
				sui_draw_gl(GL_QUADS, vertex, 4, 3, 1, 1, 1);
				{
					float light[4];
					light[0] = p->pos[0];
					light[1] = p->pos[1];
					light[2] = p->pos[2];
					light[3] = 1;
					glLightfv(GL_LIGHT2 + i, GL_POSITION, light);
					light[0] = p->age * 5 * p->size;
					light[1] = p->age * 5 * p->size;
					light[2] = p->age * 5 * p->size;
					light[3] = 1;
					glLightfv(GL_LIGHT2 + i, GL_DIFFUSE, light);
				}
			}
		}
		glEnable(GL_DEPTH_TEST);
		glDepthMask(1);
	}
}

#define SELECT_FLARE_SIZE 0.015
#define MANIPULATOR_FLARE_SIZE 1
#define SELECT_OTHER_FLARE_SIZE 0.2

void la_pfx_select_vertex(void)
{
	static double t = 0;
	egreal pos[3], output[3], size, dist;
	float v[12], uv[8] = {0, 0, 1, 0, 1, 1, 0, 1};
	uint32 vertex_count, i, j = 0;
	egreal *vertex;
	udg_get_geometry(&vertex_count, NULL, &vertex, NULL, NULL);
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();
	glDisable(GL_DEPTH_TEST);
	dist = p_get_distance_camera();
//	dist *= 2;
	for(i = 0; i < vertex_count; i++)
	{
		if(vertex[i * 3] != E_REAL_MAX && udg_get_select(i) > 0.1)
		{
			udg_get_vertex_pos(pos, i);
			p_get_projection_screen(output, pos[0], pos[1], pos[2]);
			size = ((-output[2] - dist) / dist) * SELECT_FLARE_SIZE + 0.01;
			if(output[2] < 0)
			{	
				sui_draw_set_vec3(GlobalParticleData.select_pos, j * 4 + 0, -output[0] - size, -output[1] + size, -1);
				sui_draw_set_vec3(GlobalParticleData.select_pos, j * 4 + 1, -output[0] + size, -output[1] + size, -1);
				sui_draw_set_vec3(GlobalParticleData.select_pos, j * 4 + 2, -output[0] + size, -output[1] - size, -1);
				sui_draw_set_vec3(GlobalParticleData.select_pos, j * 4 + 3, -output[0] - size, -output[1] - size, -1);
			}else
			{	
				sui_draw_set_vec3(GlobalParticleData.select_pos, j * 4 + 0, 0, 0, -1);
				sui_draw_set_vec3(GlobalParticleData.select_pos, j * 4 + 1, 0, 0, -1);
				sui_draw_set_vec3(GlobalParticleData.select_pos, j * 4 + 2, 0, 0, -1);
				sui_draw_set_vec3(GlobalParticleData.select_pos, j * 4 + 3, 0, 0, -1);
			}
			if(++j == SELECT_FLARE_SPITT * SELECT_FLARE_SPITT)
			{
				sui_set_blend_gl(GL_ONE, GL_ONE);
				sui_set_texture2D_array_gl(GlobalParticleData.select_uv, SELECT_FLARE_SPITT * SELECT_FLARE_SPITT * 4, 2, GlobalParticleData.point_material);
				sui_draw_gl(GL_QUADS, GlobalParticleData.select_pos, SELECT_FLARE_SPITT * SELECT_FLARE_SPITT * 4, 3, 0.3, 0.3, 0.3);	
				j = 0;
			}
		}
	}
	if(j != 0)
	{
		for(j *= 4; j < SELECT_FLARE_SPITT * SELECT_FLARE_SPITT * 4; j++)
			sui_draw_set_vec3(GlobalParticleData.select_pos, j, 0, 0, -1);
		sui_set_blend_gl(GL_ONE, GL_ONE);
		sui_set_texture2D_array_gl(GlobalParticleData.select_uv, SELECT_FLARE_SPITT * SELECT_FLARE_SPITT * 4, 2, GlobalParticleData.point_material);
		sui_draw_gl(GL_QUADS, GlobalParticleData.select_pos, SELECT_FLARE_SPITT * SELECT_FLARE_SPITT * 4, 3, 0.3, 0.3, 0.3);	
	}
	if(sui_get_setting_int("DISPLAY_SILLY_FLARES", TRUE))
	{
		j = 0; 
		/*for(i = 0; i < vertex_count; i++)
		{
			if(vertex[i].x < 1.1 && udg_get_select(i) > 0.1)
			{
				udg_get_vertex_pos(pos, i);
				p_get_projection_screen(output, pos[0], pos[1], pos[2]);
				if(output[2] < 0)
				{	
					nglSetVertex3d(GlobalParticleData.select_pos, j * 4 + 0, 8 * output[0] + SELECT_OTHER_FLARE_SIZE, 8 * output[1] + SELECT_OTHER_FLARE_SIZE, -1);
					nglSetVertex3d(GlobalParticleData.select_pos, j * 4 + 1, 8 * output[0] - SELECT_OTHER_FLARE_SIZE, 8 * output[1] + SELECT_OTHER_FLARE_SIZE, -1);
					nglSetVertex3d(GlobalParticleData.select_pos, j * 4 + 2, 8 * output[0] - SELECT_OTHER_FLARE_SIZE, 8 * output[1] - SELECT_OTHER_FLARE_SIZE, -1);
					nglSetVertex3d(GlobalParticleData.select_pos, j * 4 + 3, 8 * output[0] + SELECT_OTHER_FLARE_SIZE, 8 * output[1] - SELECT_OTHER_FLARE_SIZE, -1);
				}else
				{	
					nglSetVertex3d(GlobalParticleData.select_pos, j * 4 + 0, 0, 0, -1);
					nglSetVertex3d(GlobalParticleData.select_pos, j * 4 + 1, 0, 0, -1);
					nglSetVertex3d(GlobalParticleData.select_pos, j * 4 + 2, 0, 0, -1);
					nglSetVertex3d(GlobalParticleData.select_pos, j * 4 + 3, 0, 0, -1);
				}
				if(++j == SELECT_FLARE_SPITT * SELECT_FLARE_SPITT)
				{
					nglDrawArray(NGL_QUADS_FILLED, GlobalParticleData.select_pos, &GlobalParticleData.select_uv, 1, GlobalParticleData.point_material, 0);
					j = 0;
				}
			}
		}
		if(j != SELECT_FLARE_SPITT * SELECT_FLARE_SPITT)
		{
			for(j *= 4; j < SELECT_FLARE_SPITT * SELECT_FLARE_SPITT * 4; j++)
				nglSetVertex3d(GlobalParticleData.select_pos, j, 0, 0, 0);
			nglDrawArray(NGL_QUADS_FILLED, GlobalParticleData.select_pos, &GlobalParticleData.select_uv, 1, GlobalParticleData.point_material, 0);				
		}*/
		la_t_tm_get_pos(pos);
		p_get_projection_screen(output, pos[0], pos[1], pos[2]);
		t += 0.002;
		output[0] += 0.1 * sin(t); 
		output[1] += 0.1 * cos(t);
		sui_draw_set_vec3(v, 0, 2 * output[0] + MANIPULATOR_FLARE_SIZE, 2 * output[1] + MANIPULATOR_FLARE_SIZE, -1);
		sui_draw_set_vec3(v, 1, 2 * output[0] - MANIPULATOR_FLARE_SIZE, 2 * output[1] + MANIPULATOR_FLARE_SIZE, -1);
		sui_draw_set_vec3(v, 2, 2 * output[0] - MANIPULATOR_FLARE_SIZE, 2 * output[1] - MANIPULATOR_FLARE_SIZE, -1);
		sui_draw_set_vec3(v, 3, 2 * output[0] + MANIPULATOR_FLARE_SIZE, 2 * output[1] - MANIPULATOR_FLARE_SIZE, -1);
		sui_set_blend_gl(GL_ONE, GL_ONE);
		sui_set_texture2D_array_gl(uv, 4, 2, GlobalParticleData.soft_material2);
		sui_draw_gl(GL_QUADS, v, 4, 3, 1, 1, 1);

		sui_draw_set_vec3(v, 0, 4 * output[0] + MANIPULATOR_FLARE_SIZE * 0.5, 4 * output[1] + MANIPULATOR_FLARE_SIZE * 0.5, -1);
		sui_draw_set_vec3(v, 1, 4 * output[0] - MANIPULATOR_FLARE_SIZE * 0.5, 4 * output[1] + MANIPULATOR_FLARE_SIZE * 0.5, -1);
		sui_draw_set_vec3(v, 2, 4 * output[0] - MANIPULATOR_FLARE_SIZE * 0.5, 4 * output[1] - MANIPULATOR_FLARE_SIZE * 0.5, -1);
		sui_draw_set_vec3(v, 3, 4 * output[0] + MANIPULATOR_FLARE_SIZE * 0.5, 4 * output[1] - MANIPULATOR_FLARE_SIZE * 0.5, -1);
		sui_set_blend_gl(GL_ONE, GL_ONE);
		sui_set_texture2D_array_gl(uv, 4, 2, GlobalParticleData.soft_material);
		sui_draw_gl(GL_QUADS, v, 4, 3, 1, 1, 1);

		sui_draw_set_vec3(v, 0, -1.5 * output[0] + MANIPULATOR_FLARE_SIZE * 0.15, -1.5 * output[1] + MANIPULATOR_FLARE_SIZE * 0.15, -1);
		sui_draw_set_vec3(v, 1, -1.5 * output[0] - MANIPULATOR_FLARE_SIZE * 0.15, -1.5 * output[1] + MANIPULATOR_FLARE_SIZE * 0.15, -1);
		sui_draw_set_vec3(v, 2, -1.5 * output[0] - MANIPULATOR_FLARE_SIZE * 0.15, -1.5 * output[1] - MANIPULATOR_FLARE_SIZE * 0.15, -1);
		sui_draw_set_vec3(v, 3, -1.5 * output[0] + MANIPULATOR_FLARE_SIZE * 0.15, -1.5 * output[1] - MANIPULATOR_FLARE_SIZE * 0.15, -1);
		sui_set_blend_gl(GL_ONE, GL_ONE);
		sui_set_texture2D_array_gl(uv, 4, 2, GlobalParticleData.soft_material2);
		sui_draw_gl(GL_QUADS, v, 4, 3, 1, 1, 1);

		sui_draw_set_vec3(v, 0, -1.8 * output[0] - 0.4 * output[1] + MANIPULATOR_FLARE_SIZE * 0.5, -1.8 * output[1] + 0.4 * output[0] + MANIPULATOR_FLARE_SIZE * 0.5, -1);
		sui_draw_set_vec3(v, 1, -1.8 * output[0] - 0.4 * output[1] - MANIPULATOR_FLARE_SIZE * 0.5, -1.8 * output[1] + 0.4 * output[0] + MANIPULATOR_FLARE_SIZE * 0.5, -1);
		sui_draw_set_vec3(v, 2, -1.8 * output[0] - 0.4 * output[1] - MANIPULATOR_FLARE_SIZE * 0.5, -1.8 * output[1] + 0.4 * output[0] - MANIPULATOR_FLARE_SIZE * 0.5, -1);
		sui_draw_set_vec3(v, 3, -1.8 * output[0] - 0.4 * output[1] + MANIPULATOR_FLARE_SIZE * 0.5, -1.8 * output[1] + 0.4 * output[0] - MANIPULATOR_FLARE_SIZE * 0.5, -1);
		sui_set_blend_gl(GL_ONE, GL_ONE);
		sui_set_texture2D_array_gl(uv, 4, 2, GlobalParticleData.soft_material);
		sui_draw_gl(GL_QUADS, v, 4, 3, 1, 1, 1);

		sui_draw_set_vec3(v, 0, -1.8 * output[0] + 0.4 * output[1] + MANIPULATOR_FLARE_SIZE * 0.5, -1.8 * output[1] - 0.4 * output[0] + MANIPULATOR_FLARE_SIZE * 0.5, -1);
		sui_draw_set_vec3(v, 1, -1.8 * output[0] + 0.4 * output[1] - MANIPULATOR_FLARE_SIZE * 0.5, -1.8 * output[1] - 0.4 * output[0] + MANIPULATOR_FLARE_SIZE * 0.5, -1);
		sui_draw_set_vec3(v, 2, -1.8 * output[0] + 0.4 * output[1] - MANIPULATOR_FLARE_SIZE * 0.5, -1.8 * output[1] - 0.4 * output[0] - MANIPULATOR_FLARE_SIZE * 0.5, -1);
		sui_draw_set_vec3(v, 3, -1.8 * output[0] + 0.4 * output[1] + MANIPULATOR_FLARE_SIZE * 0.5, -1.8 * output[1] - 0.4 * output[0] - MANIPULATOR_FLARE_SIZE * 0.5, -1);
		sui_set_blend_gl(GL_ONE, GL_ONE);
		sui_set_texture2D_array_gl(uv, 4, 2, GlobalParticleData.soft_material);
		sui_draw_gl(GL_QUADS, v, 4, 3, 1, 1, 1);
		
		
	}

	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	glPushMatrix();
	p_view_set();
	glPushMatrix();
}

void la_pfx_draw_intro(void)
{
	uint i;
	float size, pos[12], uv[8] = {0, 0, 1, 0, 1, 1, 0, 1};
	if(GlobalParticleData.spark_count > 0)
	{
		SparkParticle *p;
		double matrix[16];
		p_projection_update();
		p_get_model_matrix(matrix);
		for(i = 0; i < GlobalParticleData.spark_length; i++)
		{
			p = &GlobalParticleData.spark[i];
			if(p->age > -1.0)
			{
				p->vector[0] *= 0.98;
				p->vector[1] *= 0.98;
				p->vector[2] *= 0.98;
				p->pos[0] += 0.1 * p->vector[0];
				p->pos[1] += 0.1 * p->vector[1] + 0.025 * p->size;
				p->pos[2] += 0.1 * p->vector[2];
				p->age -= 0.005;
				size = p->size * p->age;
				sui_draw_set_vec3(pos, 0, p->pos[0] - matrix[0] * size + matrix[1] * size, p->pos[1] - matrix[4] * size + matrix[5] * size, p->pos[2] - matrix[8] * size + matrix[9] * size);
				sui_draw_set_vec3(pos, 1, p->pos[0] - matrix[0] * size - matrix[1] * size, p->pos[1] - matrix[4] * size - matrix[5] * size, p->pos[2] - matrix[8] * size - matrix[9] * size);
				sui_draw_set_vec3(pos, 2, p->pos[0] + matrix[0] * size - matrix[1] * size, p->pos[1] + matrix[4] * size - matrix[5] * size, p->pos[2] + matrix[8] * size - matrix[9] * size);
				sui_draw_set_vec3(pos, 3, p->pos[0] + matrix[0] * size + matrix[1] * size, p->pos[1] + matrix[4] * size + matrix[5] * size, p->pos[2] + matrix[8] * size + matrix[9] * size);
				glDepthMask(0);
				sui_set_blend_gl(GL_ZERO, GL_SRC_COLOR);
				sui_set_texture2D_array_gl(uv, 4, 2, GlobalParticleData.video_material);
				sui_draw_gl(GL_QUADS, pos, 4, 3, 1, 1, 1);
				glDepthMask(1);
				if(p->age < 0)
				{
					p->age = -2.0;
					GlobalParticleData.spark_count--;
				}
			}
		}
		if(GlobalParticleData.spark_count == 0)
		{
			free(GlobalParticleData.spark);
		}
	}
	glEnable(GL_DEPTH_TEST);
}

void la_pfx_video_flare(void)
{
	float pos[8] = {0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5}, uv[8] = {0, 0, 1, 0, 1, 1, 0, 1};
	glDepthMask(0);
	glDisable(GL_DEPTH_TEST);
	sui_set_blend_gl(GL_ZERO, GL_SRC_COLOR);
	sui_set_texture2D_array_gl(uv, 4, 2, GlobalParticleData.video_material);
	sui_draw_gl(GL_QUADS, pos, 4, 2, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(1);
}

uint la_pfx_surface_material(void)
{
	return GlobalParticleData.surface_material;
}
