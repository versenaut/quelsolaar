#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"
#include "seduce.h"

#include "uv_geometry.h"

#define UNDO_BUFFER_SIZE 4096

typedef enum{
	ET_END,
	ET_UV,
	ET_SELECT
}UVEventType;

typedef struct{
	UVEventType type;
	boolean isolated;
	uint id;
	union{
		egreal	select[4];
		egreal	uv[8];
	}data[2];
}UVEvent;

struct{
	uint	node_id;
	egreal	*vertex;
	uint	vertex_length;
	uint	*polygon;
	uint	polygon_length;
	uint	u_layer_id;
	uint	v_layer_id;
	uint	select_layer;
	uint	select_version;
	egreal	*vertex_select;
	egreal	*u;
	egreal	*v;
	egreal	*corner_select;
	uint	collapse_id;
	boolean create;
	UVEvent events[UNDO_BUFFER_SIZE];
	uint	event_start;
	uint	event_end;
	uint	event_pos;
}UVGGlobal;

void uvg_set_node(uint node_id)
{
	UVGGlobal.create = FALSE;
	UVGGlobal.node_id = node_id;
}

uint uvg_get_node(void)
{
	return UVGGlobal.node_id;
}

void uvg_init()
{
	uint i;
	UVGGlobal.node_id = 0;
	UVGGlobal.vertex = NULL; 
	UVGGlobal.vertex_length = 0;
	UVGGlobal.polygon = NULL;
	UVGGlobal.polygon_length = 0;
	UVGGlobal.u_layer_id = 0;
	UVGGlobal.v_layer_id = 0;
	UVGGlobal.u = NULL;
	UVGGlobal.v = NULL;

	UVGGlobal.select_layer = -1;
	UVGGlobal.select_version = 0;
	UVGGlobal.corner_select = NULL;
	UVGGlobal.vertex_select = NULL;
	UVGGlobal.collapse_id = 0;
	UVGGlobal.create = FALSE;
	UVGGlobal.event_pos = 0;
	UVGGlobal.event_start = 0;
	UVGGlobal.event_end = 0;
	for(i = 0; i < UNDO_BUFFER_SIZE; i++)
		UVGGlobal.events[i].type = ET_END;
}

boolean uvg_update()
{
	ENode *node;
	EGeoLayer *layer;
	uint i;
	if((node = e_ns_get_node(0, UVGGlobal.node_id)) == NULL || V_NT_GEOMETRY != e_ns_get_node_type(node))
		return FALSE;

	if((layer = e_nsg_get_layer_by_id(node, 0)) == NULL)
		return FALSE;
	UVGGlobal.vertex = e_nsg_get_layer_data(node, layer);

	if((layer = e_nsg_get_layer_by_id(node, 1)) == NULL)
		return FALSE;
	if(UVGGlobal.polygon_length != e_nsg_get_polygon_length(node))
	{
		UVGGlobal.corner_select = realloc(UVGGlobal.corner_select, (sizeof *UVGGlobal.corner_select) * e_nsg_get_polygon_length(node) * 4);
		for(i = UVGGlobal.polygon_length * 4; i < e_nsg_get_polygon_length(node) * 4; i++)
			UVGGlobal.corner_select[i] = 0;
	}
	UVGGlobal.vertex_length = e_nsg_get_vertex_length(node);
	UVGGlobal.polygon_length = e_nsg_get_polygon_length(node);
	UVGGlobal.polygon = e_nsg_get_layer_data(node, layer);
	if((layer = e_nsg_get_layer_by_type(node, VN_G_LAYER_VERTEX_REAL, "select")) == NULL)
	{
		UVGGlobal.vertex_select = NULL;
	}else
	{
//		if(UVGGlobal.select_version != e_nsg_get_layer_version(layer))
		{
			UVGGlobal.vertex_select = e_nsg_get_layer_data(node, layer);
			for(i = 0; i < UVGGlobal.polygon_length; i++)
			{
				if(UVGGlobal.polygon[i * 4] < UVGGlobal.vertex_length && UVGGlobal.vertex[UVGGlobal.polygon[i * 4] * 3] != E_REAL_MAX &&
					UVGGlobal.polygon[i * 4 + 1] < UVGGlobal.vertex_length && UVGGlobal.vertex[UVGGlobal.polygon[i * 4 + 1] * 3] != E_REAL_MAX &&
					UVGGlobal.polygon[i * 4 + 2] < UVGGlobal.vertex_length && UVGGlobal.vertex[UVGGlobal.polygon[i * 4 + 2] * 3] != E_REAL_MAX)
				{

					if(UVGGlobal.vertex_select[UVGGlobal.polygon[i * 4]] > 0.01 &&
						UVGGlobal.vertex_select[UVGGlobal.polygon[i * 4 + 1]] > 0.01 &&
						UVGGlobal.vertex_select[UVGGlobal.polygon[i * 4 + 2]] > 0.01 &&
						(UVGGlobal.polygon[i * 4 + 3] >= UVGGlobal.vertex_length || UVGGlobal.vertex[UVGGlobal.polygon[i * 4 + 3] * 3] == E_REAL_MAX ||
						UVGGlobal.vertex_select[UVGGlobal.polygon[i * 4 + 3]] > 0.01))
							break;

				}
			}
			printf("i = %u UVGGlobal.polygon_length = %u\n", i, UVGGlobal.polygon_length);
			if(i == UVGGlobal.polygon_length)
				UVGGlobal.vertex_select = NULL;
			UVGGlobal.select_version = e_nsg_get_layer_version(layer);
		}
	}
	if((layer = e_nsg_get_layer_by_type(node, VN_G_LAYER_POLYGON_CORNER_REAL, "map_u")) == NULL)
	{
		UVGGlobal.u = NULL;
	}
	else
	{
		UVGGlobal.u_layer_id = e_nsg_get_layer_id(layer);
		UVGGlobal.u = e_nsg_get_layer_data(node, layer);
	}
	if((layer = e_nsg_get_layer_by_type(node, VN_G_LAYER_POLYGON_CORNER_REAL, "map_v")) == NULL)
	{
		UVGGlobal.v = NULL;
	}
	else
	{
		UVGGlobal.v_layer_id = e_nsg_get_layer_id(layer);
		UVGGlobal.v = e_nsg_get_layer_data(node, layer);
	}

	if(UVGGlobal.u != NULL && UVGGlobal.v != NULL && UVGGlobal.create)
	{
		UVGGlobal.collapse_id = uvg_get_next_polygon(UVGGlobal.collapse_id + 1);
		if(UVGGlobal.collapse_id == -1)
			UVGGlobal.collapse_id = uvg_get_next_polygon(0);
		if(UVGGlobal.collapse_id != -1)
			uvg_get_un_collapse(UVGGlobal.collapse_id);
	}
	return TRUE;
}

boolean uvg_node_has_uv()
{
	return UVGGlobal.u != NULL && UVGGlobal.v != NULL;
}

void uvg_node_create_uv()
{
	if(UVGGlobal.u == NULL)
		verse_send_g_layer_create(UVGGlobal.node_id, -1, "map_u", VN_G_LAYER_POLYGON_CORNER_REAL, 0, 0);
	if(UVGGlobal.v == NULL)
		verse_send_g_layer_create(UVGGlobal.node_id, -2, "map_v", VN_G_LAYER_POLYGON_CORNER_REAL, 0, 0);
	UVGGlobal.create = TRUE;
}

void uvg_get_un_collapse(uint id)
{
	if(UVGGlobal.u[id * 4] < UVGGlobal.u[id * 4 + 1] + 0.00001 && UVGGlobal.u[id * 4] > UVGGlobal.u[id * 4 + 1] - 0.00001 &&
		UVGGlobal.u[id * 4 + 1] < UVGGlobal.u[id * 4 + 2] + 0.00001 && UVGGlobal.u[id * 4 + 1] > UVGGlobal.u[id * 4 + 2] - 0.00001 &&
		UVGGlobal.v[id * 4] < UVGGlobal.v[id * 4 + 1] + 0.00001 && UVGGlobal.v[id * 4] > UVGGlobal.v[id * 4 + 1] - 0.00001 &&
		UVGGlobal.v[id * 4 + 1] < UVGGlobal.v[id * 4 + 2] + 0.00001 && UVGGlobal.v[id * 4 + 1] > UVGGlobal.v[id * 4 + 2] - 0.00001)
	{
		if(uvg_is_quad(id))
		{
			if(UVGGlobal.u[id * 4 + 2] < UVGGlobal.u[id * 4 + 3] + 0.00001 && UVGGlobal.u[id * 4 + 2] > UVGGlobal.u[id * 4 + 3] - 0.00001 &&
				UVGGlobal.u[id * 4 + 3] < UVGGlobal.u[id * 4] + 0.00001 && UVGGlobal.u[id * 4 + 3] > UVGGlobal.u[id * 4] - 0.00001 &&
				UVGGlobal.v[id * 4 + 2] < UVGGlobal.v[id * 4 + 3] + 0.00001 && UVGGlobal.v[id * 4 + 2] > UVGGlobal.v[id * 4 + 3] - 0.00001 &&
				UVGGlobal.v[id * 4 + 3] < UVGGlobal.v[id * 4] + 0.00001 && UVGGlobal.v[id * 4 + 3] > UVGGlobal.v[id * 4] - 0.00001)
			{
				verse_send_g_polygon_set_corner_real64(UVGGlobal.node_id, UVGGlobal.u_layer_id, id, 10 * UVGGlobal.vertex[UVGGlobal.polygon[id * 4 + 0] * 3], 10 * UVGGlobal.vertex[UVGGlobal.polygon[id * 4 + 1] * 3], 10 * UVGGlobal.vertex[UVGGlobal.polygon[id * 4 + 2] * 3], 10 * UVGGlobal.vertex[UVGGlobal.polygon[id * 4 + 3] * 3]);
				verse_send_g_polygon_set_corner_real64(UVGGlobal.node_id, UVGGlobal.v_layer_id, id, 10 * UVGGlobal.vertex[UVGGlobal.polygon[id * 4 + 0] * 3 + 1], 10 * UVGGlobal.vertex[UVGGlobal.polygon[id * 4 + 1] * 3 + 1], 10 * UVGGlobal.vertex[UVGGlobal.polygon[id * 4 + 2] * 3 + 1], 10 * UVGGlobal.vertex[UVGGlobal.polygon[id * 4 + 3] * 3 + 1]);
			}
		}else
		{
			if(UVGGlobal.u[id * 4 + 2] < UVGGlobal.u[id * 4] + 0.00001 && UVGGlobal.u[id * 4 + 2] > UVGGlobal.u[id * 4] - 0.00001 &&
				UVGGlobal.v[id * 4 + 2] < UVGGlobal.v[id * 4] + 0.00001 && UVGGlobal.v[id * 4 + 2] > UVGGlobal.v[id * 4] - 0.00001)
			{
				verse_send_g_polygon_set_corner_real64(UVGGlobal.node_id, UVGGlobal.u_layer_id, id, 10 * UVGGlobal.vertex[UVGGlobal.polygon[id * 4 + 0] * 3], 10 * UVGGlobal.vertex[UVGGlobal.polygon[id * 4 + 1] * 3], 10 * UVGGlobal.vertex[UVGGlobal.polygon[id * 4 + 2] * 3], 0);
				verse_send_g_polygon_set_corner_real64(UVGGlobal.node_id, UVGGlobal.v_layer_id, id, 10 * UVGGlobal.vertex[UVGGlobal.polygon[id * 4 + 0] * 3 + 1], 10 * UVGGlobal.vertex[UVGGlobal.polygon[id * 4 + 1] * 3 + 1], 10 * UVGGlobal.vertex[UVGGlobal.polygon[id * 4 + 2] * 3 + 1], 0);
			}
		}
	}
}

uint uvg_get_next_polygon(uint id)
{
	for(; id < UVGGlobal.polygon_length; id++)
	{
		if(UVGGlobal.polygon[id * 4] < UVGGlobal.vertex_length && UVGGlobal.vertex[UVGGlobal.polygon[id * 4] * 3] != E_REAL_MAX)
			if(UVGGlobal.polygon[id * 4 + 1] < UVGGlobal.vertex_length && UVGGlobal.vertex[UVGGlobal.polygon[id * 4 + 1] * 3] != E_REAL_MAX)
				if(UVGGlobal.polygon[id * 4 + 2] < UVGGlobal.vertex_length && UVGGlobal.vertex[UVGGlobal.polygon[id * 4 + 2] * 3] != E_REAL_MAX)
					return id;
	}
	return -1;
}

boolean uvg_is_quad(uint id)
{
	return  UVGGlobal.polygon[id * 4 + 3] < UVGGlobal.vertex_length && UVGGlobal.vertex[UVGGlobal.polygon[id * 4 + 3] * 3] != E_REAL_MAX;	
}

uint uvg_get_sides(uint id)
{
	if(UVGGlobal.polygon[id * 4 + 3] < UVGGlobal.vertex_length && UVGGlobal.vertex[UVGGlobal.polygon[id * 4 + 2] * 3] != E_REAL_MAX)
		return 4;
	return 3;
}

void uvg_get_uve(egreal *uv, uint id)
{
	id *= 4;
	if(UVGGlobal.u != NULL && UVGGlobal.v != NULL)
	{
		uv[0] = UVGGlobal.u[id];
		uv[1] = UVGGlobal.v[id];
		uv[2] = UVGGlobal.u[id + 1];
		uv[3] = UVGGlobal.v[id + 1];
		uv[4] = UVGGlobal.u[id + 2];
		uv[5] = UVGGlobal.v[id + 2];
		uv[6] = UVGGlobal.u[id + 3];
		uv[7] = UVGGlobal.v[id + 3];
	}else
	{
		uv[0] = UVGGlobal.vertex[UVGGlobal.polygon[id + 0] * 3] * 10;
		uv[1] = UVGGlobal.vertex[UVGGlobal.polygon[id + 0] * 3 + 1] * 10;
		uv[2] = UVGGlobal.vertex[UVGGlobal.polygon[id + 1] * 3] * 10;
		uv[3] = UVGGlobal.vertex[UVGGlobal.polygon[id + 1] * 3 + 1] * 10;
		uv[4] = UVGGlobal.vertex[UVGGlobal.polygon[id + 2] * 3] * 10;
		uv[5] = UVGGlobal.vertex[UVGGlobal.polygon[id + 2] * 3 + 1] * 10;
		if(uvg_is_quad(id / 4))
		{
			uv[6] = UVGGlobal.vertex[UVGGlobal.polygon[id + 3] * 3] * 10;
			uv[7] = UVGGlobal.vertex[UVGGlobal.polygon[id + 3] * 3 + 1] * 10;
		}
	}
}

boolean uvg_get_polygon_select(uint id)
{
	if(UVGGlobal.vertex_select == NULL)
		return TRUE;
	if(UVGGlobal.vertex_select[UVGGlobal.polygon[id * 4]] > 0.01 &&
		UVGGlobal.vertex_select[UVGGlobal.polygon[id * 4 + 1]] > 0.01 &&
			UVGGlobal.vertex_select[UVGGlobal.polygon[id * 4 + 2]] > 0.01 &&
				(UVGGlobal.polygon[id * 4 + 3] > UVGGlobal.vertex_length ||
					UVGGlobal.vertex_select[UVGGlobal.polygon[id * 4 + 3]] > 0.01))
		return TRUE;
	return FALSE;
}

egreal *uvg_get_corner_select(uint id)
{
	return &UVGGlobal.corner_select[id * 4];
}

uint uvg_get_struct_version(uint id)
{
	return 1;
}

uint uvg_get_uv_version(uint id)
{
	return 1;
}

uint *uvg_get_ref()
{
	return UVGGlobal.polygon;
}

egreal *uvg_get_vertex()
{
	return UVGGlobal.vertex;
}

uint uvg_get_vertex_length()
{
	return UVGGlobal.vertex_length;
}

uint uvg_get_polygon_length()
{
	return UVGGlobal.polygon_length;
}

void execute_event(UVEvent *event, uint direction)
{
	if(event->type == ET_SELECT)
	{
		uint id;
		id = event->id * 4;
		UVGGlobal.corner_select[id++] = event->data[direction].select[0];
		UVGGlobal.corner_select[id++] = event->data[direction].select[1];
		UVGGlobal.corner_select[id++] = event->data[direction].select[2];
		UVGGlobal.corner_select[id] = event->data[direction].select[3];
	}
	if(event->type == ET_UV)
	{
		egreal *uv;
		uv = event->data[direction].uv;
		verse_send_g_polygon_set_corner_real64(UVGGlobal.node_id, UVGGlobal.u_layer_id, event->id, uv[0], uv[2], uv[4], uv[6]);
		verse_send_g_polygon_set_corner_real64(UVGGlobal.node_id, UVGGlobal.v_layer_id, event->id, uv[1], uv[3], uv[5], uv[7]);
	}
}


uint find_same_event(uint id, uint type)
{
	UVEvent *event;
	uint i;
	event = &UVGGlobal.events[UVGGlobal.event_pos];
	i = UVGGlobal.event_pos;
	while(event->type != ET_END)
	{
		if(event->id == id && event->type == type)
			return i;
		i = (i + UNDO_BUFFER_SIZE - 1) % UNDO_BUFFER_SIZE;
		event = &UVGGlobal.events[i];
	}
	if(UVGGlobal.event_pos == UVGGlobal.event_end)
		UVGGlobal.event_pos = (UVGGlobal.event_pos + 1) % UNDO_BUFFER_SIZE;
	if(UVGGlobal.event_pos == UVGGlobal.event_start)
		for(UVGGlobal.event_start = (UVGGlobal.event_start + 1) % UNDO_BUFFER_SIZE; UVGGlobal.events[UVGGlobal.event_start].type != ET_END && UVGGlobal.event_start != UVGGlobal.event_end;UVGGlobal.event_start = (UVGGlobal.event_start + 1) % UNDO_BUFFER_SIZE);
	
	UVGGlobal.event_pos = (UVGGlobal.event_pos + 1) % UNDO_BUFFER_SIZE;
	return UVGGlobal.event_pos;
}

void uvg_redo()
{
	if(UVGGlobal.event_pos == UVGGlobal.event_end)
		return;
	if(UVGGlobal.events[UVGGlobal.event_pos].type == ET_END)
	{
		if(UVGGlobal.events[(UVGGlobal.event_pos + 1) % UNDO_BUFFER_SIZE].type == ET_END)
			return;
		UVGGlobal.event_pos = (UVGGlobal.event_pos + 1) % UNDO_BUFFER_SIZE;
	}
	while(UVGGlobal.event_pos != UVGGlobal.event_end && UVGGlobal.events[UVGGlobal.event_pos].type != ET_END)
	{
		execute_event(&UVGGlobal.events[UVGGlobal.event_pos], 1);
		UVGGlobal.event_pos = (UVGGlobal.event_pos + 1) % UNDO_BUFFER_SIZE;
	}
}

void uvg_undo()
{
	if(UVGGlobal.event_pos == UVGGlobal.event_start)
		return;
	if(UVGGlobal.events[UVGGlobal.event_pos].type == ET_END)
	{
		if(UVGGlobal.events[(UVGGlobal.event_pos + UNDO_BUFFER_SIZE - 1) % UNDO_BUFFER_SIZE].type == ET_END)
			return;
		UVGGlobal.event_pos = (UVGGlobal.event_pos + UNDO_BUFFER_SIZE - 1) % UNDO_BUFFER_SIZE;
	}
	while(UVGGlobal.event_pos != UVGGlobal.event_start && UVGGlobal.events[UVGGlobal.event_pos].type != ET_END)
	{
		execute_event(&UVGGlobal.events[UVGGlobal.event_pos], 0);
		UVGGlobal.event_pos = (UVGGlobal.event_pos + UNDO_BUFFER_SIZE - 1) % UNDO_BUFFER_SIZE;
	}
}

void uvg_set_all_corner_select(uint id, egreal c0, egreal c1, egreal c2, egreal c3)
{
	UVEvent *event;
	event = &UVGGlobal.events[find_same_event(id, ET_SELECT)];
	event->type = ET_SELECT;
	event->isolated = FALSE;
	event->id = id;

	id *= 4;
	event->data[0].select[0] = UVGGlobal.corner_select[id++];
	event->data[0].select[1] = UVGGlobal.corner_select[id++];
	event->data[0].select[2] = UVGGlobal.corner_select[id++];
	event->data[0].select[3] = UVGGlobal.corner_select[id];
	event->data[1].select[0] = c0;
	event->data[1].select[1] = c1;
	event->data[1].select[2] = c2;
	event->data[1].select[3] = c3;
	execute_event(event, 1);
}

void uvg_set_one_corner_select(uint id, uint corner, egreal select)
{
	UVEvent *event;
	uint i;
	i = find_same_event(id, ET_SELECT);
	event = &UVGGlobal.events[i];
	if(event->id != id)
	{
		event->type = ET_SELECT;
		event->isolated = FALSE;
		event->id = id;
		id *= 4;
		event->data[0].select[0] = UVGGlobal.corner_select[id];
		event->data[1].select[0] = UVGGlobal.corner_select[id];
		event->data[0].select[1] = UVGGlobal.corner_select[++id];
		event->data[1].select[1] = UVGGlobal.corner_select[id];
		event->data[0].select[2] = UVGGlobal.corner_select[++id];
		event->data[1].select[2] = UVGGlobal.corner_select[id];
		event->data[0].select[3] = UVGGlobal.corner_select[++id];
		event->data[1].select[3] = UVGGlobal.corner_select[id];
	}
	event->data[1].select[corner] = select;
	execute_event(event, 1);
}

void uvg_set_corner_select_all(egreal select)
{
	egreal *s;
	uint id;
	for(id = uvg_get_next_polygon(0); id != -1; id = uvg_get_next_polygon(id + 1))
	{
		s = uvg_get_corner_select(id);
		if(s[0] + 0.001 < select || s[0] - 0.001 > select ||
				s[1] + 0.001 < select || s[1] - 0.001 > select ||
				s[2] + 0.001 < select || s[2] - 0.001 > select ||
				(uvg_is_quad(id) && (s[3] + 0.001 < select || s[3] - 0.001 > select)))
			uvg_set_all_corner_select(id, select, select, select, select);
	}
}

void uvg_set_all_corner_uv_isolated(uint id, egreal u0, egreal v0, egreal u1, egreal v1, egreal u2, egreal v2, egreal u3, egreal v3)
{
	UVEvent *event;
	egreal uv[8];
	uvg_get_uve(uv, id);
	UVGGlobal.event_pos = (UVGGlobal.event_pos + 1) % UNDO_BUFFER_SIZE;
	event = &UVGGlobal.events[UVGGlobal.event_pos];
	event->type = ET_UV;
	event->isolated = TRUE;
	event->id = id;
	uvg_get_uve(event->data[0].uv, id);
	event->data[1].uv[0] = u0;
	event->data[1].uv[1] = v0;
	event->data[1].uv[2] = u1;
	event->data[1].uv[3] = v1;
	event->data[1].uv[4] = u2;
	event->data[1].uv[5] = v2;
	event->data[1].uv[6] = u3;
	event->data[1].uv[7] = v3;
	execute_event(event, 1);
}

void uvg_set_all_corner_uv(uint id, egreal u0, egreal v0, egreal u1, egreal v1, egreal u2, egreal v2, egreal u3, egreal v3)
{
	UVEvent *event;
	egreal uv[8];
	uvg_get_uve(uv, id);
	event = &UVGGlobal.events[find_same_event(id, ET_UV)];
	event->type = ET_UV;
	event->isolated = FALSE;
	event->id = id;
	uvg_get_uve(event->data[0].uv, id);
	event->data[1].uv[0] = u0;
	event->data[1].uv[1] = v0;
	event->data[1].uv[2] = u1;
	event->data[1].uv[3] = v1;
	event->data[1].uv[4] = u2;
	event->data[1].uv[5] = v2;
	event->data[1].uv[6] = u3;
	event->data[1].uv[7] = v3;
	execute_event(event, 1);
}

void uvg_set_one_corner_uv(uint id, uint corner, egreal u, egreal v)
{
	UVEvent *event;
	egreal uv[8];
	uint pos;
	uvg_get_uve(uv, id);
	pos = find_same_event(id, ET_SELECT);
	event = &UVGGlobal.events[pos];
	if(pos == UVGGlobal.event_pos)
	{
		event->type = ET_UV;
		event->isolated = FALSE;
		event->id = id;
		uvg_get_uve(event->data[0].uv, id);
		uvg_get_uve(event->data[1].uv, id);
	}
	event->data[1].uv[corner * 2] = u;
	event->data[1].uv[corner * 2 + 1] = v;
	execute_event(event, 1);
}

void uvg_action_end()
{
	if(UVGGlobal.events[UVGGlobal.event_pos].type == ET_END)
		return;
	if(UVGGlobal.event_pos == UVGGlobal.event_end)
		UVGGlobal.event_pos = (UVGGlobal.event_pos + 1) % UNDO_BUFFER_SIZE;
	if(UVGGlobal.event_pos == UVGGlobal.event_start)
		for(UVGGlobal.event_start = (UVGGlobal.event_start + 1) % UNDO_BUFFER_SIZE; UVGGlobal.events[UVGGlobal.event_start].type != ET_END && UVGGlobal.event_start != UVGGlobal.event_end;UVGGlobal.event_start = (UVGGlobal.event_start + 1) % UNDO_BUFFER_SIZE);
	UVGGlobal.event_pos = (UVGGlobal.event_pos + 1) % UNDO_BUFFER_SIZE;

	UVGGlobal.events[UVGGlobal.event_pos].type = ET_END;
	UVGGlobal.events[UVGGlobal.event_pos].id = -1;
}
