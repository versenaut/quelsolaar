
#include <math.h>
#include <string.h>

#include "st_matrix_operations.h"
#include "enough.h"
#include "seduce.h"
#include "deceive.h"

#include "lp_projection.h"

static double lp_brush_size;

void lp_brush_size_set(double size)
{
	lp_brush_size = size;
}

boolean lp_cull(egreal *v1, egreal *v2, egreal *v3)
{
	double a[3], b[3], c[3];

	p_get_projection_screen(a, v1[0], v1[1], v1[2]);
	p_get_projection_screen(b, v2[0], v2[1], v2[2]);
	p_get_projection_screen(c, v3[0], v3[1], v3[2]);

	b[0] -= a[0];
	b[1] -= a[1];
	c[0] -= a[0];
	c[1] -= a[1];

	return c[0] * b[1] - c[1] * b[0] > 0.0;
}

egreal lp_paint(BInputState *input, egreal *vertex)
{
	double output[3], f;
	p_get_projection_screen(output, vertex[0], vertex[1], vertex[2]);

	output[0] = (output[0] + input->pointer_x) / lp_brush_size;
	output[1] = (output[1] + input->pointer_y) / lp_brush_size;
	f = output[0] * output[0] + output[1] * output[1];
	if(f < 1.0)
	{
		f = sqrt(f);
		return 1.0 - f; 
	}
	return 0.0;
}

static double *lp_vertex_influence = NULL;
static double *lp_polygon_influence = NULL;
static double *lp_vertex_influence_back = NULL;
static double *lp_polygon_influence_back = NULL;

static void *lp_channels[3] = {NULL, NULL, NULL};
static egreal *lp_normal = NULL;

void lp_unlock_paint(void)
{
	if(lp_vertex_influence != NULL)
	{
		free(lp_vertex_influence);
		free(lp_vertex_influence_back);
	}
	if(lp_polygon_influence != NULL)
	{
		free(lp_polygon_influence);
		free(lp_polygon_influence_back);
	}
	if(lp_channels[0] != NULL)
		free(lp_channels[0]);
	if(lp_channels[1] != NULL)
		free(lp_channels[1]);
	if(lp_channels[2] != NULL)
		free(lp_channels[2]);
	if(lp_normal != NULL)
		free(lp_normal);
	lp_vertex_influence = NULL;
	lp_vertex_influence_back = NULL;
	lp_polygon_influence = NULL;
	lp_polygon_influence_back = NULL;
	lp_channels[0] = NULL;
	lp_channels[1] = NULL;
	lp_channels[2] = NULL;
	lp_normal = NULL;
}

void lp_normal_create(ENode *node)
{
	egreal *vertex, r, x, y, z, x2, y2, z2, x3, y3, z3;
	uint i, j, *ref, ref_length, length;

	vertex = e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node, 0));
	ref = e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node, 1));
	length = e_nsg_get_vertex_length(node);
	ref_length = e_nsg_get_polygon_length(node);
	
	lp_normal = malloc((sizeof *lp_normal) * length * 3);
	for(i = 0; i < length * 3; i++)
		lp_normal[i] = 0.00001;
	for(i = 0; i < ref_length; i++)
	{
		if(ref[i * 4] < length && ref[i * 4 + 1] < length && ref[i * 4 + 2] < length && vertex[ref[i * 4] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 1] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 2] * 3] != V_REAL64_MAX)
		{
			x2 = vertex[ref[i * 4 + 0] * 3 + 0] - vertex[ref[i * 4 + 1] * 3 + 0];
			y2 = vertex[ref[i * 4 + 0] * 3 + 1] - vertex[ref[i * 4 + 1] * 3 + 1];
			z2 = vertex[ref[i * 4 + 0] * 3 + 2] - vertex[ref[i * 4 + 1] * 3 + 2];
			r = sqrt(x2 * x2 + y2 * y2 + z2 * z2);
			x2 = x2 / r;
			y2 = y2 / r;
			z2 = z2 / r;
			x3 = vertex[ref[i * 4 + 0] * 3 + 0] - vertex[ref[i * 4 + 2] * 3 + 0];
			y3 = vertex[ref[i * 4 + 0] * 3 + 1] - vertex[ref[i * 4 + 2] * 3 + 1];
			z3 = vertex[ref[i * 4 + 0] * 3 + 2] - vertex[ref[i * 4 + 2] * 3 + 2];
			r = sqrt(x3 * x3 + y3 * y3 + z3 * z3);
			x3 = x3 / r;
			y3 = y3 / r;
			z3 = z3 / r;
			x = y2 * z3 - z2 * y3;
			y = z2 * x3 - x2 * z3;
			z = x2 * y3 - y2 * x3;
			r = sqrt(x * x + y * y + z * z);
			x = x / r;
			y = y / r;
			z = z / r;
			for(j = 0; j < 4 && ref[i * 4 + j] < length; j++)
			{
				lp_normal[ref[i * 4 + j] * 3 + 0] -= x;
				lp_normal[ref[i * 4 + j] * 3 + 1] -= y;
				lp_normal[ref[i * 4 + j] * 3 + 2] -= z;
			}
		}
	}
	for(i = 0; i < length * 3; i += 3)
	{
		r = sqrt(lp_normal[i] * lp_normal[i] + lp_normal[i + 1] * lp_normal[i + 1] + lp_normal[i + 2] * lp_normal[i + 2]);
		lp_normal[i] /= r;
		lp_normal[i + 1] /= r;
		lp_normal[i + 2] /= r;
	}
}

void lp_lock_paint(ENode *node, EGeoLayer *red, EGeoLayer *green, EGeoLayer *blue)
{
	EGeoLayer *layers[3];
	boolean v = FALSE, p = FALSE;
	egreal *real;
	uint i, j, ref_length, length, *integer;
	uint8 *integer8;

	length = e_nsg_get_vertex_length(node);
	ref_length = e_nsg_get_polygon_length(node);
	layers[0] = red;
	layers[1] = green;
	layers[2] = blue;

	for(i = 0; i < 3; i++)
	{
		if(layers[i] != NULL)
		{
			switch(e_nsg_get_layer_type(layers[i]))
			{
			case VN_G_LAYER_VERTEX_XYZ :
				lp_normal_create(node);
				real = e_nsg_get_layer_data(node, layers[i]);
				lp_channels[i] = malloc(sizeof(egreal) * 3 * length);
				for(j = 0; j < length * 3; j++)
					((egreal *)lp_channels[i])[j] = real[j];
				v = TRUE;
				break;
			case VN_G_LAYER_VERTEX_UINT32 :
				integer = e_nsg_get_layer_data(node, layers[i]);
				lp_channels[i] = malloc(sizeof(uint) * length);
				for(j = 0; j < length; j++)
					((uint *)lp_channels[i])[j] = integer[j];
				v = TRUE;
				break;
			case VN_G_LAYER_VERTEX_REAL :
				real = e_nsg_get_layer_data(node, layers[i]);
				lp_channels[i] = malloc(sizeof(egreal) * length);
				for(j = 0; j < length; j++)
					((egreal *)lp_channels[i])[j] = real[j];
				v = TRUE;
				break;
			case VN_G_LAYER_POLYGON_CORNER_UINT32 :
				integer = e_nsg_get_layer_data(node, layers[i]);
				lp_channels[i] = malloc(sizeof(uint32) * 4 * ref_length);
				for(j = 0; j < ref_length * 4; j++)
					((uint *)lp_channels[i])[j] = integer[j];
				v = TRUE;
				break;
			case VN_G_LAYER_POLYGON_CORNER_REAL :
				real = e_nsg_get_layer_data(node, layers[i]);
				lp_channels[i] = malloc(sizeof(egreal) * 4 * ref_length);
				for(j = 0; j < ref_length * 4; j++)
					((egreal *)lp_channels[i])[j] = real[j];
				v = TRUE;
				break;
			case VN_G_LAYER_POLYGON_FACE_UINT8 :
				integer8 = e_nsg_get_layer_data(node, layers[i]);
				lp_channels[i] = malloc(sizeof(uint8) * ref_length);
				for(j = 0; j < ref_length; j++)
					((uint8 *)lp_channels[i])[j] = integer8[j];
				p = TRUE;
				break;
			case VN_G_LAYER_POLYGON_FACE_UINT32 :
				integer = e_nsg_get_layer_data(node, layers[i]);
				lp_channels[i] = malloc(sizeof(uint32) * ref_length);
				for(j = 0; j < ref_length; j++)
					((uint *)lp_channels[i])[j] = integer[j];
				p = TRUE;
				break;
			case VN_G_LAYER_POLYGON_FACE_REAL :
				real = e_nsg_get_layer_data(node, layers[i]);
				lp_channels[i] = malloc(sizeof(egreal) * ref_length);
				for(j = 0; j < ref_length; j++)
					((egreal *)lp_channels[i])[j] = real[j];
				p = TRUE;
				break;
			}
		}
	}
	if(v)
	{
		lp_vertex_influence = malloc((sizeof *lp_vertex_influence) * length);
		lp_vertex_influence_back = malloc((sizeof *lp_vertex_influence) * length);
		for(i = 0; i < length; i++)
			lp_vertex_influence[i] = lp_vertex_influence_back[i] = 0;
	}
	if(p)
	{
		lp_polygon_influence = malloc((sizeof *lp_polygon_influence) * ref_length);
		lp_polygon_influence_back = malloc((sizeof *lp_polygon_influence) * ref_length);
		for(i = 0; i < ref_length; i++)
			lp_polygon_influence[i] = lp_polygon_influence_back[i] = 0;
	}
}

void lp_color_paint(BInputState *input, ENode *node)
{
	egreal *vertex, f;
	double pos[3];
	uint i, *ref, ref_length, length;

	vertex = e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node, 0));
	ref = e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node, 1));
	length = e_nsg_get_vertex_length(node);
	ref_length = e_nsg_get_polygon_length(node);

	if(lp_vertex_influence != NULL)
	{
		for(i = 0; i < length; i++)
		{
		//	lp_vertex_influence_back[i] = lp_vertex_influence[i];
			if(vertex[i * 3] != V_REAL64_MAX && lp_vertex_influence[i] < 1.0)
			{
				f = lp_paint(input, &vertex[i * 3]);
				if(f > lp_vertex_influence[i])
				{					
					lp_vertex_influence[i] = f;
					if(lp_vertex_influence[i] > 1)
						lp_vertex_influence[i] = 1;
				}
			}
		}
	}
	if(lp_polygon_influence != NULL)
	{
		for(i = 0; i < ref_length; i++)
		{
		//	lp_polygon_influence_back[i] = lp_polygon_influence[i];
			if(ref[i * 4] < length && ref[i * 4 + 1] < length && ref[i * 4 + 2] < length && vertex[ref[i * 4] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 1] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 2] * 3] != V_REAL64_MAX)
			{
				if(lp_cull(&vertex[ref[i * 4] * 3], &vertex[ref[i * 4 + 1] * 3], &vertex[ref[i * 4 + 2] * 3]))
				{
					pos[0] = vertex[ref[i * 4] * 3];
					pos[1] = vertex[ref[i * 4] * 3 + 1];
					pos[2] = vertex[ref[i * 4] * 3 + 2];
					pos[0] += vertex[ref[i * 4 + 1] * 3];
					pos[1] += vertex[ref[i * 4 + 1] * 3 + 1];
					pos[2] += vertex[ref[i * 4 + 1] * 3 + 2];
					pos[0] += vertex[ref[i * 4 + 2] * 3];
					pos[1] += vertex[ref[i * 4 + 2] * 3 + 1];
					pos[2] += vertex[ref[i * 4 + 2] * 3 + 2];
					
					if(ref[i * 4 + 3] < length && vertex[ref[i * 4 + 3] * 3] != V_REAL64_MAX)
					{
						pos[0] = (pos[0] + vertex[ref[i * 4 + 3] * 3]) / 4.0;
						pos[1] = (pos[1] + vertex[ref[i * 4 + 3] * 3 + 1]) / 4.0;
						pos[2] = (pos[2] + vertex[ref[i * 4 + 3] * 3 + 2]) / 4.0;
					}else
					{
						pos[0] /= 3.0;
						pos[1] /= 3.0;
						pos[2] /= 3.0;
					}
					f = lp_paint(input, pos);
					if(f > lp_polygon_influence[i])
					{
						lp_polygon_influence[i] = f;
						if(lp_polygon_influence[i] > 1)
							lp_polygon_influence[i] = 1;
					}
				}
			}
		}
	}
}

void lp_apply_paint(ENode *node, EGeoLayer *red, EGeoLayer *green, EGeoLayer *blue, double *value, uint integer)
{
	EGeoLayer *layers[3];
	VNodeID node_id;
	VLayerID layer_id;
	uint i, j = 0, *ref, ref_length, length, r[4];

	node_id = e_ns_get_node_id(node);
	length = e_nsg_get_vertex_length(node);
	ref_length = e_nsg_get_polygon_length(node);
	ref = e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node, 1));
	layers[0] = red;
	layers[1] = green;
	layers[2] = blue;
/*	printf("red - %s\n", e_nsg_get_layer_name(red));
	printf("green - %s\n", e_nsg_get_layer_name(green));
	printf("blue - %s\n", e_nsg_get_layer_name(blue));
*/	for(i = 0; i < 3; i++)
	{
/*		printf(" layers[%u] %p\n", i, layers[i]);*/
		if(layers[i] == NULL)
			continue;
		layer_id = e_nsg_get_layer_id(layers[i]);
/*		printf("  layer %u: %u\n", i, e_nsg_get_layer_id(layers[i]));*/
		switch(e_nsg_get_layer_type(layers[i]))
		{
			case VN_G_LAYER_VERTEX_XYZ :
			for(j = 0; j < length; j++)
			{
				if(lp_vertex_influence_back[j] < lp_vertex_influence[j])
				{
					verse_send_g_vertex_set_xyz_real64(node_id, layer_id, j,
									   ((egreal *)lp_channels[i])[j * 3 + 0] + lp_vertex_influence[j] * lp_normal[j * 3 + 0] * value[i],
									   ((egreal *)lp_channels[i])[j * 3 + 1] + lp_vertex_influence[j] * lp_normal[j * 3 + 1] * value[i],
									   ((egreal *)lp_channels[i])[j * 3 + 2] + lp_vertex_influence[j] * lp_normal[j * 3 + 2] * value[i]);
				}
			}
			break;
			case VN_G_LAYER_VERTEX_UINT32 :
			for(j = 0; j < length; j++)
				if(lp_vertex_influence_back[j] < lp_vertex_influence[j] && ((uint32 *)lp_channels[i])[j] != integer)
					verse_send_g_vertex_set_uint32(node_id, layer_id, j, integer);
			break;
			case VN_G_LAYER_VERTEX_REAL :
			for(j = 0; j < length; j++)
				if(lp_vertex_influence_back[j] < lp_vertex_influence[j])
					verse_send_g_vertex_set_real64(node_id, layer_id, j, ((egreal *)lp_channels[i])[j] * (1.0 - lp_vertex_influence[j]) + value[i] * lp_vertex_influence[j]);
			break;
			case VN_G_LAYER_POLYGON_CORNER_UINT32 :
			for(j = 0; j < ref_length; j++)
			{
				r[0] = ref[j * 4];
				r[1] = ref[j * 4 + 1];
				r[2] = ref[j * 4 + 2];
				r[3] = ref[j * 4 + 3];
				if(r[0] < length && r[1] < length && r[2] < length)
				{				
					if(0 < lp_vertex_influence[r[0]] ||
					   0 < lp_vertex_influence[r[1]] ||
					   0 < lp_vertex_influence[r[2]] ||
					   (r[3] < length && 0 < lp_vertex_influence[r[3]]))
					{
						if(((uint32 *)lp_channels[i])[j * 4] != integer
							|| ((uint32 *)lp_channels[i])[j * 4 + 1] != integer
							|| ((uint32 *)lp_channels[i])[j * 4 + 2] != integer
							|| (r[3] < length && ((uint32 *)lp_channels[i])[j * 4 + 3] != integer))
						{
							if(0 < lp_vertex_influence[r[0]])
								r[0] = integer;
							else
								r[0] = ((uint32 *)lp_channels[i])[j * 4];

							if(0 < lp_vertex_influence[r[1]])
								r[1] = integer;
							else
								r[1] = ((uint32 *)lp_channels[i])[j * 4 + 1];

							if(0 < lp_vertex_influence[r[2]])
								r[2] = integer;
							else
								r[2] = ((uint32 *)lp_channels[i])[j * 4 + 2];

							if(r[3] >= length || 0 < lp_vertex_influence[r[3]])
								r[3] = integer;
							else
								r[3] = ((uint32 *)lp_channels[i])[j * 4 + 3];
							verse_send_g_polygon_set_corner_uint32(node_id, layer_id, j, r[0], r[1], r[2], r[3]);
						}
					}
				}
			}
			break;
			case VN_G_LAYER_POLYGON_CORNER_REAL :
			{
				r[0] = lp_vertex_influence[ref[j * 4]];
				r[1] = lp_vertex_influence[ref[j * 4 + 1]];
				r[2] = lp_vertex_influence[ref[j * 4 + 2]];
				r[3] = lp_vertex_influence[ref[j * 4 + 3]];
				if(r[0] < length && r[1] < length && r[2] < length)
				{				
					if(lp_vertex_influence[r[0]] > 0.0001
						|| lp_vertex_influence[r[1]] > 0.0001
						|| lp_vertex_influence[r[2]] > 0.0001
						|| (r[3] < length && lp_vertex_influence[r[3]] > 0.0001))
					{
						if(r[3] < length)
							verse_send_g_polygon_set_corner_real64(node_id, layer_id, j,
											       ((egreal *)lp_channels[i])[r[0]] * (1.0 - lp_polygon_influence[r[0]]) + value[i] * lp_polygon_influence[r[0]],
																						((egreal *)lp_channels[i])[r[1]] * (1.0 - lp_polygon_influence[r[1]]) + value[i] * lp_polygon_influence[r[1]],
																						((egreal *)lp_channels[i])[r[2]] * (1.0 - lp_polygon_influence[r[2]]) + value[i] * lp_polygon_influence[r[2]],
																						((egreal *)lp_channels[i])[r[3]] * (1.0 - lp_polygon_influence[r[3]]) + value[i] * lp_polygon_influence[r[3]]);
						else
							verse_send_g_polygon_set_corner_real64(node_id, layer_id, j,
											       ((egreal *)lp_channels[i])[r[0]] * (1.0 - lp_polygon_influence[r[0]]) + value[i] * lp_polygon_influence[r[0]],
											       ((egreal *)lp_channels[i])[r[1]] * (1.0 - lp_polygon_influence[r[1]]) + value[i] * lp_polygon_influence[r[1]],
											       ((egreal *)lp_channels[i])[r[2]] * (1.0 - lp_polygon_influence[r[2]]) + value[i] * lp_polygon_influence[r[2]], 0);
					}
				}
			}
			break;
			case VN_G_LAYER_POLYGON_FACE_UINT8 :
			for(j = 0; j < ref_length; j++)
				if(lp_polygon_influence[j] > 0.0001)
					verse_send_g_polygon_set_face_uint8(node_id, layer_id, j, integer);
			break;
			case VN_G_LAYER_POLYGON_FACE_UINT32 :
			for(j = 0; j < ref_length; j++)
				if(lp_polygon_influence[j] > 0.0001)
					verse_send_g_polygon_set_face_uint32(node_id, layer_id, j, integer);
			break;
			case VN_G_LAYER_POLYGON_FACE_REAL :
			for(j = 0; j < ref_length; j++)
				if(lp_polygon_influence[j] > 0.0001)
					verse_send_g_polygon_set_face_real64(node_id, layer_id, j, value[i]);
			break;
		}
	}
	if(lp_vertex_influence != NULL)
		memcpy(lp_vertex_influence_back, lp_vertex_influence, length * sizeof *lp_vertex_influence_back);
	if(lp_polygon_influence != NULL)
		memcpy(lp_polygon_influence_back, lp_polygon_influence, length * sizeof *lp_polygon_influence_back);
}
