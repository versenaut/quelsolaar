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

static uint change_m_node_id = 0;
extern float co_background_color[3];
extern float co_line_color[3];

static void rename_m_fragment(void *user, char *text)
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
				default:
				break;
			}
		}
	}
}

static void clear_new_frag(VMatFrag *frag, uint type, VNMFragmentID input_a, VNMFragmentID input_b, uint mode)
{
	char *color = "color", *color_r = "color_r", *color_g = "color_g", *color_b = "color_b", *vertex = "vertex";
	double matrix[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
	double desaturate[16] = {0.3333, 0.3333, 0.3333, 0, 0.3333, 0.3333, 0.3333, 0, 0.3333, 0.3333, 0.3333, 0, 0, 0, 0, 1};
	ENode *node;
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
			break;
		case VN_M_FT_GEOMETRY :
			for(i = 0; vertex[i] != 0; i++)
				frag->geometry.layer_r[i] = vertex[i]; 
			frag->geometry.layer_r[i] = 0; 
			for(i = 0; vertex[i] != 0; i++)
				frag->geometry.layer_g[i] = vertex[i]; 
			frag->geometry.layer_g[i] = 0; 
			for(i = 0; vertex[i] != 0; i++)
				frag->geometry.layer_b[i] = vertex[i]; 
			frag->geometry.layer_b[i] = 0; 
			break;
		case VN_M_FT_TEXTURE :
			if((node = e_ns_get_node_next(0, 0, V_NT_BITMAP)) != NULL) 
				frag->texture.bitmap = e_ns_get_node_id(node);
			else
				frag->texture.bitmap = -1;
			frag->texture.mapping = input_a;
			for(i = 0; color_r[i] != 0; i++)
				frag->texture.layer_r[i] = color_r[i]; 
			frag->texture.layer_r[i] = 0; 
			for(i = 0; color_g[i] != 0; i++)
				frag->texture.layer_g[i] = color_g[i]; 
			frag->texture.layer_g[i] = 0; 
			for(i = 0; color_b[i] != 0; i++)
				frag->texture.layer_b[i] = color_b[i]; 
			frag->texture.layer_b[i] = 0; 
			frag->texture.filtered = TRUE;
			break;
		case VN_M_FT_NOISE :
			frag->noise.mapping = input_a;
			frag->noise.type = mode;
			break;
		case VN_M_FT_BLENDER :
			frag->blender.control = -1;
			frag->blender.data_a = input_a;
			frag->blender.data_b = input_b;
			frag->blender.type = mode;
			break;
		case VN_M_FT_CLAMP :
			frag->clamp.min = mode;
			frag->clamp.red = 0.5;
			frag->clamp.green = 0.5;
			frag->clamp.blue = 0.5;
			frag->clamp.data = input_a;
			break;
		case VN_M_FT_MATRIX :
			frag->matrix.data = input_a;
			if(mode == 0)
				for(i = 0; i < 16; i++)
					frag->matrix.matrix[i] = matrix[i];
			else
				for(i = 0; i < 16; i++)
					frag->matrix.matrix[i] = desaturate[i];
			break;
		case VN_M_FT_RAMP :
			frag->ramp.mapping = input_a;
			frag->ramp.channel = mode;
			frag->ramp.type = VN_M_RAMP_SQUARE;
			if(mode == 0)
			{
				frag->ramp.point_count = 3;
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
			}else
			{
				frag->ramp.point_count = 2;
				frag->ramp.ramp[0].pos = 0;
				frag->ramp.ramp[0].red = 1;
				frag->ramp.ramp[0].green = 1;
				frag->ramp.ramp[0].blue = 1;
				frag->ramp.ramp[1].pos = 1;
				frag->ramp.ramp[1].red = 0;
				frag->ramp.ramp[1].green = 0;
				frag->ramp.ramp[1].blue = 0;
			}
			break;
		case VN_M_FT_ANIMATION :
			frag->animation.label[0] = 0;
		case VN_M_FT_ALTERNATIVE :
			frag->alternative.alt_a = input_a;
			frag->alternative.alt_b = input_b;
			break;
		case VN_M_FT_OUTPUT :
			frag->output.back = input_b;
			frag->output.front = input_a;
			for(i = 0; color[i] != 0; i++)
				frag->output.label[i] = color[i];
			frag->output.label[i] = 0;
			break;
	}
}

#define LINK_SECTIONS 80
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

float frag_placement_x[1];
float frag_placement_y[1];
uint16 frag_placement_id[1] = {-1};

static void co_m_place_frag(ENode *node, uint16 id, float pos_x, float vec, uint gen)
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
			case VN_M_FT_CLAMP :
				co_m_place_frag(node, frag->clamp.data, pos_x, vec, gen + 1);
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
			default:
				break;
		}
	}
}

static void co_place_all_fragments(ENode *node)
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
		if((e_nsm_get_fragment_type(node, id) != VN_M_FT_TEXTURE && e_nsm_get_fragment_type(node, id) != VN_M_FT_CLAMP && e_nsm_get_fragment_type(node, id) != VN_M_FT_NOISE && e_nsm_get_fragment_type(node, id) != VN_M_FT_MATRIX && e_nsm_get_fragment_type(node, id) != VN_M_FT_RAMP && e_nsm_get_fragment_type(node, id) != VN_M_FT_ALTERNATIVE) && (pos = e_nsm_get_custom_data(node, id, CO_ENOUGH_NODE_SLOT)) != NULL && pos->placed != TRUE)
			co_m_place_frag(node, id, pos_x++ * 0.05, 0, 0);
	for(id = e_nsm_get_fragment_next(node, 0); id != (VNMFragmentID)-1; id = e_nsm_get_fragment_next(node, id + 1))
		if((pos = e_nsm_get_custom_data(node, id, CO_ENOUGH_NODE_SLOT)) != NULL && pos->placed != TRUE)
			co_m_place_frag(node, id, pos_x++ * 0.05, 0, 0);
}

static void co_place_all_fragments_redo(ENode *node)
{
	VNMFragmentID	id;

	/* Forget all fragment's current locations. */
	for(id = e_nsm_get_fragment_next(node, 0); id != (VNMFragmentID) ~0u; id = e_nsm_get_fragment_next(node, id + 1))
	{
		COVNMaterial *pos;
		if((pos = e_nsm_get_custom_data(node, id, CO_ENOUGH_NODE_SLOT)) != NULL)
			pos->placed = FALSE;
	}
}

static float compute_spline(float f, float v0, float v1, float v2, float v3)
{
	return ((v0 * f + v1 * (1 - f)) * f + (v1 * f + v2 * (1 - f)) * (1 - f)) * f +	((v1 * f + v2 * (1 - f)) * f + (v2 * f + v3 * (1 - f)) * (1 - f)) * (1 - f);
}

static void draw_spline(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, float color)
{
	float vertex[LINK_SECTIONS * 2];
	uint i;
	for(i = 0; i < LINK_SECTIONS; i++)
		sui_draw_set_vec2(vertex, i, compute_spline((float)((i + 1) / 2) / (float)LINK_SECTIONS / 0.5, x0, x1, x2, x3), compute_spline((float)((i + 1) / 2) / (float)LINK_SECTIONS / 0.5, y0, y1, y2, y3));
	sui_draw_gl(GL_LINES, vertex, LINK_SECTIONS, 2, co_line_color[0], co_line_color[1], co_line_color[2], color);
}

static void draw_frag_link(float x, float y, float rot, float color)
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
	sui_draw_gl(GL_LINES, vertex, 64, 2, co_line_color[0], co_line_color[1], co_line_color[2], color);
	glPopMatrix();
}

static void set_out_link(ENode *node, uint16 id, uint16 link)
{
	VMatFrag *frag, f;
	if((frag = e_nsm_get_fragment(node, id)) == NULL)
		return;
	f = *frag;
	switch(e_nsm_get_fragment_type(node, id))
	{
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
		case VN_M_FT_CLAMP :
			f.clamp.data = link;
			verse_send_m_fragment_create(e_ns_get_node_id(node), id, VN_M_FT_CLAMP, &f);
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
		default:
		break;
	}

}

static void insert_fragment(ENode *node, uint16 *link, uint prev_id, uint id, VNMFragmentType type, uint branch_id, uint mode, float x, float y)
{
	VNMFragmentID id2;
	VMatFrag f;
	clear_new_frag(&f, type, *link, branch_id, mode);
	verse_send_m_fragment_create(e_ns_get_node_id(node), id, type, &f);
	id2 = *link;
	*link = id;
	f = *e_nsm_get_fragment(node, prev_id);
	verse_send_m_fragment_create(e_ns_get_node_id(node), prev_id, e_nsm_get_fragment_type(node, prev_id), &f);
	*link = id2;
	frag_placement_id[0] = id;
	frag_placement_x[0] = x;
	frag_placement_y[0] = y;
}

static void handle_link_popup(BInputState *input, ENode *node, uint16 *link, uint frag_id, float x, float y, float scroll)
{
	SUIPUElement element[14];
	VNMFragmentID id, id2;
	VMatFrag f;
	uint ring;
	element[0].type = PU_T_ANGLE;
	element[0].text = "matrix";
	element[0].data.angle[0] = 75;
	element[0].data.angle[1] = 105;

	element[1].type = PU_T_ANGLE;
	element[1].text = "ramp";
	element[1].data.angle[0] = 45;
	element[1].data.angle[1] = 75;

	element[2].type = PU_T_ANGLE;
	element[2].text = "alternative";
	element[2].data.angle[0] = 105;
	element[2].data.angle[1] = 135;

	element[3].type = PU_T_ANGLE;
	element[3].text = "noise";
	element[3].data.angle[0] = 75 + 180;
	element[3].data.angle[1] = 105 + 180;

	element[4].type = PU_T_ANGLE;
	element[4].text = "texture";
	element[4].data.angle[0] = 45 + 180;
	element[4].data.angle[1] = 75 + 180;

	element[5].type = PU_T_ANGLE;
	element[5].text = "clamp";
	element[5].data.angle[0] = 105 + 180;
	element[5].data.angle[1] = 135 + 180;


	element[6].type = PU_T_TOP;
	element[6].text = "add color";

	element[7].type = PU_T_TOP;
	element[7].text = "mult color";

	element[8].type = PU_T_TOP;
	element[8].text = "add texture";

	element[9].type = PU_T_TOP;
	element[9].text = "mult texture";

	element[10].type = PU_T_BOTTOM;
	element[10].text = "add noise";

	element[11].type = PU_T_BOTTOM;
	element[11].text = "mult noise";

	element[12].type = PU_T_BOTTOM;
	element[12].text = "desaturate";

	element[13].type = PU_T_BOTTOM;
	element[13].text = "freznel";

	if(co_background_color[0] + co_background_color[1] + co_background_color[2] > 1.5)
		ring = sui_draw_popup(input, x, y, element, 14, 0, 1.0);
	else
		ring = sui_draw_popup(input, x, y, element, 14, 0, 0.0);
	switch(ring)
	{
		case 0 :
			insert_fragment(node, link, frag_id, e_nsm_find_empty_slot(node, 0), VN_M_FT_MATRIX, -1, 0, x, y - scroll);
		break;
		case 1 :
			insert_fragment(node, link, frag_id, e_nsm_find_empty_slot(node, 0), VN_M_FT_RAMP, -1, 0, x, y - scroll);
		break;
		case 2 :
			insert_fragment(node, link, frag_id, e_nsm_find_empty_slot(node, 0), VN_M_FT_ALTERNATIVE, -1, 0, x, y - scroll);
		break;
		case 3 :
			insert_fragment(node, link, frag_id, e_nsm_find_empty_slot(node, 0), VN_M_FT_NOISE, -1, 0, x, y - scroll);
		break;
		case 4 :
			insert_fragment(node, link, frag_id, e_nsm_find_empty_slot(node, 0), VN_M_FT_TEXTURE, -1, 0, x, y - scroll);
		break;
		case 5 :
			insert_fragment(node, link, frag_id, e_nsm_find_empty_slot(node, 0), VN_M_FT_CLAMP, -1, 0, x, y - scroll);
		break;
		case 6 :
			id = e_nsm_find_empty_slot(node, 0);
			insert_fragment(node, link, frag_id, e_nsm_find_empty_slot(node, id + 1), VN_M_FT_BLENDER, id, VN_M_BLEND_ADD, x, y - scroll);
			clear_new_frag(&f, VN_M_FT_COLOR, -1, -1, 0);
			verse_send_m_fragment_create(e_ns_get_node_id(node), id, VN_M_FT_COLOR, &f);
		break;
		case 7 :
			id = e_nsm_find_empty_slot(node, 0);
			insert_fragment(node, link, frag_id, e_nsm_find_empty_slot(node, id + 1), VN_M_FT_BLENDER, id, VN_M_BLEND_MULTIPLY, x, y - scroll);
			clear_new_frag(&f, VN_M_FT_COLOR, -1, -1, 0);
			verse_send_m_fragment_create(e_ns_get_node_id(node), id, VN_M_FT_COLOR, &f);
		break;
		case 8 :
			id = e_nsm_find_empty_slot(node, 0);
			insert_fragment(node, link, frag_id, e_nsm_find_empty_slot(node, id + 1), VN_M_FT_BLENDER, id, VN_M_BLEND_ADD, x, y - scroll);
			for(id2 = e_nsm_get_fragment_next(node, 0); id2 != (uint16)-1 && e_nsm_get_fragment_type(node, id2) != VN_M_FT_GEOMETRY; id2 = e_nsm_get_fragment_next(node, id2 + 1));
			clear_new_frag(&f, VN_M_FT_TEXTURE, id2, -1, 0);
			verse_send_m_fragment_create(e_ns_get_node_id(node), id, VN_M_FT_TEXTURE, &f);
		break;
		case 9 :
			id = e_nsm_find_empty_slot(node, 0);
			insert_fragment(node, link, frag_id, e_nsm_find_empty_slot(node, id + 1), VN_M_FT_BLENDER, id, VN_M_BLEND_MULTIPLY, x, y - scroll);
			for(id2 = e_nsm_get_fragment_next(node, 0); id2 != (uint16)-1 && e_nsm_get_fragment_type(node, id2) != VN_M_FT_GEOMETRY; id2 = e_nsm_get_fragment_next(node, id2 + 1));
			clear_new_frag(&f, VN_M_FT_TEXTURE, id2, -1, 0);
			verse_send_m_fragment_create(e_ns_get_node_id(node), id, VN_M_FT_TEXTURE, &f);
		break;
		case 10 :
			id = e_nsm_find_empty_slot(node, 0);
			insert_fragment(node, link, frag_id, e_nsm_find_empty_slot(node, id + 1), VN_M_FT_BLENDER, id, VN_M_BLEND_ADD, x, y - scroll);
			for(id2 = e_nsm_get_fragment_next(node, 0); id2 != (uint16)-1 && e_nsm_get_fragment_type(node, id2) != VN_M_FT_GEOMETRY; id2 = e_nsm_get_fragment_next(node, id2 + 1));
			clear_new_frag(&f, VN_M_FT_TEXTURE, id2, -1, 0);
			verse_send_m_fragment_create(e_ns_get_node_id(node), id, VN_M_FT_NOISE, &f);
		break;
		case 11 :
			id = e_nsm_find_empty_slot(node, 0);
			insert_fragment(node, link, frag_id, e_nsm_find_empty_slot(node, id + 1), VN_M_FT_BLENDER, id, VN_M_BLEND_MULTIPLY, x, y - scroll);
			for(id2 = e_nsm_get_fragment_next(node, 0); id2 != (uint16)-1 && e_nsm_get_fragment_type(node, id2) != VN_M_FT_GEOMETRY; id2 = e_nsm_get_fragment_next(node, id2 + 1));
			clear_new_frag(&f, VN_M_FT_TEXTURE, id2, -1, 0);
			verse_send_m_fragment_create(e_ns_get_node_id(node), id, VN_M_FT_NOISE, &f);
		break;
		case 12 :
			insert_fragment(node, link, frag_id, e_nsm_find_empty_slot(node, 0), VN_M_FT_MATRIX, -1, 1, x, y - scroll);
		break;
		case 13 :
			id = e_nsm_find_empty_slot(node, 0);
			id2 = e_nsm_find_empty_slot(node, id + 1);
			insert_fragment(node, link, frag_id, e_nsm_find_empty_slot(node, id2 + 1), VN_M_FT_BLENDER, id2, VN_M_BLEND_MULTIPLY, x, y - scroll);
			clear_new_frag(&f, VN_M_FT_RAMP, id, -1, 2);
			verse_send_m_fragment_create(e_ns_get_node_id(node), id2, VN_M_FT_RAMP, &f);
			clear_new_frag(&f, VN_M_FT_VIEW, -1, -1, 0);
			verse_send_m_fragment_create(e_ns_get_node_id(node), id, VN_M_FT_VIEW, &f);
		break;
	}
}

static boolean handle_link(BInputState *input, ENode *node, uint16 *link, char *text, uint frag_id, float angle, float scroll, float length, float color, uint16 move)
{
	static void *drag = NULL, *popup = NULL;
	COVNMaterial *pos, *target;
	VMatFrag *frag;
	VNMFragmentID id;
	float f, pos_x, pos_y;
	float a, b, dist, vertex[8] = { 0.0f };

	frag = e_nsm_get_fragment(node, *link);
	target = e_nsm_get_custom_data(node, *link, CO_ENOUGH_NODE_SLOT);
	pos = e_nsm_get_custom_data(node, frag_id, CO_ENOUGH_NODE_SLOT);
	angle = angle + angle * (1 - pos->size);
	pos_x = sin(angle * PI / 180); 
	pos_y = cos(angle * PI / 180);

	vertex[0] = pos->pos[0] + pos_x * pos->size * 0.17;
	vertex[1] = pos->size * -length + pos->pos[1] + scroll - pos_y * pos->size * 0.17;
	if((frag != NULL && target != NULL) || drag == link)
	{
		if(drag == link)
		{
			a = input->pointer_x;
			b = input->pointer_y - scroll;

		}else
		{
			a = target->pos[0];
			b = target->pos[1];
		}
		if(b > pos->pos[1])
			dist = b - pos->pos[1];
		else
			dist = pos->pos[1] - b;
		if(a > pos->pos[0])
			f = a - pos->pos[0];
		else
			f = pos->pos[0] - a;
		if(f > dist) 
			dist = f;
		dist *= 0.4;

		vertex[2] = pos->pos[0] + pos_x * (pos->size * 0.17 + dist); 
		vertex[3] = pos->size * -length + pos->pos[1] + scroll - pos_y * (pos->size * 0.17 + dist);
		vertex[4] = a; 
		vertex[5] = b + scroll + dist;
		vertex[6] = a;
		vertex[7] = b + scroll;
	}
	if(input->mode == BAM_DRAW)
	{
		glPushMatrix();
		glTranslatef(vertex[0], vertex[1], 0);
		glScalef(0.01 + 0.04 * pos->size, 0.01 + 0.04 * pos->size, 0.05);
		co_vng_ring();
		glPopMatrix();
		if(drag == link || (frag != NULL && target != NULL))
		{
			draw_spline(vertex[0], vertex[1], vertex[2], vertex[3], vertex[4], vertex[5], vertex[6], vertex[7], color);
			a = compute_spline(0.5, vertex[0], vertex[2], vertex[4], vertex[6]);
			b = compute_spline(0.5, vertex[1], vertex[3], vertex[5], vertex[7]);
			glPushMatrix();
			glTranslatef(a, b, 0);
			if(drag == NULL && 0.02 * 0.02 > (input->pointer_x - a) * (input->pointer_x - a) + (input->pointer_y - b) * (input->pointer_y - b))
				glScalef(0.05, 0.05, 0.05);
			else
				glScalef(0.02, 0.02, 0.02);
			co_vng_ring();
			glPopMatrix();
		}
	}else
	{
		if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
			if(0.05 * pos->size * 0.05 * pos->size > (input->pointer_x - vertex[0]) * (input->pointer_x - vertex[0]) + (input->pointer_y - vertex[1]) * (input->pointer_y - vertex[1]))
				drag = link;
		if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE && drag == link)
		{
			*link = (VNMFragmentID)-1;
			for(id = e_nsm_get_fragment_next(node, 0); id != (VNMFragmentID)-1; id = e_nsm_get_fragment_next(node, id + 1))
				if(id != frag_id && (target = e_nsm_get_custom_data(node, id, CO_ENOUGH_NODE_SLOT)) != NULL)
					if(sui_box_click_test(target->pos[0] - 0.15 * target->size, target->pos[1] - 0.05 + scroll, 0.3 * target->size, 0.4 * target->size))
						*link = id;
			return TRUE;
		}
		if(drag != link && input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE && move != (uint16) ~0u)
		{
			if(move != frag_id &&(input->pointer_x - vertex[0]) * (input->pointer_x - vertex[0]) + (input->pointer_y - vertex[1]) * (input->pointer_y - vertex[1]) < 0.02 * 0.02)
			{
				*link = move;
				return TRUE;
			}
			if(move != frag_id && frag != NULL && target != NULL)
			{
				a = compute_spline(0.5, vertex[0], vertex[2], vertex[4], vertex[6]);
				b = compute_spline(0.5, vertex[1], vertex[3], vertex[5], vertex[7]);
				if(0.02 * 0.02 > (input->pointer_x - a) * (input->pointer_x - a) + (input->pointer_y - b) * (input->pointer_y - b))
				{
					set_out_link(node, move, *link);
					*link = move;
					return TRUE;
				}
			}
		}
		if(target != NULL && input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
		{
			a = compute_spline(0.5, vertex[0], vertex[2], vertex[4], vertex[6]);
			b = compute_spline(0.5, vertex[1], vertex[3], vertex[5], vertex[7]);
			if(0.02 * 0.02 > (input->pointer_x - a) * (input->pointer_x - a) + (input->pointer_y - b) * (input->pointer_y - b))
				popup = link;
		}
	}
	if(popup == link)
	{
		a = compute_spline(0.5, vertex[0], vertex[2], vertex[4], vertex[6]);
		b = compute_spline(0.5, vertex[1], vertex[3], vertex[5], vertex[7]);
		handle_link_popup(input, node, link, frag_id, a, b, scroll);
	}
	if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == FALSE)
		drag = popup = NULL;
	return FALSE;
}

static void co_draw_param_text(float x, float y, uint count, char **text, float color)
{
	uint i;
	for(i = 0; i < count; i++)
	{
		sui_draw_2d_line_gl(x - 0.17, y - 0.1 + SUI_T_SIZE - 0.05 * (float)i, 
						x - 0.27, y - 0.125 + SUI_T_SIZE - 0.1 * ((float)i - 0.25 * (float)count), co_line_color[0], co_line_color[1], co_line_color[2], color);
		sui_draw_text(x - 0.3 - sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, text[i]), y - 0.125 + SUI_T_SIZE - 0.1 * ((float)i - 0.25 * (float)count), SUI_T_SIZE, SUI_T_SPACE, text[i], co_line_color[0], co_line_color[1], co_line_color[2], color);  
	}
}

extern void render_material(ENode *node, VNMFragmentID id, uint size, uint line, float *buffer);

void co_update_context_textures(void)
{
	ENode *node;
	VNMFragmentID id;
	COVNMaterial *mat;
	
	for(node = e_ns_get_node_next(0, 0, V_NT_MATERIAL); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_MATERIAL))
	{
		for(id = e_nsm_get_fragment_next(node, 0); id != (uint16)-1; id = e_nsm_get_fragment_next(node, 1 + id))
		{	
			mat = e_nsm_get_custom_data(node, id, CO_ENOUGH_NODE_SLOT);	
			if(mat != NULL)
			{
				glGenTextures(1, &mat->texture_id);
				mat->version = 56756;
				glBindTexture(GL_TEXTURE_2D, mat->texture_id);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_RESOLUTION, TEXTURE_RESOLUTION, 0, GL_RGB, GL_FLOAT, NULL);
			}
		}
	}
}

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
		data->placed = frag_placement_id[0] == frag;
		if(data->placed)
		{
			data->pos[0] = frag_placement_x[0];
			data->pos[1] = frag_placement_y[0];
		}
		e_nsm_set_custom_data(node, frag, CO_ENOUGH_NODE_SLOT, data);
		glGenTextures(1, &data->texture_id);
		buf = malloc((sizeof *buf) * TEXTURE_RESOLUTION * TEXTURE_RESOLUTION * 3);
		for(i = 0; i < TEXTURE_RESOLUTION * TEXTURE_RESOLUTION * 3;)
		{
			buf[i++] = co_background_color[0];
			buf[i++] = co_background_color[1];
			buf[i++] = co_background_color[2];
		}
		glBindTexture(GL_TEXTURE_2D, data->texture_id);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_RESOLUTION, TEXTURE_RESOLUTION, 0, GL_RGB, GL_FLOAT, buf);
		glBindTexture(GL_TEXTURE_2D, 0);
		free(buf);
	}
	if(command == E_CDC_DESTROY && (data = e_nsm_get_custom_data(node, frag, CO_ENOUGH_NODE_SLOT)) != NULL)
	{
		glDeleteTextures(1, &data->texture_id);
		free(data);
		e_nsm_set_custom_data(node, frag, CO_ENOUGH_NODE_SLOT, NULL);
	}
}

uint co_material_get_texture_id(ENode *node, VNMFragmentID id)
{
	COVNMaterial *mat;
	mat = e_nsm_get_custom_data(node, id, CO_ENOUGH_NODE_SLOT);	
	return mat->texture_id;
}

uint co_material_get_version(ENode *node, VNMFragmentID id)
{
	COVNMaterial *mat;
	mat = e_nsm_get_custom_data(node, id, CO_ENOUGH_NODE_SLOT);
/*	printf("got fragment at %p, fragment %u\n", mat, id);
	printf("id = %u co_material_get_version %p %u\n", id, mat, mat->version);
*/	return mat->version;
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

	if(ref[0] == ~0u)
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
	sui_draw_elements_gl(GL_TRIANGLES, vertex, ref, 64 * 3, 2, 1, 1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
}


void co_draw_material(ENode *node)
{
	COVNMaterial *mat;
	VNMFragmentID id;
	id = e_nsm_get_fragment_color_front(node);
	if(id == (VNMFragmentID) ~0u)
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
		mat->size += betray_get_delta_time() / 0.2;
		if(mat->size > 1.0)
			mat->size = 1.0;

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
		mat->size -= betray_get_delta_time() / 0.2;
		if(mat->size < 0.1)
			mat->size = 0.1;
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
	sui_draw_2d_line_gl(mat->size * 0.2, -0.015, mat->size * 0.2 + 0.04, 0, color, color, color, 1);
	sui_draw_text(mat->size * 0.2 + 0.04, 0, SUI_T_SIZE, SUI_T_SPACE, lable, co_line_color[0], co_line_color[1], co_line_color[2], color);  		
	glPopMatrix();
}

extern float co_handle_node_head(BInputState *input, ENode *node, boolean reset);

boolean co_handle_material(BInputState *input, ENode *node)
{
	static char *material_type_names[] = {"COLOR", "LIGHT", "REFLECTION", "TRANSPARENCY", "VOLUME", "VIEW", "GEOMETRY", "TEXTURE",  "NOISE", "BLENDER", "CLAMP", "MATRIX", "RAMP", "ANIMATION", "ALTERNATIVE", "OUTPUT"};
	static boolean show_tree = TRUE;
	static VNMFragmentID move = -1;
	static float rot_tree = 1;
	COVNMaterial *mat_pos;
	float expand = 0.0f, place[3] = {0, 0, 0};
	uint type = 0, j = 0;
	VNMFragmentID i;
	VMatFrag *frag, f;
	float y, pre_expander, color, color_light;

	y = co_handle_node_head(input, node, change_m_node_id != e_ns_get_node_id(node));
	change_m_node_id = e_ns_get_node_id(node);

	co_vng_divider(input, 0.2, y, &rot_tree, &color, &color_light, &show_tree, "FRAGMENT TREE");
	pre_expander = y;

	if(rot_tree > 0.001)
	{
		static boolean create = FALSE, c_now = FALSE, c_last;
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

							if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.1, 0, 0.12, SUI_T_SIZE, &f.color.red, &frag->color.red, co_line_color[0], co_line_color[1], co_line_color[2], color))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_COLOR, &f);
							if(co_w_slider(input, place[0], place[1] - 0.1, 0.12, &frag->color.red, 0, 1,  color))
							{
								if(f.color.red < f.color.green + 0.001 && f.color.red > f.color.green - 0.001 &&
									f.color.red < f.color.blue + 0.001 && f.color.red > f.color.blue - 0.001)
								{
									frag->color.green = frag->color.red;
									frag->color.blue = frag->color.red;
								}	
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_COLOR, frag);
								frag->color = f.color;
							}

							if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.15, 0, 0.12, SUI_T_SIZE, &f.color.green, &frag->color.green, co_line_color[0], co_line_color[1], co_line_color[2], color))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_COLOR, &f);
							if(co_w_slider(input, place[0], place[1] - 0.15, 0.12, &frag->color.green, 0, 1, color))
							{
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_COLOR, frag);
								frag->color.green = f.color.green;
							}

							if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.2, 0, 0.12, SUI_T_SIZE, &f.color.blue, &frag->color.blue, co_line_color[0], co_line_color[1], co_line_color[2], color))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_COLOR, &f);
							if(co_w_slider(input, place[0], place[1] - 0.2, 0.12, &frag->color.blue, 0, 1, color))
							{
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_COLOR, frag);
								frag->color.blue = f.color.blue;
							}
						}else if(input->mode == BAM_DRAW)
							co_draw_lable(mat_pos, y, color_light, "Color");
						if(input->mode == BAM_DRAW)
							glPopMatrix();
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
							if(sw_text_button(input, place[0] - 0.12, place[1] - 0.1, 0, SUI_T_SIZE, SUI_T_SPACE, light_types[frag->light.type], co_line_color[0], co_line_color[1], co_line_color[2], color))
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
								if(co_background_color[0] + co_background_color[1] + co_background_color[2] > 1.5)
									output = sui_draw_popup(input, place[0] - 0.12, place[1] - 0.1, e, 6, 0, 1.0);
								else
									output = sui_draw_popup(input, place[0] - 0.12, place[1] - 0.1, e, 6, 0, 0.0);
								if(output < 6 && output != frag->light.type)
								{
									f.light.type = output;
									verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_LIGHT, &f);
								}
							}
							if(input->last_mouse_button[0] == FALSE && input->mouse_button[0] == FALSE)
								active = -1;
		
							if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.15, 0, 0.12, SUI_T_SIZE, &f.light.normal_falloff, &frag->light.normal_falloff, co_line_color[0], co_line_color[1], co_line_color[2], color))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_LIGHT, &f);
							if(co_w_slider(input, place[0], place[1] - 0.15, 0.12, &frag->light.normal_falloff, 0, 1, color))
							{
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_LIGHT, frag);
								frag->light.normal_falloff = f.light.normal_falloff;
							}

							b_node = e_ns_get_node(0, f.light.brdf);
							if(b_node != NULL)
								sui_draw_text(place[0] - 0.12, place[1] - 0.2, SUI_T_SIZE, SUI_T_SPACE, e_ns_get_node_name(b_node), co_line_color[0], co_line_color[1], co_line_color[2], color_light);  
							else
								sui_draw_text(place[0] - 0.12, place[1] - 0.2, SUI_T_SIZE, SUI_T_SPACE, "NONE", co_line_color[0], co_line_color[1], co_line_color[2], color_light);  

							co_w_type_in(input, place[0] - 0.12, place[1] - 0.25, 0.24, SUI_T_SIZE, f.light.brdf_r, 16, rename_m_fragment, frag->light.brdf_r, color, color_light);
							co_w_type_in(input, place[0] - 0.12, place[1] - 0.3, 0.24, SUI_T_SIZE, f.light.brdf_g, 16, rename_m_fragment, frag->light.brdf_g, color, color_light);
							co_w_type_in(input, place[0] - 0.12, place[1] - 0.35, 0.24, SUI_T_SIZE, f.light.brdf_b, 16, rename_m_fragment, frag->light.brdf_b, color, color_light);
						}else if(input->mode == BAM_DRAW)
							co_draw_lable(mat_pos, y, color_light, light_labels[frag->light.type]);
						if(input->mode == BAM_DRAW)
							glPopMatrix();
					}
					break;
					case VN_M_FT_REFLECTION :
					{
						char *text[1] = {"Normal Falloff"};

						expand = 0.2 + 0.05 * mat_pos->size;
						place[0] = mat_pos->pos[0];
						place[1] = mat_pos->pos[1] + y;
						co_vng_fragment(place[0], place[1], expand, color);
						co_draw_material_texture(mat_pos, place[0] + (-1 + rot_tree * rot_tree) * 3 / mat_pos->size, place[1] - expand);
						if(co_material_click_test(input, mat_pos, i, place[0], place[1], expand, &move))
						{
							co_draw_param_text(place[0], place[1], 1, text, color_light);
							if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.1, 0, 0.12, SUI_T_SIZE, &f.reflection.normal_falloff, &frag->reflection.normal_falloff, co_line_color[0], co_line_color[1], co_line_color[2], color))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_REFLECTION, &f);
							if(co_w_slider(input, place[0], place[1] - 0.1, 0.12, &frag->reflection.normal_falloff, 0, 1, color))
							{
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_REFLECTION, frag);
								frag->reflection.normal_falloff = f.reflection.normal_falloff;
							}
						}if(input->mode == BAM_DRAW)
							co_draw_lable(mat_pos, y, color_light, "Reflection");
						if(input->mode == BAM_DRAW)
							glPopMatrix();
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
							if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.1, 0, 0.12, SUI_T_SIZE, &f.transparency.normal_falloff, &frag->transparency.normal_falloff, co_line_color[0], co_line_color[1], co_line_color[2], color))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_TRANSPARENCY, &f);
							if(co_w_slider(input, place[0], place[1] - 0.1, 0.12, &frag->transparency.normal_falloff, 0, 1, color))
							{
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_TRANSPARENCY, frag);
								frag->transparency.normal_falloff = f.transparency.normal_falloff;
							}
							if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.15, 0, 0.12, SUI_T_SIZE, &f.transparency.refraction_index, &frag->transparency.refraction_index, co_line_color[0], co_line_color[1], co_line_color[2], color))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_TRANSPARENCY, &f);
							if(co_w_slider(input, place[0], place[1] - 0.15, 0.12, &frag->transparency.refraction_index, 0, 2, color))
							{
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_TRANSPARENCY, frag);
								frag->transparency.refraction_index = f.transparency.refraction_index;
							}
						}else if(input->mode == BAM_DRAW)
							co_draw_lable(mat_pos, y, color_light, "Transparancy");
						if(input->mode == BAM_DRAW)
							glPopMatrix();
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
							if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.1, 0, 0.12, SUI_T_SIZE, &f.volume.diffusion, &frag->volume.diffusion, co_line_color[0], co_line_color[1], co_line_color[2], color))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_VOLUME, &f);
							if(co_w_slider(input, place[0], place[1] - 0.1, 0.12, &frag->volume.diffusion, 0, 1, color))
							{
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_VOLUME, frag);
								frag->volume.diffusion = f.volume.diffusion;
							}
							if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.15, 0, 0.12, SUI_T_SIZE, &f.volume.col_r, &frag->volume.col_r, co_line_color[0], co_line_color[1], co_line_color[2], color))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_VOLUME, &f);
							if(co_w_slider(input, place[0], place[1] - 0.15, 0.12, &frag->volume.col_r, 0, 1, color))
							{
								if(f.volume.col_r < f.volume.col_g + 0.001 && f.volume.col_r > f.volume.col_g - 0.001 &&
									f.volume.col_r < f.volume.col_b + 0.001 && f.volume.col_r > f.volume.col_b - 0.001)
								{
									frag->volume.col_g = frag->volume.col_r;
									frag->volume.col_b = frag->volume.col_r;
								}
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_VOLUME, frag);
								frag->volume = f.volume;
							}
							if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.2, 0, 0.12, SUI_T_SIZE, &f.volume.col_g, &frag->volume.col_g, co_line_color[0], co_line_color[1], co_line_color[2], color))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_VOLUME, &f);
							if(co_w_slider(input, place[0], place[1] - 0.2, 0.12, &frag->volume.col_g, 0, 1, color))
							{
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_VOLUME, frag);
								frag->volume.col_g = f.volume.col_g;
							}
							if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.25, 0, 0.12, SUI_T_SIZE, &f.volume.col_b, &frag->volume.col_b, co_line_color[0], co_line_color[1], co_line_color[2], color))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_VOLUME, &f);
							if(co_w_slider(input, place[0], place[1] - 0.25, 0.12, &frag->volume.col_b, 0, 1, color))
							{
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_VOLUME, frag);
								frag->volume.col_b = f.volume.col_b;
							}
						}else if(input->mode == BAM_DRAW)
							co_draw_lable(mat_pos, y, color_light, "Volume");
						if(input->mode == BAM_DRAW)
							glPopMatrix();
					}
					break;
					case VN_M_FT_VIEW :
					{
						place[0] = mat_pos->pos[0];
						place[1] = mat_pos->pos[1] + y;
						co_vng_fragment(place[0], place[1], 0.2, color);
						co_draw_material_texture(mat_pos, place[0] + (-1 + rot_tree * rot_tree) * 3 / mat_pos->size, place[1] - 0.2);
						if(!co_material_click_test(input, mat_pos, i, place[0], place[1], 0.2, &move) && input->mode == BAM_DRAW)
							co_draw_lable(mat_pos, y, color_light, "View");
						if(input->mode == BAM_DRAW)
							glPopMatrix();
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
						if(input->mode == BAM_DRAW)
							glPopMatrix();
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
								sui_draw_text(place[0] - 0.12, place[1] - 0.1, SUI_T_SIZE, SUI_T_SPACE, e_ns_get_node_name(b_node), co_line_color[0], co_line_color[1], co_line_color[2], color_light);  
							else
								sui_draw_text(place[0] - 0.12, place[1] - 0.1, SUI_T_SIZE, SUI_T_SPACE, "NONE", co_line_color[0], co_line_color[1], co_line_color[2], color_light);  
							co_w_type_in(input, place[0] - 0.12, place[1] - 0.15, 0.24, SUI_T_SIZE, f.texture.layer_r, 16, rename_m_fragment, frag->texture.layer_r, color, color_light);
							co_w_type_in(input, place[0] - 0.12, place[1] - 0.2, 0.24, SUI_T_SIZE, f.texture.layer_g, 16, rename_m_fragment, frag->texture.layer_g, color, color_light);
							co_w_type_in(input, place[0] - 0.12, place[1] - 0.25, 0.24, SUI_T_SIZE, f.texture.layer_b, 16, rename_m_fragment, frag->texture.layer_b, color, color_light);
						}else if(input->mode == BAM_DRAW)
							co_draw_lable(mat_pos, y, color_light, "Texture");
						if(input->mode == BAM_DRAW)
							glPopMatrix();
						if(handle_link(input, node, &frag->texture.mapping, "Mapping", i, 0, y, expand, color, move))
							verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_TEXTURE, frag);
						*frag = f;
					}
					break;
					case VN_M_FT_NOISE :
						{	
							static uint active = FALSE;
							char *text[1] = {"Type"};
							char *noise_types[] = {"PERLIN 0 - 1", "PERLIN -1 - 1", "POINT 0 - 1", "POINT -1 - 1"};
							char *noise_labels[] = {"Perlin 0 - 1 Noice", "Perlin -1 - 1 Noice", "Point 0 - 1 Noice", "Point -1 - 1 Noice"};
							expand = 0.2 + 0.05 * mat_pos->size;
							place[0] = mat_pos->pos[0];
							place[1] = mat_pos->pos[1] + y;
							co_vng_fragment(place[0], place[1], expand, color);
							co_draw_material_texture(mat_pos, place[0] + (-1 + rot_tree * rot_tree) * 3 / mat_pos->size, place[1] - expand);
							if(co_material_click_test(input, mat_pos, i, place[0], place[1], expand, &move))
							{
								co_draw_param_text(place[0], place[1], 1, text, color_light);
								if(sw_text_button(input, place[0] - 0.12, place[1] - 0.1, 0, SUI_T_SIZE, SUI_T_SPACE, noise_types[frag->noise.type], co_line_color[0], co_line_color[1], co_line_color[2], color))
									active = i;
								if(active == i)
								{
									SUIPUElement e[4];
									uint output;
									for(j = 0; j < 4; j++)
									{
										e[j].text = noise_types[j];
										e[j].type = PU_T_SQUARE;
										e[j].data.square.draw_func = NULL;
										e[j].data.square.square[0] = 0.3 + (float)((j) / 2) * 0.25;
										e[j].data.square.square[1] = 0.15 * ((float)((j) % 2) - 0.66666);
										e[j].data.square.square[2] = 0.2;
										e[j].data.square.square[3] = -0.1;
									}
									if(co_background_color[0] + co_background_color[1] + co_background_color[2] > 1.5)
										output = sui_draw_popup(input, place[0] - 0.12, place[1] - 0.1, e, 4, 0, 1.0);
									else
										output = sui_draw_popup(input, place[0] - 0.12, place[1] - 0.1, e, 4, 0, 0.0);
									if(output < 4 && output != frag->noise.type)
									{
										f.noise.type = output;
										verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_NOISE, &f);
									}
								}
								if(input->last_mouse_button[0] == FALSE && input->mouse_button[0] == FALSE)
									active = -1;
							}else if(input->mode == BAM_DRAW)
								co_draw_lable(mat_pos, y, color_light, noise_labels[frag->noise.type]);
							if(input->mode == BAM_DRAW)
								glPopMatrix();
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
								if(sw_text_button(input, place[0] - 0.12, place[1] - 0.1, 0, SUI_T_SIZE, SUI_T_SPACE, blend_types[frag->blender.type], co_line_color[0], co_line_color[1], co_line_color[2], color))
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
									if(co_background_color[0] + co_background_color[1] + co_background_color[2] > 1.5)
										output = sui_draw_popup(input, place[0] - 0.12, place[1] - 0.1, e, 6, 0, 1.0);
									else
										output = sui_draw_popup(input, place[0] - 0.12, place[1] - 0.1, e, 6, 0, 0.0);
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
							if(input->mode == BAM_DRAW)
								glPopMatrix();
							if(handle_link(input, node, &frag->blender.data_a, "A", i, -30, y, expand, color, move))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_BLENDER, frag);
							if(handle_link(input, node, &frag->blender.data_b, "B", i, 0, y, expand, color, move))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_BLENDER, frag);
							if(handle_link(input, node, &frag->blender.control, "Control", i, 30, y, expand, color, move))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_BLENDER, frag);
							*frag = f;
						}
						break;
					case VN_M_FT_CLAMP :
						{	
							static uint active = FALSE;
							char *text[4] = {"Direction", "Red", "Green", "Blue"};
							char *clap_direction[] = {"Max", "Min"};
							expand = 0.2 + 0.2 * mat_pos->size;
							place[0] = mat_pos->pos[0];
							place[1] = mat_pos->pos[1] + y;
							co_vng_fragment(place[0], place[1], expand, color);
							co_draw_material_texture(mat_pos, place[0] + (-1 + rot_tree * rot_tree) * 3 / mat_pos->size, place[1] - expand);
							if(co_material_click_test(input, mat_pos, i, place[0], place[1], expand, &move))
							{
								co_draw_param_text(place[0], place[1], 4, text, color_light);
								if(sw_text_button(input, place[0] - 0.12, place[1] - 0.1, 0, SUI_T_SIZE, SUI_T_SPACE, clap_direction[f.clamp.min], co_line_color[0], co_line_color[1], co_line_color[2], color))
									active = i;
								if(active == i)
								{
									SUIPUElement e[2];
									uint output;
									for(j = 0; j < 2; j++)
									{
										e[j].text = clap_direction[j];
										e[j].type = PU_T_SQUARE;
										e[j].data.square.draw_func = NULL;
										e[j].data.square.square[0] = 0.3 + (float)((j) / 3) * 0.25;
										e[j].data.square.square[1] = 0.15 * ((float)((j) % 3) - 0.66666);
										e[j].data.square.square[2] = 0.2;
										e[j].data.square.square[3] = -0.1;
									}
									if(co_background_color[0] + co_background_color[1] + co_background_color[2] > 1.5)
										output = sui_draw_popup(input, place[0] - 0.12, place[1] - 0.1, e, 6, 0, 1.0);
									else
										output = sui_draw_popup(input, place[0] - 0.12, place[1] - 0.1, e, 6, 0, 0.0);
									if(output < 2 && output != f.clamp.min)
									{
										f.clamp.min = output;
										verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_CLAMP, &f);
									}
								}

								if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.15, 0, 0.12, SUI_T_SIZE, &f.clamp.red, &frag->clamp.red, co_line_color[0], co_line_color[1], co_line_color[2], color))
									verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_CLAMP, &f);
								if(co_w_slider(input, place[0], place[1] - 0.15, 0.12, &frag->clamp.red, 0, 1, color))
								{
									if(f.clamp.red < f.clamp.green + 0.001 && f.clamp.red > f.clamp.green - 0.001 &&
										f.clamp.red < f.clamp.blue + 0.001 && f.clamp.red > f.clamp.blue - 0.001)
									{
										frag->clamp.green = frag->clamp.red;
										frag->clamp.blue = frag->clamp.red;
									}
									verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_CLAMP, frag);
									frag->clamp = f.clamp;
								}

								if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.2, 0, 0.12, SUI_T_SIZE, &f.clamp.green, &frag->clamp.green, co_line_color[0], co_line_color[1], co_line_color[2], color))
									verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_CLAMP, &f);
								if(co_w_slider(input, place[0], place[1] - 0.2, 0.12, &frag->clamp.green, 0, 1, color))
								{
									verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_CLAMP, frag);
									frag->clamp.green = f.clamp.green;
								}

								if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.25, 0, 0.12, SUI_T_SIZE, &f.clamp.blue, &frag->clamp.blue, co_line_color[0], co_line_color[1], co_line_color[2], color))
									verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_CLAMP, &f);
								if(co_w_slider(input, place[0], place[1] - 0.25, 0.12, &frag->clamp.blue, 0, 1, color))
								{
									verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_CLAMP, frag);
									frag->clamp.blue = f.clamp.blue;
								}
								if(input->last_mouse_button[0] == FALSE && input->mouse_button[0] == FALSE)
									active = -1;
							}else if(input->mode == BAM_DRAW)
								co_draw_lable(mat_pos, y, color_light, "Clamp");
							if(input->mode == BAM_DRAW)
								glPopMatrix();
							if(handle_link(input, node, &frag->clamp.data, "Input", i, 0, y, expand, color, move))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_CLAMP, frag);
							*frag = f;
	
						}
						break;
					case VN_M_FT_MATRIX :
						{	
							static uint mode = 2, active = -1;
							float reset[] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
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
								if(sw_text_button(input, place[0] - 0.12, place[1] - 0.1, 0, SUI_T_SIZE, SUI_T_SPACE, matrix_types[mode], co_line_color[0], co_line_color[1], co_line_color[2], color))
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
									if(co_background_color[0] + co_background_color[1] + co_background_color[2] > 1.5)
										output = sui_draw_popup(input, place[0] - 0.12, place[1] - 0.1, e, 3, 0, 1.0);
									else
										output = sui_draw_popup(input, place[0] - 0.12, place[1] - 0.1, e, 3, 0, 0.0);
									if(output < 3)
										mode = output;
								}
								if(input->last_mouse_button[0] == FALSE && input->mouse_button[0] == FALSE)
									active = -1;
								

								if(sw_text_button(input, place[0] - 0.12, place[1] - 0.15, 0, SUI_T_SIZE, SUI_T_SPACE, "Ident", co_line_color[0], co_line_color[1], co_line_color[2], color))
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
										if(sui_type_number_double(input, place[0] - 0.13, place[1] - 0.2, 0, 0.12, SUI_T_SIZE, &f.matrix.matrix[0], &frag->matrix.matrix[0], co_line_color[0], co_line_color[1], co_line_color[2], color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										if(sui_type_number_double(input, place[0] - 0.06, place[1] - 0.2, 0, 0.12, SUI_T_SIZE, &f.matrix.matrix[1], &frag->matrix.matrix[1], co_line_color[0], co_line_color[1], co_line_color[2], color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										if(sui_type_number_double(input, place[0] + 0.01, place[1] - 0.2, 0, 0.12, SUI_T_SIZE, &f.matrix.matrix[2], &frag->matrix.matrix[2], co_line_color[0], co_line_color[1], co_line_color[2], color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										if(sui_type_number_double(input, place[0] + 0.08, place[1] - 0.2, 0, 0.12, SUI_T_SIZE, &f.matrix.matrix[3], &frag->matrix.matrix[3], co_line_color[0], co_line_color[1], co_line_color[2], color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);

										if(sui_type_number_double(input, place[0] - 0.13, place[1] - 0.25, 0, 0.12, SUI_T_SIZE, &f.matrix.matrix[4], &frag->matrix.matrix[4], co_line_color[0], co_line_color[1], co_line_color[2], color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										if(sui_type_number_double(input, place[0] - 0.06, place[1] - 0.25, 0, 0.12, SUI_T_SIZE, &f.matrix.matrix[5], &frag->matrix.matrix[5], co_line_color[0], co_line_color[1], co_line_color[2], color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										if(sui_type_number_double(input, place[0] + 0.01, place[1] - 0.25, 0, 0.12, SUI_T_SIZE, &f.matrix.matrix[6], &frag->matrix.matrix[6], co_line_color[0], co_line_color[1], co_line_color[2], color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										if(sui_type_number_double(input, place[0] + 0.08, place[1] - 0.25, 0, 0.12, SUI_T_SIZE, &f.matrix.matrix[7], &frag->matrix.matrix[7], co_line_color[0], co_line_color[1], co_line_color[2], color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);

										if(sui_type_number_double(input, place[0] - 0.13, place[1] - 0.3, 0, 0.12, SUI_T_SIZE, &f.matrix.matrix[8], &frag->matrix.matrix[8], co_line_color[0], co_line_color[1], co_line_color[2], color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										if(sui_type_number_double(input, place[0] - 0.06, place[1] - 0.3, 0, 0.12, SUI_T_SIZE, &f.matrix.matrix[9], &frag->matrix.matrix[9], co_line_color[0], co_line_color[1], co_line_color[2], color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										if(sui_type_number_double(input, place[0] + 0.01, place[1] - 0.3, 0, 0.12, SUI_T_SIZE, &f.matrix.matrix[10], &frag->matrix.matrix[10], co_line_color[0], co_line_color[1], co_line_color[2], color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										if(sui_type_number_double(input, place[0] + 0.08, place[1] - 0.3, 0, 0.12, SUI_T_SIZE, &f.matrix.matrix[11], &frag->matrix.matrix[11], co_line_color[0], co_line_color[1], co_line_color[2], color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);

										if(sui_type_number_double(input, place[0] - 0.13, place[1] - 0.35, 0, 0.12, SUI_T_SIZE, &f.matrix.matrix[12], &frag->matrix.matrix[12], co_line_color[0], co_line_color[1], co_line_color[2], color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										if(sui_type_number_double(input, place[0] - 0.06, place[1] - 0.35, 0, 0.12, SUI_T_SIZE, &f.matrix.matrix[13], &frag->matrix.matrix[13], co_line_color[0], co_line_color[1], co_line_color[2], color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										if(sui_type_number_double(input, place[0] + 0.01, place[1] - 0.35, 0, 0.12, SUI_T_SIZE, &f.matrix.matrix[14], &frag->matrix.matrix[14], co_line_color[0], co_line_color[1], co_line_color[2], color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										if(sui_type_number_double(input, place[0] + 0.08, place[1] - 0.35, 0, 0.12, SUI_T_SIZE, &f.matrix.matrix[15], &frag->matrix.matrix[15], co_line_color[0], co_line_color[1], co_line_color[2], color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
									}
									break;
									case 1 :
									{
										char *text[] = {"Type", "Reset", "Contrast", "Brightness"};
										double pre, value;
										co_draw_param_text(place[0], place[1], 4, text, color_light);
										value = pre = (f.matrix.matrix[0] + f.matrix.matrix[5] + f.matrix.matrix[10]) / 3;
										if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.2, 0, 0.12, SUI_T_SIZE, &value, &frag->matrix.matrix[0], co_line_color[0], co_line_color[1], co_line_color[2], color))
										{
											f.matrix.matrix[0] *= value / pre;
											f.matrix.matrix[5] *= value / pre;
											f.matrix.matrix[10] *= value / pre;
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										}
										frag->matrix.matrix[1] = value / 3;
										if(co_w_slider(input, place[0], place[1] - 0.2, 0.12, &frag->matrix.matrix[1], 0, 1, color))
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

										if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.2, 0, 0.12, SUI_T_SIZE, &f.matrix.matrix[0], &frag->matrix.matrix[0], co_line_color[0], co_line_color[1], co_line_color[2], color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										frag->matrix.matrix[0] *= 0.25;
										if(co_w_slider(input, place[0], place[1] - 0.2, 0.12, &frag->matrix.matrix[0], 0, 1, color))
										{
											frag->matrix.matrix[0] /= 0.25;
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, frag);
											frag->matrix.matrix[0] = f.matrix.matrix[0];
										}

										if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.25, 0, 0.12, SUI_T_SIZE, &f.matrix.matrix[5], &frag->matrix.matrix[5], co_line_color[0], co_line_color[1], co_line_color[2], color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										frag->matrix.matrix[5] *= 0.25;
										if(co_w_slider(input, place[0], place[1] - 0.25, 0.12, &frag->matrix.matrix[5], 0, 1, color))
										{
											frag->matrix.matrix[5] /= 0.25;
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, frag);
											frag->matrix.matrix[5] = f.matrix.matrix[5];
										}

										if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.3, 0, 0.12, SUI_T_SIZE, &f.matrix.matrix[10], &frag->matrix.matrix[10], co_line_color[0], co_line_color[1], co_line_color[2], color))
											verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, &f);
										frag->matrix.matrix[10] *= 0.25;
										if(co_w_slider(input, place[0], place[1] - 0.3, 0.12, &frag->matrix.matrix[10], 0, 1, color))
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
				//			co_vng_fragment(place[0] - 0.015, place[1] - 0.015, expand, color);
							co_vng_fragment(place[0], place[1], expand + 0.015, color);
							co_draw_material_texture(mat_pos, place[0] + (-1 + rot_tree * rot_tree) * 3 / mat_pos->size, place[1] - 0.015 - expand);
							if(input->mode == BAM_DRAW)
								glPopMatrix();
							if(handle_link(input, node, &frag->matrix.data, "Input", i, 0, y, expand + 0.015, color, move))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_MATRIX, frag);
							*frag = f;
						}
						break;
					case VN_M_FT_RAMP :
						{
							static uint active_type = -1, active_channel = -1;
							static double new_color[3];
							static uint segment = 0, drag = ~0u, drag_frag = -1;
							static char *ramp_types[] = {"SQUARE", "LINEAR", "SMOOTH"};
							static char *ramp_channels[] = {"RED", "GREEN", "BLUE"};
							char *text[] = {"Channel", "Type", "Delete", "Ramp", "Pos", "Red", "Green", "Blue"};
							double col[3], start, end, last;
							float col_range[12], vertex[8];
							expand = 0.2 + 0.4 * mat_pos->size;
							place[0] = mat_pos->pos[0];
							place[1] = mat_pos->pos[1] + y;
							co_vng_fragment(place[0], place[1], expand, color);
							co_draw_material_texture(mat_pos, place[0] + (-1 + rot_tree * rot_tree) * 3 / mat_pos->size, place[1] - expand);
							f = *frag;
							if(co_material_click_test(input, mat_pos, i, place[0], place[1], expand, &move))
							{
								co_draw_param_text(place[0], place[1], 8, text, color_light);
								if(sw_text_button(input, place[0] - 0.12, place[1] - 0.1, 0, SUI_T_SIZE, SUI_T_SPACE, ramp_channels[frag->ramp.channel], co_line_color[0], co_line_color[1], co_line_color[2], color))
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
									if(co_background_color[0] + co_background_color[1] + co_background_color[2] > 1.5)
										output = sui_draw_popup(input, place[0] - 0.12, place[1] - 0.1, e, 3, 0, 1.0);
									else
										output = sui_draw_popup(input, place[0] - 0.12, place[1] - 0.1, e, 3, 0, 0.0);
									f.ramp.channel = output; 
									if(output < 3 && output != frag->ramp.channel)
										verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_RAMP, &f);
								}
								if(input->last_mouse_button[0] == FALSE && input->mouse_button[0] == FALSE)
									active_channel = -1;
								
								if(sw_text_button(input, place[0] - 0.12, place[1] - 0.15, 0, SUI_T_SIZE, SUI_T_SPACE, ramp_types[frag->ramp.type], co_line_color[0], co_line_color[1], co_line_color[2], color))
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
									if(co_background_color[0] + co_background_color[1] + co_background_color[2] > 1.5)
										output = sui_draw_popup(input, place[0] - 0.12, place[1] - 0.15, e, 3, 0, 1.0);
									else
										output = sui_draw_popup(input, place[0] - 0.12, place[1] - 0.15, e, 3, 0, 0.0);
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
										sui_draw_2d_line_gl(place[0] - 0.125, place[1]- 0.2, place[0] - 0.125, place[1] - 0.22, co_line_color[0], co_line_color[1], co_line_color[2], color);
										sui_draw_2d_line_gl(place[0] - 0.125, place[1]- 0.135, place[0] - 0.125, place[1] - 0.15, co_line_color[0], co_line_color[1], co_line_color[2], color);
									}else
									{
										sui_draw_2d_line_gl(place[0] - 0.125, place[1]- 0.2, place[0] - 0.125, place[1] - 0.21, co_line_color[0], co_line_color[1], co_line_color[2], color);
										sui_draw_2d_line_gl(place[0] - 0.125, place[1]- 0.145, place[0] - 0.125, place[1] - 0.15, co_line_color[0], co_line_color[1], co_line_color[2], color);
									}
								}
								if(drag != ~0u)
								{
									sui_draw_2d_line_gl(input->pointer_x, place[1]- 0.2, input->pointer_x, place[1] - 0.22, co_line_color[0], co_line_color[1], co_line_color[2], color);
									sui_draw_2d_line_gl(input->pointer_x, place[1]- 0.135, input->pointer_x, place[1] - 0.15, co_line_color[0], co_line_color[1], co_line_color[2], color);
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
													sui_draw_2d_line_gl(last, -0.2, last, -0.21, co_line_color[0], co_line_color[1], co_line_color[2], color);
													sui_draw_2d_line_gl(last, -0.15, last, -0.145, co_line_color[0], co_line_color[1], co_line_color[2], color);
												}else
												{
													sui_draw_2d_line_gl(last, -0.2, last, -0.22, co_line_color[0], co_line_color[1], co_line_color[2], color);
													sui_draw_2d_line_gl(last, -0.15, last, -0.135, co_line_color[0], co_line_color[1], co_line_color[2], color);
												}
											}
											sui_set_color_array_gl(col_range, 4, 3);
											sui_draw_gl(GL_QUADS, vertex, 4, 2, 0, 0, 0, 1);
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
											}else if(j != 0 && frag->ramp.point_count < 20)
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
									if(f.ramp.point_count > 2 && ((segment == j && co_w_close_button(input, place[0] - 0.15 + ((frag->ramp.ramp[j].pos - start) / (end - start)) * 0.25, place[1] - 0.12, color))
										|| (segment != j && co_w_close_button(input, place[0] - 0.15 + ((frag->ramp.ramp[j].pos - start) / (end - start)) * 0.25, place[1] - 0.13, color))))
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
									if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.25, 0, 0.12, SUI_T_SIZE, &f.ramp.ramp[segment].pos, &frag->ramp.ramp[segment].red, co_line_color[0], co_line_color[1], co_line_color[2], color))
										verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_RAMP, &f);
									if(co_w_slider(input, place[0], place[1] - 0.25, 0.12, &frag->ramp.ramp[segment].pos, 0, 1, color))
									{
										verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_RAMP, frag);
										frag->ramp.ramp[segment].pos = f.ramp.ramp[segment].pos;
									}


									if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.3, 0, 0.12, SUI_T_SIZE, &f.ramp.ramp[segment].red, &frag->ramp.ramp[segment].red, co_line_color[0], co_line_color[1], co_line_color[2], color))
										verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_RAMP, &f);
									if(co_w_slider(input, place[0], place[1] - 0.3, 0.12, &frag->ramp.ramp[segment].red, 0, 1, color))
									{
										verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_RAMP, frag);
										frag->ramp.ramp[segment].red = f.ramp.ramp[segment].red;
									}

									if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.35, 0, 0.12, SUI_T_SIZE, &f.ramp.ramp[segment].green, &frag->ramp.ramp[segment].green, co_line_color[0], co_line_color[1], co_line_color[2], color))
										verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_RAMP, &f);
									if(co_w_slider(input, place[0], place[1] - 0.35, 0.12, &frag->ramp.ramp[segment].green, 0, 1, color))
									{
										verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_RAMP, frag);
										frag->ramp.ramp[segment].green = f.ramp.ramp[segment].green;
									}

									if(sui_type_number_double(input, place[0] - 0.12, place[1] - 0.4, 0, 0.12, SUI_T_SIZE, &f.ramp.ramp[segment].blue, &frag->ramp.ramp[segment].blue, co_line_color[0], co_line_color[1], co_line_color[2], color))
										verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_RAMP, &f);
									if(co_w_slider(input, place[0], place[1] - 0.4, 0.12, &frag->ramp.ramp[segment].blue, 0, 1, color))
									{
										verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_RAMP, frag);
										frag->ramp.ramp[segment].blue = f.ramp.ramp[segment].blue;
									}
								}
							}else if(input->mode == BAM_DRAW)
								co_draw_lable(mat_pos, y, color_light, "Ramp");
							if(input->mode == BAM_DRAW)
								glPopMatrix();
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
						if(input->mode == BAM_DRAW)
							glPopMatrix();
					}
					case VN_M_FT_ALTERNATIVE :
						{	
							place[0] = mat_pos->pos[0];
							place[1] = mat_pos->pos[1] + y;
							co_vng_fragment(place[0], place[1], 0.20, color);
							co_draw_material_texture(mat_pos, place[0] + (-1 + rot_tree * rot_tree) * 3 / mat_pos->size, place[1] - 0.20);
							if(!co_material_click_test(input, mat_pos, i, place[0], place[1], 0.2, &move))
								co_draw_lable(mat_pos, y, color_light, "Alternative");
							if(input->mode == BAM_DRAW)
								glPopMatrix();
							if(handle_link(input, node, &frag->alternative.alt_a, "A", i, -30, y, expand, color, move))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_ALTERNATIVE, frag);
							if(handle_link(input, node, &frag->alternative.alt_b, "B", i, 0, y, expand, color, move))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_ALTERNATIVE, frag);
							*frag = f;
						}							
						break;
					case VN_M_FT_OUTPUT :
						{	
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
							if(input->mode == BAM_DRAW)
								glPopMatrix();
							if(handle_link(input, node, &frag->output.front, "Front", i, -20, y, expand, color, move))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_OUTPUT, frag);
							if(handle_link(input, node, &frag->output.back, "Back", i, 20, y, expand, color, move))
								verse_send_m_fragment_create(e_ns_get_node_id(node), i, VN_M_FT_OUTPUT, frag);
							*frag = f;
						}
						break;
				}
				if(mat_pos->expand && mat_pos->size > 0.999)
					if(co_w_close_button(input, mat_pos->pos[0] + 0.1, mat_pos->pos[1] - 0.015 + y, color))
						verse_send_m_fragment_destroy(e_ns_get_node_id(node), i);

				if(input->mode == BAM_DRAW)
				{
					sui_draw_text(mat_pos->pos[0] - 0.12, mat_pos->pos[1] - 0.035 + y, SUI_T_SIZE, SUI_T_SPACE, material_type_names[type], co_line_color[0], co_line_color[1], co_line_color[2], color_light);  
				}
			}
		}
		if(sw_text_button(input, -0.3, y - 0.05, 0, SUI_T_SIZE, SUI_T_SPACE, "Create new Fragment", co_line_color[0], co_line_color[1], co_line_color[2], color))
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
			if(co_background_color[0] + co_background_color[1] + co_background_color[2] > 1.5)
				output = sui_draw_popup(input, -0.2, y - 0.05, e, VN_M_FT_OUTPUT + 1, 0, 1.0);
			else
				output = sui_draw_popup(input, -0.2, y - 0.05, e, VN_M_FT_OUTPUT + 1, 0, 0.0);
			if(output <= VN_M_FT_OUTPUT)
			{
				if(output == VN_M_FT_LIGHT)
					clear_new_frag(&f, output, -1, -1, VN_M_LIGHT_DIRECT_AND_AMBIENT);
				else if(output == VN_M_FT_BLENDER)
					clear_new_frag(&f, output, -1, -1, VN_M_BLEND_MULTIPLY);
				else
					clear_new_frag(&f, output, -1, -1, 0);
				frag_placement_id[0] = e_nsm_find_empty_slot(node, 0);
				frag_placement_x[0] = input->pointer_x;
				frag_placement_y[0] = input->pointer_y - y;
				verse_send_m_fragment_create(e_ns_get_node_id(node), frag_placement_id[0], output, &f);
			}
		}
		if(input->last_mouse_button[0] == TRUE && input->mouse_button[0] == FALSE)
			create = FALSE;
		betray_get_key_up_down(&c_now, &c_last, 'c');
		if(c_now && !c_last)
			co_place_all_fragments_redo(node);
	}

	if(input->mode == BAM_DRAW)
	{
		glPopMatrix();
	}
	glPopMatrix();
	return co_handle_return(input);
}
