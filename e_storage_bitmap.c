
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "verse.h"
#include "st_types.h"

#include "e_types.h"
#include "e_storage_node.h"

typedef float TBChanel;

typedef struct{
	uint			global_version;
	VNodeID			node_id;
	char			layers[3][16];
	void			*data[3];
	ebreal			(*func[3])(void *data, uint pixel);
	uint			size_x;
	ebreal			size_y;
	ebreal			size_z;
}EBMHandle;

typedef struct{
	VLayerID		layer_id;
	VNBLayerType	type;
	char			name[16];
	void			*data;
	uint			version;
}ESBitmapLayer;

typedef struct{
	ENodeHead		head;
	DynLookUpTable	layertables;
	uint			size_x;
	uint			size_y;
	uint			size_z;
	uint			first;
}ESBitmapNode;


extern void	e_ns_update_node_version_struct(ESBitmapNode *node);
extern void	e_ns_update_node_version_data(ESBitmapNode *node);

uint global_version;
uint global_node_id;

EBMHandle def_handle;

ebreal e_nsb_get_aspect(ESBitmapNode *node)
{
	return (ebreal)node->size_x / (ebreal)node->size_y;
}

void e_nsb_get_size(ESBitmapNode *node, uint *x, uint *y, uint *z)
{
	*x = node->size_x;
	*y = node->size_y;
	*z = node->size_z;
}

ESBitmapLayer *e_nsb_get_layer_by_name(ESBitmapNode *node, char *name)
{
	ESBitmapLayer *layer;
	for(layer =	get_next_dlut(&node->layertables, 0); layer != NULL; layer = get_next_dlut(&node->layertables, layer->layer_id + 1))
		if(strcmp(name, layer->name) == 0)
			return layer;
	return NULL;
}

ESBitmapLayer *e_nsb_get_layer_by_id(ESBitmapNode *node, uint layer_id)
{
	return find_dlut(&node->layertables, layer_id);
}

ESBitmapLayer *e_nsb_get_layer_by_type(ESBitmapNode *node, VNBLayerType type, char *name)
{
	ESBitmapLayer *layer;
	for(layer =	get_next_dlut(&node->layertables, 0); layer != NULL; layer = get_next_dlut(&node->layertables, layer->layer_id + 1))
		if(layer->type == type && strcmp(name, layer->name) == 0)
			return layer;
	return NULL;
}

ESBitmapLayer *e_nsb_get_layer_next(ESBitmapNode *node, uint layer_id)
{
	return get_next_dlut(&node->layertables, layer_id);
}

void *e_nsb_get_layer_data(ESBitmapNode *node, ESBitmapLayer *layer)
{
	uint i;
	if(layer->data == NULL)
	{
		if(layer->type == VN_B_LAYER_UINT1)
		{
			layer->data = malloc(sizeof(uint8) * node->size_x * node->size_y * node->size_z / 8);
		}else if(layer->type == VN_B_LAYER_UINT8)
		{
			layer->data = malloc(sizeof(uint8) * node->size_x * node->size_y * node->size_z);
			for(i = 0; i < node->size_x * node->size_y * node->size_z; i++)
				((uint8 *)layer->data)[i] = ((layer->layer_id + i) % 3) * 127;
		}
		else if(layer->type == VN_B_LAYER_UINT16)
		{
			layer->data = malloc(sizeof(uint16) * node->size_x * node->size_y * node->size_z);
			for(i = 0; i < node->size_x * node->size_y * node->size_z; i++)
				((uint16 *)layer->data)[i] = ((layer->layer_id + i) % 3) * 127 * 256;
		}
		else if(layer->type == VN_B_LAYER_REAL32)
		{
			layer->data = malloc(sizeof(float) * node->size_x * node->size_y * node->size_z);
			for(i = 0; i < node->size_x * node->size_y * node->size_z; i++)
				((float *)layer->data)[i] = (float)((layer->layer_id + i) % 3) * 0.5;
		}
		else
		{
			layer->data = malloc(sizeof(double) * node->size_x * node->size_y * node->size_z);
			for(i = 0; i < node->size_x * node->size_y * node->size_z; i++)
				((double *)layer->data)[i] = (double)((layer->layer_id + i) % 3) * 0.5;
		}
		verse_send_b_layer_subscribe(node->head.node_id, layer->layer_id, 0);
	}
	return layer->data;
}

uint e_nsb_get_layer_id(ESBitmapLayer *layer)
{
	return layer->layer_id;
}

char *e_nsb_get_layer_name(ESBitmapLayer *layer)
{
	return layer->name;
}
VNBLayerType e_nsb_get_layer_type(ESBitmapLayer *layer)
{
	return layer->type;
}

uint e_nsb_get_layer_version(ESBitmapLayer *layer)
{
	return layer->version;
}

EBMHandle *e_nsb_get_empty_handle(void)
{
	def_handle.node_id = 0;
	return &def_handle;
}

ESBitmapNode *e_create_b_node(VNodeID node_id, VNodeOwner owner)
{
	ESBitmapNode	*node;
	if((node = (ESBitmapNode *) e_ns_get_node_networking(node_id)) == NULL)
	{
		node = malloc(sizeof *node);
		init_dlut(&node->layertables); 
		node->size_x = 0;
		node->size_y = 0;
		node->size_z = 0;
		e_ns_init_head((ENodeHead *)node, V_NT_BITMAP, node_id, owner);
	}
	return node;
}

void callback_send_b_layer_create(void *user_data, VNodeID node_id, VLayerID layer_id, char *name, VNBLayerType type)
{
	ESBitmapNode	*node;
	ESBitmapLayer	*layer;
	uint i;
	node = e_create_b_node(node_id, 0);
	if((layer = find_dlut(&node->layertables, layer_id)) != NULL)
	{
		if(layer->type != type)
		{
			if(layer->data)
				free(layer->data);
			layer->data = NULL;
		}
	}else
	{
		layer = malloc(sizeof *layer);
		layer->data = NULL;
		add_entry_dlut(&node->layertables, layer_id, layer);
	}
	for(i = 0; i < 15 && name[i] != 0; i++)
		layer->name[i] = name[i];
	layer->name[i] = name[i];
	layer->layer_id = layer_id;
	layer->type = type;
	layer->version++;
	global_version++;
	e_ns_update_node_version_struct(node);
}

void callback_send_b_layer_destroy(void *user_data, VNodeID node_id, VLayerID layer_id)
{
	ESBitmapNode	*node;
	ESBitmapLayer	*layer;
	node = e_create_b_node(node_id, 0);
	if((layer = find_dlut(&node->layertables, layer_id)) != NULL)
	{
		remove_entry_dlut(&node->layertables, layer_id);
		if(layer->data != NULL)
			free(layer->data);
		free(layer);
		global_version++;
	}
	e_ns_update_node_version_struct(node);
}

void callback_send_b_dimensions_set(void *user_data, VNodeID node_id, uint16 width, uint16 height, uint16 depth)
{
	ESBitmapNode	*node;
	ESBitmapLayer	*layer;
	float			*data;
	uint i, j, k;
	node = e_create_b_node(node_id, 0);
	for(layer = get_next_dlut(&node->layertables, 0); layer != NULL; layer = get_next_dlut(&node->layertables, layer->layer_id + 1))
	{
		switch(layer->type)
		{
			case VN_B_LAYER_UINT1 :
			{
			}
			break;
			case VN_B_LAYER_UINT8 :
			{
				uint8 *buf;
				buf = malloc((sizeof *buf) * width * height * depth);
				for(i = 0 ; i < depth && i < node->size_z; i++)
				{
					for(j = 0 ; j < height && j < node->size_y; j++)
					{
						for(k = 0 ; k < height && k < node->size_x; k++)
							buf[i * height * width + j * width + k] = ((uint8*)layer->data)[i * node->size_y * node->size_x + j * node->size_x + k];
						for(; k < width; k++)
							buf[i * height * width + j * width + k] = 0;
					}
					for(j *= width; j < height * width; j++)
						buf[i * height * width + j] = 0;
				}
				for(i *= height * width; i < depth * height * width; i++)
					buf[i] = 0;
				free(layer->data);
				layer->data = buf;
			}
			break;
			case VN_B_LAYER_UINT16 :
			{
				uint16 *buf;
				buf = malloc((sizeof *buf) * width * height * depth);
				for(i = 0 ; i < depth && i < node->size_z; i++)
				{
					for(j = 0 ; j < height && j < node->size_y; j++)
					{
						for(k = 0 ; k < height && k < node->size_x; k++)
							buf[i * height * width + j * width + k] = ((uint16*)layer->data)[i * node->size_y * node->size_x + j * node->size_x + k];
						for(; k < width; k++)
							buf[i * height * width + j * width + k] = 0;
					}
					for(j *= width; j < height * width; j++)
						buf[i * height * width + j] = 0;
				}
				for(i *= height * width; i < depth * height * width; i++)
					buf[i] = 0;
				free(layer->data);
				layer->data = buf;
			}
			break;
			case VN_B_LAYER_REAL32 :
			{
				real32 *buf;
				buf = malloc((sizeof *buf) * width * height * depth);
				for(i = 0 ; i < depth && i < node->size_z; i++)
				{
					for(j = 0 ; j < height && j < node->size_y; j++)
					{
						for(k = 0 ; k < height && k < node->size_x; k++)
							buf[i * height * width + j * width + k] = ((float*)layer->data)[i * node->size_y * node->size_x + j * node->size_x + k];
						for(; k < width; k++)
							buf[i * height * width + j * width + k] = 0;
					}
					for(j *= width; j < height * width; j++)
						buf[i * height * width + j] = 0;
				}
				for(i *= height * width; i < depth * height * width; i++)
					buf[i] = 0;
				free(layer->data);
				layer->data = buf;
			}
			break;
			case VN_B_LAYER_REAL64 :
			{
				real64 *buf;
				buf = malloc((sizeof *buf) * width * height * depth);
				for(i = 0 ; i < depth && i < node->size_z; i++)
				{
					for(j = 0 ; j < height && j < node->size_y; j++)
					{
						for(k = 0 ; k < height && k < node->size_x; k++)
							buf[i * height * width + j * width + k] = ((double*)layer->data)[i * node->size_y * node->size_x + j * node->size_x + k];
						for(; k < width; k++)
							buf[i * height * width + j * width + k] = 0;
					}
					for(j *= width; j < height * width; j++)
						buf[i * height * width + j] = 0;
				}
				for(i *= height * width; i < depth * height * width; i++)
					buf[i] = 0;
				free(layer->data);
				layer->data = buf;
			}
			break;
		}
	}
	node->size_x = width;
	node->size_y = height;
	node->size_z = depth;
	e_ns_update_node_version_struct(node);
}

ebreal read_pixel_int_eight_func(void *data, uint pixel)
{
	return (ebreal)((uint8 *)data)[pixel] / 255.0;
}
ebreal read_pixel_int_sixteen_func(void *data, uint pixel)
{
	return (ebreal)((uint16 *)data)[pixel] / (255.0 * 255.0 - 1);
}
ebreal read_pixel_float_func(void *data, uint pixel)
{
	return (ebreal)((float *)data)[pixel];
}
ebreal read_pixel_double_func(void *data, uint pixel)
{
	return (ebreal)((double *)data)[pixel];
}
ebreal read_pixel_empty_func(void *data, uint pixel)
{
	return 0.5;
}

void uppdate_bitmap_image_handle(EBMHandle *handle)
{
	uint i, j;
	ESBitmapNode	*node;
	ESBitmapLayer	*layer;
	node = (ESBitmapNode *) e_ns_get_node_networking(handle->node_id);
	handle->func[0] = read_pixel_empty_func;
	handle->func[1] = read_pixel_empty_func;
	handle->func[2] = read_pixel_empty_func;
	if(node == NULL || node->size_x == 0 || node->size_y == 0 || node->size_z == 0)
		return;
	handle->size_x = node->size_x;
	handle->size_y = node->size_y;
	handle->size_z = node->size_z;
	handle->global_version = global_version;
	for(layer = get_next_dlut(&node->layertables, 0); layer != NULL; layer = get_next_dlut(&node->layertables, layer->layer_id + 1))
	{
		for(i = 0; i < 3; i++)
		{
			for(j = 0; j < 16 && handle->layers[i][j] != 0 && handle->layers[i][j] == layer->name[j]; j++);
			if(j < 16 && layer->name[j] == 0)
			{
				e_nsb_get_layer_data(node, layer);
				handle->data[i] = layer->data;
				if(layer->type == VN_B_LAYER_UINT1)
					handle->func[i] = read_pixel_empty_func;
				else if(layer->type == VN_B_LAYER_UINT8)
					handle->func[i] = read_pixel_int_eight_func;
				else if(layer->type == VN_B_LAYER_UINT16)
					handle->func[i] = read_pixel_int_sixteen_func;
				else if(layer->type == VN_B_LAYER_REAL32)
					handle->func[i] = read_pixel_float_func;
				else if(layer->type == VN_B_LAYER_REAL64)
					handle->func[i] = read_pixel_double_func;
			}
		}
	}

}

EBMHandle *e_nsb_get_image_handle(VNodeID node_id, char *layer_r, char *layer_g, char *layer_b)
{
	EBMHandle *handle;
	handle = malloc(sizeof *handle);
	handle->global_version = 127538;
	handle->node_id = node_id;
	handle->size_x = 1;
	handle->size_y = 1;
	handle->size_z = 1;
	sprintf(handle->layers[0], "%s", layer_r);
	sprintf(handle->layers[1], "%s", layer_g);
	sprintf(handle->layers[2], "%s", layer_b);
	uppdate_bitmap_image_handle(handle);
	return handle;
}



void update_bitmap_image_handle_time(EBMHandle *handle, ESBitmapNode *node, uint channel, uint ofset_x, uint ofset_y, uint ofset_z, float *data)
{
	uint x, y;
	ESBitmapLayer	*layer;
	VNBTile		tile;

	for(layer = get_next_dlut(&node->layertables, 0); layer != NULL && strcmp(handle->layers[channel], layer->name); layer = get_next_dlut(&node->layertables, layer->layer_id + 1))
		;
	if(layer == NULL)
		return;
	ofset_x = ofset_z * node->size_x * node->size_y;
	if(layer->type == VN_B_LAYER_UINT1)
	{
	}
	else if(layer->type == VN_B_LAYER_UINT8)
	{
		for(y = 0; y < VN_B_TILE_SIZE; y++)
			for(x = 0; x < VN_B_TILE_SIZE; x++)
				tile.vuint8[y * VN_B_TILE_SIZE + x] = (uint8)(data[((y + ofset_y) * node->size_x + x + ofset_x) * 3 + channel] * 255.0);
		verse_send_b_tile_set(node->head.node_id, layer->layer_id, ofset_x / VN_B_TILE_SIZE, ofset_y / VN_B_TILE_SIZE, ofset_z, VN_B_LAYER_UINT8, &tile);
	}
	else if(layer->type == VN_B_LAYER_UINT16)
	{
		for(y = 0; y < VN_B_TILE_SIZE; y++)
			for(x = 0; x < VN_B_TILE_SIZE; x++)
				tile.vuint16[y * VN_B_TILE_SIZE + x] = (uint16)(data[((y + ofset_y) * node->size_x + x + ofset_x) * 3 + channel] * ((256.0 * 256.0) - 1));
		verse_send_b_tile_set(node->head.node_id, layer->layer_id, ofset_x / VN_B_TILE_SIZE, ofset_y / VN_B_TILE_SIZE, ofset_z, VN_B_LAYER_UINT16, &tile);
	}
	else if(layer->type == VN_B_LAYER_REAL32)
	{
		for(y = 0; y < VN_B_TILE_SIZE; y++)
			for(x = 0; x < VN_B_TILE_SIZE; x++)
				tile.vreal32[y * VN_B_TILE_SIZE + x] = data[((y + ofset_y) * node->size_x + x + ofset_x) * 3 + channel];
		verse_send_b_tile_set(node->head.node_id, layer->layer_id, ofset_x / VN_B_TILE_SIZE, ofset_y / VN_B_TILE_SIZE, ofset_z, VN_B_LAYER_REAL32, &tile);
	}
	else if(layer->type == VN_B_LAYER_REAL64)
	{
		for(y = 0; y < VN_B_TILE_SIZE; y++)
			for(x = 0; x < VN_B_TILE_SIZE; x++)
				tile.vreal64[y * VN_B_TILE_SIZE + x] = data[((y + ofset_y) * node->size_x + x + ofset_x) * 3 + channel];
		verse_send_b_tile_set(node->head.node_id, layer->layer_id, ofset_x / VN_B_TILE_SIZE, ofset_y / VN_B_TILE_SIZE, ofset_z, VN_B_LAYER_REAL64, &tile);
	}
}

void update_bitmap_image_handle_data(EBMHandle *handle, uint ofset_x, uint ofset_y, uint size_x, uint size_y, float *data)
{
	uint i, x, y, z;
	ESBitmapNode	*node;
	node = (ESBitmapNode *) e_ns_get_node_networking(handle->node_id);
	if(node == NULL)
		return;
	for(z = 0; z < node->size_x; z++)
		for(y = 0; y < node->size_y; y += VN_B_TILE_SIZE)
			for(x = 0; x < node->size_x; x += VN_B_TILE_SIZE)
				for(i = 0; i < 3; i++)
					update_bitmap_image_handle_time(handle, node, i, x, y, z, data);
}


void e_nsb_destroy_image_handle(EBMHandle *handle)
{
	free(handle);
}

void e_nsb_evaluate_image_handle_clamp(EBMHandle *handle, ebreal *pixel, ebreal x, ebreal y, ebreal z)
{
	uint i;
	if(handle->global_version != global_version)
		uppdate_bitmap_image_handle(handle);
	if(x > 1);
		x = 1;
	if(x < 0);
		x = 0;
	if(y > 1);
		y = 1;
	if(y < 0);
		y = 0;
	if(z > 1);
		z = 1;
	if(z < 0);
		z = 0;
	i = (uint)(x * (ebreal)handle->size_x) + (uint)(y * handle->size_y) * handle->size_x + (uint)(z * handle->size_z) * handle->size_x * handle->size_y;
	pixel[0] = handle->func[0](handle->data[0], i);
	pixel[1] = handle->func[1](handle->data[1], i);
	pixel[2] = handle->func[2](handle->data[2], i);
}

void e_nsb_evaluate_image_handle_tile(EBMHandle *handle, ebreal *output, ebreal x, ebreal y, ebreal z)
{
	uint i;
	if(handle->global_version != global_version)
		uppdate_bitmap_image_handle(handle);
	x -= (int)x;
	y -= (int)y;
	z -= (int)z;
	i = (uint)(x * (float)handle->size_x) + (uint)(y * handle->size_y) * handle->size_x + (uint)(z * handle->size_z) * handle->size_x * handle->size_y;
	output[0] = handle->func[0](handle->data[0], i);
	output[1] = handle->func[1](handle->data[1], i);
	output[2] = handle->func[2](handle->data[2], i);
}

void callback_send_b_tile_set(void *user_data, VNodeID node_id, VLayerID layer_id, uint16 tile_x, uint16 tile_y, uint16 z, VNBLayerType type, VNBTile *tile)
{
	ESBitmapNode		*node;
	ESBitmapLayer		*layer;
	float				*buf;
	uint32				i, x, y, x_ofset, y_ofset, z_ofset, pixel_id, x_tilesize, y_tilesize;
	node = e_create_b_node(node_id, 0);

	layer = find_dlut(&node->layertables, layer_id);
	if(layer == NULL)
		return;

	x_ofset = tile_x * VN_B_TILE_SIZE;
	y_ofset = tile_y * VN_B_TILE_SIZE;
	z_ofset = node->size_x * node->size_y * z;
	if(layer->type != type)
		return;
	switch(type)
	{
		case VN_B_LAYER_UINT1 :
		printf("ONE BIT TEXTURE LAYERS NOT SUPORTED");
		break;
		case VN_B_LAYER_UINT8 :
		{
			uint8	*array, value;
			array = layer->data;
			for(y = 0; y < VN_B_TILE_SIZE; y++)
			{
				for(x = 0; x < VN_B_TILE_SIZE; x++)
				{
					pixel_id = ((y_ofset + y) * node->size_x) + x_ofset + x;
					if(pixel_id < node->size_x * node->size_y)
						array[pixel_id + z_ofset] = tile->vuint8[(y * VN_B_TILE_SIZE) + x];
				}
			}
		}
		break;
		case VN_B_LAYER_UINT16 :
		{
			uint16	*array;
			array = layer->data;
			for(y = 0; y < VN_B_TILE_SIZE; y++)
			{
				for(x = 0; x < VN_B_TILE_SIZE; x++)
				{
					pixel_id = ((y_ofset + y) * node->size_x) + x_ofset + x;
					if(pixel_id < node->size_x * node->size_y)
						array[pixel_id + z_ofset] = tile->vuint16[(y * VN_B_TILE_SIZE) + x];
				}
			}
		}
		break;
		case VN_B_LAYER_REAL32 :
		{
			float	*array;
			array = layer->data;
			for(y = 0; y < VN_B_TILE_SIZE; y++)
			{
				for(x = 0; x < VN_B_TILE_SIZE; x++)
				{
					pixel_id = ((y_ofset + y) * node->size_x) + x_ofset + x;
					if(pixel_id < node->size_x * node->size_y)
						array[pixel_id + z_ofset] = tile->vreal32[(y * VN_B_TILE_SIZE) + x];
				}
			}
		}
		case VN_B_LAYER_REAL64 :
		{
			double	*array;
			array = layer->data;
			for(y = 0; y < VN_B_TILE_SIZE; y++)
			{
				for(x = 0; x < VN_B_TILE_SIZE; x++)
				{
					pixel_id = ((y_ofset + y) * node->size_x) + x_ofset + x;
					if(pixel_id < node->size_x * node->size_y)
						array[pixel_id + z_ofset] = tile->vreal64[(y * VN_B_TILE_SIZE) + x];
				}
			}
		}
		break;
	}
	layer->version++;
	e_ns_update_node_version_data(node);
}


void es_bitmap_init(void)
{
	verse_callback_set(verse_send_b_layer_create,		callback_send_b_layer_create,		NULL);
	verse_callback_set(verse_send_b_layer_destroy,		callback_send_b_layer_destroy,		NULL);
	verse_callback_set(verse_send_b_dimensions_set,		callback_send_b_dimensions_set,		NULL);
	verse_callback_set(verse_send_b_tile_set,			callback_send_b_tile_set,			NULL);
}

void delete_bitmap_layer_func(uint id, ESBitmapLayer *layer, void *user_data)
{
	if(layer->data != NULL)
		free(layer->data);
	free(layer);
}

void delete_bitmap(ESBitmapNode	*node)
{
	foreach_remove_dlut(&node->layertables, delete_bitmap_layer_func, NULL);
	free(node);
}
