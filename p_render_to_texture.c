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

#include "seduce.h"


void (APIENTRY *p_glBindFramebufferEXT)(GLenum target, GLuint framebuffer);
void (APIENTRY *p_glDeleteFramebuffersEXT)(GLsizei n, const GLuint *framebuffers);
void (APIENTRY *p_glGenFramebuffersEXT)(GLsizei n, GLuint *framebuffers);

void (APIENTRY *p_glBindRenderbufferEXT)(GLenum target, GLuint renderbuffer);
void (APIENTRY *p_glDeleteRenderbuffersEXT)(GLsizei n, const GLuint *renderbuffers);
void (APIENTRY *p_glGenRenderbuffersEXT)(GLsizei n, GLuint *renderbuffers);
GLenum (APIENTRY *p_glCheckFramebufferStatusEXT)(GLenum target);
void (APIENTRY *p_glRenderbufferStorageEXT)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);

void (APIENTRY *p_glFramebufferTexture2DEXT)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
void (APIENTRY *p_glFramebufferRenderbufferEXT)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);

typedef struct {
	uint size;
	uint fbo;
	uint depth;
	uint stencil;
} RenderSetup;

static RenderSetup g_global_fbos[2] = {128, -1, -1, -1, 128, -1, -1, -1};

void p_init_render_to_texture(void)
{
	if(p_extension_test("GL_EXT_framebuffer_object"))
	{
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
	}
}

void CheckFramebufferStatus(void)
{
    GLenum status;
    status = (GLenum) p_glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch(status) {
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
     //   default:
       //     assert(0);
    }
	exit(0);
}

void p_texture_render_bind(uint texture, uint size, uint target)
{
	RenderSetup *fbo;
	if(target == GL_TEXTURE_2D)
		fbo = &g_global_fbos[0];
	else
		fbo = &g_global_fbos[1];

	if(fbo->size != size)
	{
		if(fbo->fbo != -1)
			p_glDeleteFramebuffersEXT(1, &fbo->fbo);
		fbo->fbo = -1;
	}
	if(fbo->fbo == -1)
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
*/
	//	p_glCheckFramebufferStatusEXT();
		CheckFramebufferStatus();
	}else
	{
		p_glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo->fbo);    
		p_glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, target, texture, 0);
	}
}

void p_texture_render_unbind(void)
{
	p_glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

egreal p_noise_function(egreal *vec, uint level);

uint p_create_renderable_texture(uint size, uint format)
{
	uint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	{
		float *buffer;
		egreal vec[3] = {0, 0, 0}, f;
		uint i, j; 
		buffer = malloc((sizeof *buffer) * 3 * size * size);
		for(i = 0; i < size; i++)
		{
			for(j = 0; j < size; j++)
			{
				vec[0] = (float)i / (float)size;
				vec[1] = (float)j / (float)size;
				f = p_noise_function(vec, 7);
				f += p_noise_function(vec, 17) * 0.5;
				f += p_noise_function(vec, 31) * 0.25;
				buffer[(i * size + j) * 3 + 0] = f;
				buffer[(i * size + j) * 3 + 1] = f;
				buffer[(i * size + j) * 3 + 2] = f;
			}
		}
		glTexImage2D(GL_TEXTURE_2D, 0, format, size, size, 0, GL_RGB, GL_FLOAT, buffer);
		free(buffer);
	}

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	return texture;
}

static uint my_test_texture = -1; 

void p_pre_fbo_draw(float fov)
{
//	float fov;
	if(my_test_texture == -1)
	{
		p_init_render_to_texture();
		my_test_texture = p_create_renderable_texture(128, GL_RGBA8);
	}
	p_texture_render_bind(my_test_texture, 128, GL_TEXTURE_2D);
	glClearColor(0.1, 0.2, 0.2, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(0.2, 0.2, 0.2, 0);
}

static float color_array[8] = {0, 0, 1, 0, 1, 1, 0, 1};
static float vertex_array[12] = {-0.25, -0.25, -1, 0.25, -0.25, -1, 0.25, 0.25, -1, -0.25, 0.25, -1};

void p_post_fbo_draw(void)
{
	p_texture_render_unbind();
}



