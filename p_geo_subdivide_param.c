#include <stdio.h>
#include <stdlib.h>

//#include "ngl.h"
#include "enough.h"
#include "p_geo_subdivide_internal.h"
//#include "p_op_service.h"

#define P_PARAM_POLYS_PER_PASS 100

uint p_create_material_array(PMesh *mesh, egreal *output, uint32 *reference, uint32 ref_length, uint32 cv_count, uint slot, void *data, uint type, uint start)
{
	uint				i, k, vertex_count = 0, tri_count = 0, quad_count = 0;
	egreal				value0 = 0, value1 = 0, value2 = 0, value3 = 0, sum;
	PTessTableElement	*element;

	switch(type)
	{
		case VN_G_LAYER_VERTEX_XYZ :
		for(i = 0; i < ref_length && i < start + P_PARAM_POLYS_PER_PASS; i++)
		{
			if(reference[i * 4] < cv_count && reference[(i * 4) + 1] < cv_count && reference[(i * 4) + 2] < cv_count)
			{
				if(reference[(i * 4) + 3] >= cv_count)
				{
					value0 = ((egreal *)data)[reference[(i * 4)] * 3  + slot];
					value1 = ((egreal *)data)[reference[(i * 4) + 1] * 3  + slot];
					value2 = ((egreal *)data)[reference[(i * 4) + 2] * 3  + slot];
					element = mesh->tess.tri_tess[tri_count++];
					for(k = 0; k < element->vertex_count; k++)
					{
						sum = 0;
						sum += element->vertex_influence[(k * 3)] * value0;
						sum += element->vertex_influence[(k * 3) + 1] * value1;
						sum += element->vertex_influence[(k * 3) + 2] * value2;
						output[(vertex_count + k) * 3 + slot] = sum;
					}
					vertex_count +=	element->vertex_count;
				}
				else
				{
					value0 = ((egreal *)data)[reference[(i * 4)] * 3  + slot];
					value1 = ((egreal *)data)[reference[(i * 4) + 1] * 3  + slot];
					value2 = ((egreal *)data)[reference[(i * 4) + 2] * 3  + slot];
					value3 = ((egreal *)data)[reference[(i * 4) + 3] * 3  + slot];
					element = mesh->tess.quad_tess[quad_count++];
					for(k = 0; k < element->vertex_count; k++)
					{
						sum = 0;
						sum += element->vertex_influence[(k * 4)] * value0;
						sum += element->vertex_influence[(k * 4) + 1] * value1;
						sum += element->vertex_influence[(k * 4) + 2] * value2;	
						sum += element->vertex_influence[(k * 4) + 3] * value3;
						output[(vertex_count + k) * 3 + slot] = sum;
					}
					vertex_count +=	element->vertex_count;
				}
			}
		}
		break;
		case VN_G_LAYER_VERTEX_UINT32 :
		for(i = 0; i < ref_length && i < start + P_PARAM_POLYS_PER_PASS; i++)
		{
			if(reference[i * 4] < cv_count && reference[(i * 4) + 1] < cv_count && reference[(i * 4) + 2] < cv_count)
			{
				if(reference[(i * 4) + 3] >= cv_count)
				{
					value0 = ((uint32 *)data)[reference[(i * 4)]];
					value1 = ((uint32 *)data)[reference[(i * 4) + 1]];
					value2 = ((uint32 *)data)[reference[(i * 4) + 2]];
					element = mesh->tess.tri_tess[tri_count++];
					for(k = 0; k < element->vertex_count; k++)
					{
						sum = 0;
						sum += element->vertex_influence[(k * 3)] * value0;
						sum += element->vertex_influence[(k * 3) + 1] * value1;
						sum += element->vertex_influence[(k * 3) + 2] * value2;
						output[(vertex_count + k) * 3 + slot] = sum;
					}
					vertex_count +=	element->vertex_count;
				}
				else
				{
					value0 = ((uint32 *)data)[reference[(i * 4)]];
					value1 = ((uint32 *)data)[reference[(i * 4) + 1]];
					value2 = ((uint32 *)data)[reference[(i * 4) + 2]];
					value3 = ((uint32 *)data)[reference[(i * 4) + 3]];
					element = mesh->tess.quad_tess[quad_count++];
					for(k = 0; k < element->vertex_count; k++)
					{
						sum = 0;
						sum += element->vertex_influence[(k * 4)] * value0;
						sum += element->vertex_influence[(k * 4) + 1] * value1;
						sum += element->vertex_influence[(k * 4) + 2] * value2;	
						sum += element->vertex_influence[(k * 4) + 3] * value3;
						output[(vertex_count + k) * 3 + slot] = sum;
					}
					vertex_count +=	element->vertex_count;
				}
			}
		}
		break;
		case VN_G_LAYER_VERTEX_REAL :
		for(i = 0; i < ref_length && i < start + P_PARAM_POLYS_PER_PASS; i++)
		{
			if(reference[i * 4] < cv_count && reference[(i * 4) + 1] < cv_count && reference[(i * 4) + 2] < cv_count)
			{
				if(reference[(i * 4) + 3] >= cv_count)
				{
					value0 = ((egreal *)data)[reference[(i * 4)]];
					value1 = ((egreal *)data)[reference[(i * 4) + 1]];
					value2 = ((egreal *)data)[reference[(i * 4) + 2]];
					element = mesh->tess.tri_tess[tri_count++];
					for(k = 0; k < element->vertex_count; k++)
					{
						sum = 0;
						sum += element->vertex_influence[(k * 3)] * value0;
						sum += element->vertex_influence[(k * 3) + 1] * value1;
						sum += element->vertex_influence[(k * 3) + 2] * value2;
						output[(vertex_count + k) * 3 + slot] = sum;
					}
					vertex_count +=	element->vertex_count;
				}
				else
				{
					value0 = ((egreal *)data)[reference[(i * 4)]];
					value1 = ((egreal *)data)[reference[(i * 4) + 1]];
					value2 = ((egreal *)data)[reference[(i * 4) + 2]];
					value3 = ((egreal *)data)[reference[(i * 4) + 3]];
					element = mesh->tess.quad_tess[quad_count++];
					for(k = 0; k < element->vertex_count; k++)
					{
						sum = 0;
						sum += element->vertex_influence[(k * 4)] * value0;
						sum += element->vertex_influence[(k * 4) + 1] * value1;
						sum += element->vertex_influence[(k * 4) + 2] * value2;	
						sum += element->vertex_influence[(k * 4) + 3] * value3;
						output[(vertex_count + k) * 3 + slot] = sum;
					}
					vertex_count +=	element->vertex_count;
				}
			}
		}
		break;
		case VN_G_LAYER_POLYGON_CORNER_UINT32 :
		for(i = 0; i < ref_length && i < start + P_PARAM_POLYS_PER_PASS; i++)
		{
			if(reference[i * 4] < cv_count && reference[(i * 4) + 1] < cv_count && reference[(i * 4) + 2] < cv_count)
			{
				if(reference[(i * 4) + 3] >= cv_count)
				{
					value0 = ((uint32 *)data)[(i * 4)];
					value1 = ((uint32 *)data)[(i * 4) + 1];
					value2 = ((uint32 *)data)[(i * 4) + 2];
					element = mesh->tess.tri_tess[tri_count++];
					for(k = 0; k < element->vertex_count; k++)
					{
						sum = 0;
						sum += element->vertex_influence[(k * 3)] * value0;
						sum += element->vertex_influence[(k * 3) + 1] * value1;
						sum += element->vertex_influence[(k * 3) + 2] * value2;
						output[(vertex_count + k) * 3 + slot] = sum;
					}
					vertex_count +=	element->vertex_count;
				}
				else
				{
					value0 = ((uint32 *)data)[(i * 4)];
					value1 = ((uint32 *)data)[(i * 4) + 1];
					value2 = ((uint32 *)data)[(i * 4) + 2];
					value3 = ((uint32 *)data)[(i * 4) + 3];
					element = mesh->tess.quad_tess[quad_count++];
					for(k = 0; k < element->vertex_count; k++)
					{
						sum = 0;
						sum += element->vertex_influence[(k * 4)] * value0;
						sum += element->vertex_influence[(k * 4) + 1] * value1;
						sum += element->vertex_influence[(k * 4) + 2] * value2;	
						sum += element->vertex_influence[(k * 4) + 3] * value3;
						output[(vertex_count + k) * 3 + slot] = sum;
					}
					vertex_count +=	element->vertex_count;
				}
			}
		}
		break;
		case VN_G_LAYER_POLYGON_CORNER_REAL :
		for(i = 0; i < ref_length && i < start + P_PARAM_POLYS_PER_PASS; i++)
		{
			if(reference[i * 4] < cv_count && reference[(i * 4) + 1] < cv_count && reference[(i * 4) + 2] < cv_count)
			{
				if(reference[(i * 4) + 3] >= cv_count)
				{
					value0 = ((egreal *)data)[(i * 4)];
					value1 = ((egreal *)data)[(i * 4) + 1];
					value2 = ((egreal *)data)[(i * 4) + 2];
					element = mesh->tess.tri_tess[tri_count++];
					for(k = 0; k < element->vertex_count; k++)
					{
						sum = 0;
						sum += element->vertex_influence[(k * 3)] * value0;
						sum += element->vertex_influence[(k * 3) + 1] * value1;
						sum += element->vertex_influence[(k * 3) + 2] * value2;
						output[(vertex_count + k) * 3 + slot] = sum;
					}
					vertex_count +=	element->vertex_count;
				}
				else
				{
					value0 = ((egreal *)data)[(i * 4)];
					value1 = ((egreal *)data)[(i * 4) + 1];
					value2 = ((egreal *)data)[(i * 4) + 2];
					value3 = ((egreal *)data)[(i * 4) + 3];
					element = mesh->tess.quad_tess[quad_count++];
					for(k = 0; k < element->vertex_count; k++)
					{
						sum = 0;
						sum += element->vertex_influence[(k * 4)] * value0;
						sum += element->vertex_influence[(k * 4) + 1] * value1;
						sum += element->vertex_influence[(k * 4) + 2] * value2;	
						sum += element->vertex_influence[(k * 4) + 3] * value3;
						output[(vertex_count + k) * 3 + slot] = sum;
					}
					vertex_count +=	element->vertex_count;
				}
			}
		}
		break;
		case VN_G_LAYER_POLYGON_FACE_UINT8 :
		for(i = 0; i < ref_length && i < start + P_PARAM_POLYS_PER_PASS; i++)
		{
			if(reference[i * 4] < cv_count && reference[(i * 4) + 1] < cv_count && reference[(i * 4) + 2] < cv_count)
			{
				if(reference[(i * 4) + 3] >= cv_count)
				{
					value0 = ((uint8 *)data)[i];
					element = mesh->tess.tri_tess[tri_count++];
					for(k = 0; k < element->vertex_count; k++)
						output[(vertex_count + k) * 3 + slot] = value0;
					vertex_count +=	element->vertex_count;
				}
				else
				{
					value0 = ((uint32 *)data)[i];
					element = mesh->tess.quad_tess[quad_count++];
					for(k = 0; k < element->vertex_count; k++)
						output[(vertex_count + k) * 3 + slot] = value0;
					vertex_count +=	element->vertex_count;
				}
			}
		}
		break;
		case VN_G_LAYER_POLYGON_FACE_UINT32 :
		for(i = 0; i < ref_length && i < start + P_PARAM_POLYS_PER_PASS; i++)
		{
			if(reference[i * 4] < cv_count && reference[(i * 4) + 1] < cv_count && reference[(i * 4) + 2] < cv_count)
			{
				if(reference[(i * 4) + 3] >= cv_count)
				{
					value0 = ((uint32 *)data)[i];
					element = mesh->tess.tri_tess[tri_count++];
					for(k = 0; k < element->vertex_count; k++)
						output[(vertex_count + k) * 3 + slot] = value0;
					vertex_count +=	element->vertex_count;
				}
				else
				{
					value0 = ((uint32 *)data)[i];
					element = mesh->tess.quad_tess[quad_count++];
					for(k = 0; k < element->vertex_count; k++)
						output[(vertex_count + k) * 3 + slot] = value0;
					vertex_count +=	element->vertex_count;
				}
			}
		}
		break;
		case VN_G_LAYER_POLYGON_FACE_REAL :
		for(i = 0; i < ref_length && i < start + P_PARAM_POLYS_PER_PASS; i++)
		{
			if(reference[i * 4] < cv_count && reference[(i * 4) + 1] < cv_count && reference[(i * 4) + 2] < cv_count)
			{
				if(reference[(i * 4) + 3] >= cv_count)
				{
					value0 = ((egreal *)data)[i];
					element = mesh->tess.tri_tess[tri_count++];
					for(k = 0; k < element->vertex_count; k++)
						output[(vertex_count + k) * 3 + slot] = value0;
					vertex_count +=	element->vertex_count;
				}
				else
				{
					value0 = ((egreal *)data)[i];
					element = mesh->tess.quad_tess[quad_count++];
					for(k = 0; k < element->vertex_count; k++)
						output[(vertex_count + k) * 3 + slot] = value0;
					vertex_count +=	element->vertex_count;
				}
			}
		}
		break;
	}
	return i;
}
