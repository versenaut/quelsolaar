#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "p_geo_subdivide_internal.h"
#include "p_task.h"

extern void print_dependency(SubMesh *geometry);

SubMesh *p_geo_sub_tesselate_mesh(ENode *node, unsigned int level)
{
	uint32 i, cv_count, extra_tri, extra_quad;
	ebreal *crease_data;
	SubMesh *geometry, *last_geometry, *temp;

	unsigned int *reference;
	unsigned int reference_count;
	unsigned int vertex_count;
	unsigned int *vertex_crease; 
	unsigned int *edge_crease;
	unsigned int default_vertex_crease;
	unsigned int default_edge_crease;

	e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node, 0));
	reference = e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node, 1));
	reference_count = e_nsg_get_polygon_legnth(node);
	vertex_count = e_nsg_get_vertex_legnth(node);
	vertex_crease = NULL; 
	edge_crease = NULL;
	default_vertex_crease = 0;
	default_edge_crease = 0;
/*
	printf("layer %p %p\n", e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node, 0)), e_nsg_get_layer_by_id(node, 0));
	printf("layer %p %p\n", e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node, 1)), e_nsg_get_layer_by_id(node, 1));

	printf("type %u %u\n", e_ns_get_node_type(node), V_NT_GEOMETRY);

	printf("ref %p %u %u\n", reference, reference_count, vertex_count);
*/
	if(level > p_get_max_tess_level())
		level = p_get_max_tess_level();

	last_geometry = mech_cleanup(reference, reference_count, vertex_count, vertex_crease, edge_crease, default_vertex_crease, default_edge_crease, level);
	if(last_geometry == NULL)
		return NULL;

	geometry = allocate_mech(last_geometry, level);

	cv_count = last_geometry->vertex_count;
	geometry->vertex_count = last_geometry->cv_count;

	for(i = 1 ; i <= level ; i++)
	{ 
		geometry->tri_count = 4 * last_geometry->tri_count;
		geometry->quad_count = 4 * last_geometry->quad_count;

		geometry->vertex_count = 0;
		extra_tri = tri_backref(last_geometry);
		extra_quad = quad_backref(last_geometry);

		if(i == 1)
		{
			crease_data = create_vertex_crease_data(last_geometry, NULL, extra_tri + extra_quad);
//			create_vertex_normal_data(last_geometry, extra_tri, extra_quad);
		}
		rop_cv_vertexref(geometry, last_geometry, cv_count, crease_data);

		rop_vertexref(geometry, last_geometry, cv_count, i);			      
		rop_faceref(geometry, last_geometry);
		rop_edgeref(geometry, last_geometry, i);
		if(i > 1)
			rop_sub_re_depend(geometry, last_geometry);
		geometry->normal_tri_ref = last_geometry->normal_tri_ref;
		geometry->normal_quad_ref = last_geometry->normal_quad_ref;
		geometry->group_count_list = last_geometry->group_count_list;
		geometry->cross_product_list = last_geometry->cross_product_list;
		clear_mesh(last_geometry);
		temp = geometry;
		geometry = last_geometry;
		last_geometry = temp;
	}
	free(crease_data);
	geometry->normal_tri_ref = NULL;
	geometry->normal_quad_ref = NULL;
	geometry->group_count_list = 0;
	geometry->cross_product_list = 0;
	p_geo_destroy_sub_mesh(geometry);
	last_geometry->base_level = level;
	last_geometry->cv_count = cv_count;
	last_geometry->version = e_ns_get_node_version_struct(node);
	last_geometry->next = NULL;
	return last_geometry;
}

uint persuade_tess_lever = 3;

boolean p_geometry_compute(uint node_id)
{
	ENode *node;
	SubMesh *geometry;
	if((node = e_ns_get_node(0, node_id)) == NULL)
		return TRUE;
	geometry = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
	if(geometry != NULL)
		p_geo_destroy_sub_mesh(geometry);
	e_ns_set_custom_data(node, P_ENOUGH_SLOT, p_geo_sub_tesselate_mesh(node, persuade_tess_lever));
	return TRUE;
}


SubMesh *p_geometry_get_sds(ENode *node)
{
	SubMesh *geometry;
	geometry = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
	if(geometry == NULL)
	{
		geometry = p_geo_sub_tesselate_mesh(node, persuade_tess_lever);
		printf("geometry %p %p\n", geometry, node);
		e_ns_set_custom_data(node, P_ENOUGH_SLOT, geometry);
		return geometry;
	}
	return geometry;
}

void p_set_tess_level(uint level)
{
/*	SubMesh *geometry;
	ENode *node;
*/	if(persuade_tess_lever < level)
	{
/*		for(node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
			if((geometry == e_ns_get_custom_data(node, P_ENOUGH_SLOT)) != NULL)
				geometry->version--;
*/	}
	persuade_tess_lever = level;
}

void p_geometry_func(ENode *node, ECustomDataCommand command)
{	
	SubMesh *geometry;
	switch(command)
	{
		case E_CDC_STRUCT :
			geometry = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
			if(geometry != NULL && geometry->version != e_ns_get_node_version_struct(node))
				p_task_add(e_ns_get_node_id(node), 1, p_geometry_compute);
		break;
		case E_CDC_DESTROY :
			geometry = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
			if(geometry != NULL)
				p_geo_destroy_sub_mesh(geometry);
		break;
	}
}
