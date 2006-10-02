
#include <math.h>

//#include "ngl.h"
#include "enough.h"
#include "p_geo_subdivide_internal.h"
/*
void render_a_mesh_using_ngl(PMesh *mesh)
{
	NGLArray vertex, normal, normal_lines, ref;
	uint i;
	static float t = 0;
	float pos[] = {0, 0, 0, 1}, color[] = {0.1, 1, 1, 1};
	double f;

	pos[0] = sin(t * 0.1);
	pos[1] = cos(t * 0.1);

	color[0] = sin(t * 0.1) * 0.5 + 0.5;
	color[1] = cos(t * 0.2) * 0.5 + 0.5;
	color[1] = cos(t * 0.3) * 0.5 + 0.5;
	t += 0.1;
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, color);

	vertex = nglGetArray(NGL_VERTEX_ARRAY, mesh->render.vertex_count, 4);
	normal_lines = nglGetArray(NGL_VERTEX_ARRAY, mesh->render.vertex_count * 2, 4);
	normal = nglGetArray(NGL_REAL_PARAM_ARRAY, mesh->render.vertex_count, 3);
	ref = nglGetArray(NGL_INT_PARAM_ARRAY, mesh->render.element_count, 3);

	for(i = 0; i < mesh->render.vertex_count; i++)
		nglSetVertex3f(vertex, i, mesh->render.vertex_array[i * 3 + 0], mesh->render.vertex_array[i * 3 + 1], mesh->render.vertex_array[i * 3 + 2]);	
	for(i = 0; i < mesh->render.vertex_count; i++)


	for(i = 0; i < mesh->render.vertex_count; i++)
	{
		nglSetVertex3f(normal_lines, i * 2, mesh->render.vertex_array[i * 3 + 0], mesh->render.vertex_array[i * 3 + 1], mesh->render.vertex_array[i * 3 + 2]);	
		f = sqrt(mesh->render.normal_array[i * 3 + 0] * mesh->render.normal_array[i * 3 + 0] + mesh->render.normal_array[i * 3 + 1] * mesh->render.normal_array[i * 3 + 1] + mesh->render.normal_array[i * 3 + 2] * mesh->render.normal_array[i * 3 + 2]);
		nglSetVertex3f(normal_lines, i * 2 + 1, mesh->render.vertex_array[i * 3 + 0] + mesh->render.normal_array[i * 3 + 0] / f * 0.01, 
											mesh->render.vertex_array[i * 3 + 1] + mesh->render.normal_array[i * 3 + 1] / f * 0.01, 
												mesh->render.vertex_array[i * 3 + 2] + mesh->render.normal_array[i * 3 + 2] / f * 0.01);

		nglSetRealParam3f(normal, i, mesh->render.normal_array[i * 3 + 0] / f, mesh->render.normal_array[i * 3 + 1] / f, mesh->render.normal_array[i * 3 + 2] / f);	
	}

	for(i = 0; i < mesh->render.element_count; i += 3)
		nglSetIntParam3i(ref, i, mesh->render.reference[i], mesh->render.reference[i + 1], mesh->render.reference[i + 2]);
//	nglDrawArray(NGL_TRIANGLES_FILLED, vertex, &normal, 1 , sui_get_material(SUIM_WHITE_LIGHT_NORMAL), ref);

	nglFreeArray(vertex);
	nglFreeArray(normal);
	nglFreeArray(normal_lines);
	nglFreeArray(ref);
}*/