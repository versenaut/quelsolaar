#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"
#include "seduce.h"
#include "co_vn_graphics.h"
#include "co_vn_handle.h"
#include "co_widgets.h"

boolean co_text_search(const char *text, const char *search)
{
	uint i, j;

	for(i = 0; text[i] != 0; i++)
	{
		if(text[i] == search[0] || search[0] + 32 == text[i] || search[0] - 32 == text[i])
		{
			for(j = 0; text[i + j] != 0 && search[j] != 0 && (text[i + j] == search[j] || (text[i + j] > 64 && text[i + j] < 91 && text[i + j] + 32 == search[j]) || (search[j] > 64 && search[j] < 91 && search[j] + 32 == text[i + j])); j++);
			if(search[j] == 0)
				return TRUE;
		}
	}
	for(i = 0; text[i] != 0; i++)
	{
		if(text[i] == search[0])
		{
			for(j = 0; text[i + j] != 0 && search[j] != 0 && (text[i + j] == search[j]); j++);
			if(search[j] == 0)
				return TRUE;
		}
	}
	return FALSE;
}

boolean co_search_node(ENode *node, char *search)
{
	uint16 i, j;
	if(search[0] == 0)
		return TRUE;

	if(co_text_search(e_ns_get_node_name(node), search))
		return TRUE;

	for(i = e_ns_get_next_tag_group(node, 0); i != (uint16)-1; i = e_ns_get_next_tag_group(node, i + 1))
	{
		if(co_text_search(e_ns_get_tag_group(node, i), search))
			return TRUE;
		for(j = e_ns_get_next_tag(node, i, 0); j != (uint16)-1; j = e_ns_get_next_tag(node, i, j + 1))
		{
			if(co_text_search(e_ns_get_tag_name(node, i, j), search))
				return TRUE;
			if(VN_TAG_STRING == e_ns_get_tag_type(node, i, j))
			{
				VNTag *tag;
				tag = e_ns_get_tag(node, i, j);
				if(co_text_search(tag->vstring, search))
					return TRUE;
			}
		}
	}
	switch(e_ns_get_node_type(node))
	{
		case V_NT_OBJECT :
			{
				EObjLink *link;
				uint i, j, k;
				char **names;
				for(link = e_nso_get_next_link(node, 0); link != NULL && link != NULL; link = e_nso_get_next_link(node, e_nso_get_link_id(link) + 1))
					if(co_text_search(e_nso_get_link_name(link), search))
						return TRUE;

				for(i = e_nso_get_next_method_group(node, 0); i != (uint16)-1 ; i = e_nso_get_next_method_group(node, i + 1))
				{
					if(co_text_search(e_nso_get_method_group(node, i), search))
						return TRUE;
					for(j = e_nso_get_next_method(node, i, 0); j != (uint16)-1; j = e_nso_get_next_method(node, i, j + 1))
					{
						if(co_text_search(e_nso_get_method(node, i, j), search))
							return TRUE;
						names = e_nso_get_method_param_names(node, i, j);
						for(k = 0; k < e_nso_get_method_param_count(node, i, j); k++)
							if(co_text_search(names[k], search))
								return TRUE;
					}
				}
			}
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
			{
				for(i = e_nsm_get_fragment_next(node, 0); i != (VNMFragmentID)-1; i = e_nsm_get_fragment_next(node, i + 1))
				{
					VNMFragmentType type;
					VMatFrag *frag;
					static const char *frag_type[] = { "VN_M_FT_COLOR", "VN_M_FT_LIGHT", "VN_M_FT_REFLECTION", "VN_M_FT_TRANSPARENCY",
						"VN_M_FT_VOLUME", "VN_M_FT_VIEW", "VN_M_FT_GEOMETRY", "VN_M_FT_TEXTURE", "VN_M_FT_NOISE", "VN_M_FT_BLENDER",
						"VN_M_FT_CLAMP", "VN_M_FT_MATRIX", "VN_M_FT_RAMP", "VN_M_FT_ANIMATION", "VN_M_FT_ALTERNATIVE", "VN_M_FT_OUTPUT"};

					frag = e_nsm_get_fragment(node, i);
					type = e_nsm_get_fragment_type(node, i);
					if(type >= sizeof frag_type / sizeof *frag_type)
					{
						fprintf(stderr, "Connector: Unable to search material fragment type %u, only know of %u types\n", type, sizeof frag_type / sizeof *frag_type);
						return FALSE;
					}
					if(co_text_search(frag_type[type], search))
						return TRUE;
					switch(type)
					{	
						case VN_M_FT_COLOR :
							break;
						case VN_M_FT_LIGHT :
							if(co_text_search(frag->light.brdf_r, search))
								return TRUE;
							if(co_text_search(frag->light.brdf_g, search))
								return TRUE;
							if(co_text_search(frag->light.brdf_b, search))
								return TRUE;
							break;
						case VN_M_FT_REFLECTION :
							break;
						case VN_M_FT_TRANSPARENCY :
							break;
						case VN_M_FT_VOLUME :
							break;
						case VN_M_FT_GEOMETRY :
							if(co_text_search(frag->geometry.layer_r, search))
								return TRUE;
							if(co_text_search(frag->geometry.layer_g, search))
								return TRUE;
							if(co_text_search(frag->geometry.layer_b, search))
								return TRUE;
							break;
						case VN_M_FT_TEXTURE :
							if(co_text_search(frag->texture.layer_r, search))
								return TRUE;
							if(co_text_search(frag->texture.layer_g, search))
								return TRUE;
							if(co_text_search(frag->texture.layer_b, search))
								return TRUE;
							break;
						case VN_M_FT_NOISE :
							break;
						case VN_M_FT_BLENDER :
							{
								char *blend_modes[7] = {"VN_M_BLEND_FADE", "VN_M_BLEND_ADD", "VN_M_BLEND_SUBTRACT", "VN_M_BLEND_MULTIPLY", "VN_M_BLEND_DIVIDE", "VN_M_BLEND_DOT"};
								if(co_text_search(blend_modes[frag->blender.type], search))
									return TRUE;
							}
							break;
						case VN_M_FT_MATRIX :
							break;
						case VN_M_FT_RAMP :
							break;
						case VN_M_FT_ANIMATION :
						case VN_M_FT_ALTERNATIVE :
							break;
						case VN_M_FT_OUTPUT :
							if(co_text_search(frag->output.label, search))
								return TRUE;
							break;
						default:
							break;
					}
				}
			}
		break;
		case V_NT_BITMAP :
			{
				EGeoLayer *layer;
				for(layer = e_nsb_get_layer_next(node, 0); layer != NULL; layer = e_nsb_get_layer_next(node, e_nsg_get_layer_id(layer) + 1))
					if(co_text_search(e_nsb_get_layer_name(layer), search))
						return TRUE;
			}
		break;
		default:
		break;
	}

	return FALSE;
}

static char co_compare[64] = {0};

void co_search_clear(void)
{
	ENode *node;
	COVerseNode *co_node;
	uint i, type;

	for(type = 0; type < V_NT_NUM_TYPES; type++)
		for(node = e_ns_get_node_next(0, 0, type); node != 0; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, type))
			if((co_node = e_ns_get_custom_data(node, CONNECTOR_ENOUGH_SLOT)) != NULL)
				co_node->search = TRUE;
	for(i = 0; i < 64; i++)
		co_compare[i] = 0;
}

void co_search_update(char *search)
{
	ENode *node;
	COVerseNode *co_node;
	uint i, type;

	for(i = 0; search[i] == co_compare[i] && i < 64; i++)
		if(search[i] == co_compare[i])
			return;
	co_search_clear();

	for(type = 0; type < V_NT_NUM_TYPES; type++)
		for(node = e_ns_get_node_next(0, 0, type); node != 0; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, type))	
			if((co_node = e_ns_get_custom_data(node, CONNECTOR_ENOUGH_SLOT)) != NULL)
				if(co_node->search == TRUE)
					if(co_search_node(node, search) != TRUE)
						co_node->search = FALSE;

	for(i = 0; i < 64; i++)
		co_compare[i] = search[i];
}

