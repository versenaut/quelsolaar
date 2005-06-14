#ifdef WIN32
	#include <windows.h>
	#include <GL/gl.h>
#else
	#include <GL/gl.h>
#endif

#include "enough.h"
#include "st_types.h"
#include "p_task.h"
#include "p_extension.h"

#define TEXTURE_DEATH_COUNT 20
#define STD_TEXTURE_SIZE 16
#define PIXELS_PER_UPDATE 4096

#define GL_RGBA_FLOAT32                 0x8814
#define GL_RGB_FLOAT32                  0x8815

typedef struct{
	uint	texture_id;
	uint	node_id;
	char	layer_r[16];
	char	layer_g[16];
	char	layer_b[16];
	uint	layer_version[3];
	uint	size_version;
	uint	node_version;
	uint	size[3];
	uint	update_position;
	uint	users;
	uint	dead;
}PTextureHandle;

struct{
	PTextureHandle	**handels;
	uint			handel_count;
	uint			std_texture_id;
	uint			update_pos;
	boolean			use32bit;
	uint			floating_point_enum;
}PTextureStorage;

uint p_th_create_std_texture(void)
{
	uint i, j, texture;
	float *buf;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	buf = malloc((sizeof *buf) * STD_TEXTURE_SIZE * STD_TEXTURE_SIZE * 3);
	for(i = 0; i < STD_TEXTURE_SIZE; i++)
	{	
		for(j = 0; j < STD_TEXTURE_SIZE; j++)
		{
			if((i + j) % 2)
			{
				buf[(STD_TEXTURE_SIZE * i + j) * 3 + 0] = (float)i / STD_TEXTURE_SIZE;
				buf[(STD_TEXTURE_SIZE * i + j) * 3 + 1] = 0;
				buf[(STD_TEXTURE_SIZE * i + j) * 3 + 2] = (float)j / STD_TEXTURE_SIZE;
			}else
			{
				buf[(STD_TEXTURE_SIZE * i + j) * 3 + 0] = 0;
				buf[(STD_TEXTURE_SIZE * i + j) * 3 + 1] = 1 - ((float)j / STD_TEXTURE_SIZE);
				buf[(STD_TEXTURE_SIZE * i + j) * 3 + 2] = 1 - ((float)i / STD_TEXTURE_SIZE);
			}
		} 
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, STD_TEXTURE_SIZE, STD_TEXTURE_SIZE, 0, GL_RGB, GL_FLOAT, buf);
	return texture;
}

void p_th_init(void)
{
	uint i;
	PTextureStorage.handel_count = 64;
	PTextureStorage.handels = malloc((sizeof *PTextureStorage.handels) * PTextureStorage.handel_count);
	for(i = 0; i < PTextureStorage.handel_count; i++)
		PTextureStorage.handels[i] = NULL;
	PTextureStorage.std_texture_id = p_th_create_std_texture();
	PTextureStorage.use32bit = TRUE;
	if(p_extention_test("ATI_texture_float"))
		PTextureStorage.floating_point_enum = GL_RGB_FLOAT32;
	else
		PTextureStorage.floating_point_enum = GL_RGB;
}

void p_th_texture_restart(void)
{
	uint i, texture;
	texture = p_th_create_std_texture();
	for(i = 0; i < PTextureStorage.handel_count; i++)
	{
		if(PTextureStorage.handels[i] != NULL)
		{
			if(PTextureStorage.handels[i]->texture_id == PTextureStorage.std_texture_id)
				PTextureStorage.handels[i]->texture_id = texture;
			PTextureStorage.handels[i]->size_version++;
		}
	}
	glDeleteTextures(1, &PTextureStorage.std_texture_id);
	PTextureStorage.std_texture_id = texture;
}

uint p_th_compute_size_check_sum(uint *size)
{
	return size[0] * 17 + size[1] * 23 + size[2];
}

void p_th_create_new_texture(ENode *node, PTextureHandle *handle)
{
	uint i;
	uint8 *buf;
	e_nsb_get_size(node, &handle->size[0], &handle->size[1], &handle->size[2]);
	handle->size_version = p_th_compute_size_check_sum(handle->size);
	if(handle->size[0] * handle->size[1] * handle->size[2] != 0)
	{
		glGenTextures(1, &handle->texture_id);
		if(handle->size[2] != 1 && FALSE /* we do suport 3d textures*/)
		{
			if(TRUE)/* we dont handle any sized textures*/
			{
				for(i = 1; i * 2 - 1 < handle->size[0] || i * 2 - 1 < handle->size[1] || i * 2 - 1 < handle->size[2]; i *= 2);
				handle->size[0] = i;
				handle->size[1] = i;
				handle->size[2] = i;
			}
	/*		glBindTexture(GL_TEXTURE_3D, handle->texture_id);
			glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			buf = malloc((sizeof *buf) * handle->size[0] * handle->size[1] * handle->size[2] * 3);
			for(i = 0; i < handle->size[0] * handle->size[1] * handle->size[2] * 3; i++)
				buf = 0;
			glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, handle->size[0], handle->size[1], handle->size[2], 0, GL_RGB, GL_UNSIGNED_BYTE, buf);
			free(buf);*/
		}else
		{
			handle->size[2] = 1;
			if(TRUE)/* we dont handle any sized textures*/
			{
				for(i = 1; i * 2 - 1 < handle->size[0] || i * 2 - 1 < handle->size[1]; i *= 2);
				handle->size[0] = i;
				handle->size[1] = i;
			}
			glGenTextures(1, &handle->texture_id);
			glBindTexture(GL_TEXTURE_2D, handle->texture_id);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			buf = malloc((sizeof *buf) * handle->size[0] * handle->size[1] * 3);
			for(i = 0; i < handle->size[0] * handle->size[1] * 3; i++)
				buf[i] = i % 256;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, handle->size[0], handle->size[1], 0, GL_RGB, GL_UNSIGNED_BYTE, buf);
			free(buf);
		}
	}
}


PTextureHandle *p_th_create_texture_handle(uint node_id, char *layer_r, char *layer_g, char *layer_b)
{
	uint i, j, empty = -1;
	PTextureHandle *h; 
	for(i = 0; i < PTextureStorage.handel_count; i++)
	{
		h = PTextureStorage.handels[i];
		if(h == NULL)
		{
			if(empty == -1)
				empty = i;
		}else
		{
			if(h->node_id == node_id)
			{
				
				for(j = 0; j < 15 && layer_r[j] == h->layer_r[j] && layer_r[j] != 0; j++);
				if(h->layer_r[j] == 0)
				{
					for(j = 0; j < 15 && layer_g[j] == h->layer_g[j] && layer_g[j] != 0; j++);
					if(h->layer_g[j] == 0)
					{
						for(j = 0; j < 15 && layer_b[j] == h->layer_b[j] && layer_b[j] != 0; j++);
						if(h->layer_b[j] == 0)
						{ 
							h->users++;
							h->dead = TEXTURE_DEATH_COUNT;
							return h;
						}
					}
				}
			}
		}
	}
	if(empty == -1)
	{
		PTextureStorage.handels = realloc(PTextureStorage.handels, (sizeof *PTextureStorage.handels) * (PTextureStorage.handel_count + 16));
		for(i = 0; i < 16; i++)
			PTextureStorage.handels[PTextureStorage.handel_count + i] = NULL;
		empty = PTextureStorage.handel_count;
		PTextureStorage.handel_count += 16;
	}
	h = PTextureStorage.handels[empty] = malloc(sizeof **PTextureStorage.handels);
	h->dead = TEXTURE_DEATH_COUNT;
	h->users = 1;
	h->node_id = node_id;
	for(i = 0; i < 15 && layer_r[i] != 0; i++)
		h->layer_r[i] = layer_r[i];
	h->layer_r[i] = 0;
	for(i = 0; i < 15 && layer_g[i] != 0; i++)
		h->layer_g[i] = layer_g[i];
	h->layer_g[i] = 0;
	for(i = 0; i < 15 && layer_b[i] != 0; i++)
		h->layer_b[i] = layer_b[i];
	h->layer_b[i] = 0;
	h->size[0] = 0;
	h->size[1] = 0;
	h->size[2] = 0;
	h->update_position = 0;
	for(i = 0; i < 3; i++)
		h->layer_version[i] = -1;

	h->texture_id = PTextureStorage.std_texture_id;
	return h;
}

void p_th_destroy_texture_handle(PTextureHandle *handle){
	handle->users--;
}

uint p_th_get_texture_id(PTextureHandle *handle)
{
	return handle->texture_id;
}

uint p_th_get_texture_dimentions(PTextureHandle *handle)
{
	return GL_TEXTURE_2D;
}


/*
typedef void EBMHandle;

extern ebreal		e_nsb_get_aspect(ENode *node);
extern void			e_nsb_get_size(ENode *node, uint *x, uint *y, uint *z);
extern EBMHandle	*e_nsb_get_image_handle(VNodeID node_id, char *layer_r, char *layer_g, char *layer_b);
extern EBMHandle	*e_nsb_get_empty_handle(void);
extern void			e_nsb_evaluate_image_handle_tile(EBMHandle *handle, ebreal *output, ebreal u, ebreal v, ebreal w);
extern void			e_nsb_evaluate_image_handle_clamp(EBMHandle *handle, ebreal *output, ebreal u, ebreal v, ebreal w);
extern void			e_nsb_destroy_image_handle(EBMHandle *handle);

*/


void p_th_update_texture(PTextureHandle *handle)
{
	float *buf;
	EBMHandle *h;
	uint i, size;
	size = PIXELS_PER_UPDATE / handle->size[0];
	if(size == 0)
		size = 1;
	if(size > handle->size[1] - handle->update_position)
		size = handle->size[1] - handle->update_position;
	buf = malloc((sizeof *buf) * size * handle->size[0] * 3);
	h = e_nsb_get_image_handle(handle->node_id, handle->layer_r, handle->layer_g, handle->layer_b);
//	printf("handle->update_position %u, handle->size[0] %u, size %u\n", handle->update_position, handle->size[0], size);
	for(i = 0; i < size * handle->size[0]; i++)
		e_nsb_evaluate_image_handle_tile(h, &buf[i * 3], (ebreal)(i % handle->size[0]) / (ebreal)handle->size[0], (ebreal)(handle->update_position + i / handle->size[0]) / (ebreal)handle->size[1], 0.5);
	{
		static uint j = 0;
		buf[j % (handle->size[0] * 3)] = 1;
		j++;
	}
	glBindTexture(GL_TEXTURE_2D, handle->texture_id);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, handle->update_position, handle->size[0], size, GL_RGB, GL_FLOAT, buf);
//lTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, handle->size[0], handle->size[1], 0, GL_RGB, GL_UNSIGNED_BYTE, buf);
	free(buf);
	e_nsb_destroy_image_handle(h);
	handle->update_position += size;
	if(handle->update_position >= handle->size[1])
		handle->update_position = 0;
}
//	glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);

boolean p_th_service_handle(PTextureHandle **pointer)
{
	PTextureHandle *handle;
	EBitLayer *layer;
	uint value[3] = {0, 0, 0};
	ENode *node;
	handle = *pointer;
	if(handle == NULL) /* see if the handle is dead */
		return FALSE;
	if(handle->users == 0)
	{
		if(handle->dead--)
		if(handle->dead == 0)
		{
			if(handle->texture_id != PTextureStorage.std_texture_id)
				glDeleteTextures(1, &handle->texture_id);
			free(handle);
			*pointer = NULL;
			return TRUE;
		}
	}
//printf("serv 1\n");
	/* If the node doesnt exist or is an invalid size, show the std texture */
	if((node = e_ns_get_node(0, handle->node_id)) != NULL && e_ns_get_node_type(node) == V_NT_BITMAP)
		e_nsb_get_size(node, &value[0], &value[1], &value[2]);
//printf("serv 2\n");
	if(value[0] * value[1] * value[2] == 0)
	{
		if(handle->texture_id == PTextureStorage.std_texture_id)
			return FALSE;
		glDeleteTextures(1, &handle->texture_id);
		handle->texture_id = PTextureStorage.std_texture_id;
		return TRUE;
	}
	/* If the node is created create a texture */
//printf("serv 3\n");

	if(handle->texture_id == PTextureStorage.std_texture_id)
	{
		p_th_create_new_texture(node, handle);
		return TRUE;
	}
//printf("serv 4\n");
	/* if the texture size has changed fix it! */

	if(handle->size_version != p_th_compute_size_check_sum(value))
	{
		glDeleteTextures(1, &handle->texture_id);
		p_th_create_new_texture(node, handle);
		return TRUE;
	}
//printf("serv 5\n");
	if(handle->update_position != 0)
	{
		p_th_update_texture(handle);
		return TRUE;
	}
//printf("serv 6\n");
	if((layer = e_nsb_get_layer_by_name(node, handle->layer_r)) != NULL) /* Do we need to update the image */
		value[0] = e_nsb_get_layer_version(layer);
	else
		value[0] = -1;
	if((layer = e_nsb_get_layer_by_name(node, handle->layer_g)) != NULL)
		value[1] = e_nsb_get_layer_version(layer);
	else
		value[1] = -1;
	if((layer = e_nsb_get_layer_by_name(node, handle->layer_b)) != NULL)
		value[2] = e_nsb_get_layer_version(layer);
	else
		value[2] = -1;
//printf("serv 7\n");
	if(value[0] != handle->layer_version[0] || value[0] != handle->layer_version[0] || value[0] != handle->layer_version[0])
	{
		p_th_update_texture(handle);
		handle->layer_version[0] = value[0];
		handle->layer_version[1] = value[1];
		handle->layer_version[2] = value[2];
		return TRUE;
	}
//printf("serv 8\n");
	return FALSE;
}

boolean th_in_service = FALSE;

boolean p_texture_compute(uint dummy)
{
	uint empty;
	for(empty = PTextureStorage.handel_count + 1; empty != 0; empty--)
	{
		PTextureStorage.update_pos = (PTextureStorage.update_pos + 1) % PTextureStorage.handel_count;
		if(p_th_service_handle(&PTextureStorage.handels[PTextureStorage.update_pos]))
			return FALSE;
	}
	th_in_service = FALSE;
	return TRUE;
}

void p_texture_func(ENode *node, ECustomDataCommand command)
{
	if(command == E_CDC_DESTROY || th_in_service == TRUE)
		return;
	th_in_service = TRUE;
	p_task_add(-1, 1, p_texture_compute);
}
