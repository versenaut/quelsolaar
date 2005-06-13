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
	uint32	anim_time_s;
	uint32	anim_time_f; 
	double	anim_pos;
	double	anim_speed; 
	double	anim_accel; 
	double	anim_scale; 
	double	anim_scale_speed;
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
	VNQuat64	rot, rot_speed, rot_accelerate, rot_drag_normal;
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

static void quat64_clear(VNQuat64 *q)
{
	q->x = q->y = q->z = 0.0;
	q->w = 1.0;
}

static void quat64_from_quat32(VNQuat64 *x, const VNQuat32 *y)
{
	if(x == NULL || y == NULL)
		return;
	x->x = y->x;
	x->y = y->y;
	x->z = y->z;
	x->w = y->w;
}

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
		quat64_clear(&node->trans.rot);
		quat64_clear(&node->trans.rot_speed);
		quat64_clear(&node->trans.rot_accelerate);
		quat64_clear(&node->trans.rot_drag_normal);
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
	if(speed != NULL)
	{
		t->pos_speed[0] = speed[0];
		t->pos_speed[1] = speed[1];
		t->pos_speed[2] = speed[2];
	}
	if(accelerate != NULL)
	{
		t->pos_accelerate[0] = accelerate[0]; 
		t->pos_accelerate[1] = accelerate[1]; 
		t->pos_accelerate[2] = accelerate[2];
	}
	if(drag_normal != NULL)
	{
		t->pos_drag_normal[0] = drag_normal[0]; 
		t->pos_drag_normal[1] = drag_normal[1]; 
		t->pos_drag_normal[2] = drag_normal[2];
	}
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
	if(speed != NULL)
	{
		t->pos_speed[0] = (double)speed[0];
		t->pos_speed[1] = (double)speed[1];
		t->pos_speed[2] = (double)speed[2];
	}
	if(accelerate != NULL)
	{
		t->pos_accelerate[0] = (double)accelerate[0]; 
		t->pos_accelerate[1] = (double)accelerate[1]; 
		t->pos_accelerate[2] = (double)accelerate[2];
	}
	if(drag_normal != NULL)
	{
		t->pos_drag_normal[0] = (double)drag_normal[0]; 
		t->pos_drag_normal[1] = (double)drag_normal[1];
		t->pos_drag_normal[2] = (double)drag_normal[2];
	}
	t->pos_drag = drag;
	t->pos_time_s = time_s;
	t->pos_time_f = time_f;
}

void callback_send_o_transform_rot_real64(void *user_data, VNodeID node_id, uint32 time_s, uint32 time_f,
					  const VNQuat64 *rot, const VNQuat64 *speed, const VNQuat64 *accelerate, const VNQuat64 *drag_normal, real64 drag)
{
	ESObjectNode *node;
	ESObjectTrans *t;
	node = e_create_o_node(node_id, 0);
	t = &node->trans;
	t->rot = *rot;
	if(speed != NULL)
		t->rot_speed = *speed;
	if(accelerate != NULL)
		t->rot_accelerate = *accelerate;
	if(drag_normal != NULL)
		t->rot_drag_normal = *drag_normal;
	t->rot_drag = drag;
	t->rot_time_s = time_s;
	t->rot_time_f = time_f;
}

void callback_send_o_transform_rot_real32(void *user_data, VNodeID node_id, uint32 time_s, uint32 time_f,
					  const VNQuat32 *rot, const VNQuat32 *speed, const VNQuat32 *accelerate, const VNQuat32 *drag_normal, real32 drag)
{
	ESObjectNode *node;
	ESObjectTrans *t;
	node = e_create_o_node(node_id, 0);
	t = &node->trans;
	t->rot.x = rot->x;
	t->rot.y = rot->y;
	t->rot.z = rot->z;
	t->rot.w = rot->w;
	quat64_from_quat32(&t->rot_speed, speed);
	quat64_from_quat32(&t->rot_accelerate, accelerate);
	quat64_from_quat32(&t->rot_drag_normal, drag_normal);
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

void e_nso_get_rot(ESObjectNode *node, VNQuat64 *rot, VNQuat64 *speed, VNQuat64 *accelerate, VNQuat64 *drag_normal, real64 *drag, uint32 *time)
{
	ESObjectTrans *t;
	uint i;

	t = &((ESObjectNode *)node)->trans;
	if(rot != NULL)
		*rot = t->rot;
	if(speed != NULL)
		*speed = t->rot_speed;
	if(accelerate != NULL)
		*accelerate = t->rot_accelerate;
	if(drag_normal != NULL)
		*drag_normal = t->rot_drag_normal;
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
	/* FIXME: These equations seem to miss a 0.5 factor in the last term. /emil */
	pos[0] = t->pos[0] + t->pos_speed[0] * dt + t->pos_accelerate[0] * dt * dt;
	pos[1] = t->pos[1] + t->pos_speed[1] * dt + t->pos_accelerate[1] * dt * dt;
	pos[2] = t->pos[2] + t->pos_speed[2] * dt + t->pos_accelerate[2] * dt * dt;
}

void e_nso_get_rot_time(ESObjectNode *node, VNQuat64 *rot, uint32 time_s, uint32 time_f)
{
	ESObjectTrans *t;
	double dt;

	/* FIXME: This code is very certainly incorrect -- you can't interpolate quaternions like this, can you? */
	t = &((ESObjectNode *)node)->trans;
	dt = e_get_delta_time(time_s, time_f);
	rot->x = t->rot.x + t->rot_speed.x * dt + t->rot_accelerate.x * dt * dt;
	rot->y = t->rot.y + t->rot_speed.y * dt + t->rot_accelerate.y * dt * dt;
	rot->z = t->rot.z + t->rot_speed.y * dt + t->rot_accelerate.y * dt * dt;
	rot->w = t->rot.w + t->rot_speed.w * dt + t->rot_accelerate.w * dt * dt;
}

void e_nso_get_scale(ESObjectNode *node, double *scale)
{
	scale[0] = ((ESObjectNode *)node)->trans.scale[0];
	scale[1] = ((ESObjectNode *)node)->trans.scale[1];
	scale[2] = ((ESObjectNode *)node)->trans.scale[2];
}


void e_nso_get_rot_matrix(ESObjectNode *node, double *matrix, uint32 time_s, uint32 time_f)
{
	VNQuat64	rot;

	e_nso_get_rot_time(node, &rot, time_s, time_f);
	matrix[0] = rot.w * rot.w + rot.x * rot.x - rot.y * rot.y - rot.z * rot.z;
	matrix[4] = 2 * rot.x * rot.y - 2 * rot.w * rot.z;
	matrix[8] = 2 * rot.x * rot.z + 2 * rot.w * rot.y;
	matrix[12] = 0; 

	matrix[1] = 2 * rot.x * rot.y + 2 * rot.w * rot.z;
	matrix[5] = rot.w * rot.w - rot.x * rot.x + rot.y * rot.y - rot.z * rot.z; 
	matrix[9] = 2 * rot.y * rot.z - 2 * rot.w * rot.x;
	matrix[13] = 0;

	matrix[2] = 2 * rot.x * rot.z - 2 * rot.w * rot.y;
	matrix[6] = 2 * rot.y * rot.z + 2 * rot.w * rot.x; 
	matrix[10] = rot.w * rot.w - rot.x * rot.x - rot.y * rot.y + rot.z * rot.z;
	matrix[14] = 0; 

	matrix[3]  = 0.0;
	matrix[7]  = 0.0; 
	matrix[11] = 0.0;
	matrix[15] = 1.0; 
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

void callback_send_o_anim_run(void *user_data, VNodeID node_id, uint16 link_id, uint32 time_s, uint32 time_f, real64 pos, real64 speed, real64 accel, real64 scale, real64 scale_speed)
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
	node->links[i].anim_time_s = time_s, 
	node->links[i].anim_time_f = time_f; 
	node->links[i].anim_pos = pos; 
	node->links[i].anim_speed = speed; 
	node->links[i].anim_accel = accel; 
	node->links[i].anim_scale = scale; 
	node->links[i].anim_scale_speed = scale_speed; 
	e_ns_update_node_version_data(node);
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

void callback_send_o_method_create(void *user_data, VNodeID node_id, uint16 group_id, uint16 method_id, const char *name, uint8 param_count, VNOParamType *param_types, char * *param_names)
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

void e_nso_get_anim_time(ESLink *link, uint32 *time_s, uint32 *time_f)
{
	*time_s = link->anim_time_s;
	*time_f = link->anim_time_f;
}

double e_nso_get_anim_pos(ESLink *link)
{
	return link->anim_pos;
}

double e_nso_get_anim_speed(ESLink *link)
{
	return link->anim_speed;
}

double e_nso_get_anim_accel(ESLink *link)
{
	return link->anim_accel;
}

double e_nso_get_anim_scale(ESLink *link)
{
	return link->anim_scale;
}

double e_nso_get_anim_scale_speed(ESLink *link)
{
	return link->anim_scale_speed;
}

boolean e_nso_get_anim_active(ESLink *link)
{
	return link->anim_scale_speed > 0.00001 || link->anim_scale_speed < 0.00001 || link->anim_scale > 0.00001 || link->anim_scale < 0.00001;
}

double e_nso_get_anim_evaluate_pos(ESLink *link, uint32 time_s, uint32 time_f)
{
	double delta;
	delta = (double)(link->anim_time_s - time_s) + ((double)link->anim_time_f - (double)time_f) / (double) 0xffffffff;
	return link->anim_pos + link->anim_speed * delta + link->anim_accel * delta * delta;
}

double e_nso_get_anim_evaluate_scale(ESLink *link, uint32 time_s, uint32 time_f)
{
	double delta;
	delta = (double)(link->anim_time_s - time_s) + ((double)link->anim_time_f - (double)time_f) / (double) 0xffffffff;
	return link->anim_scale + link->anim_scale_speed * delta;
}

void es_object_init(void)
{
	verse_callback_set(verse_send_o_link_set,				callback_send_o_link_set,				NULL);
	verse_callback_set(verse_send_o_link_destroy,			callback_send_o_link_destroy,			NULL);
	verse_callback_set(verse_send_o_anim_run,				callback_send_o_anim_run,				NULL);

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
/*
typedef struct{
	uint	version;
	uint16	curve;
	uint	last_point;
}ESAnimLink;

typedef struct{
	char		lable[16];
	VNodeID		node_id;
	uint		version;
	ESAnimLink *links;
	uint		link_count;
}ESAnimhandle;

ESAnimhandle *e_nso_get_anim_handle(VNodeID node_id, char *lable)
{
	uint i;
	ESAnimhandle *h;
	h = malloc(sizeof *h);
	for(i = 0; i < 15 && lable[i] != 0; i++)
		h->lable[i] = lable[i];
	h->lable[i] = 0;
	h->node_id = node_id;
	h->version = -1;
	h->links = NULL;
	h->link_count = 0;
}

void e_nso_destroy_anim_handle(EOAnimhandle *handle)
{
	if(handle->links != NULL)
		free(handle->links);
	free(handle);
}

void e_anim_handle_update(EOAnimhandle *handle)
{
	if(h->version != )
	if(handle->links != NULL)
		free(handle->links);
	free(handle);
}*/

