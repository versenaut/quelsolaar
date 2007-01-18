#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"
#include "seduce.h"

#include "uv_geometry.h"

void uv_find_staight_edge(egreal *matrix, egreal *pos, uint poly_id, uint edge)
{
	egreal uv[8], r;
	uvg_get_uve(uv, poly_id);

	matrix[0] = uv[edge * 2] - uv[((edge + 1) % uvg_get_sides(poly_id)) * 2];
	matrix[1] = uv[edge * 2 + 1] - uv[((edge + 1) % uvg_get_sides(poly_id)) * 2 + 1];
	r = sqrt(matrix[0] * matrix[0] + matrix[1] * matrix[1]);
	matrix[0] /= r;
	matrix[1] /= r;
	if(matrix[0] + matrix[1] < 0)
	{
		if(matrix[0] - matrix[1] < 0)
		{
			matrix[1] = -matrix[1];
			matrix[0] = -matrix[0];
		}else
		{
			r = matrix[1];
			matrix[1] = matrix[0];
			matrix[0] = -r;
		}
	}else
	{
		if(matrix[0] - matrix[1] > 0)
		{
			matrix[0] = matrix[0];
			matrix[1] = matrix[1];
		}else
		{
			r = matrix[1];
			matrix[1] = -matrix[0];
			matrix[0] = r;

		}
	}

	matrix[2] = -matrix[1];
	matrix[3] = matrix[0];

	pos[0] = (uv[edge * 2] + uv[((edge + 1) % uvg_get_sides(poly_id)) * 2]) * 0.5;
	pos[1] = (uv[edge * 2 + 1] + uv[((edge + 1) % uvg_get_sides(poly_id)) * 2 + 1]) * 0.5;
}


void uv_tool_edge_streight(uint poly_id, uint edge)
{
	uint i;
	egreal uv[8], matrix[4], pos[2], r;

	uv_find_staight_edge(matrix, pos, poly_id, edge);

	uvg_get_uve(uv, poly_id);
	for(i = 0; i < 8; i++)
		uv[i] = uv[i] - pos[i % 2];

	poly_id *= 2;
	r = (uv[poly_id] * matrix[0] + uv[poly_id + 1] * matrix[1]) + pos[0];
	uv[poly_id + 1] = (uv[poly_id] * matrix[2] + uv[poly_id + 1] * matrix[3]) + pos[1];
	uv[poly_id] = r; 

	uvg_set_all_corner_uv(poly_id / 2, uv[0], uv[1], uv[2], uv[3], uv[4], uv[5], uv[6], uv[7]);
}


void uv_tool_edge_select_streight(uint poly_id, uint edge)
{
	uint i, id;
	egreal uv[8], matrix[4], pos[2], *s, new_uv[8];

	uv_find_staight_edge(matrix, pos, poly_id, edge);

	for(id = uvg_get_next_polygon(0); id != ~0u; id = uvg_get_next_polygon(id + 1))
	{
		if(uvg_get_polygon_select(id))
		{
			s = uvg_get_corner_select(id);
			for(i = 0; i < uvg_get_sides(id) && s[i] > 0.01; i++)
			if(i < uvg_get_sides(id) || id == poly_id)
			{
				uvg_get_uve(uv, id);
				for(i = 0; i < 8; i++)
					uv[i] = uv[i] - pos[i % 2];

				new_uv[0] = (uv[0] * matrix[0] + uv[1] * matrix[1]) + pos[0];
				new_uv[1] = (uv[0] * matrix[2] + uv[1] * matrix[3]) + pos[1];

				new_uv[2] = (uv[2] * matrix[0] + uv[3] * matrix[1]) + pos[0];
				new_uv[3] = (uv[2] * matrix[2] + uv[3] * matrix[3]) + pos[1];

				new_uv[4] = (uv[4] * matrix[0] + uv[5] * matrix[1]) + pos[0];
				new_uv[5] = (uv[4] * matrix[2] + uv[5] * matrix[3]) + pos[1];

				new_uv[6] = (uv[6] * matrix[0] + uv[7] * matrix[1]) + pos[0];
				new_uv[7] = (uv[6] * matrix[2] + uv[7] * matrix[3]) + pos[1];

				if(id == poly_id)
					uvg_set_all_corner_uv(id, new_uv[0], new_uv[1], new_uv[2], new_uv[3], new_uv[4], new_uv[5], new_uv[6], new_uv[7]);
				else
					uvg_set_all_corner_uv(id, new_uv[0] * s[0] + uv[0] * (1 - s[0]),
												new_uv[1] * s[0] + uv[1] * (1 - s[0]),
												new_uv[2] * s[1] + uv[2] * (1 - s[1]),
												new_uv[3] * s[1] + uv[3] * (1 - s[1]),
												new_uv[4] * s[2] + uv[4] * (1 - s[2]),
												new_uv[5] * s[2] + uv[5] * (1 - s[2]),
												new_uv[6] * s[3] + uv[6] * (1 - s[3]),
												new_uv[7] * s[3] + uv[7] * (1 - s[3]));
			}
		}
	}
}


void uv_tool_edge_poly_streight(uint poly_id, uint edge)
{
	uint i;
	egreal uv[8], matrix[4], pos[2], new_uv[8];

	uv_find_staight_edge(matrix, pos, poly_id, edge);

	uvg_get_uve(uv, poly_id);
	for(i = 0; i < 8; i++)
		uv[i] = uv[i] - pos[i % 2];

	new_uv[0] = (uv[0] * matrix[0] + uv[1] * matrix[1]) + pos[0];
	new_uv[1] = (uv[0] * matrix[2] + uv[1] * matrix[3]) + pos[1];

	new_uv[2] = (uv[2] * matrix[0] + uv[3] * matrix[1]) + pos[0];
	new_uv[3] = (uv[2] * matrix[2] + uv[3] * matrix[3]) + pos[1];

	new_uv[4] = (uv[4] * matrix[0] + uv[5] * matrix[1]) + pos[0];
	new_uv[5] = (uv[4] * matrix[2] + uv[5] * matrix[3]) + pos[1];

	new_uv[6] = (uv[6] * matrix[0] + uv[7] * matrix[1]) + pos[0];
	new_uv[7] = (uv[6] * matrix[2] + uv[7] * matrix[3]) + pos[1];

	uvg_set_all_corner_uv(poly_id, new_uv[0], new_uv[1], new_uv[2], new_uv[3], new_uv[4], new_uv[5], new_uv[6], new_uv[7]);
}
