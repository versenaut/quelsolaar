#include "la_includes.h"

#include "la_particle_fx.h"

#include "la_geometry_undo.h"
#include "la_projection.h"
#include "la_tool.h"
#include "la_draw_overlay.h"
#include <math.h>

#define DRAW_LINE_LENGTH 4096
#define RING_SECTIONS 17
#define DRAW_CLOSE_RANGE 0.05
#define DRAW_SEGMENT_LENGTH 0.01

struct{
	float		*array;
	uint		array_length;
	float		*vertex;
	uint		vertex_length;
	uint		vertex_tie_length;
	float		*ring;
	uint		*delete_poly_list;
	uint		delete_poly_length;
	uint		*delete_edge_list;
	uint		delete_edge_length;
	uint		*delete_vertex_list;
	uint		delete_vertex_length;
//	NGLArray	line;
	boolean		select_length;
}GlobalDrawLine;

void la_t_init_draw_line(void)
{
	uint i;
	GlobalDrawLine.array = malloc((sizeof *GlobalDrawLine.array) * ((DRAW_LINE_LENGTH * 2) + 2));
	GlobalDrawLine.delete_vertex_length = 0;
	GlobalDrawLine.vertex = malloc((sizeof *GlobalDrawLine.vertex) * DRAW_LINE_LENGTH * 8 * 2);
//	GlobalDrawLine.line = nglGetArray(NGL_VERTEX_ARRAY, DRAW_LINE_LENGTH, 4);
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

void la_t_new_draw_line(void)
{
	GlobalDrawLine.array_length = 0;
	GlobalDrawLine.vertex_length = 0;
	GlobalDrawLine.vertex_tie_length = 0;
	GlobalDrawLine.select_length = FALSE;
}

void la_t_draw_line_add(float x, float y, boolean add)
{
	uint i;
	float dist, tie_length;
	static float last_x, last_y, line_length;
	if(GlobalDrawLine.array_length == 0)
	{
		GlobalDrawLine.array[0] = last_x = x, 
		GlobalDrawLine.array[1] = last_y = y;
		line_length = 0;
		GlobalDrawLine.array_length++;
		return;
	}
	if(GlobalDrawLine.array_length < DRAW_LINE_LENGTH - 2)
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
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, 0, -1);
	glDisable(GL_DEPTH_TEST);
	glPushMatrix();
	glTranslatef(GlobalDrawLine.array[0], GlobalDrawLine.array[1], 0);
	sui_draw_gl(GL_LINES, GlobalDrawLine.ring, RING_SECTIONS * 8, 2, 0.1, 0.2, 0.4, 0.0);
	glPopMatrix();
//	sui_draw_gl(GL_LINES, GlobalDrawLine.array, GlobalDrawLine.array_length, 2, 0.8, 0.8, 0.8);
	sui_draw_gl(GL_LINES, GlobalDrawLine.vertex, GlobalDrawLine.vertex_tie_length, 2, 0.8, 0.8, 0.8, 0.0);
//	nglDrawArray(NGL_LINES, GlobalDrawLine.vertex, NULL, 0 ,sui_get_material(), 0);
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	glPushMatrix();
	glPushMatrix();
}

boolean draw_line_delete_test(double *vertex, double *a, double *b, boolean *del)
{
	double a2[2], b2[2], c[2], d[2];
	float start, end;
	if(a[2] > 0 || b[2] > 0)
		return FALSE;
	a2[0] = a[0] / -1;
	a2[1] = a[1] / -1;
	b2[0] = b[0] / -1;
	b2[1] = b[1] / -1;
	c[0] = GlobalDrawLine.array[0];
	c[1] = GlobalDrawLine.array[1];
	d[0] = GlobalDrawLine.array[GlobalDrawLine.array_length * 2 - 2];
	d[1] = GlobalDrawLine.array[GlobalDrawLine.array_length * 2 - 1];
	start = (a2[0] - b2[0]) * (c[1] - b2[1]) + (a2[1] - b2[1]) * (b2[0] - c[0]);
	end = (a2[0] - b2[0]) * (d[1] - b2[1]) + (a2[1] - b2[1]) * (b2[0] - d[0]);
	if((start > 0 && end < 0) || (start < 0 && end > 0))
	{
		start = (c[0] - d[0]) * (a2[1] - d[1]) + (c[1] - d[1]) * (d[0] - a2[0]);
		end = (c[0] - d[0]) * (b2[1] - d[1]) + (c[1] - d[1]) * (d[0] - b2[0]);
		if((start > 0 && end < 0) || (start < 0 && end > 0))
		{
			*del = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}

boolean la_t_draw_select_menu_test(void)
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

boolean la_t_draw_line_test_delete(void)
{
	double *vertex, a[3], b[3], c[3], d[3];
	boolean del = FALSE;
	uint vertex_length, *ref, ref_length, i;

	if(GlobalDrawLine.array_length == 0)
		return FALSE;

	udg_get_geometry(&vertex_length, &ref_length, &vertex, &ref, NULL);
	for(i = 0; i < ref_length; i++)
	{
		if(ref[i * 4] < vertex_length && ref[i * 4 + 1] < vertex_length && ref[i * 4 + 2] < vertex_length)
		{
			p_get_projection_screen(a, vertex[ref[i * 4] * 3], vertex[ref[i * 4] * 3 + 1], vertex[ref[i * 4] * 3 + 2]);
			p_get_projection_screen(b, vertex[ref[i * 4 + 1] * 3], vertex[ref[i * 4 + 1] * 3 + 1], vertex[ref[i * 4 + 1] * 3 + 2]);
			p_get_projection_screen(c, vertex[ref[i * 4 + 2] * 3], vertex[ref[i * 4 + 2] * 3 + 1], vertex[ref[i * 4 + 2] * 3 + 2]);
			if((a[0] - b[0]) * (c[1] - b[1]) + (a[1] - b[1]) * (b[0] - c[0]) > 0)
			{
				if(draw_line_delete_test(vertex, a, b, &del))
					udg_polygon_delete(i);
				else if(draw_line_delete_test(vertex, b, c, &del))
					udg_polygon_delete(i);
				else if(ref[i * 4 + 3] > vertex_length)
				{
					if(draw_line_delete_test(vertex, c, a, &del))
						udg_polygon_delete(i);
				}
				else
				{	
					p_get_projection_screen(d, vertex[ref[i * 4 + 3] * 3], vertex[ref[i * 4 + 3] * 3 + 1], vertex[ref[i * 4 + 3] * 3 + 2]);
					if(draw_line_delete_test(vertex, c, d, &del))
						udg_polygon_delete(i);
					else if(draw_line_delete_test(vertex, d, a, &del))
						udg_polygon_delete(i);
				}
			}
		}
	}
	ref = udg_get_edge_data(&i);
	while(i != 0)
	{
		i--;
		udg_get_vertex_pos(c, ref[i * 2]);
		p_get_projection_screen(a, c[0], c[1], c[2]);
		udg_get_vertex_pos(d, ref[i * 2 + 1]);
		p_get_projection_screen(b, d[0], d[1], d[2]);
		if(draw_line_delete_test(vertex, a, b, &del))
		{
			la_pfx_create_dust_line(d, c);
			udg_destroy_edge(i);
		}
	}
	return del;
}

boolean la_t_draw_line_draw_delete_overlay(void)
{
	double *vertex,	a[3], b[3], c[3], d[3];
	boolean del = FALSE;
	uint vertex_length, *ref, ref_length, i, local[4];
	glDisable(GL_DEPTH_TEST);
	udg_get_geometry(&vertex_length, &ref_length, &vertex, &ref, NULL);
	for(i = 0; i < ref_length; i++)
	{
		local[0] = ref[i * 4];
		local[1] = ref[i * 4 + 1];
		local[2] = ref[i * 4 + 2];
		local[3] = ref[i * 4 + 3];
		if(ref[i * 4] < vertex_length && ref[i * 4 + 1] < vertex_length && ref[i * 4 + 2] < vertex_length)
		{
			p_get_projection_screen(a, vertex[local[0] * 3], vertex[local[0] * 3 + 1], vertex[local[0] * 3 + 2]);
			p_get_projection_screen(b, vertex[local[1] * 3], vertex[local[1] * 3 + 1], vertex[local[1] * 3 + 2]);
			p_get_projection_screen(c, vertex[local[2] * 3], vertex[local[2] * 3 + 1], vertex[local[2] * 3 + 2]);
			if((a[0] - b[0]) * (c[1] - b[1]) + (a[1] - b[1]) * (b[0] - c[0]) > 0)
			{
				if(local[3] > vertex_length)
				{			
					if(draw_line_delete_test(vertex, a, b, &del))
						la_do_edge_delete(vertex[local[0] * 3], vertex[local[0] * 3 + 1], vertex[local[0] * 3 + 2], vertex[local[1] * 3], vertex[local[1] * 3 + 1], vertex[local[1] * 3 + 2], vertex[local[2] * 3], vertex[local[2] * 3 + 1], vertex[local[2] * 3 + 2]);
					if(draw_line_delete_test(vertex, b, c, &del))
						la_do_edge_delete(vertex[local[1] * 3], vertex[local[1] * 3 + 1], vertex[local[1] * 3 + 2], vertex[local[2] * 3], vertex[local[2] * 3 + 1], vertex[local[2] * 3 + 2], vertex[local[0] * 3], vertex[local[0] * 3 + 1], vertex[local[0] * 3 + 2]);
					if(draw_line_delete_test(vertex, c, a, &del))
						la_do_edge_delete(vertex[local[2] * 3], vertex[local[2] * 3 + 1], vertex[local[2] * 3 + 2], vertex[local[0] * 3], vertex[local[0] * 3 + 1], vertex[local[0] * 3 + 2], vertex[local[1] * 3], vertex[local[1] * 3 + 1], vertex[local[1] * 3 + 2]);
				}
				else
				{	
					p_get_projection_screen(d, vertex[local[3] * 3], vertex[local[3] * 3 + 1], vertex[local[3] * 3 + 2]);
					if(draw_line_delete_test(vertex, a, b, &del))
						la_do_edge_delete(vertex[local[0] * 3], vertex[local[0] * 3 + 1], vertex[local[0] * 3 + 2], vertex[local[1] * 3], vertex[local[1] * 3 + 1], vertex[local[1] * 3 + 2], (vertex[local[2] * 3] + vertex[local[3] * 3]) * 0.5, (vertex[local[2] * 3 + 1] + vertex[local[3] * 3 + 1]) * 0.5, (vertex[local[2] * 3 + 2] + vertex[local[3] * 3 + 2]) * 0.5);
					if(draw_line_delete_test(vertex, b, c, &del))
						la_do_edge_delete(vertex[local[1] * 3], vertex[local[1] * 3 + 1], vertex[local[1] * 3 + 2], vertex[local[2] * 3], vertex[local[2] * 3 + 1], vertex[local[2] * 3 + 2], (vertex[local[3] * 3] + vertex[local[0] * 3]) * 0.5, (vertex[local[3] * 3 + 1] + vertex[local[0] * 3 + 1]) * 0.5, (vertex[local[3] * 3 + 2] + vertex[local[0] * 3 + 2]) * 0.5);
					if(draw_line_delete_test(vertex, c, d, &del))
						la_do_edge_delete(vertex[local[2] * 3], vertex[local[2] * 3 + 1], vertex[local[2] * 3 + 2], vertex[local[3] * 3], vertex[local[3] * 3 + 1], vertex[local[3] * 3 + 2], (vertex[local[0] * 3] + vertex[local[1] * 3]) * 0.5, (vertex[local[0] * 3 + 1] + vertex[local[1] * 3 + 1]) * 0.5, (vertex[local[0] * 3 + 2] + vertex[local[1] * 3 + 2]) * 0.5);
					if(draw_line_delete_test(vertex, d, a, &del))
						la_do_edge_delete(vertex[local[3] * 3], vertex[local[3] * 3 + 1], vertex[local[3] * 3 + 2], vertex[local[0] * 3], vertex[local[0] * 3 + 1], vertex[local[0] * 3 + 2], (vertex[local[1] * 3] + vertex[local[2] * 3]) * 0.5, (vertex[local[1] * 3 + 1] + vertex[local[2] * 3 + 1]) * 0.5, (vertex[local[1] * 3 + 2] + vertex[local[2] * 3 + 2]) * 0.5);
				}
			}
		}
	}
	ref = udg_get_edge_data(&i);
	while(i != 0)
	{
		glPopMatrix();
		glPopMatrix();
		glPushMatrix();
		glTranslated(0, 0, -1);
		i--;
		p_get_projection_screen(a, vertex[ref[i * 2] * 3], vertex[ref[i * 2] * 3 + 1], vertex[ref[i * 2] * 3 + 2]);
		p_get_projection_screen(b, vertex[ref[i * 2 + 1] * 3], vertex[ref[i * 2 + 1] * 3 + 1], vertex[ref[i * 2 + 1] * 3 + 2]);
		if(draw_line_delete_test(vertex, a, b, &del))
			la_do_edge_delete_air(a, b);
		glPopMatrix();
		glPushMatrix();
		p_view_set();
		glPushMatrix();
	}
	glEnable(GL_DEPTH_TEST);
	return del;
}


/*

*/
boolean la_t_draw_line_test_select(SelectionMode mode)
{
	double *vertex, pos[3];
	boolean output = FALSE;
	uint length, i, k, front;
	UNDOTag	*tag;
    udg_get_geometry(&length, NULL, &vertex, NULL, NULL);
    GlobalDrawLine.array[GlobalDrawLine.array_length * 2] = GlobalDrawLine.array[0];
    GlobalDrawLine.array[GlobalDrawLine.array_length * 2 + 1] = GlobalDrawLine.array[1];
    for(i = 0; i < length; i++)
    {
        if(vertex[i * 3] != V_REAL64_MAX)
        {
            front = 0;
            p_get_projection_screen(pos, vertex[i * 3], vertex[i * 3 + 1], vertex[i * 3 + 2]);
            pos[0] = pos[0] / -1;
            pos[1] = pos[1] / -1;
            // if(pos[2] > 0)
            for(k = 0; k < GlobalDrawLine.array_length * 2; k += 2)
                if((GlobalDrawLine.array[k + 1] > pos[1] && GlobalDrawLine.array[k + 3] < pos[1]) || (GlobalDrawLine.array[k + 1] < pos[1] && GlobalDrawLine.array[k + 3] > pos[1]))
                    if((((pos[1] - GlobalDrawLine.array[k + 3]) / (GlobalDrawLine.array[k + 1] - GlobalDrawLine.array[k + 3])) * (GlobalDrawLine.array[k] - GlobalDrawLine.array[k + 2])) > pos[0] - GlobalDrawLine.array[k + 2])
                        front++;
            if(front % 2 == 1 /*&&front % 2 == 1*/)
            {
                output = TRUE;
                switch(mode)
                {
                case SM_SELECT :
                    udg_set_select(i, 1);
                break;
                case SM_DESELECT :
                    udg_set_select(i, 0);
                break;					
                case SM_SUB :
                    if(udg_get_select(i) - 0.25 > 0)
                        udg_set_select(i, udg_get_select(i) - 0.25);
                    else
                        udg_set_select(i, 0);
                break;
                case SM_ADD :
                    if(udg_get_select(i) + 0.25 < 1)
                        udg_set_select(i, udg_get_select(i) + 0.25);
					else
                        udg_set_select(i, 1);
				break;	
				}
			}
		}
	}
	tag = udg_get_tags(&length);
    for(i = 0; i < length; i++)
	{
        front = 0;
        p_get_projection_screen(pos, tag[i].vec[0], tag[i].vec[1], tag[i].vec[2]);
        pos[0] = pos[0] / -1;
        pos[1] = pos[1] / -1;
        // if(pos[2] > 0)
        for(k = 0; k < GlobalDrawLine.array_length * 2; k += 2)
            if((GlobalDrawLine.array[k + 1] > pos[1] && GlobalDrawLine.array[k + 3] < pos[1]) || (GlobalDrawLine.array[k + 1] < pos[1] && GlobalDrawLine.array[k + 3] > pos[1]))
                if((((pos[1] - GlobalDrawLine.array[k + 3]) / (GlobalDrawLine.array[k + 1] - GlobalDrawLine.array[k + 3])) * (GlobalDrawLine.array[k] - GlobalDrawLine.array[k + 2])) > pos[0] - GlobalDrawLine.array[k + 2])
                    front++;
        if(front % 2 == 1 /*&&front % 2 == 1*/)
        {
            output = TRUE;
            switch(mode)
            {
            case SM_SELECT :
                udg_select_tag(i, 1);
            break;
            case SM_DESELECT :
                udg_select_tag(i, 0);
            break;					
            case SM_SUB :
                if(tag[i].select - 0.25 > 0)
                    udg_select_tag(i, tag[i].select - 0.25);
                else
                    udg_set_select(i, 0);
            break;
            case SM_ADD :
                if(tag[i].select + 0.25 < 1)
                    udg_select_tag(i, tag[i].select + 0.25);
				else
                    udg_select_tag(i, 1);
			break;	
			}
		}
	}
	return output;
}
