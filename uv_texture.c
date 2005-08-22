#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"
#include "seduce.h"
#include "persuade.h"

#include "uv.h"
#include "uv_geometry.h"

boolean poly_texture_hit_test(uint id, egreal x, egreal y)
{
	egreal x0, y0, x1, y1, x2, y2, x3, y3, r;
	egreal uv[8];


	uvg_get_uve(uv, id);
	x0 = uv[0] - x; 
	y0 = uv[1] - y;
	x1 = uv[2] - x; 
	y1 = uv[3] - y;
	x2 = uv[4] - x; 
	y2 = uv[5] - y;
	x3 = uv[6] - x; 
	y3 = uv[7] - y;

	if(!uvg_is_quad(id))
	{
		if(x0 * -(y1 - y0) + y0 * (x1 - x0) < 0)
		{
			if(x1 * -(y2 - y1) + y1 * (x2 - x1) < 0 &&
				x2 * -(y0 - y2) + y2 * (x0 - x2) < 0)
			{
				return TRUE;
			}
		}else
		{
			if(x1 * -(y2 - y1) + y1 * (x2 - x1) > 0 &&
				x2 * -(y0 - y2) + y2 * (x0 - x2) > 0)
			{
				return TRUE;
			}
		}
	}else
	{
		if(x0 * -(y1 - y0) + y0 * (x1 - x0) < 0)
		{
			if(x1 * -(y2 - y1) + y1 * (x2 - x1) < 0 &&
				x2 * -(y3 - y2) + y2 * (x3 - x2) < 0 &&
				x3 * -(y0 - y3) + y3 * (x0 - x3) < 0)
			{
				return TRUE;
			}
		}else
		{
			if(x1 * -(y2 - y1) + y1 * (x2 - x1) > 0 &&
				x2 * -(y3 - y2) + y2 * (x3 - x2) > 0 &&
				x3 * -(y0 - y3) + y3 * (x0 - x3) > 0)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}


uint get_surface_poly_id(egreal x, egreal y)
{
	static uint last = -1;
	if(poly_texture_hit_test(last, x, y))
		return last;
	for(last = uvg_get_next_polygon(0); last != -1 && !poly_texture_hit_test(last, x, y); last = uvg_get_next_polygon(last + 1));
	return last;
}

void uv_draw_texture_compute_shade(egreal *output, uint id)
{
	uint *ref;
	egreal *vertex, *a, *b, *origo, normal[3], f;
	ref = uvg_get_ref();
	vertex = uvg_get_vertex();
	origo = &vertex[ref[id * 4] * 3];
	a = &vertex[ref[id * 4 + 1] * 3];
	b = &vertex[ref[id * 4 + 2] * 3];
	normal[0] = (a[1] - origo[1]) * (b[2] - origo[2]) - (a[2] - origo[2]) * (b[1] - origo[1]);
	normal[1] = (a[2] - origo[2]) * (b[0] - origo[0]) - (a[0] - origo[0]) * (b[2] - origo[2]);
	normal[2] = (a[0] - origo[0]) * (b[1] - origo[1]) - (a[1] - origo[1]) * (b[0] - origo[0]);
	f = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
	normal[0] /= f;
	normal[1] /= f;
	normal[2] /= f;
	f = normal[0] * 0.4 + normal[1] * 0.4 + normal[2] * 0.4;
	if(f < 0)
		f = 0;
	output[0] = f * 1;
	output[1] = f * 0.6;
	output[2] = f * 0.2;
	output[0] += 0.2 * (normal[1] + 1) + 0.1;
	output[1] += 0.4 * (normal[1] + 1) + 0.1;
	output[2] += 0.5 * (normal[1] + 1) + 0.1;

}

void uv_draw_texture(ENode *node)
{
	double x_tile, y_tile; 
	egreal output[VN_B_TILE_SIZE * VN_B_TILE_SIZE * 3];
	uint i, j, k, l, x, y, z;
	VNBTile tile;
	EBitLayer *layer[3];
	e_nsb_get_size(node, &x, &y, &z);
	if(x * y * z >= VN_B_TILE_SIZE * VN_B_TILE_SIZE)
	
	if((layer[0] = e_nsb_get_layer_by_name(node, "col_r")) == NULL)
		return;
	if((layer[1] = e_nsb_get_layer_by_name(node, "col_g")) == NULL)
		return;
	if((layer[2] = e_nsb_get_layer_by_name(node, "col_b")) == NULL)
		return;
	printf("3 layers %p %p %p\n", layer[0], layer[1], layer[2]);

	for(i = 0 ; i < y / VN_B_TILE_SIZE; i++)
	{
		y_tile = (double)(i * VN_B_TILE_SIZE) / (double)y;
		for(j = 0 ; j < x / VN_B_TILE_SIZE; j++)
		{
			x_tile = (double)(j * VN_B_TILE_SIZE) / (double)x;
			for(k = 0 ; k < VN_B_TILE_SIZE; k++)
				for(l = 0 ; l < VN_B_TILE_SIZE; l++)
					uv_draw_texture_compute_shade(&output[(k * 8 + l) * 3], get_surface_poly_id(x_tile + (double)l / (double)x, y_tile + (double)k / (double)y));

			for(k = 0 ; k < 3; k++)
			{
				if(layer[k] != NULL)
				{
					switch(e_nsb_get_layer_type(layer[k]))
					{
						case VN_B_LAYER_UINT8 :
							for(l = 0 ; l < VN_B_TILE_SIZE * VN_B_TILE_SIZE; l++)
							{
								if(output[l * 3 + k] < 1)
									tile.vuint8[l] = output[l * 3 + k] * 255.0;
								else
									tile.vuint8[l] = 255;
							}
						break;
						case VN_B_LAYER_UINT16 :
							for(l = 0 ; l < VN_B_TILE_SIZE * VN_B_TILE_SIZE; l++)
							{
								if(output[l * 3 + k] < 1)
									tile.vuint16[l] = output[l * 3 + k] * 255.0 * 256.0;
								else
									tile.vuint8[l] = 255;
							}
						break;
						case VN_B_LAYER_REAL32 :
							for(l = 0 ; l < VN_B_TILE_SIZE * VN_B_TILE_SIZE; l++)
								tile.vreal32[l] = output[l * 3 + k];
						break;
						case VN_B_LAYER_REAL64 :
							for(l = 0 ; l < VN_B_TILE_SIZE * VN_B_TILE_SIZE; l++)
								tile.vreal64[l] = output[l * 3 + k];
						break;
					}
					verse_send_b_tile_set(e_ns_get_node_id(node), e_nsb_get_layer_id(layer[k]), j, i, 0, e_nsb_get_layer_type(layer[k]), &tile);
				}
			}
		}
	}
}

void uv_bitmap_node_create_func(uint connection, uint id, VNodeType type, void *user)
{
	verse_send_b_dimensions_set(id, 256, 256, 1);
	verse_send_b_layer_create(id, -1, "col_r", VN_B_LAYER_UINT8);
	verse_send_b_layer_create(id, -1, "col_g", VN_B_LAYER_UINT8);
	verse_send_b_layer_create(id, -1, "col_b", VN_B_LAYER_UINT8);
}

