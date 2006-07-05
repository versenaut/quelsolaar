#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"
#include "seduce.h"
#include "co_vn_graphics.h"
#include "co_vn_handle.h"
#include "co_widgets.h"

uint32 change_node_id = 0;

void rename_node_func(void *user, char *text)
{
	verse_send_node_name_set(change_node_id, text);
}

void rename_group_func(void *user, char *text)
{
	uint i;
	ENode *node;
	if((node = e_ns_get_node(0, change_node_id)) != NULL)
	{
		for(i = e_ns_get_next_tag_group(node, 0); i != -1 && user != e_ns_get_tag_group(node, i); i = e_ns_get_next_tag_group(node, i + 1));
		if(i != -1)
			verse_send_tag_group_create(change_node_id, i, text);
	}
}

void rename_tag_func(void *user, char *text)
{
	uint i, j;
	ENode *node;
	if((node = e_ns_get_node(0, change_node_id)) != NULL)
	{
		for(i = e_ns_get_next_tag_group(node, 0); i != -1 ; i = e_ns_get_next_tag_group(node, i + 1))
		{
			for(j = e_ns_get_next_tag(node, i, 0); j != -1 && user != e_ns_get_tag_name(node, i, j); j = e_ns_get_next_tag(node, i, j + 1));
			if(j != -1)
			{
				verse_send_tag_create(change_node_id, i, j, text, e_ns_get_tag_type(node, i, j), e_ns_get_tag(node, i, j));
				return;
			}
		}
	}
}

void tag_string_func(void *user, char *text)
{
	uint i, j;
	VNTag tag;
	ENode *node;
	if((node = e_ns_get_node(0, change_node_id)) != NULL)
	{
		for(i = e_ns_get_next_tag_group(node, 0); i != -1 ; i = e_ns_get_next_tag_group(node, i + 1))
		{
			for(j = e_ns_get_next_tag(node, i, 0); j != -1 && user != e_ns_get_tag_name(node, i, j); j = e_ns_get_next_tag(node, i, j + 1));
			if(j != -1)
			{
				tag.vstring = text;
				verse_send_tag_create(change_node_id, i, j, e_ns_get_tag_name(node, i, j), e_ns_get_tag_type(node, i, j), &tag);
				return;
			}
		}
	}
}

void tag_number_func(void *user, char *text)
{
	uint i, j;
	float f;
	VNTag tag;
	ENode *node;
	if((node = e_ns_get_node(0, change_node_id)) != NULL)
	{
		for(i = e_ns_get_next_tag_group(node, 0); i != -1 ; i = e_ns_get_next_tag_group(node, i + 1))
		{
			for(j = e_ns_get_next_tag(node, i, 0); j != -1 && user != e_ns_get_tag_name(node, i, j); j = e_ns_get_next_tag(node, i, j + 1));
			if(j != -1)
			{
				if(VN_TAG_REAL64 == e_ns_get_tag_type(node, i, j) && sscanf(text, "%f", &f) == 1)
				{
					tag.vreal64 = (double)f;
					verse_send_tag_create(change_node_id, i, j, e_ns_get_tag_name(node, i, j), VN_TAG_REAL64, &tag);
				}
				if(VN_TAG_UINT32 == e_ns_get_tag_type(node, i, j) && sscanf(text, "%u", &tag.vuint32) == 1)
					verse_send_tag_create(change_node_id, i, j, e_ns_get_tag_name(node, i, j), VN_TAG_UINT32, &tag);
				if(VN_TAG_LINK == e_ns_get_tag_type(node, i, j) && sscanf(text, "%u", &tag.vlink) == 1 && e_ns_get_node(0, tag.vlink) != NULL)
					verse_send_tag_create(change_node_id, i, j, e_ns_get_tag_name(node, i, j), VN_TAG_LINK, &tag);
				return;
			}
		}
	}
}



void tag_vector_func(void *user, char *text)
{
	uint i, j, k;
	float f;
	VNTag tag, *t;
	ENode *node;
	if((node = e_ns_get_node(0, change_node_id)) != NULL)
	{
		for(i = e_ns_get_next_tag_group(node, 0); i != -1 ; i = e_ns_get_next_tag_group(node, i + 1))
		{
			for(j = e_ns_get_next_tag(node, i, 0); j != -1 ; j = e_ns_get_next_tag(node, i, j + 1))
			if(VN_TAG_REAL64_VEC3 == e_ns_get_tag_type(node, i, j))
			{
				t = e_ns_get_tag(node, i, j);
				for(k = 0; k < 3; k++)
				{
					if(user == &t->vreal64_vec3[k] && sscanf(text, "%e", &f) == 1)
					{
						tag.vreal64_vec3[k] = f;
						tag.vreal64_vec3[(k + 1) % 3] = t->vreal64_vec3[(k + 1) % 3];
						tag.vreal64_vec3[(k + 2) % 3] = t->vreal64_vec3[(k + 2) % 3];
						verse_send_tag_create(change_node_id, i, j, e_ns_get_tag_name(node, i, j), VN_TAG_REAL64_VEC3, &tag);
						return;
					}
				}
			}
		}
	}
}

void tag_animation_func(void *user, char *text)
{
	uint i, j, k;
	float f;
	VNTag tag, *t;
	ENode *node;
	if((node = e_ns_get_node(0, change_node_id)) != NULL)
	{
		for(i = e_ns_get_next_tag_group(node, 0); i != -1 ; i = e_ns_get_next_tag_group(node, i + 1))
		{
			for(j = e_ns_get_next_tag(node, i, 0); j != -1 ; j = e_ns_get_next_tag(node, i, j + 1))
			if(VN_TAG_ANIMATION == e_ns_get_tag_type(node, i, j))
			{
				t = e_ns_get_tag(node, i, j);
				tag.vanimation.curve = t->vanimation.curve;
				tag.vanimation.start = t->vanimation.start;
				tag.vanimation.end = t->vanimation.end;
				if(user == &t->vanimation.curve && sscanf(text, "%u", &t->vanimation.curve) == 1)
				{
					verse_send_tag_create(change_node_id, i, j, e_ns_get_tag_name(node, i, j), VN_TAG_ANIMATION, &tag);
					return;
				}
				if(user == &t->vanimation.start && sscanf(text, "%f", &f) == 1)
				{
					tag.vanimation.start = f;
					verse_send_tag_create(change_node_id, i, j, e_ns_get_tag_name(node, i, j), VN_TAG_ANIMATION, &tag);
					return;
				}
				if(user == &t->vanimation.end && sscanf(text, "%f", &f) == 1)
				{
					tag.vanimation.end = f;
					verse_send_tag_create(change_node_id, i, j, e_ns_get_tag_name(node, i, j), VN_TAG_ANIMATION, &tag);
					return;
				}
			}
		}
	}
}

void tag_blob_count_func(void *user, char *text)
{
	uint i, j, k, count;
	char blob[VN_TAG_MAX_BLOB_SIZE];
	VNTag tag, *t;
	ENode *node;
	if((node = e_ns_get_node(0, change_node_id)) != NULL)
	{
		for(i = e_ns_get_next_tag_group(node, 0); i != -1 ; i = e_ns_get_next_tag_group(node, i + 1))
		{
			for(j = e_ns_get_next_tag(node, i, 0); j != -1 && user != e_ns_get_tag_name(node, i, j); j = e_ns_get_next_tag(node, i, j + 1));
			if(j != -1)
			{
				if(VN_TAG_BLOB == e_ns_get_tag_type(node, i, j) && sscanf(text, "%u", &count) == 1 && count < VN_TAG_MAX_BLOB_SIZE)
				{
					t = e_ns_get_tag(node, i, j);
					for(k = 0; k < t->vblob.size; k++)
						((uint8 *)blob)[i] = ((uint8 *)t->vblob.blob)[i];
					for(k = t->vblob.size; k < count; k++)
						((uint8 *)blob)[i] = 0;
					tag.vblob.size = count;
					tag.vblob.blob = blob;
					verse_send_tag_create(change_node_id, i, j, e_ns_get_tag_name(node, i, j), VN_TAG_BLOB, &tag);
				}
				return;
			}
		}
	}
}

void clear_tag(VNTag *tag, VNTagType type)
{
	switch(type)
	{
		case VN_TAG_BOOLEAN :
			tag->vboolean = TRUE;
		break;
		case VN_TAG_UINT32 :
			tag->vuint32 = 0;
		break;
		case VN_TAG_REAL64 :
			tag->vreal64 = 0;
		break;
		case VN_TAG_STRING :
			tag->vstring = "text";
		break;
		case VN_TAG_REAL64_VEC3 :
			tag->vreal64_vec3[0] = 1;
			tag->vreal64_vec3[1] = 0;
			tag->vreal64_vec3[2] = 0;
		break;
		case VN_TAG_LINK :
			tag->vlink = 0;
		break;
		case VN_TAG_ANIMATION :
			tag->vanimation.curve = 0;
			tag->vanimation.end = 1;
			tag->vanimation.start = 0;
		break;
		case VN_TAG_BLOB :
			tag->vblob.size = 5;
			tag->vblob.blob = "ABCDE";
		break;
	}
}

//float co_handle_head(BInputState *input, ENode *node)
//boolean co_handle_popup_exit(BInputState *input, ENode *node)

float co_handle_node_head(BInputState *input, ENode *node, boolean reset);

boolean co_handle_head(BInputState *input, ENode *node, float *length)
{
	co_handle_node_head(input, node, FALSE);
	return TRUE;
}

float co_handle_node_head(BInputState *input, ENode *node, boolean reset)
{
	static float pos = 0, tag_rot = 0, temp = 0;
	static boolean show_tags = TRUE, grab = FALSE;
	static uint16 a_tag = -1, a_group = -1;
	float size, color, color_light;
	double value;
	char *type_names[] = {"Object", "Geometry", "Material", "Bitmap", "Text", "Curve", "Audio"};
	uint16 i, j, k, l;
	uint32 m;
	VNTag *tag, t;
	float y;

	change_node_id = e_ns_get_node_id(node);

	if(input->mode == BAM_DRAW)
		glPushMatrix();

	y = betray_get_screen_mode(NULL, NULL, NULL) - 0.1;

	if(input->mode != BAM_DRAW)
	{
		if(input->pointer_x > 0.7 && input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
			grab = TRUE;
		if(input->mouse_button[0] == FALSE)
			grab = FALSE;
		if(grab == TRUE)
			pos += input->delta_pointer_y * 2;
		if(reset)
			pos = 0;
	}
	y += pos;

	if(input->mode == BAM_DRAW)
		sui_draw_text(0.0, y - 0.075, SUI_T_SIZE, SUI_T_SPACE, "Name:", 1.0 - 1.0 / 4.0, 1.0 - 1.0 / 4.0, 1.0 - 1.0 / 4.0);  
	co_w_type_in(input, 0.15, y - 0.075, 0.5, SUI_T_SIZE, e_ns_get_node_name(node), 16, rename_node_func, NULL, 0, 0.4);
	if(input->mode == BAM_DRAW)
	{
		sui_draw_text(0.0, y - 0.125, SUI_T_SIZE, SUI_T_SPACE, "Type:", 1.0 - 1.0 / 4.0, 1.0 - 1.0 / 4.0, 1.0 - 1.0 / 4.0); 
		sui_draw_text(0.15, y - 0.125, SUI_T_SIZE, SUI_T_SPACE, type_names[e_ns_get_node_type(node)], 1.0 - 1.0 / 4.0, 1.0 - 1.0 / 4.0, 1.0 - 1.0 / 4.0); 
	}
	co_w_scroll(input, 0, 2);
	y -= 0.15;
	co_vng_divider(input, 0.2, y, &tag_rot, &color, &color_light, &show_tags, "Tags");
	y -= 0.05;
	
	if(tag_rot > 0.01)
	{
		if(sw_text_button(input, -0.3, y, 0, SUI_T_SIZE, SUI_T_SPACE, "Create new tag group", color, color, color))
		{
			char nr[32];
			j = 0;
			for(i = e_ns_get_next_tag_group(node, 0); i != (uint16) ~0u ; i = e_ns_get_next_tag_group(node, i + 1))
				j++;
			sprintf(nr, "group_%u", j);
			verse_send_tag_group_create(e_ns_get_node_id(node), (uint16) ~0u, nr);
		}
		y -= 0.05;
		if((uint16) -1 == e_ns_get_next_tag_group(node, 0))
		{
			sui_draw_text(-0.3, y, SUI_T_SIZE, SUI_T_SPACE, "No Tag Groups", color_light, color_light, color_light);  
			y -= 0.05;
		}
		for(i = e_ns_get_next_tag_group(node, 0); i != (uint16) -1 ; i = e_ns_get_next_tag_group(node, i + 1))
		{
			size = 0.0;
			sui_draw_text(0.0, y + size, SUI_T_SIZE, SUI_T_SPACE, "Group name:", color_light, color_light, color_light);  
			co_w_type_in(input, 0.15, y + size, 0.5, SUI_T_SIZE, e_ns_get_tag_group(node, i), 16, rename_group_func, e_ns_get_tag_group(node, i), color, color_light);
			if(sw_text_button(input, -0.27, y + size, 0, SUI_T_SIZE, SUI_T_SPACE, "Create new tag", color, color, color))
			{
				char nr[32];
				k = 0;
				for(j = e_ns_get_next_tag(node, i, 0); j != (uint16) ~0u ; j = e_ns_get_next_tag(node, i, j + 1))
					k++;
				sprintf(nr, "tag_%u", k);
				t.vboolean = TRUE;
				verse_send_tag_create(e_ns_get_node_id(node), i, (uint16) ~0u, nr, VN_TAG_BOOLEAN, &t);
			}
			if(co_w_close_button(input, 0.635, y + size, color, color, color))
				verse_send_tag_group_destroy(e_ns_get_node_id(node), i);
			size = -0.05;
			for(j = e_ns_get_next_tag(node, i, 0); j != (uint16) ~0u ; j = e_ns_get_next_tag(node, i, j + 1))
			{
				static char *tag_type_names[] = {"VN_TAG_BOOLEAN", "VN_TAG_UINT32","VN_TAG_REAL64", "VN_TAG_STRING", "VN_TAG_REAL64_VEC3", "VN_TAG_LINK", "VN_TAG_ANIMATION", "VN_TAG_BLOB"};
				tag = e_ns_get_tag(node, i, j);
				k = e_ns_get_tag_type(node, i, j);

				sui_draw_text(0.0, y + size, SUI_T_SIZE, SUI_T_SPACE, "Tag name:", color_light, color_light, color_light);  
				co_w_type_in(input, 0.15, y + size, 0.5, SUI_T_SIZE, e_ns_get_tag_name(node, i, j), 16, rename_tag_func, e_ns_get_tag_name(node, i, j), color, color_light);
				
				if(co_w_close_button(input, 0.635, y + size, color, color, color))
					verse_send_tag_destroy(e_ns_get_node_id(node), i, j);
				size -= 0.05;
				sui_draw_text(0.0, y + size, SUI_T_SIZE, SUI_T_SPACE, "Tag type:", color_light, color_light, color_light);  
				sui_draw_text(0.15, y + size, SUI_T_SIZE, SUI_T_SPACE, tag_type_names[k], color, color, color);  

				if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE && sui_box_click_test(0.0, y + size - SUI_T_SIZE, 0.7, SUI_T_SIZE * 3))
				{
					a_group = i;
					a_tag = j;
				}

				if(a_group == i && a_tag == j)
				{
					SUIPUElement e[8];
					uint output;
					for(l = 0; l < 8; l++)
						e[l].text = tag_type_names[l]; 
					for(l = 0; l < 4; l++)
					{
						e[l].type = PU_T_ANGLE; 
						e[l].data.angle[0] = 45 + (float)l * 22.5;
						e[l].data.angle[1] = 45 + (float)(l + 1) * 22.5;
					}
					for(; l < 8; l++)
					{
						e[l].type = PU_T_ANGLE; 
						e[l].data.angle[0] = 180 + 45 + (float)(l - 4) * 22.5;
						e[l].data.angle[1] = 180 + 45 + (float)(l - 3) * 22.5;
					}
					output = sui_draw_popup(input, 0.15, y + size, e, 8, 0, 0.9);
					clear_tag(&t, output);
					if(output != -1 && output != k)
						verse_send_tag_create(e_ns_get_node_id(node), i, j, e_ns_get_tag_name(node, i, j), output, &t);
				}
				size -= 0.05;
				t = *tag;
				switch(k)
				{
					case VN_TAG_BOOLEAN :
						sui_draw_text(0.0, y + size, SUI_T_SIZE, SUI_T_SPACE, "Boolean:", color_light, color_light, color_light);  
						if(co_w_checkbox(input, 0.17, y + size, &t.vboolean, color, color, color))
							verse_send_tag_create(e_ns_get_node_id(node), i, j, e_ns_get_tag_name(node, i, j), k, &t);
						sui_draw_rounded_square(-0.01, y + size + 0.125, 0.7, -0.14, color_light, color_light, color_light);
						size -= 0.05;
					break;
					case VN_TAG_UINT32 :
						sui_draw_text(0.0, y + size, SUI_T_SIZE, SUI_T_SPACE, "Integer:", color_light, color_light, color_light);  
						if(sui_type_number_uint(input, 0.15, y + size, 0, 0.5, SUI_T_SIZE, &t.vuint32, tag, color, color, color))
							verse_send_tag_create(e_ns_get_node_id(node), i, j, e_ns_get_tag_name(node, i, j), k, &t);
						sui_draw_rounded_square(-0.01, y + size + 0.125, 0.7, -0.14, color_light, color_light, color_light);
						size -= 0.05;
					break;
					case VN_TAG_REAL64 :
						sui_draw_text(0.0, y + size, SUI_T_SIZE, SUI_T_SPACE, "Real:", color_light, color_light, color_light);  
						if(sui_type_number_double(input, 0.15, y + size, 0, 0.5, SUI_T_SIZE, &t.vreal64, tag, color, color, color))
							verse_send_tag_create(e_ns_get_node_id(node), i, j, e_ns_get_tag_name(node, i, j), k, &t);
						sui_draw_rounded_square(-0.01, y + size + 0.125, 0.7, -0.14, color_light, color_light, color_light);
						size -= 0.05;
					break;
					case VN_TAG_STRING :
						sui_draw_text(0.0, y + size, SUI_T_SIZE, SUI_T_SPACE, "String:", color_light, color_light, color_light);  
						co_w_type_in(input, 0.15, y + size, 0.7, SUI_T_SIZE, tag->vstring, 16, tag_string_func, e_ns_get_tag_name(node, i, j), color, color_light);
						sui_draw_rounded_square(-0.01, y + size + 0.125, 0.7, -0.14, color_light, color_light, color_light);
						size -= 0.05;
					break;
					case VN_TAG_REAL64_VEC3 :
						sui_draw_text(0.0, y + size, SUI_T_SIZE, SUI_T_SPACE, "Vector:", color_light, color_light, color_light);  
						if(sui_type_number_double(input, 0.15, y + size, 0, 0.5, SUI_T_SIZE, &t.vreal64_vec3[0], &tag->vreal64_vec3[0], color, color, color))
							verse_send_tag_create(e_ns_get_node_id(node), i, j, e_ns_get_tag_name(node, i, j), k, &t);
						size -= 0.05;
						if(sui_type_number_double(input, 0.15, y + size, 0, 0.5, SUI_T_SIZE, &t.vreal64_vec3[1], &tag->vreal64_vec3[1], color, color, color))
							verse_send_tag_create(e_ns_get_node_id(node), i, j, e_ns_get_tag_name(node, i, j), k, &t);
						size -= 0.05;
						if(sui_type_number_double(input, 0.15, y + size, 0, 0.5, SUI_T_SIZE, &t.vreal64_vec3[2], &tag->vreal64_vec3[2], color, color, color))
							verse_send_tag_create(e_ns_get_node_id(node), i, j, e_ns_get_tag_name(node, i, j), k, &t);
						size -= 0.05;
						sui_draw_rounded_square(-0.01, y + size + 0.275, 0.7, -0.24, color_light, color_light, color_light);
					break;
					case VN_TAG_LINK :
						sui_draw_text(0.0, y + size, SUI_T_SIZE, SUI_T_SPACE, "'Link:", color_light, color_light, color_light);  
						if(sui_type_number_uint(input, 0.15, y + size, 0, 0.5, SUI_T_SIZE, &t.vlink, tag, color, color, color))
							verse_send_tag_create(e_ns_get_node_id(node), i, j, e_ns_get_tag_name(node, i, j), k, &t);
						sui_draw_rounded_square(-0.01, y + size + 0.125, 0.7, -0.14, color_light, color_light, color_light);
						size -= 0.05;
					break;
					case VN_TAG_ANIMATION :
						sui_draw_text(0.0, y + size, SUI_T_SIZE, SUI_T_SPACE, "Curve:", color_light, color_light, color_light);
						if(sui_type_number_uint(input, 0.15, y + size, 0, 0.5, SUI_T_SIZE, &t.vanimation.curve, &tag->vanimation.curve, color, color, color))
							verse_send_tag_create(e_ns_get_node_id(node), i, j, e_ns_get_tag_name(node, i, j), k, &t);
						size -= 0.05;
						sui_draw_text(0.0, y + size, SUI_T_SIZE, SUI_T_SPACE, "Start:", color_light, color_light, color_light);  
						if(sui_type_number_uint(input, 0.15, y + size, 0, 0.5, SUI_T_SIZE, &t.vanimation.start, &tag->vanimation.start, color, color, color))
							verse_send_tag_create(e_ns_get_node_id(node), i, j, e_ns_get_tag_name(node, i, j), k, &t);
						size -= 0.05;
						sui_draw_text(0.0, y + size, SUI_T_SIZE, SUI_T_SPACE, "End:", color_light, color_light, color_light);  
						if(sui_type_number_uint(input, 0.15, y + size, 0, .5, SUI_T_SIZE, &t.vanimation.end, &tag->vanimation.end, color, color, color))
							verse_send_tag_create(e_ns_get_node_id(node), i, j, e_ns_get_tag_name(node, i, j), k, &t);
						size -= 0.05;
						sui_draw_rounded_square(-0.01, y + size + 0.275, 0.7, -0.24, color_light, color_light, color_light);
					break;
					case VN_TAG_BLOB :
						sui_draw_text(0.0, y + size, SUI_T_SIZE, SUI_T_SPACE, "Size:", color_light, color_light, color_light);  
						m = t.vblob.size;
						if(sui_type_number_uint(input, 0.15, y + size, 0, 0.5, SUI_T_SIZE, &m, tag, color, color, color))
						{
							uint8 blob[VN_TAG_MAX_BLOB_SIZE];
							t.vblob.size = m;
							if(t.vblob.size >= VN_TAG_MAX_BLOB_SIZE)
								t.vblob.size = VN_TAG_MAX_BLOB_SIZE - 1;
							for(l = 0; l < tag->vblob.size; l++)
								blob[l] = ((uint8 *)t.vblob.blob)[l];
							for(; l < t.vblob.size; l++)
								blob[l] = 0;
							t.vblob.blob = blob;
							verse_send_tag_create(e_ns_get_node_id(node), i, j, e_ns_get_tag_name(node, i, j), k, &t);
						}

						size -= 0.05;
						sui_draw_text(0.0, y + size, SUI_T_SIZE, SUI_T_SPACE, "Blob:", color_light, color_light, color_light); 
						size += 0.05;
						for(l = 0; l < tag->vblob.size; l++)
						{
							uint32 entry;
							if(l % 8 == 0)
								size -= 0.05;
							entry = ((uint8*)tag->vblob.blob)[l];
					/*		{
								char nr[54];
								sprintf(nr, "%u", entry);
								sui_draw_text(0.15 + (0.07 * (float)(l % 8)), y + size, SUI_T_SIZE, SUI_T_SPACE, "Size:", color_light, color_light, color_light);
							}*/
							if(sui_type_number_uint(input, 0.15 + (0.07 * (float)(l % 8)), y + size, 0, 0.5, SUI_T_SIZE, &entry, &((uint8*)tag->vblob.blob)[l], color, color, color))
							{
								uint8 blob[VN_TAG_MAX_BLOB_SIZE];
								for(m = 0; m < tag->vblob.size; m++)
									blob[m] = ((uint8 *)t.vblob.blob)[m];
								if(entry < 256)
									blob[l] = (uint8)entry;
								else
									blob[l] = 255;
								t.vblob.blob = blob;
								verse_send_tag_create(e_ns_get_node_id(node), i, j, e_ns_get_tag_name(node, i, j), k, &t);
							}
						}
						sui_draw_rounded_square(-0.01, y + size + 0.175 + 0.05 * (float)(tag->vblob.size / 8), 0.7, -0.19 - 0.05 * (float)(tag->vblob.size / 8), color_light, color_light, color_light);
						size -= 0.05;
					break;
				}
			}
			sui_draw_rounded_square(-0.3, y + 0.025, 1, size, color_light, color_light, color_light);
			y += size - 0.02;
		}

		if(input->mouse_button[0] == FALSE)
		{
			a_group = -1;
			a_tag = -1;
		}
	}
	if(input->mode == BAM_DRAW)
		glPopMatrix();
	return (betray_get_screen_mode(NULL, NULL, NULL) - 0.3) * (1 - tag_rot) + y * tag_rot;
}

boolean co_handle_return(BInputState *input)
{
	if(input->mode == BAM_EVENT && input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE)
		if(0.4 * 0.4 > (input->pointer_x + 1) * (input->pointer_x + 1) + input->pointer_y * input->pointer_y)
			if(0.4 * 0.4 > (input->click_pointer_x + 1) * (input->click_pointer_x + 1) + input->click_pointer_y * input->click_pointer_y)
				return FALSE;		
	return TRUE;
}
