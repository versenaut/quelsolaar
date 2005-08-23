/*
 * 
*/

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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "persuade.h"
#include "p_task.h"

typedef struct{
	PTextureHandle	*texture;
	uint16			mapping;
	uint16			geometry;
	float			color[4];
	boolean			lighting;
	uint			blend_src;
	uint			blend_dst;
	uint			version;
}PMatFallBack;


extern PMatFallBack *p_shader_compute_fallback(ENode *node, PMatFallBack *fallback);

void p_shader_fallback_func(ENode *node, ECustomDataCommand command)
{	
	PMatFallBack *s;
	switch(command)
	{
		case E_CDC_DATA :
			e_ns_set_custom_data(node, P_ENOUGH_SLOT, p_shader_compute_fallback(node, e_ns_get_custom_data(node, P_ENOUGH_SLOT)));
		break;
		case E_CDC_DESTROY :
			s = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
			if(s != NULL)
			{
				if(s->texture != NULL)
					p_th_destroy_texture_handle(s->texture);
				free(s);
			}
		break;
	}
}




void p_shader_bind_fallback(uint node_id)
{
	PMatFallBack *s = NULL;
	ENode *node;
	node = e_ns_get_node(0, node_id);
	if(node != NULL)
	{
		s = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
		if(s == NULL)
		{
			glEnable(GL_NORMALIZE);
			glEnable(GL_LIGHTING);
			glColor4f(0.2, 0.6, 1, 1);
			glBlendFunc(GL_ZERO, GL_ONE);
		}else
		{
			if(s->texture != NULL)
			{
				glEnable(GL_TEXTURE);
				glBindTexture(p_th_get_texture_dimentions(s->texture), p_th_get_texture_id(s->texture));
			}else
				glDisable(GL_TEXTURE);
			glColor4f(s->color[0], s->color[1], s->color[2], s->color[3]);

			if(s->lighting)
				glEnable(GL_LIGHTING);
			else
				glDisable(GL_LIGHTING);
			glBlendFunc(s->blend_src, s->blend_dst);
		}
	}
}

void p_shader_fallback_parce(ENode *node, PMatFallBack *fallback, uint16 frag_id)
{
	VMatFrag *f;
	VNMFragmentType type;
	if(!e_nsm_enter_fragment(node, frag_id))
		return;		
	f = e_nsm_get_fragment(node, frag_id);
	if(NULL == f)
	 	return;
	type =  e_nsm_get_fragment_type(node, frag_id);
	switch(type)
	{
		case VN_M_FT_COLOR :
			fallback->color[0] = f->color.red;
			fallback->color[1] = f->color.green;
			fallback->color[2] = f->color.blue;
		break;
		case VN_M_FT_LIGHT :
			fallback->lighting = TRUE;
		break;
		case VN_M_FT_TRANSPARENCY :
			fallback->blend_src = GL_SRC_ALPHA;
			fallback->blend_dst = GL_ONE_MINUS_SRC_ALPHA; 
			fallback->color[3] = 0.02;
		break;
		case VN_M_FT_GEOMETRY :
			fallback->geometry = frag_id;
		break;
		case VN_M_FT_TEXTURE :
			if(VN_M_FT_GEOMETRY == e_nsm_get_fragment_type(node, f->texture.mapping))
			{
				if(fallback->texture == NULL && e_nsm_get_fragment_type(node, f->texture.mapping) == VN_M_FT_GEOMETRY)
				{
					fallback->texture = p_th_create_texture_handle(f->texture.bitmap, f->texture.layer_r, f->texture.layer_g, f->texture.layer_b);
					fallback->mapping = f->texture.mapping;
				}
			}
		break;
		case VN_M_FT_BLENDER :
			p_shader_fallback_parce(node, fallback, f->blender.data_a);
			p_shader_fallback_parce(node, fallback, f->blender.data_b);
		break;
		case VN_M_FT_MATRIX :
			p_shader_fallback_parce(node, fallback, f->matrix.data);
		break;
	}
	e_nsm_leave_fragment(node, frag_id);
}



PMatFallBack *p_shader_compute_fallback(ENode *node, PMatFallBack *fallback)
{
	VMatFrag *f, *f_a, *f_b, *f_c;
	VNMFragmentType type, type_a, type_b, type_c;
	if(NULL == e_nsm_get_fragment(node, e_nsm_get_fragment_color_front(node)))
	 	return NULL;
	if(fallback == NULL)
		fallback = malloc(sizeof *fallback);
	else if(fallback->texture != NULL)
		p_th_destroy_texture_handle(fallback->texture);
	fallback->texture = NULL;
	fallback->mapping = -1;
	fallback->geometry = -1;
	fallback->color[0] = 1;
	fallback->color[1] = 0;
	fallback->color[2] = 1;
	fallback->color[3] = 1;
	fallback->lighting = FALSE;
	fallback->blend_src = GL_ONE;
	fallback->blend_dst = GL_ZERO; 

	f = e_nsm_get_fragment(node, e_nsm_get_fragment_color_front(node));
	type =  e_nsm_get_fragment_type(node, e_nsm_get_fragment_color_front(node));
	if(type == VN_M_FT_BLENDER)
	{
		type_a = e_nsm_get_fragment_type(node, f->blender.data_a);
		type_b = e_nsm_get_fragment_type(node, f->blender.data_b);
		type_c = e_nsm_get_fragment_type(node, f->blender.control);
		f_a = e_nsm_get_fragment(node, f->blender.data_a);
		f_b = e_nsm_get_fragment(node, f->blender.data_b);
		f_c = e_nsm_get_fragment(node, f->blender.control);
		if((type_a == VN_M_FT_TRANSPARENCY || type_b == VN_M_FT_TRANSPARENCY) && type_a != type_b)
		{
			p_shader_fallback_parce(node, fallback, e_nsm_get_fragment_type(node, f->blender.data_a));
			p_shader_fallback_parce(node, fallback, e_nsm_get_fragment_type(node, f->blender.data_b));
			switch(f->blender.type)
			{
				case VN_M_BLEND_FADE :
					fallback->blend_src = GL_SRC_ALPHA;
					fallback->blend_dst = GL_ONE_MINUS_SRC_ALPHA;
					fallback->color[4] = 0.5;
				break;
				case VN_M_BLEND_ADD :
					fallback->blend_src = GL_ZERO;
					fallback->blend_dst = GL_SRC_COLOR;
				break;
				case VN_M_BLEND_MULTIPLY :
					fallback->blend_src = GL_ONE;
					fallback->blend_dst = GL_ONE;
				break;
			}
			return fallback;
		}
	}
	p_shader_fallback_parce(node, fallback, e_nsm_get_fragment_color_front(node));
	return fallback;
}

