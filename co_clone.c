#include "st_matrix_operations.h"
#include "enough.h"
#include "seduce.h"
#include "persuade.h"

uint32 co_clone_from_node = -1;
uint32 co_clone_to_node = -1;

boolean co_clone_object_sub = FALSE;

void co_clone_object(ENode *node, ENode *clone)
{
	double a[4], b[4], c[4], d[4], e[4];
	VNQuat64 qa, qb, qc, qd;
	EObjLink *link;
	uint time[2];
	VNodeID id;

	if(!co_clone_object_sub)
	{
		id = e_ns_get_node_id(clone);
		e_nso_get_pos(node, a, b, c, d, e, time);
		verse_send_o_transform_pos_real64(id, time[0], time[1], a, b, c, d, e[0]);
		e_nso_get_rot(node, &qa, &qb, &qc, &qd, e, time);
		verse_send_o_transform_rot_real64(id, time[0], time[1], &qa, &qb, &qc, &qd, e[0]);
		e_nso_get_scale(node, a);
		verse_send_o_transform_scale_real64(id, a[0], a[1], a[2]);
		e_nso_get_light(node, a);
		verse_send_o_light_set(id, a[0], a[1], a[2]);
		for(link = e_nso_get_next_link(node, 0); link != 0; link = e_nso_get_next_link(node, e_nso_get_link_id(link) + 1))
			verse_send_o_link_set(id, -1, e_nso_get_link_node(link), e_nso_get_link_name(link), e_nso_get_link_target_id(link));
		verse_send_o_hide(id, e_nso_get_hide(node));
		co_clone_object_sub = TRUE;
	}
}


void co_material_object(ENode *node, ENode *clone)
{
	VNMFragmentID frag;
	VNodeID node_id;
	if(!co_clone_object_sub)
	{
		node_id = e_ns_get_node_id(clone);
		for(frag = e_nsm_get_fragment_next(node, 0); frag != (VNMFragmentID)-1; frag = e_nsm_get_fragment_next(node, frag + 1))
			verse_send_m_fragment_create(node_id, frag, e_nsm_get_fragment_type(node, frag), e_nsm_get_fragment(node, frag));
		co_clone_object_sub = TRUE;
	}
}

void co_clone_node(ENode *node)
{
	co_clone_from_node = e_ns_get_node_id(node);
	co_clone_object_sub = FALSE;
	verse_send_node_create(-1, e_ns_get_node_type(node), VN_OWNER_MINE);
}

void co_clone_node_update(void)
{
	ENode *node, *clone;
	if((node = e_ns_get_node(0, co_clone_from_node)) != NULL && (clone = e_ns_get_node(0, co_clone_to_node)) != NULL && e_ns_get_node_type(clone) == e_ns_get_node_type(node))
	{
		switch(e_ns_get_node_type(clone))
		{
			case V_NT_OBJECT :
				co_clone_object(node, clone);
			break;
			case V_NT_MATERIAL :
				co_material_object(node, clone);
			break;

		}
	}
}

void co_node_create_func(uint connection, uint id, VNodeType type, void *user)
{
	ENode *node;
	if((node = e_ns_get_node(0, co_clone_from_node)) != NULL && type == e_ns_get_node_type(node))
		co_clone_to_node = id;
}


extern void			e_nso_get_anim_time(EObjLink *link, uint32 *time_s, uint32 *time_f);

extern void			e_nso_get_anim_pos(EObjLink *link, double *pos);
extern void			e_nso_get_anim_speed(EObjLink *link, double *speed);
extern void			e_nso_get_anim_accel(EObjLink *link, double *accel);
extern void			e_nso_get_anim_scale(EObjLink *link, double *scale);
extern void			e_nso_get_anim_scale_speed(EObjLink *link, double *scale_speed);
extern boolean		e_nso_get_anim_active(EObjLink *link);
extern void			e_nso_get_anim_evaluate_pos(EObjLink *link, double *pos, uint32 time_s, uint32 time_f);
extern void			e_nso_get_anim_evaluate_scale(EObjLink *link, double *scale, uint32 time_s, uint32 time_f);


extern char *		e_nso_get_method_group(ENode *node, uint16 group_id);
extern uint16		e_nso_get_next_method_group(ENode *node, uint16 group_id);
extern char *		e_nso_get_method(ENode *node, uint16 group_id, uint16 method_id);
extern uint16		e_nso_get_next_method(ENode *node, uint16 group_id, uint16 method_id);
extern uint			e_nso_get_method_param_count(ENode *node, uint16 group_id, uint16 method_id);
extern char **		e_nso_get_method_param_names(ENode *node, uint16 group_id, uint16 method_id);
extern VNOParamType *	e_nso_get_method_param_types(ENode *node, uint16 group_id, uint16 method_id);
