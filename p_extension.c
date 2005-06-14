#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
	#include <windows.h>
	#include <GL/gl.h>
#else
	#include <GL/gl.h>
#endif
#include "verse.h"
#include "persuade.h"
#include "p_task.h"

static void *(*p_gl_GetProcAddress)(const char* proc) = NULL;

void p_extension_init(void *(*gl_GetProcAddress)(const char* proc))
{
	p_gl_GetProcAddress = gl_GetProcAddress;
}

void * p_extension_get_address(const char* proc)
{
	return p_gl_GetProcAddress(proc);
}

boolean p_extension_test(const char *string)
{
	const char *extension, *a;
	uint i;

	extension = glGetString(GL_EXTENSIONS);
	printf("%s\n", extension);

	for(a = extension; a != 0; a++)
	{
		for(i = 0; string[i] != 0 && a[i] != 0 && string[i] == a[i]; i++);
		if(string[i] == 0)
			return TRUE;
	}
	return FALSE;
}
