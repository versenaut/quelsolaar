
#include <stdio.h>
#include <stdlib.h>

//#include "ngl.h"

#ifdef WIN32
	#include <windows.h>
	#include <GL/gl.h>
#else
	#include <GL/gl.h>
#endif

#include "enough.h"
#include "st_types.h"
#include "p_task.h"
#include "persuade.h"
#include "p_object.h"

void p_object_func(ENode *node, ECustomDataCommand command)
{	
	PObject *o;
	uint i;
	switch(command)
	{
		case E_CDC_CREATE :
			o = malloc(sizeof *o);
			o->set_count = 0;
			o->sets = NULL;
			o->temp.draw = NULL;
			o->temp.progress = NULL;
			o->task = FALSE;
			e_ns_set_custom_data(node, P_ENOUGH_SLOT, o);
			for(i = 0; i < LIGHTS_PER_OBJECT; i++) 
				o->lights[i] = 0;
			o->environment = p_env_compute(NULL);
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

boolean p_service_object_geometry(ENode *node, PRenderSets *set, ENode *g_node)
{
	if((set->draw == NULL && set->progress == NULL) || (set->progress == NULL && !p_rm_validate(set->draw)))
	{
		set->progress = p_rm_create(g_node);
		if(set->progress == NULL && set->draw != NULL)
		{
			p_rm_destroy(set->draw);
			set->draw = NULL;
		}
		return set->progress != NULL;
	}
	if(set->progress != NULL)
	{
		if(p_rm_validate(set->progress))
		{
			p_rm_service(set->progress, e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(g_node, 0)));
			if(p_rm_drawable(set->progress))
			{
				if(set->draw != NULL)
					p_rm_destroy(set->draw);
				set->draw = set->progress;
				set->progress = NULL;
				set->vertex_version = e_nsg_get_layer_version(e_nsg_get_layer_by_id(g_node, 0));
			}
		}else
		{
			p_rm_destroy(set->progress);
			set->progress = NULL;
		}
		return TRUE;
	}

//	if(set->draw == NULL &&)
//	p_rm_compute(set->progress, e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(g_node, 0)))
	return FALSE;
}
/*
typedef struct{
	uint32	id;
	uint32	material;
	uint32	target;
	uint	vertex_version;
	PMesh	*draw;	
	PMesh	*progress;	
}PRenderSets;
*/
PRenderSets *p_object_add_render_set(PObject *o, uint32 geometry, uint32 material, uint32 target)
{
	uint i, j;

	for(i = 0; i < o->set_count && o->sets[i].id != -1; i++)
		if(o->sets[i].id == geometry && o->sets[i].material == material && o->sets[i].target == target)
			return &o->sets[i];
	if(i == o->set_count)
	{
		o->sets = realloc(o->sets, (sizeof *o->sets) * (i + 16));
		for(j = i; j < i + 16; j++)
		{
			o->sets[j].id = -1;
			o->sets[j].draw = NULL;	
			o->sets[j].progress = NULL;	
		}
	}
	o->sets[i].id = geometry;
	o->sets[i].material = material;
	o->sets[i].target = target;
	o->sets[i].param = p_param_array_allocate(geometry, /*uint32*/ material /*ENode *g_node, ENode *m_node*/);
	o->set_count++;
	return &o->sets[i];
}

void p_object_compute(ENode *node)
{
	EObjLink *link, *link2;
	ENode	*g_node, *m_node;
	PObject *o;
	PRenderSets *set;
	uint32 material = -1;
	o = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
	for(link = e_nso_get_next_link(node, 0); link != NULL; link = e_nso_get_next_link(node, e_nso_get_link_id(link) + 1))
	{
		g_node = e_ns_get_node(0, e_nso_get_link_node(link));
		if(V_NT_GEOMETRY == e_ns_get_node_type(g_node))
		{
			for(link2 = e_nso_get_next_link(node, 0); link2 != NULL; link2 = e_nso_get_next_link(node, e_nso_get_link_id(link2) + 1))
			{
				m_node = e_ns_get_node(0, e_nso_get_link_node(link2));
				if(V_NT_MATERIAL == e_ns_get_node_type(m_node))
				{
					material = e_nso_get_link_node(link2);
					break;
				}
			}
			set = p_object_add_render_set(o, e_nso_get_link_node(link), material, -1);
			p_service_object_geometry(node, set, g_node);
		}
	}
}

boolean p_object_task_func(uint id)
{
	PObject *o;
	ENode *node;
	node = e_ns_get_node(0, id);
	if(node!= NULL)
	{
		p_object_compute(node);
		o = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
//		o->task = FALSE;
	}
	return TRUE;
}

void p_draw_object(ENode *node)
{
	PObject *o;
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
	glPopMatrix();
}
/*
extern void *p_param_array_allocate(ENode *g_node, ENode *m_node);
extern void p_param_array_destroy(void *p);
extern boolean p_array_update(void *p, PMesh *mesh, ENode *g_node, ENode *m_node);
extern egreal **p_param_get_array(void *p);

*/
void p_render_object(ENode *node)
{
	double matrix[16], dpos[3], dlight[3];
	float pos[4] = {1, 2, 1, 1}, light[4] = {1, 1, 1, 1};
	ENode *light_node;
	PRenderSets *set;
	PObject *o;
	uint32 i, j = 0;
	o = e_ns_get_custom_data(node, P_ENOUGH_SLOT);

	if(!o->task)
		p_task_add(e_ns_get_node_id(node), 1, p_object_task_func);
	o->task = TRUE;
/*	for(i = 0; i < LIGHTS_PER_OBJECT; i++)
		glDisable(GL_LIGHT0 + i);

	/*just adding a standard light */
/*	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0 + j, GL_POSITION, pos);
	glLightfv(GL_LIGHT0 + j, GL_DIFFUSE, light);
	j++;

/*	for(i = 0; i < LIGHTS_PER_OBJECT && j < 12; i++)
	{
		light_node = e_ns_get_node(0, o->lights[i]);
		if(light_node != NULL && V_NT_OBJECT == e_ns_get_node_type(light_node))
		{
		//	e_nso_get_position(light_node, dpos);
			e_nso_get_light(light_node, dlight);
			if(dlight[0] > 0.00001 || dlight[1] > 0.00001 || dlight[2] > 0.00001)
			{
				glEnable(GL_LIGHT0 + j);
				pos[0] = dpos[0];
				pos[1] = dpos[1];
				pos[2] = dpos[2];
				light[0] = dlight[0];
				light[1] = dlight[1];
				light[2] = dlight[2];
				glLightfv(GL_LIGHT0 + j, GL_POSITION, pos);
				glLightfv(GL_LIGHT0 + j, GL_DIFFUSE, light);
				j++;
			}
		}
	}*/
	glPushMatrix();
//	e_nso_get_matrix(node, matrix);
	for(i = 0; i < o->set_count; i++)
	{
		set = &o->sets[i];
		if(set->draw != NULL)
		{
		//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glCullFace(GL_BACK);
			glEnable(GL_DEPTH_TEST);
			p_shader_bind(set->material);
			{
				p_array_update(set->param, set->draw, set->id, set->material);
				p_shader_param_load(node, set->material, p_param_get_array(set->param), p_env_get_environment(o->environment), p_env_get_diffuse(o->environment));

				glEnableClientState(GL_NORMAL_ARRAY);
				glEnableClientState(GL_VERTEX_ARRAY);
				glColor3f(0.8, 0.3, 0.4);
			#ifdef E_GEOMETRY_REAL_PRESISSION_64_BIT
				glVertexPointer(3, GL_DOUBLE, 0, p_rm_get_vertex(set->draw));
				glNormalPointer(GL_DOUBLE, 0 , p_rm_get_normal(set->draw));
		//		glNormalPointer(GL_DOUBLE, 0 , p_rm_get_vertex(set->draw));
			#endif
			#ifdef E_GEOMETRY_REAL_PRESISSION_32_BIT
				glVertexPointer(3, GL_FLOAT, 0, p_rm_get_vertex(set->draw));
				glNormalPointer(GL_FLOAT, 0 , p_rm_get_normal(set->draw));
		//		glNormalPointer(GL_FLOAT, 0 , p_rm_get_vertex(set->draw));
			#endif

				glDrawElements(GL_TRIANGLES, p_rm_get_ref_length(set->draw), GL_UNSIGNED_INT, p_rm_get_reference(set->draw));
				glDisableClientState(GL_NORMAL_ARRAY);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				p_shader_unbind(set->material);
			}
			glDisable(GL_DEPTH_TEST);
		}	
	}
	glPopMatrix();
}

void simple_draw_gl(uint draw_type, float *array, uint length, uint dimentions, float red, float green, float blue)
{
	glColor4f(red, green, blue, 0);	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(dimentions, GL_FLOAT , 0, array);
	glDrawArrays(draw_type, 0, length);
	glDisable(GL_BLEND);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
}

void p_draw_scene(void)
{
	ENode *node;
	double pos[3];
	float f[4] = {0, 0, 0, 1};
	float light[18] = {0.1, 0, 0, -0.1, 0, 0, 0, 0.1, 0, 0, -0.1, 0, 0, 0, 0.1, 0, 0, -0.1};
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	for(node = e_ns_get_node_next(0, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
	{
		e_nso_get_light(node, pos);
		if(pos[0] > 0.001 || pos[1] > 0.001 || pos[2] > 0.001)
		{
			f[0] = pos[0];
			f[1] = pos[1];
			f[2] = pos[2];
			f[3] = 1;
			glLightfv(GL_LIGHT0, GL_DIFFUSE, f);
			e_nso_get_pos_time(node, pos, 0, 0);
			f[0] = pos[0];
			f[1] = pos[1];
			f[2] = pos[2];
			f[3] = 1;
			glLightfv(GL_LIGHT0, GL_POSITION, f);
			break;
		}
	}

	glPushMatrix();
	glTranslated(pos[0], pos[1], pos[2]);
/*	if(node == NULL)
	{
		float l[4] = {1, 1, 1, 1};
		glLightfv(GL_LIGHT0, GL_POSITION, l);
	}*/
	simple_draw_gl(GL_LINES, light, 6, 3, 0.5, 0.5, 0.5);
	glPopMatrix();
	
	for(node = e_ns_get_node_next(0, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
	{
		glPushMatrix();
		e_nso_get_pos_time(node, pos, 0, 0);
		glTranslated(pos[0], pos[1], pos[2]);
		p_object_compute(node);
		p_render_object(node);
		glPopMatrix();
	}
	glDisable(GL_LIGHTING);
}
