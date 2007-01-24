
#include "st_matrix_operations.h"
#include "enough.h"
#include "seduce.h"
#include "deceive.h"

typedef struct{
	float *tri;
	float *color;
	uint version;	
	uint tri_count;
	uint layer_version[3];
}ViewGeometry;

ViewGeometry *lp_geometry_update(ENode *node, ViewGeometry *g)
{
	egreal *vertex;
	uint i, *ref, ref_length, length;

	if(g == NULL)
	{
		g = malloc(sizeof *g);
		g->tri = NULL;
		g->color = NULL;
		g->version = 0;
		g->tri_count = 0;
	}
	if(g->version != e_ns_get_node_version_data(node))
	{
		g->version = e_ns_get_node_version_data(node);
		vertex = e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node, 0));
		ref = e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node, 1));
		length = e_nsg_get_vertex_length(node);
		ref_length = e_nsg_get_polygon_length(node);
		g->tri_count = 0;
		for(i = 0; i < ref_length; i++)
		{
			if(ref[i * 4] < length && ref[i * 4 + 1] < length && ref[i * 4 + 2] < length && vertex[ref[i * 4] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 1] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 2] * 3] != V_REAL64_MAX)
			{
				if(ref[i * 4 + 3] < length && vertex[ref[i * 4 + 3] * 3] != V_REAL64_MAX)
					g->tri_count += 2;
				else
					g->tri_count++;
			}
		}
		if(g->tri_count != 0)
		{
			if(g->tri != NULL)
				free(g->tri);
			g->tri = malloc((sizeof *g->tri) * 3 * 3 * g->tri_count);
			if(g->color != NULL)
				free(g->color);
			g->color = malloc((sizeof *g->color) * 4 * 3 * g->tri_count);
		}
		g->tri_count = 0;
		for(i = 0; i < ref_length; i++)
		{
			if(ref[i * 4] < length && ref[i * 4 + 1] < length && ref[i * 4 + 2] < length && vertex[ref[i * 4] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 1] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 2] * 3] != V_REAL64_MAX)
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
				if(ref[i * 4 + 3] < length && vertex[ref[i * 4 + 3] * 3] != V_REAL64_MAX)
				{
					g->tri[g->tri_count * 9 + 0] = vertex[ref[i * 4 + 0] * 3 + 0];
					g->tri[g->tri_count * 9 + 1] = vertex[ref[i * 4 + 0] * 3 + 1];
					g->tri[g->tri_count * 9 + 2] = vertex[ref[i * 4 + 0] * 3 + 2];
					g->tri[g->tri_count * 9 + 3] = vertex[ref[i * 4 + 2] * 3 + 0];
					g->tri[g->tri_count * 9 + 4] = vertex[ref[i * 4 + 2] * 3 + 1];
					g->tri[g->tri_count * 9 + 5] = vertex[ref[i * 4 + 2] * 3 + 2];
					g->tri[g->tri_count * 9 + 6] = vertex[ref[i * 4 + 3] * 3 + 0];
					g->tri[g->tri_count * 9 + 7] = vertex[ref[i * 4 + 3] * 3 + 1];
					g->tri[g->tri_count * 9 + 8] = vertex[ref[i * 4 + 3] * 3 + 2];
					g->tri_count++;
				}
			}
		}
	}
	return g;
}

extern VNGLayerType e_nsg_get_layer_type(EGeoLayer *layer);
extern uint			e_nsg_get_layer_id(EGeoLayer *layer);

void lp_compute_int_color(float *color, uint i)
{
	color[0] = get_rand(i++);
	color[1] = get_rand(i++);
	color[2] = get_rand(i);
}

float lp_vertex_alpha(egreal *select, uint vertex)
{
	if(select == NULL)
		return 1.0;
	if(select[vertex] > 0.1)
		return 1.0;
	else
		return 0.2;
}

float lp_quad_alpha(egreal *select, uint *ref)
{
	if(select == NULL)
		return 1.0;
	if(select[ref[0]] > 0.1 && select[ref[1]] > 0.1 && select[ref[2]] > 0.1 && select[ref[3]] > 0.1)
		return 1.0;
	else
		return 0.2;
}

float lp_tri_alpha(egreal *select, uint *ref)
{
	if(select == NULL)
		return 1.0;
	if(select[ref[0]] > 0.1 && select[ref[1]] > 0.1 && select[ref[2]] > 0.1)
		return 1.0;
	else
		return 0.2;
}

extern EGeoLayer *	e_nsg_get_layer_by_name(ENode *g_node, const char *name);
extern void *		e_nsg_get_layer_data(ENode *g_node, EGeoLayer *layer);

void lp_color_update(ENode *node, ViewGeometry *g, EGeoLayer *layer, uint slot)
{
	egreal *vertex, *real, *select = 0;
	float f, color[3];
	uint i, *ref, ref_length, length, *integer;
	uint8 *integer8;
	EGeoLayer *slayer;

	slayer = e_nsg_get_layer_by_name(node, "select");
	if(slayer != NULL)
		select = e_nsg_get_layer_data(node, slayer);

	if(g->layer_version[slot] != e_nsg_get_layer_version(layer))
	{
		g->layer_version[slot] = e_nsg_get_layer_version(layer);
		vertex = e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node, 0));
		ref = e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node, 1));
		length = e_nsg_get_vertex_length(node);
		ref_length = e_nsg_get_polygon_length(node);
		g->tri_count = 0;
		if(select == slayer)
			select = NULL;

		switch(e_nsg_get_layer_type(layer))
		{
			case VN_G_LAYER_VERTEX_XYZ :
			for(i = 0; i < ref_length; i++)
			{
				if(ref[i * 4] < length && ref[i * 4 + 1] < length &&
				   ref[i * 4 + 2] < length && vertex[ref[i * 4] * 3] != V_REAL64_MAX &&
				   vertex[ref[i * 4 + 1] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 2] * 3] != V_REAL64_MAX)
				{
					g->color[g->tri_count * 12 + 0] = vertex[ref[i * 4 + 0] * 3 + 0];
					g->color[g->tri_count * 12 + 1] = vertex[ref[i * 4 + 0] * 3 + 1];
					g->color[g->tri_count * 12 + 2] = vertex[ref[i * 4 + 0] * 3 + 2];
					g->color[g->tri_count * 12 + 3] = lp_vertex_alpha(select, ref[i * 4 + 0]);
					g->color[g->tri_count * 12 + 4] = vertex[ref[i * 4 + 1] * 3 + 0];
					g->color[g->tri_count * 12 + 5] = vertex[ref[i * 4 + 1] * 3 + 1];
					g->color[g->tri_count * 12 + 6] = vertex[ref[i * 4 + 1] * 3 + 2];
					g->color[g->tri_count * 12 + 7] = lp_vertex_alpha(select, ref[i * 4 + 1]);
					g->color[g->tri_count * 12 + 8] = vertex[ref[i * 4 + 2] * 3 + 0];
					g->color[g->tri_count * 12 + 9] = vertex[ref[i * 4 + 2] * 3 + 1];
					g->color[g->tri_count * 12 + 10] = vertex[ref[i * 4 + 2] * 3 + 2];
					g->color[g->tri_count * 12 + 11] = lp_vertex_alpha(select, ref[i * 4 + 2]);
					g->tri_count++;
					if(ref[i * 4 + 3] < length && vertex[ref[i * 4 + 3] * 3] != V_REAL64_MAX)
					{
						g->color[g->tri_count * 12 + 0] = vertex[ref[i * 4 + 0] * 3 + 0];
						g->color[g->tri_count * 12 + 1] = vertex[ref[i * 4 + 0] * 3 + 1];
						g->color[g->tri_count * 12 + 2] = vertex[ref[i * 4 + 0] * 3 + 2];
						g->color[g->tri_count * 12 + 3] = lp_vertex_alpha(select, ref[i * 4 + 0]);
						g->color[g->tri_count * 12 + 4] = vertex[ref[i * 4 + 2] * 3 + 0];
						g->color[g->tri_count * 12 + 5] = vertex[ref[i * 4 + 2] * 3 + 1];
						g->color[g->tri_count * 12 + 6] = vertex[ref[i * 4 + 2] * 3 + 2];
						g->color[g->tri_count * 12 + 7] = lp_vertex_alpha(select, ref[i * 4 + 2]);
						g->color[g->tri_count * 12 + 8] = vertex[ref[i * 4 + 3] * 3 + 0];
						g->color[g->tri_count * 12 + 9] = vertex[ref[i * 4 + 3] * 3 + 1];
						g->color[g->tri_count * 12 + 10] = vertex[ref[i * 4 + 3] * 3 + 2];
						g->color[g->tri_count * 12 + 11] = lp_vertex_alpha(select, ref[i * 4 + 3]);
						g->tri_count++;
					}
				}
			}
			break;
			case VN_G_LAYER_VERTEX_UINT32 :
			integer = e_nsg_get_layer_data(node, layer);
			for(i = 0; i < ref_length; i++)
			{
				if(ref[i * 4] < length && ref[i * 4 + 1] < length && ref[i * 4 + 2] < length && vertex[ref[i * 4] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 1] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 2] * 3] != V_REAL64_MAX)
				{
					lp_compute_int_color(color, integer[ref[i * 4 + 0]]);
					g->color[g->tri_count * 12 + 0] = color[0];
					g->color[g->tri_count * 12 + 1] = color[1];
					g->color[g->tri_count * 12 + 2] = color[2];
					g->color[g->tri_count * 12 + 3] = lp_vertex_alpha(select, ref[i * 4 + 0]);
					lp_compute_int_color(color, integer[ref[i * 4 + 1]]);
					g->color[g->tri_count * 12 + 4] = color[0];
					g->color[g->tri_count * 12 + 5] = color[1];
					g->color[g->tri_count * 12 + 6] = color[2];
					g->color[g->tri_count * 12 + 7] = lp_vertex_alpha(select, ref[i * 4 + 1]);
					lp_compute_int_color(color, integer[ref[i * 4 + 2]]);
					g->color[g->tri_count * 12 + 8] = color[0];
					g->color[g->tri_count * 12 + 9] = color[1];
					g->color[g->tri_count * 12 + 10] = color[2];
					g->color[g->tri_count * 12 + 11] = lp_vertex_alpha(select, ref[i * 4 + 2]);
					g->tri_count++;
					if(ref[i * 4 + 3] < length && vertex[ref[i * 4 + 3] * 3] != V_REAL64_MAX)
					{
						lp_compute_int_color(color, integer[ref[i * 4 + 0]]);
						g->color[g->tri_count * 12 + 0] = color[0];
						g->color[g->tri_count * 12 + 1] = color[1];
						g->color[g->tri_count * 12 + 2] = color[2];
						g->color[g->tri_count * 12 + 3] = lp_vertex_alpha(select, ref[i * 4 + 0]);
						lp_compute_int_color(color, integer[ref[i * 4 + 2]]);
						g->color[g->tri_count * 12 + 4] = color[0];
						g->color[g->tri_count * 12 + 5] = color[1];
						g->color[g->tri_count * 12 + 6] = color[2];
						g->color[g->tri_count * 12 + 7] = lp_vertex_alpha(select, ref[i * 4 + 2]);
						lp_compute_int_color(color, integer[ref[i * 4 + 3]]);
						g->color[g->tri_count * 12 + 8] = color[0];
						g->color[g->tri_count * 12 + 9] = color[1];
						g->color[g->tri_count * 12 + 10] = color[2];
						g->color[g->tri_count * 12 + 11] = lp_vertex_alpha(select, ref[i * 4 + 3]);
						g->tri_count++;
					}
				}
			}
			break;
			case VN_G_LAYER_VERTEX_REAL :
			real = e_nsg_get_layer_data(node, layer);
			for(i = 0; i < ref_length; i++)
			{
				if(ref[i * 4] < length && ref[i * 4 + 1] < length && ref[i * 4 + 2] < length && vertex[ref[i * 4] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 1] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 2] * 3] != V_REAL64_MAX)
				{
					g->color[g->tri_count * 12 + slot] = real[ref[i * 4 + 0]];
					g->color[g->tri_count * 12 + 3] = lp_vertex_alpha(select, ref[i * 4 + 0]);
					g->color[g->tri_count * 12 + 4 + slot] = real[ref[i * 4 + 1]];
					g->color[g->tri_count * 12 + 7] = lp_vertex_alpha(select, ref[i * 4 + 1]);
					g->color[g->tri_count * 12 + 8 + slot] = real[ref[i * 4 + 2]];
					g->color[g->tri_count * 12 + 11] = lp_vertex_alpha(select, ref[i * 4 + 2]);
	
					g->tri_count++;
					if(ref[i * 4 + 3] < length && vertex[ref[i * 4 + 3] * 3] != V_REAL64_MAX)
					{
						g->color[g->tri_count * 12 + slot] = real[ref[i * 4 + 0]];
						g->color[g->tri_count * 12 + 3] = lp_vertex_alpha(select, ref[i * 4 + 0]);
						g->color[g->tri_count * 12 + 4 + slot] = real[ref[i * 4 + 2]];
						g->color[g->tri_count * 12 + 7] = lp_vertex_alpha(select, ref[i * 4 + 2]);
						g->color[g->tri_count * 12 + 8 + slot] = real[ref[i * 4 + 3]];
						g->color[g->tri_count * 12 + 11] = lp_vertex_alpha(select, ref[i * 4 + 3]);
						g->tri_count++;
					}
				}
			}
			break;
			case VN_G_LAYER_POLYGON_CORNER_UINT32 :
			integer = e_nsg_get_layer_data(node, layer);
			for(i = 0; i < ref_length; i++)
			{
				if(ref[i * 4] < length && ref[i * 4 + 1] < length && ref[i * 4 + 2] < length && vertex[ref[i * 4] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 1] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 2] * 3] != V_REAL64_MAX)
				{
					if(ref[i * 4 + 3] < length && vertex[ref[i * 4 + 3] * 3] != V_REAL64_MAX)
						f = lp_quad_alpha(select, &ref[i * 4]);
					else
						f = lp_tri_alpha(select, &ref[i * 4]);
					lp_compute_int_color(color, integer[i * 4 + 0]);
					g->color[g->tri_count * 12 + 0] = color[0];
					g->color[g->tri_count * 12 + 1] = color[1];
					g->color[g->tri_count * 12 + 2] = color[2];
					g->color[g->tri_count * 12 + 3] = f;
					lp_compute_int_color(color, integer[i * 4 + 1]);
					g->color[g->tri_count * 12 + 4] = color[0];
					g->color[g->tri_count * 12 + 5] = color[1];
					g->color[g->tri_count * 12 + 6] = color[2];
					g->color[g->tri_count * 12 + 7] = f;
					lp_compute_int_color(color, integer[i * 4 + 2]);
					g->color[g->tri_count * 12 + 8] = color[0];
					g->color[g->tri_count * 12 + 9] = color[1];
					g->color[g->tri_count * 12 + 10] = color[2];
					g->color[g->tri_count * 12 + 11] = f;
					g->tri_count++;
					if(ref[i * 4 + 3] < length && vertex[ref[i * 4 + 3] * 3] != V_REAL64_MAX)
					{
						f = lp_quad_alpha(select, &ref[i * 4]);
						lp_compute_int_color(color, integer[i * 4 + 0]);
						g->color[g->tri_count * 12 + 0] = color[0];
						g->color[g->tri_count * 12 + 1] = color[1];
						g->color[g->tri_count * 12 + 2] = color[2];
						g->color[g->tri_count * 12 + 3] = f;
						lp_compute_int_color(color, integer[i * 4 + 2]);
						g->color[g->tri_count * 12 + 4] = color[0];
						g->color[g->tri_count * 12 + 5] = color[1];
						g->color[g->tri_count * 12 + 6] = color[2];
						g->color[g->tri_count * 12 + 7] = f;
						lp_compute_int_color(color, integer[i * 4 + 3]);
						g->color[g->tri_count * 12 + 8] = color[0];
						g->color[g->tri_count * 12 + 9] = color[1];
						g->color[g->tri_count * 12 + 10] = color[2];
						g->color[g->tri_count * 12 + 11] = f;
						g->tri_count++;
					}

				}
			}
			break;
			case VN_G_LAYER_POLYGON_CORNER_REAL :
			real = e_nsg_get_layer_data(node, layer);
			for(i = 0; i < ref_length; i++)
			{
				if(ref[i * 4] < length && ref[i * 4 + 1] < length && ref[i * 4 + 2] < length && vertex[ref[i * 4] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 1] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 2] * 3] != V_REAL64_MAX)
				{
					if(ref[i * 4 + 3] < length && vertex[ref[i * 4 + 3] * 3] != V_REAL64_MAX)
						f = lp_quad_alpha(select, &ref[i * 4]);
					else
						f = lp_tri_alpha(select, &ref[i * 4]);
					g->color[g->tri_count * 12 + slot] = real[i * 4 + 0];
					g->color[g->tri_count * 12 + 3] = f;
					g->color[g->tri_count * 12 + 4 + slot] = real[i * 4 + 1];
					g->color[g->tri_count * 12 + 7] = f;
					g->color[g->tri_count * 12 + 8 + slot] = real[i * 4 + 2];
					g->color[g->tri_count * 12 + 11] = f;
					g->tri_count++;
					if(ref[i * 4 + 3] < length && vertex[ref[i * 4 + 3] * 3] != V_REAL64_MAX)
					{
						g->color[g->tri_count * 12 + slot] = real[i * 4 + 0];
						g->color[g->tri_count * 12 + 3] = f;
						g->color[g->tri_count * 12 + 4 + slot] = real[i * 4 + 2];
						g->color[g->tri_count * 12 + 7] = f;
						g->color[g->tri_count * 12 + 8 + slot] = real[i * 4 + 3];
						g->color[g->tri_count * 12 + 11] = f;
						g->tri_count++;
					}
				}
			}
			break;
			case VN_G_LAYER_POLYGON_FACE_UINT8 :
			integer8 = e_nsg_get_layer_data(node, layer);
			for(i = 0; i < ref_length; i++)
			{
				if(ref[i * 4] < length && ref[i * 4 + 1] < length && ref[i * 4 + 2] < length && vertex[ref[i * 4] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 1] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 2] * 3] != V_REAL64_MAX)
				{
					lp_compute_int_color(color, integer8[i]);
					if(ref[i * 4 + 3] < length && vertex[ref[i * 4 + 3] * 3] != V_REAL64_MAX)
					{
						f = lp_quad_alpha(select, &ref[i * 4]);
						g->color[g->tri_count * 12 + 0] = color[0];
						g->color[g->tri_count * 12 + 1] = color[1];
						g->color[g->tri_count * 12 + 2] = color[2];
						g->color[g->tri_count * 12 + 3] = f;
						g->color[g->tri_count * 12 + 4] = color[0];
						g->color[g->tri_count * 12 + 5] = color[1];
						g->color[g->tri_count * 12 + 6] = color[2];
						g->color[g->tri_count * 12 + 7] = f;
						g->color[g->tri_count * 12 + 8] = color[0];
						g->color[g->tri_count * 12 + 9] = color[1];
						g->color[g->tri_count * 12 + 10] = color[2];
						g->color[g->tri_count * 12 + 11] = f;
						g->tri_count++;
					}else
						f = lp_tri_alpha(select, &ref[i * 4]);
					g->color[g->tri_count * 12 + 0] = color[0];
					g->color[g->tri_count * 12 + 1] = color[1];
					g->color[g->tri_count * 12 + 2] = color[2];
					g->color[g->tri_count * 12 + 3] = f;
					g->color[g->tri_count * 12 + 4] = color[0];
					g->color[g->tri_count * 12 + 5] = color[1];
					g->color[g->tri_count * 12 + 6] = color[2];
					g->color[g->tri_count * 12 + 7] = f;
					g->color[g->tri_count * 12 + 8] = color[0];
					g->color[g->tri_count * 12 + 9] = color[1];
					g->color[g->tri_count * 12 + 10] = color[2];
					g->color[g->tri_count * 12 + 11] = f;
					g->tri_count++;
				}
			}
			break;
			case VN_G_LAYER_POLYGON_FACE_UINT32 :
			integer = e_nsg_get_layer_data(node, layer);
			for(i = 0; i < ref_length; i++)
			{
				if(ref[i * 4] < length && ref[i * 4 + 1] < length && ref[i * 4 + 2] < length && vertex[ref[i * 4] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 1] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 2] * 3] != V_REAL64_MAX)
				{
					lp_compute_int_color(color, integer[i]);
					if(ref[i * 4 + 3] < length && vertex[ref[i * 4 + 3] * 3] != V_REAL64_MAX)
					{
						f = lp_quad_alpha(select, &ref[i * 4]);
						g->color[g->tri_count * 12 + 0] = color[0];
						g->color[g->tri_count * 12 + 1] = color[1];
						g->color[g->tri_count * 12 + 2] = color[2];
						g->color[g->tri_count * 12 + 3] = f;
						g->color[g->tri_count * 12 + 4] = color[0];
						g->color[g->tri_count * 12 + 5] = color[1];
						g->color[g->tri_count * 12 + 6] = color[2];
						g->color[g->tri_count * 12 + 7] = f;
						g->color[g->tri_count * 12 + 8] = color[0];
						g->color[g->tri_count * 12 + 9] = color[1];
						g->color[g->tri_count * 12 + 10] = color[2];
						g->color[g->tri_count * 12 + 11] = f;
						g->tri_count++;
					}else
						f = lp_tri_alpha(select, &ref[i * 4]);
					g->color[g->tri_count * 12 + 0] = color[0];
					g->color[g->tri_count * 12 + 1] = color[1];
					g->color[g->tri_count * 12 + 2] = color[2];
					g->color[g->tri_count * 12 + 3] = f;
					g->color[g->tri_count * 12 + 4] = color[0];
					g->color[g->tri_count * 12 + 5] = color[1];
					g->color[g->tri_count * 12 + 6] = color[2];
					g->color[g->tri_count * 12 + 7] = f;
					g->color[g->tri_count * 12 + 8] = color[0];
					g->color[g->tri_count * 12 + 9] = color[1];
					g->color[g->tri_count * 12 + 10] = color[2];
					g->color[g->tri_count * 12 + 11] = f;
					g->tri_count++;
				}
			}
			break;
			case VN_G_LAYER_POLYGON_FACE_REAL :
			real = e_nsg_get_layer_data(node, layer);
			for(i = 0; i < ref_length; i++)
			{
				if(ref[i * 4] < length && ref[i * 4 + 1] < length && ref[i * 4 + 2] < length && vertex[ref[i * 4] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 1] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 2] * 3] != V_REAL64_MAX)
				{
					if(ref[i * 4 + 3] < length && vertex[ref[i * 4 + 3] * 3] != V_REAL64_MAX)
					{
						f = lp_quad_alpha(select, &ref[i * 4]);
						g->color[g->tri_count * 12 + 0 + slot] = real[i];
						g->color[g->tri_count * 12 + 4 + slot] = real[i];
						g->color[g->tri_count * 12 + 8 + slot] = real[i];
						g->color[g->tri_count * 12 + 3] = f;
						g->color[g->tri_count * 12 + 7] = f;
						g->color[g->tri_count * 12 + 11] = f;
						g->tri_count++;
					}else
						f = lp_tri_alpha(select, &ref[i * 4]);
					g->color[g->tri_count * 12 + 0 + slot] = real[i];
					g->color[g->tri_count * 12 + 4 + slot] = real[i];
					g->color[g->tri_count * 12 + 8 + slot] = real[i];
					g->color[g->tri_count * 12 + 3] = f;
					g->color[g->tri_count * 12 + 7] = f;
					g->color[g->tri_count * 12 + 11] = f;
					g->tri_count++;

				}
			}
			break;
		}
	}
}

void lp_color_gray(ViewGeometry *g)
{
	uint i;
	if(g->layer_version[1] != g->layer_version[0] && g->layer_version[2] != g->layer_version[0])
		for(i = 0; i < g->tri_count * 3; i++)
			g->color[i * 4 + 2] = g->color[i * 4 + 1] = g->color[i * 4];
	g->layer_version[1] = g->layer_version[0];
	g->layer_version[2] = g->layer_version[0];
}

void lp_geometry_draw(ENode *node, EGeoLayer *red, EGeoLayer *green, EGeoLayer *blue)
{
	static ViewGeometry *g = NULL;
	VNGLayerType type = -1;

	if(node == NULL)
		return;

	g = lp_geometry_update(node, g);
	if(red != NULL)
	{
		lp_color_update(node, g, red, 0);
		type = e_nsg_get_layer_type(red);
	}
	if(red == green && red == blue && type != VN_G_LAYER_VERTEX_XYZ && type != VN_G_LAYER_VERTEX_UINT32 && type != VN_G_LAYER_POLYGON_CORNER_UINT32 && type != VN_G_LAYER_POLYGON_FACE_UINT8 && type != VN_G_LAYER_POLYGON_FACE_UINT32)
		lp_color_gray(g);
	else
	{
		if(green != NULL)
			lp_color_update(node, g, green, 1);
		if(blue != NULL)
			lp_color_update(node, g, blue, 2);
	}

	if(g->tri_count != 0)
	{
		sui_set_color_array_gl(g->color, g->tri_count * 3, 4);
		sui_draw_gl(GL_TRIANGLES, g->tri, g->tri_count * 3, 3, 1, 0, 1, 1.0f);
	}
}
