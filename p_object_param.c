#include <stdio.h>
#include <stdlib.h>

#include "p_geo_subdivide_internal.h"


typedef struct{
	uint length;
	uint version[3];
	uint compleat[3];
}PGeoParam;

typedef struct{
	egreal **arrays;
	PGeoParam *data;
	uint count;
	uint version;
}PGeoParams;

/*extern uint p_create_material_array(PMesh *mesh, egreal *output, uint32 *reference, uint32 ref_length, uint32 cv_count, uint slot, void *data, uint type, uint start);*/

extern uint p_shader_get_param_count(ENode *node);

PGeoParams *p_param_array_allocate(uint32 geometry, uint32 material)
{
	ENode *node;
	uint i, j, count;
	PGeoParams *p;
	if((node = e_ns_get_node(0, material)) != NULL)
		count = p_shader_get_param_count(node);
	else
		count = 0;
	p = malloc(sizeof *p);
	if(count == 0)
	{
		p->arrays = NULL;
		p->data = NULL;
	}else
	{
		p->arrays = malloc((sizeof *p->arrays) * count);
		p->data = malloc((sizeof *p->data) * count);
	}
	p->count = count;
	p->version = 0;
	for(i = 0; i < count; i++)
	{
		p->arrays[i] = NULL;
		p->data[i].length = 0;
		for(j = 0; j < 3; j++)
		{
			p->data[i].version[j] = 0;
			p->data[i].compleat[j] = 0;
		}
	}
	return p;
}

extern uint p_shader_get_param_count(ENode *node);
extern VMatFrag *p_shader_get_param(ENode *node, uint nr);

void p_param_array_destroy(PGeoParams *p)
{
	uint i;
	for(i = 0; i < p->count; i++)
			free(p->arrays);
	if(p->count > 0)
	{
		free(p->arrays);
		free(p->data);
	}
	free(p);
}

boolean p_array_update(PGeoParams *p, PMesh *mesh, uint32 geometry, uint32 material)
{
	ENode *g_node, *m_node;
	EGeoLayer *layer, *ref_layer;
	char *names[3];
	uint i, j;
	VMatFrag *frag;
	if((m_node = e_ns_get_node(0, material)) == NULL)
		return FALSE;
	if((g_node = e_ns_get_node(0, geometry)) == NULL)
		return FALSE;

	if(p->version == e_ns_get_node_version_data(g_node) + e_ns_get_node_version_data(m_node))
		return FALSE;

	if(p->count != p_shader_get_param_count(m_node))
	{
		if(p_shader_get_param_count(m_node) == 0)
		{
			for(i = 0; i < p->count; i++)
					free(p->arrays);
			if(p->count > 0)
			{
				free(p->arrays);
				free(p->data);
				p->arrays = NULL;
				p->data = NULL;
			}
		}else
		{
			p->arrays = realloc(p->arrays, (sizeof *p->arrays) * p_shader_get_param_count(m_node));
			p->data = realloc(p->data, (sizeof *p->data) * p_shader_get_param_count(m_node));
			for(i = p->count; i < p_shader_get_param_count(m_node); i++)
			{
				p->arrays[i] = NULL;
				p->data[i].length = 0;
				for(j = 0; j < 3; j++)
				{
					p->data[i].version[j] = 0;
					p->data[i].compleat[j] = 0;
				}
			}
		}
	}

	for(i = 0; i < p->count; i++)
	{
		if(p->arrays[i] != NULL && p->data[i].length != mesh->render.vertex_count)
		{
			free(p->arrays[i]);
			p->arrays[i] = NULL;
		}
	}	

	for(i = 0; i < p->count; i++)
	{
		frag = p_shader_get_param(m_node, i);
		if(p->arrays[i] == NULL)
		{
			p->data[i].length = mesh->render.vertex_count;
			p->arrays[i] = malloc((sizeof *p->arrays[i]) * 3 * p->data[i].length);
			for(j = 0; j < 3 * p->data[i].length; j++)
				p->arrays[i][j] = 0;
			return TRUE;
		}
		names[0] = frag->geometry.layer_r;
		names[1] = frag->geometry.layer_g;
		names[2] = frag->geometry.layer_b;
		for(j = 0; j < 3; j++)
		{
			layer = e_nsg_get_layer_by_name(g_node, names[j]);
			if(layer != NULL && (p->data[i].version[j] != e_nsg_get_layer_version(layer) || p->data[i].compleat[j] < e_nsg_get_polygon_length(g_node)))
			{
				if(p->data[i].version[j] != e_nsg_get_layer_version(layer))
					p->data[i].compleat[j] = 0;
/*				p->data[i].compleat[j] = p_create_material_array(mesh, p->arrays[i], e_nsg_get_layer_data(g_node, e_nsg_get_layer_by_id(g_node, 1)), e_nsg_get_polygon_length(g_node), e_nsg_get_vertex_length(g_node), j, e_nsg_get_layer_data(g_node, layer), e_nsg_get_layer_type(layer), p->data[i].compleat[j]);*/
				p->data[i].version[j] = e_nsg_get_layer_version(layer);
				return TRUE;
			}
		}

	}
	p->version = e_ns_get_node_version_data(g_node) + e_ns_get_node_version_data(m_node);
	return TRUE;
}

egreal **p_param_get_array(PGeoParams *p)
{
	return p->arrays;
}
