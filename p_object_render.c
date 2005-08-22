
#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
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
#include "p_shader.h"

extern uint *p_rm_get_mnormal_ref(PMesh *mesh);
void *p_rm_get_param(PMesh *mesh);
uint p_rm_get_param_count(PMesh *mesh);
boolean p_object_task_func(uint id);

void p_render_set_transform(ENode *node)
{
	double matrix[16], scale[3];
	e_nso_get_matrix(node, matrix, 0, 0);
	e_nso_get_pos_time(node, &matrix[12], 0, 0);
//	printf("pos = %f %f %f\n", matrix[12], matrix[13], matrix[14]);
	glMultMatrixd(matrix);
	e_nso_get_scale(node, scale);
	glScaled(1 / scale[0], 1 / scale[1], 1 / scale[2]);
}


void p_render_z(void)
{

	ENode *node;
	PMesh *mesh;
	PObject *o;
	uint32 i, j = 0, count, range, *ref;

	for(node = e_ns_get_node_next(0, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
	{
		o = e_ns_get_custom_data(node, P_ENOUGH_SLOT);

		if(!o->task)
			p_task_add(e_ns_get_node_id(node), 1, p_object_task_func);
		o->task = TRUE;
		glPushMatrix();
		p_render_set_transform(node);
		for(i = 0; i < o->mesh_count && o->meshes[i] != NULL; i++)
		{
			mesh = o->meshes[i];
			if(p_rm_drawable(mesh))
			{
				#ifdef E_GEOMETRY_REAL_PRECISION_64_BIT
				glVertexPointer(3, GL_DOUBLE, 0, p_rm_get_vertex(mesh));
				glNormalPointer(GL_DOUBLE, 0 , p_rm_get_normal(mesh));
				#endif
				#ifdef E_GEOMETRY_REAL_PRECISION_32_BIT
				glVertexPointer(3, GL_FLOAT, 0, p_rm_get_vertex(mesh));
				glNormalPointer(GL_FLOAT, 0 , p_rm_get_normal(mesh));
				#endif
				glDrawElements(GL_TRIANGLES, p_rm_get_ref_length(mesh), GL_UNSIGNED_INT, p_rm_get_reference(mesh));
			}	
		}
		glPopMatrix();
	}
}


void p_render_object_shadow(ENode *node)
{
	double matrix[16], scale[3];
	PMesh *mesh;
	PObject *o;
	uint32 i, j = 0, count, range, *ref;
	o = e_ns_get_custom_data(node, P_ENOUGH_SLOT);

	if(!o->task)
		p_task_add(e_ns_get_node_id(node), 1, p_object_task_func);
	o->task = TRUE;
	p_set_shadow_light(0, 0);


	p_shader_shadow_bind();
	glPushMatrix();
	p_render_set_transform(node);
	for(i = 0; i < o->mesh_count && o->meshes[i] != NULL; i++)
	{
		mesh = o->meshes[i];
		if(p_rm_drawable(mesh))
		{
			#ifdef E_GEOMETRY_REAL_PRECISION_64_BIT
			glVertexPointer(3, GL_DOUBLE, 0, p_rm_get_vertex(mesh));
			glNormalPointer(GL_DOUBLE, 0 , p_rm_get_normal(mesh));
			#endif
			#ifdef E_GEOMETRY_REAL_PRECISION_32_BIT
			glVertexPointer(3, GL_FLOAT, 0, p_rm_get_vertex(mesh));
			glNormalPointer(GL_FLOAT, 0 , p_rm_get_normal(mesh));
			#endif
			count = p_rm_get_mat_count(mesh);
			ref = p_rm_get_reference(mesh);
			glDrawElements(GL_TRIANGLES, p_rm_get_ref_length(mesh), GL_UNSIGNED_INT, ref);
		}	
	}
	glPopMatrix();
}

void p_render_object(ENode *node)
{
	double matrix[16], scale[3];
	PMesh *mesh;
	PObject *o;
	uint32 i, j = 0, count, range, *ref;

	o = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
	if(o == NULL)
		return;

	if(!o->task)
		p_task_add(e_ns_get_node_id(node), 1, p_object_task_func);
	o->task = TRUE;

	for(i = 0; i < o->mesh_count && o->meshes[i] != NULL; i++)
	{
		mesh = o->meshes[i];
		if(p_rm_drawable(mesh))
		{
#ifdef E_GEOMETRY_REAL_PRECISION_64_BIT
			glVertexPointer(3, GL_DOUBLE, 0, p_rm_get_vertex(mesh));
			glNormalPointer(GL_DOUBLE, 0 , p_rm_get_normal(mesh));
#endif
#ifdef E_GEOMETRY_REAL_PRECISION_32_BIT
			glVertexPointer(3, GL_FLOAT, 0, p_rm_get_vertex(mesh));
			glNormalPointer(GL_FLOAT, 0 , p_rm_get_normal(mesh));
#endif
			count = p_rm_get_mat_count(mesh);
			ref = p_rm_get_reference(mesh);
			range = 0;
			for(j = 0; j < count; j++)
			{
				p_shader_bind(p_rm_get_material(mesh, j));
				p_shader_param_load(node, p_rm_get_material(mesh, j), p_rm_get_param(mesh), p_rm_get_param_count(mesh), p_env_get_environment(o->environment), p_env_get_diffuse(o->environment));
				glDrawElements(GL_TRIANGLES, p_rm_get_material_range(mesh, j) - range, GL_UNSIGNED_INT, &ref[range]);
				range = p_rm_get_material_range(mesh, j);
				p_shader_unbind(p_rm_get_material(mesh, j));
			}
//			p_shader_unbind(set->material);
		}	
	}
}

void p_render_lit_and_transformed_object(ENode *node)
{
	double matrix[16], scale[3];
	PMesh *mesh;
	PObject *o;
	uint32 i, j = 0, count, range, *ref;
	o = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
	p_set_light(&o->light, 3, 0, 0);


	glPushMatrix();
	p_render_set_transform(node);
	p_render_object(node);

	glPopMatrix();
}

void p_draw_scene(void)
{
	ENode *node;
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	if(p_shaders_suported())
	{
		p_set_enable_shadow(1);
		for(node = e_ns_get_node_next(0, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
			p_render_lit_and_transformed_object(node);
		glClearStencil(0);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 0, 0xff);
		glStencilMask(0xff);
		glCullFace(GL_BACK);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
		glColorMask(0, 0, 0, 0);
		glDepthMask(0);
		glDepthFunc(GL_LEQUAL);
		for(node = e_ns_get_node_next(0, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
			p_render_lit_and_transformed_object(node);
		glCullFace(GL_FRONT);
		glStencilOp(GL_KEEP, GL_DECR, GL_KEEP); 
		glColor3f(0, 0.8, 0);
		for(node = e_ns_get_node_next(0, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
			p_render_lit_and_transformed_object(node);
		glEnable(GL_DEPTH_TEST);

		glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glColorMask(1, 1, 1, 1);
		glDepthMask(1);

		p_set_enable_shadow(-1);

		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_EQUAL, 0, 0xFF);
	}

	for(node = e_ns_get_node_next(0, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
		p_render_lit_and_transformed_object(node);
	if(p_shaders_suported())
		glDisable(GL_STENCIL_TEST);






	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//	p_shader_unbind(set->material);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
}
