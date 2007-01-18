#include <stdio.h>
#include <stdlib.h>

#include "st_types.h"

void init_dlut(DynLookUpTable *table)
{
	void **data, **end;
	table->data = malloc(ARRAY_CHUNK_SIZE * sizeof *table->data);
	table->allocated = ARRAY_CHUNK_SIZE;
	data = table->data;
	for(end = data + table->allocated; data < end; data++)
		*data = NULL;
}

void free_dlut(DynLookUpTable *table)
{
	free(table->data);	
}

void *find_dlut(DynLookUpTable *table, uint key)
{
	if(key < table->allocated)
		return table->data[key];
	return NULL;
}

void *get_next_dlut(DynLookUpTable *table, uint key)
{
	uint i;
	for(i = key; i < table->allocated && table->data[i] == NULL; i++);
	if(i >= table->allocated)
		return NULL;
	return table->data[i];
}

uint get_next_empty_dlut(DynLookUpTable *table, uint key)
{
	uint i;
	for(i = key; i < table->allocated && table->data[i] != NULL; i++);
	return i;
}

void *add_entry_dlut(DynLookUpTable *table, uint key, void *pointer)
{
	void **data, **end;
	if(key < table->allocated)
	{
		data = table->data[key];
		table->data[key] = pointer;
		return data;
	}
	table->data = realloc(table->data,(sizeof *table->data) * (key + ARRAY_CHUNK_SIZE));
	data = table->data + table->allocated;
	table->allocated = key + ARRAY_CHUNK_SIZE;
	for(end = table->data + table->allocated; data < end; data++)
		*data = NULL;
	table->data[key] = pointer;
	return NULL;
}

uint add_entry_in_empty_dlut(DynLookUpTable *table, void *pointer)
{
	uint id;
	id = get_next_empty_dlut(table, 0);
	add_entry_dlut(table, id, pointer);
	return id;
}

void remove_entry_dlut(DynLookUpTable *table, uint key)
{
	if(key < table->allocated)
	{
		table->data[key] = NULL;
	}
}

uint count_entry_dlut(DynLookUpTable *table)
{
	uint i, count = 0; 
	for(i = 0; i < table->allocated; i++)
		if(table->data[i] != NULL)
			count++;
	return count;
}

void clean_dlut(DynLookUpTable *table)
{
	uint i;

	for(i = table->allocated - 1; i > 1 && table->data[i] == NULL; i--);
	if(i + ARRAY_CHUNK_SIZE < table->allocated)
		table->data = realloc(table->data,(sizeof *table->data) * (i + ARRAY_CHUNK_SIZE));
}

void foreach_remove_dlut(DynLookUpTable *table, void (*func)(uint key, void *pointer, void *user_data), void *user_data)
{
	uint i;
	void **data;
	data = table->data;
	for(i = 0; i < table->allocated; i++)
		if(data[i] != NULL)
			func(i, data[i], user_data);
	free(table->data);	
}

double get_rand(uint32 index)
{
	index = (index << 13) ^ index;
	return (((index * (index * index * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) * 0.5;
}
