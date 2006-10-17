
#include "st_matrix_operations.h"
#include "enough.h"
#include "seduce.h"
#include "deceive.h"

#include "lp_layer_groups.h"
#include "lp_projection.h"


void lp_geometry_draw(ENode *node, EGeoLayer *red, EGeoLayer *green, EGeoLayer *blue);
extern uint lp_pu_empty(BInputState *input, uint node_id);

void lp_unlock_paint(void);
void lp_lock_paint(ENode *node, EGeoLayer *red, EGeoLayer *green, EGeoLayer *blue);
void lp_color_paint(BInputState *input, ENode *node);
void lp_apply_paint(ENode *node, EGeoLayer *red, EGeoLayer *green, EGeoLayer *blue, double *value, uint integer);

typedef enum{
	PLIM_NONE,
	PLIM_VIEW,
	PLIM_PAINT,
	PLIM_POPUP
}PLInputMode;

PLInputMode input_mode = PLIM_NONE;
void lp_menu(BInputState *input, ENode *node, double *slider, uint *integer);

void lp_input_handler(BInputState *input, void *user)
{
	static double slider[3] = {0, 0, 0};
	static uint node_id = -1, integer = 0;
	ENode *node;
	node = e_ns_get_node(0, node_id);

	if(V_NT_GEOMETRY != e_ns_get_node_type(node))
		node = NULL;

	if(node == NULL)
	{
		node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY);
		if(node != NULL)
			node_id = e_ns_get_node_id(node);
	}
	if(input->mode == BAM_MAIN)
	{
		verse_callback_update(3000);
		return;
	}
	if(node != NULL)
		lp_update_layer_groups(node);


	if(input->mode == BAM_DRAW)
	{
//		glDisable(GL_DEPTH_TEST);
		glClearColor(0.4, 0.4, 0.4, 0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glPushMatrix();
		p_view_set();
		sui_set_blend_gl(GL_ONE, GL_ZERO);
		if(node != 0)
		{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		lp_geometry_draw(node, lp_layer_get_red(lp_layer_current_get()), lp_layer_get_green(lp_layer_current_get()), lp_layer_get_blue(lp_layer_current_get()));
		glPolygonOffset(1.0, 1.0);
		glDepthFunc(GL_LEQUAL);
		glPolygonOffset(0, 0);
		sui_set_blend_gl(GL_ONE, GL_ONE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		lp_geometry_draw(node, lp_layer_get_red(lp_layer_current_get()), lp_layer_get_green(lp_layer_current_get()), lp_layer_get_blue(lp_layer_current_get()));
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_POLYGON_OFFSET_FILL);


		}sui_set_blend_gl(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glPopMatrix();
	}
	lp_menu(input, node, slider, &integer);
	switch(input_mode)
	{
		case PLIM_NONE :
			if(input->mouse_button[1] && !input->last_mouse_button[1])
			{
				input_mode = PLIM_VIEW;
				p_view_change_start(input);
			}
			if(input->mouse_button[0] && !input->last_mouse_button[0] && node != NULL)
			{
				input_mode = PLIM_PAINT;
				lp_lock_paint(node, lp_layer_get_red(lp_layer_current_get()), lp_layer_get_green(lp_layer_current_get()), lp_layer_get_blue(lp_layer_current_get()));
			}
			if(input->mouse_button[2] && !input->last_mouse_button[2])
				input_mode = PLIM_POPUP;
			
		break;
		case PLIM_VIEW :
			p_view_change(input);
			if(!input->mouse_button[1])
				input_mode = PLIM_NONE;				
		break;
		case PLIM_PAINT :
			if(input->mouse_button[0])
			{
				static uint counter = 0;
				double value[3] = {0.9, 0.9, 0.9};
				lp_color_paint(input, node);
				if(counter++ % 5 == 0)
					lp_apply_paint(node, lp_layer_get_red(lp_layer_current_get()), lp_layer_get_green(lp_layer_current_get()), lp_layer_get_blue(lp_layer_current_get()), slider, integer);
			}else
			{
				input_mode = PLIM_NONE;
				lp_unlock_paint();
			}
		break;
		case PLIM_POPUP :
			node_id = lp_pu_empty(input, node_id);
			if(!input->mouse_button[2])
				input_mode = PLIM_NONE;
		break;
	}

}
