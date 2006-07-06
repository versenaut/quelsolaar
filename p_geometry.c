
#include "verse.h"

#include "seduce.h"

#include "enough.h"
#include "p_sds_geo.h"
#include "p_task.h"
#include "p_sds_table.h"

typedef enum{
	PGS_COUNT,
	PGS_CLEAN,
	PGS_NEIGHBOR,
	PGS_ALLOCATE_NEXT,
	PGS_DIVIDE,
	PGS_DONE_STAGE,
	PGS_FINAL_CLEAN,
	PGS_READY
}PGeoStage;

uint p_sds_level = 1;
uint p_sds_poly_cap = 100000;
float p_sds_compute_timer = 1000;

void p_geo_set_sds_level(uint level)
{

	if(level < 1)
		level = 1;
	else if(level > p_get_max_tess_level())
		p_sds_level = p_get_max_tess_level();
	else
		p_sds_level = level;
}

uint p_geo_get_sds_level(void)
{
	return p_sds_level;
}

void p_geo_set_sds_poly_cap(uint poly_cap)
{
	p_sds_poly_cap = poly_cap;
}

uint p_geo_get_sds_poly_cap(void)
{
	return p_sds_poly_cap;
}

void p_geo_set_sds_compute_timer(float timer)
{
	p_sds_compute_timer = timer;
}

float p_geo_get_sds_compute_timer(void)
{
	return p_sds_compute_timer;
}

boolean p_geo_sds_compute_func(uint id)
{
	PPolyStore *mesh;
	uint *ref, ref_count, vertex_count, *edge_crease, version; 
	egreal *vertex;
	PPolyStore *old = NULL;
	ENode *node;
	EGeoLayer *p, *v, *e;
	PTimer ttimer;
	float timer = 0;
	if((node = e_ns_get_node(0, id)) == NULL || e_ns_get_node_type(node) != V_NT_GEOMETRY)
		return TRUE;

	p = e_nsg_get_layer_by_id(node,  1);
	v = e_nsg_get_layer_by_id(node,  0);
	e = e_nsg_get_layer_crease_edge_layer(node);
	if(p == 0 || v == 0)
		return TRUE;

	mesh = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
	version = e_ns_get_node_version_struct(node);

	if(mesh != NULL)
	{
		if(mesh->version == version && mesh->stage[0] == PGS_READY)
		{
			return TRUE;
		}
		if(mesh->version != version/* && mesh->stage[0] != PGS_READY*/)
		{
			p_sds_free(mesh, FALSE);
			mesh = NULL;
		}
	}
	ref = e_nsg_get_layer_data(node, p);
	ref_count = e_nsg_get_polygon_length(node);
	vertex = e_nsg_get_layer_data(node, v);
	vertex_count = e_nsg_get_vertex_length(node);
	edge_crease = e_nsg_get_layer_data(node, e);

	if(mesh == NULL)
		mesh = p_sds_create(ref, ref_count, vertex, vertex_count, version);


	p_timer_start(&ttimer);

	while(p_timer_elapsed(&ttimer) < 0.05)
	{
		switch(mesh->stage[0])
		{
			case PGS_COUNT :

				timer += p_sds_stage_count_poly(mesh, ref, ref_count, vertex, vertex_count, 1.0 - ((egreal)e_nsg_get_layer_crease_edge_value(node) / 4294967295.0));
				
				if(mesh->base_quad_count + mesh->base_tri_count == 0)
				{
					p_sds_free(mesh, FALSE);
					e_ns_set_custom_data(node, P_ENOUGH_SLOT, NULL);
					return TRUE;
				}
				break;
			case PGS_CLEAN :
				if(edge_crease == NULL)
					timer += p_sds_stage_clean_poly(mesh, ref, ref_count, vertex, vertex_count);
				else
					timer += p_sds_stage_clean_poly_cerease(mesh, ref, ref_count, vertex, vertex_count, edge_crease);
			break;
			case PGS_NEIGHBOR :
				timer += p_sds_compute_neighbor(mesh);
				mesh->stage[0]++;
			break;
			case PGS_ALLOCATE_NEXT :
				timer += 100;
				p_sds_allocate_next(mesh);
				mesh->stage[0]++;
				mesh->stage[1] = 0;
			break;
			case PGS_DIVIDE :
				timer += p_sds_divide(mesh);		
			break;
			case PGS_DONE_STAGE :
				((PPolyStore *)mesh->next)->next = mesh;
				mesh = mesh->next;
				p_sds_free(mesh->next, TRUE);
				mesh->next = NULL;
				if(mesh->level == p_sds_level || (mesh->level > 1 && mesh->quad_length / 2 + mesh->tri_length / 3 > p_sds_poly_cap))
					mesh->stage[0] = PGS_FINAL_CLEAN;
				else
					mesh->stage[0] = PGS_ALLOCATE_NEXT;
			break;
			case PGS_FINAL_CLEAN :
				p_sds_final_clean(mesh);
			break;
			case PGS_READY :
				e_ns_set_custom_data(node, P_ENOUGH_SLOT, mesh);
				return TRUE;
		}
	}
	e_ns_set_custom_data(node, P_ENOUGH_SLOT, mesh);
	return FALSE;
}

void p_geometry_func(ENode *node, ECustomDataCommand command)
{	
	PPolyStore *geometry;
	switch(command)
	{
		case E_CDC_STRUCT :
			geometry = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
			if(geometry != NULL && geometry->stage[0] == PGS_READY)
				p_task_add(e_ns_get_node_id(node), 1, p_geo_sds_compute_func);
		break;
		case E_CDC_DESTROY :
			geometry = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
			if(geometry != NULL)
				p_sds_free(geometry, FALSE);
		break;
	}
}

PPolyStore *p_sds_get_mesh(ENode *node)
{
	PPolyStore *geometry;
	if(node == NULL || V_NT_GEOMETRY !=	e_ns_get_node_type(node))
		return NULL;
	geometry = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
	if(geometry == NULL)
	{
		p_task_add(e_ns_get_node_id(node), 1, p_geo_sds_compute_func);
		return NULL;
	}
	if(geometry->stage[0] != PGS_READY)
		return NULL;
	return geometry;
}

