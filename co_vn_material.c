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

uint change_m_node_id = 0;

void rename_m_fragment(void *user, char *text)
{
	VNMFragmentID i;
	uint j;
	ENode *node;
	VMatFrag *frag, f;
	if((node = e_ns_get_node(0, change_m_node_id)) != NULL)
	{
		for(i = e_nsm_get_fragment_next(node, 0); i != (VNMFragmentID)-1; i = e_nsm_get_fragment_next(node, i + 1))
		{
			frag = e_nsm_get_fragment(node, i);
			switch(e_nsm_get_fragment_type(node, i))
			{
				case VN_M_FT_LIGHT :
					if(frag->light.brdf_r == user || frag->light.brdf_g == user || frag->light.brdf_b == user)
					{
						f = *frag;
						if(frag->light.brdf_r == user)
							for(j = 0; j < 16; j++)
								f.light.brdf_r[j] = text[j];
						if(frag->light.brdf_g == user)
							for(j = 0; j < 16; j++)
								f.light.brdf_g[j] = text[j];
						if(frag->light.brdf_b == user)
							for(j = 0; j < 16; j++)
								f.light.brdf_b[j] = text[j];
						verse_send_m_fragment_create(change_m_node_id, i, VN_M_FT_LIGHT, &f);
					}
				break;
				case VN_M_FT_GEOMETRY :
					if(frag->geometry.layer_r == user || frag->geometry.layer_g == user || frag->geometry.layer_b == user)
					{
						f = *frag;
						if(frag->geometry.layer_r == user)
							for(j = 0; j < 16; j++)
								f.geometry.layer_r[j] = text[j];
						if(frag->geometry.layer_g == user)
							for(j = 0; j < 16; j++)
								f.geometry.layer_g[j] = text[j];
						if(frag->geometry.layer_b == user)
							for(j = 0; j < 16; j++)
								f.geometry.layer_b[j] = text[j];
						verse_send_m_fragment_create(change_m_node_id, i, VN_M_FT_GEOMETRY, &f);
					}
				break;
				case VN_M_FT_TEXTURE :
					if(frag->texture.layer_r == user || frag->texture.layer_g == user || frag->texture.layer_b == user)
					{
						f = *frag;
						if(frag->texture.layer_r == user)
							for(j = 0; j < 16; j++)
								f.texture.layer_r[j] = text[j];
						if(frag->texture.layer_g == user)
							for(j = 0; j < 16; j++)
								f.texture.layer_g[j] = text[j];
						if(frag->texture.layer_b == user)
							for(j = 0; j < 16; j++)
								f.texture.layer_b[j] = text[j];
						verse_send_m_fragment_create(change_m_node_id, i, VN_M_FT_TEXTURE, &f);
					}
				break;
				case VN_M_FT_ANIMATION :
					if(frag->animation.label == user)
					{
						f = *frag;
						if(frag->animation.label == user)
							for(j = 0; j < 16; j++)
								f.animation.label[j] = text[j];
						verse_send_m_fragment_create(change_m_node_id, i, VN_M_FT_TEXTURE, &f);
					}
				break;
				case VN_M_FT_OUTPUT :
					if(frag->output.label == user)
					{
						f = *frag;
						for(j = 0; j < 16; j++)
							f.output.label[j] = text[j];
						verse_send_m_fragment_create(change_m_node_id, i, VN_M_FT_OUTPUT, &f);
					}
				break;
			}
		}
	}
}

void clear_new_frag(VMatFrag *frag, uint type)
{
	char *color = "color", *name = "name", *group = "group", *col_r = "col_r", *col_g = "col_g", *col_b = "col_b";
	uint i;
	switch(type)
	{	
		case VN_M_FT_COLOR :
			frag->color.red = 0.5;
			frag->color.green = 0.5;
			frag->color.blue = 0.5;
			break;
		case VN_M_FT_LIGHT :
			frag->light.brdf = -1;
			frag->light.brdf_r[0] = 0; 
			frag->light.brdf_g[0] = 0; 
			frag->light.brdf_b[0] = 0;
			frag->light.normal_falloff = 0;
			frag->light.type = VN_M_LIGHT_DIRECT_AND_AMBIENT;
			break;
		case VN_M_FT_REFLECTION :
			frag->reflection.normal_falloff = 0;
			break;
		case VN_M_FT_TRANSPARENCY :
			frag->transparency.normal_falloff = 0;
			frag->transparency.refraction_index = 1;
			break;
		case VN_M_FT_VOLUME :
			frag->volume.diffusion = 0.5;
			frag->volume.col_r = 0.5;
			frag->volume.col_g = 0.5;
			frag->volume.col_b = 0.5;
			frag->volume.color = -1;
			break;
		case VN_M_FT_GEOMETRY :
			for(i = 0; col_r[i] != 0; i++)
				frag->geometry.layer_r[i] = col_r[i]; 
			frag->geometry.layer_r[i] = 0; 
			for(i = 0; col_g[i] != 0; i++)
				frag->geometry.layer_g[i] = col_g[i]; 
			frag->geometry.layer_g[0] = 0; 
			for(i = 0; col_b[i] != 0; i++)
				frag->geometry.layer_b[i] = col_b[i]; 
			frag->geometry.layer_b[0] = 0; 
			break;
		case VN_M_FT_TEXTURE :
			frag->texture.bitmap = -1;
			frag->texture.mapping = -1;
			for(i = 0; col_r[i] != 0; i++)
				frag->texture.layer_r[i] = col_r[i]; 
			frag->texture.layer_r[i] = 0; 
			for(i = 0; col_g[i] != 0; i++)
				frag->texture.layer_g[i] = col_g[i]; 
			frag->texture.layer_g[0] = 0; 
			for(i = 0; col_b[i] != 0; i++)
				frag->texture.layer_b[i] = col_b[i]; 
			frag->texture.layer_b[0] = 0; 
			break;
		case VN_M_FT_NOISE :
			frag->noise.mapping = -1;
			frag->noise.type = 0;
			break;
		case VN_M_FT_BLENDER :
			frag->blender.control = -1;
			frag->blender.data_a = -1;
			frag->blender.data_b = -1;
			frag->blender.type = VN_M_BLEND_ADD;
			break;
		case VN_M_FT_MATRIX :
			frag->matrix.data = -1;
			for(i = 0; i < 16; i++)
			{
				if((i % 5) == 0)
					frag->matrix.matrix[i] = 1;
				else
					frag->matrix.matrix[i] = 0;
			}
			break;
		case VN_M_FT_RAMP :
			frag->ramp.mapping = -1;
			frag->ramp.channel = VN_M_RAMP_BLUE;
			frag->ramp.point_count = 3;
			frag->ramp.type = VN_M_RAMP_SQUARE;
			frag->ramp.ramp[0].pos = 0;
			frag->ramp.ramp[0].red = 1;
			frag->ramp.ramp[0].green = 0;
			frag->ramp.ramp[0].blue = 0;
			frag->ramp.ramp[1].pos = 0.5;
			frag->ramp.ramp[1].red = 0;
			frag->ramp.ramp[1].green = 1;
			frag->ramp.ramp[1].blue = 0;
			frag->ramp.ramp[2].pos = 1;
			frag->ramp.ramp[2].red = 0;
			frag->ramp.ramp[2].green = 0;
			frag->ramp.ramp[2].blue = 1;
			break;
		case VN_M_FT_ANIMATION :
			frag->animation.label[0] = 0;
		case VN_M_FT_ALTERNATIVE :
			frag->alternative.alt_a = -1;
			frag->alternative.alt_b = -1;
			break;
		case VN_M_FT_OUTPUT :
			frag->output.back = -1;
			frag->output.front = -1;
			for(i = 0; color[i] != 0; i++)
				frag->output.label[i] = color[i];
			frag->output.label[i] = 0;
			break;
	}
}

#define LINK_SECTIONS 40
#define TEXTURE_RESOLUTION 128

typedef struct{
	float		pos[2];
	float		size;
	uint		texture_id;
	uint		version;
	boolean		expand;
	boolean		recursive;
	boolean		placed;
}COVNMaterial;


void co_m_place_frag(ENode *node, uint16 id, float pos_x, float vec, uint gen)
{
	VMatFrag *frag;
	COVNMaterial *pos;
	frag = e_nsm_get_fragment(node, id);
	if(frag == NULL)
		return;
	pos = e_nsm_get_custom_data(node, id, CO_ENOUGH_NODE_SLOT);
	if(pos != NULL && !pos->placed)
	{

		pos_x += vec;

		pos->pos[0] = pos_x;
		pos->pos[1] = (float)gen * -0.14 - 0.1;
		pos->placed = TRUE;
		vec *= 0.5;
		switch(e_nsm_get_fragment_type(node, id))
		{	
			case VN_M_FT_TEXTURE :
				co_m_place_frag(node, frag->texture.mapping, pos_x, vec, gen + 1);
				break;
			case VN_M_FT_NOISE :
				co_m_place_frag(node, frag->noise.mapping, pos_x, vec, gen + 1);
				break;
			case VN_M_FT_BLENDER :
				co_m_place_frag(node, frag->blender.data_a, pos_x, vec - 0.1, gen + 1);
				co_m_place_frag(node, frag->blender.data_b, pos_x, vec, gen + 1);
				co_m_place_frag(node, frag->blender.control, pos_x, vec + 0.1, gen + 1);
				break;
			case VN_M_FT_MATRIX :
				co_m_place_frag(node, frag->matrix.data, pos_x, vec, gen + 1);
				break;
			case VN_M_FT_RAMP :
				co_m_place_frag(node, frag->ramp.mapping, pos_x, vec, gen + 1);
				break;
			case VN_M_FT_ALTERNATIVE :
				co_m_place_frag(node, frag->alternative.alt_a, pos_x, vec - 0.1, gen + 1);
				co_m_place_frag(node, frag->alternative.alt_b, pos_x, vec + 0.1, gen + 1);
				break;
			case VN_M_FT_OUTPUT :
				co_m_place_frag(node, frag->output.front, pos_x, vec, gen + 1);
			break;
		}
	}
}

void co_place_all_fragments(ENode *node)
{
	VNMFragmentID id;
	COVNMaterial *pos;
	float pos_x = 0;
	for(id = e_nsm_get_fragment_next(node, 0); id != (VNMFragmentID)-1; id = e_nsm_get_fragment_next(node, id + 1))
		if(e_nsm_get_fragment_type(node, id) == VN_M_FT_OUTPUT && (pos = e_nsm_get_custom_data(node, id, CO_ENOUGH_NODE_SLOT)) != NULL && pos->placed != TRUE)
			co_m_place_frag(node, id, pos_x++ * 0.05, 0, 0);
	for(id = e_nsm_get_fragment_next(node, 0); id != (VNMFragmentID)-1; id = e_nsm_get_fragment_next(node, id + 1))
		if(e_nsm_get_fragment_type(node, id) == VN_M_FT_BLENDER && (pos = e_nsm_get_custom_data(node, id, CO_ENOUGH_NODE_SLOT)) != NULL && pos->placed != TRUE)
			co_m_place_frag(node, id, pos_x++ * 0.05, 0, 0);
	for(id = e_nsm_get_fragment_next(node, 0); id != (VNMFragmentID)-1; id = e_nsm_get_fragment_next(node, id + 1))
		if((e_nsm_get_fragment_type(node, id) != VN_M_FT_TEXTURE && e_nsm_get_fragment_type(node, id) != VN_M_FT_NOISE && e_nsm_get_fragment_type(node, id) != VN_M_FT_MATRIX && e_nsm_get_fragment_type(node, id) != VN_M_FT_RAMP && e_nsm_get_fragment_type(node, id) != VN_M_FT_ALTERNATIVE) && (pos = e_nsm_get_custom_data(node, id, CO_ENOUGH_NODE_SLOT)) != NULL && pos->placed != TRUE)
			co_m_place_frag(node, id, pos_x++ * 0.05, 0, 0);
	for(id = e_nsm_get_fragment_next(node, 0); id != (VNMFragmentID)-1; id = e_nsm_get_fragment_next(node, id + 1))
		if((pos = e_nsm_get_custom_data(node, id, CO_ENOUGH_NODE_SLOT)) != NULL && pos->placed != TRUE)
			co_m_place_frag(node, id, pos_x++ * 0.05, 0, 0);
}


float compute_spline(float f, float v0, float v1, float v2, float v3)
{
	return ((v0 * f + v1 * (1 - f)) * f + (v1 * f + v2 * (1 - f)) * (1 - f)) * f +	((v1 * f + v2 * (1 - f)) * f + (v2 * f + v3 * (1 - f)) * (1 - f)) * (1 - f);
}

void draw_spline(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, float color)
{
	float vertex[LINK_SECTIONS * 2];
	uint i;
	for(i = 0; i < LINK_SECTIONS; i++)
		sui_draw_set_vec2(vertex, i, compute_spline((float)((i + 1) / 2) / (float)LINK_SECTIONS / 0.5, x0, x1, x2, x3), compute_spline((float)((i + 1) / 2) / (float)LINK_SECTIONS / 0.5, y0, y1, y2, y3));
	sui_draw_gl(GL_LINES, vertex, LINK_SECTIONS, 2, color, color, color);
}

void draw_frag_link(float x, float y, float rot, float color)
{
	static float vertex[128] = {100001};
	if(vertex[0] > 100000)
	{
		uint i;
		for(i = 0; i < 32; i++)
		{
			sui_draw_set_vec2(vertex, i * 2, 0.02 * cos((double)i * 3.14 / 32), -0.15 - 0.02 * sin((double)i * 3.14 / 32));
			sui_draw_set_vec2(vertex, i * 2 + 1, 0.02 * cos((double)(i + 1) * 3.14 / 32), -0.15 - 0.02 * sin((double)(i + 1) * 3.14 / 32));
		}
	}
	glPushMatrix();
	glTranslatef(x, y, 0);
	glRotatef(-rot, 0, 0, 1);
	sui_draw_gl(GL_LINES, vertex, 64, 2, color, color, color);
	glPopMatrix();
}


void set_out_link(ENode *node, uint16 id, uint16 link)
{
	VMatFrag *frag, f;
	if((frag = e_nsm_get_fragment(node, id)) == NULL)
		return;
	f = *frag;
	switch(e_nsm_get_fragment_type(node, id))
	{
		case VN_M_FT_VOLUME :
			f.volume.color = link;
			verse_send_m_fragment_create(e_ns_get_node_id(node), id, VN_M_FT_VOLUME, &f);
		break;
		case VN_M_FT_TEXTURE :
			f.texture.mapping = link;
			verse_send_m_fragment_create(e_ns_get_node_id(node), id, VN_M_FT_TEXTURE, &f);
		break;
		case VN_M_FT_NOISE :
			f.noise.mapping = link;
			verse_send_m_fragment_create(e_ns_get_node_id(node), id, VN_M_FT_NOISE, &f);
		break;
		case VN_M_FT_BLENDER :
			if((frag = e_nsm_get_fragment(node, f.blender.data_a)) == NULL)
				f.blender.data_a = link;
			else if((frag = e_nsm_get_fragment(node, f.blender.data_b)) == NULL)
				f.blender.data_b = link;
			else if((frag = e_nsm_get_fragment(node, f.blender.control)) == NULL && f.blender.type == VN_M_BLEND_FADE)
				f.blender.control = link;
			else
				f.blender.data_a = link;
			verse_send_m_fragment_create(e_ns_get_node_id(node), id, VN_M_FT_BLENDER, &f);
		break;
		case VN_M_FT_MATRIX :
			f.matrix.data = link;
			verse_send_m_fragment_create(e_ns_get_node_id(node), id, VN_M_FT_MATRIX, &f);
		break;
		case VN_M_FT_RAMP :
			f.ramp.mapping = link;
			verse_send_m_fragment_create(e_ns_get_node_id(node), id, VN_M_FT_RAMP, &f);
		break;
		case VN_M_FT_ALTERNATIVE :
			if((frag = e_nsm_get_fragment(node, f.alternative.alt_a)) == NULL)
				f.alternative.alt_a = link;
			if((frag = e_nsm_get_fragment(node, f.alternative.alt_b)) == NULL)
				f.alternative.alt_b = link;
			else
				f.alternative.alt_a = link;
			verse_send_m_fragment_create(e_ns_get_node_id(node), id, VN_M_FT_ALTERNATIVE, &f);
		break;
		case VN_M_FT_OUTPUT :
			f.output.front = link;
			verse_send_m_fragment_create(e_ns_get_node_id(node), id, VN_M_FT_OUTPUT, &f);
	}

}

boolean handle_link(BInputState *input, ENode *node, uint16 *link, char *text, uint frag_id, float angle, float scroll, float length, float color, uint16 move)
{
	static void *drag = NULL;
	COVNMaterial *pos, *target;
	VMatFrag *frag;
	uint i, *active;
	VNMFragmentID id;
	float f, pos_x, pos_y;
	float a, b;

	pos_x = sin(-angle * PI / 180); 
	pos_y = cos(-angle * PI / 180); 
	frag = e_nsm_get_fragment(node, *link);
	target = e_nsm_get_custom_data(node, *link, CO_ENOUGH_NODE_SLOT);
	pos = e_nsm_get_custom_data(node, frag_id, CO_ENOUGH_NODE_SLOT);
	if(input->mode == BAM_DRAW)
	{
		draw_frag_link(pos->pos[0], pos->pos[1] - length + scroll, angle, color);
		if(drag == link)
			draw_spline(pos->pos[0] - pos_x * 0.17, pos->pos[1] - length + scroll - pos_y * 0.17,
						pos->pos[0] - pos_x * 0.5, pos->pos[1] - length + scroll - pos_y * 0.5,
						pos->pos[0] + (input->pointer_x - pos->pos[0]) / pos->size, pos->pos[1] + scroll + (input->pointer_y - scroll - pos->pos[1]) / pos->size + 0.05 / pos->size,
						pos->pos[0] + (input->pointer_x - pos->pos[0]) / pos->size, pos->pos[1] + scroll + (input->pointer_y - scroll - pos->pos[1]) / pos->size, color);
		else if(frag != NULL && target != NULL)
		{
			draw_spline(pos->pos[0] - pos_x * 0.17, pos->pos[1] - length + scroll - pos_y * 0.17,
						pos->pos[0] - pos_x * 0.5, pos->pos[1] - length + scroll - pos_y * 0.5,
						pos->pos[0] + (target->pos[0] - pos->pos[0]) / pos->size, pos->pos[1] + scroll + (target->pos[1] - pos->pos[1]) / pos->size + 0.05 / pos->size,
						pos->pos[0] + (target->pos[0] - pos->pos[0]) / pos->size, pos->pos[1] + scroll + (target->pos[1] - pos->pos[1]) / pos->size, color);
		
			a = compute_spline(0.5, pos->pos[0] - pos_x * 0.17, pos->pos[0] - pos_x * 0.5, pos->pos[0] + (target->pos[0] - pos->pos[0]) / pos->size, pos->pos[0] + (target->pos[0] - pos->pos[0]) / pos->size);
			b = compute_spline(0.5, pos->pos[1] - length + scroll - pos_y * 0.17, pos->pos[1] - length + scroll - pos_y * 0.5, pos->pos[1] + scroll + (target->pos[1] - pos->pos[1]) / pos->size + 0.05 / pos->size, pos->pos[1] + scroll + (target->pos[1] - pos->pos[1]) / pos->size);
			glPushMatrix();
			glTranslatef(a, b, 0);
			a = compute_spline(0.5, 
				pos->pos[0] - (pos_x * 0.17) * pos->size, 
				pos->pos[0] - (pos_x * 0.5) * pos->size, 
				target->pos[0], 
				target->pos[0]);
			b = compute_spline(0.5, 
				pos->pos[1] + scroll - (length + pos_y * 0.17) * pos->size, 
				pos->pos[1] + scroll - (length + pos_y * 0.5) * pos->size, 
				scroll + target->pos[1] + 0.05, 
				scroll + target->pos[1]);

			if(0.02 * 0.02 > (input->pointer_x - a) * (input->pointer_x - a) + (input->pointer_y - b) * (input->pointer_y - b))
				glScalef(0.05 / pos->size, 0.05 / pos->size, 0.05 / pos->size);
			else
				glScalef(0.02 / pos->size, 0.02 / pos->size, 0.02 / pos->size);
			co_vng_ring();
			glPopMatrix();
		}
	}else
	{
		if(pos->expand)
		{
			if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
				if((input->pointer_x - (pos->pos[0] - pos_x * 0.15)) * 
					(input->pointer_x - (pos->pos[0] - pos_x * 0.15)) + 
					(input->pointer_y - (pos->pos[1] - length + scroll - pos_y * 0.15)) * 
					(input->pointer_y - (pos->pos[1] - length + scroll - pos_y * 0.15)) < 0.02 * 0.02)
					drag = link;
			if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE && drag == link)
			{
				*link = (VNMFragmentID)-1;
				for(id = e_nsm_get_fragment_next(node, 0); id != (VNMFragmentID)-1; id = e_nsm_get_fragment_next(node, id + 1))
					if((target = e_nsm_get_custom_data(node, id, CO_ENOUGH_NODE_SLOT)) != NULL)
						if(sui_box_click_test(target->pos[0] - 0.15 * target->size, target->pos[1] - 0.05 + scroll, 0.3 * target->size, 0.4 * target->size))
							*link = id;
				return TRUE;
			}
		}
		if(drag != link && input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE && move != -1)
		{
			if((input->pointer_x - (pos->pos[0] - pos_x * 0.15)) * 
				(input->pointer_x - (pos->pos[0] - pos_x * 0.15)) + 
				(input->pointer_y - (pos->pos[1] - length + scroll - pos_y * 0.15)) * 
				(input->pointer_y - (pos->pos[1] - length + scroll - pos_y * 0.15)) < 0.02 * 0.02)
			{
				*link = move;
				return TRUE;
			}
			if(frag != NULL && target != NULL)
			{
				a = compute_spline(0.5, 
					pos->pos[0] - (pos_x * 0.17) * pos->size, 
					pos->pos[0] - (pos_x * 0.5) * pos->size, 
					target->pos[0], 
					target->pos[0]);
				b = compute_spline(0.5, 
					pos->pos[1] + scroll - (length + pos_y * 0.17) * pos->size, 
					pos->pos[1] + scroll - (length + pos_y * 0.5) * pos->size, 
					scroll + target->pos[1] + 0.05, 
					scroll + target->pos[1]);
				if(0.02 * 0.02 > (input->pointer_x - a) * (input->pointer_x - a) + (input->pointer_y - b) * (input->pointer_y - b))
				{
					set_out_link(node, move, *link);
					*link = move;
					return TRUE;
				}
			}
		}
	}
	if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == FALSE)
		drag = NULL;
	return FALSE;
}

void co_draw_param_text(float x, float y, uint count, char **text, float color)
{
	uint i;
	for(i = 0; i < count; i++)
	{
		sui_draw_2d_line_gl(x - 0.17, y - 0.1 + SUI_T_SIZE - 0.05 * (float)i, 
						x - 0.27, y - 0.125 + SUI_T_SIZE - 0.1 * ((float)i - 0.25 * (float)count), color, color, color);
		sui_draw_text(x - 0.3 - sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, text[i]), y - 0.125 + SUI_T_SIZE - 0.1 * ((float)i - 0.25 * (float)count), SUI_T_SIZE, SUI_T_SPACE, text[i], color, color, color);  
	}
}

extern void render_material(ENode *node, VNMFragmentID id, uint size, uint line, float *buffer);

void material_func(ENode *node, VNMFragmentID frag, ECustomDataCommand command)
{
	COVNMaterial *data;
	if(command == E_CDC_CREATE)
	{
		float *buf;
		uint i;
		data = malloc(sizeof *data);
		data->pos[0] = 0.45 - 0.1 * (float)(frag % 10);
		data->pos[1] = -0.1 * (float)(frag % 10 + frag / 10);
		data->version = 56756;
		data->size = 1;
		data->recursive = FALSE;
		data->expand = FALSE;
		data->placed = FALSE;
		e_nsm_set_custom_data(node, frag, CO_ENOUGH_NODE_SLOT, data);
		glGenTextures(1, &data->texture_id);
		buf = malloc((sizeof *buf) * TEXTURE_RESOLUTION * TEXTURE_RESOLUTION * 3);
		for(i = 0; i < TEXTURE_RESOLUTION * TEXTURE_RESOLUTION * 3; i++)
			buf[i] = (float)(i % 5) / 4.0;
		glBindTexture(GL_TEXTURE_2D, data->texture_id);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_RESOLUTION, TEXTURE_RESOLUTION, 0, GL_RGB, GL_FLOAT, buf);
		free(buf);
	}
	if(command == E_CDC_DESTROY && (data = e_ns_get_custom_data(node, CO_ENOUGH_NODE_SLOT)) != NULL)
	{
		free(data);
		glDeleteTextures(1, &data->texture_id);
		e_nsm_set_custom_data(node, frag, CO_ENOUGH_NODE_SLOT, NULL);
	}
}

uint co_material_get_texture_id(ENode *node, VNMFragmentID id)
{
	COVNMaterial *mat;
	mat = e_nsm_get_custom_data(node, id, CO_ENOUGH_NODE_SLOT);	
	return mat->texture_id;
}

boolean *co_material_get_recursive(ENode *node, VNMFragmentID id)
{
	COVNMaterial *mat;
	mat = e_nsm_get_custom_data(node, id, CO_ENOUGH_NODE_SLOT);	
	return &mat->recursive;
}

uint co_material_get_version(ENode *node, VNMFragmentID id)
{
	COVNMaterial *mat;
	mat = e_nsm_get_custom_data(node, id, CO_ENOUGH_NODE_SLOT);	
	return mat->version;
}

void co_material_set_version(ENode *node, VNMFragmentID id, uint version)
{
	COVNMaterial *mat;
	mat = e_nsm_get_custom_data(node, id, CO_ENOUGH_NODE_SLOT);	
	mat->version = version;
}

void co_draw_material_texture(COVNMaterial *mat, float x, float y)
{
	static float vertex[65 * 2], uv[65 * 2];
	static uint	ref[64 * 3] = {-1};
	if(ref[0] == -1)
	{
		float a, b;
		uint i;
		for(i = 0; i < 64; i++)
		{	
			a = sin((float)i / 64.0 * PI * 2.0);
			b = cos((float)i / 64.0 * PI * 2.0);
			sui_draw_set_vec2(uv, i, a * 0.5 + 0.5, b * 0.5 + 0.5);
			sui_draw_set_vec2(vertex, i, 0.14 * b, 0.14 * a);
		}
		sui_draw_set_vec2(uv, i, 0.5, 0.5);
		sui_draw_set_vec2(vertex, i, 0, 0);
		for(i = 0; i < 64; i++)
			sui_draw_set_ivec3(ref, i, i, (i + 1) % 64, 64);
	}
	glPushMatrix();
	glTranslatef(x, y, 0);
	sui_set_texture2D_array_gl(uv, 65, 2, mat->texture_id);
//	sui_draw_gl(GL_TRIANGLES, vertex, 65, 2, 0, 0, 0);
	sui_draw_ellements_gl(GL_TRIANGLES, vertex, ref, 64 * 3, 2, 1, 1, 1);
	glPopMatrix();
}


void co_draw_material(ENode *node)
{
	COVNMaterial *mat;
	VNMFragmentID id;
	id = e_nsm_get_fragment_color_front(node);
	if(id == -1)
		id = e_nsm_get_fragment_color_back(node);
	if(e_nsm_get_fragment(node, id) != NULL)
	{
		mat = e_nsm_get_custom_data(node, id, CO_ENOUGH_NODE_SLOT);
		glPushMatrix();
		glScalef(4 / 1.425, 4 / 1.425, 4 / 1.425);
		co_draw_material_texture(mat, 0, 0);
		glPopMatrix();
	}
}
	
boolean co_material_click_test(BInputState *input, COVNMaterial *mat, VNMFragmentID id, float pos_x, float pos_y, float length, VNMFragmentID *move)
{
	static float delta_x, delta_y;

	if(input->mode != BAM_EVENT)
		return mat->expand;
	
	if(mat->expand)
	{
		mat->size = 0.1 + mat->size * 0.9;
		if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
		{
			if(sui_box_click_test(pos_x - 0.15, pos_y - 0.025, 0.3, 0.045))
			{
				*move = id;
				delta_x = mat->pos[0] - input->pointer_x;
				delta_y = mat->pos[1] - input->pointer_y;
			}
			if(0.15 * 0.15 > (pos_x - input->click_pointer_x) * (pos_x - input->click_pointer_x) + (pos_y - input->click_pointer_y - length) * (pos_y - input->click_pointer_y - length))
			{
				*move = id;
				delta_x = mat->pos[0] - input->pointer_x;
				delta_y = mat->pos[1] - input->pointer_y;
			}
		}
		if(*move == id && input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE)
			if(0.15 * 0.15 > (pos_x - input->click_pointer_x) * (pos_x - input->click_pointer_x) + (pos_y - input->click_pointer_y - length) * (pos_y - input->click_pointer_y - length))
				if(0.005 * 0.005 > (input->pointer_x - input->click_pointer_x) * (input->pointer_x - input->click_pointer_x) + (input->pointer_y - input->click_pointer_y) * (input->pointer_y - input->click_pointer_y))
					mat->expand = FALSE;
	}
	else
	{
		if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
		{
			if(sui_box_click_test(pos_x - 0.15 * mat->size, pos_y - 0.335 * mat->size, 0.3 * mat->size, 0.355 * mat->size))
			{
				*move = id;
				delta_x = mat->pos[0] - input->pointer_x;
				delta_y = mat->pos[1] - input->pointer_y;
			}
		}
		if(*move == id && input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE)
			if(0.005 * 0.005 > (input->pointer_x - input->click_pointer_x) * (input->pointer_x - input->click_pointer_x) + (input->pointer_y - input->click_pointer_y) * (input->pointer_y - input->click_pointer_y))
				mat->expand = TRUE;
		mat->size = 0.002 + mat->size * 0.98;
	}
	
	if(*move == id)
	{
		mat->pos[0] = input->pointer_x + delta_x;
		mat->pos[1] = input->pointer_y + delta_y;
	}
	if(input->last_mouse_button[0] == FALSE && input->mouse_button[0] == FALSE)
		*move = -1;

	return mat->expand;
}
	
void co_draw_lable(COVNMaterial *mat, float y, float color, char *lable)
{
	glPushMatrix();
	glTranslatef(mat->pos[0], mat->pos[1] + y, 0);
	glScalef(1 / mat->size, 1 / mat->size, 1);
	sui_draw_2d_line_gl(mat->size * 0.2, -0.015, mat->size * 0.2 + 0.04, 0, color, color, color);
	sui_draw_text(mat->size * 0.2 + 0.04, 0, SUI_T_SIZE, SUI_T_SPACE, lable, color, color, color);  		
	glPopMatrix();
}

extern float co_handle_node_head(BInputState *input, ENode *node);

boolean co_handle_material(BInputState *input, ENode *node)
{
	static char *material_type_names[] = {"COLOR", "LIGHT", "REFLECTION", "TRANSPARENCY", "VOLUME", "GEOMETRY", "TEXTURE",  "NOISE", "BLENDER", "MATRIX", "RAMP", "ANIMATION", "ALTERNATIVE", "OUTPUT"};
	static boolean show_tree = TRUE;
	static VNMFragmentID move = -1;
	static float rot_tree = 1;
	COVNMaterial *mat_pos;
	float expand, place[3] = {0, 0, 0};
	uint type = 0, j, z_sort;
	VNMFragmentID i, link;
	boolean output;
	VMatFrag *frag, f;
	float y, pre_expander, color, color_light;

	y = co_handle_node_head(input, node);
	change_m_node_id = e_ns_get_node_id(node);

	co_vng_divider(input, 0.2, y, &rot_tree, &color, &color_light, &show_tree, "LIGHT");
	pre_expander = y;

	if(rot_tree > 0.001)
	{
		static boolean create = FALSE;
		co_place_all_fragments(node);

		for(i = e_nsm_get_fragment_next(node, 0); i != (VNMFragmentID)-1; i = e_nsm_get_fragment_next(node, i + 1))
		{
			mat_pos = e_nsm_get_custom_data(node, i, CO_ENOUGH_NODE_SLOT);
			if(mat_pos != NULL)
			{
				frag = e_nsm_get_fragment(node, i);
				type = e_nsm_get_fragment_type(node, i);
				f = *frag;
				if(input->mode == BAM_DRAW)
				{
					glPushMatrix();
					glTranslatef(mat_pos->pos[0], mat_pos->pos[1] + y, 0);
					glScalef(mat_pos->size, mat_pos->size, mat_pos->size);
					glTranslatef(-mat_pos->pos[0], -mat_pos->pos[1] - y, 0);
				}

			/*	co_draw_material_texture(mat_pos, mat_pos->pos[0], mat_pos->pos[1]);*/
				switch(type)
				{	
					case VN_M_FT_COLOR :
					{
						char *text[3] = {"Red", "Green", "Blue"};
						expand = 0.2 + 0.15 * mat_pos->size;
						place[0] = mat_pos->pos[0];
						place[1] = mat_pos->pos[1] + y;
						co_vng_fragment(place[0], place[1], expand, color);
						co_draw_material_texture(mat_pos, place[0] + (-1 + rot_tree * rot_tree) * 3 / mat_pos->size, place[1] - expand);
						if(co_material_click_test(input, mat_pos, i, place[0], place[1], expand, &move))
						{
							co_draw_param_text(place[0], place[1], 3, text, color_light);

							if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.1, 0.12, SUI_T_SIZE, &f.color.red, &frag->color.red, color, color, color))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_COLOR, &f);
							if(co_w_slider(input, place[0], place[1] - 0.1, 0.12, &frag->color.red, color, color, color))
							{
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_COLOR, frag);
								frag->color.red = f.color.red;
							}

							if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.15, 0.12, SUI_T_SIZE, &f.color.green, &frag->color.green, color, color, color))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_COLOR, &f);
							if(co_w_slider(input, place[0], place[1] - 0.15, 0.12, &frag->color.green, color, color, color))
							{
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_COLOR, frag);
								frag->color.green = f.color.green;
							}

							if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.2, 0.12, SUI_T_SIZE, &f.color.blue, &frag->color.blue, color, color, color))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_COLOR, &f);
							if(co_w_slider(input, place[0], place[1] - 0.2, 0.12, &frag->color.blue, color, color, color))
							{
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_COLOR, frag);
								frag->color.blue = f.color.blue;
							}
						}else if(input->mode == BAM_DRAW)
							co_draw_lable(mat_pos, y, color_light, "Color");
					}
					break;
					case VN_M_FT_LIGHT :
					{
						static uint active = FALSE;
						char *text[6] = {"Type", "Normal Falloff", "BRDF", "Red", "Green", "Blue"};
						char *light_types[] = {"DIRECT", "AMBIENT", "D_AND_A", "BACK_DIRECT", "BACK_AMBIENT", "BACK_D_AND_A"};
						char *light_labels[] = {"Direct Light", "Ambient Light", "Direct and Ambient Light", "Back Direct light", "Back Ambient Light", "Back Direct and Ambient Light"};
						ENode *b_node;
						expand = 0.2 + 0.3 * mat_pos->size;
						place[0] = mat_pos->pos[0];
						place[1] = mat_pos->pos[1] + y;
						co_vng_fragment(place[0], place[1], expand, color);
						co_draw_material_texture(mat_pos, place[0] + (-1 + rot_tree * rot_tree) * 3 / mat_pos->size, place[1] - expand);
						if(co_material_click_test(input, mat_pos, i, place[0], place[1], expand, &move))
						{
							co_draw_param_text(place[0], place[1], 6, text, color_light);
							if(sw_text_button(input, place[0] - 0.12, place[1] - 0.1, 0, SUI_T_SIZE, SUI_T_SPACE, light_types[frag->light.type], color, color, color))
								active = i;
							if(active == i)
							{
								SUIPUElement e[6];
								uint output;
								for(j = 0; j < 6; j++)
								{
									e[j].text = light_types[j];
									e[j].type = PU_T_SQUARE;
									e[j].data.square.draw_func = NULL;
									e[j].data.square.square[0] = 0.3 + (float)((j) / 3) * 0.25;
									e[j].data.square.square[1] = 0.15 * ((float)((j) % 3) - 0.66666);
									e[j].data.square.square[2] = 0.2;
									e[j].data.square.square[3] = -0.1;
								}
								output = sui_draw_popup(input, place[0] - 0.12, place[1] - 0.1, e, 6, 0);
								if(output < 6 && output != frag->light.type)
								{
									f.light.type = output;
									verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_LIGHT, &f);
								}
							}
							if(input->last_mouse_button[0] == FALSE && input->mouse_button[0] == FALSE)
								active = -1;
		
							if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.15, 0.12, SUI_T_SIZE, &f.light.normal_falloff, &frag->light.normal_falloff, color, color, color))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_LIGHT, &f);
							if(co_w_slider(input, place[0], place[1] - 0.15, 0.12, &frag->light.normal_falloff, color, color, color))
							{
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_LIGHT, frag);
								frag->light.normal_falloff = f.light.normal_falloff;
							}

							b_node = e_ns_get_node(0, f.light.brdf);
							if(b_node != NULL)
								sui_draw_text(place[0] - 0.12, place[1] - 0.2, SUI_T_SIZE, SUI_T_SPACE, e_ns_get_node_name(b_node), color_light, color_light, color_light);  
							else
								sui_draw_text(place[0] - 0.12, place[1] - 0.2, SUI_T_SIZE, SUI_T_SPACE, "NONE", color_light, color_light, color_light);  

							co_w_type_in(input, place[0] - 0.12, place[1] - 0.25, 0.24, SUI_T_SIZE, f.light.brdf_r, 16, rename_m_fragment, frag->light.brdf_r, color, color_light);
							co_w_type_in(input, place[0] - 0.12, place[1] - 0.3, 0.24, SUI_T_SIZE, f.light.brdf_g, 16, rename_m_fragment, frag->light.brdf_g, color, color_light);
							co_w_type_in(input, place[0] - 0.12, place[1] - 0.35, 0.24, SUI_T_SIZE, f.light.brdf_b, 16, rename_m_fragment, frag->light.brdf_b, color, color_light);
						}else if(input->mode == BAM_DRAW)
							co_draw_lable(mat_pos, y, color_light, light_labels[frag->light.type]);
					}
					break;
					case VN_M_FT_REFLECTION :
					{
						static uint active = FALSE;
						char *text[1] = {"Normal Falloff"};
						expand = 0.2 + 0.05 * mat_pos->size;
						place[0] = mat_pos->pos[0];
						place[1] = mat_pos->pos[1] + y;
						co_vng_fragment(place[0], place[1], expand, color);
						co_draw_material_texture(mat_pos, place[0] + (-1 + rot_tree * rot_tree) * 3 / mat_pos->size, place[1] - expand);
						if(co_material_click_test(input, mat_pos, i, place[0], place[1], expand, &move))
						{
							co_draw_param_text(place[0], place[1], 1, text, color_light);
							if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.1, 0.12, SUI_T_SIZE, &f.reflection.normal_falloff, &frag->reflection.normal_falloff, color, color, color))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_REFLECTION, &f);
							if(co_w_slider(input, place[0], place[1] - 0.1, 0.12, &frag->reflection.normal_falloff, color, color, color))
							{
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_REFLECTION, frag);
								frag->reflection.normal_falloff = f.reflection.normal_falloff;
							}
						}if(input->mode == BAM_DRAW)
							co_draw_lable(mat_pos, y, color_light, "Reflection");
					}
					break;
					case VN_M_FT_TRANSPARENCY :
					{
						char *text[2] = {"Normal Falloff", "Refraction index"};
						expand = 0.2 + 0.1 * mat_pos->size;
						place[0] = mat_pos->pos[0];
						place[1] = mat_pos->pos[1] + y;
						co_vng_fragment(place[0], place[1], expand, color);
						co_draw_material_texture(mat_pos, place[0] + (-1 + rot_tree * rot_tree) * 3 / mat_pos->size, place[1] - expand);
						if(co_material_click_test(input, mat_pos, i, place[0], place[1], expand, &move))
						{
							co_draw_param_text(place[0], place[1], 2, text, color_light);
							if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.1, 0.12, SUI_T_SIZE, &f.transparency.normal_falloff, &frag->transparency.normal_falloff, color, color, color))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_TRANSPARENCY, &f);
							if(co_w_slider(input, place[0], place[1] - 0.1, 0.12, &frag->transparency.normal_falloff, color, color, color))
							{
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_TRANSPARENCY, frag);
								frag->transparency.normal_falloff = f.transparency.normal_falloff;
							}
							if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.15, 0.12, SUI_T_SIZE, &f.transparency.refraction_index, &frag->transparency.refraction_index, color, color, color))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_TRANSPARENCY, &f);
							if(co_w_slider(input, place[0], place[1] - 0.15, 0.12, &frag->transparency.refraction_index, color, color, color))
							{
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_TRANSPARENCY, frag);
								frag->transparency.refraction_index = f.transparency.refraction_index;
							}
						}else if(input->mode == BAM_DRAW)
							co_draw_lable(mat_pos, y, color_light, "Transparancy");
					}
					break;
					case VN_M_FT_VOLUME :
					{
						char *text[4] = {"Diffucion", "Red", "Green", "Blue"};
						expand = 0.2 + 0.2 * mat_pos->size;
						place[0] = mat_pos->pos[0];
						place[1] = mat_pos->pos[1] + y;
						co_vng_fragment(place[0], place[1], expand, color);
						co_draw_material_texture(mat_pos, place[0] + (-1 + rot_tree * rot_tree) * 3 / mat_pos->size, place[1] - expand);
						if(co_material_click_test(input, mat_pos, i, place[0], place[1], expand, &move))
						{
							co_draw_param_text(place[0], place[1], 4, text, color_light);
							if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.1, 0.12, SUI_T_SIZE, &f.volume.diffusion, &frag->volume.diffusion, color, color, color))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_VOLUME, &f);
							if(co_w_slider(input, place[0], place[1] - 0.1, 0.12, &frag->volume.diffusion, color, color, color))
							{
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_VOLUME, frag);
								frag->volume.diffusion = f.volume.diffusion;
							}
							if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.15, 0.12, SUI_T_SIZE, &f.volume.col_r, &frag->volume.col_r, color, color, color))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_VOLUME, &f);
							if(co_w_slider(input, place[0], place[1] - 0.15, 0.12, &frag->volume.col_r, color, color, color))
							{
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_VOLUME, frag);
								frag->volume.col_r = f.volume.col_r;
							}
							if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.2, 0.12, SUI_T_SIZE, &f.volume.col_g, &frag->volume.col_g, color, color, color))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_VOLUME, &f);
							if(co_w_slider(input, place[0], place[1] - 0.2, 0.12, &frag->volume.col_g, color, color, color))
							{
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_VOLUME, frag);
								frag->volume.col_g = f.volume.col_g;
							}
							if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.25, 0.12, SUI_T_SIZE, &f.volume.col_b, &frag->volume.col_b, color, color, color))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_VOLUME, &f);
							if(co_w_slider(input, place[0], place[1] - 0.25, 0.12, &frag->volume.col_b, color, color, color))
							{
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_VOLUME, frag);
								frag->volume.col_b = f.volume.col_b;
							}
						}if(input->mode == BAM_DRAW)
							co_draw_lable(mat_pos, y, color_light, "Volume");
						if(handle_link(input, node, &frag->volume.color, "Color", i, 0, y, expand, color, move))
						{
							verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_VOLUME, frag);
							*frag = f;
						}
					}
					break;
					case VN_M_FT_GEOMETRY :
					{
						char *text[3] = {"Red", "Green", "Blue"};
						expand = 0.2 + 0.15 * mat_pos->size;
						place[0] = mat_pos->pos[0];
						place[1] = mat_pos->pos[1] + y;
						co_vng_fragment(place[0], place[1], expand, color);
						co_draw_material_texture(mat_pos, place[0] + (-1 + rot_tree * rot_tree) * 3 / mat_pos->size, place[1] - expand);
						if(co_material_click_test(input, mat_pos, i, place[0], place[1], expand, &move))
						{
							co_draw_param_text(place[0], place[1], 3, text, color_light);
							co_w_type_in(input, place[0] - 0.12, place[1] - 0.1, 0.24, SUI_T_SIZE, f.geometry.layer_r, 16, rename_m_fragment, frag->geometry.layer_r, color, color_light);
							co_w_type_in(input, place[0] - 0.12, place[1] - 0.15, 0.24, SUI_T_SIZE, f.geometry.layer_g, 16, rename_m_fragment, frag->geometry.layer_g, color, color_light);
							co_w_type_in(input, place[0] - 0.12, place[1] - 0.2, 0.24, SUI_T_SIZE, f.geometry.layer_b, 16, rename_m_fragment, frag->geometry.layer_b, color, color_light);
						}else if(input->mode == BAM_DRAW)
							co_draw_lable(mat_pos, y, color_light, "Geometry");
					}
					break;
					case VN_M_FT_TEXTURE :
					{
						char *text[4] = {"Texture", "Red", "Green", "Blue"};
						ENode *b_node;
						expand = 0.2 + 0.2 * mat_pos->size;
						place[0] = mat_pos->pos[0];
						place[1] = mat_pos->pos[1] + y;
						co_vng_fragment(place[0], place[1], expand, color);
						co_draw_material_texture(mat_pos, place[0] + (-1 + rot_tree * rot_tree) * 3 / mat_pos->size, place[1] - expand);
						if(co_material_click_test(input, mat_pos, i, place[0], place[1], expand, &move))
						{
							co_draw_param_text(place[0], place[1], 4, text, color_light);
							b_node = e_ns_get_node(0, f.texture.bitmap);
							if(b_node != NULL)
								sui_draw_text(place[0] - 0.12, place[1] - 0.1, SUI_T_SIZE, SUI_T_SPACE, e_ns_get_node_name(b_node), color_light, color_light, color_light);  
							else
								sui_draw_text(place[0] - 0.12, place[1] - 0.1, SUI_T_SIZE, SUI_T_SPACE, "NONE", color_light, color_light, color_light);  
							co_w_type_in(input, place[0] - 0.12, place[1] - 0.15, 0.24, SUI_T_SIZE, f.texture.layer_r, 16, rename_m_fragment, frag->texture.layer_r, color, color_light);
							co_w_type_in(input, place[0] - 0.12, place[1] - 0.2, 0.24, SUI_T_SIZE, f.texture.layer_g, 16, rename_m_fragment, frag->texture.layer_g, color, color_light);
							co_w_type_in(input, place[0] - 0.12, place[1] - 0.25, 0.24, SUI_T_SIZE, f.texture.layer_b, 16, rename_m_fragment, frag->texture.layer_b, color, color_light);
						}else if(input->mode == BAM_DRAW)
							co_draw_lable(mat_pos, y, color_light, "Texture");
						if(handle_link(input, node, &frag->texture.mapping, "Mapping", i, 0, y, expand, color, move))
							verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_TEXTURE, frag);
						*frag = f;
					}
					break;
					case VN_M_FT_NOISE :
						{	
							static uint active = FALSE;
							char *text[1] = {"Type"};
							char *noise_types[] = {"PERLIN 0 - 1", "PERLIN -1 - 1"};
							char *noise_labels[] = {"Perlin 0 - 1 Noice", "Perlin -1 - 1 Noice"};
							expand = 0.2 + 0.05 * mat_pos->size;
							place[0] = mat_pos->pos[0];
							place[1] = mat_pos->pos[1] + y;
							co_vng_fragment(place[0], place[1], expand, color);
							co_draw_material_texture(mat_pos, place[0] + (-1 + rot_tree * rot_tree) * 3 / mat_pos->size, place[1] - expand);
							if(co_material_click_test(input, mat_pos, i, place[0], place[1], expand, &move))
							{
								co_draw_param_text(place[0], place[1], 1, text, color_light);
								if(sw_text_button(input, place[0] - 0.12, place[1] - 0.1, 0, SUI_T_SIZE, SUI_T_SPACE, noise_types[frag->noise.type], color, color, color))
									active = i;
								if(active == i)
								{
									SUIPUElement e[2];
									uint output;
									for(j = 0; j < 2; j++)
									{
										e[j].text = noise_types[j];
										e[j].type = PU_T_SQUARE;
										e[j].data.square.draw_func = NULL;
										e[j].data.square.square[0] = 0.3;
										e[j].data.square.square[1] = 0.15 * (float)j;
										e[j].data.square.square[2] = 0.2;
										e[j].data.square.square[3] = -0.1;
									}
									output = sui_draw_popup(input, place[0] - 0.12, place[1] - 0.1, e, 2, 0);
									if(output < 2 && output != frag->noise.type)
									{
										frag->noise.type = output;
										verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_NOISE, &f);
									}
								}
								if(input->last_mouse_button[0] == FALSE && input->mouse_button[0] == FALSE)
									active = -1;
							}else if(input->mode == BAM_DRAW)
								co_draw_lable(mat_pos, y, color_light, noise_labels[frag->noise.type]);
							if(handle_link(input, node, &frag->noise.mapping, "B", i, 0, y, expand, color, move))
							{
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_NOISE, frag);
								*frag = f;
							}
						}
						break;
					case VN_M_FT_BLENDER :
						{	
							static uint active = FALSE;
							char *text[1] = {"Type"};
							char *blend_types[] = {"FADE", "ADD", "SUBTRACT", "MULTIPLY", "DIVIDE", "DOT"};
							char *blender_labels[] = {"Blend Fade", "Blend Add", "Blend Subtract", "Blend Multiply", "Blend Divide", "Blend Dot"};
							expand = 0.2 + 0.05 * mat_pos->size;
							place[0] = mat_pos->pos[0];
							place[1] = mat_pos->pos[1] + y;
							co_vng_fragment(place[0], place[1], expand, color);
							co_draw_material_texture(mat_pos, place[0] + (-1 + rot_tree * rot_tree) * 3 / mat_pos->size, place[1] - expand);
							if(co_material_click_test(input, mat_pos, i, place[0], place[1], expand, &move))
							{
								co_draw_param_text(place[0], place[1], 1, text, color_light);
								if(sw_text_button(input, place[0] - 0.12, place[1] - 0.1, 0, SUI_T_SIZE, SUI_T_SPACE, blend_types[frag->blender.type], color, color, color))
									active = i;
								if(active == i)
								{
									SUIPUElement e[6];
									uint output;
									for(j = 0; j < 6; j++)
									{
										e[j].text = blend_types[j];
										e[j].type = PU_T_SQUARE;
										e[j].data.square.draw_func = NULL;
										e[j].data.square.square[0] = 0.3 + (float)((j) / 3) * 0.25;
										e[j].data.square.square[1] = 0.15 * ((float)((j) % 3) - 0.66666);
										e[j].data.square.square[2] = 0.2;
										e[j].data.square.square[3] = -0.1;
									}
									output = sui_draw_popup(input, place[0] - 0.12, place[1] - 0.1, e, 6, 0);
									if(output < 6 && output != frag->blender.type)
									{
										f.blender.type = output;
										verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_BLENDER, &f);
									}
								}
								if(input->last_mouse_button[0] == FALSE && input->mouse_button[0] == FALSE)
									active = -1;
							}else if(input->mode == BAM_DRAW)
								co_draw_lable(mat_pos, y, color_light, blender_labels[frag->blender.type]);
							if(handle_link(input, node, &frag->blender.data_a, "A", i, -30, y, expand, color, move))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_BLENDER, frag);
							if(handle_link(input, node, &frag->blender.data_b, "B", i, 0, y, expand, color, move))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_BLENDER, frag);
							if(handle_link(input, node, &frag->blender.control, "Control", i, 30, y, expand, color, move))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_BLENDER, frag);
							*frag = f;
						}
						break;
					case VN_M_FT_MATRIX :
						{	
							static uint mode = 2, active = -1;
							float size = 0.3, reset[] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
							char *matrix_types[] = {"Matrix", "Balance", "Color Balance"};
							place[0] = mat_pos->pos[0];
							place[1] = mat_pos->pos[1] + y;


							if(mode == 0)
								expand = 0.2 + 0.3 * mat_pos->size;
							if(mode == 1)
								expand = 0.2 + 0.2 * mat_pos->size;
							if(mode == 2)
								expand = 0.2 + 0.25 * mat_pos->size;

							if(co_material_click_test(input, mat_pos, i, place[0], place[1], expand, &move))
							{
								if(sw_text_button(input, place[0] - 0.12, place[1] - 0.1, 0, SUI_T_SIZE, SUI_T_SPACE, matrix_types[mode], color, color, color))
									active = i;
								if(active == i)
								{
									SUIPUElement e[3];
									uint output;
									for(j = 0; j < 3; j++)
									{
										e[j].text = matrix_types[j];
										e[j].type = PU_T_SQUARE;
										e[j].data.square.draw_func = NULL;
										e[j].data.square.square[0] = 0.3 + (float)((j) / 3) * 0.25;
										e[j].data.square.square[1] = 0.15 * ((float)((j) % 3) - 0.66666);
										e[j].data.square.square[2] = 0.2;
										e[j].data.square.square[3] = -0.1;
									}
									output = sui_draw_popup(input, place[0] - 0.12, place[1] - 0.1, e, 3, 0);
									if(output < 3)
										mode = output;
								}
								if(input->last_mouse_button[0] == FALSE && input->mouse_button[0] == FALSE)
									active = -1;
								

								if(sw_text_button(input, place[0] - 0.12, place[1] - 0.15, 0, SUI_T_SIZE, SUI_T_SPACE, "Ident", color, color, color))
								{
									for(j = 0; j < 16;  j++)
										f.matrix.matrix[j] = reset[j];
									verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
								}

								switch(mode)
								{
									case 0 :
									{
										char *text[] = {"Type", "Reset", "Matrix"};
										co_draw_param_text(place[0], place[1], 3, text, color_light);
										if(sui_type_number_double(input, place[0] - 0.13, place[1] - 0.2, 0.12, SUI_T_SIZE, &f.matrix.matrix[0], &frag->matrix.matrix[0], color, color, color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										if(sui_type_number_double(input, place[0] - 0.06, place[1] - 0.2, 0.12, SUI_T_SIZE, &f.matrix.matrix[1], &frag->matrix.matrix[1], color, color, color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										if(sui_type_number_double(input, place[0] + 0.01, place[1] - 0.2, 0.12, SUI_T_SIZE, &f.matrix.matrix[2], &frag->matrix.matrix[2], color, color, color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										if(sui_type_number_double(input, place[0] + 0.08, place[1] - 0.2, 0.12, SUI_T_SIZE, &f.matrix.matrix[3], &frag->matrix.matrix[3], color, color, color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);

										if(sui_type_number_double(input, place[0] - 0.13, place[1] - 0.25, 0.12, SUI_T_SIZE, &f.matrix.matrix[4], &frag->matrix.matrix[4], color, color, color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										if(sui_type_number_double(input, place[0] - 0.06, place[1] - 0.25, 0.12, SUI_T_SIZE, &f.matrix.matrix[5], &frag->matrix.matrix[5], color, color, color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										if(sui_type_number_double(input, place[0] + 0.01, place[1] - 0.25, 0.12, SUI_T_SIZE, &f.matrix.matrix[6], &frag->matrix.matrix[6], color, color, color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										if(sui_type_number_double(input, place[0] + 0.08, place[1] - 0.25, 0.12, SUI_T_SIZE, &f.matrix.matrix[7], &frag->matrix.matrix[7], color, color, color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);

										if(sui_type_number_double(input, place[0] - 0.13, place[1] - 0.3, 0.12, SUI_T_SIZE, &f.matrix.matrix[8], &frag->matrix.matrix[8], color, color, color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										if(sui_type_number_double(input, place[0] - 0.06, place[1] - 0.3, 0.12, SUI_T_SIZE, &f.matrix.matrix[9], &frag->matrix.matrix[9], color, color, color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										if(sui_type_number_double(input, place[0] + 0.01, place[1] - 0.3, 0.12, SUI_T_SIZE, &f.matrix.matrix[10], &frag->matrix.matrix[10], color, color, color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										if(sui_type_number_double(input, place[0] + 0.08, place[1] - 0.3, 0.12, SUI_T_SIZE, &f.matrix.matrix[11], &frag->matrix.matrix[11], color, color, color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);

										if(sui_type_number_double(input, place[0] - 0.13, place[1] - 0.35, 0.12, SUI_T_SIZE, &f.matrix.matrix[12], &frag->matrix.matrix[12], color, color, color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										if(sui_type_number_double(input, place[0] - 0.06, place[1] - 0.35, 0.12, SUI_T_SIZE, &f.matrix.matrix[13], &frag->matrix.matrix[13], color, color, color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										if(sui_type_number_double(input, place[0] + 0.01, place[1] - 0.35, 0.12, SUI_T_SIZE, &f.matrix.matrix[14], &frag->matrix.matrix[14], color, color, color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										if(sui_type_number_double(input, place[0] + 0.08, place[1] - 0.35, 0.12, SUI_T_SIZE, &f.matrix.matrix[15], &frag->matrix.matrix[15], color, color, color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
									}
									break;
									case 1 :
									{
										char *text[] = {"Type", "Reset", "Contrast", "Brightness"};
										double pre, value;
										co_draw_param_text(place[0], place[1], 4, text, color_light);
										value = pre = (f.matrix.matrix[0] + f.matrix.matrix[5] + f.matrix.matrix[10]) / 3;
										if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.2, 0.12, SUI_T_SIZE, &value, &frag->matrix.matrix[0], color, color, color))
										{
											f.matrix.matrix[0] *= value / pre;
											f.matrix.matrix[5] *= value / pre;
											f.matrix.matrix[10] *= value / pre;
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										}
										frag->matrix.matrix[1] = value / 3;
										if(co_w_slider(input, place[0], place[1] - 0.2, 0.12, &frag->matrix.matrix[1], color, color, color))
										{
											frag->matrix.matrix[0] *= frag->matrix.matrix[1] / pre * 3;
											frag->matrix.matrix[5] *= frag->matrix.matrix[1] / pre * 3;
											frag->matrix.matrix[10] *= frag->matrix.matrix[1] / pre * 3;
											frag->matrix.matrix[1] = f.matrix.matrix[1];
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, frag);
										}
									}
									break;
									case 2 :
									{
										char *text[] = {"Type", "Reset", "Red", "Green", "Blue"};
										co_draw_param_text(place[0], place[1], 5, text, color_light);

										if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.2, 0.12, SUI_T_SIZE, &f.matrix.matrix[0], &frag->matrix.matrix[0], color, color, color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										frag->matrix.matrix[0] *= 0.25;
										if(co_w_slider(input, place[0], place[1] - 0.2, 0.12, &frag->matrix.matrix[0], color, color, color))
										{
											frag->matrix.matrix[0] /= 0.25;
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, frag);
											frag->matrix.matrix[0] = f.matrix.matrix[0];
										}

										if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.25, 0.12, SUI_T_SIZE, &f.matrix.matrix[5], &frag->matrix.matrix[5], color, color, color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										frag->matrix.matrix[5] *= 0.25;
										if(co_w_slider(input, place[0], place[1] - 0.25, 0.12, &frag->matrix.matrix[5], color, color, color))
										{
											frag->matrix.matrix[5] /= 0.25;
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, frag);
											frag->matrix.matrix[5] = f.matrix.matrix[5];
										}

										if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.3, 0.12, SUI_T_SIZE, &f.matrix.matrix[10], &frag->matrix.matrix[10], color, color, color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										frag->matrix.matrix[10] *= 0.25;
										if(co_w_slider(input, place[0], place[1] - 0.3, 0.12, &frag->matrix.matrix[10], color, color, color))
										{
											frag->matrix.matrix[10] /= 0.25;
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, frag);
											frag->matrix.matrix[10] = f.matrix.matrix[10];
										}
									}
									break;
								}
							}else if(input->mode == BAM_DRAW)
								co_draw_lable(mat_pos, y, color_light, "Matrix");
							co_vng_fragment(place[0] - 0.015, place[1] - 0.015, expand, color);
							co_draw_material_texture(mat_pos, place[0] + (-1 + rot_tree * rot_tree) * 3 / mat_pos->size, place[1] - 0.015 - expand);
							if(handle_link(input, node, &frag->matrix.data, "Input", i, 0, y, expand, color, move))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, frag);
							*frag = f;
						}
						break;
					case VN_M_FT_RAMP :
						{
							static uint active_type = -1, active_channel = -1;
							static double *active = NULL, new_color[3];
							static uint segment = 0, drag = -1, drag_frag = -1;
							static char *ramp_types[] = {"SQUARE", "LINEAR", "SMOOTH"};
							static char *ramp_channels[] = {"RED", "GREEN", "BLUE"};
							char *text[] = {"Channel", "Type", "Delete", "Ramp", "Red", "Green", "Blue"};
							double col[3], start, end, last;
							float col_range[12], vertex[8];
							uint mode;
							expand = 0.2 + 0.35 * mat_pos->size;
							place[0] = mat_pos->pos[0];
							place[1] = mat_pos->pos[1] + y;
							co_vng_fragment(place[0], place[1], expand, color);
							co_draw_material_texture(mat_pos, place[0] + (-1 + rot_tree * rot_tree) * 3 / mat_pos->size, place[1] - expand);
							f = *frag;
							if(co_material_click_test(input, mat_pos, i, place[0], place[1], expand, &move))
							{
								co_draw_param_text(place[0], place[1], 7, text, color_light);
								if(sw_text_button(input, place[0] - 0.12, place[1] - 0.1, 0, SUI_T_SIZE, SUI_T_SPACE, ramp_channels[frag->ramp.channel], color, color, color))
									active_channel = i;
								if(active_channel == i)
								{
									SUIPUElement e[3];
									uint output;
									for(j = 0; j < 3; j++)
									{
										e[j].text = ramp_channels[j];
										e[j].type = PU_T_SQUARE;
										e[j].data.square.draw_func = NULL;
										e[j].data.square.square[0] = 0.3;
										e[j].data.square.square[1] = 0.15 * ((float)((j) % 3) - 0.66666);
										e[j].data.square.square[2] = 0.2;
										e[j].data.square.square[3] = -0.1;
									}
									output = sui_draw_popup(input, place[0] - 0.12, place[1] - 0.1, e, 3, 0);
									f.ramp.channel = output; 
									if(output < 3 && output != frag->ramp.channel)
										verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_RAMP, &f);
								}
								if(input->last_mouse_button[0] == FALSE && input->mouse_button[0] == FALSE)
									active_channel = -1;
								
								if(sw_text_button(input, place[0] - 0.12, place[1] - 0.15, 0, SUI_T_SIZE, SUI_T_SPACE, ramp_types[frag->ramp.type], color, color, color))
									active_type = i;
								if(active_type == i)
								{
									SUIPUElement e[3];
									uint output;
									for(j = 0; j < 3; j++)
									{
										e[j].text = ramp_types[j];
										e[j].type = PU_T_SQUARE;
										e[j].data.square.draw_func = NULL;
										e[j].data.square.square[0] = 0.3;
										e[j].data.square.square[1] = 0.15 * ((float)((j) % 3) - 0.66666);
										e[j].data.square.square[2] = 0.2;
										e[j].data.square.square[3] = -0.1;
									}
									output = sui_draw_popup(input, place[0] - 0.12, place[1] - 0.15, e, 3, 0);
									f.ramp.type = output; 
									if(output < 3 && output != frag->ramp.type)
										verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_RAMP, &f);
								}
								if(input->last_mouse_button[0] == TRUE && input->mouse_button[0] == FALSE)
									active_type = -1;
								
								place[1] -= 0.05;
								start = frag->ramp.ramp[0].pos;
								col[0] = frag->ramp.ramp[0].red; 
								col[1] = frag->ramp.ramp[0].green; 
								col[2] = frag->ramp.ramp[0].blue;
								end = frag->ramp.ramp[frag->ramp.point_count - 1].pos;	
								last = 0;
								if(drag != j)
								{
									if(segment == 0)
									{
										sui_draw_2d_line_gl(place[0] - 0.125, place[1]- 0.2, place[0] - 0.125, place[1] - 0.22, color, color, color);
										sui_draw_2d_line_gl(place[0] - 0.125, place[1]- 0.135, place[0] - 0.125, place[1] - 0.15, color, color, color);
									}else
									{
										sui_draw_2d_line_gl(place[0] - 0.125, place[1]- 0.2, place[0] - 0.125, place[1] - 0.21, color, color, color);
										sui_draw_2d_line_gl(place[0] - 0.125, place[1]- 0.145, place[0] - 0.125, place[1] - 0.15, color, color, color);
									}
								}
								if(drag != -1)
								{
									sui_draw_2d_line_gl(input->pointer_x, place[1]- 0.2, input->pointer_x, place[1] - 0.22, color, color, color);
									sui_draw_2d_line_gl(input->pointer_x, place[1]- 0.135, input->pointer_x, place[1] - 0.15, color, color, color);
								}
								for(j = 0; j < frag->ramp.point_count; j++)
								{
									if(input->mode == BAM_DRAW)
									{
										glPushMatrix();
										glTranslatef(place[0] - 0.125, place[1], 0);
										if(j != 0)
										{
											sui_draw_set_vec3(col_range, 0, col[0], col[1], col[2]);
											sui_draw_set_vec3(col_range, 1, col[0], col[1], col[2]);
											col[0] = frag->ramp.ramp[j].red; 
											col[1] = frag->ramp.ramp[j].green; 
											col[2] = frag->ramp.ramp[j].blue; 
											sui_draw_set_vec3(col_range, 2, col[0], col[1], col[2]);
											sui_draw_set_vec3(col_range, 3, col[0], col[1], col[2]);
											sui_draw_set_vec2(vertex, 0, last, -0.15);
											sui_draw_set_vec2(vertex, 1, last, -0.2);
											last = ((frag->ramp.ramp[j].pos - start) / (end - start)) * 0.25;
											sui_draw_set_vec2(vertex, 2, last, -0.2);
											sui_draw_set_vec2(vertex, 3, last, -0.15);

											if(drag != j)
											{
												if(segment != j)
												{
													sui_draw_2d_line_gl(last, -0.2, last, -0.21, color, color, color);
													sui_draw_2d_line_gl(last, -0.15, last, -0.145, color, color, color);
												}else
												{
													sui_draw_2d_line_gl(last, -0.2, last, -0.22, color, color, color);
													sui_draw_2d_line_gl(last, -0.15, last, -0.135, color, color, color);
												}
											}
											sui_set_color_array_gl(col_range, 4, 3);
											sui_draw_gl(GL_QUADS, vertex, 4, 2, 0, 0, 0);
										}
										glPopMatrix();
									}else
									{
										if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
										{
											float pos_x, pos_y;
											pos_x = place[0] + (((frag->ramp.ramp[j].pos - start) / (end - start)) * 0.25 - 0.125);
											pos_y = place[1];
											if(sui_box_click_test(pos_x - 0.01, pos_y - 0.2, 0.02, 0.05))
											{
												segment = j;
												drag = j;
												drag_frag = i;
												new_color[0] = frag->ramp.ramp[j].red;
												new_color[1] = frag->ramp.ramp[j].green;	
												new_color[2] = frag->ramp.ramp[j].blue;
											}else if(j != 0 && frag->ramp.point_count < 20) /* FIX THIS! */
											{
												float last_pos_x;
												last_pos_x = place[0] + (((frag->ramp.ramp[j - 1].pos - start) / (end - start)) * 0.25 - 0.125);
												if(sui_box_click_test(last_pos_x + 0.01, pos_y - 0.2, pos_x - last_pos_x - 0.02, 0.05))
												{
													uint k;
													f = *frag;
													f.ramp.point_count++;

													f.ramp.ramp[j].pos = ((input->pointer_x - place[0] + 0.125) * (end - start) * 4 + start);
	
													f.ramp.ramp[j].red = f.ramp.ramp[j].red * 0.5 + f.ramp.ramp[j - 1].red * 0.5;
													f.ramp.ramp[j].green = f.ramp.ramp[j].green * 0.5 + f.ramp.ramp[j - 1].green * 0.5;	
													f.ramp.ramp[j].blue = f.ramp.ramp[j].blue * 0.5 + f.ramp.ramp[j - 1].blue * 0.5;

													for(k = j + 1; k < f.ramp.point_count; k++)
														f.ramp.ramp[k] = frag->ramp.ramp[k - 1];
													verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_RAMP, &f);
												}
											}
										}
										if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE && drag <= frag->ramp.point_count && drag_frag == i)
										{
											float new_pos;
											uint k = 0;
											f = *frag;
											new_pos = ((input->pointer_x - place[0] + 0.125) * (end - start) * 4 + start);
											for(j = 0; j < drag || j < frag->ramp.point_count; j++)
											{
												if(j == drag)
													k++;
												if(((k >= drag && k >= frag->ramp.point_count) || frag->ramp.ramp[k].pos > new_pos) && (j == 0 || f.ramp.ramp[j - 1].pos < new_pos))
												{
													f.ramp.ramp[j].pos = new_pos; 
													f.ramp.ramp[j].red = new_color[0];
													f.ramp.ramp[j].green = new_color[1];	
													f.ramp.ramp[j].blue = new_color[2];
													k--;
												}else
													f.ramp.ramp[j] = frag->ramp.ramp[k];
												k++;
											}
											f.ramp.point_count = j;
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_RAMP, &f);
											drag = -1;
											drag_frag = -1;
										}
									}
									if(f.ramp.point_count > 2 && ((segment == j && co_w_close_button(input, place[0] - 0.15 + ((frag->ramp.ramp[j].pos - start) / (end - start)) * 0.25, place[1] - 0.12, color, color, color))
										|| (segment != j && co_w_close_button(input, place[0] - 0.15 + ((frag->ramp.ramp[j].pos - start) / (end - start)) * 0.25, place[1] - 0.13, color, color, color))))
									{
										uint k;
										f = *frag;
										f.ramp.point_count--;
										for(k = j; k < f.ramp.point_count; k++)
											f.ramp.ramp[k] = frag->ramp.ramp[k + 1];
										verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_RAMP, &f);
									}
								}
								if(segment < frag->ramp.point_count)
								{
									if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.25, 0.12, SUI_T_SIZE, &f.ramp.ramp[segment].red, &frag->ramp.ramp[segment].red, color, color, color))
										verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_RAMP, &f);
									if(co_w_slider(input, place[0], place[1] - 0.25, 0.12, &frag->ramp.ramp[segment].red, color, color, color))
									{
										verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_RAMP, frag);
										frag->color.red = f.color.red;
									}

									if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.3, 0.12, SUI_T_SIZE, &f.ramp.ramp[segment].green, &frag->ramp.ramp[segment].green, color, color, color))
										verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_RAMP, &f);
									if(co_w_slider(input, place[0], place[1] - 0.3, 0.12, &frag->ramp.ramp[segment].green, color, color, color))
									{
										verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_RAMP, frag);
										frag->color.green = f.color.green;
									}

									if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.35, 0.12, SUI_T_SIZE, &f.ramp.ramp[segment].blue, &frag->ramp.ramp[segment].blue, color, color, color))
										verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_RAMP, &f);
									if(co_w_slider(input, place[0], place[1] - 0.35, 0.12, &frag->ramp.ramp[segment].blue, color, color, color))
									{
										verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_RAMP, frag);
										frag->color.blue = f.color.blue;
									}
								}
							}else if(input->mode == BAM_DRAW)
								co_draw_lable(mat_pos, y, color_light, "Ramp");
							if(handle_link(input, node, &frag->ramp.mapping, "MAPPING", i, 0, y, expand, color, move))
							{
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_RAMP, frag);
								*frag = f;
							}
						}
						break;
					case VN_M_FT_ANIMATION :
					{
						char *text[3] = {"Red", "Green", "Blue"};
						expand = 0.2 + 0.15 * mat_pos->size;
						place[0] = mat_pos->pos[0];
						place[1] = mat_pos->pos[1] + y;
						co_vng_fragment(place[0], place[1], expand, color);
						co_draw_material_texture(mat_pos, place[0] + (-1 + rot_tree * rot_tree) * 3 / mat_pos->size, place[1] - expand);
						if(co_material_click_test(input, mat_pos, i, place[0], place[1], expand, &move))
						{
							co_draw_param_text(place[0], place[1], 3, text, color_light);
							co_w_type_in(input, place[0] - 0.12, place[1] - 0.1, 0.24, SUI_T_SIZE, f.animation.label, 16, rename_m_fragment, frag->animation.label, color, color_light);
						}else if(input->mode == BAM_DRAW)
							co_draw_lable(mat_pos, y, color_light, "Animation");
					}
					case VN_M_FT_ALTERNATIVE :
						{	
							place[0] = mat_pos->pos[0];
							place[1] = mat_pos->pos[1] + y;
							co_vng_fragment(place[0], place[1], 0.20, color);
							co_draw_material_texture(mat_pos, place[0] + (-1 + rot_tree * rot_tree) * 3 / mat_pos->size, place[1] - 0.20);
							if(!co_material_click_test(input, mat_pos, i, place[0], place[1], 0.2, &move))
								co_draw_lable(mat_pos, y, color_light, "Alternative");
							if(handle_link(input, node, &frag->alternative.alt_a, "A", i, -30, y, expand, color, move))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_ALTERNATIVE, frag);
							if(handle_link(input, node, &frag->alternative.alt_b, "B", i, 0, y, expand, color, move))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_ALTERNATIVE, frag);
							*frag = f;
						}							
						break;
					case VN_M_FT_OUTPUT :
						{	
							static uint active = FALSE;
							char *text[1] = {"Output"};
							expand = 0.2 + 0.05 * mat_pos->size;
							place[0] = mat_pos->pos[0];
							place[1] = mat_pos->pos[1] + y;
							co_vng_fragment(place[0], place[1], expand, color);
							co_draw_material_texture(mat_pos, place[0] + (-1 + rot_tree * rot_tree) * 3 / mat_pos->size, place[1] - expand);
							if(co_material_click_test(input, mat_pos, i, place[0], place[1], expand, &move))
							{
								co_draw_param_text(place[0], place[1], 1, text, color_light);
								co_w_type_in(input, place[0] - 0.12, place[1] - 0.1, 0.24, SUI_T_SIZE, f.output.label, 16, rename_m_fragment, frag->output.label, color, color_light);
							}else if(input->mode == BAM_DRAW)
								co_draw_lable(mat_pos, y, color_light, "Output");
							if(handle_link(input, node, &frag->output.front, "Front", i, -20, y, expand, color, move))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_OUTPUT, frag);
							if(handle_link(input, node, &frag->output.back, "Back", i, 20, y, expand, color, move))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_OUTPUT, frag);
							*frag = f;
						}
						break;
				}
				if(mat_pos->expand)
					if(co_w_close_button(input, mat_pos->pos[0] + 0.1, mat_pos->pos[1] - 0.015 + y, color, color, color))
						verse_send_m_fragment_destroy(e_ns_get_node_id(node), i);

				if(input->mode == BAM_DRAW)
				{
					sui_draw_text(mat_pos->pos[0] - 0.12, mat_pos->pos[1] - 0.035 + y, SUI_T_SIZE, SUI_T_SPACE, material_type_names[type], color_light, color_light, color_light);  
					glPopMatrix();
				}
			}
		}
		if(sw_text_button(input, -0.3, y - 0.05, 0, SUI_T_SIZE, SUI_T_SPACE, "Create new Fragment", color, color, color))
			create = TRUE;
		if(create)
		{
			SUIPUElement e[VN_M_FT_OUTPUT + 1];
			uint output;
			for(i = 0; i < VN_M_FT_OUTPUT + 1; i++)
			{
				e[i].type = PU_T_BOTTOM; 
				e[i].text = material_type_names[i];
			}
			output = sui_draw_popup(input, -0.2, y - 0.05, e, VN_M_FT_OUTPUT + 1, 0);
			if(output <= VN_M_FT_OUTPUT)
			{
				clear_new_frag(&f, output);
				verse_send_m_fragment_create(e_ns_get_node_id(node), (uint16)-1, output, &f);
			}
		}
		if(input->last_mouse_button[0] == TRUE && input->mouse_button[0] == FALSE)
			create = FALSE;
	}

	if(input->mode == BAM_DRAW)
	{
		glPopMatrix();
	}
	glPopMatrix();
	return co_handle_return(input);
}
