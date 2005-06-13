#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
	#include <windows.h>
	#include <GL/gl.h>
#else
	#include <GL/gl.h>
#endif
#include "verse.h"
#include "enough.h"


#include "p_extension.h"
#include "p_shader.h"
#include "p_task.h"

typedef struct{
	uint32	id;
	boolean (*func)(uint id);
	float	importance;
	float	wait;
}PTask;

struct{
	PTask	tasks[P_MAX_TASKS];
	uint	count;
	uint	current;
	boolean	init;
}PGlobalTaskMannager;

void p_task_add(uint id, float importance, boolean (*func)(uint id))
{
	PTask *t;
	{
		ENode *n; 
		uint *a = NULL, i;
		n = e_ns_get_node(0, id);
		for(i = 0; i < PGlobalTaskMannager.count; i++)
		{
			if(PGlobalTaskMannager.tasks[i].id == id) 
			{
				printf("\n!!!!!!!!!!! already found == %u", id );
				return;
			}
		}
	/*	printf("\n!!!!!!!!!!! func %p\n",  func);
		if(n != NULL)
		{
			printf("\n!!!!!!!!!!!adding type %u id %u func %p\n", e_ns_get_node_type(n), id, func);
		}*/
	}
	if(PGlobalTaskMannager.count == P_MAX_TASKS)
		return;

	t = &PGlobalTaskMannager.tasks[PGlobalTaskMannager.count++];
	t->id = id;
	t->func = func;
	t->importance = importance;
	t->wait = importance;
}

extern boolean p_init_table(uint max_tesselation_level);

void p_task_compute(uint count)
{
	PTask *t;
	uint i;
	if(PGlobalTaskMannager.init == FALSE)
	{
		for(i = 0; i < count && !PGlobalTaskMannager.init; i++)
			PGlobalTaskMannager.init = p_init_table(0);
		return;
	}
	for(i = 0; i < count; i++)
	{
		if(PGlobalTaskMannager.count == 0)
			return;
//		for(t = &PGlobalTaskMannager.tasks[PGlobalTaskMannager.current]; t->wait < 0.99; t = &PGlobalTaskMannager.tasks[PGlobalTaskMannager.current])
//		{
//			t->wait += t->importance;
			PGlobalTaskMannager.current = (PGlobalTaskMannager.current + 1) % PGlobalTaskMannager.count;
//		}
//		t->wait = t->importance;
			t = &PGlobalTaskMannager.tasks[PGlobalTaskMannager.current];
		if(t->func(t->id))
		{
			PGlobalTaskMannager.tasks[PGlobalTaskMannager.current] = PGlobalTaskMannager.tasks[--PGlobalTaskMannager.count];
			if(PGlobalTaskMannager.current == PGlobalTaskMannager.count)
				PGlobalTaskMannager.current = 0;
		}
	}
}

extern void p_th_init();

extern void p_object_func(ENode *node, ECustomDataCommand command);
extern void p_geometry_func(ENode *node, ECustomDataCommand command);
extern void p_shader_func(ENode *node, ECustomDataCommand command);
extern void p_texture_func(ENode *node, ECustomDataCommand command);
extern void p_array_init();
extern void p_env_init(uint size);

void persuade_init(uint max_tesselation_level, void *(*gl_GetProcAddress)(const char* proc))
{
	uint i;
	p_extention_init(gl_GetProcAddress);
	p_array_init();
	p_shader_init();
	p_env_init(8);
	PGlobalTaskMannager.count = 0;
	PGlobalTaskMannager.current = 0;
	PGlobalTaskMannager.init = FALSE;
	PGlobalTaskMannager.init = p_init_table(max_tesselation_level);

	PGlobalTaskMannager.init = TRUE;

	for(i = 0; i < P_MAX_TASKS; i++)
	{
		PGlobalTaskMannager.tasks[i].id = -1;
		PGlobalTaskMannager.tasks[i].func = NULL;
	}
	while(!p_init_table(max_tesselation_level));
	p_th_init();

	e_ns_set_custom_func(P_ENOUGH_SLOT, V_NT_OBJECT, p_object_func);
	e_ns_set_custom_func(P_ENOUGH_SLOT, V_NT_GEOMETRY, p_geometry_func);
	e_ns_set_custom_func(P_ENOUGH_SLOT, V_NT_BITMAP, p_texture_func);
}
