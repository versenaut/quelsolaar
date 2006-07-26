#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
	#include <windows.h>
	#include <GL/gl.h>
#else
	#include <sys/time.h>
#if defined(__APPLE__) || defined(MACOSX)
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif
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
}PGlobalTaskManager;

void p_task_add(uint id, float importance, boolean (*func)(uint id))
{
	PTask *t;
	{
		ENode *n; 
		uint *a = NULL, i;
		n = e_ns_get_node(0, id);
		for(i = 0; i < PGlobalTaskManager.count; i++)
		{
			if(PGlobalTaskManager.tasks[i].id == id) 
			{
/*				printf("\n!!!!!!!!!!! already found == %u", id );*/
				return;
			}
		}
/*		printf("\n!!!!!!!!!!! func %p\n",  func);
		if(n != NULL)
		{
			printf("\n!!!!!!!!!!!adding type %u id %u func %p\n", e_ns_get_node_type(n), id, func);
		}*/
	}
	if(PGlobalTaskManager.count == P_MAX_TASKS)
		return;

	t = &PGlobalTaskManager.tasks[PGlobalTaskManager.count++];
	t->id = id;
	t->func = func;
	t->importance = importance;
	t->wait = importance;
}

extern boolean p_init_table(uint max_tesselation_level);

void p_task_compute(uint count)
{
	PTimer timer;
	PTask *t;
	uint i;
/*	if(PGlobalTaskManager.init == FALSE)
	{
		for(i = 0; i < count && !PGlobalTaskManager.init; i++)
			PGlobalTaskManager.init = p_init_table(0);
		return;
	}*/

	p_timer_start(&timer);
	while(p_timer_elapsed(&timer) < 0.01 && PGlobalTaskManager.count != 0)
//	for(i = 0; i < count && PGlobalTaskManager.count != 0; i++)
	{
//		for(t = &PGlobalTaskManager.tasks[PGlobalTaskManager.current]; t->wait < 0.99; t = &PGlobalTaskManager.tasks[PGlobalTaskManager.current])
//		{
//			t->wait += t->importance;
			PGlobalTaskManager.current = (PGlobalTaskManager.current + 1) % PGlobalTaskManager.count;
//		}
//		t->wait = t->importance;
			t = &PGlobalTaskManager.tasks[PGlobalTaskManager.current];
		if(t->func(t->id))
		{
			PGlobalTaskManager.tasks[PGlobalTaskManager.current] = PGlobalTaskManager.tasks[--PGlobalTaskManager.count];
		}
	}
}

extern void p_th_init(void);

extern void p_object_func(ENode *node, ECustomDataCommand command);
extern void p_geometry_func(ENode *node, ECustomDataCommand command);
extern void p_shader_func(ENode *node, ECustomDataCommand command);
extern void p_texture_func(ENode *node, ECustomDataCommand command);
extern void p_array_init(void);
extern void p_env_init(uint size);
extern void p_init_render_to_texture(void);
extern void p_create_flare(void);

void persuade_init(uint max_tesselation_level, void *(*gl_GetProcAddress)(const char* proc))
{
	uint i;
	p_extension_init(gl_GetProcAddress);
	p_array_init();
	p_shader_init();
	p_th_init();
	p_env_init(8);
	p_init_render_to_texture();
	p_create_flare();
	PGlobalTaskManager.count = 0;
	PGlobalTaskManager.current = 0;
	PGlobalTaskManager.init = FALSE;
	PGlobalTaskManager.init = p_init_table(max_tesselation_level);

	PGlobalTaskManager.init = TRUE;

	for(i = 0; i < P_MAX_TASKS; i++)
	{
		PGlobalTaskManager.tasks[i].id = -1;
		PGlobalTaskManager.tasks[i].func = NULL;
	}
	while(!p_init_table(max_tesselation_level))
		;
	

	e_ns_set_custom_func(P_ENOUGH_SLOT, V_NT_OBJECT, p_object_func);
	e_ns_set_custom_func(P_ENOUGH_SLOT, V_NT_GEOMETRY, p_geometry_func);
	e_ns_set_custom_func(P_ENOUGH_SLOT, V_NT_BITMAP, p_texture_func);
}

#if defined _WIN32
static timer_period = -1.0;		/* Conversion factor for QueryPerformanceCounter. */
#endif

void p_timer_start(PTimer *t)
{
	if(t == NULL)
		return;
#if defined _WIN32
	QueryPerformanceCounter(&t->now);
	if(timer_period < 0.0)		/* Hopefully, this is a pretty quick test. */
	{
		LARGE_INTEGER	freq;

		QueryPerformanceFrequency(&freq);
		timer_period = 1.0 / freq.QuadPart;	/* Convert Hz into period. */
	}
#else
	gettimeofday(&t->now, NULL);
#endif
}

double p_timer_elapsed(const PTimer *t)
{
	PTimer	t2;

	if(t == NULL)
		return 0.0;
	p_timer_start(&t2);
#if defined _WIN32
	return timer_period * (t2.now.QuadPart - t->now.QuadPart);
#else
	return t2.now.tv_sec - t->now.tv_sec + 1e-6 * (t2.now.tv_usec - t->now.tv_usec);
#endif
}

extern void p_th_context_update(void);
extern void p_impostor_context_update(void);

void p_context_update(void)
{
	ENode *node;
	p_th_context_update();
	for(node = e_ns_get_node_next(0, 0, V_NT_MATERIAL); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_MATERIAL))
	{
		p_shader_func(node, E_CDC_DESTROY);
		p_shader_func(node, E_CDC_CREATE);
		e_ns_update_node_version_struct(node);
	}
	p_impostor_context_update();
	p_create_flare();
	p_shader_init();
}
