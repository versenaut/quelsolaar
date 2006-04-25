#include "la_includes.h"
#include "la_geometry_undo.h"



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "verse.h"
#include "enough.h"


#define PLANE_COUNT 10240
#define HULL_COUNT 640


typedef struct{
	egreal plane[3];
	egreal dist;
}CPlane;


typedef struct{
	CPlane *planes;
	uint 	count;
}CHull;

typedef enum{
	PS_FULL = 0,
	PS_PARSIAL = 1,
	PS_OUTSIDE = 2,
}PlaneStatus;

CPlane global_planes[PLANE_COUNT];
uint global_planes_used = 0;
CHull global_hulls[HULL_COUNT];
uint global_hulls_used = 0;


void compute_normal(egreal *normal, egreal *vertex, uint *ref)
{
	egreal r, v0[3], v1[3];
	v0[0] = vertex[ref[1] * 3 + 0] - vertex[ref[0] * 3 + 0];
	v0[1] = vertex[ref[1] * 3 + 1] - vertex[ref[0] * 3 + 1];
	v0[2] = vertex[ref[1] * 3 + 2] - vertex[ref[0] * 3 + 2];
	r = sqrt(v0[0] * v0[0] + v0[1] * v0[1] + v0[2] * v0[2]);
	v0[0] /= r;
	v0[1] /= r;
	v0[2] /= r;
	v1[0] = vertex[ref[2] * 3 + 0] - vertex[ref[0] * 3 + 0];
	v1[1] = vertex[ref[2] * 3 + 1] - vertex[ref[0] * 3 + 1];
	v1[2] = vertex[ref[2] * 3 + 2] - vertex[ref[0] * 3 + 2];
	r = sqrt(v1[0] * v1[0] + v1[1] * v1[1] + v1[2] * v1[2]);
	v1[0] /= r;
	v1[1] /= r;
	v1[2] /= r;
	normal[0] = v0[1] * v1[2] - v0[2] * v1[1];
	normal[1] = v0[2] * v1[0] - v0[0] * v1[2];
	normal[2] = v0[0] * v1[1] - v0[1] * v1[0];
	r = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
	normal[0] /= r;
	normal[1] /= r;
	normal[2] /= r;
}

PlaneStatus convex_test(egreal *vertex, uint *ref, egreal *test)
{
	egreal normal[3], f;
	compute_normal(normal, vertex, ref);
	f = normal[0] * (test[0] - vertex[ref[0] * 3]) + normal[1] * (test[1] - vertex[ref[0] * 3 + 1]) + normal[2] * (test[2] - vertex[ref[0] * 3 + 2]);
	if(f > -0.001)
		return PS_FULL;
	if(f < 0.001)
		return PS_OUTSIDE;
	return PS_PARSIAL;
}

PlaneStatus poly_to_poly_test(uint *polygon, egreal *vertex, uint *test)
{
	egreal *origo, normal[3], *v1, *v2, *v3, f;
	uint inside = 0, outside = 0;
	PlaneStatus output = PS_FULL;
	compute_normal(normal, vertex, polygon);
	origo = &vertex[polygon[0] * 3];
	v1 = &vertex[test[0] * 3];
	v2 = &vertex[test[1] * 3];
	v3 = &vertex[test[2] * 3];
	f = (v1[0] - origo[0]) * normal[0] + (v1[1] - origo[1]) * normal[1] + (v1[2] - origo[2]) * normal[2];
	if(f > -0.001)
		inside++;
	if(f < 0.001)
		outside++;
	f = (v2[0] - origo[0]) * normal[0] + (v2[1] - origo[1]) * normal[1] + (v2[2] - origo[2]) * normal[2];
	if(f > -0.001)
		inside++;
	if(f < 0.001)
		outside++;
	f = (v3[0] - origo[0]) * normal[0] + (v3[1] - origo[1]) * normal[1] + (v3[2] - origo[2]) * normal[2];
	if(f > -0.001)
		inside++;
	if(f < 0.001)
		outside++;
	if(inside == 3)
		return PS_FULL;
	if(outside == 3)
		return PS_OUTSIDE;
	return PS_PARSIAL;
}


boolean add_plane(CHull *hull, egreal *normal, egreal dist)
{
	uint i;
	for(i = 0; i < hull->count; i++)
		if(0.999999999999997 < normal[0] * hull->planes[i].plane[0] + normal[1] * hull->planes[i].plane[1] + normal[2] * hull->planes[i].plane[2])
			return FALSE;
	hull->planes[i].plane[0] = normal[0];
	hull->planes[i].plane[1] = normal[1];
	hull->planes[i].plane[2] = normal[2];
	hull->planes[i].dist = dist;
	hull->count++;
	return TRUE;
}

void sort_planes(CPlane *plane, uint *length)
{
	egreal f, bestf, normal[3] = {0, 0, 0};
	uint i, j, besti = 0;
	CPlane p;
	normal[0] = 0;
	normal[1] = 0;
	normal[2] = 0;
	for(i = 0; i < *length; i++)
	{
		normal[0] += plane[i].plane[0];
		normal[1] += plane[i].plane[1];
		normal[2] += plane[i].plane[2];
		bestf = 100000000;
		for(j = i + 1; j < *length; j++)
		{
			f = normal[0] * plane[j].plane[0] + normal[1] * plane[j].plane[1] + normal[2] * plane[j].plane[2];
			if(f < bestf)
			{
				bestf = f;
				besti = j;
			}
		}
		p = plane[i + 1];
		plane[i + 1] = plane[besti]; 
		plane[besti] = p; 
	}
}


uint *c_triangelize(uint *output_length, uint *ref, uint poly_count, egreal *vertex, uint vertex_count)
{
	egreal normal[3];
	uint i, j = 0, *tri;
	for(i = 0; i < poly_count * 4; i += 4)
	{
		if(ref[i] < vertex_count && ref[i + 1] < vertex_count &&  ref[i + 2] < vertex_count && vertex[ref[i] * 3] != E_REAL_MAX && vertex[ref[i + 1] * 3] != E_REAL_MAX && vertex[ref[i + 2] * 3] != E_REAL_MAX)
		{
			j++;
			if(ref[i + 3] < vertex_count && vertex[ref[i + 3] * 3] != E_REAL_MAX)
				j++;
		}
	}
	tri = malloc((sizeof *tri) * j * 6);
	j  = 0;
	for(i = 0; i < poly_count * 4; i += 4)
	{
		if(ref[i] < vertex_count && ref[i + 1] < vertex_count &&  ref[i + 2] < vertex_count && vertex[ref[i] * 3] != E_REAL_MAX && vertex[ref[i + 1] * 3] != E_REAL_MAX && vertex[ref[i + 2] * 3] != E_REAL_MAX)
		{
			if(ref[i + 3] < vertex_count && vertex[ref[i + 3] * 3] != E_REAL_MAX)
			{
				compute_normal(normal, vertex, &ref[i]);
				if(0 < normal[0] * (vertex[ref[i + 3] * 3 + 0] - vertex[ref[i] * 3 + 0]) +
					normal[1] * (vertex[ref[i + 3] * 3 + 1] - vertex[ref[i] * 3 + 1]) +
					normal[2] * (vertex[ref[i + 3] * 3 + 2] - vertex[ref[i] * 3 + 2]))
				{
					tri[j++] = ref[i];
					tri[j++] = ref[i + 1];
					tri[j++] = ref[i + 2];
					tri[j++] = ref[i];
					tri[j++] = ref[i + 2];
					tri[j++] = ref[i + 3];
				}else
				{
					tri[j++] = ref[i];
					tri[j++] = ref[i + 1];
					tri[j++] = ref[i + 3];
					tri[j++] = ref[i + 1];
					tri[j++] = ref[i + 2];
					tri[j++] = ref[i + 3];
				}
			}else
			{
				tri[j++] = ref[i];
				tri[j++] = ref[i + 1];
				tri[j++] = ref[i + 2];
			}
		}
	}
	*output_length = j / 3;
	return tri;
}


uint *tri_edge_ref(uint *ref, uint ref_length, uint vertex_length)
{
	uint i, j, c, *edge, *vertex;
	edge = malloc((sizeof *edge) * ref_length * 6);
	vertex = malloc((sizeof *vertex) * vertex_length);
	for(i = 0; i < vertex_length;i++)
		vertex[i] = -1;
	for(i = 0; i < ref_length * 6; i++)
		edge[i] = -1;
	j = 0;
	while(j < ref_length)
	{
		for(i = 0; i < ref_length * 3; i++)
		{
			if(vertex[ref[i]] == -1)
				vertex[ref[i]] = i;
			else if(vertex[ref[i]] == i)
				vertex[ref[i]] = -1;
			else
			{
				c = vertex[ref[i]];
				if(edge[i] == -1 && ref[i - i % 3 + (i + 1) % 3] == ref[c - c % 3 + (c + 2) % 3])
				{
					edge[i] = c - c % 3 + (c + 2) % 3;
					edge[c - c % 3 + (c + 2) % 3] = i;
					j = 0;
				}
				if(edge[c] == -1 && ref[i - i % 3 + (i + 2) % 3] == ref[c - c % 3 + (c + 1) % 3])
				{
					edge[i - i % 3 + (i + 2) % 3] = c;
					edge[c] = i - i % 3 + (i + 2) % 3;
					j = 0;
				}
			}
			j++;
		}
	}
	free(vertex);
	return edge;
}



/*
	accepted[0] = ref[ref_length * 3 - 3];
	accepted[1] = ref[ref_length * 3 - 2];
	accepted[2] = ref[ref_length * 3 - 1];
*/

PlaneStatus hull_poly_test(uint *ref, PlaneStatus *status, uint ref_length, egreal *vertex, uint *polygon)
{
	egreal *origo, normal[3], *v1, *v2, *v3, f;
	uint inside, outside, i;
	PlaneStatus output = PS_FULL;

	for(i = 0; status[i] != PS_FULL; i++);

	compute_normal(normal, vertex, polygon);
	origo = &vertex[polygon[0] * 3];
	v1 = &vertex[ref[i * 3] * 3];
	v2 = &vertex[ref[i * 3 + 1] * 3];
	v3 = &vertex[ref[i * 3 + 2] * 3];
	if(-0.01 > (v1[0] - origo[0]) * normal[0] + (v1[1] - origo[1]) * normal[1] + (v1[2] - origo[2]) * normal[2])
		return PS_OUTSIDE;
	if(-0.01 > (v2[0] - origo[0]) * normal[0] + (v2[1] - origo[1]) * normal[1] + (v2[2] - origo[2]) * normal[2])
		return PS_OUTSIDE;
	if(-0.01 > (v3[0] - origo[0]) * normal[0] + (v3[1] - origo[1]) * normal[1] + (v3[2] - origo[2]) * normal[2])
		return PS_OUTSIDE;

	v1 = &vertex[polygon[0] * 3];
	v2 = &vertex[polygon[1] * 3];
	v3 = &vertex[polygon[2] * 3];
	
	for(i = 0; i < ref_length; i++)
	{
		if(status[i] != PS_OUTSIDE && polygon != &ref[i * 3])
		{
			compute_normal(normal, vertex, &ref[i * 3]);
			origo = &vertex[ref[i * 3] * 3];
			inside = 0;
			outside = 0;
			f = (v1[0] - origo[0]) * normal[0] + (v1[1] - origo[1]) * normal[1] + (v1[2] - origo[2]) * normal[2];
			if(0.001 < f)
				inside++;
			if(-0.001 > f)
				outside++;
			f = (v2[0] - origo[0]) * normal[0] + (v2[1] - origo[1]) * normal[1] + (v2[2] - origo[2]) * normal[2];
			if(0.001 < f)
				inside++;
			if(-0.001 > f)
				outside++;
			f = (v3[0] - origo[0]) * normal[0] + (v3[1] - origo[1]) * normal[1] + (v3[2] - origo[2]) * normal[2];
			if(0.001 < f)
				inside++;
			if(-0.001 > f)
				outside++;
			if(inside == 3)
				;
			else if(outside == 3)
				return PS_OUTSIDE;
			else
				output = PS_PARSIAL;
		}
	/*	if(status[i] != PS_OUTSIDE && polygon != &ref[i * 3])
		{
			compute_normal(normal, vertex, &ref[i * 3]);
			origo = &vertex[ref[i * 3] * 3];
			test = 0;
			if(-0.01 < (v1[0] - origo[0]) * normal[0] + (v1[1] - origo[1]) * normal[1] + (v1[2] - origo[2]) * normal[2])
				test++;
			if(-0.01 < (v2[0] - origo[0]) * normal[0] + (v2[1] - origo[1]) * normal[1] + (v2[2] - origo[2]) * normal[2])
				test++;
			if(-0.01 < (v3[0] - origo[0]) * normal[0] + (v3[1] - origo[1]) * normal[1] + (v3[2] - origo[2]) * normal[2])
				test++;
			if(test == 0)
				return PS_OUTSIDE;
			if(test < 3)
				output = PS_PARSIAL;
		}*/
	}
	return output;
}



boolean colision_point(float *pos, float *vector, float *normal, float *dist, float size)
{
	uint i, j;
	CPlane *best = NULL, *best2 = NULL, *p;
	CHull *h;
	float f, f2;
	f2 = 10000;
	if(dist != NULL)
		*dist = 0;
/*	vector[0] = -vector[0];
	vector[1] = -vector[1];
	vector[2] = -vector[2];*/
	for(i = 0; i < global_hulls_used; i++)
	{
		h = &global_hulls[i];
		for(j = 0; j < h->count; j++)
		{
			p = &h->planes[j];
			if(p->plane[0] * pos[0] +
				p->plane[1] * pos[1] +
				p->plane[2] * pos[2] < 
				p->dist - size)
				break;
		}
		if(j == h->count)
		{
			best = NULL;
			f2 = 1000000;
			for(j = 0; j < h->count; j++)  /* Rather compute the one wo has to move the least.*/
			{
				p = &h->planes[j];				
				f = p->plane[0] * vector[0] +
					p->plane[1] * vector[1] +
					p->plane[2] * vector[2];
				if(f > 0.0)
				{
					f = ((p->plane[0] * pos[0] +
						p->plane[1] * pos[1] +
						p->plane[2] * pos[2]) - (p->dist - size)) / f;
					if(f < f2)
					{
						f2 = f;
						best = p;
					}
				}
			}
			if(best != NULL)
			{
				pos[0] -= vector[0] * f2;
				pos[1] -= vector[1] * f2;
				pos[2] -= vector[2] * f2;
			}
		}
	}
	if(best != NULL)
	{
/*		f = vector->x * -best->vector.x +
			vector->y * -best->vector.y +
			vector->z * -best->vector.z;
		vector->x += best->vector.x * f;
		vector->y += best->vector.y * f;
		vector->z += best->vector.z * f;
		if(normal != NULL)
		{
			normal->x = best->vector.x;
			normal->y = best->vector.y;
			normal->z = best->vector.z;
		}*/
		return TRUE;
	}
	return FALSE;
}


boolean test_point(float *pos, float *vector, float *normal, float *dist)
{
	uint i, j;
	CPlane *best = NULL, *best2 = NULL;
	egreal f, f2, size = 0.001;
	f2 = 10000;
	if(dist != NULL)
		*dist = 0;
	for(i = 0; i < global_hulls_used; i++)
	{
		for(j = 0; j < global_hulls[i].count; j++)
		{

			if(global_hulls[i].planes[j].plane[0] * pos[0] +
				global_hulls[i].planes[j].plane[1] * pos[1] +
				global_hulls[i].planes[j].plane[2] * pos[2] < 
				global_hulls[i].planes[j].dist - size)
				break;
		}
		if(j == global_hulls[i].count)
		{
			best = NULL;
			for(j = 0; j < global_hulls[i].count; j++)
			{
				
				f = global_hulls[i].planes[j].plane[0] * vector[0] +
					global_hulls[i].planes[j].plane[1] * vector[1] +
					global_hulls[i].planes[j].plane[2] * vector[2];
				if(f > 0)
				{
					f = (global_hulls[i].planes[j].plane[0] * pos[0] +
						global_hulls[i].planes[j].plane[1] * pos[1] +
						global_hulls[i].planes[j].plane[2] * pos[2]) - (global_hulls[i].planes[j].dist - size);
					
					if(f < f2)
					{
						f2 = f;
						best = &global_hulls[i].planes[j];
					}
				}
			}
			if(best != NULL)
			{
				if(best->dist > 0)
				{
					f = best->dist - (best->plane[0] * pos[0] +
						best->plane[1] * pos[1] +
						best->plane[2] * pos[2]);
					printf("Af = %f\n", f);
					f /= best->plane[0] * -vector[0] +
						best->plane[1] * -vector[1] +
						best->plane[2] * -vector[2];
					printf("Bf = %f\n", f);
					pos[0] -= vector[0] * f;
					pos[1] -= vector[1] * f;
					pos[2] -= vector[2] * f;
					if(dist != NULL)
						*dist *= f;
				}else
				{
					f = (best->plane[0] * pos[0] +
						best->plane[1] * pos[1] +
						best->plane[2] * pos[2]) - (best->dist - size);
					f /= best->plane[0] * vector[0] +
						best->plane[1] * vector[1] +
						best->plane[2] * vector[2];
					pos[0] -= vector[0] * f;
					pos[1] -= vector[1] * f;
					pos[2] -= vector[2] * f;
					if(dist != NULL)
						*dist *= f;
				}
				best2 = best;
			}
		}
	}
	if(best2 != NULL)
	{
		f = vector[0] * -best2->plane[0] +
			vector[1] * -best2->plane[1] +
			vector[2] * -best2->plane[2];
		vector[0] += best2->plane[0] * f;
		vector[1] += best2->plane[1] * f;
		vector[2] += best2->plane[2] * f;
		if(normal != NULL)
		{
			if(best2->dist > 0)
			{
				normal[0] = best2->plane[0];
				normal[1] = best2->plane[1];
				normal[2] = best2->plane[2];
			}else
			{
				normal[0] = -best2->plane[0];
				normal[1] = -best2->plane[1];
				normal[2] = -best2->plane[2];
			}
		}
		return TRUE;
	}
	return FALSE;
}

#define POINT_COUNT 5000

float point_vertex_array[POINT_COUNT * 3];
float point_color_array[POINT_COUNT * 3];

float *test_vertex_array = NULL;
float *test_color_array = NULL;
uint test_length = 0;

float get_fake_rand(uint32 index)
{
	index = (index << 13) ^ index;
	return (((index * (index * index * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0) - 1;
}


void create_test_draw(uint *ref, uint *group, uint ref_length, egreal *vertex)
{
	float vector[3];
	uint i;
	test_vertex_array = malloc((sizeof *test_vertex_array) * 9 * (ref_length + 20));
	test_color_array = malloc((sizeof *test_color_array) * 9 * (ref_length + 20));

	for(i = 0; i < POINT_COUNT; i++)
	{
		point_vertex_array[i * 3 + 0] = get_fake_rand(i * 3 + 0) * 0.5;
		point_vertex_array[i * 3 + 1] = get_fake_rand(i * 3 + 1) * 0.005;
		point_vertex_array[i * 3 + 2] = get_fake_rand(i * 3 + 2) * 0.5;
		vector[0] = 0;
		vector[1] = -1;
		vector[2] = 0;
	/*	if(test_point(&point_vertex_array[i * 3], vector))
		{
			point_color_array[i * 3 + 0] = 1;
			point_color_array[i * 3 + 1] = 0.6;
			point_color_array[i * 3 + 2] = 0.3;
		}else
		{
			point_color_array[i * 3 + 0] = 0.03;
			point_color_array[i * 3 + 1] = 0.1;
			point_color_array[i * 3 + 2] = 0.06;
		}*/
	}

	for(i = 0; i < 20; i++)
	{
		test_vertex_array[i * 9 + 0] = 0.5;
		test_vertex_array[i * 9 + 1] = (float)i * -0.05;
		test_vertex_array[i * 9 + 2] = 0;
		test_vertex_array[i * 9 + 3] = 0.5 + 0.02;
		test_vertex_array[i * 9 + 4] = (float)i * -0.05;
		test_vertex_array[i * 9 + 5] = 0;
		test_vertex_array[i * 9 + 6] = 0.5;
		test_vertex_array[i * 9 + 7] = (float)i * -0.05 + 0.02;
		test_vertex_array[i * 9 + 8] = 0;
		test_color_array[i * 9 + 0] = get_fake_rand(i * 3 + 0);
		test_color_array[i * 9 + 1] = get_fake_rand(i * 3 + 1);
		test_color_array[i * 9 + 2] = get_fake_rand(i * 3 + 2);
		test_color_array[i * 9 + 3] = get_fake_rand(i * 3 + 0);
		test_color_array[i * 9 + 4] = get_fake_rand(i * 3 + 1);
		test_color_array[i * 9 + 5] = get_fake_rand(i * 3 + 2);
		test_color_array[i * 9 + 6] = get_fake_rand(i * 3 + 0);
		test_color_array[i * 9 + 7] = get_fake_rand(i * 3 + 1);
		test_color_array[i * 9 + 8] = get_fake_rand(i * 3 + 2);
	}
	
	for(i = 0; i < ref_length * 3; i++)
	{
		test_vertex_array[i * 3 + 0 + 180] = vertex[ref[i] * 3 + 0];
		test_vertex_array[i * 3 + 1 + 180] = vertex[ref[i] * 3 + 1];
		test_vertex_array[i * 3 + 2 + 180] = vertex[ref[i] * 3 + 2];
		test_color_array[i * 3 + 0 + 180] = get_fake_rand(group[i / 3] * 3 + 0);
		test_color_array[i * 3 + 1 + 180] = get_fake_rand(group[i / 3] * 3 + 1);
		test_color_array[i * 3 + 2 + 180] = get_fake_rand(group[i / 3] * 3 + 2);
	}
	test_length = ref_length * 3 + 60;
}

void update_points()
{
	static uint rand = 0;
	float vector[3], r;
	uint i;


	for(i = 0; i < POINT_COUNT / 2; i++)
	{
		if(point_vertex_array[i * 6 + 1] < -0.5)
		{
			point_vertex_array[i * 6 + 0] = get_fake_rand(rand++) * 0.5;
			point_vertex_array[i * 6 + 1] = 1 + get_fake_rand(rand++) * 0.005;
			point_vertex_array[i * 6 + 2] = get_fake_rand(rand++) * 0.5;
		}
		vector[0] = 0;
		vector[1] = -1;
		vector[2] = 0;
		point_vertex_array[i * 6 + 0] += vector[0] * 0.001;
		point_vertex_array[i * 6 + 1] += vector[1] * 0.001;
		point_vertex_array[i * 6 + 2] += vector[2] * 0.001;

//		if(!test_point(&point_vertex_array[i * 6], vector, NULL, NULL));

		if(colision_point(&point_vertex_array[i * 6], vector, NULL, NULL, 0.01))
		{
			r = sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
			vector[0] /= r;
			vector[1] /= r;
			vector[2] /= r;
			point_vertex_array[i * 6 + 0] = point_vertex_array[i * 6 + 0] + vector[0] * 0.01;
			point_vertex_array[i * 6 + 1] = point_vertex_array[i * 6 + 1] + vector[1] * 0.01;
			point_vertex_array[i * 6 + 2] = point_vertex_array[i * 6 + 2] + vector[2] * 0.01;
		//	test_point(&point_vertex_array[i * 6], vector);
		}
		point_vertex_array[i * 6 + 3] = point_vertex_array[i * 6 + 0] + vector[0] * 0.02;
		point_vertex_array[i * 6 + 4] = point_vertex_array[i * 6 + 1] + vector[1] * 0.02;
		point_vertex_array[i * 6 + 5] = point_vertex_array[i * 6 + 2] + vector[2] * 0.02;
	}
}



void fps_move(float *pos, float *vec, float gravity, float stairs)
{
	float move[3] = {0, 0, 0};
/*	move[0] = vec[0] * 0.5;
	move[1] = vec[1] * 0.5 + stairs;
	move[2] = vec[2] * 0.5;	*/
	move[1] += stairs;
	pos[0] += move[0];
	pos[1] += move[1];
	pos[2] += move[2];
	colision_point(pos, move, NULL, NULL, 0.01);

	pos[0] += vec[0];
	pos[1] += vec[1];
	pos[2] += vec[2];
	colision_point(pos, vec, NULL, NULL, 0.01);
	
	move[1] = -stairs - gravity;
	pos[0] += move[0];
	pos[1] += move[1];
	pos[2] += move[2];
	colision_point(pos, move, NULL, NULL, 0.01);
}

void test_fps()
{
	static float pos[3] = {0, 1, 0}, rot = 0;
	float time, vector[3] = {0, 0, 0}, vertex[6];
	static boolean w = FALSE, a = FALSE, d = FALSE, lw = FALSE, la = FALSE, ld = FALSE;
	time = betray_get_delta_time();
	betray_get_key_up_down(&w, &lw, 'w');
	betray_get_key_up_down(&a, &la, 'a');
	betray_get_key_up_down(&d, &ld, 'd');
	if(pos[1] < -1)
		pos[1] = 1;

	if(a)
		rot += time * 10;
	if(d)
		rot -= time * 10;
	if(w)
	{
		vector[0] = sin(rot) * time * 0.5;
		vector[1] = 0;
		vector[2] = cos(rot) * time * 0.5;
	}
	fps_move(pos, vector, 0.4 * time, 0.01);

	vertex[0] = pos[0];
	vertex[1] = pos[1];
	vertex[2] = pos[2];
	vertex[3] = pos[0] + sin(rot) * 0.1;
	vertex[4] = pos[1];
	vertex[5] = pos[2] + cos(rot) * 0.1;
	sui_draw_gl(GL_LINES, vertex, 2, 3, 1, 0, 1);

}

void test_draw()
{
	if(test_vertex_array == NULL)
		return;
/*	glDisable(GL_BLEND);
	update_points();
	sui_set_color_array_gl(test_color_array, test_length, 3);
	sui_draw_gl(GL_TRIANGLES, test_vertex_array, test_length, 3, 1, 1, 1);
//	sui_set_color_array_gl(point_color_array, POINT_COUNT, 3);
	glDisable(GL_DEPTH_TEST);
	sui_draw_gl(GL_LINES, point_vertex_array, POINT_COUNT, 3, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);*/
	test_fps();
}

void create_hull(uint *ref, uint *n, uint ref_length, egreal *vertex, uint key, PlaneStatus *status)
{
	uint i, j;
	boolean found;
	PlaneStatus s;

	for(i = 0; i < ref_length; i++)
			status[i] = PS_OUTSIDE;

	status[key] = PS_FULL;
	found = TRUE;
	while(found == TRUE) /* build basic hull */
	{
		found = FALSE;
		for(i = 0; i < ref_length * 3; i++)
		{
			if(n[i] != -1 && status[i / 3] == PS_OUTSIDE /*&& accepted[i / 3] != PS_FULL*/ && status[n[i] / 3] == PS_FULL)
			{
			//	status[i / 3] = hull_poly_test(ref, status, ref_length, vertex, &ref[(i / 3) * 3]);
				status[i / 3] = convex_test(vertex, &ref[(i / 3) * 3], &vertex[ref[(n[i] / 3) * 3 + ((n[i] + 2) % 3)] * 3]);
				if(status[i / 3] != PS_OUTSIDE)
				{
					if(PS_FULL == poly_to_poly_test(&ref[(i / 3) * 3], vertex, &ref[key * 3]))
					{
						for(j = 0; j < ref_length; j++)
							if(status[j] != PS_OUTSIDE && j != key)
								if(PS_OUTSIDE == poly_to_poly_test(&ref[j * 3], vertex, &ref[(i / 3) * 3]))
									status[i / 3] = PS_OUTSIDE;
						if(status[i / 3] != PS_OUTSIDE)
							found = TRUE;
					}else
						status[i / 3] = PS_OUTSIDE;
				}
			}
		}
	}

	/* remove anything blocking key polygon */
	for(i = 0; i < ref_length; i++)
	{
		if(status[i] != PS_OUTSIDE && i != key)
		{
			if(PS_OUTSIDE == poly_to_poly_test(&ref[i * 3], vertex, &ref[key * 3]))
				status[i] = PS_OUTSIDE;
		}
	}

	/* make sure hull is convex */
	for(i = 0; i < ref_length; i++)
	{
		if(status[i] != PS_OUTSIDE && i != key)
		{
			for(j = 0; j < ref_length; j++)
			{
				if(status[j] != PS_OUTSIDE)
				{
					s = poly_to_poly_test(&ref[j * 3], vertex, &ref[i * 3]);
					if(s > status[i])
						status[i] = s;
				}
			}
		}
	}
	/* See if we need to add other polys */
	for(i = 0; i < ref_length; i++) 
	{
		if(status[i] == PS_OUTSIDE)
		{
			if(PS_FULL == poly_to_poly_test(&ref[i * 3], vertex, &ref[key * 3]))
			{
				status[i] = PS_FULL;
				for(j = 0; j < ref_length; j++)
				{
					if(status[j] != PS_OUTSIDE)
					{
						s = poly_to_poly_test(&ref[j * 3], vertex, &ref[i * 3]);
						if(s == PS_OUTSIDE)
						{
							status[i] = PS_OUTSIDE;
							break;
						}
						if(s == PS_PARSIAL)
							status[i] = PS_PARSIAL;
					}
				}
				if(status[i] != PS_OUTSIDE)
				{
					for(j = 0; j < ref_length; j++)
					{
						if(status[j] != PS_OUTSIDE)
						{
							if(PS_OUTSIDE == poly_to_poly_test(&ref[j * 3], vertex, &ref[i * 3]))
							{
								status[j] = PS_OUTSIDE;
								break;
							}
						}
					}
				}
			}
		}
	}

	/* make sure hull is convex again */

	for(i = 0; i < ref_length; i++)
	{
		if(status[i] != PS_OUTSIDE && i != key)
		{
			for(j = 0; j < ref_length; j++)
			{
				if(status[j] != PS_OUTSIDE)
				{
					s = poly_to_poly_test(&ref[j * 3], vertex, &ref[i * 3]);
					if(s > status[i])
						status[i] = s;
				}
			}
		}
	}

	/* pick up some extra Parsials if possible*/

	for(i = 0; i < ref_length; i++)
	{
		if(status[i] == PS_OUTSIDE && PS_FULL == poly_to_poly_test(&ref[i * 3], vertex, &ref[key * 3]))
		{
			status[i] = PS_FULL;
			for(j = 0; j < ref_length; j++)
			{
				if(status[j] != PS_OUTSIDE && j != i)
				{
					s = poly_to_poly_test(&ref[j * 3], vertex, &ref[i * 3]);
					if(s == PS_PARSIAL)
						status[i] = PS_PARSIAL;
					if(s == PS_OUTSIDE)
					{
						status[i] = PS_OUTSIDE;
						break;
					}
				}
			}
		}
	}
	return;
	/* pick up some extra Parsials if possible*/
/*	for(i = 0; i < ref_length; i++)
	{
		if(status[i] == PS_PARSIAL)
		{
			for(j = 0; j < ref_length; j++)
				if(status[j] != PS_OUTSIDE && j != i)
					if(poly_to_poly_test(&ref[j * 3], vertex, &ref[i * 3]) != PS_FULL)
						break;
			if(j == ref_length)
				status[i] = PS_FULL;
		}
	}*/
}

void create_hulls2(uint *ref, uint *n, uint ref_length, egreal *vertex)
{
	uint i, j, key, tmp, best;
	egreal normal[3];
	boolean found;
	PlaneStatus *status, *accepted, *group, s;
	accepted = malloc((sizeof *accepted) * ref_length);
	status = malloc((sizeof *status) * ref_length);
	group = malloc((sizeof *group) * ref_length);

	for(i = 0; i < ref_length; i++)
		group[i] = 0;
	for(i = 0; i < ref_length; i++)
		accepted[i] = PS_OUTSIDE;
	for(i = 0; i < ref_length; i++)
		status[i] = PS_OUTSIDE;
	key = -1;
	best = 0;
	for(i = 0; i < ref_length; i++)
	{
		create_hull(ref, n, ref_length, vertex, i, status);
		tmp = 0;
		for(j = 0; j < ref_length; j++)
			if(status[j] == PS_FULL)
				tmp++;
		if(tmp > best)
		{
			best = tmp;
			key = i;
			if(tmp == ref_length)
				break;
		}
	}
//	key = 41;
	while(key != -1)
	{
		create_hull(ref, n, ref_length, vertex, key, status);
		global_hulls[global_hulls_used].count = 0;
		global_hulls[global_hulls_used].planes = &global_planes[global_planes_used];
		for(i = 0; i < ref_length; i++)
		{
			if(status[i] != PS_OUTSIDE)
			{
				compute_normal(normal, vertex, &ref[i * 3]);
				add_plane(&global_hulls[global_hulls_used], normal, normal[0] * vertex[ref[i * 3] * 3] +
											normal[1] * vertex[ref[i * 3] * 3 + 1] +
											normal[2] * vertex[ref[i * 3] * 3 + 2]);
			}
		}
		global_planes_used += global_hulls[global_hulls_used].count;
	//	sort_planes(global_hulls[global_hulls_used].planes, &global_hulls[global_hulls_used].count);
		global_hulls_used++;

		for(i = 0; i < ref_length; i++)
		{
			if(status[i] == PS_FULL)
			{
				accepted[i] = PS_FULL;
				group[i] = global_hulls_used;
			}if(status[i] == PS_PARSIAL && accepted[i] == PS_OUTSIDE)
			{
				accepted[i] = PS_PARSIAL;
		//		group[i] = global_hulls_used + 1;
			}
			status[i] = PS_OUTSIDE;
		}
		accepted[key] = PS_FULL;

	//	group[key] = global_hulls_used + 7;
		key = -1;
		best = 0;
		for(i = 0; i < ref_length; i++)
		{
			if(accepted[i] != PS_FULL)
			{
				create_hull(ref, n, ref_length, vertex, i, status);
				tmp = 0;
				for(j = 0; j < ref_length; j++)
					if(status[j] == PS_FULL && accepted[j] != PS_FULL)
						tmp++;
				if(tmp > best)
				{
					best = tmp;
					key = i;
				}
			}
		}
		printf("key = %u\n", key); 
//		key = -1;
	}


//	group[0] = 5;

	create_test_draw(ref, group, ref_length, vertex);
//	printf("Exit!\n");
}



uint close_a_hole(uint *triangles, uint *n, /*uint *group,*/ uint *length, uint start)
{
	uint i;
	i = start;
	while(TRUE)
	{
	//	printf("i %i %i, %i\n", i, (i / 3 * 3) + (i + 1) % 3, *length);
		i = (i / 3 * 3) + (i + 1) % 3;
	/*	if(i > *length * 3)
			exit(0);*/
		if(n[i] == -1)
		{
			if(triangles[start] == triangles[(i / 3 * 3) + (i + 1) % 3])
			{
				n[start] = i;
				n[i] = start;
				return -1;
			}else
			{
			/*	group[*length] = triangles[start / 3];*/
				triangles[*length * 3 + 0] = triangles[start];
				triangles[*length * 3 + 1] = triangles[(i / 3 * 3) + (i + 1) % 3];
				triangles[*length * 3 + 2] = triangles[(start / 3 * 3) + (start + 1) % 3];
				n[*length * 3 + 0] = -1;
				n[*length * 3 + 1] = i;
				n[*length * 3 + 2] = start;
				n[-1] = *length * 3 + 0;
				n[i] = *length * 3 + 1;
				n[start] = *length * 3 + 2;
				(*length)++;
				return (*length - 1) * 3;
			}
		}

		i = n[i];
	}	
}



void close_all_holes(uint *triangles, uint *n, /*uint *group,*/ uint *length)
{
	uint i, start;
	for(i = 0; i < *length; i++)
	{
		if(n[i] == -1)
		{
			start = i;
			printf("start %i\n", start);
			while((start = close_a_hole(triangles, n, /*group,*/ length, start)) != -1); 
		}
	}
}

void print_volumes()
{
	uint i, j;
	printf("hulls used = %i\n", global_hulls_used);
	for(i = 0; i < global_hulls_used; i++)
	{
		printf("plane_count = %i\n", global_hulls[i].count);
		for(j = 0; j < global_hulls[i].count; j++)
		{
			printf("dist = %f\n", global_hulls[i].planes[j].dist);
			printf("vec = %f %f %f\n\n", global_hulls[i].planes[j].plane[0], global_hulls[i].planes[j].plane[1], global_hulls[i].planes[j].plane[2]);
		}
	}
}/*

extern void pack_real32(FILE *f, float data);
extern void save_uint16(FILE *f, uint16 value);

void save_volumes(FILE *f)
{
	uint i, j, count = 0;
	save_uint16(f, global_hulls_used);
	for(i = 0; i < global_hulls_used; i++)
	{
		save_uint16(f, global_hulls[i].count);
		for(j = 0; j < global_hulls[i].count; j++)
		{
			count++;
			pack_real32(f, global_hulls[i].planes[j].dist);
			pack_real32(f, global_hulls[i].planes[j].plane[0]);
			pack_real32(f, global_hulls[i].planes[j].plane[1]);
			pack_real32(f, global_hulls[i].planes[j].plane[2]);
		}
	}
	printf("hulls used = %i planes = %u\n", global_hulls_used, count);
}*/


void c_create_volumes(FILE *f, uint *ref, uint ref_count, uint vertex_count, egreal *vertex)
{
	uint *map, *tri, tri_count, *n;
	global_planes_used = 0;
	global_hulls_used = 0;
	tri = c_triangelize(&tri_count, ref, ref_count, vertex, vertex_count);
	n = tri_edge_ref(tri, tri_count, vertex_count);
//	close_all_holes(tri, n,  &tri_count);
	create_hulls2(tri, n, tri_count, vertex);
	print_volumes();
	free(n);
	free(tri);
}


void la_close_all_holes()
{
	uint *ref, ref_count, vertex_count;
	egreal *vertex;
	udg_get_geometry(&vertex_count, &ref_count, &vertex, &ref, NULL);
	c_create_volumes(NULL, ref, ref_count, vertex_count, vertex);
}
