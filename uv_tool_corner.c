#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"
#include "seduce.h"

#include "uv_geometry.h"

void uv_tool_corner_colaps(uint poly_id, uint corner)
{
	uint vertex, *ref, length, i;
	egreal uv[8];
	uvg_get_uve(uv, poly_id);
	ref = uvg_get_ref();
	vertex = ref[poly_id * 4 + corner];
	length = uvg_get_polygon_length() * 4;
	for(i = 0; i < length; i++)
		if(uvg_get_polygon_select(i))
			if(ref[i] == vertex && uvg_get_next_polygon(i / 4) == i / 4 && uvg_get_polygon_select(i / 4))
				uvg_set_one_corner_uv(i / 4, i % 4, uv[corner * 2], uv[corner * 2 + 1]);
}

void uv_tool_corner_select(uint poly_id, uint corner)
{
	uint vertex, *ref, length, i;
	ref = uvg_get_ref();
	vertex = ref[poly_id * 4 + corner];
	length = uvg_get_polygon_length() * 4;
	for(i = 0; i < length; i++)
		if(uvg_get_polygon_select(i))
			if(ref[i] == vertex && uvg_get_next_polygon(i / 4) == i / 4 && uvg_get_polygon_select(i / 4))
				uvg_set_one_corner_select(i / 4, i % 4, 1);
}

void uv_streight_matrix(egreal *matrix, egreal vec1_x, egreal vec1_y, egreal vec2_x, egreal vec2_y)
{
	egreal mid[2];

	mid[0] = vec1_x + vec2_x;
	mid[1] = vec1_y + vec2_y;
	if(mid[0] < 0)
	{
		if(mid[1] < 0)
		{
			if(vec1_x - vec1_y > vec2_x - vec2_y)
			{
				matrix[0] = 0;
				matrix[1] = -1;
				matrix[2] = -1;
				matrix[3] = 0;
			}else
			{
				matrix[0] = -1;
				matrix[1] = 0;
				matrix[2] = 0;
				matrix[3] = -1;
			}
		}else
		{
			if(vec1_x + vec1_y < vec2_x + vec2_y)
			{
				matrix[0] = -1;
				matrix[1] = 0;
				matrix[2] = 0;
				matrix[3] = 1;
			}else
			{
				matrix[0] = 0;
				matrix[1] = 1;
				matrix[2] = -1;
				matrix[3] = 0;
			}
		}
	}else
	{
		if(mid[1] < 0)
		{
			if(vec1_x - vec1_y < vec2_x - vec2_y)
			{
				matrix[0] = 1;
				matrix[1] = 0;
				matrix[2] = 0;
				matrix[3] = -1;
			}else
			{
				matrix[0] = 0;
				matrix[1] = 1;
				matrix[2] = -1;
				matrix[3] = 0;
			}
		}else
		{
			if(vec1_x + vec1_y > vec2_x + vec2_y)
			{
				matrix[0] = 0;
				matrix[1] = 1;
				matrix[2] = 1;
				matrix[3] = 0;	
			}else
			{
				matrix[0] = 1;
				matrix[1] = 0;
				matrix[2] = 0;
				matrix[3] = 1;
			}
		}
	}
}

void uv_tool_edge_straight_uv(uint poly_id, uint corner)
{
	uint vertex, *ref, length, i, sides;
	egreal uv[8], matrix[4], matrix2[4], center[2], r, new_uv[8], tmp[2];
	uvg_get_uve(uv, poly_id);
	sides = uvg_get_sides(poly_id);

	center[0] = uv[corner * 2];
	center[1] = uv[corner * 2 + 1];

	matrix[0] = uv[((corner + 1) % sides) * 2] - center[0];
	matrix[1] = uv[((corner + 1) % sides) * 2 + 1] - center[1];

	matrix[2] = uv[((corner + sides - 1) % sides) * 2] - center[0];
	matrix[3] = uv[((corner + sides - 1) % sides) * 2 + 1] - center[1];

	r = sqrt(matrix[0] * matrix[0] + matrix[1] * matrix[1]);
	matrix[0] /= r;
	matrix[1] /= r;

	r = sqrt(matrix[2] * matrix[2] + matrix[3] * matrix[3]);
	matrix[2] /= r;
	matrix[3] /= r;


	r = matrix[1] * matrix[2] + matrix[0] * matrix[3];


	uv_streight_matrix(matrix2, matrix[0], matrix[1], matrix[2], matrix[3]);

	new_uv[0] = (uv[0] - center[0]) * matrix[0] / r + (uv[1] - center[1]) * -matrix[2] / r;
	new_uv[1] = (uv[0] - center[0]) * -matrix[1] / r + (uv[1] - center[1]) * matrix[3] / r;

	new_uv[2] = (uv[2] - center[0]) * matrix[0] / r + (uv[3] - center[1]) * -matrix[2] / r;
	new_uv[3] = (uv[2] - center[0]) * -matrix[1] / r + (uv[3] - center[1]) * matrix[3] / r;

	new_uv[4] = (uv[4] - center[0]) * matrix[0] / r + (uv[5] - center[1]) * -matrix[2] / r;
	new_uv[5] = (uv[4] - center[0]) * -matrix[1] / r + (uv[5] - center[1]) * matrix[3] / r;

	new_uv[6] = (uv[6] - center[0]) * matrix[0] / r + (uv[7] - center[1]) * -matrix[2] / r;
	new_uv[7] = (uv[6] - center[0]) * -matrix[1] / r + (uv[7] - center[1]) * matrix[3] / r;
	uvg_set_all_corner_uv(poly_id, new_uv[0] * matrix[0] + new_uv[1] * matrix[1] + center[0],
									new_uv[0] * matrix[2] + new_uv[1] * matrix[3] + center[1],

									new_uv[2] * matrix[0] + new_uv[3] * matrix[1] + center[0],
									new_uv[2] * matrix[2] + new_uv[3] * matrix[3] + center[1],

									new_uv[4] * matrix[0] + new_uv[5] * matrix[1] + center[0],
									new_uv[4] * matrix[2] + new_uv[5] * matrix[3] + center[1],

									new_uv[6] * matrix[0] + new_uv[7] * matrix[1] + center[0],
									new_uv[6] * matrix[2] + new_uv[7] * matrix[3] + center[1]);
}
