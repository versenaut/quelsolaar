#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"
#include "seduce.h"

#include "uv_geometry.h"
#include "uv.h"

void uv_pu_empty(BInputState *input)
{
	SUIPUElement element[4];
	static float x = 0, y = 0;
	float *cam;
	uint ring;
	element[0].type = PU_T_ANGLE;
	element[0].text = "NOTHING";
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
	element[3].type = PU_T_ANGLE;
	element[3].text = "NOTHING";
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
	ring = sui_draw_popup(input, x, y, element, 4, 2);
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
	ring = sui_draw_popup(input, x, y, element, 6, 2);
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
	element[0].text = "Collapse";
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
	ring = sui_draw_popup(input, x, y, element, 4, 2);
	switch(ring)
	{
		case 0 :
/*			uv_tool_corner_streight_uv(poly,  corner);*/
			uv_tool_corner_colaps(poly,  corner);
/*			uv_tool_edge_streight(poly,  corner);*/
		break;
		case 1 :
			uv_tool_corner_select(poly,  corner);
		break;
		case 2 :
/*			uv_tool_corner_streight_uv(poly,  corner);
			uv_tool_corner_colaps(poly,  corner);
*/			uv_tool_edge_straight_uv(poly,  corner);
		break;
		case 3 :
/*			uv_tool_corner_streight_uv(poly,  corner);
			uv_tool_corner_colaps(poly,  corner);
			uv_tool_edge_streight(poly,  corner);
*/		break;
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
	element[0].text = "Straight Edge";
	element[0].data.angle[0] = -45;
	element[0].data.angle[1] = 45;
	element[1].type = PU_T_ANGLE;
	element[1].text = "Straight Select";
	element[1].data.angle[0] = 45;
	element[1].data.angle[1] = 135;
	element[2].type = PU_T_ANGLE;
	element[2].text = "Straight Poly";
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
	ring = sui_draw_popup(input, x, y, element, 4, 2);
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
	ring = sui_draw_popup(input, x, y, element, 7, 2);
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
