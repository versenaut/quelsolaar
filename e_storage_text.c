#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "verse.h"
#include "st_types.h"

#include "e_types.h"
#include "e_storage_node.h"


typedef struct{
	VBufferID		buffer_id;
	char			name[16];
	char			*data;
	uint			length;
	uint			allocated;
	uint			version;
}ESTextBuffer;

typedef struct{
	ENodeHead		head;
	DynLookUpTable	buffertables;
	char			language[512];
}ESTextNode;

char* e_nst_get_language(ESTextNode *t_node)
{
	return t_node->language;
}


ESTextBuffer *e_nst_get_buffer_by_name(ESTextNode *node, char *name)
{
	ESTextBuffer *buffer;
	for(buffer = get_next_dlut(&node->buffertables, 0); buffer != NULL; buffer = get_next_dlut(&node->buffertables, buffer->buffer_id + 1))
		if(strcmp(name, buffer->name) == 0)
			return buffer;
	return NULL;
}

ESTextBuffer *e_nst_get_buffer_by_id(ESTextNode *node,  uint buffer_id)
{
	return find_dlut(&node->buffertables, buffer_id);
}

ESTextBuffer *e_nst_get_buffer_next(ESTextNode *node, uint buffer_id)
{
	return get_next_dlut(&node->buffertables, buffer_id);
}

char *e_nst_get_buffer_data(ESTextNode *node, ESTextBuffer *buffer)
{
	return buffer->data;
}

uint e_nst_get_buffer_data_length(ESTextNode *node, ESTextBuffer *buffer)
{
	return buffer->length;
}

uint e_nst_get_buffer_id(ESTextBuffer *buffer)
{
	return buffer->buffer_id;
}

char *e_nst_get_buffer_name(ESTextBuffer *buffer)
{
	return buffer->name;
}


uint e_nst_get_buffer_version(ESTextBuffer *buffer)
{
	return buffer->version;
}

ESTextNode *e_create_t_node(VNodeID node_id, VNodeOwner owner)
{
	ESTextNode	*node;
	if((node = (ESTextNode *) e_ns_get_node_networking(node_id)) == NULL)
	{
		node = malloc(sizeof *node);
		init_dlut(&node->buffertables); 
		node->language[0] = 0;
		e_ns_init_head((ENodeHead *)node, V_NT_TEXT, node_id, owner);
	}
	return node;
}


static void delete_text_buffer_func(uint id, void *buffer, void *user_data)
{
	ESTextBuffer	*real_buffer = buffer;

	if(real_buffer->data != NULL)
		free(real_buffer->data);
	free(real_buffer);
}

void delete_text(ESTextNode	*node)
{
	foreach_remove_dlut(&node->buffertables, delete_text_buffer_func, NULL);
	free(node);
}


void callback_send_t_set_language(void *user, VNodeID node_id, const char *language)
{
	ESTextNode	*node;
	uint i;
	node = e_create_t_node(node_id, 0);
	for(i = 0; i < 511 && language[i] != 0; i++)
		node->language[i] = language[i];
	node->language[i] = 0;
	e_ns_update_node_version_struct(node);
}

void callback_send_t_buffer_create(void *user, VNodeID node_id, VBufferID buffer_id, const char *name)
{
	ESTextNode	*node;
	ESTextBuffer *buffer;
	uint i;
	node = e_create_t_node(node_id, 0);
	if((buffer = find_dlut(&node->buffertables, buffer_id)) == NULL)
	{
		buffer = malloc(sizeof *buffer);
		buffer->data = NULL;
		buffer->length = 0;
		buffer->allocated = 0;
		buffer->version = 0;
		buffer->buffer_id = buffer_id;
		add_entry_dlut(&node->buffertables, buffer_id, buffer);
		printf("A added new buffer\n");
	}
	for(i = 0; i < 15 && name[i] != 0; i++)
		buffer->name[i] = name[i];
	buffer->name[i] = name[i];
	buffer->version++;
	printf("B added new buffer\n");
	e_ns_update_node_version_struct(node);
}

void callback_send_t_buffer_destroy(void *user, VNodeID node_id, VBufferID buffer_id)
{
	ESTextNode	*node;
	ESTextBuffer *buffer;
	node = e_create_t_node(node_id, 0);
	if((buffer = find_dlut(&node->buffertables, buffer_id)) != NULL)
	{
		remove_entry_dlut(&node->buffertables, buffer_id);
		if(buffer->data != NULL)
			free(buffer->data);
		free(buffer);
		e_ns_update_node_version_struct(node);
	}
}

void es_text_init(void)
{
	verse_callback_set(verse_send_t_set_language,		callback_send_t_set_language,		NULL);
	verse_callback_set(verse_send_t_buffer_create,		callback_send_t_buffer_create,		NULL);
	verse_callback_set(verse_send_t_buffer_destroy,		callback_send_t_buffer_destroy,		NULL);
}