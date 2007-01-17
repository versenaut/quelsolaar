#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"
#include "seduce.h"
#include "uv.h"
#include "uv_geometry.h"

float view_cam_pos[3] = {-0.5, -0.5, 1};
float view_cam_pos_goal[3] = {-0.5, -0.5, 1};
float view_cam_move_start[2];
float view_cam_speed = 0.1;

void uv_handle_view(BInputState *input, float *pos, boolean active)
{
	if(input->mode == BAM_DRAW)
	{
		glDisable(GL_DEPTH_TEST);
		glPushMatrix();
		glTranslatef(view_cam_pos[0], view_cam_pos[1], -view_cam_pos[2]);

	}else
	{
		if(active && input->mouse_button[1])
		{
			if(input->mouse_button[0])
			{
				view_cam_pos_goal[2] *= 1 + 2 * ((input->pointer_y - view_cam_move_start[1]) + (input->pointer_x - view_cam_move_start[0]));
				if(view_cam_pos_goal[2] < 0.05)
					view_cam_pos_goal[2] = 0.05;
				view_cam_speed = 0.1;
			}else
			{
				view_cam_pos_goal[0] += (input->pointer_x - view_cam_move_start[0]) * view_cam_pos[2];
				view_cam_pos_goal[1] += (input->pointer_y - view_cam_move_start[1]) * view_cam_pos[2];
				view_cam_speed = 0.3;
			}
		}/*else if(active && input->mouse_button[0])
		if(0.025 * 0.025 < (input->pointer_x - input->click_pointer_x) * (input->pointer_x - input->click_pointer_x) + (input->pointer_y - input->click_pointer_y) * (input->pointer_y - input->click_pointer_y))
		{
			if(input->click_pointer_x > 0.9 || input->click_pointer_x < -0.9)
			{
				view_cam_pos_goal[2] *= 1 + 2 * ((input->pointer_y - view_cam_move_start[1]) + (input->pointer_x - view_cam_move_start[0]));
				if(view_cam_pos_goal[2] < 0.05)
					view_cam_pos_goal[2] = 0.05;
				view_cam_speed = 0.1;
			}else
			{
				view_cam_pos_goal[0] += (input->pointer_x - view_cam_move_start[0]) * view_cam_pos[2];
				view_cam_pos_goal[1] += (input->pointer_y - view_cam_move_start[1]) * view_cam_pos[2];
				view_cam_speed = 0.3;
			}
		}*/
		view_cam_move_start[0] = input->pointer_x;
		view_cam_move_start[1] = input->pointer_y;
		view_cam_pos[0] = (view_cam_pos_goal[0] * view_cam_speed) + (view_cam_pos[0] * (1.0 - view_cam_speed));
		view_cam_pos[1] = (view_cam_pos_goal[1] * view_cam_speed) + (view_cam_pos[1] * (1.0 - view_cam_speed));
		view_cam_pos[2] = (view_cam_pos_goal[2] * view_cam_speed) + (view_cam_pos[2] * (1.0 - view_cam_speed));
	}
	pos[0] = view_cam_pos[0];
	pos[1] = view_cam_pos[1];
	pos[2] = view_cam_pos[2];
} 

float uv_get_pos_x(float x)
{
	return x * view_cam_pos[2] - view_cam_pos[0];
}

float uv_get_pos_y(float y)
{
	return y * view_cam_pos[2] - view_cam_pos[1];
}

float uv_get_view_x(float x)
{
	return (x + view_cam_pos[0]) / view_cam_pos[2];
}

float uv_get_view_y(float y)
{
	return (y + view_cam_pos[1]) / view_cam_pos[2];
}

void uv_center(float x, float y)
{
	view_cam_pos[0] = x;
	view_cam_pos[1] = y;
	view_cam_pos[2] = 4;
	view_cam_speed = 0.9;
}

float *get_cam_pos(void)
{
	return view_cam_pos;
}
/*
	x - uv[0 * 2 + 0] * uv[1 * 2 + 1] - uv[0 * 2 + 1] + y - uv[0 * 2 + 1] *	uv[0 * 2 + 0] - uv[1 * 2 + 0]

	x - uv[1 * 2 + 0] * uv[2 * 2 + 1] - uv[1 * 2 + 1] + y - uv[1 * 2 + 1] *	uv[1 * 2 + 0] - uv[2 * 2 + 0]

	x - uv[2 * 2 + 0] * uv[3 * 2 + 1] - uv[2 * 2 + 1] + y - uv[2 * 2 + 1] *	uv[2 * 2 + 0] - uv[3 * 2 + 0]

	x - uv[3 * 2 + 0] * uv[4 * 2 + 1] - uv[3 * 2 + 1] + y - uv[3 * 2 + 1] *	uv[3 * 2 + 0] - uv[4 * 2 + 0]

*/
boolean uv_test_tri(egreal *uv, float x, float y)
{
	if(0 > (x - uv[0 * 2 + 0]) * (uv[1 * 2 + 1] - uv[0 * 2 + 1]) + (y - uv[0 * 2 + 1]) * (uv[0 * 2 + 0] - uv[1 * 2 + 0]))
	{
		if(0 > (x - uv[1 * 2 + 0]) * (uv[2 * 2 + 1] - uv[1 * 2 + 1]) + (y - uv[1 * 2 + 1]) * (uv[1 * 2 + 0] - uv[2 * 2 + 0]))
			if(0 > (x - uv[2 * 2 + 0]) * (uv[0 * 2 + 1] - uv[2 * 2 + 1]) + (y - uv[2 * 2 + 1]) * (uv[2 * 2 + 0] - uv[0 * 2 + 0]))
					return TRUE;
	}else
	{
		if(0 < (x - uv[1 * 2 + 0]) * (uv[2 * 2 + 1] - uv[1 * 2 + 1]) + (y - uv[1 * 2 + 1]) * (uv[1 * 2 + 0] - uv[2 * 2 + 0]))
			if(0 < (x - uv[2 * 2 + 0]) * (uv[0 * 2 + 1] - uv[2 * 2 + 1]) + (y - uv[2 * 2 + 1]) * (uv[2 * 2 + 0] - uv[0 * 2 + 0]))
					return TRUE;
	}
	return FALSE;
}

boolean uv_test_quad(egreal *uv, float x, float y)
{
	if(0 > (x - uv[0 * 2 + 0]) * (uv[1 * 2 + 1] - uv[0 * 2 + 1]) + (y - uv[0 * 2 + 1]) * (uv[0 * 2 + 0] - uv[1 * 2 + 0]))
	{
		if(0 > (x - uv[1 * 2 + 0]) * (uv[2 * 2 + 1] - uv[1 * 2 + 1]) + (y - uv[1 * 2 + 1]) * (uv[1 * 2 + 0] - uv[2 * 2 + 0]))
			if(0 > (x - uv[2 * 2 + 0]) * (uv[3 * 2 + 1] - uv[2 * 2 + 1]) + (y - uv[2 * 2 + 1]) * (uv[2 * 2 + 0] - uv[3 * 2 + 0]))
				if(0 > (x - uv[3 * 2 + 0]) * (uv[0 * 2 + 1] - uv[3 * 2 + 1]) + (y - uv[3 * 2 + 1]) * (uv[3 * 2 + 0] - uv[0 * 2 + 0]))
					return TRUE;
	}else
	{
		if(0 < (x - uv[1 * 2 + 0]) * (uv[2 * 2 + 1] - uv[1 * 2 + 1]) + (y - uv[1 * 2 + 1]) * (uv[1 * 2 + 0] - uv[2 * 2 + 0]))
			if(0 < (x - uv[2 * 2 + 0]) * (uv[3 * 2 + 1] - uv[2 * 2 + 1]) + (y - uv[2 * 2 + 1]) * (uv[2 * 2 + 0] - uv[3 * 2 + 0]))
				if(0 < (x - uv[3 * 2 + 0]) * (uv[0 * 2 + 1] - uv[3 * 2 + 1]) + (y - uv[3 * 2 + 1]) * (uv[3 * 2 + 0] - uv[0 * 2 + 0]))
					return TRUE;
	}
	return FALSE;
}

boolean uv_test_broken_quad(egreal *uv, float x, float y)
{
	if(0 > (x - uv[0 * 2 + 0]) * (uv[1 * 2 + 1] - uv[0 * 2 + 1]) + (y - uv[0 * 2 + 1]) * (uv[0 * 2 + 0] - uv[1 * 2 + 0]))
	{
		if(0 > (x - uv[1 * 2 + 0]) * (uv[2 * 2 + 1] - uv[1 * 2 + 1]) + (y - uv[1 * 2 + 1]) * (uv[1 * 2 + 0] - uv[2 * 2 + 0]))
			if(0 > (x - uv[2 * 2 + 0]) * (uv[0 * 2 + 1] - uv[2 * 2 + 1]) + (y - uv[2 * 2 + 1]) * (uv[2 * 2 + 0] - uv[0 * 2 + 0]))
					return TRUE;
	}else
	{
		if(0 < (x - uv[1 * 2 + 0]) * (uv[2 * 2 + 1] - uv[1 * 2 + 1]) + (y - uv[1 * 2 + 1]) * (uv[1 * 2 + 0] - uv[2 * 2 + 0]))
			if(0 < (x - uv[2 * 2 + 0]) * (uv[0 * 2 + 1] - uv[2 * 2 + 1]) + (y - uv[2 * 2 + 1]) * (uv[2 * 2 + 0] - uv[0 * 2 + 0]))
					return TRUE;
	}
	if(0 > (x - uv[0 * 2 + 0]) * (uv[3 * 2 + 1] - uv[0 * 2 + 1]) + (y - uv[0 * 2 + 1]) * (uv[0 * 2 + 0] - uv[3 * 2 + 0]))
	{
		if(0 > (x - uv[3 * 2 + 0]) * (uv[2 * 2 + 1] - uv[3 * 2 + 1]) + (y - uv[3 * 2 + 1]) * (uv[3 * 2 + 0] - uv[2 * 2 + 0]))
			if(0 > (x - uv[2 * 2 + 0]) * (uv[0 * 2 + 1] - uv[2 * 2 + 1]) + (y - uv[2 * 2 + 1]) * (uv[2 * 2 + 0] - uv[0 * 2 + 0]))
					return TRUE;
	}else
	{
		if(0 < (x - uv[3 * 2 + 0]) * (uv[2 * 2 + 1] - uv[3 * 2 + 1]) + (y - uv[3 * 2 + 1]) * (uv[3 * 2 + 0] - uv[2 * 2 + 0]))
			if(0 < (x - uv[2 * 2 + 0]) * (uv[0 * 2 + 1] - uv[2 * 2 + 1]) + (y - uv[2 * 2 + 1]) * (uv[2 * 2 + 0] - uv[0 * 2 + 0]))
					return TRUE;
	}
	return FALSE;
}

uint uv_test_corner(uint id, float x, float y)
{
	egreal uv[8];
	egreal tmp[8];
	uint i;
	uvg_get_uve(uv, id);
	if(uvg_is_quad(id))
	{
		for(i = 0; i < 4; i++)
		{
			get_quad_corner(tmp, i, uv, 0);
			if(uv_test_quad(tmp, x, y))
				return i;
		}
	}else
	{
		for(i = 0; i < 3; i++)
		{
			get_tri_corner(tmp, i, uv, 0);
			if(uv_test_tri(tmp, x, y))
				return i;
		}
	}
	return -1;
}

uint uv_test_edge(uint id, float x, float y)
{
	egreal uv[8];
	egreal tmp[8];
	uint i;
	uvg_get_uve(uv, id);
	if(uvg_is_quad(id))
	{
		for(i = 0; i < 4; i++)
		{
			get_quad_edge(tmp, i, uv);
			if(uv_test_quad(tmp, x, y))
				return i;
		}
	}else
	{
		for(i = 0; i < 3; i++)
		{
			get_tri_edge(tmp, i, uv);
			if(uv_test_quad(tmp, x, y))
				return i;
		}
	}
	return -1;
}

boolean uv_test_polygon(uint id, float x, float y)
{
	egreal uv[8];
	uvg_get_uve(uv, id);
	
	if(uvg_is_quad(id))
	{
		if(uv_test_quad(uv, x, y))
			return TRUE;
	}
	else
	{
		if(uv_test_tri(uv, x, y))
			return TRUE;
	}
	return FALSE;
}

uint uv_test_all_polygons(float x, float y)
{
	uint id;
	egreal uv[8];
	for(id = uvg_get_next_polygon(0); id != -1; id = uvg_get_next_polygon(id + 1))
	{
		if(uvg_get_polygon_select(id))
		{
			uvg_get_uve(uv, id);
			if(uvg_is_quad(id))
			{
				if(uv_test_quad(uv, x, y))
					return id;
			}
			else
			{
				if(uv_test_tri(uv, x, y))
					return id;
			}
		}
	}
	for(id = uvg_get_next_polygon(0); id != -1; id = uvg_get_next_polygon(id + 1))
	{
		if(uvg_get_polygon_select(id) && uvg_is_quad(id))
		{
			uvg_get_uve(uv, id);
			if(uv_test_broken_quad(uv, x, y))
				return id;
		}
	}
	return -1;
}

void uv_find_closest_corner(egreal *output, float x, float y, boolean selected)
{
	uint i, id;
	egreal uv[8], *select, f, shortest;

	output[0] = 0;
	output[1] = 0;
	shortest = x * x + y * y;

	f = x * x + (y - 1) * (y - 1);
	if(f < shortest)
	{
		output[1] = 1;
		shortest = f;
	}
	f = (x - 1) * (x - 1) + y * y;
	if(f < shortest)
	{
		output[0] = 1;
		output[1] = 0;
		shortest = f;
	}
	f = (x - 1) * (x - 1) + (y - 1) * (y - 1);
	if(f < shortest)
	{
		output[0] = 1;
		output[1] = 1;
		shortest = f;
	}

	for(id = uvg_get_next_polygon(0); id != -1; id = uvg_get_next_polygon(id + 1))
	{
		uvg_get_uve(uv, id);
		select = uvg_get_corner_select(id);
		for(i = 0; i < uvg_get_sides(id) * 2; i += 2)
		{
			if(select[i / 2] < 0.01 || selected)
			{
				f = (x - uv[i]) * (x - uv[i]) + (y - uv[i + 1]) * (y - uv[i + 1]);
				if(f < shortest)
				{
					output[0] = uv[i];
					output[1] = uv[i + 1];
					shortest = f;
				}
			}
		}
	}
}
