#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
	#include <windows.h>
	#include <GL/gl.h>
#else
	#include <GL/gl.h>
#endif
#include "verse.h"
#include "persuade.h"
#include "p_task.h"

#include "p_sds_array.h"
#include "p_extension.h"


GLvoid (APIENTRY *p_glVertexAttribPointerARB)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
GLvoid (APIENTRY *p_glEnableVertexAttribArrayARB)(GLuint index);
GLvoid (APIENTRY *p_glDisableVertexAttribArrayARB)(GLuint index);


GLvoid (APIENTRY *p_glBindBufferARB)(GLenum target, uint buffer);
GLvoid (APIENTRY *p_glGenBuffersARB)(uint n, uint *buffers);
GLvoid (APIENTRY *p_glDeleteBuffersARB)(uint n, const uint *buffers);
GLvoid (APIENTRY *p_glBufferDataARB)(GLenum target, uint size, const void *data, GLenum usage);
GLvoid *(APIENTRY *p_glMapBufferARB)(GLenum target, GLenum access);
GLvoid (APIENTRY *p_glUnmapBufferARB)(GLenum target);


#define GL_ARRAY_BUFFER_ARB                             0x8892
#define GL_ELEMENT_ARRAY_BUFFER_ARB                     0x8893
#define GL_DYNAMIC_DRAW_ARB                             0x88E8
#define GL_WRITE_ONLY_ARB                               0x88B9
#define GL_BUFFER_MAP_POINTER_ARB                       0x88BD
#define GL_STATIC_DRAW_ARB                              0x88E4

boolean use_vertex_buffer_object = FALSE;

void p_array_init()
{
	p_glVertexAttribPointerARB = p_extension_get_address("glVertexAttribPointerARB");
	p_glEnableVertexAttribArrayARB = p_extension_get_address("glEnableVertexAttribArrayARB");
	p_glDisableVertexAttribArrayARB = p_extension_get_address("glDisableVertexAttribArrayARB");

/*	if(p_extension_test("GL_ARB_vertex_buffer_object"))
	{
		use_vertex_buffer_object = TRUE;
	
		p_glGenBuffersARB = p_extension_get_address("glGenBuffersARB");
		p_glDeleteBuffersARB = p_extension_get_address("glDeleteBuffersARB");
		p_glBindBufferARB = p_extension_get_address("glBindBufferARB");
		p_glBufferDataARB = p_extension_get_address("glBufferDataARB");
		p_glMapBufferARB = p_extension_get_address("glMapBufferARB");
		p_glUnmapBufferARB = p_extension_get_address("glUnmapBufferARB");
	}*/
}

egreal *p_ra_get_array_real(PRenderArray *array, uint length)
{
	if(array->length == 0)
	{
		if(use_vertex_buffer_object)
		{
			p_glGenBuffersARB(1, &array->array.buffer_id);
			p_glBindBufferARB(GL_ARRAY_BUFFER_ARB, array->array.buffer_id);
			p_glBufferDataARB(GL_ARRAY_BUFFER_ARB, length * 3 * sizeof(egreal), NULL, GL_DYNAMIC_DRAW_ARB);
		}else
			array->array.real = malloc((sizeof *array->array.real) * length * 3);
		array->length = length;
	}
	if(use_vertex_buffer_object)
	{
		p_glBindBufferARB(GL_ARRAY_BUFFER_ARB, array->array.buffer_id);
		return p_glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
	}else
		return array->array.real;
}

uint *p_ra_get_array_integer(PRenderArray *array, uint length)
{
	if(array->length == 0)
	{
		if(use_vertex_buffer_object)
		{
			p_glGenBuffersARB(1, &array->array.buffer_id);
			p_glBindBufferARB(GL_ARRAY_BUFFER_ARB, array->array.buffer_id);
			p_glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, length * sizeof(uint), NULL, GL_STATIC_DRAW_ARB);
		}else
			array->array.integer = malloc((sizeof *array->array.integer) * length);
		array->length = length;
	}
	if(use_vertex_buffer_object)
	{
		p_glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, array->array.buffer_id);
		return p_glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
	}else
		return array->array.integer;
}


void p_ra_set_array_real(PRenderArray *array, egreal *data, uint length)
{
/*	uint i;
	for(i = 0; i < length * 3 ; i++)
		data[i] = (egreal)(i % 7) / 6.0;
*/
	if(use_vertex_buffer_object)
		p_glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
}

void p_ra_set_array_integer(PRenderArray *array, uint *data, uint length)
{
	if(use_vertex_buffer_object)
		p_glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB);
}

void p_ra_clear_array(PRenderArray *array)
{
	array->length = 0;
}

void p_ra_free_array(PRenderArray *array)
{
	if(array->length != 0)
	{
		if(use_vertex_buffer_object)
			p_glDeleteBuffersARB(1, &array->array.buffer_id);
		else
			free(array->array.real);
	}
	array->length = 0;
}

void p_ra_bind_vertex_array(PRenderArray *array)
{
	void *p = NULL;
	if(use_vertex_buffer_object)
		p_glBindBufferARB(GL_ARRAY_BUFFER_ARB, array->array.buffer_id);
	else
		p = array->array.real;
	glEnableClientState(GL_VERTEX_ARRAY);
	#ifdef E_GEOMETRY_REAL_PRECISION_64_BIT
	glVertexPointer(3, GL_DOUBLE, 0, p);
	#endif
	#ifdef E_GEOMETRY_REAL_PRECISION_32_BIT
	glVertexPointer(3, GL_FLOAT, 0, p);
	#endif
}

void p_ra_bind_normal_array(PRenderArray *array)
{
	void *p = NULL;
	if(use_vertex_buffer_object)
		p_glBindBufferARB(GL_ARRAY_BUFFER_ARB, array->array.buffer_id);
	else
		p = array->array.real;
	#ifdef E_GEOMETRY_REAL_PRECISION_64_BIT
	glNormalPointer(GL_DOUBLE, 0, p);
	#endif
	#ifdef E_GEOMETRY_REAL_PRECISION_32_BIT
	glNormalPointer(GL_FLOAT, 0, p);
	#endif
}

void p_ra_bind_param_array(PRenderArray *array, uint id)
{
	void *p = NULL;
	if(use_vertex_buffer_object)
		p_glBindBufferARB(GL_ARRAY_BUFFER_ARB, array->array.buffer_id);
	else
		p = array->array.real;
	p_glEnableVertexAttribArrayARB(id);
	#ifdef E_GEOMETRY_REAL_PRECISION_32_BIT
	p_glVertexAttribPointerARB(id, 3, GL_FLOAT, FALSE, 0, p);
	#endif
		#ifdef E_GEOMETRY_REAL_PRECISION_64_BIT
	p_glVertexAttribPointerARB(id, 3, GL_DOUBLE, FALSE, 0, p);
	#endif
}

void p_ra_unbind_param_array(uint count)
{
	uint i;
	for(i = 0; i < count; i++)
		p_glDisableVertexAttribArrayARB(i);
}

void p_ra_bind_uv_array(PRenderArray *array)
{
	void *p = NULL;
	if(use_vertex_buffer_object)
		p_glBindBufferARB(GL_ARRAY_BUFFER_ARB, array->array.buffer_id);
	else
		p = array->array.real;
	#ifdef E_GEOMETRY_REAL_PRECISION_64_BIT
	glTexCoordPointer(3, GL_DOUBLE, 0, p);
	#endif
	#ifdef E_GEOMETRY_REAL_PRECISION_32_BIT
	glTexCoordPointer(3, GL_FLOAT, 0, p);
	#endif
}

void p_ra_bind_color_array(PRenderArray *array)
{
	void *p = NULL;
	if(use_vertex_buffer_object)
		p_glBindBufferARB(GL_ARRAY_BUFFER_ARB, array->array.buffer_id);
	else
		p = array->array.real;
	#ifdef E_GEOMETRY_REAL_PRECISION_64_BIT
	glColorPointer (3, GL_DOUBLE, 0, p);
	#endif
	#ifdef E_GEOMETRY_REAL_PRECISION_32_BIT
	glColorPointer (3, GL_FLOAT, 0, p);
	#endif
}

uint *p_current_reference;

void p_ra_bind_reference_array(PRenderArray *array)
{
	void *p = NULL;
	if(use_vertex_buffer_object)
	{
		p_glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, array->array.buffer_id);
		p_current_reference = 0;
	}
	else
		p = array->array.real;
}

void p_ra_draw(uint start, uint end)
{
	glDrawElements(GL_TRIANGLES, end - start, GL_UNSIGNED_INT, &p_current_reference[start]);
}
