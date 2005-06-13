#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
	#include <windows.h>
	#include <GL/gl.h>
#else
	#include <GL/gl.h>
#define APIENTRY
#endif
#include "verse.h"
#include "persuade.h"
#include "p_task.h"
#include "p_extension.h"
#include "p_sds_array.h"

/*typedef unsigned int GLhandleARB;*/

#define GL_VERTEX_SHADER_ARB                        0x8B31
#define GL_FRAGMENT_SHADER_ARB                      0x8B30
#define GL_TEXTURE0_ARB								0x84C0

GLhandleARB (APIENTRY *p_glCreateShaderObjectARB)(GLenum shaderType);
GLvoid      (APIENTRY *p_glDeleteObjectARB)(GLhandleARB obj);
GLhandleARB (APIENTRY *p_glCreateProgramObjectARB)(GLvoid);
GLvoid      (APIENTRY *p_glAttachObjectARB)(GLhandleARB containerObj, GLhandleARB obj);
GLvoid      (APIENTRY *p_glShaderSourceARB)(GLhandleARB shaderObj, GLsizei count, const char **string, const GLint *length);
GLvoid      (APIENTRY *p_glCompileShaderARB)(GLhandleARB shaderObj);
GLvoid      (APIENTRY *p_glLinkProgramARB)(GLhandleARB programObj);
GLvoid      (APIENTRY *p_glUseProgramObjectARB)(GLhandleARB programObj);
GLint       (APIENTRY *p_glGetUniformLocationARB)(GLhandleARB programObj, const char *name);
GLvoid      (APIENTRY *p_glUniform4fARB)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
GLvoid      (APIENTRY *p_glUniform1iARB)(GLint location, GLint i);
GLvoid		(APIENTRY *p_glUniformMatrix4fvARB)(GLint location, GLsizei count, GLboolean transpose, GLfloat *value);
GLint       (APIENTRY *p_glGetAttribLocationARB)(GLhandleARB programObj, const char *name);
GLvoid		(APIENTRY *p_glVertexAttrib4fARB)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
GLvoid		(APIENTRY *p_glActiveTextureARB)(GLenum texture);

GLvoid		(APIENTRY *p_glBindAttribLocationARB)(GLhandleARB programObj, GLuint index, const char *name);
GLvoid		(APIENTRY *p_glVertexAttribPointerARB)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
GLvoid		(APIENTRY *p_glEnableVertexAttribArrayARB)(GLuint index);
GLvoid		(APIENTRY *p_glDisableVertexAttribArrayARB)(GLuint index);

GLvoid		(APIENTRY *p_glGetInfoLogARB)(GLhandleARB object, GLsizei maxLenght, GLsizei *length, char *infoLog);

extern void p_shader_fallback_func(ENode *node, ECustomDataCommand command);
extern void p_shader_bind_fallback(uint32 node_id);

typedef struct{
	uint			fragment;
	PTextureHandle	*handle;
	uint			location;
}PShaderTexture;

typedef struct{
	uint		data_version;
	uint		struct_version;
	uint		prog_obj;
	uint		vertex_obj;
	uint		fragment_obj;
	uint		*uniforms_fragments;
	uint		u_f_count;
	PShaderTexture *texture_fragments;
	uint		t_f_count;
	uint		*geometry_fragments;
	uint		g_f_count;
	uint		environment[2];
	void		*temp;
}PShader;

typedef struct{
	PShader		*s;
	void		*writer_temp;
	char		*v_shader;
	uint		v_length; 
	char		*f_shader;
	uint		f_length; 
	uint		stage;
}PShaderGenTemp;

PShader *p_standard_shader;
boolean p_programmable_shaders_suported = FALSE;

char p_standard_fragment_shader_code[256] = "void main(void)"
"{"
"	gl_FragColor = vec4(0.2, 0.6, 0.9, 1);"
"}";

char p_standard_vertex_shader_code[256] = "void main(void)"
"{" 
"	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
"}";

PShader *p_shader_allocate()
{
	PShader *s;
	s = malloc(sizeof *s);
    s->prog_obj = p_glCreateProgramObjectARB();
	s->vertex_obj = p_glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    s->fragment_obj = p_glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	p_glAttachObjectARB(s->prog_obj, s->vertex_obj);
	p_glAttachObjectARB(s->prog_obj, s->fragment_obj);
	s->uniforms_fragments = NULL;
	s->u_f_count = 0;
	s->texture_fragments = NULL;
	s->t_f_count = 0;
	s->geometry_fragments = NULL;
	s->g_f_count = 0;
	s->data_version = 0;
	s->struct_version = 0;
	s->temp = NULL;
	return s;
}

extern void p_shader_func(ENode *node, ECustomDataCommand command);

void p_shader_init()
{
//	uint length;
	if(/*p_extention_test("GL_ARB_shading_language_100")*/FALSE)
	{
		p_glCreateShaderObjectARB = p_extention_get_address("glCreateShaderObjectARB");
		p_glDeleteObjectARB = p_extention_get_address("glDeleteObjectARB");
		p_glCreateProgramObjectARB = p_extention_get_address("glCreateProgramObjectARB");
		p_glAttachObjectARB = p_extention_get_address("glAttachObjectARB");
		p_glShaderSourceARB = p_extention_get_address("glShaderSourceARB");
		p_glCompileShaderARB = p_extention_get_address("glCompileShaderARB");
		p_glLinkProgramARB = p_extention_get_address("glLinkProgramARB");
		p_glUseProgramObjectARB = p_extention_get_address("glUseProgramObjectARB");

		p_glGetUniformLocationARB = p_extention_get_address("glGetUniformLocationARB");
		p_glUniform4fARB = p_extention_get_address("glUniform4fARB");
		p_glUniform1iARB = p_extention_get_address("glUniform1iARB");
		p_glUniformMatrix4fvARB = p_extention_get_address("glUniformMatrix4fvARB");
		p_glBindAttribLocationARB = p_extention_get_address("glBindAttribLocationARB");
		p_glGetAttribLocationARB = p_extention_get_address("glGetAttribLocationARB");
		p_glVertexAttrib4fARB = p_extention_get_address("glVertexAttrib4fARB");
		p_glActiveTextureARB = p_extention_get_address("glActiveTextureARB");

		p_glGetInfoLogARB = p_extention_get_address("glGetInfoLogARB");
		p_programmable_shaders_suported = TRUE;

		p_standard_shader = p_shader_allocate();
		e_ns_set_custom_func(P_ENOUGH_SLOT, V_NT_MATERIAL, p_shader_func);
	}else
	{
		p_programmable_shaders_suported = FALSE;
		e_ns_set_custom_func(P_ENOUGH_SLOT, V_NT_MATERIAL, p_shader_fallback_func);
	}
}
void p_shader_destroy(PShader *shader)
{
	if(shader->prog_obj != p_standard_shader->prog_obj)
	{
		p_glDeleteObjectARB(shader->vertex_obj);
		p_glDeleteObjectARB(shader->fragment_obj);
		p_glDeleteObjectARB(shader->prog_obj);
		if(shader->uniforms_fragments != NULL)
			free(shader->uniforms_fragments);
		if(shader->texture_fragments != NULL)
			free(shader->texture_fragments);
		if(shader->geometry_fragments != NULL)
			free(shader->geometry_fragments);
	}
	free(shader);
}

void p_shader_uniform_add(PShader *s, uint fragment_id, uint locator_id)
{
	if(s->u_f_count % 16 == 0)
		s->uniforms_fragments = realloc(s->uniforms_fragments, (sizeof *s->uniforms_fragments) * 2 * (s->u_f_count + 16));
	s->uniforms_fragments[s->u_f_count * 2] = fragment_id;
	s->uniforms_fragments[s->u_f_count * 2 + 1] = locator_id;
	s->u_f_count++;
}

void p_shader_texture_add(PShader *s, uint fragment_id, uint locator_id, PTextureHandle *handle)
{
	if(s->t_f_count % 16 == 0)
		s->texture_fragments = realloc(s->texture_fragments, (sizeof *s->texture_fragments) * (s->t_f_count + 16));
	s->texture_fragments[s->t_f_count].fragment = fragment_id;
	s->texture_fragments[s->t_f_count].handle = handle;
	s->texture_fragments[s->t_f_count].location = locator_id;
	s->t_f_count++;
}

void p_shader_geometry_add(PShader *s, uint fragment_id, uint locator_id)
{
	if(s->g_f_count % 16 == 0)
		s->geometry_fragments = realloc(s->geometry_fragments, (sizeof *s->geometry_fragments) * 2 * (s->g_f_count + 16));
	s->geometry_fragments[s->g_f_count * 2] = fragment_id;
	s->geometry_fragments[s->g_f_count * 2 + 1] = locator_id;
	s->g_f_count++;
}

void p_shader_uniform_locate(PShader *s, ENode *node)
{
	uint16 frag;
	VMatFrag *f;
	char frag_name[64];
	for(frag = e_nsm_get_fragment_next(node, 0); frag != (uint16)-1; frag = e_nsm_get_fragment_next(node, frag + 1))
	{
		switch(e_nsm_get_fragment_type(node, frag))
		{
			case VN_M_FT_COLOR :
				f = e_nsm_get_fragment(node, frag);
				sprintf(frag_name, "color_%u", frag);
				p_shader_uniform_add(s, frag, p_glGetUniformLocationARB(s->prog_obj, frag_name));
				break;
			case VN_M_FT_ANIMATION :
				f = e_nsm_get_fragment(node, frag);
				sprintf(frag_name, "anim_%u", frag);
				p_shader_uniform_add(s, frag, p_glGetUniformLocationARB(s->prog_obj, frag_name));
				break;
			case VN_M_FT_MATRIX :
				f = e_nsm_get_fragment(node, frag);
				sprintf(frag_name, "matrix_%u", frag);
				p_shader_uniform_add(s, frag, p_glGetUniformLocationARB(s->prog_obj, frag_name));
				break;
			case VN_M_FT_GEOMETRY :
				f = e_nsm_get_fragment(node, frag);
				sprintf(frag_name, "attrib_geometry_%u", frag);
				p_shader_geometry_add(s, frag, p_glGetAttribLocationARB(s->prog_obj, frag_name));
				break;
			case VN_M_FT_TEXTURE :
				f = e_nsm_get_fragment(node, frag);
				sprintf(frag_name, "texture_%u", frag);
				p_shader_texture_add(s, frag, p_glGetUniformLocationARB(s->prog_obj, frag_name), 
					p_th_create_texture_handle(f->texture.bitmap, f->texture.layer_r, f->texture.layer_g, f->texture.layer_b));
				break;
		}
	}
	s->environment[0] = p_glGetUniformLocationARB(s->prog_obj, "environment");
	s->environment[1] = p_glGetUniformLocationARB(s->prog_obj, "diffuse_environment");
}
	/*
extern PTextureHandle *p_th_create_texture_handle(uint node_id, char *layer_r, char *layer_g, char *layer_b);
extern void		p_th_destroy_texture_handle(PTextureHandle *handle);
extern uint		p_th_get_texture_id(PTextureHandle *handle);


int my_vec3_location = glGetAttribLocationARB(my_program, “my_3d_vector”);

glBegin(GL_TRIANGLES);
	glVertexAttrib3f(my_vec3_location, 4.0f, 2.0f, 7.0f);
-----------------
int my_sampler_uniform_location = glGetUniformLocationARB(my_program, “my_color_texture”);

glActiveTexture(GL_TEXTURE0 + i);
glBindTexture(GL_TEXTURE_2D, my_texture_object);

glUniform1iARB(my_sampler_uniform_location, i);

*/

#define GL_TEXTURE_CUBE_MAP_EXT             0x8513 

void p_shader_param_load(ENode *parent, uint32 node_id, PRenderArray *array, uint count, uint environment, uint diffuse_environment)
{
	ENode *node;
	PShader *s;
	uint i, j;
	uint16 frag;
	VMatFrag *f;
	float m[16];
	if(!p_programmable_shaders_suported)
		return;
	if((node = e_ns_get_node(0, node_id)) == NULL || V_NT_MATERIAL != e_ns_get_node_type(node))
		return;
	s = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
	for(i = 0; i < s->u_f_count; i++)
	{
		frag = s->uniforms_fragments[i * 2];
		f = e_nsm_get_fragment(node, frag);
		switch(e_nsm_get_fragment_type(node, frag))
		{
			case VN_M_FT_COLOR :
				p_glUniform4fARB(s->uniforms_fragments[i * 2 + 1], (float)f->color.red, (float)f->color.green, (float)f->color.blue, 1.0);
				break;
			case VN_M_FT_ANIMATION :
				p_glUniform4fARB(s->uniforms_fragments[i * 2 + 1], (float)1.0, (float)1.0, (float)1.0, 1.0);
				break;
			case VN_M_FT_MATRIX :
				for(j = 0; j < 16; j++)
					m[j] = (float)f->matrix.matrix[j];
				p_glUniformMatrix4fvARB(s->uniforms_fragments[i * 2 + 1], 1, FALSE, m);
				break;
		}
	}
	for(i = 0; i < s->t_f_count; i++)
	{
		p_glActiveTextureARB(GL_TEXTURE0_ARB + i);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, p_th_get_texture_id(s->texture_fragments[i].handle));
		p_glUniform1iARB(s->texture_fragments[i].location, i);
	}
	p_glActiveTextureARB(GL_TEXTURE0_ARB + i);
	glEnable(GL_TEXTURE_CUBE_MAP_EXT);
	glBindTexture(GL_TEXTURE_CUBE_MAP_EXT, environment);
	p_glUniform1iARB(s->environment[0], i);
	i++;
	p_glActiveTextureARB(GL_TEXTURE0_ARB + i);
	glEnable(GL_TEXTURE_CUBE_MAP_EXT);
	glBindTexture(GL_TEXTURE_CUBE_MAP_EXT, diffuse_environment);
	p_glUniform1iARB(s->environment[1], i);
	for(i = 0; i < s->g_f_count && i < count; i++)
		p_ra_bind_param_array(&array[i], s->geometry_fragments[i * 2 + 1]);
}


uint p_shader_get_param_count(ENode *node)
{
	if(p_programmable_shaders_suported)
	{
		PShader *s;
		s = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
		return s->g_f_count;
	}else 
		return 0;
}
VMatFrag *p_shader_get_param(ENode *node, uint nr)
{
	if(p_programmable_shaders_suported)
	{
		PShader *s;
		s = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
		return e_nsm_get_fragment(node, s->geometry_fragments[nr * 2]);
	}else 
		return NULL;
}


void p_shader_bind(uint32 node_id)
{
	if(p_programmable_shaders_suported)
	{
		PShader *s;
		ENode *node;
		s = p_standard_shader;
		node = e_ns_get_node(0, node_id);
		if(node != NULL && V_NT_MATERIAL == e_ns_get_node_type(node))
			s = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
		p_glUseProgramObjectARB(s->prog_obj);
	}else
		p_shader_bind_fallback(node_id);
}
void p_shader_unbind(uint32 node_id)
{
	if(p_programmable_shaders_suported)
	{
		PShader *s;
		ENode *node;
		uint i;
		node = e_ns_get_node(0, node_id);
		if(node != NULL && V_NT_MATERIAL == e_ns_get_node_type(node))
		{
			s = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
			for(i = 0; i < s->g_f_count; i++)
				p_glDisableVertexAttribArrayARB(s->geometry_fragments[i * 2 + 1]);

			for(i = 0; i < s->t_f_count; i++)
			{
				p_glActiveTextureARB(GL_TEXTURE0_ARB + i);
				glDisable(GL_TEXTURE_2D);
			}
			p_glActiveTextureARB(GL_TEXTURE0_ARB + i);
			glDisable(GL_TEXTURE_CUBE_MAP_EXT);
			i++;
			p_glActiveTextureARB(GL_TEXTURE0_ARB + i);
			glDisable(GL_TEXTURE_CUBE_MAP_EXT);
			p_glActiveTextureARB(GL_TEXTURE0_ARB);
		}
		p_glUseProgramObjectARB(0);
	}
}



extern void *p_shader_write(ENode *node, char **v_code, uint *v_length, char **f_code, uint *f_length, void *t);
extern void p_shader_write_destroy_temp(void *t);

boolean p_shader_compute(uint node_id)
{
	PShaderGenTemp *t;
	PShader *shader;
	ENode *node;
	if((node = e_ns_get_node(0, node_id)) == NULL || V_NT_MATERIAL != e_ns_get_node_type(node))
		return TRUE;

	shader = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
	if(shader->struct_version == e_ns_get_node_version_struct(node))
		return TRUE;

	t = shader->temp;

	if(e_nsm_get_fragment(node, e_nsm_get_fragment_color_front(node)) == NULL)
	{
		if(t->stage > 0)
			p_shader_destroy(t->s);
		if(t->writer_temp != NULL)
			p_shader_write_destroy_temp(t->writer_temp);
		p_shader_destroy(shader);
		shader = malloc(sizeof *shader);
		*shader = *p_standard_shader;
		e_ns_set_custom_data(node, P_ENOUGH_SLOT, shader);
		free(t);
		return TRUE;
	}

	switch(t->stage)
	{
		case 0 :
			t->s = p_shader_allocate();
			t->s->struct_version = e_ns_get_node_version_struct(node);
			t->stage = 1;
		break;
		case 1 :
			if(t->s->struct_version != e_ns_get_node_version_struct(node) && t->writer_temp != NULL)
			{
				t->s->struct_version = e_ns_get_node_version_struct(node);
				p_shader_write_destroy_temp(t->writer_temp);
				t->writer_temp = NULL;
			}
			t->writer_temp = p_shader_write(node, &t->v_shader, &t->v_length, &t->f_shader, &t->f_length, t->writer_temp);
			if(t->writer_temp == NULL)
				t->stage = 2;

		break;
		case 2 :
			printf("\n%s\n", t->f_shader);
			printf("\n%s\n", t->v_shader);

			p_glShaderSourceARB(t->s->vertex_obj, 1, &t->v_shader, &t->v_length);
			p_glCompileShaderARB(t->s->vertex_obj);
			{
				char buf[2000];
				GLsizei length;
				p_glGetInfoLogARB(t->s->vertex_obj, 2000, &length, buf);
				printf("Errors:\n%s\n", buf);
			}
			t->stage = 3;
		break;
		case 3 :
			p_glShaderSourceARB(t->s->fragment_obj, 1, &t->f_shader, &t->f_length);
			p_glCompileShaderARB(t->s->fragment_obj);
			{
				char buf[2000];
				GLsizei length;
				p_glGetInfoLogARB(t->s->fragment_obj, 2000, &length, buf);
				printf("Errors:\n%s\n", buf);
			}
			t->stage = 4;
		break;
		case 4 :
			p_glLinkProgramARB(t->s->prog_obj);
			{
				char buf[2000];
				GLsizei length;
				p_glGetInfoLogARB(t->s->prog_obj, 2000, &length, buf);
				printf("Errors:\n%s\n", buf);
			}
			t->stage = 5;
		break;
		case 5 :
			if(t->s->struct_version != e_ns_get_node_version_struct(node))
			{
				t->s->struct_version = e_ns_get_node_version_struct(node);
				t->stage = 1;
			}else
			{
				p_shader_uniform_locate(t->s, node);
				t->stage = 6;
			}
		break;
		case 6 :
			p_shader_destroy(shader);
			e_ns_set_custom_data(node, P_ENOUGH_SLOT, t->s);
			free(t);
		return TRUE;
	}
	return FALSE;
}

void p_shader_func(ENode *node, ECustomDataCommand command)
{	
	PShaderGenTemp *t;
	PShader *s;
	switch(command)
	{
		case E_CDC_CREATE :
			s = malloc(sizeof *s);
			*s = *p_standard_shader;
			e_ns_set_custom_data(node, P_ENOUGH_SLOT, s);
		break;
		case E_CDC_STRUCT :
			s = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
			if(s->temp == NULL)
			{
				t = malloc(sizeof *t);
				t->writer_temp = NULL;
				t->stage = 0;
				s->temp = t;
				p_task_add(e_ns_get_node_id(node), 1, p_shader_compute);
			}
		break;
		case E_CDC_DESTROY :
			p_shader_destroy(e_ns_get_custom_data(node, P_ENOUGH_SLOT));
		break;
	}
}

