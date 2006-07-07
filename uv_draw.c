#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"
#include "seduce.h"
#include "uv.h"
#include "uv_geometry.h"
#include <math.h>

#define DRAW_LINE_LENGTH 4096
#define RING_SECTIONS 17
#define DRAW_CLOSE_RANGE 0.05
#define DRAW_SEGMENT_LENGTH 0.008

struct{
	float		*array;
	uint		array_length;
	float		*vertex;
	uint		vertex_length;
	uint		vertex_tie_length;
	float		*ring;
	boolean		select_length;
}GlobalDrawLine;

void uv_init_draw_line()
{
	uint i;
	GlobalDrawLine.array = malloc((sizeof *GlobalDrawLine.array) * ((DRAW_LINE_LENGTH * 2) + 2));
	GlobalDrawLine.vertex = malloc((sizeof *GlobalDrawLine.vertex) * DRAW_LINE_LENGTH * 8 * 2);
	GlobalDrawLine.ring = malloc((sizeof *GlobalDrawLine.ring) * RING_SECTIONS * 8 * 2);
	for(i = 0; i < RING_SECTIONS; i++)
	{
		sui_draw_set_vec2(GlobalDrawLine.ring, i * 8 + 0, 1.0 * DRAW_CLOSE_RANGE * sin(((double)i + 0.0) * PI / RING_SECTIONS * 2), 1.0 * DRAW_CLOSE_RANGE * cos(((double)i + 0.0) * PI / RING_SECTIONS * 2));
		sui_draw_set_vec2(GlobalDrawLine.ring, i * 8 + 1, 1.0 * DRAW_CLOSE_RANGE * sin(((double)i + 0.5) * PI / RING_SECTIONS * 2), 1.0 * DRAW_CLOSE_RANGE * cos(((double)i + 0.5) * PI / RING_SECTIONS * 2));
		sui_draw_set_vec2(GlobalDrawLine.ring, i * 8 + 2, 1.0 * DRAW_CLOSE_RANGE * sin(((double)i + 0.5) * PI / RING_SECTIONS * 2), 1.0 * DRAW_CLOSE_RANGE * cos(((double)i + 0.5) * PI / RING_SECTIONS * 2));
		sui_draw_set_vec2(GlobalDrawLine.ring, i * 8 + 3, 0.9 * DRAW_CLOSE_RANGE * sin(((double)i + 0.5) * PI / RING_SECTIONS * 2), 0.9 * DRAW_CLOSE_RANGE * cos(((double)i + 0.5) * PI / RING_SECTIONS * 2));
		sui_draw_set_vec2(GlobalDrawLine.ring, i * 8 + 4, 0.9 * DRAW_CLOSE_RANGE * sin(((double)i + 0.5) * PI / RING_SECTIONS * 2), 0.9 * DRAW_CLOSE_RANGE * cos(((double)i + 0.5) * PI / RING_SECTIONS * 2));
		sui_draw_set_vec2(GlobalDrawLine.ring, i * 8 + 5, 0.9 * DRAW_CLOSE_RANGE * sin(((double)i + 1.0) * PI / RING_SECTIONS * 2), 0.9 * DRAW_CLOSE_RANGE * cos(((double)i + 1.0) * PI / RING_SECTIONS * 2));
		sui_draw_set_vec2(GlobalDrawLine.ring, i * 8 + 6, 0.9 * DRAW_CLOSE_RANGE * sin(((double)i + 1.0) * PI / RING_SECTIONS * 2), 0.9 * DRAW_CLOSE_RANGE * cos(((double)i + 1.0) * PI / RING_SECTIONS * 2));
		sui_draw_set_vec2(GlobalDrawLine.ring, i * 8 + 7, 1.0 * DRAW_CLOSE_RANGE * sin(((double)i + 1.0) * PI / RING_SECTIONS * 2), 1.0 * DRAW_CLOSE_RANGE * cos(((double)i + 1.0) * PI / RING_SECTIONS * 2));
	}
}

void uv_new_draw_line(void)
{
	GlobalDrawLine.array_length = 0;
	GlobalDrawLine.vertex_length = 0;
	GlobalDrawLine.vertex_tie_length = 0;
	GlobalDrawLine.select_length = FALSE;
}

void uv_draw_line_add(float x, float y, boolean add)
{
	uint i;
	float dist, tie_length, *cam;
	static float last_x, last_y, line_length;
	if(GlobalDrawLine.array_length == 0)
	{
		GlobalDrawLine.array[0] = last_x = x, 
		GlobalDrawLine.array[1] = last_y = y;
		line_length = 0;
		GlobalDrawLine.array_length++;
		return;
	}
	if(GlobalDrawLine.array_length != DRAW_LINE_LENGTH - 2)
	{
		dist = sqrt((last_x - x) * (last_x - x) + (last_y - y) * (last_y - y));
		if(add && (dist > 0.002 || GlobalDrawLine.array_length != 0) && GlobalDrawLine.select_length == FALSE)
		{
			for(line_length += dist; line_length > DRAW_SEGMENT_LENGTH && GlobalDrawLine.vertex_length < DRAW_LINE_LENGTH * 8; line_length -= DRAW_SEGMENT_LENGTH)
			{
				sui_draw_set_vec2(GlobalDrawLine.vertex, GlobalDrawLine.vertex_length++, x - (x - last_x) / dist * line_length, y - (y - last_y) / dist * line_length);	
			}
			GlobalDrawLine.vertex_tie_length = GlobalDrawLine.vertex_length;
			dist = sqrt((GlobalDrawLine.array[0] - x) * (GlobalDrawLine.array[0] - x) + (GlobalDrawLine.array[1] - y) * (GlobalDrawLine.array[1] - y));
			for(tie_length = line_length + dist; tie_length > DRAW_SEGMENT_LENGTH && GlobalDrawLine.vertex_tie_length < DRAW_LINE_LENGTH * 8; tie_length -= DRAW_SEGMENT_LENGTH)
			{
				sui_draw_set_vec2(GlobalDrawLine.vertex, GlobalDrawLine.vertex_tie_length++, GlobalDrawLine.array[0] - (GlobalDrawLine.array[0] - x) / dist * tie_length, GlobalDrawLine.array[1] - (GlobalDrawLine.array[1] - y) / dist * tie_length);	
			}

		//	nglSetVertex2f(GlobalDrawLine.line, GlobalDrawLine.array_length, x, y);
			i = 2 * GlobalDrawLine.array_length++;
			GlobalDrawLine.array[i++] = x;
			GlobalDrawLine.array[i] = y;
			if(GlobalDrawLine.select_length == FALSE && dist < DRAW_CLOSE_RANGE)
				if(DRAW_CLOSE_RANGE * DRAW_CLOSE_RANGE < (last_x - GlobalDrawLine.array[0]) * (last_x - GlobalDrawLine.array[0]) + (last_y - GlobalDrawLine.array[1]) * (last_y - GlobalDrawLine.array[1]))
					GlobalDrawLine.select_length = TRUE;
			last_x = x;
			last_y = y;
		}
	}
	glPushMatrix();
	cam = get_cam_pos();
	glTranslatef(-cam[0], -cam[1], cam[2] - 1);

	glPushMatrix();
	glTranslatef(GlobalDrawLine.array[0], GlobalDrawLine.array[1], 0);
	sui_draw_gl(GL_LINES, GlobalDrawLine.ring, RING_SECTIONS * 8, 2, 0.1, 0.2, 0.4, 1.0f);
	glPopMatrix();
//	sui_draw_gl(GL_LINES, GlobalDrawLine.array, GlobalDrawLine.array_length, 2, 0.8, 0.8, 0.8);
	sui_draw_gl(GL_LINES, GlobalDrawLine.vertex, GlobalDrawLine.vertex_tie_length, 2, 0.8, 0.8, 0.8, 1.0f);
//	nglDrawArray(NGL_LINES, GlobalDrawLine.vertex, NULL, 0 ,sui_get_material(), 0);
	glPopMatrix();
}
/*
boolean uv_draw_select_menu_test(void)
{
	if(GlobalDrawLine.select_length)
	{
		uint i;
		i = 2 * GlobalDrawLine.array_length - 2;
		if(DRAW_CLOSE_RANGE > (GlobalDrawLine.array[i] - GlobalDrawLine.array[0]) * (GlobalDrawLine.array[i] - GlobalDrawLine.array[0]) + (GlobalDrawLine.array[i + 1] - GlobalDrawLine.array[1]) * (GlobalDrawLine.array[i + 1] - GlobalDrawLine.array[1]))
			return TRUE;
	}
	return FALSE;
}
*/


boolean uv_test_draw_line_point(egreal *pos)
{
	uint i, front = 0;
    for(i = 0; i < GlobalDrawLine.array_length * 2; i += 2)
        if((GlobalDrawLine.array[i + 1] > pos[1] && GlobalDrawLine.array[i + 3] < pos[1]) || (GlobalDrawLine.array[i + 1] < pos[1] && GlobalDrawLine.array[i + 3] > pos[1]))
            if((((pos[1] - GlobalDrawLine.array[i + 3]) / (GlobalDrawLine.array[i + 1] - GlobalDrawLine.array[i + 3])) * (GlobalDrawLine.array[i] - GlobalDrawLine.array[i + 2])) > pos[0] - GlobalDrawLine.array[i + 2])
                front++;
	return front % 2 == 1;				
}

void uv_draw_line_test_select(void)
{
	egreal pos[2], *s, select[4];
	uint id, i, poly;
	boolean change;
    GlobalDrawLine.array[GlobalDrawLine.array_length * 2] = GlobalDrawLine.array[0];
    GlobalDrawLine.array[GlobalDrawLine.array_length * 2 + 1] = GlobalDrawLine.array[1];
	for(id = uvg_get_next_polygon(0); id != -1; id = uvg_get_next_polygon(id + 1))
	{
		if(uvg_get_polygon_select(id))
		{
			poly = uvg_get_sides(id);
			s = uvg_get_corner_select(id);
			for(i = 0; i < poly; i++)
				select[i] = s[i];

			for(i = 0; i < poly; i++)
			{
				get_inner_corner(pos, id, i);
				pos[0] = uv_get_view_x(pos[0]);
				pos[1] = uv_get_view_y(pos[1]);
				if(uv_test_draw_line_point(pos))
				{
					uvg_set_one_corner_select(id, i, 1);
					uv_trans_unhide();
				}
			}
		}
	}
}
