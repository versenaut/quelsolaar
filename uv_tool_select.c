#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"
#include "seduce.h"

#include "uv_geometry.h"

extern void uv_trans_get_pos(egreal *pos);

void uv_tool_flip_x()
{
	uint id, i;
	egreal uv[8], center[2], *s;
	uv_trans_get_pos(center);
	for(id = uvg_get_next_polygon(0); id != -1; id = uvg_get_next_polygon(id + 1))
	{
		if(uvg_get_polygon_select(id))
		{
			s = uvg_get_corner_select(id);
			uvg_get_uve(uv, id);
			for(i = 0; i < uvg_get_sides(id); i++)
				uv[i * 2] = center[0] * 2 - uv[i * 2];
			if(s[0] > 0.001 || s[1] > 0.001 || s[2] > 0.001 || (i == 4 && s[3] > 0.001))
				uvg_set_all_corner_uv(id, uv[0], uv[1], uv[2], uv[3], uv[4], uv[5], uv[6], uv[7]);
		}
	}
}

void uv_tool_flip_y()
{
	uint id, i;
	egreal uv[8], center[2], *s;
	uv_trans_get_pos(center);
	for(id = uvg_get_next_polygon(0); id != -1; id = uvg_get_next_polygon(id + 1))
	{
		if(uvg_get_polygon_select(id))
		{
			s = uvg_get_corner_select(id);
			uvg_get_uve(uv, id);
			for(i = 0; i < uvg_get_sides(id); i++)
				uv[i * 2 + 1] = center[1] * 2 - uv[i * 2 + 1];
			if(s[0] > 0.001 || s[1] > 0.001 || s[2] > 0.001 || (i == 4 && s[3] > 0.001))
				uvg_set_all_corner_uv(id, uv[0], uv[1], uv[2], uv[3], uv[4], uv[5], uv[6], uv[7]);
		}
	}
}

void uv_find_square(egreal *pos, egreal *scale)
{
	uint id, i;
	egreal uv[8], *s;
	pos[0] = 100000000;
	pos[1] = 100000000;
	scale[0] = -100000000;
	scale[1] = -100000000;
	for(id = uvg_get_next_polygon(0); id != -1; id = uvg_get_next_polygon(id + 1))
	{
		if(uvg_get_polygon_select(id))
		{
			s = uvg_get_corner_select(id);
			uvg_get_uve(uv, id);
			for(i = 0; i < uvg_get_sides(id); i++)
			{
				if(s[i] > 0.01)
				{
					if(uv[i * 2] < pos[0])
						pos[0] = uv[i * 2];
					if(uv[i * 2 + 1] < pos[1])
						pos[1] = uv[i * 2 + 1];
					if(uv[i * 2] > scale[0])
						scale[0] = uv[i * 2];
					if(uv[i * 2 + 1] > scale[1])
						scale[1] = uv[i * 2 + 1];
				}
			}
		}
	}
	scale[0] -= pos[0];
	scale[1] -= pos[1];
}

void uv_fit_selection(boolean preserve_aspect)
{
	uint id, i;
	egreal uv[8], pos[2], scale[2], *s;
	uv_find_square(pos, scale);
	if(preserve_aspect)
	{
		if(scale[0] < scale[1])
			scale[0] = scale[1];
		else
			scale[1] = scale[0];
	}
	for(id = uvg_get_next_polygon(0); id != -1; id = uvg_get_next_polygon(id + 1))
	{
		if(uvg_get_polygon_select(id))
		{
			s = uvg_get_corner_select(id);
			uvg_get_uve(uv, id);
			if(s[0] > 0.01 || s[1] > 0.01 || s[2] > 0.01 || s[3] > 0.01)
			{
				uv[0] = uv[0] * (1 - s[0]) + (uv[0] - pos[0]) / scale[0] * s[0];
				uv[1] = uv[1] * (1 - s[0]) + (uv[1] - pos[1]) / scale[1] * s[0];
				uv[2] = uv[2] * (1 - s[1]) + (uv[2] - pos[0]) / scale[0] * s[1];
				uv[3] = uv[3] * (1 - s[1]) + (uv[3] - pos[1]) / scale[1] * s[1];
				uv[4] = uv[4] * (1 - s[2]) + (uv[4] - pos[0]) / scale[0] * s[2];
				uv[5] = uv[5] * (1 - s[2]) + (uv[5] - pos[1]) / scale[1] * s[2];
				uv[6] = uv[6] * (1 - s[3]) + (uv[6] - pos[0]) / scale[0] * s[3];
				uv[7] = uv[7] * (1 - s[3]) + (uv[7] - pos[1]) / scale[1] * s[3];
				uvg_set_all_corner_uv(id, uv[0], uv[1], uv[2], uv[3], uv[4], uv[5], uv[6], uv[7]);
			}
		}
	}
}
