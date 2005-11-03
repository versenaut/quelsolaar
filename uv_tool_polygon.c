#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"
#include "seduce.h"

#include "uv_geometry.h"


void uv_tool_poly_square(uint poly_id)
{
	uint vertex;
	egreal uv[8], vec[4], pos[2], new_uv[8];
	if(uvg_get_sides(poly_id) != 4)
		return;
	uvg_get_uve(uv, poly_id);


	pos[0] = (uv[0] + uv[2] + uv[4] + uv[6]) / 4;
	pos[1] = (uv[1] + uv[3] + uv[5] + uv[7]) / 4;
	
	vec[0] = (-(uv[0] - pos[0]) + (uv[3] - pos[1]) + (uv[4] - pos[0]) + -(uv[7] - pos[1])) / 4;
	vec[1] = ((uv[1] - pos[1]) + (uv[2] - pos[0]) + -(uv[5] - pos[1]) + -(uv[6] - pos[0])) / 4;



	uvg_set_all_corner_uv(poly_id, pos[0] + vec[0],
									pos[1] + vec[1],

									pos[0] + vec[1],
									pos[1] - vec[0],

									pos[0] - vec[0],
									pos[1] - vec[1],

									pos[0] - vec[1],
									pos[1] + vec[0]);
}
void uv_tool_poly_rectangle(uint poly_id)
{
	uint vertex;
	egreal uv[8], axis[4], vec[2], pos[2], length[2], r;
	if(uvg_get_sides(poly_id) != 4)
		return;
	uvg_get_uve(uv, poly_id);


	pos[0] = (uv[0] + uv[2] + uv[4] + uv[6]) / 4;
	pos[1] = (uv[1] + uv[3] + uv[5] + uv[7]) / 4;
	
	axis[0] = ((uv[0] - uv[2]) + (uv[6] - uv[4])) / 4;
	axis[1] = ((uv[1] - uv[3]) + (uv[7] - uv[5])) / 4;

	length[0] = sqrt(axis[0] * axis[0] + axis[1] * axis[1]);

	axis[2] = ((uv[2] - uv[4]) + (uv[0] - uv[6])) / 4;
	axis[3] = ((uv[3] - uv[5]) + (uv[1] - uv[7])) / 4;

	length[1] = sqrt(axis[2] * axis[2] + axis[3] * axis[3]);

	if(0 < axis[0] * axis[3] - axis[1] * axis[2])
		length[1] = -length[1];

	if(0 < axis[1] * axis[2] - axis[0] * axis[3])
		length[0] = -length[0];


	vec[0] = axis[0] + axis[3];
	vec[1] = axis[1] + -axis[2];

	r = sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
	vec[0] /= r;
	vec[1] /= r;

	uvg_set_all_corner_uv(poly_id, pos[0] + vec[0] * length[0] + vec[1] * length[1],
								pos[1] + vec[1] * length[0] - vec[0] * length[1],

								pos[0] - vec[0] * length[0] + vec[1] * length[1],
								pos[1] - vec[1] * length[0] - vec[0] * length[1],

								pos[0] - vec[0] * length[0] - vec[1] * length[1],
								pos[1] - vec[1] * length[0] + vec[0] * length[1],

								pos[0] + vec[0] * length[0] - vec[1] * length[1],
								pos[1] + vec[1] * length[0] + vec[0] * length[1]);
}

void uv_tool_poly_rectangle_newer_but_not_newest(uint poly_id)
{
	uint vertex;
	egreal uv[8], vec[4], pos[2], length[2], r;
	if(uvg_get_sides(poly_id) != 4)
		return;
	uvg_get_uve(uv, poly_id);


	pos[0] = (uv[0] + uv[2] + uv[4] + uv[6]) / 4;
	pos[1] = (uv[1] + uv[3] + uv[5] + uv[7]) / 4;
	
	vec[0] = ((uv[0] - uv[2]) + (uv[6] - uv[4])) / 4;
	vec[1] = ((uv[1] - uv[3]) + (uv[7] - uv[5])) / 4;

	length[0] = sqrt(vec[0] * vec[0] + vec[1] * vec[1]);

	vec[2] = ((uv[2] - uv[4]) + (uv[0] - uv[6])) / 4;
	vec[3] = ((uv[3] - uv[5]) + (uv[1] - uv[7])) / 4;

	length[1] = sqrt(vec[2] * vec[2] + vec[3] * vec[3]);

	vec[0] += vec[3];
	vec[1] += -vec[2];

	r = sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
	vec[0] /= r;
	vec[1] /= r;

	uvg_set_all_corner_uv(poly_id, pos[0] + vec[0] * length[0] + vec[1] * length[1],
								pos[1] + vec[1] * length[0] - vec[0] * length[1],

								pos[0] - vec[0] * length[0] + vec[1] * length[1],
								pos[1] - vec[1] * length[0] - vec[0] * length[1],

								pos[0] - vec[0] * length[0] - vec[1] * length[1],
								pos[1] - vec[1] * length[0] + vec[0] * length[1],

								pos[0] + vec[0] * length[0] - vec[1] * length[1],
								pos[1] + vec[1] * length[0] + vec[0] * length[1]);
}


void uv_tool_poly_rectangle_old(uint poly_id)
{
	uint vertex;
	egreal uv[8], vec[4], pos[2], new_uv[8];
	if(uvg_get_sides(poly_id) != 4)
		return;
	uvg_get_uve(uv, poly_id);


	pos[0] = (uv[0] + uv[2] + uv[4] + uv[6]) / 4;
	pos[1] = (uv[1] + uv[3] + uv[5] + uv[7]) / 4;
	
	vec[0] = ((uv[0] - uv[2]) + (uv[6] - uv[4])) / 4;
	vec[1] = ((uv[1] - uv[3]) + (uv[7] - uv[5])) / 4;

	vec[2] = ((uv[2] - uv[4]) + (uv[0] - uv[6])) / 4;
	vec[3] = ((uv[3] - uv[5]) + (uv[1] - uv[7])) / 4;


	uvg_set_all_corner_uv(poly_id, pos[0] + vec[0] + vec[2],
							pos[1] + vec[1] + vec[3],

							pos[0] - vec[0] + vec[2],
							pos[1] - vec[1] + vec[3],

							pos[0] - vec[0] - vec[2],
							pos[1] - vec[1] - vec[3],

							pos[0] + vec[0] - vec[2],
							pos[1] + vec[1] - vec[3]);
}


void uv_tool_poly_normal_select(uint poly)
{
	uint *ref, id, i;
	egreal uv[8], *vertex, a[3], b[3], c[3], normal[3], r;
	ref = uvg_get_ref();
	vertex = uvg_get_vertex();

	poly *= 4;
	a[0] = vertex[ref[poly + 1] * 3 + 0] - vertex[ref[poly] * 3 + 0];
	a[1] = vertex[ref[poly + 1] * 3 + 1] - vertex[ref[poly] * 3 + 1];
	a[2] = vertex[ref[poly + 1] * 3 + 2] - vertex[ref[poly] * 3 + 2];
	b[0] = vertex[ref[poly + 2] * 3 + 0] - vertex[ref[poly] * 3 + 0];
	b[1] = vertex[ref[poly + 2] * 3 + 1] - vertex[ref[poly] * 3 + 1];
	b[2] = vertex[ref[poly + 2] * 3 + 2] - vertex[ref[poly] * 3 + 2];

	normal[0] = a[1] * b[2] - a[2] * b[1];
	normal[1] = a[2] * b[0] - a[0] * b[2];
	normal[2] = a[0] * b[1] - a[1] * b[0];

	r = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);

	normal[0] /= r;
	normal[1] /= r;
	normal[2] /= r;

	for(id = uvg_get_next_polygon(0); id != -1; id = uvg_get_next_polygon(id + 1))
	{
		if(uvg_get_polygon_select(id))
		{
			a[0] = vertex[ref[id * 4 + 1] * 3 + 0] - vertex[ref[id * 4] * 3 + 0];
			a[1] = vertex[ref[id * 4 + 1] * 3 + 1] - vertex[ref[id * 4] * 3 + 1];
			a[2] = vertex[ref[id * 4 + 1] * 3 + 2] - vertex[ref[id * 4] * 3 + 2];
			b[0] = vertex[ref[id * 4 + 2] * 3 + 0] - vertex[ref[id * 4] * 3 + 0];
			b[1] = vertex[ref[id * 4 + 2] * 3 + 1] - vertex[ref[id * 4] * 3 + 1];
			b[2] = vertex[ref[id * 4 + 2] * 3 + 2] - vertex[ref[id * 4] * 3 + 2];
			c[0] = a[1] * b[2] - a[2] * b[1];
			c[1] = a[2] * b[0] - a[0] * b[2];
			c[2] = a[0] * b[1] - a[1] * b[0];

			r = sqrt(c[0] * c[0] + c[1] * c[1] + c[2] * c[2]);

			c[0] /= r;
			c[1] /= r;
			c[2] /= r;

			if(c[0] * normal[0] + c[1] * normal[1] + c[2] * normal[2] > 0.9)
				uvg_set_all_corner_select(id, 1, 1, 1, 1);
		}
	}
}


void uv_tool_poly_plane_select(uint poly)
{
	uint *ref, id, i;
	egreal uv[8], *vertex, a[3], b[3], normal[3], dist, r, size;
	ref = uvg_get_ref();
	vertex = uvg_get_vertex();

	a[0] = vertex[ref[poly * 4 + 1] * 3 + 0] - vertex[ref[poly * 4] * 3 + 0];
	a[1] = vertex[ref[poly * 4 + 1] * 3 + 1] - vertex[ref[poly * 4] * 3 + 1];
	a[2] = vertex[ref[poly * 4 + 1] * 3 + 2] - vertex[ref[poly * 4] * 3 + 2];
	b[0] = vertex[ref[poly * 4 + 2] * 3 + 0] - vertex[ref[poly * 4] * 3 + 0];
	b[1] = vertex[ref[poly * 4 + 2] * 3 + 1] - vertex[ref[poly * 4] * 3 + 1];
	b[2] = vertex[ref[poly * 4 + 2] * 3 + 2] - vertex[ref[poly * 4] * 3 + 2];

	size = sqrt((a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]) + (a[2] - b[2]) * (a[2] - b[2]));

	normal[0] = a[1] * b[2] - a[2] * b[1];
	normal[1] = a[2] * b[0] - a[0] * b[2];
	normal[2] = a[0] * b[1] - a[1] * b[0];

	r = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);

	normal[0] /= r;
	normal[1] /= r;
	normal[2] /= r;

	dist = vertex[ref[poly * 4] * 3 + 0] * normal[0] + vertex[ref[poly * 4] * 3 + 1] * normal[1] + vertex[ref[poly * 4] * 3 + 2] * normal[2];

	for(id = uvg_get_next_polygon(0); id != -1; id = uvg_get_next_polygon(id + 1))
	{
		if(uvg_get_polygon_select(id))
		{
			for(i = 0; i < uvg_get_sides(id); i++)
			{
				r = (vertex[ref[id * 4 + i] * 3 + 0] - (normal[0] * dist)) * normal[0] +
					(vertex[ref[id * 4 + i] * 3 + 1] - (normal[1] * dist)) * normal[1] +
					(vertex[ref[id * 4 + i] * 3 + 2] - (normal[2] * dist)) * normal[2];
				if(size * 0.01 > r && size * -0.01 < r)
					uvg_set_one_corner_select(id, i, 1);
			}
		}
	}
}



void uv_tool_poly_project(uint poly)
{
	uint *ref, id, i;
	egreal uv[8], uv_center[2] = {0, 0}, *vertex, a[3], b[3], c[3], *s, dist, r, length[2] = {0, 0}, center[3] = {0, 0, 0};
	ref = uvg_get_ref();
	vertex = uvg_get_vertex();

	a[0] = vertex[ref[poly * 4 + 1] * 3 + 0] - vertex[ref[poly * 4] * 3 + 0];
	a[1] = vertex[ref[poly * 4 + 1] * 3 + 1] - vertex[ref[poly * 4] * 3 + 1];
	a[2] = vertex[ref[poly * 4 + 1] * 3 + 2] - vertex[ref[poly * 4] * 3 + 2];
	b[0] = vertex[ref[poly * 4 + 2] * 3 + 0] - vertex[ref[poly * 4] * 3 + 0];
	b[1] = vertex[ref[poly * 4 + 2] * 3 + 1] - vertex[ref[poly * 4] * 3 + 1];
	b[2] = vertex[ref[poly * 4 + 2] * 3 + 2] - vertex[ref[poly * 4] * 3 + 2];

	c[0] = a[1] * b[2] - a[2] * b[1];
	c[1] = a[2] * b[0] - a[0] * b[2];
	c[2] = a[0] * b[1] - a[1] * b[0];

	r = sqrt(c[0] * c[0] + c[1] * c[1] + c[2] * c[2]);

	c[0] /= r;
	c[1] /= r;
	c[2] /= r;
	
	for(i = 0; i < uvg_get_sides(poly); i++)
	{
		center[0] += vertex[ref[poly * 4 + i] * 3 + 0];
		center[1] += vertex[ref[poly * 4 + i] * 3 + 1];
		center[2] += vertex[ref[poly * 4 + i] * 3 + 2];
	}

	center[0] /= uvg_get_sides(poly);
	center[1] /= uvg_get_sides(poly);
	center[2] /= uvg_get_sides(poly);

	if(c[1] < 0.9)
	{
		a[0] = -c[2];
		a[1] = 0;
		a[2] = c[0];
	}else
	{
		a[0] = c[1];
		a[1] = -c[0];
		a[2] = 0;
	}
	r = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
	a[4] = a[0] / r;
	a[5] = a[1] / r;
	a[6] = a[2] / r;
	b[0] = c[1] * a[2] - c[2] * a[1];
	b[1] = c[2] * a[0] - c[0] * a[2];
	b[2] = c[0] * a[1] - c[1] * a[0];
	r = sqrt(b[0] * b[0] + b[1] * b[1] + b[2] * b[2]);
	b[0] = b[0] / r;
	b[1] = b[1] / r;
	b[2] = b[2] / r;

	
	r = 0;
	uvg_get_uve(uv, poly);
	for(i = 0; i < uvg_get_sides(poly); i++)
	{
		uv_center[0] += uv[i * 2] * 0.25;
		uv_center[1] += uv[i * 2 + 1] * 0.25;
		c[0] = vertex[ref[poly * 4 + i] * 3 + 0] - vertex[ref[poly * 4 + (i + 1) % uvg_get_sides(poly)] * 3 + 0];
		c[1] = vertex[ref[poly * 4 + i] * 3 + 1] - vertex[ref[poly * 4 + (i + 1) % uvg_get_sides(poly)] * 3 + 1];
		c[2] = vertex[ref[poly * 4 + i] * 3 + 2] - vertex[ref[poly * 4 + (i + 1) % uvg_get_sides(poly)] * 3 + 2];
		r = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];
		if(r > length[0])
			length[0] = r;
		c[0] = uv[i * 2 + 0] - uv[((i + 1) % uvg_get_sides(poly)) * 2 + 0];
		c[1] = uv[i * 2 + 1] - uv[((i + 1) % uvg_get_sides(poly)) * 2 + 1];
		r = c[0] * c[0] + c[1] * c[1];
		if(r > length[1])
			length[1] = r;
	}
	r = sqrt(length[1]) / sqrt(length[0]);

	for(id = uvg_get_next_polygon(0); id != -1; id = uvg_get_next_polygon(id + 1))
	{
		if(uvg_get_polygon_select(id))
		{
			s = uvg_get_corner_select(id);
			uvg_get_uve(uv, id);
			for(i = 0; i < uvg_get_sides(id); i++)
			{
				c[0] = vertex[ref[id * 4 + i] * 3 + 0] - center[0];
				c[1] = vertex[ref[id * 4 + i] * 3 + 1] - center[1];
				c[2] = vertex[ref[id * 4 + i] * 3 + 2] - center[2];
				uv[i * 2 + 0] = ((c[0] * a[0] + c[1] * a[1] + c[2] * a[2]) * s[i] + uv[i * 2 + 0] * (1 - s[i])) * r + uv_center[0];
				uv[i * 2 + 1] = ((c[0] * b[0] + c[1] * b[1] + c[2] * b[2]) * s[i] + uv[i * 2 + 1] * (1 - s[i])) * r + uv_center[1];
			}
			if(s[0] > 0.001 || s[1] > 0.001 || s[2] > 0.001 || (i == 4 && s[3] > 0.001))
				uvg_set_all_corner_uv(id, uv[0], uv[1], uv[2], uv[3], uv[4], uv[5], uv[6], uv[7]);
		}
	}
}



void uv_tool_project(uint u, uint v, boolean unselected)
{
	uint *ref, id, i;
	egreal uv[8], *vertex, *select, center[4], scale, scale2;
	ref = uvg_get_ref();
	vertex = uvg_get_vertex();
	for(id = uvg_get_next_polygon(0); id != -1; id = uvg_get_next_polygon(id + 1))
	{
		if(uvg_get_polygon_select(id))
		{
			select = uvg_get_corner_select(id);
			for(i = 0; !unselected && i < uvg_get_sides(id) && select[i] < 0.01; i++);
			if(i < uvg_get_sides(id))	 
			{
				center[0] = center[1] = vertex[ref[id * 4 + i] * 3 + u];
				center[2] = center[3] = vertex[ref[id * 4 + i] * 3 + v];
				break;
			}
		}
	}
	for(; id != -1; id = uvg_get_next_polygon(id + 1))
	{
		if(uvg_get_polygon_select(id))
		{
			select = uvg_get_corner_select(id);
			for(i = 0; i < uvg_get_sides(id); i++)
			{
				if(select[i] > 0.1 || unselected)
				{
					if(center[0] < vertex[ref[id * 4 + i] * 3 + u])
						center[0] = vertex[ref[id * 4 + i] * 3 + u];
					if(center[1] > vertex[ref[id * 4 + i] * 3 + u])
						center[1] = vertex[ref[id * 4 + i] * 3 + u];
					if(center[2] < vertex[ref[id * 4 + i] * 3 + v])
						center[2] = vertex[ref[id * 4 + i] * 3 + v];
					if(center[3] > vertex[ref[id * 4 + i] * 3 + v])
						center[3] = vertex[ref[id * 4 + i] * 3 + v];
				}
			}
		}
	}
	if(center[1] - center[0] < center[3] - center[2])
		scale = center[1] - center[0];
	else
		scale = center[3] - center[2];
	center[0] = (center[0] - (center[1] - center[0]) * 0.5) / (center[1] - center[0]);
	center[2] = (center[2] - (center[3] - center[2]) * 0.5) / (center[3] - center[2]);
	for(id = uvg_get_next_polygon(0); id != -1; id = uvg_get_next_polygon(id + 1))
	{
		if(uvg_get_polygon_select(i / 4))
		{
			select = uvg_get_corner_select(id);
			if(unselected)
			{
				uvg_get_uve(uv, id);
				uvg_set_all_corner_uv(id, vertex[ref[id * 4 + 0] * 3 + u] / scale - center[0] - 0.5, 
											vertex[ref[id * 4 + 0] * 3 + v] / scale - center[2] - 0.5,
											vertex[ref[id * 4 + 1] * 3 + u] / scale - center[0] - 0.5, 
											vertex[ref[id * 4 + 1] * 3 + v] / scale - center[2] - 0.5,
											vertex[ref[id * 4 + 2] * 3 + u] / scale - center[0] - 0.5, 
											vertex[ref[id * 4 + 2] * 3 + v] / scale - center[2] - 0.5,
											vertex[ref[id * 4 + 3] * 3 + u] / scale - center[0] - 0.5, 
											vertex[ref[id * 4 + 3] * 3 + v] / scale - center[2] - 0.5);
			}
			else
			{
				for(i = 0; unselected && i < uvg_get_sides(id) && select[i] < 0.01; i++);
				if(i < uvg_get_sides(id))
				{
					uvg_get_uve(uv, id);
					uvg_set_all_corner_uv(id, (vertex[ref[id * 4 + 0] * 3 + u] / scale - center[0] - 0.5) * select[0] + uv[0] * (1 - select[0]), 
												(vertex[ref[id * 4 + 0] * 3 + v] / scale - center[2] - 0.5) * select[0] + uv[1] * (1 - select[0]),
												(vertex[ref[id * 4 + 1] * 3 + u] / scale - center[0] - 0.5) * select[1] + uv[2] * (1 - select[1]), 
												(vertex[ref[id * 4 + 1] * 3 + v] / scale - center[2] - 0.5) * select[1] + uv[3] * (1 - select[1]),
												(vertex[ref[id * 4 + 2] * 3 + u] / scale - center[0] - 0.5) * select[2] + uv[4] * (1 - select[2]), 
												(vertex[ref[id * 4 + 2] * 3 + v] / scale - center[2] - 0.5) * select[2] + uv[5] * (1 - select[2]),
												(vertex[ref[id * 4 + 3] * 3 + u] / scale - center[0] - 0.5) * select[3] + uv[6] * (1 - select[3]), 
												(vertex[ref[id * 4 + 3] * 3 + v] / scale - center[2] - 0.5) * select[3] + uv[7] * (1 - select[3]));
				}
			}
		}
	}
}

void uv_get_quad_vectors(egreal *vectors, uint id)
{
	egreal uv[8], r;
	uvg_get_uve(uv, id);
	vectors[0] = uv[0] - uv[2];
	vectors[1] = uv[1] - uv[3];
	r = sqrt(vectors[0] * vectors[0] + vectors[1] * vectors[1]);
	vectors[0] /= r;
	vectors[1] /= r;
	vectors[2] = uv[2] - uv[4];
	vectors[3] = uv[3] - uv[5];
	r = sqrt(vectors[2] * vectors[2] + vectors[3] * vectors[3]);
	vectors[2] /= r;
	vectors[3] /= r;
	vectors[4] = uv[4] - uv[6];
	vectors[5] = uv[5] - uv[7];
	r = sqrt(vectors[4] * vectors[4] + vectors[5] * vectors[5]);
	vectors[4] /= r;
	vectors[5] /= r;
	vectors[6] = uv[6] - uv[0];
	vectors[7] = uv[7] - uv[1];
	r = sqrt(vectors[6] * vectors[6] + vectors[7] * vectors[7]);
	vectors[6] /= r;
	vectors[7] /= r;	
}

void uv_get_tri_vectors(egreal *vectors, uint id)
{
	egreal uv[8], r;
	uvg_get_uve(uv, id);
	vectors[0] = uv[0] - uv[2];
	vectors[1] = uv[1] - uv[3];
	r = sqrt(vectors[0] * vectors[0] + vectors[1] * vectors[1]);
	vectors[0] /= r;
	vectors[1] /= r;
	vectors[2] = uv[2] - uv[4];
	vectors[3] = uv[3] - uv[5];
	r = sqrt(vectors[2] * vectors[2] + vectors[3] * vectors[3]);
	vectors[2] /= r;
	vectors[3] /= r;
	vectors[4] = uv[4] - uv[0];
	vectors[5] = uv[5] - uv[1];
	r = sqrt(vectors[4] * vectors[4] + vectors[5] * vectors[5]);
	vectors[4] /= r;
	vectors[5] /= r;	
}

void uv_tool_poly_stack(uint poly)
{
	egreal v1[8],  v2[8], uv[8], *select, sums[4], r;
	uint id, i, found = 0;
	uvg_get_uve(uv, poly);
	if(uvg_is_quad(poly))
	{
		uv_get_quad_vectors(v1, poly);
		for(id = uvg_get_next_polygon(0); id != -1; id = uvg_get_next_polygon(id + 1))
		{
			if(uvg_get_polygon_select(id) && id != poly)
			{
				select = uvg_get_corner_select(id);
				if(select[0] > 0.01 && select[1] > 0.01 && select[2] > 0.01 && select[3] > 0.01)
				{
					uv_get_quad_vectors(v2, id);
					sums[0] = v2[0] * v1[0] + v2[1] * v1[1]
							+ v2[2] * v1[2] + v2[3] * v1[3]
							+ v2[4] * v1[4] + v2[5] * v1[5]
							+ v2[6] * v1[6] + v2[7] * v1[7];

					sums[1] = v2[0] * v1[2] + v2[1] * v1[3]
							+ v2[2] * v1[4] + v2[3] * v1[5]
							+ v2[4] * v1[6] + v2[5] * v1[7]
							+ v2[6] * v1[0] + v2[7] * v1[1];

					sums[2] = v2[0] * v1[4] + v2[1] * v1[5]
							+ v2[2] * v1[6] + v2[3] * v1[7]
							+ v2[4] * v1[0] + v2[5] * v1[1]
							+ v2[6] * v1[2] + v2[7] * v1[3];

					sums[3] = v2[0] * v1[6] + v2[1] * v1[7]
							+ v2[2] * v1[0] + v2[3] * v1[1]
							+ v2[4] * v1[2] + v2[5] * v1[3]
							+ v2[6] * v1[4] + v2[7] * v1[5];
					r = 0;
					for(i = 0; i < 4; i++)
					{
						if(sums[i] > r)
						{
							r = sums[i];
							found = i;
						}
					}
					for(i = 0; i < 4; i++)
					{
						if(-sums[i] > r)
						{
							r = -sums[i];
							found = i + 4;
						}
					}
					printf("found == %f\n", found);
					if(found > 3)
						uvg_set_all_corner_uv(id, uv[(found % 4) * 2 + 0],
												uv[(found % 4) * 2 + 1],
												uv[((found + 1) % 4) * 2 + 0],
												uv[((found + 1) % 4) * 2 + 1],
												uv[((found + 2) % 4) * 2 + 0],
												uv[((found + 2) % 4) * 2 + 1],
												uv[((found + 3) % 4) * 2 + 0],
												uv[((found + 3) % 4) * 2 + 1]);
					else 
						uvg_set_all_corner_uv(id, uv[(found % 4) * 2 + 0],
												uv[(found % 4) * 2 + 1],
												uv[((found + 1) % 4) * 2 + 0],
												uv[((found + 1) % 4) * 2 + 1],
												uv[((found + 2) % 4) * 2 + 0],
												uv[((found + 2) % 4) * 2 + 1],
												uv[((found + 3) % 4) * 2 + 0],
												uv[((found + 3) % 4) * 2 + 1]);
				}
			}
		}
	}
}
