/*
#include "la_includes.h"

#include "st_matrix_operations.h"
#include "la_geometry_undo.h"

extern uint *edge_reference(uint *ref, uint poly_count, uint vertex_count);

void compute_edge(double *output, uint vertex_a, uint vertex_b, Point *vertex)
{
	output[0] = vertex[ref[vertex_a]].x + vertex[ref[vertex_b]].x;
	output[1] = vertex[ref[vertex_a]].y + vertex[ref[vertex_b]].y;
	output[2] = vertex[ref[vertex_a]].z + vertex[ref[vertex_b]].z;
}

void compute_middle(double *output, uint *ref, uint poly_id, uint vertex_count, Point *vertex)
{
	output[0] = vertex[ref[vertex_a]].x + vertex[ref[vertex_b]].x;
	output[1] = vertex[ref[vertex_a]].y + vertex[ref[vertex_b]].y;
	output[2] = vertex[ref[vertex_a]].z + vertex[ref[vertex_b]].z;
}

void subdivide_triangle(double *output, uint *ref, uint poly_id, uint back_ref, Point *vertex)
{
	poly_id *= 4;
	output[0] = vertex[ref[poly_id]].x + vertex[ref[poly_id + 1]].x;
	output[0] = vertex[ref[poly_id]].y + vertex[ref[poly_id + 1]].y;
	output[0] = vertex[ref[poly_id]].z + vertex[ref[poly_id + 1]].z;
}

subdivide()
{
	corner = compute_all_corners(poly)
	vertex[0] = corner[0];
	vertex[1] = corner[0] + corner[1] + corner[2] + corner[3];
	vertex[2] = vertex[1] + corner[0] + corner[1] + naighbour[0];
}

subdivide_next()
{
	corner = compute_all_corners(poly)
	vertex[0] = corner[0]; (level two!)
	vertex[1] = corner[0] + corner[1] + corner[2] + corner[3]; (level one!)
	vertex[2] = vertex[1] + corner[0] + corner[1] + naighbour[0];
	vertex[3] = 
	vertex[4] =
}

typedef struct{
	CORNER_VERTEX_0,
	CORNER_VERTEX_1,
	CORNER_VERTEX_2,
	CORNER_VERTEX_3,
}VertexWeight

void subdivide_geometry()
{
	uint ref_count, *ref, vertex_count, *back_ref;
	udg_get_geometry(&vertex_count, &ref_count, NULL, &ref, NULL);
	back_ref = edge_reference(ref, ref_count, vertex_count);
}
*/
