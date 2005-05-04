/*
 * 
*/

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "verse.h"
#include "enough.h"

static void save_object(FILE *f, ENode *o_node)
{
	static const char *method_types[] = {
		"VN_O_METHOD_PTYPE_INT8", "VN_O_METHOD_PTYPE_INT16", "VN_O_METHOD_PTYPE_INT32",
		"VN_O_METHOD_PTYPE_UINT8", "VN_O_METHOD_PTYPE_UINT16", "VN_O_METHOD_PTYPE_UINT32",
		"VN_O_METHOD_PTYPE_REAL32", "VN_O_METHOD_PTYPE_REAL64",
		"VN_O_METHOD_PTYPE_REAL32_VEC2", "VN_O_METHOD_PTYPE_REAL32_VEC3", "VN_O_METHOD_PTYPE_REAL32_VEC4",
		"VN_O_METHOD_PTYPE_REAL64_VEC2", "VN_O_METHOD_PTYPE_REAL64_VEC3", "VN_O_METHOD_PTYPE_REAL64_VEC4",
		"VN_O_METHOD_PTYPE_REAL32_MAT4", "VN_O_METHOD_PTYPE_REAL32_MAT9", "VN_O_METHOD_PTYPE_REAL32_MAT16",
		"VN_O_METHOD_PTYPE_REAL64_MAT4", "VN_O_METHOD_PTYPE_REAL64_MAT9", "VN_O_METHOD_PTYPE_REAL64_MAT16",
		"VN_O_METHOD_PTYPE_STRING",
		"VN_O_METHOD_PTYPE_NODE", "VN_O_METHOD_PTYPE_LAYER"};
	double tmp[4];
	VNQuat64	rot;
	uint32 time[2], i;
	EObjLink *link;
	uint16 group_id, method_id;

	fprintf(f, "\t<position>\n");
	e_nso_get_pos(o_node, tmp, NULL, NULL, NULL, NULL, time);
	fprintf(f, "\t\t%f %f %f\n", tmp[0], tmp[1], tmp[2]);
	fprintf(f, "\t</position>\n");
	fprintf(f, "\t<rotation>\n");
	e_nso_get_rot(o_node, &rot, NULL, NULL, NULL, NULL, time);
	fprintf(f, "\t\t%f %f %f %f\n", rot.x, rot.y, rot.z, rot.w);
	fprintf(f, "\t</rotation>\n");
	fprintf(f, "\t<scale>\n");
	e_nso_get_scale(o_node, tmp);
	fprintf(f, "\t\t%f %f %f\n", tmp[0], tmp[1], tmp[2]);
	fprintf(f, "\t</scale>\n");
	e_nso_get_light(o_node, tmp);
	if(tmp[0] != 0 || tmp[1] != 0 || tmp[2] != 0)
	{
		fprintf(f, "\t<light>\n");
		fprintf(f, "\t\t%f %f %f\n", tmp[0], tmp[1], tmp[2]);
		fprintf(f, "\t</light>\n");
	}
	if(e_nso_get_next_link(o_node, 0) != NULL)
	{
		fprintf(f, "\t<links>\n");
		for(link = e_nso_get_next_link(o_node, 0); link != NULL; link = e_nso_get_next_link(o_node, 1 + e_nso_get_link_id(link)))
			fprintf(f, "\t\t<link node=\"%u\" label=\"%s\" target=\"%u\"/>\n", e_nso_get_link_node(link), e_nso_get_link_name(link), e_nso_get_link_target_id(link));
		fprintf(f, "\t</links>\n");
	}

	if(e_nso_get_next_method_group(o_node, 0) != (uint16)-1)
	{
		fprintf(f, "\t<methodgroups>\n");
		for(group_id = e_nso_get_next_method_group(o_node, 0); group_id != (uint16)-1 ; group_id = e_nso_get_next_method_group(o_node, group_id + 1))
		{
			fprintf(f, "\t\t<methodgroup name=\"%s\">\n", e_nso_get_method_group(o_node, group_id));
			for(method_id = e_nso_get_next_method(o_node, group_id, 0); method_id != (uint16)-1 ; method_id = e_nso_get_next_method(o_node, group_id, method_id + 1))
			{
				fprintf(f, "\t\t\t<method name=\"%s\">\n", e_nso_get_method(o_node, group_id, method_id));
				for(i = 0; i < e_nso_get_method_param_count(o_node, group_id, method_id); i++)
				{
					fprintf(f, "\t\t\t<methodparam name=\"%s\" type=\"%s\"/>\n", e_nso_get_method_param_names(o_node, group_id, method_id)[i], method_types[e_nso_get_method_param_types(o_node, group_id, method_id)[i]]);
				}
				fprintf(f, "\t\t\t</method>\n");
			}
			fprintf(f, "\t\t</methodgroup>\n");
		}
		fprintf(f, "\t</methodgroups>\n");
	}
}

static void save_geometry(FILE *f, ENode *g_node)
{
	static const char *layer_type[] = {"VN_G_LAYER_VERTEX_XYZ", "VN_G_LAYER_VERTEX_UINT32", "VN_G_LAYER_VERTEX_REAL",
		"VN_G_LAYER_POLYGON_CORNER_UINT32", "VN_G_LAYER_POLYGON_CORNER_REAL", "VN_G_LAYER_POLYGON_FACE_UINT8",
		"VN_G_LAYER_POLYGON_FACE_UINT32", "VN_G_LAYER_POLYGON_FACE_REAL"};
	EGeoLayer *layer;
	uint *vertex_holes = NULL, *polygon_holes = NULL;
	uint i, vertex_count = 0, poly_count = 0;
	uint *ref;
	egreal *vertex;
	void *data;
	uint16 bone_id;

	fprintf(f, "\t<layers>\n");
	vertex_count = e_nsg_get_vertex_length(g_node);
	poly_count = e_nsg_get_polygon_length(g_node);
	vertex = e_nsg_get_layer_data(g_node, e_nsg_get_layer_by_id(g_node,  0));
	ref = e_nsg_get_layer_data(g_node, e_nsg_get_layer_by_id(g_node,  1));

	for(layer = e_nsg_get_layer_next(g_node, 0); layer != NULL; layer = e_nsg_get_layer_next(g_node, e_nsg_get_layer_id(layer) + 1))
	{
		data = e_nsg_get_layer_data(g_node, layer);
		switch(e_nsg_get_layer_type(layer))
		{
			case VN_G_LAYER_VERTEX_XYZ :
				fprintf(f, "\t\t<layer name=\"%s\" type=\"VN_G_LAYER_VERTEX_XYZ\">\n", e_nsg_get_layer_name(layer));
				for(i = 0; i < vertex_count; i++)
					if(vertex[i * 3] != E_REAL_MAX)
						fprintf(f, "\t\t\t<v>%u %f %f %f</v>\n", i, ((egreal *)data)[i * 3], ((egreal *)data)[i * 3 + 1], ((egreal *)data)[i * 3 + 2]);
			break;
			case VN_G_LAYER_VERTEX_UINT32 :
				fprintf(f, "\t\t<layer name=\"%s\" type=\"VN_G_LAYER_VERTEX_UINT32\">\n", e_nsg_get_layer_name(layer));
				for(i = 0; i < vertex_count; i++)
					if(vertex[i * 3] != E_REAL_MAX)
						fprintf(f, "\t\t\t<v>%u %u</v>\n", i, ((uint32 *)data)[i]);
			break;
			case VN_G_LAYER_VERTEX_REAL :
				fprintf(f, "\t\t<layer name=\"%s\" type=\"VN_G_LAYER_VERTEX_REAL\">\n", e_nsg_get_layer_name(layer));
				for(i = 0; i < vertex_count; i++)
					if(vertex[i * 3] != E_REAL_MAX)
						fprintf(f, "\t\t\t<v>%u %f</v>\n", i, ((egreal *)data)[i]);
			break;
			case VN_G_LAYER_POLYGON_CORNER_UINT32 :
				fprintf(f, "\t\t<layer name=\"%s\" type=\"VN_G_LAYER_POLYGON_CORNER_UINT32\">\n", e_nsg_get_layer_name(layer));
				for(i = 0; i < poly_count; i++)
					if(ref[i * 4] < vertex_count && vertex[ref[i * 4] * 3] != E_REAL_MAX &&
						ref[i * 4 + 1] < vertex_count && vertex[ref[i * 4 + 1] * 3] != E_REAL_MAX &&
						ref[i * 4 + 2] < vertex_count && vertex[ref[i * 4 + 2] * 3] != E_REAL_MAX)
						fprintf(f, "\t\t\t<p>%u %u %u %u</p>\n", ((uint32 *)data)[i * 4], ((uint32 *)data)[i * 4 + 1], ((uint32 *)data)[i * 4 + 2], ((uint32 *)data)[i * 4 + 3]);
			break;
			case VN_G_LAYER_POLYGON_CORNER_REAL :
				fprintf(f, "\t\t<layer name=\"%s\" type=\"VN_G_LAYER_POLYGON_CORNER_REAL\">\n", e_nsg_get_layer_name(layer));
				for(i = 0; i < poly_count; i++)
					if(ref[i * 4] < vertex_count && vertex[ref[i * 4] * 3] != E_REAL_MAX &&
						ref[i * 4 + 1] < vertex_count && vertex[ref[i * 4 + 1] * 3] != E_REAL_MAX &&
						ref[i * 4 + 2] < vertex_count && vertex[ref[i * 4 + 2] * 3] != E_REAL_MAX)
						fprintf(f, "\t\t\t<p>%f %f %f %f</p>\n", ((egreal *)data)[i * 4], ((egreal *)data)[i * 4 + 1], ((egreal *)data)[i * 4 + 2], ((egreal *)data)[i * 4 + 3]);
			break;
			case VN_G_LAYER_POLYGON_FACE_UINT8 :
				fprintf(f, "\t\t<layer name=\"%s\" type = \"VN_G_LAYER_POLYGON_FACE_UINT8\">\n", e_nsg_get_layer_name(layer));
				for(i = 0; i < poly_count; i++)
					if(ref[i * 4] < vertex_count && vertex[ref[i * 4] * 3] != E_REAL_MAX &&
						ref[i * 4 + 1] < vertex_count && vertex[ref[i * 4 + 1] * 3] != E_REAL_MAX &&
						ref[i * 4 + 2] < vertex_count && vertex[ref[i * 4 + 2] * 3] != E_REAL_MAX)
						fprintf(f, "\t\t\t<p>%u</p>\n", ((uint8 *)data)[i]);
			break;
			case VN_G_LAYER_POLYGON_FACE_UINT32 :
				fprintf(f, "\t\t<layer name=\"%s\" type=\"VN_G_LAYER_POLYGON_FACE_UINT32\">\n", e_nsg_get_layer_name(layer));
				for(i = 0; i < poly_count; i++)
					if(ref[i * 4] < vertex_count && vertex[ref[i * 4] * 3] != E_REAL_MAX &&
						ref[i * 4 + 1] < vertex_count && vertex[ref[i * 4 + 1] * 3] != E_REAL_MAX &&
						ref[i * 4 + 2] < vertex_count && vertex[ref[i * 4 + 2] * 3] != E_REAL_MAX)
						fprintf(f, "\t\t\t<p>%u</p>\n", ((uint32 *)data)[i]);
			break;
			case VN_G_LAYER_POLYGON_FACE_REAL :
				fprintf(f, "\t\t<layer name=\"%s\" type=\"VN_G_LAYER_POLYGON_FACE_REAL\">\n", e_nsg_get_layer_name(layer));
				for(i = 0; i < poly_count; i++)
					if(ref[i * 4] < vertex_count && vertex[ref[i * 4] * 3] != E_REAL_MAX &&
						ref[i * 4 + 1] < vertex_count && vertex[ref[i * 4 + 1] * 3] != E_REAL_MAX &&
						ref[i * 4 + 2] < vertex_count && vertex[ref[i * 4 + 2] * 3] != E_REAL_MAX)
						fprintf(f, "\t\t\t<p>%u</p>\n", ((egreal *)data)[i]);
			break;
		}
		fprintf(f, "\t\t</layer>\n");
	}
	fprintf(f, "\t</layers>\n");

	fprintf(f, "\t<vertexcrease layer=\"%s\" default=\"%u\" />\n", e_nsg_get_layer_crease_vertex_name(g_node), e_nsg_get_layer_crease_vertex_value(g_node));
	fprintf(f, "\t<edgecrease layer=\"%s\" default=\"%u\" />\n", e_nsg_get_layer_crease_edge_name(g_node), e_nsg_get_layer_crease_edge_value(g_node));

	fprintf(f, "\t<bones>\n");

	for(bone_id = e_nsg_get_bone_next(g_node, 0); bone_id != (uint16)-1 ; bone_id = e_nsg_get_bone_next(g_node, bone_id + 1))
	{
		double tmp[4];
		fprintf(f, "\t\t<bone weight=\"%s\" reference=\"%s\">\n", e_nsg_get_bone_weight(g_node, bone_id), e_nsg_get_bone_reference(g_node, bone_id));
		fprintf(f, "\t\t\t<bonepos>\n");
		e_nsg_get_bone_pos64(g_node, bone_id, tmp);
		fprintf(f, "\t\t\t\t%f %f %f\n", tmp[0], tmp[1], tmp[2]);
		fprintf(f, "\t\t\t</bonepos>\n");
		fprintf(f, "\t\t\t<bonerot>\n");
		e_nsg_get_bone_rot64(g_node, bone_id, tmp);
		fprintf(f, "\t\t\t\t%f %f %f %f\n", tmp[0], tmp[1], tmp[2], tmp[3]);
		fprintf(f, "\t\t\t</bonerot>\n");
		fprintf(f, "\t\t</bone>\n");
	}
	fprintf(f, "\t</bones>\n");
}

static void save_material(FILE *f, ENode *m_node)
{
	static const char *light_type[] = {"VN_M_LIGHT_DIRECT", "VN_M_LIGHT_AMBIENT", "VN_M_LIGHT_DIRECT_AND_AMBIENT", "VN_M_LIGHT_BACK_DIRECT", "VN_M_LIGHT_BACK_AMBIENT", "VN_M_LIGHT_BACK_DIRECT_AND_AMBIENT"};
	static const char *noise_type[] = {"VN_M_NOISE_PERLIN_ZERO_TO_ONE", "VN_M_NOISE_PERLIN_MINUS_ONE_TO_ONE"};
	static const char *ramp_type[] = {"VN_M_RAMP_SQUARE", "VN_M_RAMP_LINEAR", "VN_M_RAMP_SMOOTH"};
	static const char *ramp_channel[] = {"VN_M_RAMP_RED", "VN_M_RAMP_GREEN", "VN_M_RAMP_BLUE"};
	static const char *blend_type[] = {"VN_M_BLEND_FADE", "VN_M_BLEND_ADD", "VN_M_BLEND_SUBTRACT", "VN_M_BLEND_MULTIPLY", "VN_M_BLEND_DIVIDE", "VN_M_BLEND_DOT"};
	static const char *frag_names[] = {"VN_M_FT_COLOR", "VN_M_FT_LIGHT", "VN_M_FT_REFLECTION", "VN_M_FT_TRANSPARENCY", "VN_M_FT_VOLUME", "VN_M_FT_GEOMETRY", "VN_M_FT_TEXTURE", "VN_M_FT_NOISE", "VN_M_FT_BLENDER", "VN_M_FT_MATRIX", "VN_M_FT_RAMP", "VN_M_FT_ANIMATION", "VN_M_FT_ALTERNATIVE", "VN_M_FT_OUTPUT"};
	VMatFrag *frag;
	VNMFragmentID id;
	uint i;

	fprintf(f, "\t<fragments>\n");
	for(id = e_nsm_get_fragment_next(m_node, 0); id != (uint16)-1 ; id = e_nsm_get_fragment_next(m_node, id + 1))
	{
		frag = e_nsm_get_fragment(m_node, id);
		fprintf(f, "\t\t<fragment id=\"%u\" type=\"%s\">\n", id, frag_names[e_nsm_get_fragment_type(m_node, id)]);
		switch(e_nsm_get_fragment_type(m_node, id))
		{
			case VN_M_FT_COLOR :
				fprintf(f, "\t\t\t<color>%f %f %f</color>\n", 
					frag->color.red, 
					frag->color.green, 
					frag->color.blue);
			break;
			case VN_M_FT_LIGHT :
				fprintf(f,
					"\t\t\t<type>%s</type>\n"
					"\t\t\t<normal_falloff>%f</normal_falloff>\n"
					"\t\t\t<brdf>%u</brdf>\n"
					"\t\t\t<brdf_r>%s</brdf_r>\n"
					"\t\t\t<brdf_g>%s</brdf_g>\n"
					"\t\t\t<brdf_b>%s</brdf_b>\n",
					light_type[frag->light.type],
					frag->light.normal_falloff,
					frag->light.brdf,
					frag->light.brdf_r,
					frag->light.brdf_g,
					frag->light.brdf_b);
			break;
			case VN_M_FT_REFLECTION :
				fprintf(f, "\t\t\t<normal_falloff>%f</normal_falloff>\n",
					frag->reflection.normal_falloff);
			break;
			case VN_M_FT_TRANSPARENCY :
				fprintf(f,
					"\t\t\t<normal_falloff>%f</normal_falloff>\n"
					"\t\t\t<refraction_index>%f</refraction_index>\n",
					frag->transparency.normal_falloff, frag->transparency.refraction_index);
			break;
			case VN_M_FT_VOLUME :
				fprintf(f,
					"\t\t\t<diffusion>%f</diffusion>\n"
					"\t\t\t<col>%f %f %f</col>\n"
					"\t\t\t<color>%u</color>\n",
					frag->volume.diffusion,
					frag->volume.col_r, frag->volume.col_g, frag->volume.col_b,
					frag->volume.color);
			break;
			case VN_M_FT_GEOMETRY :
				fprintf(f,
					"\t\t\t<layer_r>%s</layer_r>\n"
					"\t\t\t<layer_g>%s</layer_g>\n"
					"\t\t\t<layer_b>%s</layer_b>\n",
					frag->geometry.layer_r,
					frag->geometry.layer_g,
					frag->geometry.layer_b);
			break;
			case VN_M_FT_TEXTURE :
				fprintf(f,
					"\t\t\t<bitmap>%u</bitmap>\n"
					"\t\t\tlayer_r>%s</layer_r>\n"
					"\t\t\tlayer_g>%s</layer_g>\n"
					"\t\t\tlayer_b>%s</layer_b>\n"
					"mapping>%u</mapping>\n",
					frag->texture.bitmap,
					frag->texture.layer_r,
					frag->texture.layer_g,
					frag->texture.layer_b,
					frag->texture.mapping);
			break;
			case VN_M_FT_NOISE :
				fprintf(f,
					"\t\t\t<type>%s</type>\n"
					"\t\t\t<mapping>%u</mapping>\n",
					noise_type[frag->noise.type],
					frag->noise.mapping);
			break;
			case VN_M_FT_BLENDER :
				fprintf(f,
					"\t\t\t<type>%s</type>\n"
					"\t\t\t<data_a>%u</data_a>\n"
					"\t\t\t<data_b>%u</data_b>\n"
					"\t\t\t<control>%u</control>\n",
					blend_type[frag->blender.type],
					frag->blender.data_a,
					frag->blender.data_b,
					frag->blender.control);
			break;
			case VN_M_FT_MATRIX :
				fprintf(f,
					"\t\t\t<data>%u</data>\n", frag->matrix.data);
				fprintf(f, "\t\t\t<matrix>\n");
				fprintf(f, "\t\t\t\t%f %f %f %f\n", frag->matrix.matrix[0], frag->matrix.matrix[1], frag->matrix.matrix[2], frag->matrix.matrix[3]);
				fprintf(f, "\t\t\t\t%f %f %f %f\n", frag->matrix.matrix[4], frag->matrix.matrix[5], frag->matrix.matrix[6], frag->matrix.matrix[7]);
				fprintf(f, "\t\t\t\t%f %f %f %f\n", frag->matrix.matrix[8], frag->matrix.matrix[9], frag->matrix.matrix[10], frag->matrix.matrix[11]);
				fprintf(f, "\t\t\t\t%f %f %f %f\n", frag->matrix.matrix[12], frag->matrix.matrix[13], frag->matrix.matrix[14], frag->matrix.matrix[15]);
				fprintf(f, "\t\t\t</matrix>\n");
			break;
			case VN_M_FT_RAMP :
				fprintf(f,
					"\t\t\t<type>%s</ramp>\n"
					"\t\t\t<channel>%s</channel>\n"
					"\t\t\t<mapping>%u</mapping>\n",
					ramp_type[frag->ramp.type],
					ramp_channel[frag->ramp.channel],
					frag->ramp.mapping);
				fprintf(f, "\t\t\t<ramp>\n");
				for(i = 0; i < frag->ramp.point_count; i++)
					fprintf(f, "\t\t\t\t<ramppoint><color>%f %f %f</color><pos>%f</pos></ramppoint>\n",
						frag->ramp.ramp[i].red,
						frag->ramp.ramp[i].green,
						frag->ramp.ramp[i].blue,
						frag->ramp.ramp[i].pos);
				fprintf(f, "\t\t\t</ramp>\n");
			break;
			case VN_M_FT_ANIMATION :
				fprintf(f, "\t\t\t<label>%s</label>\n", frag->animation.label);
			break;
			case VN_M_FT_ALTERNATIVE :
				fprintf(f,
					"\t\t\t<alt_a>%u</alt_a>\n"
					"\t\t\t<alt_b>%u</alt_b>>\n",
					frag->alternative.alt_a,
					frag->alternative.alt_b);
			break;
			case VN_M_FT_OUTPUT :
				fprintf(f,
					"\t\t\t<label>%s</label>\n"
					"\t\t\t<front>%u</front>\n"
					"\t\t\t<back>%u</back>\n",
					frag->output.label,
					frag->output.front,
					frag->output.back);
			break;
		}
		fprintf(f, "\t\t</fragment>\n");
	}
	fprintf(f, "\t</fragments>\n");
}

static void tile_get(VNBTile *tile, int x, int y, int z, const void *data, VNBLayerType type, const uint *size)
{
	uint32	wt = (size[0] + VN_B_TILE_SIZE - 1) / VN_B_TILE_SIZE, ht = (size[1] + VN_B_TILE_SIZE - 1) / VN_B_TILE_SIZE,
		tw = x == (wt - 1) && (size[0] % VN_B_TILE_SIZE) != 0 ? size[0] % VN_B_TILE_SIZE : VN_B_TILE_SIZE,
		th = y == (ht - 1) && (size[1] % VN_B_TILE_SIZE) != 0 ? size[1] % VN_B_TILE_SIZE : VN_B_TILE_SIZE,
		zoff, tx, ty, put, get;

	zoff = z * size[0] * size[1];	/* Z offset, in pixels. */

	printf("tile (%u,%u) is %ux%u\n", x, y, tw, th);
	memset(tile, 0, sizeof *tile);
	for(ty = 0; ty < th; ty++)
	{
		get = zoff + (y * VN_B_TILE_SIZE + ty) * size[0] + x * VN_B_TILE_SIZE;
		put = ty * VN_B_TILE_SIZE;	/* Reset for each row, as they might be short. */
		for(tx = 0; tx < tw; tx++, put++, get++)
		{
			if(type == VN_B_LAYER_UINT1)
				tile->vuint1[put] = 0;	/* FIXME: Code missing here. Not certain of Enough's 1-bit format. */
			else if(type == VN_B_LAYER_UINT8)
				tile->vuint8[put] = ((uint8 *) data)[get];
			else if(type == VN_B_LAYER_UINT16)
				tile->vuint16[put] = ((uint16 *) data)[get];
			else if(type == VN_B_LAYER_REAL32)
				tile->vreal32[put] = ((real32 *) data)[get];
			else if(type == VN_B_LAYER_REAL64)
				tile->vreal64[put] = ((real64 *) data)[get];
		}
	}
}

static void save_bitmap(FILE *f, ENode *b_node)
{
	static const char *layer_type[] = {"VN_B_LAYER_UINT1", "VN_B_LAYER_UINT8", "VN_B_LAYER_UINT16", "VN_B_LAYER_REAL32", "VN_B_LAYER_REAL64"};
	EBitLayer *layer;
	uint size[3], i, j, k, start, tiles[2], tx, ty;
	void *data;
	VNBTile	tile;
	VNBLayerType	type;

	e_nsb_get_size(b_node, &size[0], &size[1], &size[2]);
	fprintf(f, "\t<dimensions>%u %u %u</dimensions>\n", size[0], size[1], size[2]);
	fprintf(f, "\t<layers>\n");
	
	tiles[0] = (size[0] + VN_B_TILE_SIZE - 1) / VN_B_TILE_SIZE;
	tiles[1] = (size[1] + VN_B_TILE_SIZE - 1) / VN_B_TILE_SIZE;
	
	for(layer = e_nsb_get_layer_next(b_node, 0); layer != NULL; layer = e_nsb_get_layer_next(b_node, e_nsb_get_layer_id(layer) + 1))
	{
		fprintf(f, "\t\t<layer name=\"%s\" type=\"%s\">\n", e_nsb_get_layer_name(layer), layer_type[e_nsb_get_layer_type(layer)]);
		fprintf(f, "\t\t<tiles>\n");
		data = e_nsb_get_layer_data(b_node, layer);
		type = e_nsb_get_layer_type(layer);

		for(i = 0; i < size[2]; i++)
		{
			for(j = 0; j < tiles[1]; j++)
			{
				for(k = 0; k < tiles[0]; k++)
				{
					fprintf(f, "\t\t<tile tile_x=\"%u\" tile_y=\"%u\" tile_z=\"%u\">\n", k, j, i);
					tile_get(&tile, k, j, i, data, e_nsb_get_layer_type(layer), size);
					for(ty = 0; ty < VN_B_TILE_SIZE; ty++)
					{
						fprintf(f, "\t\t");
						for(tx = 0; tx < VN_B_TILE_SIZE; tx++)
						{
							if(type == VN_B_LAYER_UINT1)
								fprintf(f, " %c", tile.vuint1[ty * VN_B_TILE_SIZE / CHAR_BIT] & (1 << (CHAR_BIT - tx - 1)) ? '1' : '0');
							else if(type == VN_B_LAYER_UINT8)
								fprintf(f, " %u", tile.vuint8[ty * VN_B_TILE_SIZE + tx]);
							else if(type == VN_B_LAYER_UINT16)
								fprintf(f, " %u", tile.vuint16[ty * VN_B_TILE_SIZE + tx]);
							else if(type == VN_B_LAYER_REAL32)
								fprintf(f, " %g", tile.vreal32[ty * VN_B_TILE_SIZE + tx]);
							else if(type == VN_B_LAYER_REAL64)
								fprintf(f, " %g", tile.vreal64[ty * VN_B_TILE_SIZE + tx]);
						}
						fprintf(f, "\n");
					}
					fprintf(f, "\t\t</tile>\n");
				}
			}
		}
		fprintf(f, "\t\t</tiles>\n");
		fprintf(f, "\t\t</layer>\n");
	}
	fprintf(f, "\t</layers>\n");
}

static void save_text(FILE *f, ENode *t_node)
{
	ETextBuffer *buffer;

	fprintf(f, "\t<language>%s</language>\n", e_nst_get_language(t_node));
	fprintf(f, "\t<buffers>\n");
	for(buffer = e_nst_get_buffer_next(t_node, 0); buffer != NULL; buffer = e_nst_get_buffer_next(t_node, e_nst_get_buffer_id(buffer) + 1))
	{
		fprintf(f, "\t<buffer>\n");
		fprintf(f, "%s", e_nst_get_buffer_data(t_node, buffer));
		fprintf(f, "\t</buffer>\n");
	}
	fprintf(f, "\t</buffers>\n");
}

static void save_curve(FILE *f, ENode *c_node)
{
	ECurve *curve;
	real64 pre_value[4];
	uint32 pre_pos[4];
	real64 value[4];
	real64 pos;
	real64 post_value[4];
	uint32 post_pos[4];
	uint i, j;

	fprintf(f, "\t<curves>\n");
	for(curve = e_nsc_get_curve_next(c_node, 0); curve != NULL; curve = e_nsc_get_curve_next(c_node, e_nsc_get_curve_id(curve) + 1))
	{
		fprintf(f, "\t\t<curve name=\"%s\" dimensions=\"%u\">\n", e_nsc_get_curve_name(curve), e_nsc_get_curve_dimensions(curve));
		for(i = e_nsc_get_point_next(curve, 0); i != -1; i = e_nsc_get_point_next(curve, i + 1))
		{
			fprintf(f, "\t\t\t<point>\n\t\t\t\t");
			e_nsc_get_point(curve, i, pre_value, pre_pos, value, &pos, post_value, post_pos);
			for(j = 0; j < e_nsc_get_curve_dimensions(curve); j++)
				fprintf(f, "%f ", pre_value[j]);
			fprintf(f, "\n\t\t\t\t");
			for(j = 0; j < e_nsc_get_curve_dimensions(curve); j++)
				fprintf(f, "%u ", pre_pos[j]);
			fprintf(f, "\n\t\t\t\t");
			for(j = 0; j < e_nsc_get_curve_dimensions(curve); j++)
				fprintf(f, "%f ", value[j]);
			fprintf(f, "\n\t\t\t\t%f\n\t\t\t\t", pos);
			for(j = 0; j < e_nsc_get_curve_dimensions(curve); j++)
				fprintf(f, "%f ", post_value[j]);
			fprintf(f, "\n\t\t\t\t");
			for(j = 0; j < e_nsc_get_curve_dimensions(curve); j++)
				fprintf(f, "%u ", post_pos[j]);
			fprintf(f, "\n\t\t\t</point>\n");
		}
		fprintf(f, "\t\t</curve>\n");
	}
	fprintf(f, "\t</curves>\n");
}

static void save_node(FILE *f, ENode *node)
{
	static const char *node_type[] = {"V_NT_OBJECT", "V_NT_GEOMETRY", "V_NT_MATERIAL", "V_NT_BITMAP", "V_NT_TEXT", "V_NT_CURVE", "V_NT_AUDIO"};
	static const char *tag_type[] = {"VN_TAG_BOOLEAN", "VN_TAG_UINT32", "VN_TAG_REAL64", "VN_TAG_STRING", "VN_TAG_REAL64_VEC3", "VN_TAG_LINK", "VN_TAG_ANIMATION", "VN_TAG_BLOB", "VN_TAG_TYPE_COUNT"};
	uint16 group_id, tag_id;
	uint i;
	VNTag *tag;

	fprintf(f, "<node id=\"%u\" type=\"%s\" name=\"%s\">\n", e_ns_get_node_id(node), node_type[e_ns_get_node_type(node)], e_ns_get_node_name(node));
	if(e_ns_get_next_tag_group(node, 0) != (uint16)-1)
	{
		fprintf(f, "\t<tags>\n");

		for(group_id = e_ns_get_next_tag_group(node, 0); group_id != (uint16)-1 ; group_id = e_ns_get_next_tag_group(node, group_id + 1))
		{
			fprintf(f, "\t\t<taggroup name=\"%s\">\n", e_ns_get_tag_group(node, group_id));
			for(tag_id = e_ns_get_next_tag(node, group_id, 0); tag_id != (uint16)-1 ; tag_id = e_ns_get_next_tag(node, group_id, tag_id + 1))
			{
				fprintf(f, "\t\t\t<tag name=\"%s\" type=\"%s\">\n", e_ns_get_tag_name(node, group_id, tag_id), tag_type[e_ns_get_tag_type(node, group_id, tag_id)]);
				tag = e_ns_get_tag(node, group_id, tag_id);
				switch(e_ns_get_tag_type(node, group_id, tag_id))
				{
					case VN_TAG_BOOLEAN :
						if(tag->vboolean)
							fprintf(f, "\t\t\t\t<data> TRUE\n");
						else
							fprintf(f, "\t\t\t\t<data> TRUE\n");
					break;
					case VN_TAG_UINT32 :
						fprintf(f, "\t\t\t\t<data> %u\n", tag->vuint32);
					break;
					case VN_TAG_REAL64 :
						fprintf(f, "\t\t\t\t<data> %f\n", tag->vreal64);
					break;
					case VN_TAG_STRING :
						fprintf(f, "\t\t\t\t<data> %s\n", tag->vstring);
					break;
					case VN_TAG_REAL64_VEC3 :
						fprintf(f, "\t\t\t\t<data> %f %f %f\n", tag->vreal64_vec3[0], tag->vreal64_vec3[1], tag->vreal64_vec3[2]);
					break;
					case VN_TAG_LINK :
						fprintf(f, "\t\t\t\t<data> %u\n", tag->vlink);
					break;
					case VN_TAG_BLOB :
						fprintf(f, "\t\t\t\t<data> ");
						for(i = 0; i < tag->vblob.size; i++)
							fprintf(f, "%u ", ((uint8 *)tag->vblob.blob)[i]);
						fprintf(f, "\n");
					break;
				}
				fprintf(f, "\t\t\t\t</data> ");
				fprintf(f, "\t\t\t</tag>\n");
			}
			fprintf(f, "\t\t</taggroup>\n");
		}
		fprintf(f, "\t</tags>\n");
	}
	switch(e_ns_get_node_type(node))
	{
		case V_NT_OBJECT :
			save_object(f, node);
		break;
		case V_NT_GEOMETRY :
			save_geometry(f, node);
		break;
		case V_NT_MATERIAL :
			save_material(f, node);
		break;
		case V_NT_BITMAP :
			save_bitmap(f, node);
		break;
		case V_NT_TEXT :
			save_text(f, node);
		break;
		case V_NT_CURVE :
			save_curve(f, node);
		break;
		case V_NT_AUDIO :
		break;
	}
	fprintf(f, "</node>\n\n");

}

static void save_data(FILE *f)
{
	ENode *node;
	uint i;

	fprintf(f, "<?xml version=\"1.0\"?>\n\n");
	fprintf(f, "<vml>\n");
	for(i = 0; i < V_NT_NUM_TYPES; i++)
		for(node = e_ns_get_node_next(0, 0, i); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, i))
			save_node(f, node);
	fprintf(f, "</vml>\n\n");
	fclose(f);
}

static void download_data(void)
{
	void *layer;
	ENode *node;

	for(node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
		for(layer = e_nsg_get_layer_next(node, 0); layer != NULL; layer = e_nsg_get_layer_next(node, e_nsg_get_layer_id(layer) + 1))
			e_nsg_get_layer_data(node, layer);
	for(node = e_ns_get_node_next(0, 0, V_NT_BITMAP); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_BITMAP))
		for(layer = e_nsb_get_layer_next(node, 0); layer != NULL; layer = e_nsb_get_layer_next(node, e_nsb_get_layer_id(layer) + 1))
			e_nsb_get_layer_data(node, layer);
}

static const char * find_param(int argc, char **argv, const char *option, const char *default_text)
{
	int i;

	for(i = 1; i < argc - 1; i++)
		if(strcmp(argv[i], option) == 0)
			return argv[i + 1];
	return default_text;
}

int main(int argc, char **argv)
{
	uint32 i, seconds, s, interval = 10;
	const char *name, *pass, *address, *file;
	FILE *f;
	int	repeat = 0;

	enough_init();
	name = find_param(argc, argv, "-n", "saver");
	pass = find_param(argc, argv, "-p", "pass");
	address = find_param(argc, argv, "-a", "localhost");
	file = find_param(argc, argv, "-f", "dump.vml");
	repeat = find_param(argc, argv, "-r", NULL) != NULL;

	for(i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "help") == 0)
		{
			printf("usage: verse_saver <pamams>\n");
			printf("params:\n");
			printf("-h This text.\n");
			printf("-n <name>\n");
			printf("-p <pass>\n");
			printf("-a <address>\n");
			printf("-f <filename>\n");
			printf("-i <save interval in seconds>\n");
			printf("-1 Save only once, then exit\n");
			return FALSE;
		}
	}
	if(e_vc_connect(address, name, pass, NULL) == -1)
	{
		printf("Error: Invalid server address '%s'\n", address);
		return TRUE;
	}
	printf("Connecting to %s\n", address);
	while(interval != 0)
	{
		verse_session_get_time(&seconds, NULL);
		s = seconds;
		while(seconds < s + interval/* && verse_session_get_size() == 0*/)
		{
			verse_callback_update(1000);
			download_data();
			verse_session_get_time(&seconds, NULL);
		//	printf(".");
		}
		if((f = fopen(file, "w")) != NULL)
			save_data(f);
		printf("Save complete\n");
		if(!repeat)
			break;
	}
	return TRUE;
}
