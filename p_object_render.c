
#include <stdio.h>
#include <stdlib.h>

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
		if(p_rm_drawable(mesh) && p_rm_get_shadow(mesh))
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

void p_render_object(ENode *node, boolean transparency, boolean test)
{
	double matrix[16], scale[3];
	PMesh *mesh;
	PObject *o;
	uint32 i, j = 0, count, range, *ref, mat;
	o = e_ns_get_custom_data(node, P_ENOUGH_SLOT);

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
				mat = p_rm_get_material(mesh, j);
				if(transparency == p_shader_transparancy(mat))
				{
					p_shader_bind(mat);
					p_shader_param_load(node, mat, p_rm_get_param(mesh), p_rm_get_param_count(mesh), /*p_env_get_environment(o->environment)*/o->impostor.environment, o->impostor.blur/*, p_env_get_diffuse(o->environment)*/);
					glDrawElements(GL_TRIANGLES, p_rm_get_material_range(mesh, j) - range, GL_UNSIGNED_INT, &ref[range]);
					range = p_rm_get_material_range(mesh, j);
					p_shader_unbind(mat);
				}
			}
		}	
	}
}

void p_render_lit_and_transformed_object(ENode *node, boolean transparency)
{
	double matrix[16], scale[3];
	PMesh *mesh;
	PObject *o;
	uint32 i, j = 0, count, range, *ref;
	o = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
	p_set_light(&o->light, 3, 0, 0);

	glPushMatrix();
	p_render_set_transform(node);
	p_render_object(node, transparency, FALSE);
	glPopMatrix();
}

boolean p_render_with_shadows = TRUE;
boolean p_render_wireframe = FALSE;

void p_render_set_shadows(boolean shadows)
{
	p_render_with_shadows = shadows;
}

boolean p_render_get_shadows(void)
{
	return p_render_with_shadows;
}

void p_render_set_wireframe(boolean wireframe)
{
	p_render_wireframe = wireframe;
}

boolean p_render_get_wireframe(void)
{
	return p_render_wireframe;
}

void p_draw_all_impostors(void);
void p_draw_object_impostor(ENode *node);
void p_update_object_impostors(void);
boolean p_draw_object_as_impostor(ENode *node);
void p_set_enable_shadow(uint id);
void p_draw_flares(void);

void p_draw_scene(void)
{
	ENode *node;
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	if(p_shaders_supported() && p_render_with_shadows && !p_render_wireframe)
	{
		glCullFace(GL_FRONT);
		glEnable(GL_CULL_FACE);
		p_set_enable_shadow(1);
		for(node = e_ns_get_node_next(0, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
			if(!p_draw_object_as_impostor(node) && !e_nso_get_hide(node))
				p_render_lit_and_transformed_object(node, FALSE);
		glClearStencil(0);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);

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
			if(!e_nso_get_hide(node))
				p_render_object_shadow(node);

		glCullFace(GL_FRONT);
		glStencilOp(GL_KEEP, GL_DECR, GL_KEEP); 
		glColor3f(0, 0.8, 0);
		for(node = e_ns_get_node_next(0, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
			if(!e_nso_get_hide(node))
				p_render_object_shadow(node);
		glEnable(GL_DEPTH_TEST);
		glPolygonOffset(0, 0);

		glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_BLEND);

		glColorMask(1, 1, 1, 1);
		glDepthMask(1);

		p_set_enable_shadow(-1);

		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_EQUAL, 0, 0xFF);
	}	
	if(p_render_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_BLEND);
	for(node = e_ns_get_node_next(0, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
	{
		if(!p_draw_object_as_impostor(node) && !e_nso_get_hide(node))
			p_render_lit_and_transformed_object(node, FALSE);
	}
	if(p_shaders_supported())
		glDisable(GL_STENCIL_TEST);
	for(node = e_ns_get_node_next(0, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
	{
		if(!p_draw_object_as_impostor(node) && !e_nso_get_hide(node))
			p_render_lit_and_transformed_object(node, TRUE);
	}
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.9);
	glDisableClientState(GL_NORMAL_ARRAY);
	p_draw_flares();
	for(node = e_ns_get_node_next(0, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
	{
		if(p_draw_object_as_impostor(node) && !e_nso_get_hide(node))
			p_draw_object_impostor(node);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);
	glPopMatrix();
	p_set_enable_shadow(-1);
	glEnableClientState(GL_NORMAL_ARRAY);
	p_update_object_impostors();
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisable(GL_LIGHTING);
}
