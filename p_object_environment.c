
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

#define TRUE 1
#define FALSE 0
#include "st_types.h"

      /* EXT_texture_cube_map */ 
#define GL_EXT_texture_cube_map             1 
#define GL_NORMAL_MAP_EXT                   0x8511 
#define GL_REFLECTION_MAP_EXT               0x8512 
#define GL_TEXTURE_CUBE_MAP_EXT             0x8513 
#define GL_TEXTURE_BINDING_CUBE_MAP_EXT     0x8514 
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT  0x8515 
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT  0x8516 
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT  0x8517 
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT  0x8518 
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT  0x8519 
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT  0x851A 
#define GL_PROXY_TEXTURE_CUBE_MAP_EXT       0x851B 
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE_EXT    0x851C 
#define GL_CLAMP_TO_EDGE					0x812F
        

GLuint texture_id, texture_id2, sphere_map;
#define TEX_SIZE 128 

uint p_env_cube_threw_qube(float *vector_cube, float *input, float *output, unsigned int size, unsigned int pos)
{
	unsigned int i, j;
	float x, y, z, r, g, b, a;
	for(i = pos; i < size * size * 6 && i < pos + 2; i++)
	{
		x = vector_cube[i * 3];
		y = vector_cube[i * 3 + 1];
		z = vector_cube[i * 3 + 2];
		r = input[i * 3] / (size * size * 3);
		g = input[i * 3 + 1] / (size * size * 3); 
		b = input[i * 3 + 2] / (size * size * 3);
		if(r + g + b > 0)
		{
			for(j = 0; j < size * size * 6; j++)
			{
				a = x * vector_cube[j * 3] + y * vector_cube[j * 3 + 1] + z * vector_cube[j * 3 + 2];
				if(a > 0)
				{
					output[j * 3] += r * a;
					output[j * 3 + 1] += g * a;
					output[j * 3 + 2] += b * a;
				}
			}
		}
	}
	return i;
}


void p_env_create_vector_cube(float *data, unsigned int size)
{
	unsigned int i, j;
	float half_size, x, y, z, r;
	half_size = (float)(size * 0.5) - 0.5;

	for(i = 0; i < size; i++)
	{
		for(j = 0; j < size; j++)
		{
			x = half_size;
			y = half_size - (float)i;
			z = half_size - (float)j;
			r = sqrt(x * x + y * y + z * z);
			*data++ = x / r;
			*data++ = y / r;
			*data++ = z / r;
		}
	}
	for(i = 0; i < size; i++)
	{
		for(j = 0; j < size; j++)
		{
			x = (float)j - half_size;
			y = half_size;
			z = (float)i - half_size;
			r = sqrt(x * x + y * y + z * z);
			*data++ = x / r;
			*data++ = y / r;
			*data++ = z / r;			
		}
	}
	for(i = 0; i < size; i++)
	{
		for(j = 0; j < size; j++)
		{
			x = (float)j - half_size;
			y = half_size - (float)i;
			z = half_size;		
			r = sqrt(x * x + y * y + z * z);
			*data++ = x / r;
			*data++ = y / r;
			*data++ = z / r;
		}
	}
	for(i = 0; i < size; i++)
	{
		for(j = 0; j < size; j++)
		{
			x = -half_size;
			y = half_size - (float)i;
			z = (float)j - half_size;		
			r = sqrt(x * x + y * y + z * z);
			*data++ = x / r;
			*data++ = y / r;
			*data++ = z / r;
		}
	} 
	for(i = 0; i < size; i++)
	{
		for(j = 0; j < size; j++)
		{
			x = (float)j - half_size;
			y = -half_size;
			z = half_size - (float)i;		
			r = sqrt(x * x + y * y + z * z);
			*data++ = x / r;
			*data++ = y / r;
			*data++ = z / r;
		}
	}
	for(i = 0; i < size; i++)
	{
		for(j = 0; j < size; j++)
		{
			x = half_size - (float)j;
			y = half_size - (float)i;
			z = -half_size;		
			r = sqrt(x * x + y * y + z * z);
			*data++ = x / r;
			*data++ = y / r;
			*data++ = z / r;
		}
	}	
}

unsigned int p_env_find_pixel(float x, float y, float z,unsigned int size)
{
	unsigned int i, j, found;
	float r, half_size;

	half_size = 0.5 * (float)size;

	r = x;
	found = 0;
	if(y > r)
	{
		r = y;
		found = 1;
	}
	if(z > r)
	{
		r = z;
		found = 2;
	}
	if(-x > r)
	{
		r = -x;
		found = 3;
	}
	if(-y > r)
	{
		r = -y;
		found = 4;
	}
	if(-z > r)
	{
		r = -z;
		found = 5;
	}
	switch(found)
	{
		case 0 :
		i = size - ((y / x) + 1.0) * half_size;
		j = size - ((z / x) + 1.0) * half_size;
		return i * size + j;
		break;
		case 1 :
		i = ((x / y) + 1.0) * half_size;
		j = ((z / y) + 1.0) * half_size;
		return i * size + j + size * size;			
		break;
		case 2 :
		i = size - ((y / z) + 1.0) * half_size;
		j = ((x / z) + 1.0) * half_size;
		return i * size + j + size * size * 2;
		break;
		case 3 :
		i = size - ((y / -x) + 1.0) * half_size;
		j = ((z / -x) + 1.0) * half_size;
		return i * size + j + size * size * 3;
		break;
		case 4 :
		i = size - ((z / -y) + 1.0) * half_size;
		j = ((x / -y) + 1.0) * half_size;
		return i * size + j + size * size * 4;
		break;
		case 5 :
		i = size - ((y / -z) + 1.0) * half_size;
		j = size - ((x / -z) + 1.0) * half_size;
		return i * size + j + size * size * 5;
		break;
	}	
	return 0;
}

void p_env_create_sky(float *vector, float *data, float light, unsigned int size)
{
	int i;
	float temp;
	for(i = 0; i < size * size * 6; i++)
	{
		temp = vector[i * 3 + 1];
		if(vector[i * 3 + 1] > 0)
		{
			temp = (1 - temp) * 1.2; 
			data[i * 3] = light * (temp * temp * temp + 0.1);
			data[i * 3 + 1] = light * (temp * temp + 0.2);
			data[i * 3 + 2] = light * (temp + 0.4);
		}
		else
		{
			data[i * 3] = 0.2 * light;
			data[i * 3 + 1] = 0.2 * light;
			data[i * 3 + 2] = 0.2 * light;
		}
	}
}

uint p_std_environment_id = -1;
uint p_std_diffuse_environment_id = -1;
uint p_std_env_size = 128;
float *p_vec_buffer = NULL;

typedef struct{
	uint stage;
	uint sub_stage;
	uint size;
	float *a_buffer;
	float *b_buffer;
	uint environment_id;
	uint diffuse_environment_id;
}PObjEnv;


PObjEnv *p_env_compute(PObjEnv *env)
{
	uint i;

	if(env != NULL && env->size != p_std_env_size)
	{
		if(env->a_buffer != NULL)
			free(env->a_buffer);
		if(env->b_buffer != NULL)
			free(env->b_buffer);
		if(env->environment_id != p_std_environment_id)
			glDeleteTextures(1, &env->environment_id);
		if(env->diffuse_environment_id != p_std_diffuse_environment_id)
			glDeleteTextures(1, &env->diffuse_environment_id);
		free(env);
		env = NULL;
	}

	if(env == NULL)
	{
		env = malloc(sizeof *env);
		env->stage = 0;
		env->sub_stage = 0;
		env->size = p_std_env_size;
		env->a_buffer = malloc((sizeof *env->a_buffer) * p_std_env_size * p_std_env_size * 6 * 4);
		env->b_buffer = malloc((sizeof *env->b_buffer) * p_std_env_size * p_std_env_size * 6 * 3);
		for(i = 0; i < p_std_env_size * p_std_env_size * 6 * 3; i++)
		{
			env->a_buffer[i] = 0.6;
			env->b_buffer[i] = 0;
		}

		for(; i < p_std_env_size * p_std_env_size * 6 * 4; i++)
			env->a_buffer[i] = 1300;
		env->environment_id = p_std_environment_id;
		env->diffuse_environment_id = p_std_diffuse_environment_id;
		return env;
	}
	switch(env->stage)
	{
		case 0 :
			p_env_create_sky(p_vec_buffer, env->a_buffer, 1, env->size);
			env->stage++;
			env->sub_stage = 0;
		break;
		case 1 :
			glEnable(GL_TEXTURE_CUBE_MAP_EXT);
			env->sub_stage++;
			if(env->sub_stage == 1)
				glGenTextures(1, &env->environment_id);
			glBindTexture(GL_TEXTURE_CUBE_MAP_EXT, env->environment_id);
			if(env->sub_stage == 1)
			{
				glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT, 0, GL_RGB, env->size, env->size, 0, GL_RGB, GL_FLOAT, env->a_buffer); 
			}else if(env->sub_stage == 2)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT, 0, GL_RGB, env->size, env->size, 0, GL_RGB, GL_FLOAT, &env->a_buffer[env->size * env->size * 3]); 
			else if(env->sub_stage == 3)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT, 0, GL_RGB, env->size, env->size, 0, GL_RGB, GL_FLOAT, &env->a_buffer[env->size * env->size * 3 * 2]); 
			else if(env->sub_stage == 4)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT, 0, GL_RGB, env->size, env->size, 0, GL_RGB, GL_FLOAT, &env->a_buffer[env->size * env->size * 3 * 3]); 
			else if(env->sub_stage == 5)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT, 0, GL_RGB, env->size, env->size, 0, GL_RGB, GL_FLOAT, &env->a_buffer[env->size * env->size * 3 * 4]); 
			else if(env->sub_stage == 6)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT, 0, GL_RGB, env->size, env->size, 0, GL_RGB, GL_FLOAT, &env->a_buffer[env->size * env->size * 3 * 5]); 
				env->stage++;
				env->sub_stage = 0;
			}

			glDisable(GL_TEXTURE_CUBE_MAP_EXT);
		break;
		case 2 :
			env->sub_stage = p_env_cube_threw_qube(p_vec_buffer, env->a_buffer, env->b_buffer, env->size, env->sub_stage);
			if(env->sub_stage == env->size * env->size * 6)
			{
				env->stage++;
				env->sub_stage = 0;
				free(env->a_buffer);
				env->a_buffer = NULL;
			}
		break;
		case 3 :
			glEnable(GL_TEXTURE_CUBE_MAP_EXT);
			env->sub_stage++;
			if(env->sub_stage == 1)
				glGenTextures(1, &env->diffuse_environment_id);
			glBindTexture(GL_TEXTURE_CUBE_MAP_EXT, env->diffuse_environment_id);
			if(env->sub_stage == 1)
			{

				glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT, 0, GL_RGB, env->size, env->size, 0, GL_RGB, GL_FLOAT, env->b_buffer); 
			}else if(env->sub_stage == 2)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT, 0, GL_RGB, env->size, env->size, 0, GL_RGB, GL_FLOAT, &env->b_buffer[env->size * env->size * 3]); 
			else if(env->sub_stage == 3)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT, 0, GL_RGB, env->size, env->size, 0, GL_RGB, GL_FLOAT, &env->b_buffer[env->size * env->size * 3 * 2]); 
			else if(env->sub_stage == 4)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT, 0, GL_RGB, env->size, env->size, 0, GL_RGB, GL_FLOAT, &env->b_buffer[env->size * env->size * 3 * 3]); 
			else if(env->sub_stage == 5)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT, 0, GL_RGB, env->size, env->size, 0, GL_RGB, GL_FLOAT, &env->b_buffer[env->size * env->size * 3 * 4]); 
			else if(env->sub_stage == 6)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT, 0, GL_RGB, env->size, env->size, 0, GL_RGB, GL_FLOAT, &env->b_buffer[env->size * env->size * 3 * 5]); 
				env->stage++;
				free(env->b_buffer);
				env->b_buffer = NULL;
			}

			glDisable(GL_TEXTURE_CUBE_MAP_EXT);
		break;
	}
	return env;
}


void p_env_destroy(PObjEnv *env)
{
	if(env->a_buffer != NULL)
		free(env->a_buffer);
	if(env->b_buffer != NULL)
		free(env->b_buffer);
	if(env->environment_id != p_std_environment_id)
		glDeleteTextures(1, &env->environment_id);
	if(env->diffuse_environment_id != p_std_diffuse_environment_id)
		glDeleteTextures(1, &env->diffuse_environment_id);
	free(env);
}

void p_env_init(unsigned int size)
{
/*	PObjEnv *env = NULL;
	p_std_env_size = size;
	p_vec_buffer = malloc((sizeof *p_vec_buffer) * size * size * 6 * 4);
	p_env_create_vector_cube(p_vec_buffer, size);
	while(env == NULL || env->stage != 4)
		env = p_env_compute(env);
	p_std_environment_id = env->environment_id;
	p_std_diffuse_environment_id = env->diffuse_environment_id;
	p_env_destroy(env);*/
}

uint p_env_get_environment(PObjEnv *env)
{
	return env->environment_id;
}

uint p_env_get_diffuse(PObjEnv *env)
{
	return env->diffuse_environment_id;
}

void p_env_draw_pixel(float *vector, float *data, float size, float  x, float y, float z, float  r, float g, float b)
{
	float a, temp, dif;
	int i;

	dif = (1.0 - size);
	a = sqrt(x * x + y * y + z * z);
	x = x / a;
	y = y / a;
	z = z / a;
	r = r / a;
	g = g / a;
	b = b / a;
	for(i = 0; i < TEX_SIZE * TEX_SIZE * 6; i++)
	{
		a =	vector[i * 3] * x + vector[i * 3 + 1] * y + vector[i * 3 + 2] * z;
		if(a > size)
		{
			temp = a - size;
			data[i * 3] += (r * temp) / dif;
			data[i * 3 + 1] += (g * temp) / dif;
			data[i * 3 + 2] += (b * temp) / dif;
		}
	}
}
