#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"
#include "seduce.h"
#include "co_vn_graphics.h"
#include "co_vn_handle.h"
#include "co_widgets.h"

boolean co_text_search(char *text, char *search)
{
	uint i, j;
	for(i = 0; text[i] != 0; i++)
	{
		if(text[i] == search[0] || search[0] + 32 == text[i] || search[0] - 32 == text[i])
		{
//			for(j = 0; text[i + j] != 0 && search[j] != 0 && (text[i + j] == search[j] || search[j] + 32 == text[i + j] || search[j] - 32 == text[i + j]); j++);
			for(j = 0; text[i + j] != 0 && search[j] != 0 && (text[i + j] == search[j] || (text[i + j] > 64 && text[i + j] < 91 && text[i + j] + 32 == search[j]) || (search[j] > 64 && search[j] < 91 && search[j] + 32 == text[i + j])); j++);
			if(search[j] == 0)
				return TRUE;
		}
	}
	return FALSE;
}

boolean co_search_node(ENode *node, char *search)
{
	uint i, j;
	if(search[0] == 0)
		return TRUE;

	if(co_text_search(e_ns_get_node_name(node), search))
		return TRUE;

	for(i = e_ns_get_next_tag_group(node, 0); i != -1; i = e_ns_get_next_tag_group(node, i))
	{
		if(co_text_search(e_ns_get_tag_group(node, i), search))
			return TRUE;
		for(j = e_ns_get_next_tag(node, i, 0); j != -1; j = e_ns_get_next_tag(node, i, j))
		{
			if(co_text_search(e_ns_get_tag_name(node, j, i), search))
				return TRUE;
			if(VN_TAG_STRING == e_ns_get_tag_type(node, j, i))
			{
				VNTag *tag;
				tag = e_ns_get_tag(node, j, i);
				if(co_text_search(tag->vstring, search))
					return TRUE;
			}
		}
	}
	switch(e_ns_get_node_type(node))
	{
		case V_NT_OBJECT :
		break;
		case V_NT_GEOMETRY :
			{
				EGeoLayer *layer;
				for(layer = e_nsg_get_layer_next(node, 0); layer != NULL; layer = e_nsg_get_layer_next(node, e_nsg_get_layer_id(layer) + 1))
					if(co_text_search(e_nsg_get_layer_name(layer), search))
						return TRUE;
			}
		break;
		case V_NT_MATERIAL :
		break;
		case V_NT_BITMAP :
			{
				EGeoLayer *layer;
				for(layer = e_nsb_get_layer_next(node, 0); layer != NULL; layer = e_nsb_get_layer_next(node, e_nsg_get_layer_id(layer) + 1))
					if(co_text_search(e_nsb_get_layer_name(layer), search))
						return TRUE;
			}
		break;
		case V_NT_TEXT :
		break;
/*		case V_NT_PARTICLE :
		break;
*/		case V_NT_CURVE :
		break;
	}

	return FALSE;
}
