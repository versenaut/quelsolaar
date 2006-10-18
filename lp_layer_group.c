
#include "st_matrix_operations.h"
#include "enough.h"
#include "seduce.h"
#include "deceive.h"


#include "lp_layer_groups.h"
#include "lp_projection.h"

typedef struct{
	char		name[16];
	EGeoLayer	*red;
	EGeoLayer	*green;
	EGeoLayer	*blue;
	boolean		gray;
}LayerGroup;

LayerGroup	*lp_groups = NULL;
uint		lp_groups_count = 0;
uint		lp_groups_allocated = 0;
uint		lp_group_current = 0;

uint lp_layer_current_get()
{
	return lp_group_current;
}

void lp_layer_current_set(uint group)
{
	lp_group_current = group;
}

EGeoLayer *lp_layer_get_red(uint group)
{
	return lp_groups[group].red;
}

EGeoLayer *lp_layer_get_green(uint group)
{
	return lp_groups[group].green;
}

EGeoLayer *lp_layer_get_blue(uint group)
{
	return lp_groups[group].blue;
}

boolean lp_layer_current_is_integer(void)
{
	VNGLayerType    type = -1;
	uint            current = lp_layer_current_get();
  	 
	if(current >= lp_groups_allocated)
		return FALSE;
  	 
	if(NULL != lp_layer_get_red(current))
		type = e_nsg_get_layer_type(lp_layer_get_red(current));
	else if(NULL != lp_layer_get_green(current))
		type = e_nsg_get_layer_type(lp_layer_get_green(current));
	else if(NULL != lp_layer_get_blue(current))
		type = e_nsg_get_layer_type(lp_layer_get_blue(current));
	if(type >= 0)
		return type == VN_G_LAYER_VERTEX_UINT32 || type == VN_G_LAYER_POLYGON_CORNER_UINT32 || type == VN_G_LAYER_POLYGON_FACE_UINT8 || type == VN_G_LAYER_POLYGON_FACE_UINT32;
	return FALSE;
}

char *lp_layer_get_name(uint group)
{
	return lp_groups[group].name;
}

uint lp_layer_get_group_count()
{
	return lp_groups_count;
}

void lp_update_layer_groups(ENode *node)
{
	EGeoLayer *layer;
	char *name;
	VNGLayerType type;
	uint i, j;
	lp_groups_count = 0;
	for(layer = e_nsg_get_layer_next(node, 0); layer != NULL; layer = e_nsg_get_layer_next(node, e_nsg_get_layer_id(layer) + 1))
	{
		if(1 !=  e_nsg_get_layer_id(layer))
		{
			name = e_nsg_get_layer_name(layer);
			type = e_nsg_get_layer_type(layer);
			i = lp_groups_count;
			if(type != VN_G_LAYER_VERTEX_XYZ && type != VN_G_LAYER_VERTEX_UINT32 && type != VN_G_LAYER_POLYGON_CORNER_UINT32 && type != VN_G_LAYER_POLYGON_FACE_UINT8 && type != VN_G_LAYER_POLYGON_FACE_UINT32)
			{
				for(i = 0; i < lp_groups_count; i++)
				{
					for(j = 0; lp_groups[i].name[j] != 0 && lp_groups[i].name[j] == name[j]; j++);
					if(name[j] == 95 && lp_groups[i].name[j] == 0 && (name[j + 1] == 'r' || name[j + 1] == 'g' || name[j + 1] == 'b' || name[j + 1] == 'u' || name[j + 1] == 'v' || name[j + 1] == 'w') && name[j + 2] == 0)
					{
						j++;
						if(name[j] == 'r' || name[j] == 'u')
						{
							lp_groups[i].red = layer;
							break;
						}
						if(name[j] == 'g' || name[j] == 'v')
						{
							lp_groups[i].green = layer;
							break;
						}
						if(name[j] == 'b' || name[j] == 'w')
						{
							lp_groups[i].blue = layer;
							break;
						}
					}
				}
			}
			if(i == lp_groups_count)
			{
				if(lp_groups_count == lp_groups_allocated)
				{
					lp_groups_allocated += 32;
					lp_groups = realloc(lp_groups, (sizeof *lp_groups) * lp_groups_allocated);
				}
				for(j = 0; name[j] != 0 && name[j] != 95; j++)
					lp_groups[lp_groups_count].name[j] = name[j];
				lp_groups[lp_groups_count].name[j] = 0;

		//		if(lp_groups[lp_groups_count].name[j] == 95)
		//			lp_groups[lp_groups_count].gray = TRUE;
			
				if(name[j] == 95 && (name[j + 1] == 'r' || name[j + 1] == 'g' || name[j + 1] == 'b' || name[j + 1] == 'u' || name[j + 1] == 'v' || name[j + 1] == 'w') && name[j + 2] == 0)
				{
					lp_groups[lp_groups_count].red = NULL;
					lp_groups[lp_groups_count].green = NULL;
					lp_groups[lp_groups_count].blue = NULL;
					j++;
					if(name[j] == 'r' || name[j] == 'u')
						lp_groups[lp_groups_count].red = layer;
					if(name[j] == 'g' || name[j] == 'v')
						lp_groups[lp_groups_count].green = layer;
					if(name[j] == 'b' || name[j] == 'w')
						lp_groups[lp_groups_count].blue = layer;
				}else
				{
					lp_groups[lp_groups_count].red = layer;
					lp_groups[lp_groups_count].green = NULL;
					lp_groups[lp_groups_count].blue = NULL;
				}
				lp_groups_count++;
			}
		}
	}
}
