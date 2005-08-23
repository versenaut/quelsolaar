
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include "ngl.h"

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

#include "persuade.h"
#include "p_task.h"
#include "p_object.h"


double p_light_compute_brightnes(double *obj, uint32 node_id, uint32 time_s, uint32 time_f)
{
	double pos[3], f;
	ENode *node;
	node = e_ns_get_node(0, node_id);
	if(node == NULL)
		return 0;

	e_nso_get_pos_time(node, pos, time_s, time_f);
	pos[0] -= obj[0];
	pos[1] -= obj[1];
	pos[2] -= obj[2];
	f = pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2] + 0.01;
	e_nso_get_light(node, pos);
	return (pos[0] + pos[1] + pos[2]) / f;
}

void p_light_update(PObjLight *light, uint32 node_id, uint light_count, uint32 time_s, uint32 time_f)
{
	double pos[3], brightnes[LIGHTS_PER_OBJECT], f;
	ENode *node;
	uint i, j, tmp;
	node = e_ns_get_node(0, node_id);
	e_nso_get_pos_time(node, pos, time_s, time_f);

	for(i = 0; i < LIGHTS_PER_OBJECT; i++)
		brightnes[i] = p_light_compute_brightnes(pos, light->lights[i], time_s, time_f);

	for(i = 0; i < light_count - 2; i++)
	{
		if(brightnes[i] < brightnes[i + 1])
		{
			f = brightnes[i];
			brightnes[i] = brightnes[i + 1];
			brightnes[i + 1] = f;
			tmp = light->lights[i];
			light->lights[i] = light->lights[i + 1];
			light->lights[i + 1] = tmp;
		}
	}
	if(light_count != 1 && brightnes[i] < brightnes[i + 1] && light->lightfade > 0.999)
	{
		f = brightnes[i];
		brightnes[i] = brightnes[i + 1];
		brightnes[i + 1] = f;
		tmp = light->lights[i];
		light->lights[i] = light->lights[i + 1];
		light->lights[i + 1] = tmp;
	}
	i++;
	if(light_count != 1 && brightnes[i] < brightnes[i + 1])
	{
		if(light->lightfade < 0.001)
		{
			f = brightnes[i];
			brightnes[i] = brightnes[i + 1];
			brightnes[i + 1] = f;
			tmp = light->lights[i];
			light->lights[i] = light->lights[i + 1];
			light->lights[i + 1] = tmp;
		}else
			light->lightfade -= 0.02;
	}else if(light->lightfade < 0.999)
		light->lightfade += 0.02;

	for(; i < LIGHTS_PER_OBJECT - 1; i++)
	{
		if(brightnes[i] < brightnes[i + 1])
		{
			f = brightnes[i];
			brightnes[i] = brightnes[i + 1];
			brightnes[i + 1] = f;
			tmp = light->lights[i];
			light->lights[i] = light->lights[i + 1];
			light->lights[i + 1] = tmp;
		}
	}
	for(j = 0; j < 200; j++)
	{
		node = e_ns_get_node_next(light->node_id + 1, 0, V_NT_OBJECT);
		if(node == NULL)
			light->node_id = e_ns_get_node_id(e_ns_get_node_next(0, 0, V_NT_OBJECT));
		else
			light->node_id = e_ns_get_node_id(node);
		for(i = 0; i < LIGHTS_PER_OBJECT && light->node_id != light->lights[i]; i++);
		f = p_light_compute_brightnes(pos, light->node_id, time_s, time_f);
		if(i == LIGHTS_PER_OBJECT && f > brightnes[LIGHTS_PER_OBJECT - 1])
		{
			light->lights[LIGHTS_PER_OBJECT - 1] = light->node_id;
			return;
		}
	}
}

extern void sui_draw_3d_line_gl(float start_x, float start_y,  float start_z, float end_x, float end_y, float end_z, float red, float green, float blue);

uint current_shadow_light = -1;

void p_set_enable_shadow(uint id)
{
	double color[3];
	ENode *node;
	if(id != -1)
	{
		for(node = e_ns_get_node_next(0, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
		{
			e_nso_get_light(node, color);
			if(color[0] > 0.1)
				current_shadow_light = e_ns_get_node_id(node);
		}
	}else
		current_shadow_light = id;
}

void p_set_shadow_light(uint32 time_s, uint32 time_f)
{
	ENode *node;
	node = e_ns_get_node(0, current_shadow_light);
	if(node != NULL)
	{
		double pos[3], color[3];
		float f_pos[4] = {0, 0, 0, 1}, f_color[3], mult = 1, zero[3] = {0, 0, 0};
		e_nso_get_pos_time(node, pos, time_s, time_f);
		e_nso_get_pos(node, pos, NULL, NULL, NULL, NULL, NULL);
		e_nso_get_light(node, color);
		f_pos[0] = (float)pos[0];
		f_pos[1] = (float)pos[1];
		f_pos[2] = (float)pos[2];
		f_color[0] = (float)color[0];
		f_color[1] = (float)color[1];
		f_color[2] = (float)color[2];
		glLightfv(GL_LIGHT0, GL_POSITION, f_pos);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, f_color);
		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0);
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 1);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0);
	}
}

void p_set_light(PObjLight *light, uint light_count, uint32 time_s, uint32 time_f)
{
	double pos[3], color[3];
	float f_pos[4] = {0, 0, 0, 1}, f_color[3], mult = 1, zero[3] = {0, 0, 0};
	ENode *node;
	uint i;
//	glEnable(GL_LIGHTING);

	for(i = 0; i < light_count; i++)
	{
		glEnable(GL_LIGHT0 + i);
//		printf("i %u %u\n", i, light->lights[i]);
		node = e_ns_get_node(0, light->lights[i]);
		if(node == NULL || current_shadow_light == light->lights[i])
		{
			f_pos[0] = 1;
			f_pos[1] = 1;
			f_pos[2] = 1;
//			printf("bad\n");
			glLightfv(GL_LIGHT0 + i, GL_POSITION, f_pos);
			glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, zero);
		}
		else
		{
			e_nso_get_pos_time(node, pos, time_s, time_f);
			e_nso_get_pos(node, pos, NULL, NULL, NULL, NULL, NULL);
			e_nso_get_light(node, color);
		/*	if(i == light_count - 1)
				mult = light->lightfade;
			else*/
				mult = 1;
			f_pos[0] = (float)pos[0];
			f_pos[1] = (float)pos[1];
			f_pos[2] = (float)pos[2];
			f_color[0] = (float)color[0] * mult * 0.3;
			f_color[1] = (float)color[1] * mult * 0.3;
			f_color[2] = (float)color[2] * mult * 0.3;
//			printf("good %f %f %f\n", color[0], color[1], color[2]);
			glLightfv(GL_LIGHT0 + i, GL_POSITION, f_pos);
			glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, f_color);
			glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, 0);
			glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, 1);
			glLightf(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, 0);

		}
	}
//	printf("end\n");
}
