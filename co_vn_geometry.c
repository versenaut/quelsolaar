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
extern float co_background_color[3];
extern float co_line_color[3];

typedef struct{
	float *tri;
	float *quad;
	uint tri_count;
	uint quad_count;
	uint version;
	float scale;
	float center[3];
}PeGeometry;

void co_geometry_destroy(PeGeometry *g)
{
	if(g != NULL)
	{
		if(g->tri != NULL)
			free(g->tri);
		if(g->quad != NULL)
			free(g->quad);
		free(g);
	}
}


PeGeometry *co_geometry_draw(ENode *node, PeGeometry *g, boolean fill, boolean scale, float red, float green, float blue, float alpha)
{
	egreal *vertex;
	uint i, *ref, ref_length, length;

	if(g == NULL)
	{
		g = malloc(sizeof *g);
		g->tri = NULL;
		g->quad = NULL;
		g->version = 0;
		g->tri_count = 0;
		g->quad_count = 0;
	}
	if(g->version != e_ns_get_node_version_data(node))
	{
		float f, range[6] = { 0.0 };

		g->version = e_ns_get_node_version_data(node);
		vertex = e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node, 0));
		ref = e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node, 1));
		length = e_nsg_get_vertex_length(node);
		ref_length = e_nsg_get_polygon_length(node);
		g->quad_count = 0;
		g->tri_count = 0;
		if(g->tri != NULL)
			free(g->tri);
		if(g->quad != NULL)
			free(g->quad);
		for(i = 0; i < ref_length; i++)
		{
			if(ref[i * 4] < length && ref[i * 4 + 1] < length && ref[i * 4 + 2] < length && vertex[ref[i * 4] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 1] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 2] * 3] != V_REAL64_MAX)
			{
				if(ref[i * 4 + 3] < length && vertex[ref[i * 4 + 3] * 3] != V_REAL64_MAX)
					g->quad_count++;
				else
					g->tri_count++;
			}
		}
		if(g->tri_count != 0)
			g->tri = malloc((sizeof *g->tri) * 3 * 3 * g->tri_count);
		else
			g->tri = NULL;
		if(g->quad_count != 0)
			g->quad = malloc((sizeof *g->quad) * 4 * 3 * g->quad_count);
		else
			g->quad = NULL;
		g->quad_count = 0;
		g->tri_count = 0;
		for(i = 0; i < ref_length; i++)
		{
			if(ref[i * 4] < length && ref[i * 4 + 1] < length && ref[i * 4 + 2] < length && vertex[ref[i * 4] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 1] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 2] * 3] != V_REAL64_MAX)
			{
				if(ref[i * 4 + 3] < length && vertex[ref[i * 4 + 3] * 3] != V_REAL64_MAX)
				{
					g->quad[g->quad_count * 12 + 0] = vertex[ref[i * 4 + 0] * 3 + 0];
					g->quad[g->quad_count * 12 + 1] = vertex[ref[i * 4 + 0] * 3 + 1];
					g->quad[g->quad_count * 12 + 2] = vertex[ref[i * 4 + 0] * 3 + 2];
					g->quad[g->quad_count * 12 + 3] = vertex[ref[i * 4 + 1] * 3 + 0];
					g->quad[g->quad_count * 12 + 4] = vertex[ref[i * 4 + 1] * 3 + 1];
					g->quad[g->quad_count * 12 + 5] = vertex[ref[i * 4 + 1] * 3 + 2];
					g->quad[g->quad_count * 12 + 6] = vertex[ref[i * 4 + 2] * 3 + 0];
					g->quad[g->quad_count * 12 + 7] = vertex[ref[i * 4 + 2] * 3 + 1];
					g->quad[g->quad_count * 12 + 8] = vertex[ref[i * 4 + 2] * 3 + 2];
					g->quad[g->quad_count * 12 + 9] = vertex[ref[i * 4 + 3] * 3 + 0];
					g->quad[g->quad_count * 12 + 10] = vertex[ref[i * 4 + 3] * 3 + 1];
					g->quad[g->quad_count * 12 + 11] = vertex[ref[i * 4 + 3] * 3 + 2];
					g->quad_count++;
				}
				else
				{
					g->tri[g->tri_count * 9 + 0] = vertex[ref[i * 4 + 0] * 3 + 0];
					g->tri[g->tri_count * 9 + 1] = vertex[ref[i * 4 + 0] * 3 + 1];
					g->tri[g->tri_count * 9 + 2] = vertex[ref[i * 4 + 0] * 3 + 2];
					g->tri[g->tri_count * 9 + 3] = vertex[ref[i * 4 + 1] * 3 + 0];
					g->tri[g->tri_count * 9 + 4] = vertex[ref[i * 4 + 1] * 3 + 1];
					g->tri[g->tri_count * 9 + 5] = vertex[ref[i * 4 + 1] * 3 + 2];
					g->tri[g->tri_count * 9 + 6] = vertex[ref[i * 4 + 2] * 3 + 0];
					g->tri[g->tri_count * 9 + 7] = vertex[ref[i * 4 + 2] * 3 + 1];
					g->tri[g->tri_count * 9 + 8] = vertex[ref[i * 4 + 2] * 3 + 2];
					g->tri_count++;
				}
			}
		}
		if(g->tri_count != 0)
		{
			range[0] = g->tri[0];
			range[1] = g->tri[0];
			range[2] = g->tri[1];
			range[3] = g->tri[1];
			range[4] = g->tri[2];
			range[5] = g->tri[2];
		}
		else if(g->quad_count != 0)
		{
			range[0] = g->quad[0];
			range[1] = g->quad[0];
			range[2] = g->quad[1];
			range[3] = g->quad[1];
			range[4] = g->quad[2];
			range[5] = g->quad[2];
		}

		for(i = 0; i < g->tri_count * 3 * 3; i += 3)
		{
			if(g->tri[i] > range[0])
				range[0] = g->tri[i];
			if(g->tri[i] < range[1])
				range[1] = g->tri[i];
			if(g->tri[i + 1] > range[2])
				range[2] = g->tri[i + 1];
			if(g->tri[i + 1] < range[3])
				range[3] = g->tri[i + 1];
			if(g->tri[i + 2] > range[4])
				range[4] = g->tri[i + 2];
			if(g->tri[i + 2] < range[5])
				range[5] = g->tri[i + 2];
		}
		for(i = 0; i < g->quad_count * 4 * 3; i += 3)
		{
			if(g->quad[i] > range[0])
				range[0] = g->quad[i];
			if(g->quad[i] < range[1])
				range[1] = g->quad[i];
			if(g->quad[i + 1] > range[2])
				range[2] = g->quad[i + 1];
			if(g->quad[i + 1] < range[3])
				range[3] = g->quad[i + 1];
			if(g->quad[i + 2] > range[4])
				range[4] = g->quad[i + 2];
			if(g->quad[i + 2] < range[5])
				range[5] = g->quad[i + 2];
		}
		f = range[0] - range[1];
		if(f < range[2] - range[3])
			f = range[2] - range[3];
		if(f < range[4] - range[5])
			f = range[4] - range[5];
		g->scale = 1 / f;
		g->center[0] = (range[0] + range[1]) / 2.0;
		g->center[1] = (range[2] + range[3]) / 2.0;
		g->center[2] = (range[4] + range[5]) / 2.0;
	}
	if(scale)
	{
		glPushMatrix();

		glScaled(g->scale, g->scale, g->scale);
		glTranslated(-g->center[0], -g->center[1], -g->center[2]);
	}
	if(!fill)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if(g->tri_count != 0)
		sui_draw_gl(GL_TRIANGLES, g->tri, g->tri_count * 3, 3, red, green, blue, alpha);
	if(g->quad_count != 0)
		sui_draw_gl(GL_QUADS, g->quad, g->quad_count * 4,  3, red, green, blue, alpha);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if(scale)
		glPopMatrix();	
	return g;
}


static uint change_g_node_id = 0;

void rename_g_layer_func(void *user, char *text)
{
	EBitLayer *layer;
	ENode *node;
	uint16 bone_id;
	if((node = e_ns_get_node(0, change_g_node_id)) != NULL)
	{
		for(layer = e_nsg_get_layer_next(node, 0); layer != NULL && layer != user ; layer = e_nsg_get_layer_next(node, e_nsg_get_layer_id(layer) + 1));
		if(layer == user)
			verse_send_g_layer_create(change_g_node_id, e_nsg_get_layer_id(layer), text, e_nsg_get_layer_type(layer), 0, 0);

		for(bone_id = e_nsg_get_bone_next(node, 0); bone_id != (uint16)-1; bone_id = e_nsg_get_bone_next(node, bone_id + 1))
		{
			double 		pos[3];
			e_nsg_get_bone_pos64(node, bone_id, pos);

			if(user == e_nsg_get_bone_weight(node, bone_id))
				verse_send_g_bone_create(change_g_node_id, bone_id, text, e_nsg_get_bone_reference(node, bone_id), e_nsg_get_bone_parent(node, bone_id), pos[0], pos[1], pos[2],
							 e_nsg_get_bone_pos_label(node, bone_id), e_nsg_get_bone_rot_label(node, bone_id), e_nsg_get_bone_scale_label(node, bone_id));
			else if(user == e_nsg_get_bone_reference(node, bone_id))
				verse_send_g_bone_create(change_g_node_id, bone_id, e_nsg_get_bone_weight(node, bone_id), text, e_nsg_get_bone_parent(node, bone_id), pos[0], pos[1], pos[2],
							 e_nsg_get_bone_pos_label(node, bone_id), e_nsg_get_bone_rot_label(node, bone_id), e_nsg_get_bone_scale_label(node, bone_id));
			else if(user == e_nsg_get_bone_pos_label(node, bone_id))
				verse_send_g_bone_create(change_g_node_id, bone_id, e_nsg_get_bone_weight(node, bone_id), e_nsg_get_bone_reference(node, bone_id),
							 e_nsg_get_bone_parent(node, bone_id), pos[0], pos[1], pos[2],
							 text, e_nsg_get_bone_rot_label(node, bone_id), e_nsg_get_bone_scale_label(node, bone_id));
			else if(user == e_nsg_get_bone_rot_label(node, bone_id))
				verse_send_g_bone_create(change_g_node_id, bone_id, e_nsg_get_bone_weight(node, bone_id), e_nsg_get_bone_reference(node, bone_id),
							 e_nsg_get_bone_parent(node, bone_id), pos[0], pos[1], pos[2],
							 e_nsg_get_bone_pos_label(node, bone_id), text, e_nsg_get_bone_scale_label(node, bone_id));
			else if(user == e_nsg_get_bone_scale_label(node, bone_id))
				verse_send_g_bone_create(change_g_node_id, bone_id, e_nsg_get_bone_weight(node, bone_id), e_nsg_get_bone_reference(node, bone_id),
							 e_nsg_get_bone_parent(node, bone_id), pos[0], pos[1], pos[2],
							 e_nsg_get_bone_pos_label(node, bone_id), e_nsg_get_bone_rot_label(node, bone_id), text);
		}
	}
}

extern float co_handle_node_head(BInputState *input, ENode *node, boolean reset);

boolean co_handle_geometry(BInputState *input, ENode *node)
{
	float y, color, color_light, pre_expander;
	static float rot_layer = 0, rot_crease = 0, rot_bone = 0;
	static boolean show_layer = TRUE, show_crease = TRUE, show_bone = TRUE;

	y = co_handle_node_head(input, node, change_g_node_id != e_ns_get_node_id(node));
	change_g_node_id = e_ns_get_node_id(node);

	co_vng_divider(input, 0.2, y, &rot_layer, &color, &color_light, &show_layer, "Layers");

	pre_expander = y;
	if(rot_layer > 0.001)
	{
		EGeoLayer *layer;
		y -= 0.05;
		if(sw_text_button(input, -0.27, y, 0, SUI_T_SIZE, SUI_T_SPACE, "Create new Layer", co_line_color[0], co_line_color[1], co_line_color[2], color))
		{
			EGeoLayer *l;
			uint i;
			char nr[32];
			i = 0;
			for(l = e_nsg_get_layer_next(node, 0); l != NULL ; l = e_nsg_get_layer_next(node, e_nsg_get_layer_id(l) + 1))
				i++;
			sprintf(nr, "layer_%u", i);
			verse_send_g_layer_create(change_g_node_id, -1, nr, VN_G_LAYER_VERTEX_XYZ, 0, 0);
		}
		if(sw_text_button(input, 0, y, 0, SUI_T_SIZE, SUI_T_SPACE, "Vertex Color", co_line_color[0], co_line_color[1], co_line_color[2], color))
		{
			if(e_nsg_get_layer_by_name(node, "color_r") == NULL)
				verse_send_g_layer_create(change_g_node_id, -1, "color_r", VN_G_LAYER_VERTEX_REAL, 0, 0);
			if(e_nsg_get_layer_by_name(node, "color_g") == NULL)
				verse_send_g_layer_create(change_g_node_id, -2, "color_g", VN_G_LAYER_VERTEX_REAL, 0, 0);
			if(e_nsg_get_layer_by_name(node, "color_b") == NULL)
				verse_send_g_layer_create(change_g_node_id, -3, "color_b", VN_G_LAYER_VERTEX_REAL, 0, 0);
		}
		if(sw_text_button(input, 0.27, y, 0, SUI_T_SIZE, SUI_T_SPACE, "Texture Mapping", co_line_color[0], co_line_color[1], co_line_color[2], color))
		{
			if(e_nsg_get_layer_by_name(node, "map_u") == NULL)
				verse_send_g_layer_create(change_g_node_id, -1, "map_u", VN_G_LAYER_POLYGON_CORNER_REAL, 0, 0);
			if(e_nsg_get_layer_by_name(node, "map_v") == NULL)
				verse_send_g_layer_create(change_g_node_id, -2, "map_v", VN_G_LAYER_POLYGON_CORNER_REAL, 0, 0);
		}
		if(sw_text_button(input, 0.54, y, 0, SUI_T_SIZE, SUI_T_SPACE, "Vertex Select", co_line_color[0], co_line_color[1], co_line_color[2], color))
		{
			if(e_nsg_get_layer_by_name(node, "select") == NULL)
				verse_send_g_layer_create(change_g_node_id, -1, "select", VN_G_LAYER_VERTEX_REAL, 0, 0);
		}
		y -= 0.05;
		for(layer = e_nsg_get_layer_next(node, 0); layer != NULL ; layer = e_nsg_get_layer_next(node, e_nsg_get_layer_id(layer) + 1))
		{
			char *names[] = {"VERTEX_XYZ", "VERTEX_UINT32", "VERTEX_REAL", "POLYGON_CORNER_UINT32", "POLYGON_CORNER_REAL", "POLYGON_FACE_UINT8", "POLYGON_FACE_UINT32", "POLYGON_FACE_REAL"};
			static uint popup = ~0;
			VNGLayerType type;

			sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Layer name:", co_line_color[0], co_line_color[1], co_line_color[2], color_light);  
			co_w_type_in(input, 0.15, y, 0.5, SUI_T_SIZE, e_nsg_get_layer_name(layer), 16, rename_g_layer_func, e_nsg_get_layer_name(layer), color, color_light);

			if(e_nsg_get_layer_id(layer) > 1 && co_w_close_button(input, 0.635, y, color))
				verse_send_g_layer_destroy(change_g_node_id, e_nsg_get_layer_id(layer));
			y -= 0.05;

			type = e_nsg_get_layer_type(layer);
			if(type > VN_G_LAYER_VERTEX_REAL)
				type -= VN_G_LAYER_POLYGON_CORNER_UINT32 - 1 - VN_G_LAYER_VERTEX_REAL;
			if(type > 7)
				type = 7;
			if((int) type < 0)
				type = 0;
			
			sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Layer type:", co_line_color[0], co_line_color[1], co_line_color[2], color_light);  
			if(sw_text_button(input, 0.15, y, 0, SUI_T_SIZE, SUI_T_SPACE, names[type], co_line_color[0], co_line_color[1], co_line_color[2], color))
				popup = e_nsg_get_layer_id(layer);

			if(popup == e_nsg_get_layer_id(layer))
			{
				SUIPUElement e[7];
				uint i, output;
				for(i = 0; i < 3; i++)
				{
					e[i].type = PU_T_ANGLE; 
					e[i].data.angle[0] = 45 + (float)i * 30;
					e[i].data.angle[1] = 45 + (float)(i + 1) * 30;
					e[i].text = names[i]; 
				}
				for(; i < 7; i++)
				{
					e[i].type = PU_T_ANGLE; 
					e[i].data.angle[0] = 180 + 45 + (float)(i - 3) * 22.5;
					e[i].data.angle[1] = 180 + 45 + (float)(i - 2) * 22.5;
					e[i].text = names[i]; 
				}
				if(co_background_color[0] + co_background_color[1] + co_background_color[2] > 1.5)
					output = sui_draw_popup(input, 0.15, y, e, 7, 0, 1.0);
				else
					output = sui_draw_popup(input, 0.15, y, e, 7, 0, 0.0);
				if(output != ~0u && output != type)
				{
					if(output > VN_G_LAYER_VERTEX_REAL)
						output += VN_G_LAYER_POLYGON_CORNER_UINT32 - VN_G_LAYER_VERTEX_REAL - 1; 
					verse_send_g_layer_create(e_ns_get_node_id(node), popup, e_nsg_get_layer_name(layer), output, 0, 0);
				}
			}
			if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == FALSE)
				popup = -1;
			y -= 0.05;
			sui_draw_rounded_square(-0.3, y + 0.125, 1, -0.09, co_line_color[0], co_line_color[1], co_line_color[2], color_light);
		}
	}
	glPopMatrix();
	y -= 0.05 + (y - pre_expander) * (1 - rot_layer);

	co_vng_divider(input, 0.2, y, &rot_crease, &color, &color_light, &show_crease, "Crease");
	pre_expander = y;
	if(rot_crease > 0.001)
	{
		y -= 0.05;
		sui_draw_text(-0.27, y, SUI_T_SIZE, SUI_T_SPACE, "vertex crease", co_line_color[0], co_line_color[1], co_line_color[2], color_light); 
		sui_draw_text(0.0, y - 0.05, SUI_T_SIZE, SUI_T_SPACE, "Vertex", co_line_color[0], co_line_color[1], co_line_color[2], color_light);  
/*		if(sui_type_number_double(input, 0.15, y - 0.05, 0, 0.15, SUI_T_SIZE, &transform[0], &transform[0], co_line_color[0], co_line_color[1], co_line_color[2], color))
			verse_send_o_transform_pos_real64(e_ns_get_node_id(node), 0, transform, NULL, NULL, NULL, 0);
		if(co_w_slider(input, 0.3, y - 0.05, 0.35, &transform[0], color, color, color))
			verse_send_o_transform_pos_real64(e_ns_get_node_id(node), 0, transform, NULL, NULL, NULL, 0);
*/
	}
	glPopMatrix();
	y -= 0.05 + (y - pre_expander) * (1 - rot_layer);

	co_vng_divider(input, 0.2, y, &rot_bone, &color, &color_light, &show_bone, "Bones");
	pre_expander = y;
	if(rot_bone > 0.001)
	{
		uint16 bone;
		y -= 0.05;
		if(sw_text_button(input, -0.27, y, 0, SUI_T_SIZE, SUI_T_SPACE, "Create new Bone", co_line_color[0], co_line_color[1], co_line_color[2], color))
		{
			uint i;
			char nr[32];
			i = 3;
			for(bone = e_nsg_get_bone_next(node, 0); bone != (uint16)-1; bone = e_nsg_get_bone_next(node, bone + 1))
				i++;
			sprintf(nr, "weight_%u", i);
			verse_send_g_bone_create(change_g_node_id, -1, nr, "reference", 0, 0, 0, 0, "bone_pos", "bone_rot", "bone_scale");
		}
		y -= 0.05;
		for(bone = e_nsg_get_bone_next(node, 0); bone != (uint16)-1; bone = e_nsg_get_bone_next(node, bone + 1))
		{
			double t[3];
			char *ref, *text[] = {"X", "Y", "Z", "X", "Y", "Z", "W"};
			uint i;
			uint32 parent;
			if(co_w_close_button(input, 0.645, y, color))
				verse_send_g_bone_destroy(change_g_node_id, bone);

			sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Bone Weight:", co_line_color[0], co_line_color[1], co_line_color[2], color_light);
			co_w_type_in(input, 0.15, y, 0.5, SUI_T_SIZE, e_nsg_get_bone_weight(node, bone), 16, rename_g_layer_func, e_nsg_get_bone_weight(node, bone), color, color_light);
			y -= 0.05;

			ref = e_nsg_get_bone_reference(node, bone);
			sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Bone Ref:", co_line_color[0], co_line_color[1], co_line_color[2], color_light);  
			co_w_type_in(input, 0.15, y, 0.5, SUI_T_SIZE, e_nsg_get_bone_reference(node, bone), 16, rename_g_layer_func, e_nsg_get_bone_reference(node, bone), color, color_light);
			y -= 0.05;

			sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Bone Pos Label:", co_line_color[0], co_line_color[1], co_line_color[2], color_light);
			co_w_type_in(input, 0.15, y, 0.5, SUI_T_SIZE, e_nsg_get_bone_pos_label(node, bone), 16, rename_g_layer_func, e_nsg_get_bone_pos_label(node, bone), color, color_light);
			y -= 0.05;

			sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, "Bone Ros Label:", co_line_color[0], co_line_color[1], co_line_color[2], color_light);  
			co_w_type_in(input, 0.15, y, 0.5, SUI_T_SIZE, e_nsg_get_bone_rot_label(node, bone), 16, rename_g_layer_func, e_nsg_get_bone_rot_label(node, bone), color, color_light);
			y -= 0.05;

			e_nsg_get_bone_pos64(node, bone, t);
			parent = e_nsg_get_bone_parent(node, bone);
			sui_draw_text(-0.25, y, SUI_T_SIZE, SUI_T_SPACE, "PARENT", co_line_color[0], co_line_color[1], co_line_color[2], color_light);
			if(sui_type_number_uint(input,0.15, y, 0, 0.15, SUI_T_SIZE, &parent, e_nsg_get_bone_weight(node, bone), color, color, color))
				verse_send_g_bone_create(change_g_node_id, bone, e_nsg_get_bone_weight(node, bone), ref, parent, t[0], t[1], t[2],
							 e_nsg_get_bone_rot_label(node, bone), e_nsg_get_bone_rot_label(node, bone), e_nsg_get_bone_scale_label(node, bone));
			y -= 0.05;
			sui_draw_text(-0.25, y, SUI_T_SIZE, SUI_T_SPACE, "POSITION", co_line_color[0], co_line_color[1], co_line_color[2], color_light);
			sui_draw_text(-0.25, y - 0.15, SUI_T_SIZE, SUI_T_SPACE, "ROTATION", co_line_color[0], co_line_color[1], co_line_color[2], color_light);

			for(i = 0; i < 3; i++)
			{	
				double	*edit;

				edit = t + i;
				sui_draw_text(0.0, y, SUI_T_SIZE, SUI_T_SPACE, text[i], co_line_color[0], co_line_color[1], co_line_color[2], color_light);  
				if(sui_type_number_double(input, 0.15, y, 0, 0.15, SUI_T_SIZE, edit, edit, co_line_color[0], co_line_color[1], co_line_color[2], color))
					verse_send_g_bone_create(change_g_node_id, bone, e_nsg_get_bone_weight(node, bone), ref, parent,
								 t[0], t[1], t[2],
								 e_nsg_get_bone_pos_label(node, bone),
								 e_nsg_get_bone_rot_label(node, bone),
								 e_nsg_get_bone_scale_label(node, bone));
				co_w_slider(input, 0.3, y, 0.35, edit, 0, 1, color);
				y -= 0.05;
			}
			sui_draw_rounded_square(-0.3, y + 0.525, 1, -0.49, co_line_color[0], co_line_color[1], co_line_color[2], color_light);
		}
	}
	glPopMatrix();
	y -= 0.05 + (y - pre_expander) * (1 - rot_layer);
	return co_handle_return(input);
}

