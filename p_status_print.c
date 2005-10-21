
#include "verse.h"
#include "seduce.h"
#include "enough.h"
#include "p_sds_geo.h"
#include "p_sds_obj.h"
#include "p_task.h"
#include "p_object.h"
/*
typedef struct{
	uint32	id;
	uint32	material;
	uint32	target;
	uint	vertex_version;
	PMesh	*draw;	
	PMesh	*progress;
	void	*param;
}PRenderSets;

typedef struct{
	uint32		node_id;
	PRenderSets *sets;
	uint		set_count;
	PRenderSets temp;
	uint32		lights[LIGHTS_PER_OBJECT];
	void		*environment;
	boolean		task;
}PObject;
*/
void p_status_print()
{
	ENode *node;
	PMesh *o_mesh;
	PObject *obj;
	PPolyStore *g_mesh;
	uint version, stage, id, i;
	char text[256];
	
	float line = 0.5;

	sui_draw_text(-0.9, line, SUI_T_SIZE, SUI_T_SPACE, "objects:", 1, 1, 1);
	printf("objects:\n");
	line -= 0.05;
	for(node = e_ns_get_node_next(0, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
	{
		obj = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
		sprintf(text, "nodeid = %u pointer %p count %u \n", e_ns_get_node_id(node), node, obj->mesh_count);
		sui_draw_text(-0.9, line, SUI_T_SIZE, SUI_T_SPACE, text, 1, 1, 1);
		printf(text);
		line -= 0.05;
		for(i = 0; i < obj->mesh_count && obj->meshes[i] != NULL; i++)
		{
			if(p_rm_drawable(obj->meshes[i]))
				sprintf(text, "geo_id %u DRAWABLE\n", p_rm_get_geometry_node(obj->meshes[i]));
			else
				sprintf(text, "geo_id %u INPROGGRES\n", p_rm_get_geometry_node(obj->meshes[i]));
			sui_draw_text(-0.5, line, SUI_T_SIZE, SUI_T_SPACE, text, 1, 1, 1);
			printf(text);
			line -= 0.05;
		}
	}
	sui_draw_text(-0.9, line, SUI_T_SIZE, SUI_T_SPACE, "geometry:", 1, 1, 1);
	printf("geometry:\n");
	line -= 0.05;

	for(node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_GEOMETRY))
	{
		g_mesh = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
		if(g_mesh != NULL)
		{
			version = g_mesh->version;
			stage = g_mesh->stage[0];
			glPushMatrix();
			glScalef(10, 10, 10);
			{
				static float f;
				glRotatef(f, 1, 1, 0.1);
				f += 1;
			}
//			sds_test_draw_2(g_mesh, e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node,  0)));
			glPopMatrix();
		}else
		{	
			version = -1;
			stage = -1;
		}
		sprintf(text, "nodeid = %u pointer %p version %u mdata %p mversion %u mstage %u == %u\n",
			e_ns_get_node_id(node), node, e_ns_get_node_version_struct(node), g_mesh, version, stage, 6);
		sui_draw_text(-0.9, line, SUI_T_SIZE, SUI_T_SPACE, text, 1, 1, 1);
		printf(text);
		line -= 0.05;
	}
}