#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"
#include "seduce.h"

#include "uv_geometry.h"
#include "uv.h"

uint unfold_side = 0;
uint unfold_last = 0;
egreal unfold_uv[8];

void uv_start_polygon_unfold(uint id, float x, float y)
{
	uvg_get_uve(unfold_uv, id);
	unfold_side = 0 > (x - unfold_uv[0 * 2 + 0]) * (unfold_uv[1 * 2 + 1] - unfold_uv[0 * 2 + 1]) + (y - unfold_uv[0 * 2 + 1]) * (unfold_uv[0 * 2 + 0] - unfold_uv[1 * 2 + 0]);
	unfold_last = ~0u;
}

void find_unfold_vertex(egreal *edge_vertex, egreal *edge_vertex2, egreal *vertex, egreal *u, egreal *v)
{
	egreal vec[3], vec2[3], r, r2;
	vec[0] = edge_vertex2[0] - edge_vertex[0];
	vec[1] = edge_vertex2[1] - edge_vertex[1];
	vec[2] = edge_vertex2[2] - edge_vertex[2];
	r = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	vec[0] /= r;
	vec[1] /= r;
	vec[2] /= r;

	vec2[0] = vertex[0] - edge_vertex[0];
	vec2[1] = vertex[1] - edge_vertex[1];
	vec2[2] = vertex[2] - edge_vertex[2];
	if(unfold_side)
		*u = -(vec[0] * vec2[0] + vec[1] * vec2[1] + vec[2] * vec2[2]);
	else
		*u = (vec[0] * vec2[0] + vec[1] * vec2[1] + vec[2] * vec2[2]);
	r2 = sqrt(vec2[0] * vec2[0] + vec2[1] * vec2[1] + vec2[2] * vec2[2]);
	*v = sqrt(r2 * r2 - *u * *u) / r; 
	*u = *u / r;
}

void find_unfold_uv(egreal *prev_edge1, egreal *prev_edge2, uint id, uint edge)
{
	egreal u, v, *vertex, vec[3];
	uint *ref;
	unfold_uv[edge * 2] = prev_edge2[0];
	unfold_uv[edge * 2 + 1] = prev_edge2[1];
	unfold_uv[((edge + 1) % uvg_get_sides(id)) * 2] = prev_edge1[0];
	unfold_uv[((edge + 1) % uvg_get_sides(id)) * 2 + 1] = prev_edge1[1];

	ref = uvg_get_ref();

	if(unfold_side)
	{
		vec[0] = prev_edge1[0] - prev_edge2[0];
		vec[1] = prev_edge1[1] - prev_edge2[1];
	}else
	{
		vec[0] = prev_edge2[0] - prev_edge1[0];
		vec[1] = prev_edge2[1] - prev_edge1[1];
	}

	vertex = uvg_get_vertex();
	if(uvg_is_quad(id))
	{
		find_unfold_vertex(&vertex[ref[id * 4 + (edge + 1) % 4] * 3], &vertex[ref[id * 4 + edge] * 3], &vertex[ref[id * 4 + (edge + 2) % 4] * 3], &u, &v);

		unfold_uv[((edge + 2) % 4) * 2] = prev_edge1[0] + vec[0] * u - vec[1] * v;
		unfold_uv[((edge + 2) % 4) * 2 + 1] = prev_edge1[1] + vec[1] * u + vec[0] * v;

		find_unfold_vertex(&vertex[ref[id * 4 + (edge + 1) % 4] * 3], &vertex[ref[id * 4 + edge] * 3], &vertex[ref[id * 4 + (edge + 3) % 4] * 3], &u, &v);

		unfold_uv[((edge + 3) % 4) * 2] = prev_edge1[0] + vec[0] * u - vec[1] * v;
		unfold_uv[((edge + 3) % 4) * 2 + 1] = prev_edge1[1] + vec[1] * u + vec[0] * v;

		uvg_set_all_corner_uv(id, unfold_uv[0], unfold_uv[1], unfold_uv[2], unfold_uv[3], unfold_uv[4], unfold_uv[5], unfold_uv[6], unfold_uv[7]);							
	}else
	{
		find_unfold_vertex(&vertex[ref[id * 4 + (edge + 1) % 3] * 3], &vertex[ref[id * 4 + edge] * 3], &vertex[ref[id * 4 + (edge + 2) % 3] * 3], &u, &v);

		unfold_uv[((edge + 2) % 3) * 2] = prev_edge1[0] + vec[0] * u - vec[1] * v;
		unfold_uv[((edge + 2) % 3) * 2 + 1] = prev_edge1[1] + vec[1] * u + vec[0] * v;

		uvg_set_all_corner_uv(id, unfold_uv[0], unfold_uv[1], unfold_uv[2], unfold_uv[3], unfold_uv[4], unfold_uv[5], unfold_uv[6], unfold_uv[7]);							
	}

}

uint uv_polygon_unfold(uint id, float x, float y)
{
	egreal uv[8];
	boolean in[4];
	uint i, *ref, id2, edge[2], pass_edge;

	if(id == ~0u || id != uvg_get_next_polygon(id))
		return ~0u;
	uvg_get_uve(uv, id);
	if(uvg_is_quad(id) && uv_test_quad(uv, x, y))
		unfold_last = ~0u;
	if(!uvg_is_quad(id) && uv_test_tri(uv, x, y))
		unfold_last = ~0u;

	in[0] = 0 > (x - unfold_uv[0 * 2 + 0]) * (unfold_uv[1 * 2 + 1] - unfold_uv[0 * 2 + 1]) + (y - unfold_uv[0 * 2 + 1]) * (unfold_uv[0 * 2 + 0] - unfold_uv[1 * 2 + 0]);
	in[1] = 0 > (x - unfold_uv[1 * 2 + 0]) * (unfold_uv[2 * 2 + 1] - unfold_uv[1 * 2 + 1]) + (y - unfold_uv[1 * 2 + 1]) * (unfold_uv[1 * 2 + 0] - unfold_uv[2 * 2 + 0]);
	if(uvg_is_quad(id))
	{
		in[2] = 0 > (x - unfold_uv[2 * 2 + 0]) * (unfold_uv[3 * 2 + 1] - unfold_uv[2 * 2 + 1]) + (y - unfold_uv[2 * 2 + 1]) * (unfold_uv[2 * 2 + 0] - unfold_uv[3 * 2 + 0]);
		in[3] = 0 > (x - unfold_uv[3 * 2 + 0]) * (unfold_uv[0 * 2 + 1] - unfold_uv[3 * 2 + 1]) + (y - unfold_uv[3 * 2 + 1]) * (unfold_uv[3 * 2 + 0] - unfold_uv[0 * 2 + 0]);
	}else
		in[2] = 0 > (x - unfold_uv[2 * 2 + 0]) * (unfold_uv[0 * 2 + 1] - unfold_uv[2 * 2 + 1]) + (y - unfold_uv[2 * 2 + 1]) * (unfold_uv[2 * 2 + 0] - unfold_uv[0 * 2 + 0]);
	ref = uvg_get_ref();

	for(pass_edge = 0; pass_edge < uvg_get_sides(id); pass_edge++)
	{
		if(in[pass_edge] != unfold_side)
		{
			edge[0] = ref[id * 4 + pass_edge];
			edge[1] = ref[id * 4 + (pass_edge + 1) % uvg_get_sides(id)];
			for(id2 = uvg_get_next_polygon(0); id2 != ~0u; id2 = uvg_get_next_polygon(id2 + 1))
			{
				if(unfold_last != id2 && uvg_get_polygon_select(id2))
				{
					for(i = 0; i < uvg_get_sides(id2); i++)
					{
						if(ref[id2 * 4 + i] == edge[1] && ref[id2 * 4 + (i + 1) % uvg_get_sides(id2)] == edge[0])
						{
							unfold_last = id;
							find_unfold_uv(&uv[pass_edge * 2], &uv[((pass_edge + 1) % uvg_get_sides(id)) * 2], id2, i);
							return id2;
						}
					}
				}
			}
			return ~0u;
		}
	}
	return id;
}

/*
uint unfold_side = 0;
uint unfold_last = 0;
egreal unfold_uv[8];

void uv_start_polygon_unfold(uint id, float x, float y)
{
	uvg_get_uve(unfold_uv, id);
	unfold_side = 0 > (x - unfold_uv[0 * 2 + 0]) * (unfold_uv[1 * 2 + 1] - unfold_uv[0 * 2 + 1]) + (y - unfold_uv[0 * 2 + 1]) * (unfold_uv[0 * 2 + 0] - unfold_uv[1 * 2 + 0]);
	unfold_last = -1;
}

void find_unfold_vertex(egreal *edge_vertex, egreal *edge_vertex2, egreal *vertex, egreal *u, egreal *v)
{
	egreal vec[3], vec2[3], r;
	vec[0] = edge_vertex2[0] - edge_vertex[0];
	vec[1] = edge_vertex2[1] - edge_vertex[1];
	vec[2] = edge_vertex2[2] - edge_vertex[2];
	r = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	vec[0] /= r;
	vec[1] /= r;
	vec[2] /= r;

	vec2[0] = vertex[0] - edge_vertex[0];
	vec2[1] = vertex[1] - edge_vertex[1];
	vec2[2] = vertex[2] - edge_vertex[2];

	*u = vec[0] * vec2[0] + vec[1] * vec2[1] + vec[2] * vec2[2];
	r = sqrt(vec2[0] * vec2[0] + vec2[1] * vec2[1] + vec2[2] * vec2[2]);
	printf("r = %f\n", r);
	*v = sqrt(r * r - *u * *u); 
}

void find_unfold_uv(egreal *prev_edge1, egreal *prev_edge2, uint id, uint edge)
{
	egreal u, v, r,  *vertex, vec[3];
	uint *ref;
	unfold_uv[edge * 2] = prev_edge2[0];
	unfold_uv[edge * 2 + 1] = prev_edge2[1];
	unfold_uv[((edge + 1) % uvg_get_sides(id)) * 2] = prev_edge1[0];
	unfold_uv[((edge + 1) % uvg_get_sides(id)) * 2 + 1] = prev_edge1[1];

	ref = uvg_get_ref();

	vec[0] = prev_edge2[0] - prev_edge1[0];
	vec[1] = prev_edge2[1] - prev_edge1[1];

	r = sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
	vec[0] /= r;
	vec[1] /= r;

	vertex = uvg_get_vertex();

	if(uvg_is_quad(id))
	{
		find_unfold_vertex(&vertex[ref[id * 4 + (edge + 1) % 4] * 3], &vertex[ref[id * 4 + edge] * 3], &vertex[ref[id * 4 + (edge + 2) % 4] * 3], &u, &v);

		unfold_uv[((edge + 2) % 4) * 2] = prev_edge1[0] + vec[0] * u - vec[1] * v;
		unfold_uv[((edge + 2) % 4) * 2 + 1] = prev_edge1[1] + vec[1] * u + vec[0] * v;

		find_unfold_vertex(&vertex[ref[id * 4 + (edge + 1) % 4] * 3], &vertex[ref[id * 4 + edge] * 3], &vertex[ref[id * 4 + (edge + 3) % 4] * 3], &u, &v);

		unfold_uv[((edge + 3) % 4) * 2] = prev_edge1[0] + vec[0] * u - vec[1] * v;
		unfold_uv[((edge + 3) % 4) * 2 + 1] = prev_edge1[1] + vec[1] * u + vec[0] * v;

		uvg_set_all_corner_uv(id, unfold_uv[0], unfold_uv[1], unfold_uv[2], unfold_uv[3], unfold_uv[4], unfold_uv[5], unfold_uv[6], unfold_uv[7]);							
	}else
	{
		find_unfold_vertex(&vertex[ref[id * 4 + (edge + 1) % 3] * 3], &vertex[ref[id * 4 + edge] * 3], &vertex[ref[id * 4 + (edge + 2) % 3] * 3], &u, &v);

		unfold_uv[((edge + 2) % 3) * 2] = prev_edge1[0] + vec[0] * u - vec[1] * v;
		unfold_uv[((edge + 2) % 3) * 2 + 1] = prev_edge1[1] + vec[1] * u + vec[0] * v;

		uvg_set_all_corner_uv(id, unfold_uv[0], unfold_uv[1], unfold_uv[2], unfold_uv[3], unfold_uv[4], unfold_uv[5], unfold_uv[6], unfold_uv[7]);							
	}

}

uint uv_polygon_unfold(uint id, float x, float y)
{
	egreal uv[8];
	boolean in[4];
	uint i, *ref, id2, count = 0, edge[2], pass_edge;

	if(id == -1 || id != uvg_get_next_polygon(id))
		return -1;
	uvg_get_uve(uv, id);
	if(uvg_is_quad(id) && uv_test_quad(uv, x, y))
		unfold_last = -1;
	if(!uvg_is_quad(id) && uv_test_tri(uv, x, y))
		unfold_last = -1;

	in[0] = 0 > (x - unfold_uv[0 * 2 + 0]) * (unfold_uv[1 * 2 + 1] - unfold_uv[0 * 2 + 1]) + (y - unfold_uv[0 * 2 + 1]) * (unfold_uv[0 * 2 + 0] - unfold_uv[1 * 2 + 0]);
	in[1] = 0 > (x - unfold_uv[1 * 2 + 0]) * (unfold_uv[2 * 2 + 1] - unfold_uv[1 * 2 + 1]) + (y - unfold_uv[1 * 2 + 1]) * (unfold_uv[1 * 2 + 0] - unfold_uv[2 * 2 + 0]);
	if(uvg_is_quad(id))
	{
		in[2] = 0 > (x - unfold_uv[2 * 2 + 0]) * (unfold_uv[3 * 2 + 1] - unfold_uv[2 * 2 + 1]) + (y - unfold_uv[2 * 2 + 1]) * (unfold_uv[2 * 2 + 0] - unfold_uv[3 * 2 + 0]);
		in[3] = 0 > (x - unfold_uv[3 * 2 + 0]) * (unfold_uv[0 * 2 + 1] - unfold_uv[3 * 2 + 1]) + (y - unfold_uv[3 * 2 + 1]) * (unfold_uv[3 * 2 + 0] - unfold_uv[0 * 2 + 0]);
	}else
		in[2] = 0 > (x - unfold_uv[2 * 2 + 0]) * (unfold_uv[0 * 2 + 1] - unfold_uv[2 * 2 + 1]) + (y - unfold_uv[2 * 2 + 1]) * (unfold_uv[2 * 2 + 0] - unfold_uv[0 * 2 + 0]);
	ref = uvg_get_ref();

	for(pass_edge = 0; pass_edge < uvg_get_sides(id); pass_edge++)
	{
		if(in[pass_edge] != unfold_side)
		{
			edge[0] = ref[id * 4 + pass_edge];
			edge[1] = ref[id * 4 + (pass_edge + 1) % uvg_get_sides(id)];
			for(id2 = uvg_get_next_polygon(0); id2 != -1; id2 = uvg_get_next_polygon(id2 + 1))
			{
				if(unfold_last != id2)
				{
					for(i = 0; i < uvg_get_sides(id2); i++)
					{
						if(ref[id2 * 4 + i] == edge[1] && ref[id2 * 4 + (i + 1) % uvg_get_sides(id2)] == edge[0])
						{
							unfold_last = id;
							find_unfold_uv(&uv[pass_edge * 2], &uv[((pass_edge + 1) % uvg_get_sides(id)) * 2], id2, i);
							return id2;
						}
					}
				}
			}
			return -1;
		}
	}
	return id;
}*/
