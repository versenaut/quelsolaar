
#include <math.h>

#include "st_matrix_operations.h"
#include "enough.h"
#include "seduce.h"
#include "persuade.h"
#include "deceive.h"

#include "co_vn_handle.h"

#define SUMMARY_COLOR 0.5

static float co_draw_node_summary(ENode *node, float x, float y, uint recursion)
{
	char *type_name[] = {"object", "geometry", "material", "bitmap", "text", "curve", "audio"};
	ENode *link_node;
	uint i, j;
	VNTag *tag;
	boolean content;
	float line = y;
	char text[2048];
	EObjLink *link;

	sui_draw_text(x, line, SUI_T_SIZE * 0.7, SUI_T_SPACE * 0.7, e_ns_get_node_name(node), SUMMARY_COLOR, SUMMARY_COLOR, SUMMARY_COLOR);
	line -= 0.02;

	content = FALSE;
	for(i = e_ns_get_next_tag_group(node, 0); i != (uint16)-1; i = e_ns_get_next_tag_group(node, i + 1))
	{
		if(content == FALSE)
		{
			sui_draw_text(x, line, SUI_T_SIZE * 0.7, SUI_T_SPACE * 0.7, "Tags:", SUMMARY_COLOR, SUMMARY_COLOR, SUMMARY_COLOR);
			line -= 0.02;
			content = TRUE;
		}
		sui_draw_text(x + 0.02, line, SUI_T_SIZE * 0.7, SUI_T_SPACE * 0.7, e_ns_get_tag_group(node, i), SUMMARY_COLOR, SUMMARY_COLOR, SUMMARY_COLOR);
		line -= 0.02;
		for(j = e_ns_get_next_tag(node, i, 0); j != (uint16)-1; j = e_ns_get_next_tag(node, i, j + 1))
		{
			tag = e_ns_get_tag(node, i, j);
			text[0] = 0;
			switch(e_ns_get_tag_type(node, i, j))
			{
				case VN_TAG_BOOLEAN :
					if(tag->vboolean)
						sprintf(text, "%s = TRUE", e_ns_get_tag_name(node, i, j));
					else
						sprintf(text, "%s = FALSE", e_ns_get_tag_name(node, i, j));
				break;
				case VN_TAG_UINT32 :
						sprintf(text, "%s = %u", e_ns_get_tag_name(node, i, j), tag->vuint32);
				break;
				case VN_TAG_REAL64 :
						sprintf(text, "%s = %f", e_ns_get_tag_name(node, i, j), tag->vreal64);
				break;
				case VN_TAG_STRING :
						sprintf(text, "%s = %s", e_ns_get_tag_name(node, i, j), tag->vstring);
				break;
				case VN_TAG_REAL64_VEC3 :
						sprintf(text, "%s = %f %f %f", e_ns_get_tag_name(node, i, j), tag->vreal64_vec3[0], tag->vreal64_vec3[1], tag->vreal64_vec3[2]);
				break;
				case VN_TAG_LINK :
						sprintf(text, "%s = %u", e_ns_get_tag_name(node, i, j), tag->vlink);
				break;
				case VN_TAG_ANIMATION :
						sprintf(text, "%s = %u (%u-%u)", e_ns_get_tag_name(node, i, j), tag->vanimation.curve, tag->vanimation.start, tag->vanimation.end);
				break;
				case VN_TAG_BLOB :
						sprintf(text, "%s = size", e_ns_get_tag_name(node, i, j), tag->vblob.size);
				break;
			}
			sui_draw_text(x + 0.04, line, SUI_T_SIZE * 0.7, SUI_T_SPACE * 0.7, text, SUMMARY_COLOR, SUMMARY_COLOR, SUMMARY_COLOR);
			line -= 0.02;
		}
	}

	switch(e_ns_get_node_type(node))
	{
		case V_NT_OBJECT :
			{
				sui_draw_text(0.2, line, SUI_T_SIZE * 0.7, SUI_T_SPACE * 0.7, "Links:", SUMMARY_COLOR, SUMMARY_COLOR, SUMMARY_COLOR);
				line -= 0.02;
				for(link = e_nso_get_next_link(node, 0); link != NULL; link = e_nso_get_next_link(node, e_nso_get_link_id(link) + 1))
				{
					
					if((link_node = e_ns_get_node(0, e_nso_get_link_node(link))) != NULL)
					{
						sprintf(text, "lable: %s");
						sui_draw_text(x + 0.02, line, SUI_T_SIZE * 0.7, SUI_T_SPACE * 0.7, text, SUMMARY_COLOR, SUMMARY_COLOR, SUMMARY_COLOR);
						line -= 0.02;
						if(recursion != 0)
							line -= co_draw_node_summary(link_node, x + 0.04, line, recursion - 1);
					}
				}

				content = FALSE;
				for(i = e_nso_get_next_method_group(node, 0); i != (uint16)-1 ; i = e_nso_get_next_method_group(node, i + 1))
				{
					if(content == FALSE)
					{
						sui_draw_text(x, line, SUI_T_SIZE * 0.7, SUI_T_SPACE * 0.7, "Methods:", SUMMARY_COLOR, SUMMARY_COLOR, SUMMARY_COLOR);
						line -= 0.02;
						content = TRUE;
					}
					sui_draw_text(x + 0.02, line, SUI_T_SIZE * 0.7, SUI_T_SPACE * 0.7, e_nso_get_method_group(node, i), SUMMARY_COLOR, SUMMARY_COLOR, SUMMARY_COLOR);
					line -= 0.02;
					for(j = e_nso_get_next_method(node, i, 0); j != (uint16)-1; j = e_nso_get_next_method(node, i, j + 1))
					{
						sui_draw_text(x + 0.04, line, SUI_T_SIZE * 0.7, SUI_T_SPACE * 0.7, e_nso_get_method(node, i, j), SUMMARY_COLOR, SUMMARY_COLOR, SUMMARY_COLOR);
						line -= 0.02;
					}
				}
			}
		break;
		case V_NT_GEOMETRY :
			{
				EGeoLayer *layer;
				content = FALSE;
				for(layer = e_nsg_get_layer_next(node, 0); layer != NULL; layer = e_nsg_get_layer_next(node, e_nsg_get_layer_id(layer) + 1))
				{
					if(content == FALSE)
					{
						sui_draw_text(x, line, SUI_T_SIZE * 0.7, SUI_T_SPACE * 0.7, "layers:", SUMMARY_COLOR, SUMMARY_COLOR, SUMMARY_COLOR);
						line -= 0.02;
						content = TRUE;
					}
					sui_draw_text(x + 0.02, line, SUI_T_SIZE * 0.7, SUI_T_SPACE * 0.7, e_nsg_get_layer_name(layer), SUMMARY_COLOR, SUMMARY_COLOR, SUMMARY_COLOR);
					line -= 0.02;
				}
			}
		break;
		case V_NT_MATERIAL :
			{
				content = FALSE;
				for(i = e_nsm_get_fragment_next(node, 0); i != (VNMFragmentID)-1; i = e_nsm_get_fragment_next(node, i + 1))
				{
					if(e_nsm_get_fragment_type(node, i) == VN_M_FT_OUTPUT)
					{
						VMatFrag *frag;
						if(content == FALSE)
						{
							sui_draw_text(x, line, SUI_T_SIZE * 0.7, SUI_T_SPACE * 0.7, "Methods:", SUMMARY_COLOR, SUMMARY_COLOR, SUMMARY_COLOR);
							line -= 0.02;
							content = TRUE;
						}
						frag = e_nsm_get_fragment(node, i);
						sui_draw_text(x + 0.02, line, SUI_T_SIZE * 0.7, SUI_T_SPACE * 0.7, frag->output.label, SUMMARY_COLOR, SUMMARY_COLOR, SUMMARY_COLOR);
						line -= 0.02;
					}
				}
			}
		break;
		case V_NT_BITMAP :
			{
				EGeoLayer *layer;
				uint size[3];
				content = FALSE;
				e_nsb_get_size(node, &size[0], &size[1], &size[2]);
				sprintf(text, "size: = %u * %u * %u", size[0], size[1], size[2]);
				sui_draw_text(x, line, SUI_T_SIZE * 0.7, SUI_T_SPACE * 0.7, text, SUMMARY_COLOR, SUMMARY_COLOR, SUMMARY_COLOR);
				line -= 0.02;
				for(layer = e_nsb_get_layer_next(node, 0); layer != NULL; layer = e_nsb_get_layer_next(node, e_nsb_get_layer_id(layer) + 1))
				{
					if(content == FALSE)
					{
						sui_draw_text(x, line, SUI_T_SIZE * 0.7, SUI_T_SPACE * 0.7, "layers:", SUMMARY_COLOR, SUMMARY_COLOR, SUMMARY_COLOR);
						line -= 0.02;
						content = TRUE;
					}
					sui_draw_text(x + 0.02, line, SUI_T_SIZE * 0.7, SUI_T_SPACE * 0.7, e_nsb_get_layer_name(layer), SUMMARY_COLOR, SUMMARY_COLOR, SUMMARY_COLOR);
					line -= 0.02;
				}
			}
		break;
		case V_NT_TEXT :
		break;
		case V_NT_CURVE :
		break;
	}
	return line;
}

void co_draw_sumary(ENode *node, float x, float y)
{
	glPushMatrix();
	glTranslatef(-x, -y, -1);
	sui_draw_2d_line_gl(0.03, 0.015, 0.18, 0.09, SUMMARY_COLOR, SUMMARY_COLOR, SUMMARY_COLOR);
	co_draw_node_summary(node, 0.2, 0.1, 10);
	glPopMatrix();
}
