#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"
#include "seduce.h"

#include "uv_geometry.h"
#include "uv.h"
/*
	for(node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
		count++;
	element = malloc((sizeof *element) * count);
	element[0].type = PU_T_ANGLE;
	element[0].text = "Settings";
	element[0].data.angle[0] = -45;
	element[0].data.angle[1] = 45;
	element[1].type = PU_T_ANGLE;
	element[1].text = "Undo";
	element[1].data.angle[0] = 45;
	element[1].data.angle[1] = 135;
	element[2].type = PU_T_ANGLE;
	element[2].text = "Redo";
	element[2].data.angle[0] = 225;
	element[2].data.angle[1] = 315;
	element[3].type = PU_T_BOTTOM;
	element[3].text = "NEW GEOMETRY";
	count = 4;
	for(node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
	{
		element[count].type = PU_T_BOTTOM;
		element[count].text = e_ns_get_node_name(node);
		count++;
	}
*/

#define MAX_NODE_DISPLAY_COUNT 5

void uvo_set_bitmap_node(uint node_id);

void sui_button_uv_create(float pos_x, float pos_y, float red, float green, float blue);

void uv_pu_empty(BInputState *input)
{
	SUIPUElement element[6 + MAX_NODE_DISPLAY_COUNT * 2];
	static float x, y, static_geo_start = 1, static_bitmap_start = 1;
	uint start = 0, ring, count = 3, i, geo_count;
	float *cam;
	ENode *node;

	element[0].type = PU_T_ANGLE;
	element[0].text = "Undo";
	element[0].data.angle[0] = 55;
	element[0].data.angle[1] = 125;
	element[1].type = PU_T_ANGLE;
	element[1].text = "Redo";
	element[1].data.angle[0] = 235;
	element[1].data.angle[1] = 305;
	element[2].type = PU_T_TOP;
	element[2].text = "Creat Texture";
	count = 3;
	
	start = static_geo_start;
	node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY);
	for(i = 1; node != NULL && i < start; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
		i++;
	if(start != 0)
	{
		if(y - 0.125 < input->pointer_y)
			static_geo_start -= betray_get_delta_time() * 10 * (0.2 + input->pointer_y - (y - 0.125));
		element[count].type = PU_T_BOTTOM;
		element[count].text = "More...";
		count++;
	}
	for(; node != NULL && count < MAX_NODE_DISPLAY_COUNT + 3; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
	{
		element[count].type = PU_T_BOTTOM;
		element[count].text = e_ns_get_node_name(node);
		count++;
	}
	if(count == MAX_NODE_DISPLAY_COUNT + 3 && node != NULL)
	{
		if(y - 0.05 * MAX_NODE_DISPLAY_COUNT - 0.075 > input->pointer_y)
			static_geo_start += betray_get_delta_time() * 10 * (0.2 + (y - 0.05 * MAX_NODE_DISPLAY_COUNT - 0.075) - input->pointer_y);
		element[count].type = PU_T_BOTTOM;
		element[count].text = "More...";
		count++;
	}
	geo_count = count;

	start = static_bitmap_start;
	node = e_ns_get_node_next(0, 0, V_NT_BITMAP);
	for(i = 1; node != NULL && i < start; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_BITMAP))
		i++;
	if(start != 0)
	{
		if(y + 0.125 > input->pointer_y)
			static_bitmap_start -= betray_get_delta_time() * 10 * (0.2 + (y + 0.125) - input->pointer_y);
		element[count].type = PU_T_TOP;
		element[count].text = "More...";
		count++;
	}
	for(; node != NULL && count < geo_count + MAX_NODE_DISPLAY_COUNT; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_BITMAP))
	{
		element[count].type = PU_T_TOP;
		element[count].text = e_ns_get_node_name(node);
		count++;
	}
	if(count == geo_count + MAX_NODE_DISPLAY_COUNT && node != NULL)
	{
		if(y + 0.05 * MAX_NODE_DISPLAY_COUNT + 0.075 < input->pointer_y)
			static_bitmap_start += betray_get_delta_time() * 10 * (0.2 + input->pointer_y - (y + 0.05 * MAX_NODE_DISPLAY_COUNT + 0.075));
		element[count].type = PU_T_TOP;
		element[count].text = "More...";
		count++;
	}

	if(input->mode == BAM_DRAW)
	{
		glDisable(GL_DEPTH_TEST);
		glPushMatrix();
		cam = get_cam_pos();
		glTranslatef(-cam[0], -cam[1], cam[2] - 1);
	}
	if(input->mouse_button[2] == TRUE && input->last_mouse_button[2] == FALSE)
	{
		static_geo_start = 0;
		static_bitmap_start = 1;
		x = input->pointer_x;
		y = input->pointer_y;
	}
	ring = sui_draw_popup(input, x, y, element, count, 2, 0);


	start = static_geo_start;
	node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY);
	for(i = 1; node != NULL && i < start; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
		i++;
	if(start != 0)
		count = 1;
	else
		count = 0;
	for(; node != NULL && count < MAX_NODE_DISPLAY_COUNT; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
	{
		if(NULL != e_nsg_get_layer_by_name(node, "map_u") && NULL != e_nsg_get_layer_by_name(node, "map_v"))
			sui_button_uv_create(x + 0.15, y - 0.1 - (float)count * 0.05, 1, 1, 1);
		count++;
	}

	

	switch(ring)
	{
		case 0 :
			uvg_undo();
		break;
		case 1 :
			uvg_redo();
		break;
		case 2 :
			uv_draw_texture(e_ns_get_node_next(0, 0, V_NT_BITMAP));
		break;
	}




	start = static_geo_start;
	node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY);
	for(i = 1; node != NULL && i < start; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
		i++;
	count = 3;
	if(ring > 3 && ring != -1)		
		for(; node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
			if(ring == count++)
				uvg_set_node(e_ns_get_node_id(node));
	


	start = static_bitmap_start;
	node = e_ns_get_node_next(0, 0, V_NT_BITMAP);
	for(i = 1; node != NULL && i < start; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_BITMAP))
		i++;
	count = 0;
	if(ring > 3 && ring != -1)		
		for(; node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_BITMAP))
			if(ring == count++)
				uvo_set_bitmap_node(e_ns_get_node_id(node));



	if(input->mode == BAM_DRAW)
		glPopMatrix();

}
void uv_pu_empty_old(BInputState *input)
{
	SUIPUElement element[MAX_NODE_DISPLAY_COUNT + 4];
	static float x = 0, y = 0;
	float *cam;
	uint ring, top_count = 0, bottom_count = 0;
/*
	for(node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
		top_count++;
*/
	element[0].type = PU_T_ANGLE;
	element[0].text = "NOTHING";
	element[0].data.angle[0] = -45;
	element[0].data.angle[1] = 45;
	element[1].type = PU_T_ANGLE;
	element[1].text = "UnDo";
	element[1].data.angle[0] = 45;
	element[1].data.angle[1] = 135;
	element[2].type = PU_T_ANGLE;
	element[2].text = "ReDo";
	element[2].data.angle[0] = 225;
	element[2].data.angle[1] = 315;
	element[3].type = PU_T_ANGLE;
	element[3].text = "CREATE BITMAP";
	element[3].data.angle[0] = 135;
	element[3].data.angle[1] = 225;

	if(input->mode == BAM_DRAW)
	{
		glPushMatrix();
		cam = get_cam_pos();
		glTranslatef(-cam[0], -cam[1], cam[2] - 1);
	}
	if(input->mouse_button[2] == TRUE && input->last_mouse_button[2] == FALSE)
	{
		x = input->pointer_x;
		y = input->pointer_y;
	}
	ring = sui_draw_popup(input, x, y, element, 4, 2, 0);
	switch(ring)
	{
		case 0 :
		break;
		case 1 :
			uvg_undo();
		break;
		case 2 :
			uvg_redo();
		break;
		case 3 :
			uv_draw_texture(e_ns_get_node_next(0, 0, V_NT_BITMAP));
		break;
	}
	if(input->mode == BAM_DRAW)
		glPopMatrix();
}

void uv_pu_polygon(BInputState *input, uint poly)
{
	SUIPUElement element[6];
	static float x = 0, y = 0;
	float *cam;
	uint ring;
	element[0].type = PU_T_ANGLE;
	element[0].text = "Square";
	element[0].data.angle[0] = -30;
	element[0].data.angle[1] = 30;
	element[1].type = PU_T_ANGLE;
	element[1].text = "Rectangle";
	element[1].data.angle[0] = 30;
	element[1].data.angle[1] = 90;
	element[2].type = PU_T_ANGLE;
	element[2].text = "Normal Select";
	element[2].data.angle[0] = 90;
	element[2].data.angle[1] = 150;
	element[3].type = PU_T_ANGLE;
	element[3].text = "Plane Select";
	element[3].data.angle[0] = 150;
	element[3].data.angle[1] = 210;
	element[4].type = PU_T_ANGLE;
	element[4].text = "Poly Project";
	element[4].data.angle[0] = 210;
	element[4].data.angle[1] = 270;
	element[5].type = PU_T_ANGLE;
	element[5].text = "Strip";
	element[5].data.angle[0] = 270;
	element[5].data.angle[1] = 330;



	if(input->mode == BAM_DRAW)
	{
		glPushMatrix();
		cam = get_cam_pos();
		glTranslatef(-cam[0], -cam[1], cam[2] - 1);
	}
	if(input->mouse_button[2] == TRUE && input->last_mouse_button[2] == FALSE)
	{
		x = input->pointer_x;
		y = input->pointer_y;
	}
	ring = sui_draw_popup(input, x, y, element, 6, 2, 0);
	switch(ring)
	{
		case 0 : 
			uv_tool_poly_square(poly);
		break;
		case 1 :
			uv_tool_poly_rectangle(poly);
		break;
		case 2 :
			uv_tool_poly_normal_select(poly);
		break;
		case 3 :
			uv_tool_poly_plane_select(poly);
		break;
		case 4 :
			uv_tool_poly_project(poly);
		break;
		case 5 :
	//		uv_strip(poly, 0);
			 uv_strip_plane(poly);
		break;
	}
	if(input->mode == BAM_DRAW)
		glPopMatrix();
}

void uv_pu_corner(BInputState *input, uint poly, uint corner)
{
	SUIPUElement element[4];
	static float x = 0, y = 0;
	float *cam;
	uint ring;
	element[0].type = PU_T_ANGLE;
	element[0].text = "Colapse";
	element[0].data.angle[0] = -45;
	element[0].data.angle[1] = 45;
	element[1].type = PU_T_ANGLE;
	element[1].text = "Select";
	element[1].data.angle[0] = 45;
	element[1].data.angle[1] = 135;
	element[2].type = PU_T_ANGLE;
	element[2].text = "Straight";
	element[2].data.angle[0] = 225;
	element[2].data.angle[1] = 315;
	element[3].type = PU_T_ANGLE;
	element[3].text = "CORNER";
	element[3].data.angle[0] = 135;
	element[3].data.angle[1] = 225;

	if(input->mode == BAM_DRAW)
	{
		glPushMatrix();
		cam = get_cam_pos();
		glTranslatef(-cam[0], -cam[1], cam[2] - 1);
	}
	if(input->mouse_button[2] == TRUE && input->last_mouse_button[2] == FALSE)
	{
		x = input->pointer_x;
		y = input->pointer_y;
	}
	ring = sui_draw_popup(input, x, y, element, 4, 2, 0);
	switch(ring)
	{
		case 0 :
//			uv_tool_corner_streight_uv(poly,  corner);
			uv_tool_corner_colaps(poly,  corner);
//			uv_tool_edge_streight(poly,  corner);
		break;
		case 1 :
			uv_tool_corner_select(poly,  corner);
		break;
		case 2 :
//			uv_tool_corner_streight_uv(poly,  corner);
//			uv_tool_corner_colaps(poly,  corner);
			uv_tool_edge_straight_uv(poly,  corner);
		break;
		case 3 :
//			uv_tool_corner_streight_uv(poly,  corner);
//			uv_tool_corner_colaps(poly,  corner);
//			uv_tool_edge_streight(poly,  corner);
		break;
	}
	if(input->mode == BAM_DRAW)
		glPopMatrix();
}

void uv_pu_edge(BInputState *input, uint poly, uint edge)
{
	SUIPUElement element[4];
	static float x = 0, y = 0;
	float *cam;
	uint ring;
	element[0].type = PU_T_ANGLE;
	element[0].text = "Streight Edge";
	element[0].data.angle[0] = -45;
	element[0].data.angle[1] = 45;
	element[1].type = PU_T_ANGLE;
	element[1].text = "Streight Select";
	element[1].data.angle[0] = 45;
	element[1].data.angle[1] = 135;
	element[2].type = PU_T_ANGLE;
	element[2].text = "Streight Poly";
	element[2].data.angle[0] = 225;
	element[2].data.angle[1] = 315;
	element[3].type = PU_T_ANGLE;
	element[3].text = "EDGE";
	element[3].data.angle[0] = 135;
	element[3].data.angle[1] = 225;

	if(input->mode == BAM_DRAW)
	{
		glPushMatrix();
		cam = get_cam_pos();
		glTranslatef(-cam[0], -cam[1], cam[2] - 1);
	}
	if(input->mouse_button[2] == TRUE && input->last_mouse_button[2] == FALSE)
	{
		x = input->pointer_x;
		y = input->pointer_y;
	}
	ring = sui_draw_popup(input, x, y, element, 4, 2, 0);
	switch(ring)
	{
		case 0 :
			uv_tool_edge_streight(poly, edge);
		break;
		case 1 :
			uv_tool_edge_select_streight(poly, edge);
		break;
		case 2 :
			uv_tool_edge_poly_streight(poly, edge);
		break;
		case 3 :
			uv_tool_edge_streight(poly, edge);
		break;
	}
	if(input->mode == BAM_DRAW)
		glPopMatrix();
}


void uv_pu_manip(BInputState *input)
{
	SUIPUElement element[7];
	static float x = 0, y = 0;
	float *cam;
	uint ring;
	element[0].type = PU_T_ANGLE;
	element[0].text = "Flip Horozontal";
	element[0].data.angle[0] = 45;
	element[0].data.angle[1] = 90;

	element[1].type = PU_T_ANGLE;
	element[1].text = "Flip Vertical";
	element[1].data.angle[0] = 90;
	element[1].data.angle[1] = 135;

	element[2].type = PU_T_ANGLE;
	element[2].text = "Square";
	element[2].data.angle[0] = 225;
	element[2].data.angle[1] = 270;

	element[3].type = PU_T_ANGLE;
	element[3].text = "Square";
	element[3].data.angle[0] = 270;
	element[3].data.angle[1] = 315;

	element[4].type = PU_T_TOP;
	element[4].text = "Project Front";

	element[5].type = PU_T_TOP;
	element[5].text = "Project Side";

	element[6].type = PU_T_TOP;
	element[6].text = "Project Top";


	if(input->mode == BAM_DRAW)
	{
		glPushMatrix();
		cam = get_cam_pos();
		glTranslatef(-cam[0], -cam[1], cam[2] - 1);
	}
	if(input->mouse_button[2] == TRUE && input->last_mouse_button[2] == FALSE)
	{
		x = input->pointer_x;
		y = input->pointer_y;
	}
	ring = sui_draw_popup(input, x, y, element, 7, 2, 0);
	switch(ring)
	{

		case 0 :
			uv_tool_flip_x();
		break;
		case 1 :
			uv_tool_flip_y();
		break;
		case 2 :
			uv_fit_selection(FALSE);
		break;
		case 3 :
			uv_fit_selection(TRUE);
		break;
		case 4 :
			uv_tool_project(0, 1, FALSE);
		break;
		case 5 :
			uv_tool_project(1, 2, FALSE);
		break;
		case 6 :
			uv_tool_project(0, 2, FALSE);
		break;
	}
	if(input->mode == BAM_DRAW)
		glPopMatrix();
}
