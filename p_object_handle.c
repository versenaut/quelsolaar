
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include "ngl.h"

#ifdef _WIN32
	#include <windows.h>
	#include <GL/gl.h>
#else
	#include <GL/gl.h>
#endif

/*#include "enough.h"*/
#include "persuade.h"
#include "p_task.h"
#include "p_object.h"
#include "st_types.h"

extern void p_init_impostor(PObjImpostor *imp);

void p_object_func(ENode *node, ECustomDataCommand command)
{	
	PObject *o;
	uint i;
	switch(command)
	{
		case E_CDC_CREATE :
			o = malloc(sizeof *o);
			o->meshes = malloc((sizeof *o->meshes) * 16);
			o->mesh_count = 16;
			for(i = 0; i < o->mesh_count; i++)
				o->meshes[i] = NULL;
			o->task = FALSE;
			e_ns_set_custom_data(node, P_ENOUGH_SLOT, o);
			for(i = 0; i < LIGHTS_PER_OBJECT; i++) 
				o->light.lights[i] = -1;
			o->light.lightfade = 1;
			o->environment = p_env_compute(NULL);
			o->version = -1;
			p_init_impostor(&o->impostor);
			break;
		case E_CDC_DESTROY :
			o = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
			p_env_destroy(o->environment);
			free(o);
		break;
	}
}

extern void *p_env_compute(void *env);
extern void p_env_destroy(void *env);
extern void p_env_init(unsigned int size);
extern uint p_env_get_environment(void *env);
extern uint p_env_get_diffuse(void *env);


void p_link_update(ENode *node)
{
	EObjLink *link;
	ENode	*g_node;
	PObject *o;
	uint i, j, count;
	o = e_ns_get_custom_data(node, P_ENOUGH_SLOT);

//	if(o->version != e_ns_get_node_version_struct(node))
	{
		for(link = e_nso_get_next_link(node, 0); link != NULL; link = e_nso_get_next_link(node, e_nso_get_link_id(link) + 1))
		{
			g_node = e_ns_get_node(0, e_nso_get_link_node(link));
			if(g_node != NULL && V_NT_GEOMETRY == e_ns_get_node_type(g_node))
			{
				for(i = 0; i < o->mesh_count && o->meshes[i] != NULL && p_rm_get_geometry_node(o->meshes[i]) != e_ns_get_node_id(g_node); i++);
				if(i == o->mesh_count)
				{
					o->meshes = realloc(o->meshes, (sizeof * o->meshes) * (o->mesh_count + 16));
					for(j = o->mesh_count; j < o->mesh_count + 16; j++)
						o->meshes[j] = NULL;
					o->mesh_count += 16;
				}
				if(o->meshes[i] == NULL)
					o->meshes[i] = p_rm_create(g_node);
			}
		}
		for(i = 0; i < o->mesh_count && o->meshes[i] != NULL; i++);
		count = i;

		for(i = 0; i < o->mesh_count && o->meshes[i] != NULL; i++)
		{
			for(link = e_nso_get_next_link(node, 0); link != NULL; link = e_nso_get_next_link(node, e_nso_get_link_id(link) + 1))
				if(p_rm_get_geometry_node(o->meshes[i]) == e_nso_get_link_node(link))
					break;
			if(link == NULL)
			{
			/*	for(j = i + 1; j < count; j++)
					o->meshes[j - 1] = o->meshes[j];
				o->meshes[j - 1] = NULL;*/
			}
		}
		o->version = e_ns_get_node_version_struct(node);
	}
}

boolean p_service_object_geometry(ENode *node)
{
	PObject *o;
	uint i;
	o = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
	for(i = 0; i < o->mesh_count && o->meshes[i] != NULL; i++)
		o->meshes[i] = p_rm_service(o->meshes[i], node, NULL);
	return FALSE;
}

boolean p_object_task_func(uint id)
{
	PObject *o;
	ENode *node;
	node = e_ns_get_node(0, id);
		
	if(node != NULL)
	{
		p_link_update(node);
		p_service_object_geometry(node);
		o = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
		p_light_update(&o->light, id, 5, 0, 0);
		o->task = FALSE;
	}
	return TRUE;
}

void p_draw_object(ENode *node)
{
/*	PObject *o;
	ENode *next_node;
	uint32 time_s, time_f;
	double matrix[16];
	EObjLink *link;
	verse_session_get_time(&time_s, &time_f);
	e_nso_get_matrix(node, matrix, time_s, time_f);
	glPushMatrix();
	glLoadMatrixd(matrix);
	o = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
	for(link = e_nso_get_next_link(node, 0); link != NULL; link = e_nso_get_next_link(node, e_nso_get_link_id(link) + 1))
		if((next_node = e_ns_get_node(0, e_nso_get_link_node(link))) != NULL && e_ns_get_node_type(next_node) == V_NT_OBJECT)
			p_draw_object(next_node);
	if(o->temp.draw != NULL)
	{
		//render_a_mesh_using_ngl(o->temp.draw);
	}
	glPopMatrix();*/
}
/*
extern void *p_param_array_allocate(ENode *g_node, ENode *m_node);
extern void p_param_array_destroy(void *p);
extern boolean p_array_update(void *p, PMesh *mesh, ENode *g_node, ENode *m_node);
extern egreal **p_param_get_array(void *p);

*/
