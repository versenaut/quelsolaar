#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "verse.h"
#include "st_types.h"

#include "e_types.h"
#include "e_storage_node.h"
/*
typedef enum {
	VN_A_BLOCK_SIZE_INT8 = 1024,
	VN_A_BLOCK_SIZE_INT16 = 512,
	VN_A_BLOCK_SIZE_INT24 = 384,
	VN_A_BLOCK_SIZE_INT32 = 256,
	VN_A_BLOCK_SIZE_REAL32 = 256,
	VN_A_BLOCK_SIZE_REAL64 = 128,
} VNAConstants;

typedef enum {
	VN_A_LAYER_INT8,
	VN_A_LAYER_INT16,
	VN_A_LAYER_INT24,
	VN_A_LAYER_INT32,
	VN_A_LAYER_REAL32,
	VN_A_LAYER_REAL64,
} VNABlockType;
*/
typedef struct{
	VLayerID		buffer_id;
	char			name[16];
	char			*data;
	uint			length;
	uint			allocated;
	real64			frequency;
	uint			version;
	VNABlockType	type;
}ESAudioBuffer;

typedef struct{
	VBufferID		stream_id;
	char			name[16];
	uint			version;
}ESAudioStream;

typedef struct{
	ENodeHead		head;
	DynLookUpTable	buffertables;
	DynLookUpTable	streamtables;
}ESAudioNode;

extern void	e_ns_update_node_version_struct(ESAudioNode *node);

ESAudioBuffer *e_nsa_get_buffer_by_name(ESAudioNode *node, char *name)
{
	ESAudioBuffer *buffer;
	for(buffer = get_next_dlut(&node->buffertables, 0); buffer != NULL; buffer = get_next_dlut(&node->buffertables, buffer->buffer_id + 1))
		if(strcmp(name, buffer->name) == 0)
			return buffer;
	return NULL;
}

ESAudioBuffer *e_nsa_get_buffer_by_id(ESAudioNode *node,  uint buffer_id)
{
	return find_dlut(&node->buffertables, buffer_id);
}

ESAudioBuffer *e_nsa_get_buffer_next(ESAudioNode *node, uint buffer_id)
{
	return get_next_dlut(&node->buffertables, buffer_id);
}

char *e_nsa_get_buffer_data(ESAudioNode *node, ESAudioBuffer *buffer)
{
	return buffer->data;
}

uint e_nsa_get_buffer_data_length(ESAudioNode *node, ESAudioBuffer *buffer)
{
	return buffer->length;
}

uint e_nsa_get_buffer_id(ESAudioBuffer *buffer)
{
	return buffer->buffer_id;
}

char *e_nsa_get_buffer_name(ESAudioBuffer *buffer)
{
	return buffer->name;
}

VNABlockType e_nsa_get_buffer_type(ESAudioBuffer *buffer)
{
	return buffer->type;
}

double e_nsa_get_buffer_frequency(ESAudioBuffer *buffer)
{
	return buffer->frequency;
}

uint e_nsa_get_buffer_version(ESAudioBuffer *buffer)
{
	return buffer->version;
}



ESAudioStream *e_nsa_get_stream_by_name(ESAudioNode *node, char *name)
{
	ESAudioStream *stream;
	for(stream = get_next_dlut(&node->streamtables, 0); stream != NULL; stream = get_next_dlut(&node->streamtables, stream->stream_id + 1))
		if(strcmp(name, stream->name) == 0)
			return stream;
	return NULL;
}

ESAudioBuffer *e_nsa_get_stream_by_id(ESAudioNode *node,  uint stream_id)
{
	return find_dlut(&node->streamtables, stream_id);
}

ESAudioBuffer *e_nsa_get_stream_next(ESAudioNode *node, uint stream_id)
{
	return get_next_dlut(&node->streamtables, stream_id);
}


uint e_nsa_get_stream_id(ESAudioStream *stream)
{
	return stream->stream_id;
}

char *e_nsa_get_stream_name(ESAudioStream *stream)
{
	return stream->name;
}

uint e_nsa_get_stream_version(ESAudioStream *stream)
{
	return stream->version;
}


ESAudioNode *e_create_a_node(VNodeID node_id, VNodeOwner owner)
{
	ESAudioNode *node;
	if((node = (ESAudioNode *) e_ns_get_node_networking(node_id)) == NULL)
	{
		node = malloc(sizeof *node);
		init_dlut(&node->buffertables); 
		init_dlut(&node->streamtables); 
		e_ns_init_head((ENodeHead *)node, V_NT_AUDIO, node_id, owner);
	}
	return node;
}


void delete_audio_buffer_func(uint id, void *buffer, void *user_data)
{
	ESAudioBuffer	*real_buffer = buffer;

	if(real_buffer->data != NULL)
		free(real_buffer->data);
	free(real_buffer);
}

void delete_audio_stream_func(uint id, void *buffer, void *user_data)
{
	free(buffer);
}

void delete_audio(ESAudioNode	*node)
{
	foreach_remove_dlut(&node->buffertables, delete_audio_buffer_func, NULL);
	foreach_remove_dlut(&node->streamtables, delete_audio_stream_func, NULL);
	free(node);
}

void callback_send_a_buffer_create(void *user_data, VNodeID node_id, VLayerID buffer_id, const char *name, VNABlockType type, real64 frequency)
{
	ESAudioNode	*node;
	ESAudioBuffer	*buffer;
	uint i;
	node = e_create_a_node(node_id, 0);
	if((buffer = find_dlut(&node->buffertables, buffer_id)) != NULL)
	{
		if(buffer->type != type)
		{
			if(buffer->data)
				free(buffer->data);
			buffer->data = NULL;
		}
	}else
	{
		buffer = malloc(sizeof *buffer);
		buffer->data = NULL;
		add_entry_dlut(&node->buffertables, buffer_id, buffer);
	}
	for(i = 0; i < 15 && name[i] != 0; i++)
		buffer->name[i] = name[i];
	buffer->name[i] = name[i];
	buffer->buffer_id = buffer_id;
	buffer->frequency = frequency;
	buffer->type = type;
	buffer->version++;
	e_ns_update_node_version_struct(node);
}

void callback_send_a_buffer_destroy(void *user_data, VNodeID node_id, VLayerID buffer_id)
{
	ESAudioNode		*node;
	ESAudioBuffer	*buffer;
	node = e_create_a_node(node_id, 0);
	if((buffer = find_dlut(&node->buffertables, buffer_id)) != NULL)
	{
		remove_entry_dlut(&node->buffertables, buffer_id);
		if(buffer->data != NULL)
			free(buffer->data);
		free(buffer);
	}
	e_ns_update_node_version_struct(node);
}

void callback_send_a_stream_create(void *user_data, VNodeID node_id, VLayerID stream_id, const char *name)
{
	ESAudioNode		*node;
	ESAudioStream	*stream;
	uint i;
	node = e_create_a_node(node_id, 0);
	if((stream = find_dlut(&node->streamtables, stream_id)) == NULL)
	{
		stream = malloc(sizeof *stream);
		add_entry_dlut(&node->streamtables, stream_id, stream);
	}
	for(i = 0; i < 15 && name[i] != 0; i++)
		stream->name[i] = name[i];
	stream->name[i] = name[i];
	stream->stream_id = stream_id;
	stream->version++;
	e_ns_update_node_version_struct(node);
}

void callback_send_a_stream_destroy(void *user_data, VNodeID node_id, VLayerID stream_id)
{
	ESAudioNode		*node;
	ESAudioStream	*stream;
	node = e_create_a_node(node_id, 0);
	if((stream = find_dlut(&node->streamtables, stream_id)) != NULL)
	{
		remove_entry_dlut(&node->streamtables, stream_id);
		free(stream);
	}
	e_ns_update_node_version_struct(node);
}


void es_audio_init(void)
{
	verse_callback_set(verse_send_a_buffer_destroy,		callback_send_a_buffer_destroy,		NULL);
	verse_callback_set(verse_send_a_buffer_create,		callback_send_a_buffer_create,		NULL);
	verse_callback_set(verse_send_a_stream_destroy,		callback_send_a_stream_destroy,		NULL);
	verse_callback_set(verse_send_a_stream_create,		callback_send_a_stream_create,		NULL);
}
