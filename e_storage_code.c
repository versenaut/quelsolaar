
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "verse.h"
#include "st_types.h"
#include "e_storage_node.h"

#define CODE_BUFFER_OVER_ALLOC 512

typedef struct{
	VLayerID		buffer_id;
	uint			node_id;
	char			*name;
	uint			length;
	uint			allocated;
	char			*data;
	boolean			subscribed;
}ESTextBuffer;

typedef struct{
	ENodeHead		head;
	DynLookUpTable	buffertables;
	char			*language;
	char			*info;
}ESTextNode;


extern void		e_ns_update_node_version_struct(ESTextNode *node);
extern void		e_ns_update_node_version_data(ESTextNode *node);

ESTextNode	*e_create_t_node(VNodeID node_id, VNodeOwner owner)
{
	ESTextNode	*node;
	if((node = (ESTextNode *) e_ns_get_node_networking(node_id)) == NULL)
	{
		node = malloc(sizeof *node);
		init_dlut(&node->buffertables);
		node->language = NULL;
		node->info = NULL;
		e_ns_init_head((ENodeHead *)node, V_NT_TEXT, node_id, owner);
	}
	return node;
}

char *copy_text(const char *text)
{
	char *output;
	uint i;
	for(i = 0; text[i] != 0; i++);
	if(i = 0)
		return NULL;
	else
	{
		output = malloc((sizeof *output) * (i + 1));
		for(i = 0; text[i] != 0; i++)
			output[i] = text[i];
		output[i] = 0;
		return output;
	}
}

void callback_send_t_content_type_set(void *user, VNodeID node_id, const char *language, const char *info)
{
	ESTextNode	*node;
	node = e_create_t_node(node_id, 0);
	if(node->language != NULL)
		free(node->language);
	node->language = copy_text(language);
	if(node->info != NULL)
		free(node->info);
	node->info = copy_text(info);
	e_ns_update_node_version_struct(node);
}

void callback_send_t_buffer_create(void *user, VNodeID node_id, uint16 buffer_id, const char *name)
{
	ESTextNode	*node;
	ESTextBuffer *buffer;
	node = e_create_t_node(node_id, 0);
	buffer = find_dlut(&node->buffertables, buffer_id);
	if(buffer == NULL)
	{
		buffer = malloc(sizeof *buffer);
		buffer->buffer_id = buffer_id;
		buffer->node_id = node_id;
		buffer->length = 0;
		buffer->allocated = 0;
		buffer->data = NULL;
		buffer->subscribed = FALSE;
		add_entry_dlut(&node->buffertables, buffer_id, buffer);
	}
	else if(buffer->name != NULL)
		free(buffer->name);
	buffer->name = copy_text(name);
	e_ns_update_node_version_struct(node);
}

void callback_send_t_buffer_destroy(void *user, VNodeID node_id, uint16 buffer_id)
{
	ESTextNode	*node;
	ESTextBuffer *buffer;
	node = (ESTextNode *)e_ns_get_node_networking(node_id);
	if(node == NULL)
		return;
	buffer = find_dlut(&node->buffertables, buffer_id);
	if(buffer != NULL)
	{
		if(buffer->name != NULL)
			free(buffer->name);
		if(buffer->data != NULL)
			free(buffer->data);
		free(buffer);
		remove_entry_dlut(&node->buffertables, buffer_id);
	}
	e_ns_update_node_version_struct(node);
}

void callback_send_t_insert(void *user, VNodeID node_id, uint16 buffer_id, uint32 start, uint32 length, const char *text)
{
	ESTextNode	*node;
	ESTextBuffer *buffer;
	uint i;
	node = (ESTextNode *)e_ns_get_node_networking(node_id);
	if(node == NULL)
		return;
	buffer = find_dlut(&node->buffertables, buffer_id);
	if(buffer != NULL)
	{
		if(length + buffer->length > buffer->allocated);
		{
			buffer->allocated = length + buffer->length + CODE_BUFFER_OVER_ALLOC;
			buffer->data = realloc(buffer->data, (sizeof *buffer->data) * buffer->allocated);
		}
		for(i = start; i < buffer->length; i++)
			buffer->data[i + length] = buffer->data[i];
		buffer->data[i + length] = 0;
		for(i = 0; i < length; i++)
			buffer->data[i + start] = text[i];

		printf("reseaving- %s -end\n", buffer->data);
		buffer->length += length;
	}
	e_ns_update_node_version_data(node);
}

void callback_send_t_delete(void *user, VNodeID node_id, uint16 buffer_id, uint32 start, uint32 length)
{
	ESTextNode	*node;
	ESTextBuffer *buffer;
	uint i;
	node = (ESTextNode *)e_ns_get_node_networking(node_id);
	if(node == NULL)
		return;
	buffer = find_dlut(&node->buffertables, buffer_id);
	if(buffer != NULL)
	{
		for(i = start; i + length < buffer->length; i++)
			buffer->data[i] = buffer->data[i + length];
		buffer->data[i] = 0;
		buffer->length -= length;
	}
	e_ns_update_node_version_data(node);
}

void delete_text(ESTextNode *node)
{
	ESTextBuffer *buffer;
	uint id;
	for(buffer = get_next_dlut(&node->buffertables, 0); buffer != NULL; buffer = get_next_dlut(&node->buffertables, id))
	{
		if(buffer->data != NULL)
			free(buffer->data);
		if(buffer->name != NULL)
			free(buffer->name);
		id = buffer->buffer_id + 1;
		free(buffer);
	}
	if(node->language != NULL)
		free(node->language);
	if(node->info != NULL)
		free(node->info);
	free_dlut(&node->buffertables);
	free(node);
}

char *e_nst_get_language(ESTextNode *node)
{
	return node->language;
}

char *e_nst_get_info(ESTextNode *node)
{
	return node->info;
}

ESTextBuffer *e_nst_get_buffer_by_name(ESTextNode *node, char *name)
{
	ESTextBuffer *buffer;
	uint i;
	for(buffer = get_next_dlut(&node->buffertables, 0); buffer != NULL; buffer = get_next_dlut(&node->buffertables, buffer->buffer_id + 1))
	{
		if(buffer->name != NULL)
		{	
			for(i = 0; buffer->name[i] != 0 && name[i] != 0 && buffer->name[i] == name[i]; i++);
			if(buffer->name[i] == name[i])
				return buffer;
		}
	}
	return NULL;
}

ESTextBuffer *e_nst_get_buffer_by_id(ESTextNode *node, uint buffer_id)
{
	return find_dlut(&node->buffertables, buffer_id);
}

ESTextBuffer *e_nst_get_buffer_next(ESTextNode *node, uint buffer_id)
{
	return get_next_dlut(&node->buffertables, buffer_id);
}

uint e_nst_get_buffer_id(ESTextBuffer *buffer)
{
	return buffer->buffer_id;
}

char * e_nst_get_buffer_name(ESTextBuffer *buffer)
{
	return buffer->name;
}


uint e_nst_get_buffer_length(ESTextBuffer *buffer)
{
	if(buffer->subscribed == FALSE)
	{
		buffer->subscribed = TRUE;
//		verse_send_c_buffer_subscribe(buffer->node_id, buffer->buffer_id);
	}
	return buffer->length;
}

char * e_nst_get_buffer_text(ESTextBuffer *buffer)
{
	if(buffer->subscribed == FALSE)
	{
		buffer->subscribed = TRUE;
//		verse_send_c_buffer_subscribe(buffer->node_id, buffer->buffer_id);
	}
	return buffer->data;
}

void es_text_init(void)
{
//	verse_callback_set(verse_send_c_content_type_set,	callback_send_c_content_type_set,	NULL);
//	verse_callback_set(verse_send_c_buffer_create,		callback_send_c_buffer_create,		NULL);
//	verse_callback_set(verse_send_c_buffer_destroy,		callback_send_c_buffer_destroy,		NULL);
//	verse_callback_set(verse_send_c_insert,				callback_send_c_insert,				NULL);
//	verse_callback_set(verse_send_c_delete,				callback_send_c_delete,				NULL);
}
