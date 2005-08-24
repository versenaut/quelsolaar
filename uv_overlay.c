#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"
#include "seduce.h"
#include "persuade.h"

#include "uv.h"
#include "uv_geometry.h"


#define CORNER_SIZE 0.2
#define INV_CORNER_SIZE (1.0 - CORNER_SIZE) 

#define POINT_CORNER_SIZE 0.15
#define INV_POINT_CORNER_SIZE (1.0 - CORNER_SIZE) 

#define EDGE_SIZE 0.1
#define INV_EDGE_SIZE (1.0 - EDGE_SIZE) 


#define TRI_CORNER_SIZE 0.14
#define INV_TRI_CORNER_SIZE (1.0 - 0.28) 

#define SC_RED 0.6
#define SC_GREEN 0.0
#define SC_BLUE -0.4

#define USC_RED 0.4
#define USC_GREEN 0.3
#define USC_BLUE 0.4

#define PUSC_GRAY 0.2

uint texture_node_id = -1;

void uvo_set_bitmap_node(uint node_id)
{
//	texture_node_id = node_id;
}

void uvo_draw_square()
{
	float vertex[] = {0, 0, 1, 0, 1, 1, 0, 1};
	ENode *node;
	if((node = e_ns_get_node(0, texture_node_id)) != NULL);
	{
		PTextureHandle *h = NULL;
		h = p_th_create_texture_handle(texture_node_id, "col_r", "col_g", "col_b");
		sui_set_texture2D_array_gl(vertex, 4, 2, p_th_get_texture_id(h));
		sui_draw_gl(GL_QUADS, vertex, 4, 2, 1, 1, 1);
		p_th_destroy_texture_handle(h);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	sui_draw_gl(GL_QUADS, vertex, 4, 2, 0.1, 0.2, 0.4);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void get_tri_edge(egreal *output, uint edge, egreal *uv)
{
	edge *= 2; 
	output[0] = uv[(0 + edge) % 6];
	output[1] = uv[(1 + edge) % 6];
	output[2] = uv[(0 + edge) % 6] * INV_EDGE_SIZE + uv[(2 + edge) % 6] * EDGE_SIZE;
	output[3] = uv[(1 + edge) % 6] * INV_EDGE_SIZE + uv[(3 + edge) % 6] * EDGE_SIZE;
	output[4] = uv[(4 + edge) % 6] * INV_EDGE_SIZE + uv[(2 + edge) % 6] * EDGE_SIZE;
	output[5] = uv[(5 + edge) % 6] * INV_EDGE_SIZE + uv[(3 + edge) % 6] * EDGE_SIZE;
	output[6] = uv[(4 + edge) % 6];
	output[7] = uv[(5 + edge) % 6];
}
/*
void get_tri_cornere(egreal *output, uint corner, egreal *uv)
{
	corner *= 2; 
	output[0] = uv[corner % 6];
	output[1] = uv[(1 + corner) % 6];
	output[2] = uv[corner % 6] * INV_CORNER_SIZE + uv[(2 + corner) % 6] * CORNER_SIZE;
	output[3] = uv[(1 + corner) % 6] * INV_CORNER_SIZE + uv[(3 + corner) % 6] * CORNER_SIZE;
	output[4] = uv[corner % 6] * INV_TRI_CORNER_SIZE + uv[(2 + corner) % 6] * TRI_CORNER_SIZE + uv[(4 + corner) % 6] * TRI_CORNER_SIZE;
	output[5] = uv[(1 + corner) % 6] * INV_TRI_CORNER_SIZE + uv[(3 + corner) % 6] * TRI_CORNER_SIZE + uv[(5 + corner) % 6] * TRI_CORNER_SIZE;
	output[6] = uv[corner % 6] * INV_CORNER_SIZE + uv[(4 + corner) % 6] * CORNER_SIZE;
	output[7] = uv[(1 + corner) % 6] * INV_CORNER_SIZE + uv[(5 + corner) % 6] * CORNER_SIZE;
}
*/
void get_quad_edge(egreal *output, uint edge, egreal *uv)
{
	edge *= 2; 
	output[0] = uv[(0 + edge) % 8];
	output[1] = uv[(1 + edge) % 8];
	output[2] = uv[(0 + edge) % 8] * INV_EDGE_SIZE + uv[(2 + edge) % 8] * EDGE_SIZE;
	output[3] = uv[(1 + edge) % 8] * INV_EDGE_SIZE + uv[(3 + edge) % 8] * EDGE_SIZE;
	output[4] = uv[(6 + edge) % 8] * INV_EDGE_SIZE + uv[(4 + edge) % 8] * EDGE_SIZE;
	output[5] = uv[(7 + edge) % 8] * INV_EDGE_SIZE + uv[(5 + edge) % 8] * EDGE_SIZE;
	output[6] = uv[(6 + edge) % 8];
	output[7] = uv[(7 + edge) % 8];
}

void get_tri_corner(egreal *output, uint corner, egreal *uv, egreal select)
{
	egreal inv_select;
	inv_select = 1 - select;
	select = inv_select * 0.8 + 0.2;
	corner *= 2; 
	output[0] = uv[(0 + corner) % 6];
	output[1] = uv[(1 + corner) % 6];

	output[2] = (uv[(2 + corner) % 6] - output[0]) * CORNER_SIZE;
	output[3] = (uv[(3 + corner) % 6] - output[1]) * CORNER_SIZE;

	output[6] = (uv[(4 + corner) % 6] - output[0]) * CORNER_SIZE;
	output[7] = (uv[(5 + corner) % 6] - output[1]) * CORNER_SIZE;

	output[4] = output[0] + (output[2] + output[6]) * 0.6;
	output[5] = output[1] + (output[3] + output[7]) * 0.6;

	output[2] = (output[2] + output[0]) * select + output[4] * (1 - select);
	output[3] = (output[3] + output[1]) * select + output[5] * (1 - select);

	output[6] = (output[6] + output[0]) * select + output[4] * (1 - select);
	output[7] = (output[7] + output[1]) * select + output[5] * (1 - select);

	output[0] = output[0] * inv_select + (output[2] + output[6] - output[4]) * (1 - inv_select);
	output[1] = output[1] * inv_select + (output[3] + output[7] - output[5]) * (1 - inv_select);
}

void get_quad_corner(egreal *output, uint corner, egreal *uv, egreal select)
{
	egreal inv_select;
	select /= 6;
	inv_select = 1 - select; 
	corner *= 2; 
	output[0] = (uv[(0 + corner) % 8] * inv_select + uv[(2 + corner) % 8] * select) * inv_select + (uv[(6 + corner) % 8] * inv_select + uv[(4 + corner) % 8] * select) * select;
	output[1] = (uv[(1 + corner) % 8] * inv_select + uv[(3 + corner) % 8] * select) * inv_select + (uv[(7 + corner) % 8] * inv_select + uv[(5 + corner) % 8] * select) * select;
	output[2] = (uv[(0 + corner) % 8] * inv_select + uv[(2 + corner) % 8] * select) * INV_CORNER_SIZE + (uv[(6 + corner) % 8] * inv_select + uv[(4 + corner) % 8] * select) * CORNER_SIZE;
	output[3] = (uv[(1 + corner) % 8] * inv_select + uv[(3 + corner) % 8] * select) * INV_CORNER_SIZE + (uv[(7 + corner) % 8] * inv_select + uv[(5 + corner) % 8] * select) * CORNER_SIZE;
	output[4] = (uv[(0 + corner) % 8] * INV_CORNER_SIZE + uv[(2 + corner) % 8] * CORNER_SIZE) * INV_CORNER_SIZE + (uv[(6 + corner) % 8] * INV_CORNER_SIZE + uv[(4 + corner) % 8] * CORNER_SIZE) * CORNER_SIZE;
	output[5] = (uv[(1 + corner) % 8] * INV_CORNER_SIZE + uv[(3 + corner) % 8] * CORNER_SIZE) * INV_CORNER_SIZE + (uv[(7 + corner) % 8] * INV_CORNER_SIZE + uv[(5 + corner) % 8] * CORNER_SIZE) * CORNER_SIZE;
	output[6] = (uv[(0 + corner) % 8] * INV_CORNER_SIZE + uv[(2 + corner) % 8] * CORNER_SIZE) * inv_select		+ (uv[(6 + corner) % 8] * INV_CORNER_SIZE + uv[(4 + corner) % 8] * CORNER_SIZE) * select;
	output[7] = (uv[(1 + corner) % 8] * INV_CORNER_SIZE + uv[(3 + corner) % 8] * CORNER_SIZE) * inv_select		+ (uv[(7 + corner) % 8] * INV_CORNER_SIZE + uv[(5 + corner) % 8] * CORNER_SIZE) * select;
}



void get_inner_corner(egreal *output, uint id, uint corner)
{
	egreal uv[8];
	corner *= 2; 
	uvg_get_uve(uv, id);
	if(uvg_is_quad(id))
	{
		output[0] = (uv[(0 + corner) % 8] * INV_CORNER_SIZE + uv[(2 + corner) % 8] * CORNER_SIZE) * INV_CORNER_SIZE + (uv[(6 + corner) % 8] * INV_CORNER_SIZE + uv[(4 + corner) % 8] * CORNER_SIZE) * CORNER_SIZE;
		output[1] = (uv[(1 + corner) % 8] * INV_CORNER_SIZE + uv[(3 + corner) % 8] * CORNER_SIZE) * INV_CORNER_SIZE + (uv[(7 + corner) % 8] * INV_CORNER_SIZE + uv[(5 + corner) % 8] * CORNER_SIZE) * CORNER_SIZE;
	}
	else
	{	
		output[0] = uv[(0 + corner) % 6];
		output[1] = uv[(1 + corner) % 6];
		output[0] = output[0] + ((uv[(2 + corner) % 6] - output[0]) * CORNER_SIZE + (uv[(4 + corner) % 6] - output[0]) * CORNER_SIZE) * 0.6;
		output[1] = output[1] + ((uv[(3 + corner) % 6] - output[1]) * CORNER_SIZE + (uv[(5 + corner) % 6] - output[1]) * CORNER_SIZE) * 0.6;
	}
}


boolean uv_test_tri(egreal *uv, float x, float y);
boolean uv_test_quad(egreal *uv, float x, float y);
float uv_get_pos_x(float x);
float uv_get_pos_y(float y);

float smooth_move = 0.1;
float inv_smooth_move = 0.9;

void uvo_set_overlay_motion(float move)
{
	smooth_move = move;
	inv_smooth_move = 1 - move;
}

void uvo_draw_overlay(BInputState *input)
{
	static float *array = NULL, *color = NULL;
	static uint array_length = 0;
	egreal uv[8], *select, vertex[8];
	float c[3];
	uint size, id, pos, i, j;
	glEnable(GL_LINE_SMOOTH);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	size = 0;
	for(id = uvg_get_next_polygon(0); id != -1; id = uvg_get_next_polygon(id + 1))
	{
		size += 24;
		if(uvg_is_quad(id))
			size += 8;
	}
	
	if(size != array_length)
	{
		array = realloc(array, (sizeof *array) * size * 2);
		color = realloc(color, (sizeof *color) * size * 3);
		for(i = array_length * 2; i < size * 2; i++)
			array[i] = 0;
		array_length = size;
	}
	pos = 0;
	for(id = uvg_get_next_polygon(0); id != -1; id = uvg_get_next_polygon(id + 1))
	{
		uvg_get_uve(uv, id);
		if(uvg_is_quad(id))
		{
			select = uvg_get_corner_select(id);
			if(uvg_get_polygon_select(id))
				c[0] = 0.6;
			else
				c[0] = 0.15;
			for(j = 0; j < 8; j++)
				array[pos * 2 + j] = array[pos * 2 + j] * inv_smooth_move + uv[j] * smooth_move;
			for(j = 0; j < 4 * 3; j++)
				color[pos * 3 + j] = c[0];
			pos += 4;
			for(i = 0; i < 4; i++)
			{
				if(uvg_get_polygon_select(id))
				{
					c[0] = (USC_RED + SC_RED * select[i]);
					c[1] = (USC_GREEN + SC_GREEN * select[i]);
					c[2] = (USC_BLUE + SC_BLUE * select[i]);
				}else
				{
					c[0] = PUSC_GRAY;
					c[1] = PUSC_GRAY;
					c[2] = PUSC_GRAY;
				}
				get_quad_corner(vertex, i, uv, 1 - select[i]);
				for(j = 0; j < 8; j++)
					array[pos * 2 + j] = array[pos * 2 + j] * inv_smooth_move + vertex[j] * smooth_move;
				for(j = 0; j < 4; j++)
					sui_draw_set_vec3(color, pos++, c[0], c[1], c[2]);
			}
		}
		else
		{
			select = uvg_get_corner_select(id);
			if(uvg_get_polygon_select(id))
				c[0] = 0.6;
			else
				c[0] = 0.15;
			for(j = 0; j < 6; j++)
				array[pos * 2 + j] = array[pos * 2 + j] * inv_smooth_move + uv[j] * smooth_move;
			for(j = 0; j < 2; j++)
				array[pos * 2 + 6 + j] = array[pos * 2 + 6 + j] * inv_smooth_move + uv[j] * smooth_move;
			for(j = 0; j < 4 * 3; j++)
				color[pos * 3 + j] = c[0];
			pos += 4;
			for(i = 0; i < 3; i++)
			{
				if(uvg_get_polygon_select(id))
				{
					c[0] = (USC_RED + SC_RED * select[i]);
					c[1] = (USC_GREEN + SC_GREEN * select[i]);
					c[2] = (USC_BLUE + SC_BLUE * select[i]);
				}else
				{
					c[0] = PUSC_GRAY;
					c[1] = PUSC_GRAY;
					c[2] = PUSC_GRAY;
				}
				get_tri_corner(vertex, i, uv, 1 - select[i]);
				for(j = 0; j < 8; j++)
					array[pos * 2 + j] = array[pos * 2 + j] * inv_smooth_move + vertex[j] * smooth_move;				
				for(j = 0; j < 4; j++)
					sui_draw_set_vec3(color, pos++, c[0], c[1], c[2]);
			}

		}
	}
//	sui_set_blend_gl(GL_ONE, GL_ONE);

	sui_set_color_array_gl(color, pos, 3);
	sui_draw_gl(GL_QUADS, array, pos, 2, 0, 0, 0);

//	free(array);
//	free(color);
/*	for(id = uvg_get_next_polygon(0); id != -1; id = uvg_get_next_polygon(id + 1))
	{
		uvg_get_uve(uv, id);

		for(i = 0; i < 8; i++)
		{
			vertex[i] = (egreal)uv[i];
			printf("%f ", uv[i]);
		}
		printf("id %u\n", id);
		if(uvg_is_quad(id))
		{

	//		if(uv_test_quad(uv, uv_get_pos_x(input->pointer_x), uv_get_pos_y(input->pointer_y)))
				sui_draw_gl(GL_QUADS, vertex, 4, 2, 1, 1, 1);
		}
		else
		{
	//		if(uv_test_tri(uv, uv_get_pos_x(input->pointer_x), uv_get_pos_y(input->pointer_y)))
				sui_draw_gl(GL_TRIANGLES, vertex, 3, 2, 1, 1, 1);
		}
	}*/
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void draw_a_polygon(uint id, float x, float y)
{
	egreal uv[8];
	uint part;
	if(id == -1 || id != uvg_get_next_polygon(id))
		return;
	uvg_get_uve(uv, id);

	if(uvg_is_quad(id))
	{
		part = uv_test_edge(id, x, y);
		if(part < 4)
		{
			if(part == 0)
			 	sui_draw_2d_line_gl(uv[0] * 0.2 + uv[2] * 0.05 + uv[4] * 0.05 + uv[6] * 0.7,
								uv[1] * 0.2 + uv[3] * 0.05 + uv[5] * 0.05 + uv[7] * 0.7,
								uv[0] * 0.7 + uv[2] * 0.05 + uv[4] * 0.05 + uv[6] * 0.2,
								uv[1] * 0.7 + uv[3] * 0.05 + uv[5] * 0.05 + uv[7] * 0.2,
									0.45, 0.3, 0);
			else if(part == 1)
				sui_draw_2d_line_gl(uv[0] * 0.7 + uv[2] * 0.2 + uv[4] * 0.05 + uv[6] * 0.05,
								uv[1] * 0.7 + uv[3] * 0.2 + uv[5] * 0.05 + uv[7] * 0.05,
								uv[0] * 0.2 + uv[2] * 0.7 + uv[4] * 0.05 + uv[6] * 0.05,
								uv[1] * 0.2 + uv[3] * 0.7 + uv[5] * 0.05 + uv[7] * 0.05,
									0.45, 0.3, 0);
			else if(part == 2)
				sui_draw_2d_line_gl(uv[0] * 0.05 + uv[2] * 0.7 + uv[4] * 0.2 + uv[6] * 0.05,
								uv[1] * 0.05 + uv[3] * 0.7 + uv[5] * 0.2 + uv[7] * 0.05,
								uv[0] * 0.05 + uv[2] * 0.2 + uv[4] * 0.7 + uv[6] * 0.05,
								uv[1] * 0.05 + uv[3] * 0.2 + uv[5] * 0.7 + uv[7] * 0.05,
									0.45, 0.3, 0);
			else if(part == 3)
				sui_draw_2d_line_gl(uv[0] * 0.05 + uv[2] * 0.05 + uv[4] * 0.7 + uv[6] * 0.2,
								uv[1] * 0.05 + uv[3] * 0.05 + uv[5] * 0.7 + uv[7] * 0.2,
								uv[0] * 0.05 + uv[2] * 0.05 + uv[4] * 0.2 + uv[6] * 0.7,
								uv[1] * 0.05 + uv[3] * 0.05 + uv[5] * 0.2 + uv[7] * 0.7,
									0.45, 0.3, 0);

		}else
		{
			sui_draw_2d_line_gl(uv[0] * 0.7 + uv[2] * 0.2 + uv[4] * 0.05 + uv[6] * 0.05,
								uv[1] * 0.7 + uv[3] * 0.2 + uv[5] * 0.05 + uv[7] * 0.05,
								uv[0] * 0.2 + uv[2] * 0.7 + uv[4] * 0.05 + uv[6] * 0.05,
								uv[1] * 0.2 + uv[3] * 0.7 + uv[5] * 0.05 + uv[7] * 0.05,
									0.3, 0.2, 0);
			sui_draw_2d_line_gl(uv[0] * 0.05 + uv[2] * 0.7 + uv[4] * 0.2 + uv[6] * 0.05,
								uv[1] * 0.05 + uv[3] * 0.7 + uv[5] * 0.2 + uv[7] * 0.05,
								uv[0] * 0.05 + uv[2] * 0.2 + uv[4] * 0.7 + uv[6] * 0.05,
								uv[1] * 0.05 + uv[3] * 0.2 + uv[5] * 0.7 + uv[7] * 0.05,
									0.3, 0.2, 0);
			sui_draw_2d_line_gl(uv[0] * 0.05 + uv[2] * 0.05 + uv[4] * 0.7 + uv[6] * 0.2,
								uv[1] * 0.05 + uv[3] * 0.05 + uv[5] * 0.7 + uv[7] * 0.2,
								uv[0] * 0.05 + uv[2] * 0.05 + uv[4] * 0.2 + uv[6] * 0.7,
								uv[1] * 0.05 + uv[3] * 0.05 + uv[5] * 0.2 + uv[7] * 0.7,
									0.3, 0.2, 0);
			sui_draw_2d_line_gl(uv[0] * 0.2 + uv[2] * 0.05 + uv[4] * 0.05 + uv[6] * 0.7,
								uv[1] * 0.2 + uv[3] * 0.05 + uv[5] * 0.05 + uv[7] * 0.7,
								uv[0] * 0.7 + uv[2] * 0.05 + uv[4] * 0.05 + uv[6] * 0.2,
								uv[1] * 0.7 + uv[3] * 0.05 + uv[5] * 0.05 + uv[7] * 0.2,
									0.3, 0.2, 0);
		}
	}else
	{
		part = uv_test_edge(id, x, y);
		if(part < 3)
		{
			if(part == 1)
				sui_draw_2d_line_gl(uv[0] * 0.7 + uv[2] * 0.2 + uv[4] * 0.1,
								uv[1] * 0.7 + uv[3] * 0.2 + uv[5] * 0.1,
								uv[0] * 0.2 + uv[2] * 0.7 + uv[4] * 0.1,
								uv[1] * 0.2 + uv[3] * 0.7 + uv[5] * 0.1,
									0.45, 0.3, 0);
			else if(part == 2)
				sui_draw_2d_line_gl(uv[0] * 0.1 + uv[2] * 0.2 + uv[4] * 0.7,
								uv[1] * 0.1 + uv[3] * 0.2 + uv[5] * 0.7,
								uv[0] * 0.1 + uv[2] * 0.7 + uv[4] * 0.2,
								uv[1] * 0.1 + uv[3] * 0.7 + uv[5] * 0.2,
									0.45, 0.3, 0);
			else if(part == 0)
				sui_draw_2d_line_gl(uv[0] * 0.7 + uv[2] * 0.1 + uv[4] * 0.2,
								uv[1] * 0.7 + uv[3] * 0.1 + uv[5] * 0.2,
								uv[0] * 0.2 + uv[2] * 0.1 + uv[4] * 0.7,
								uv[1] * 0.2 + uv[3] * 0.1 + uv[5] * 0.7,
									0.45, 0.3, 0);
		}else
		{
			sui_draw_2d_line_gl(uv[0] * 0.7 + uv[2] * 0.2 + uv[4] * 0.1,
								uv[1] * 0.7 + uv[3] * 0.2 + uv[5] * 0.1,
								uv[0] * 0.2 + uv[2] * 0.7 + uv[4] * 0.1,
								uv[1] * 0.2 + uv[3] * 0.7 + uv[5] * 0.1,
									0.3, 0.2, 0);
			sui_draw_2d_line_gl(uv[0] * 0.1 + uv[2] * 0.2 + uv[4] * 0.7,
								uv[1] * 0.1 + uv[3] * 0.2 + uv[5] * 0.7,
								uv[0] * 0.1 + uv[2] * 0.7 + uv[4] * 0.2,
								uv[1] * 0.1 + uv[3] * 0.7 + uv[5] * 0.2,
									0.3, 0.2, 0);
			sui_draw_2d_line_gl(uv[0] * 0.7 + uv[2] * 0.1 + uv[4] * 0.2,
								uv[1] * 0.7 + uv[3] * 0.1 + uv[5] * 0.2,
								uv[0] * 0.2 + uv[2] * 0.1 + uv[4] * 0.7,
								uv[1] * 0.2 + uv[3] * 0.1 + uv[5] * 0.7,
									0.3, 0.2, 0);
		}
	}
}
