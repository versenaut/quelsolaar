#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"
#include "seduce.h"
#include "co_vn_graphics.h"
#include "co_vn_handle.h"
#include "co_widgets.h"

extern float co_handle_node_head(BInputState *input, ENode *node);
extern boolean co_handle_head(BInputState *input, ENode *node, float *length);

uint change_a_node_id;

void rename_a_buffer_func(void *user, char *text)
{
	EAudioBuffer *buffer;
	ENode *node;
	if((node = e_ns_get_node(0, change_a_node_id)) != NULL)
	{
		for(buffer = e_nsa_get_buffer_next(node, 0); buffer != NULL && buffer != user ; buffer = e_nsa_get_buffer_next(node, e_nsa_get_buffer_id(buffer) + 1));
		if(buffer == user)
			verse_send_a_buffer_create(change_a_node_id, e_nsa_get_buffer_id(buffer), text, e_nsa_get_buffer_type(buffer), e_nsa_get_buffer_frequency(buffer));
	}
}

void rename_a_stream_func(void *user, char *text)
{
	EAudioStream *stream;
	ENode *node;
	if((node = e_ns_get_node(0, change_a_node_id)) != NULL)
	{
		for(stream = e_nsa_get_stream_next(node, 0); stream != NULL && stream != user ; stream = e_nsa_get_stream_next(node, e_nsa_get_stream_id(stream) + 1));
		if(stream == user)
			verse_send_a_stream_create(change_a_node_id, e_nsa_get_stream_id(stream), text);
	}
}


boolean co_handle_audio(BInputState *input, ENode *node)
{
	static char *buffer_type_names[] = {"VN_A_LAYER_INT8", "VN_A_LAYER_INT16", "VN_A_LAYER_INT24", "VN_A_LAYER_INT32", "VN_A_LAYER_REAL32", "VN_A_LAYER_REAL64"};
	static float rot_buffer = 0, rot_stream = 0;
	static boolean show_buffer = TRUE, show_stream = TRUE;
	static uint16 buffer_edit_id = 0, a_buffer_edit_type_id = -1;
	EAudioBuffer *buffer;
	EAudioStream *stream;
	double frequency;
	float pos = 1, y, pre_expander, color, color_light;
	uint i;
	change_a_node_id = e_ns_get_node_id(node);
	
	y = co_handle_node_head(input, node);

	co_vng_divider(input, 0.2, y, &rot_buffer, &color, &color_light, &show_buffer, "Layers");
	pre_expander = y;

	if(rot_buffer > 0.001)
	{
		if(sw_text_button(input, -0.3, y - 0.05, 0, SUI_T_SIZE, SUI_T_SPACE, "Create new Layer", color, color, color))
		{

			char nr[32];
			i = 0;
			for(buffer = e_nsa_get_buffer_next(node, 0); buffer != NULL ; buffer = e_nsa_get_buffer_next(node, e_nsa_get_buffer_id(buffer) + 1))
				i++;
			sprintf(nr, "buffer_%u", i);
			verse_send_a_buffer_create(e_ns_get_node_id(node), (uint16)-1, nr, VN_A_BLOCK_INT16, 44100);
		}
		y -= 0.05;
		if(NULL == e_nsa_get_buffer_next(node, 0))
		{
			sui_draw_text(-0.3, y - 0.1, SUI_T_SIZE, SUI_T_SPACE, "NO AUDIO LAYERS", color_light, color_light, color_light);
			y -= 0.05;
		}

		y -= 0.05;

		for(buffer = e_nsa_get_buffer_next(node, 0); buffer != NULL ; buffer = e_nsa_get_buffer_next(node, e_nsa_get_buffer_id(buffer) + 1))
		{
			boolean e;
			sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Layer name:", color_light, color_light, color_light);  
			co_w_type_in(input, 0.15, y, 0.5, SUI_T_SIZE, e_nsa_get_buffer_name(buffer), 16, rename_a_buffer_func, buffer, color, color_light);
			if(co_w_close_button(input, 0.635, y, color, color, color))
				verse_send_a_buffer_destroy(e_ns_get_node_id(node), e_nsa_get_buffer_id(buffer));
			y -= 0.05;
	
			sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Layer type:", color_light, color_light, color_light);  
			sui_draw_text(0.15, y, SUI_T_SIZE, SUI_T_SPACE, buffer_type_names[e_nsa_get_buffer_type(buffer)], color, color, color);  

			if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE && sui_box_click_test(0.0, y - SUI_T_SIZE, 0.7, SUI_T_SIZE * 3))
				a_buffer_edit_type_id = e_nsa_get_buffer_id(buffer);
			if(a_buffer_edit_type_id == e_nsa_get_buffer_id(buffer))
			{
				SUIPUElement e[6];
				uint output;
				for(i = 0; i < 6; i++)
					e[i].text = buffer_type_names[i]; 
				for(i = 0; i < 3; i++)
				{
					e[i].type = PU_T_ANGLE; 
					e[i].data.angle[0] = 30 + (float)i * 40;
					e[i].data.angle[1] = 30 + (float)(i + 1) * 40;
				}
				for(; i < 6; i++)
				{
					e[i].type = PU_T_ANGLE; 
					e[i].data.angle[0] = 180 + 30 + (float)(i - 3) * 40;
					e[i].data.angle[1] = 180 + 30 + (float)(i - 2) * 40;
				}
				output = sui_draw_popup(input, 0.15, y, e, 8, 0);
				if(output != -1 && output != e_nsa_get_buffer_type(buffer))
					verse_send_a_buffer_create(change_a_node_id, e_nsa_get_buffer_id(buffer), e_nsa_get_buffer_name(buffer), output, e_nsa_get_buffer_frequency(buffer));
			}
			if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == FALSE)
				a_buffer_edit_type_id = -1;
			y -= 0.05;

			sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Frequency:", color_light, color_light, color_light);
			frequency = e_nsa_get_buffer_frequency(buffer);
			if(sui_type_number_double(input, 0.15, y, 0.5, SUI_T_SIZE, &frequency, buffer, color, color, color))
				verse_send_a_buffer_create(change_a_node_id, e_nsa_get_buffer_id(buffer), e_nsa_get_buffer_name(buffer), e_nsa_get_buffer_type(buffer), frequency);
			y -= 0.05;
			e = buffer_edit_id != e_nsa_get_buffer_id(buffer);
			if(co_w_checkbox(input, 0.17, y, &e, color, color, color) && !e)
				buffer_edit_id = e_nsa_get_buffer_id(buffer);
			y -= 0.05;
			sui_draw_rounded_square(-0.3, y + 0.225, 1, -0.19, color_light, color_light, color_light);
		}

	}
	y -= (y - pre_expander + 0.05) * (1 - rot_buffer);
	if(input->mode == BAM_DRAW)
		glPopMatrix();
	co_vng_divider(input, 0.2, y, &rot_stream, &color, &color_light, &show_stream, "Streams");
	pre_expander = y;

	if(rot_stream > 0.001)
	{
		if(sw_text_button(input, -0.3, y - 0.05, 0, SUI_T_SIZE, SUI_T_SPACE, "Create new Stream", color, color, color))
		{
			char nr[32];
			i = 0;
			for(stream = e_nsa_get_stream_next(node, 0); stream != NULL ; stream = e_nsa_get_stream_next(node, e_nsa_get_stream_id(stream) + 1))
				i++;
			sprintf(nr, "stream_%u", i);
			verse_send_a_stream_create(e_ns_get_node_id(node), (uint16)-1, nr);
		}
		y -= 0.05;
		if(NULL == e_nsa_get_stream_next(node, 0))
		{
			sui_draw_text(-0.3, y - 0.1, SUI_T_SIZE, SUI_T_SPACE, "NO AUDIO STREAMS", color_light, color_light, color_light);
			y -= 0.05;
		}

		y -= 0.05;

		for(stream = e_nsa_get_stream_next(node, 0); stream != NULL ; stream = e_nsa_get_stream_next(node, e_nsa_get_stream_id(stream) + 1))
		{
			boolean e;
			sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Stream name:", color_light, color_light, color_light);  
			co_w_type_in(input, 0.15, y, 0.5, SUI_T_SIZE, e_nsa_get_stream_name(stream), 16, rename_a_stream_func, stream, color, color_light);
			if(co_w_close_button(input, 0.635, y, color, color, color))
				verse_send_a_stream_destroy(e_ns_get_node_id(node), e_nsa_get_buffer_id(stream));
			y -= 0.05;
			sui_draw_rounded_square(-0.3, y + 0.075, 1, -0.04, color_light, color_light, color_light);
		}

	}
	y -= 0.05 + (y - pre_expander) * (1 - rot_buffer);
	if(input->mode == BAM_DRAW)
		glPopMatrix();
	if(input->mode == BAM_DRAW)
		glPopMatrix();
	return co_handle_return(input);
}
