#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"
#include "seduce.h"
#include "co_vn_graphics.h"
#include "co_vn_handle.h"
#include "co_widgets.h"

extern boolean co_handle_head(BInputState *input, ENode *node, float *length);
extern float co_handle_node_head(BInputState *input, ENode *node);
/*
ETextBuffer *		e_nst_get_buffer_next(ENode *node, uint buffer_id);
char *				e_nst_get_buffer_data(ENode *node, ESTextBuffer *buffer);
uint				e_nst_get_buffer_data_length(ENode *node, ESTextBuffer *buffer);
uint				e_nst_get_buffer_id(ETextBuffer *buffer);
char *				e_nst_get_buffer_name(ETextBuffer *buffer);
extern void verse_send_t_buffer_create(VNodeID node_id, VNMBufferID buffer_id, const char *name);
extern void verse_send_t_buffer_destroy(VNodeID node_id, VNMBufferID buffer_id);
*/
uint change_t_node_id;


void rename_buffer_func(void *user, char *text)
{
	ETextBuffer *buffer;
	ENode *node;
	if((node = e_ns_get_node(0, change_t_node_id)) != NULL)
	{
		for(buffer = e_nst_get_buffer_next(node, 0); buffer != NULL ; buffer = e_nst_get_buffer_next(node, e_nst_get_buffer_id(buffer) + 1))
		if(buffer == user)
			verse_send_t_buffer_create(change_t_node_id, e_nst_get_buffer_id(buffer), text);
	}
}

void rename_language_func(void *user, char *text)
{
	ENode *node;
	if((node = e_ns_get_node(0, change_t_node_id)) != NULL)
		verse_send_t_set_language(change_t_node_id, text);
}

boolean co_handle_text(BInputState *input, ENode *node)
{
	static float rot_buffer = 0, rot_language = 0;
	static boolean show_buffer = TRUE, show_language = TRUE;
	static uint16 buffer_edit_id = 0;
	ETextBuffer *buffer;
	float pos = 1, y, pre_expander, color, color_light;
	uint i;
	change_t_node_id = e_ns_get_node_id(node);
	
	y = co_handle_node_head(input, node);

	co_vng_divider(input, 0.2, y, &rot_language, &color, &color_light, &show_language, "Language");
	pre_expander = y;
	y -= 0.05;
	sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Language:", color_light, color_light, color_light);  
	co_w_type_in(input, 0.15, y, 0.5, SUI_T_SIZE, e_nst_get_language(node), 512, rename_language_func, NULL, color, color_light);
	y -= 0.05 + (y - pre_expander) * (1 - rot_language);
	if(input->mode == BAM_DRAW)
		glPopMatrix();
	co_vng_divider(input, 0.2, y, &rot_buffer, &color, &color_light, &show_buffer, "Buffers");
	pre_expander = y;

	if(rot_buffer > 0.001)
	{
		if(sw_text_button(input, -0.3, y - 0.05, 0, SUI_T_SIZE, SUI_T_SPACE, "Create new Buffer", color, color, color))
		{
			char nr[32];
			i = 0;
			for(buffer = e_nst_get_buffer_next(node, 0); buffer != NULL ; buffer = e_nst_get_buffer_next(node, e_nst_get_buffer_id(buffer) + 1))
				i++;
			sprintf(nr, "buffer_%u", i);
			verse_send_t_buffer_create(e_ns_get_node_id(node), (uint16)-1, nr);
		}
		y -= 0.05;
		if(NULL == e_nst_get_buffer_next(node, 0))
		{
			sui_draw_text(-0.3, y - 0.1, SUI_T_SIZE, SUI_T_SPACE, "NO TEXT BUFFERS", color_light, color_light, color_light);
			y -= 0.05;
		}

		y -= 0.05;

		for(buffer = e_nst_get_buffer_next(node, 0); buffer != NULL ; buffer = e_nst_get_buffer_next(node, e_nst_get_buffer_id(buffer) + 1))
		{
			boolean e;
			sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Buffer name:", color_light, color_light, color_light);  
			co_w_type_in(input, 0.15, y, 0.5, SUI_T_SIZE, e_nst_get_buffer_name(buffer), 16, rename_buffer_func, buffer, color, color_light);
			if(co_w_close_button(input, 0.635, y, color, color, color))
				verse_send_t_buffer_destroy(e_ns_get_node_id(node), e_nst_get_buffer_id(buffer));
			y -= 0.05;
			e = buffer_edit_id != e_nst_get_buffer_id(buffer);
			if(co_w_checkbox(input, 0.17, y, &e, color, color, color) && !e)
				buffer_edit_id = e_nst_get_buffer_id(buffer);
			y -= 0.05;
			sui_draw_rounded_square(-0.3, y + 0.125, 1, -0.09, color_light, color_light, color_light);
		}

	}
	y -= 0.05 + (y - pre_expander) * (1 - rot_buffer);
	if(input->mode == BAM_DRAW)
		glPopMatrix();
	if(input->mode == BAM_DRAW)
		glPopMatrix();
	return co_handle_return(input);
}
