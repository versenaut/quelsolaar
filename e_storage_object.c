#include "e_includes.h"

#include "verse.h"
#include "st_types.h"
#include "e_types.h"
#include "e_storage_node.h"

#include "st_matrix_operations.h"

typedef struct{
	uint16 link_id; 
	VNodeID link;
	char name[16]; 
	uint32 target_id;
}ESLink;

typedef struct{
	char			name[16];
	uint8			param_count;
	VNOParamType	*param_types;
	char			**param_names;
}ESMethod;

typedef struct{
	char			name[16];
	uint			count;
	ESMethod		*methods;
}ESMethodGroup;

typedef struct{
	double		pos[3];
	double		pos_speed[3];
	double		pos_accelerate[3];
	double		pos_drag_normal[3];
	double		pos_drag;
	uint32		pos_time_s;
	uint32		pos_time_f;
	double		rot[4];
	double		rot_speed[4];
	double		rot_accelerate[4];
	double		rot_drag_normal[4];
	double		rot_drag;
	uint32		rot_time_s;
	uint32		rot_time_f;
	double		scale[3];
}ESObjectTrans;

typedef struct{
	ENodeHead		head;
	ESLink			*links;
	uint			link_count;
	double			light[3];
	ESObjectTrans	trans;	
	uint			group_count;
	ESMethodGroup	*groups;
}ESObjectNode;

extern void		e_ns_update_node_version_struct(ESObjectNode *node);
extern void		e_ns_update_node_version_data(ESObjectNode *node);

ESObjectNode *e_create_o_node(VNodeID node_id, VNodeOwner owner)
{
	ESObjectNode	*node;
	uint i;
	if((node = (ESObjectNode *) e_ns_get_node_networking(node_id)) == NULL)
	{
		node = malloc(sizeof(ESObjectNode));
		node->links = NULL;
		node->link_count = 0;
		for(i = 0; i < 3; i++)
			node->light[i] = 0;

		for(i = 0; i < 3; i++)
			node->trans.pos[i] = node->trans.pos_speed[i] = node->trans.pos_accelerate[i] = node->trans.pos_drag_normal[i] = 0;
		for(i = 0; i < 3; i++)
			node->trans.rot[i] = node->trans.rot_speed[i] = node->trans.rot_accelerate[i] = node->trans.rot_drag_normal[i] = 0;
		node->trans.rot[i] = node->trans.rot_speed[i] = node->trans.rot_accelerate[i] = node->trans.rot_drag_normal[i] = 1;
		for(i = 0; i < 3; i++)
			node->trans.scale[i] = 1;
		node->trans.pos_drag = 0;
		node->trans.pos_time_s = 0;
		node->trans.pos_time_f = 0;
		node->group_count = 0;
		node->groups = NULL;
		e_ns_init_head((ENodeHead *)node, V_NT_OBJECT, node_id, owner);
		verse_send_o_transform_subscribe(node_id, VN_FORMAT_REAL64);
	}
	return node;
}

void callback_send_o_transform_pos_real64(void *user_data, VNodeID node_id, uint32 time_s, uint32 time_f, real64 *pos, real64 *speed, real64 *accelerate, real64 *drag_normal, real64 drag)
{
	ESObjectNode *node;
	ESObjectTrans *t;
	node = e_create_o_node(node_id, 0);
	t = &node->trans;
	t->pos[0] = pos[0]; 
	t->pos[1] = pos[1]; 
	t->pos[2] = pos[2]; 
	t->pos_speed[0] = speed[0];
	t->pos_speed[1] = speed[1];
	t->pos_speed[2] = speed[2];
	t->pos_accelerate[0] = accelerate[0]; 
	t->pos_accelerate[1] = accelerate[1]; 
	t->pos_accelerate[2] = accelerate[2];
	t->pos_drag_normal[0] = drag_normal[0]; 
	t->pos_drag_normal[1] = drag_normal[1]; 
	t->pos_drag_normal[2] = drag_normal[2]; 
	t->pos_drag = drag;
	t->pos_time_s = time_s;
	t->pos_time_f = time_f;
}

void callback_send_o_transform_pos_real32(void *user_data, VNodeID node_id, uint32 time_s, uint32 time_f, real32 *pos, real32 *speed, real32 *accelerate, real32 *drag_normal, real32 drag)
{
	ESObjectNode *node;
	ESObjectTrans *t;
	node = e_create_o_node(node_id, 0);
	t = &node->trans;
	t->pos[0] = (double)pos[0]; 
	t->pos[1] = (double)pos[1]; 
	t->pos[2] = (double)pos[2]; 
	t->pos_speed[0] = (double)speed[0];
	t->pos_speed[1] = (double)speed[1];
	t->pos_speed[2] = (double)speed[2];
	t->pos_accelerate[0] = (double)accelerate[0]; 
	t->pos_accelerate[1] = (double)accelerate[1]; 
	t->pos_accelerate[2] = (double)accelerate[2];
	t->pos_drag_normal[0] = (double)drag_normal[0]; 
	t->pos_drag_normal[1] = (double)drag_normal[1]; 
	t->pos_drag_normal[2] = (double)drag_normal[2]; 
	t->pos_drag = drag;
	t->pos_time_s = time_s;
	t->pos_time_f = time_f;
}


void callback_send_o_transform_rot_real64(void *user_data, VNodeID node_id, uint32 time_s, uint32 time_f, real64 *rot, real64 *speed, real64 *accelerate, real64 *drag_normal, real64 drag)
{
	ESObjectNode *node;
	ESObjectTrans *t;
	node = e_create_o_node(node_id, 0);
	t = &node->trans;
	t->rot[0] = rot[0]; 
	t->rot[1] = rot[1]; 
	t->rot[2] = rot[2]; 
	t->rot[3] = rot[3]; 
	t->rot_speed[0] = speed[0];
	t->rot_speed[1] = speed[1];
	t->rot_speed[2] = speed[2];
	t->rot_speed[3] = speed[3];
	t->rot_accelerate[0] = accelerate[0]; 
	t->rot_accelerate[1] = accelerate[1]; 
	t->rot_accelerate[2] = accelerate[2];
	t->rot_accelerate[3] = accelerate[3];
	t->rot_drag_normal[0] = drag_normal[0]; 
	t->rot_drag_normal[1] = drag_normal[1]; 
	t->rot_drag_normal[2] = drag_normal[2]; 
	t->rot_drag_normal[3] = drag_normal[3];
	t->rot_drag = drag;
	t->rot_time_s = time_s;
	t->rot_time_f = time_f;
}

void callback_send_o_transform_rot_real32(void *user_data, VNodeID node_id, uint32 time_s, uint32 time_f, real32 *rot, real32 *speed, real32 *accelerate, real32 *drag_normal, real32 drag)
{
	ESObjectNode *node;
	ESObjectTrans *t;
	node = e_create_o_node(node_id, 0);
	t = &node->trans;
	t->rot[0] = (double)rot[0]; 
	t->rot[1] = (double)rot[1]; 
	t->rot[2] = (double)rot[2]; 
	t->rot[3] = (double)rot[3]; 
	t->rot_speed[0] = (double)speed[0];
	t->rot_speed[1] = (double)speed[1];
	t->rot_speed[2] = (double)speed[2];
	t->rot_speed[3] = (double)speed[3];
	t->rot_accelerate[0] = (double)accelerate[0]; 
	t->rot_accelerate[1] = (double)accelerate[1]; 
	t->rot_accelerate[2] = (double)accelerate[2];
	t->rot_accelerate[3] = (double)accelerate[3];
	t->rot_drag_normal[0] = (double)drag_normal[0]; 
	t->rot_drag_normal[1] = (double)drag_normal[1]; 
	t->rot_drag_normal[2] = (double)drag_normal[2]; 
	t->rot_drag_normal[3] = (double)drag_normal[3]; 
	t->rot_drag = drag;
	t->rot_time_s = time_s;
	t->rot_time_f = time_f;
}


void callback_send_o_transform_scale_real64(void *user_data, VNodeID node_id, real64 scale_x, real64 scale_y, real64 scale_z)
{
	ESObjectNode *node;
	ESObjectTrans *t;
	node = e_create_o_node(node_id, 0);
	t = &node->trans;
	t->scale[0] = scale_x;
	t->scale[1] = scale_y;
	t->scale[2] = scale_z;
}

void callback_send_o_transform_scale_real32(void *user_data, VNodeID node_id, real32 scale_x, real32 scale_y, real32 scale_z)
{
	ESObjectNode *node;
	ESObjectTrans *t;
	node = e_create_o_node(node_id, 0);
	t = &node->trans;
	t->scale[0] = (double)scale_x;
	t->scale[1] = (double)scale_y;
	t->scale[2] = (double)scale_z;
}

void e_nso_get_pos(ESObjectNode *node, double *pos, double *speed, double *accelerate, double *drag_normal, double *drag, uint32 *time)
{
	ESObjectTrans *t;
	uint i;
	t = &((ESObjectNode *)node)->trans;
	if(pos != NULL)
		for(i = 0; i < 3; i++)
			pos[i] = t->pos[i]; 
	if(speed != NULL)
		for(i = 0; i < 3; i++)
			speed[i] = t->pos_speed[i]; 
	if(accelerate != NULL)
		for(i = 0; i < 3; i++)
			accelerate[i] = t->pos_accelerate[i]; 
	if(drag_normal != NULL)
		for(i = 0; i < 3; i++)
			drag_normal[i] = t->pos_drag_normal[i]; 
	if(drag != NULL)
		*drag = t->pos_drag; 
	if(time != NULL)
	{
		time[0] = t->pos_time_s;
		time[1] = t->pos_time_f;
	}
}
void e_nso_get_rot(ESObjectNode *node, double *rot, double *speed, double *accelerate, double *drag_normal, double *drag, uint32 *time)
{
	ESObjectTrans *t;
	uint i;
	t = &((ESObjectNode *)node)->trans;
	if(rot != NULL)
		for(i = 0; i < 4; i++)
			rot[i] = t->rot[i]; 
	if(speed != NULL)
		for(i = 0; i < 4; i++)
			speed[i] = t->rot_speed[i]; 
	if(accelerate != NULL)
		for(i = 0; i < 4; i++)
			accelerate[i] = t->rot_accelerate[i]; 
	if(drag_normal != NULL)
		for(i = 0; i < 4; i++)
			drag_normal[i] = t->rot_drag_normal[i]; 
	if(drag != NULL)
		*drag = t->rot_drag; 
	if(time != NULL)
	{
		time[0] = t->pos_time_s;
		time[1] = t->pos_time_f;
	}
}

double e_get_delta_time(uint32 time_s, uint32 time_f)
{
	return (double)time_s + (double)time_f / (double)(0xffffffff);
}


void e_nso_get_pos_time(ESObjectNode *node, double *pos, uint32 time_s, uint32 time_f)
{
	ESObjectTrans *t;
	double dt;
	t = &((ESObjectNode *)node)->trans;
	dt = e_get_delta_time(time_s, time_f);
	pos[0] = t->pos[0] + t->pos_speed[0] * dt + t->pos_accelerate[0] * dt * dt;
	pos[1] = t->pos[1] + t->pos_speed[1] * dt + t->pos_accelerate[1] * dt * dt;
	pos[2] = t->pos[2] + t->pos_speed[2] * dt + t->pos_accelerate[2] * dt * dt;
}

void e_nso_get_rot_time(ESObjectNode *node, double *rot, uint32 time_s, uint32 time_f)
{
	ESObjectTrans *t;
	double dt;
	t = &((ESObjectNode *)node)->trans;
	dt = e_get_delta_time(time_s, time_f);
	rot[0] = t->rot[0] + t->rot_speed[0] * dt + t->rot_accelerate[0] * dt * dt;
	rot[1] = t->rot[1] + t->rot_speed[1] * dt + t->rot_accelerate[1] * dt * dt;
	rot[2] = t->rot[2] + t->rot_speed[2] * dt + t->rot_accelerate[2] * dt * dt;
	rot[3] = t->rot[3] + t->rot_speed[3] * dt + t->rot_accelerate[3] * dt * dt;
}

void e_nso_get_scale(ESObjectNode *node, double *scale)
{
	scale[0] = ((ESObjectNode *)node)->trans.scale[0];
	scale[1] = ((ESObjectNode *)node)->trans.scale[1];
	scale[2] = ((ESObjectNode *)node)->trans.scale[2];
}


void e_nso_get_rot_matrix(ESObjectNode *node, double *matrix, uint32 time_s, uint32 time_f)
{
	double r[4];
	e_nso_get_rot_time(node, r, time_s, time_f);
	matrix[0] = r[3] * r[3] + r[0] * r[0] - r[1] * r[1] - r[2] * r[2];
	matrix[4] = 2 * r[0] * r[1] - 2 * r[3] * r[2];
	matrix[8] = 2 * r[0] * r[2] + 2 * r[3] * r[1];
	matrix[12] = 0; 

	matrix[1] = 2 * r[0] * r[1] + 2 * r[3] * r[2];
	matrix[5] = r[3] * r[3] - r[0] * r[0] + r[1] * r[1] - r[2] * r[2]; 
	matrix[9] = 2 * r[1] * r[2] - 2 * r[3] * r[0];
	matrix[13] = 0;

	matrix[2] = 2 * r[0] * r[2] - 2 * r[3] * r[1];
	matrix[6] = 2 * r[1] * r[2] + 2 * r[3] * r[0]; 
	matrix[10] = r[3] * r[3] - r[0] * r[0] - r[1] * r[1] + r[2] * r[2];
	matrix[14] = 0; 

	matrix[3] = 0;
	matrix[7] = 0; 
	matrix[11] = 0;
	matrix[15] = 1; 
}

void e_nso_get_matrix(ESObjectNode *node, double *matrix, uint32 time_s, uint32 time_f)
{
	double pos[3];
	e_nso_get_rot_matrix(node, matrix, time_s, time_f);
	matrix[0] *= ((ESObjectNode *)node)->trans.scale[0];
	matrix[4] *= ((ESObjectNode *)node)->trans.scale[0];
	matrix[8] *= ((ESObjectNode *)node)->trans.scale[0];

	matrix[1] *= ((ESObjectNode *)node)->trans.scale[1];
	matrix[5] *= ((ESObjectNode *)node)->trans.scale[1]; 
	matrix[9] *= ((ESObjectNode *)node)->trans.scale[1];

	matrix[2] *= ((ESObjectNode *)node)->trans.scale[2];
	matrix[6] *= ((ESObjectNode *)node)->trans.scale[2]; 
	matrix[10] *= ((ESObjectNode *)node)->trans.scale[2];
	e_nso_get_pos_time(node, pos, time_s, time_f);
	matrix[3] = 0;
	matrix[7] = 0; 
	matrix[11] = 0;
	matrix[15] = 1; 
}

void e_nso_get_light(ESObjectNode *node, double *light)
{
	light[0] = ((ESObjectNode *)node)->light[0];
	light[1] = ((ESObjectNode *)node)->light[1];
	light[2] = ((ESObjectNode *)node)->light[2];
}


void callback_send_o_light_set(void *user_data, VNodeID node_id, real64 light_r, real64 light_g, real64 light_b)
{
	ESObjectNode	*node;
	node = e_create_o_node(node_id, 0);
	node->light[0] = light_r;
	node->light[1] = light_g;
	node->light[2] = light_b;
	e_ns_update_node_version_data(node);
}

void callback_send_o_link_set(void *user_data, VNodeID node_id, uint16 link_id, VNodeID link, const char *name, uint32 target_id)
{
	ESObjectNode	*node;
	uint i;
	node = e_create_o_node(node_id, 0);
	if(node->link_count <= link_id)
	{
		i = node->link_count;
		node->link_count = link_id + 16;
		node->links = realloc(node->links, (sizeof *node->links) * node->link_count);
		for(; i < node->link_count; i++)
			node->links[i].name[0] = 0;
	}

	node->links[link_id].link_id = link_id;
	node->links[link_id].link = link; 
	for(i = 0; i < 15 && name[i] != 0; i++)
		node->links[link_id].name[i] = name[i];
	node->links[link_id].name[i] = 0;
	node->links[link_id].target_id = target_id;
	e_ns_update_node_version_struct(node);
}

void callback_send_o_link_destroy(void *user_data, VNodeID node_id, uint16 link_id)
{
	ESObjectNode	*node;
	node = e_create_o_node(node_id, 0);
	if(link_id >= node->link_count)
		return;
	node->links[link_id].name[0] = 0;
	e_ns_update_node_version_struct(node);
}

void callback_send_o_method_group_destroy(void *user_data, VNodeID node_id, uint8 group_id)
{
	ESObjectNode	*node;
	uint i, j;
	node = e_create_o_node(node_id, 0);
	if(node->group_count > group_id)
	{
		if(node->groups[group_id].count != 0 && node->groups[group_id].name[0] != 0)
		{
			for(i = 0; i < node->groups[group_id].count; i++)
			{
				if(node->groups[group_id].methods[i].name[0] != 0)
				{
					for(j = 0; j < node->groups[group_id].methods[i].param_count; j++)
						free(node->groups[group_id].methods[i].param_names[j]);
					free(node->groups[group_id].methods[i].param_types);
					free(node->groups[group_id].methods[i].param_names);
				}
			}
			free(node->groups[group_id].methods);
		}
		node->groups[group_id].name[0] = 0;
	}
}

void callback_send_o_method_group_create(void *user_data, VNodeID node_id, uint16 group_id, const char *name)
{
	ESObjectNode	*node;
	uint i;
	node = e_create_o_node(node_id, 0);
	verse_send_o_method_group_subscribe(node_id, group_id);
	if(node->group_count <= group_id)
	{
		node->groups = realloc(node->groups, sizeof(*node->groups) * (group_id + 8));
		for(i = node->group_count; i < group_id + 8; i++)
		{
			node->groups[i].name[0] = 0;
			node->groups[i].count = 0;
			node->groups[i].methods = NULL;
		}
		node->group_count = i;
	}
	for(i = 0; i < 15 && name[i] != 0; i++)
		node->groups[group_id].name[i] = name[i];
	node->groups[group_id].name[i] = 0;
}

void callback_send_o_method_destroy(void *user_data, VNodeID node_id, uint16 group_id, uint16 method_id)
{
	ESObjectNode	*node;
	uint i;
	node = e_create_o_node(node_id, 0);
	if(node->group_count > group_id)
	{
		if(node->groups[group_id].count > method_id)
		{
			if(node->groups[group_id].methods[method_id].name[0] != 0)
			{
				ESMethod *m;
				ESMethodGroup *g;
				g = &node->groups[group_id];
				m = &node->groups[group_id].methods[method_id];


				node->groups[group_id].methods[method_id].name[0] = 0;
				for(i = 0; i < node->groups[group_id].methods[method_id].param_count; i++)
					free(node->groups[group_id].methods[method_id].param_names[i]);
				free(node->groups[group_id].methods[method_id].param_types);
				free(node->groups[group_id].methods[method_id].param_names);
			}
		}
	}
}

void callback_send_o_method_create(void *user_data, VNodeID node_id, uint16 group_id, uint16 method_id, const char *name, uint8 param_count, VNOParamType *param_types, const char * *param_names)
{
	ESObjectNode	*node;
	uint i, j;
	char *text;
	callback_send_o_method_destroy(NULL, node_id, group_id, method_id);
	node = e_create_o_node(node_id, 0);
	if(node->group_count > group_id)
	{
		if(node->groups[group_id].count <= method_id)
		{
			node->groups[group_id].methods = realloc(node->groups[group_id].methods, (sizeof *node->groups[group_id].methods) * (method_id + 8));
			for(i = node->groups[group_id].count; i < method_id + 8; i++)
			{
				node->groups[group_id].methods[i].name[0] = 0;
				node->groups[group_id].methods[i].param_count = 0;
				node->groups[group_id].methods[i].param_types = NULL;
				node->groups[group_id].methods[i].param_names = NULL;
			}
			node->groups[group_id].count = i;
		}
		for(i = 0; i < 15 && name[i] != 0; i++)
			node->groups[group_id].methods[method_id].name[i] = name[i];
		node->groups[group_id].methods[method_id].name[i] = 0;
		node->groups[group_id].methods[method_id].param_count = param_count;
		if(param_count != 0)
		{
			node->groups[group_id].methods[method_id].param_types = malloc((sizeof *node->groups[group_id].methods[method_id].param_types) * param_count);
			for(i = 0; i < param_count; i++)
				node->groups[group_id].methods[method_id].param_types[i] = param_types[i];
			node->groups[group_id].methods[method_id].param_names = malloc((sizeof *node->groups[group_id].methods[method_id].param_names) * param_count);
			for(i = 0; i < param_count; i++)
			{
				node->groups[group_id].methods[method_id].param_names[i] = malloc((sizeof *node->groups[group_id].methods[method_id].param_names[i]) * 16);
				
				text = node->groups[group_id].methods[method_id].param_names[i];
				for(j = 0; j < 15 && param_names[i][j] != 0; j++)
					text[j] = param_names[i][j];
				text[j] = 0;
			}
		}
	}	
}

char *e_nso_get_method_group(ESObjectNode *node, uint16 group_id)
{
	if(node->group_count > group_id)
		if(node->groups[group_id].name[0] != 0)
			return node->groups[group_id].name;
	return NULL;
}

uint16 e_nso_get_next_method_group(ESObjectNode *node, uint16 group_id)
{
	if(node->group_count > group_id)
	{
		for(;node->group_count > group_id && node->groups[group_id].name[0] == 0; group_id++);
		if(node->group_count == group_id)
			return -1;
		return group_id;
	}else
		return -1;
}

char *e_nso_get_method(ESObjectNode *node, uint16 group_id, uint16 method_id)
{
	if(node->group_count > group_id)
		if(node->groups[group_id].name[0] != 0)
			if(node->groups[group_id].count > group_id)
				if(node->groups[group_id].methods[method_id].name[0] != 0)
					return node->groups[group_id].methods[method_id].name;
	return NULL;
}


uint16 e_nso_get_next_method(ESObjectNode *node, uint16 group_id, uint16 method_id)
{
	ESMethodGroup *group;
	if(node->group_count > group_id && node->groups[group_id].name[0] != 0)
	{
		for(group = &node->groups[group_id]; group->count > method_id && group->methods[method_id].name[0] == 0; method_id++);
		if(group->count == method_id)
			return (uint16)-1;
		return method_id;
	}
	return (uint16)-1;
}

uint e_nso_get_method_param_count(ESObjectNode *node, uint16 group_id, uint16 method_id)
{
	return node->groups[group_id].methods[method_id].param_count;
}

char **e_nso_get_method_param_names(ESObjectNode *node, uint16 group_id, uint16 method_id)
{
	return node->groups[group_id].methods[method_id].param_names;
}

VNOParamType *e_nso_get_method_param_types(ESObjectNode *node, uint16 group_id, uint16 method_id)
{
	return node->groups[group_id].methods[method_id].param_types;
}

void delete_object(ESObjectNode	*node)
{
	uint i, j, k;
	for(i = 0; i < node->group_count; i++)
	{
		if(node->groups[i].name[0] != 0 && node->groups[i].count > 0)
		{
			for(j = 0; j < node->groups[i].count; j++)
			{
				if(node->groups[i].methods[j].name[0] != 0)
				{
					free(node->groups[i].methods[j].param_types);
					for(k = 0; k < node->groups[i].methods[j].param_count; k++)
						free(node->groups[i].methods[j].param_names[k]);
					free(node->groups[i].methods[j].param_names);
				}
			}
			free(node->groups[i].methods);
		}
	}
	free(node->groups);
	free(node);
}

void *e_nso_get_link(ESObjectNode	*node, uint16  id)
{
	if(node->link_count < id || node->links[id].name[0] == 0)
		return NULL;
	return &node->links[id];
}

void *e_nso_get_next_link(ESObjectNode	*node, uint16  id)
{
	for(; id < node->link_count && node->links[id].name[0] == 0; id++);
	if(id == node->link_count)
		return NULL;
	return &node->links[id];
}

uint16 e_nso_get_link_id(ESLink *link)
{
	return link->link_id; 
}

VNodeID e_nso_get_link_node(ESLink *link)
{
	return link->link;
}

char *e_nso_get_link_name(ESLink *link)
{
	return link->name; 
}

uint32 e_nso_get_link_target_id(ESLink *link)
{
	return link->target_id;
}

void es_object_init(void)
{
	verse_callback_set(verse_send_o_link_set,				callback_send_o_link_set,				NULL);
	verse_callback_set(verse_send_o_link_destroy,			callback_send_o_link_destroy,			NULL);

	verse_callback_set(verse_send_o_transform_pos_real32,	callback_send_o_transform_pos_real32,	NULL);
	verse_callback_set(verse_send_o_transform_rot_real32,	callback_send_o_transform_rot_real32,	NULL);
	verse_callback_set(verse_send_o_transform_scale_real32,	callback_send_o_transform_scale_real32,	NULL);
	verse_callback_set(verse_send_o_transform_pos_real64,	callback_send_o_transform_pos_real64,	NULL);
	verse_callback_set(verse_send_o_transform_rot_real64,	callback_send_o_transform_rot_real64,	NULL);
	verse_callback_set(verse_send_o_transform_scale_real64,	callback_send_o_transform_scale_real64,	NULL);

	verse_callback_set(verse_send_o_light_set,				callback_send_o_light_set,				NULL);
	verse_callback_set(verse_send_o_method_group_destroy,	callback_send_o_method_group_destroy,	NULL);
	verse_callback_set(verse_send_o_method_group_create,	callback_send_o_method_group_create,	NULL);
	verse_callback_set(verse_send_o_method_destroy,			callback_send_o_method_destroy,			NULL);
	verse_callback_set(verse_send_o_method_create,			callback_send_o_method_create,			NULL);
}
