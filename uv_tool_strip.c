#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"
#include "seduce.h"

#include "uv_geometry.h"

uint uv_find_next_strip(uint edge_1, uint edge_2)
{
	uint *ref, i, id, size;
	ref = uvg_get_ref();
	for(id = uvg_get_next_polygon(0); id != -1; id = uvg_get_next_polygon(id + 1))
	{
		if(uvg_get_polygon_select(id))
		{
			size = uvg_get_sides(id);
			for(i = 0; i < size; i++)
				if(ref[id * 4 + i] == edge_2 && ref[id * 4 + (i + 1) % size] == edge_1)
					return id * 4 + ((i + 2) % size);
		}
	}
	return -1;
}

egreal uv_get_stripth_length(uint polygon, uint edge)
{
	uint *ref;
	egreal *v1, *v2, *vertex, r;
	ref = uvg_get_ref();
	vertex = uvg_get_vertex();
	edge += uvg_get_sides(polygon);
	v1 = &vertex[ref[polygon * 4 + (edge + 1) % uvg_get_sides(polygon)] * 3];
	v2 = &vertex[ref[polygon * 4 + (edge + 2) % uvg_get_sides(polygon)] * 3];
	r = sqrt((v1[0] - v2[0]) * (v1[0] - v2[0]) + (v1[1] - v2[1]) * (v1[1] - v2[1]) +(v1[2] - v2[2]) * (v1[2] - v2[2]));
	v1 = &vertex[ref[polygon * 4 + (edge + 0) % uvg_get_sides(polygon)] * 3];
	v2 = &vertex[ref[polygon * 4 + (edge - 1) % uvg_get_sides(polygon)] * 3];
	return sqrt((v1[0] - v2[0]) * (v1[0] - v2[0]) + (v1[1] - v2[1]) * (v1[1] - v2[1]) +(v1[2] - v2[2]) * (v1[2] - v2[2])) * 0.5 + r * 0.5;
}

void uv_get_stripth_vector(egreal *vec, uint polygon, uint edge)
{
	egreal *vertex, *v1, *v2, uv[8], r;
	uint sides, *ref;

	ref = uvg_get_ref();
	vertex = uvg_get_vertex();

	uvg_get_uve(uv, polygon);
	sides = uvg_get_sides(polygon);
	edge += sides;
	vec[0] = uv[(edge % sides) * 2 + 0] - uv[((edge + 1) % sides) * 2 + 0];
	vec[1] = uv[(edge % sides) * 2 + 1] - uv[((edge + 1) % sides) * 2 + 1];
	r = sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
	vec[0] = (uv[(edge + 1) % sides * 2 + 0] - uv[((edge + 2) % sides) * 2 + 0]) + (uv[(edge % sides) * 2 + 0] - uv[((edge - 1) % sides) * 2 + 0]);
	vec[1] = (uv[(edge + 1) % sides * 2 + 1] - uv[((edge + 2) % sides) * 2 + 1]) + (uv[(edge % sides) * 2 + 1] - uv[((edge - 1) % sides) * 2 + 1]);
	r /= sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
	v1 = &vertex[ref[polygon * 4 + (edge + 0) % sides] * 3];
	v2 = &vertex[ref[polygon * 4 + (edge + 1) % sides] * 3];
	r /= sqrt((v1[0] - v2[0]) * (v1[0] - v2[0]) + (v1[1] - v2[1]) * (v1[1] - v2[1]) + (v1[2] - v2[2]) * (v1[2] - v2[2]));
	vec[0] *= r;
	vec[1] *= r;
}


void uv_next_strip(egreal *uv1, egreal *uv2, egreal *vec, uint array_pos, egreal length)
{
	egreal uv[8];
	uint sides, edge;
	edge = array_pos % 4;
	sides = uvg_get_sides(array_pos / 4);

	uv[((edge + 2) % sides) * 2] = uv2[0];
	uv[((edge + 2) % sides) * 2 + 1] = uv2[1];
	uv[((edge + 3) % sides) * 2] = uv1[0];
	uv[((edge + 3) % sides) * 2 + 1] = uv1[1];
	uv2[0] += vec[0] * length;
	uv2[1] += vec[1] * length;
	uv1[0] += vec[0] * length;
	uv1[1] += vec[1] * length;
	uv[(edge % sides) * 2] = uv1[0];
	uv[(edge % sides) * 2 + 1] = uv1[1];
	uv[((edge + 1) % sides) * 2] = uv2[0];
	uv[((edge + 1) % sides) * 2 + 1] = uv2[1];

	uvg_set_all_corner_select(array_pos / 4, 1, 1, 1, 1);
	uvg_set_all_corner_uv(array_pos / 4, uv[0], uv[1], uv[2], uv[3], uv[4], uv[5], uv[6], uv[7]);

}

void uv_strip_old(uint polygon, uint edge)
{
	uint *ref, id, i;
	egreal uv1[2], uv2[2], uv[8], vec[8];
	ref = uvg_get_ref();
	id = polygon * 4 + edge;
	i = 0;

	uv_get_stripth_vector(&vec[0], polygon, (edge + 0) % 4);
	uv_get_stripth_vector(&vec[2], polygon, (edge + 1) % 4);
	uv_get_stripth_vector(&vec[4], polygon, (edge + 2) % 4);
	uv_get_stripth_vector(&vec[6], polygon, (edge + 3) % 4);

	uvg_get_uve(uv, polygon);

	uv1[0] = uv[((edge + 0) % 4) * 2];
	uv1[1] = uv[((edge + 0) % 4) * 2 + 1];
	uv2[0] = uv[((edge + 1) % 4) * 2];
	uv2[1] = uv[((edge + 1) % 4) * 2 + 1];

	while(TRUE)
	{
		id = uv_find_next_strip(ref[id], ref[(id / 4) * 4 + (id + 1) % uvg_get_sides(id / 4)]);
		if(id == -1 || !uvg_is_quad(id / 4))
			break;
		uv_next_strip(uv1, uv2, vec, id, uv_get_stripth_length(id / 4, id % 4));
		if(i++ == 6)
			return;
	}
}

void uv_strip(uint polygon, uint edge)
{
	uint *ref, id[2], i;
	boolean *used, active[2] = {TRUE, TRUE};
	egreal edge_pos[4][2], uv[8], vec[8];
	ref = uvg_get_ref();
	i = 0;


	used = malloc((sizeof *used) * uvg_get_polygon_length());
	for(i = 0; i < uvg_get_polygon_length(); i++)
		used[i] = FALSE;

	uv_get_stripth_vector(&vec[0], polygon, edge);
	uv_get_stripth_vector(&vec[2], polygon, (edge + 2) % 4);

	uvg_get_uve(uv, polygon);

	edge_pos[0][0] = uv[((edge + 0) % 4) * 2];
	edge_pos[0][1] = uv[((edge + 0) % 4) * 2 + 1];
	edge_pos[1][0] = uv[((edge + 1) % 4) * 2];
	edge_pos[1][1] = uv[((edge + 1) % 4) * 2 + 1];

	edge_pos[2][0] = uv[((edge + 2) % 4) * 2];
	edge_pos[2][1] = uv[((edge + 2) % 4) * 2 + 1];
	edge_pos[3][0] = uv[((edge + 3) % 4) * 2];
	edge_pos[3][1] = uv[((edge + 3) % 4) * 2 + 1];


	id[0] = polygon * 4 + edge;
	id[1] = polygon * 4 + (edge + 2) % uvg_get_sides(polygon);

	while(active[0] || active[1])
	{
		for(i = 0; i < 2; i++)
		{
			if(active[i])
			{
				id[i] = uv_find_next_strip(ref[id[i]], ref[(id[i] / 4) * 4 + (id[i] + 1) % uvg_get_sides(id[i] / 4)]);
				if(id[i] == -1 || !uvg_is_quad(id[i] / 4) || used[id[i] / 4])
					active[i] = FALSE;
				else
				{
					used[id[i] / 4] = TRUE;
					uv_next_strip(edge_pos[i * 2], edge_pos[i * 2 + 1], &vec[i * 2], id[i], uv_get_stripth_length(id[i] / 4, id[i] % 4));
				}
			}
		}
	}
	free(used);
}

void uv_strip_horizontal(uint polygon)
{
	egreal vec[8];
	if(!uvg_is_quad(polygon))
		return;
	uv_get_stripth_vector(&vec[0], polygon, 0);

	if((vec[0] + vec[1] > 0 && vec[0] - vec[1] > 0) || (vec[0] + vec[1] < 0 && vec[0] - vec[1] < 0))
		uv_strip(polygon, 0);
	else
		uv_strip(polygon, 1);
}
void uv_strip_vertical(uint polygon)
{
	egreal vec[8];
	if(!uvg_is_quad(polygon))
		return;
	uv_get_stripth_vector(&vec[0], polygon, 0);

	if((vec[0] + vec[1] > 0 && vec[0] - vec[1] > 0) || (vec[0] + vec[1] < 0 && vec[0] - vec[1] < 0))
		uv_strip(polygon, 1);
	else
		uv_strip(polygon, 0);
}

void uv_strip_plane(uint polygon)
{
	uint *ref, id[4], i;
	boolean *used, active[2] = {TRUE, TRUE};
	egreal edge_pos[8][2], uv[8], vec[8];
	ref = uvg_get_ref();
	i = 0;


	used = malloc((sizeof *used) * uvg_get_polygon_length());
	for(i = 0; i < uvg_get_polygon_length(); i++)
		used[i] = FALSE;

	uv_get_stripth_vector(&vec[0], polygon, 0);
	uv_get_stripth_vector(&vec[1], polygon, 1);
	uv_get_stripth_vector(&vec[2], polygon, 2);
	uv_get_stripth_vector(&vec[3], polygon, 3);

	uvg_get_uve(uv, polygon);
	for(i = 0; i < 4; i++)
	{
		edge_pos[i * 2][0] = uv[i * 2];
		edge_pos[i * 2][1] = uv[i * 2 + 1];
		edge_pos[i * 2 + 1][0] = uv[((i + 1) % 4) * 2];
		edge_pos[i * 2 + 1][1] = uv[((i + 1) % 4) * 2 + 1];
		id[i] = polygon * 4 + i;
	}

	while(active[0] || active[1] || active[2] || active[3])
	{
		for(i = 0; i < 4; i++)
		{
			if(active[i])
			{
				id[i] = uv_find_next_strip(ref[id[i]], ref[(id[i] / 4) * 4 + (id[i] + 1) % uvg_get_sides(id[i] / 4)]);
				if(id[i] == -1 || !uvg_is_quad(id[i] / 4) || used[id[i] / 4])
					active[i] = FALSE;
				else
				{
					used[id[i] / 4] = TRUE;
					uv_next_strip(edge_pos[i * 2], edge_pos[i * 2 + 1], &vec[i * 2], id[i], uv_get_stripth_length(id[i] / 4, id[i] % 4));
				}
			}
		}
	}
	free(used);
}