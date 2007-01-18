/*
 * 
*/

#include <stdio.h>

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

#define GL_FRAMEBUFFER_EXT                     0x8D40

#define GL_RENDERBUFFER_EXT                    0x8D41

#define GL_STENCIL_INDEX1_EXT                  0x8D46
#define GL_STENCIL_INDEX4_EXT                  0x8D47
#define GL_STENCIL_INDEX8_EXT                  0x8D48
#define GL_STENCIL_INDEX16_EXT                 0x8D49

#define GL_RENDERBUFFER_WIDTH_EXT              0x8D42
#define GL_RENDERBUFFER_HEIGHT_EXT             0x8D43
#define GL_RENDERBUFFER_INTERNAL_FORMAT_EXT    0x8D44
#define GL_RENDERBUFFER_RED_SIZE_EXT           0x8D50
#define GL_RENDERBUFFER_GREEN_SIZE_EXT         0x8D51
#define GL_RENDERBUFFER_BLUE_SIZE_EXT          0x8D52
#define GL_RENDERBUFFER_ALPHA_SIZE_EXT         0x8D53
#define GL_RENDERBUFFER_DEPTH_SIZE_EXT         0x8D54
#define GL_RENDERBUFFER_STENCIL_SIZE_EXT       0x8D55

#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT            0x8CD0
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT            0x8CD1
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT          0x8CD2
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT  0x8CD3
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT     0x8CD4

#define GL_COLOR_ATTACHMENT0_EXT                0x8CE0
#define GL_DEPTH_ATTACHMENT_EXT                 0x8D00
#define GL_STENCIL_ATTACHMENT_EXT               0x8D20

#define GL_FRAMEBUFFER_COMPLETE_EXT                          0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT             0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT     0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT             0x8CD9
#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT                0x8CDA
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT            0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT            0x8CDC
#define GL_FRAMEBUFFER_UNSUPPORTED_EXT                       0x8CDD

#define GL_FRAMEBUFFER_BINDING_EXT             0x8CA6
#define GL_RENDERBUFFER_BINDING_EXT            0x8CA7
#define GL_MAX_COLOR_ATTACHMENTS_EXT           0x8CDF
#define GL_MAX_RENDERBUFFER_SIZE_EXT           0x84E8


#define GL_DEPTH_COMPONENT24              0x81A6
#define GL_DEPTH_COMPONENT32              0x81A7
#define GL_STENCIL_INDEX8	             0x8D48
#define GL_CLAMP_TO_EDGE                  0x812F

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

#include "enough.h"
#include "st_types.h"
#include "p_task.h"
#include "p_extension.h"

#if defined(__APPLE__) || defined(MACOSX)
	#define APIENTRY
#endif

static void (APIENTRY *p_glBindFramebufferEXT)(GLenum target, GLuint framebuffer) = NULL;
static void (APIENTRY *p_glDeleteFramebuffersEXT)(GLsizei n, const GLuint *framebuffers) = NULL;
static void (APIENTRY *p_glGenFramebuffersEXT)(GLsizei n, GLuint *framebuffers) = NULL;

static void (APIENTRY *p_glBindRenderbufferEXT)(GLenum target, GLuint renderbuffer) = NULL;
static void (APIENTRY *p_glDeleteRenderbuffersEXT)(GLsizei n, const GLuint *renderbuffers) = NULL;
static void (APIENTRY *p_glGenRenderbuffersEXT)(GLsizei n, GLuint *renderbuffers) = NULL;
static GLenum (APIENTRY *p_glCheckFramebufferStatusEXT)(GLenum target) = NULL;
static void (APIENTRY *p_glRenderbufferStorageEXT)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height) = NULL;

static void (APIENTRY *p_glFramebufferTexture2DEXT)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) = NULL;
static void (APIENTRY *p_glFramebufferRenderbufferEXT)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) = NULL;

extern egreal p_noise_function(egreal *vec, uint level);

typedef struct {
	uint size;
	uint fbo;
	uint depth;
	uint stencil;
} RenderSetup;

static RenderSetup g_global_fbos[10];	/* One for TEXTURE_2D, then one for each size in 1..256 range. */

static boolean fbo_supported = FALSE;

boolean p_render_to_texture_supported(void)
{
	return fbo_supported;
}

void p_init_render_to_texture(void)
{
	fbo_supported = FALSE;
	if(1 && p_extension_test("GL_EXT_framebuffer_object"))
	{
		uint	i;

		p_glBindFramebufferEXT = p_extension_get_address("glBindFramebufferEXT");
		p_glDeleteFramebuffersEXT = p_extension_get_address("glDeleteFramebuffersEXT");
		p_glGenFramebuffersEXT = p_extension_get_address("glGenFramebuffersEXT");
		p_glBindRenderbufferEXT = p_extension_get_address("glBindRenderbufferEXT");
		p_glDeleteRenderbuffersEXT = p_extension_get_address("glDeleteRenderbuffersEXT");
		p_glGenRenderbuffersEXT = p_extension_get_address("glGenRenderbuffersEXT");
		p_glCheckFramebufferStatusEXT = p_extension_get_address("glCheckFramebufferStatusEXT");
		p_glRenderbufferStorageEXT = p_extension_get_address("glRenderbufferStorageEXT");
		p_glFramebufferTexture2DEXT = p_extension_get_address("glFramebufferTexture2DEXT");
		p_glFramebufferRenderbufferEXT = p_extension_get_address("glFramebufferRenderbufferEXT");

		/* Initialize statically-sized textures. */
		g_global_fbos[0].size = 128;
		for(i = 1; i < sizeof g_global_fbos / sizeof *g_global_fbos; i++)
		{
			RenderSetup	*su = g_global_fbos + i;

			su->size = 1 << (i - 1);
			su->fbo = su->depth = su->stencil = 0;
		}
		fbo_supported = TRUE;
	}
}

static void p_check_framebuffer_status(void)
{
	GLenum status;

	if(!p_glCheckFramebufferStatusEXT)
		return;

	status = (GLenum) p_glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status)
	{
        case GL_FRAMEBUFFER_COMPLETE_EXT:
	    return;
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            printf("Unsupported framebuffer format\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            printf("Framebuffer incomplete, missing attachment\n");
            break;
/*	case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
            printf("Framebuffer incomplete, duplicate attachment\n");
            break;*/
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            printf("Framebuffer incomplete, attached images must have same dimensions\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            printf("Framebuffer incomplete, attached images must have same format\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            printf("Framebuffer incomplete, missing draw buffer\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            printf("Framebuffer incomplete, missing read buffer\n");
            break;
	}
	fbo_supported = FALSE;
}

/* Map texture size to index into array. This might be slow, but I think we'll manage. */
static int size_to_index(uint size)
{
	switch(size)
	{
	case 1:		return 1;
	case 2:		return 2;
	case 4:		return 3;
	case 8:		return 4;
	case 16:	return 5;
	case 32:	return 6;
	case 64:	return 7;
	case 128:	return 8;
	case 256:	return 9;
	}
	return -1;
}

void p_texture_render_bind(uint texture, uint size, uint target)
{
	RenderSetup *fbo;

	if(!p_glBindFramebufferEXT)
		return;

	if(target == GL_TEXTURE_2D)
		fbo = &g_global_fbos[0];
	else	/* Look up a statically sized slot, based on the requested texture size. */
	{
		int	index = size_to_index(size);

		if(index == -1)
			return;
		fbo = &g_global_fbos[index];
	}

	/* If the above slot-selection code works properly, this part is never needed.
	 * Which is a very good thing, since it causes rendering to flicker on Linux.
	*/
	if(fbo->size != size)
	{
		if(fbo->fbo != 0)
		{
			p_glDeleteFramebuffersEXT(1, &fbo->fbo);
			fbo->fbo = 0;
		}
		if(fbo->depth != 0)
		{
			p_glDeleteRenderbuffersEXT(1, &fbo->depth);
			fbo->depth = 0;
		}
	}
	if(fbo->fbo == 0)
	{
		fbo->size = size;
		p_glGenFramebuffersEXT(1, &fbo->fbo);
		p_glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo->fbo);    

		p_glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texture, 0);

		p_glGenRenderbuffersEXT(1, &fbo->depth);		
		p_glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fbo->depth);
		p_glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, size, size);
		p_glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, fbo->depth);

/*		p_glGenRenderbuffersEXT(1, &p_global_stencil);		
		p_glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, p_global_stencil);
		p_glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_STENCIL_INDEX8, size, size);
		p_glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, p_global_stencil);

	        p_glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, p_global_stencil);
	        p_glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_STENCIL_INDEX, size, size);
		p_glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, p_global_stencil);

		p_glCheckFramebufferStatusEXT();
*/		p_check_framebuffer_status();
	}
	else
	{
		p_glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo->fbo);    
		p_glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texture, 0);
	}
}

void p_texture_render_unbind(void)
{
	if(!p_glBindFramebufferEXT)
		return;
	p_glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	p_glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

uint p_create_renderable_texture(uint size, uint format)
{
	uint	texture;
	float	*buffer;

	glGenTextures(1, &texture);
	if(texture == 0)
		return 0;
	glBindTexture(GL_TEXTURE_2D, texture);
	if((buffer = malloc(3 * size * size * sizeof *buffer)) != NULL)
	{
		egreal	vec[] = { 0, 0, 0 }, f;
		uint	i, j;

		for(i = 0; i < size; i++)
		{
			for(j = 0; j < size; j++)
			{
				vec[0] = (egreal) i / size;
				vec[1] = (egreal) j / size;
				f = p_noise_function(vec, 7);
				f += p_noise_function(vec, 17) * 0.5;
				f += p_noise_function(vec, 31) * 0.25;
				buffer[3 * (i * size + j) + 0] = f;
				buffer[3 * (i * size + j) + 1] = f;
				buffer[3 * (i * size + j) + 2] = f;
			}
		}
		glTexImage2D(GL_TEXTURE_2D, 0, format, size, size, 0, GL_RGB, GL_FLOAT, buffer);
		free(buffer);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else
	{
		glDeleteTextures(1, &texture);
		texture = 0;
	}
	return texture;
}
