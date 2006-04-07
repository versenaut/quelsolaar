#include <math.h>
#include "st_matrix_operations.h"
#include "enough.h"
#include "seduce.h"
#include "deceive.h"

#define MAX_GROUP_DISPLAY_COUNT 16 
#define MAX_NODE_DISPLAY_COUNT 16 

uint lp_pu_empty(BInputState *input, uint node_id)
{
	SUIPUElement element[4 + MAX_NODE_DISPLAY_COUNT + MAX_GROUP_DISPLAY_COUNT];
	static float x, y, static_start = 0;
	uint start = 0, ring, count = 4, i, groups;
	ENode *node;

	start = static_start;
/*	element[0].type = PU_T_ANGLE;
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
	element[3].text = "NEW GEOMETRY";*/
	count = 0;

	
	node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY);
	for(i = 1; node != NULL && i < start; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
		i++;


	groups = lp_layer_get_group_count();
	if(groups > MAX_GROUP_DISPLAY_COUNT)
		groups = MAX_GROUP_DISPLAY_COUNT;
	
	for(i = 0; i < groups; i++)
	{
		element[count].type = PU_T_TOP;
		element[count].text = lp_layer_get_name(i);
		count++;
	}





	if(start != 0)
	{
		if(y - 0.175 < input->pointer_y)
			static_start -= betray_get_delta_time() * 10 * (0.2 + input->pointer_y + (y - 0.175));
		element[count].type = PU_T_BOTTOM;
		element[count].text = "More...";
		count++;
	}

	for(; node != NULL && count < MAX_NODE_DISPLAY_COUNT +  + 3; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
	{
		element[count].type = PU_T_BOTTOM;
		element[count].text = e_ns_get_node_name(node);
		count++;
	}
	if(count == MAX_NODE_DISPLAY_COUNT + 3 && node != NULL)
	{
		if(y - 0.05 * MAX_NODE_DISPLAY_COUNT - 0.075 > input->pointer_y)
			static_start += betray_get_delta_time() * 10 * (0.2 + (y - 0.05 * MAX_NODE_DISPLAY_COUNT - 0.075) - input->pointer_y);
		element[count].type = PU_T_BOTTOM;
		element[count].text = "More...";
		count++;
	}
	if(input->mode == BAM_DRAW)
	{
		glDisable(GL_DEPTH_TEST);
		glPopMatrix();
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0, 0, -1);
	}
	if(input->mouse_button[2] == TRUE && input->last_mouse_button[2] == FALSE)
	{
		x = input->pointer_x;
		y = input->pointer_y;
	}
	ring = sui_draw_popup(input, x, y, element, count, 2, 0);

				/*udg_set_modeling_node(e_ns_get_node_id(node))*/;

//	sw_drawbackground();
	if(input->mode == BAM_DRAW)
	{
		glPopMatrix();
		glPushMatrix();
		glPushMatrix();
		glEnable(GL_DEPTH_TEST);
	}
	node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY);
	for(i = 1; node != NULL && i < start; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
		i++;

	if(ring > 0 && ring < groups)
		lp_layer_current_set(ring);

	count = groups;
//	if(start != 0)
//		count++;
	if(ring != -1)		
		for(; node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
			if(ring == count++)
				return e_ns_get_node_id(node);

	return node_id;
}