extern void		uvg_init(void);
extern void		uvg_set_node(uint node_id);
extern uint		uvg_get_node(void);

extern boolean	uvg_update(void);

extern uint		uvg_get_next_polygon(uint id);

extern boolean	uvg_is_quad(uint id);
extern uint		uvg_get_sides(uint id);

extern void		uvg_get_uve(egreal *uv, uint id);
extern boolean	uvg_get_polygon_select(uint id);
extern egreal	*uvg_get_corner_select(uint id);

extern uint		uvg_get_struct_version(uint id);
extern uint		uvg_get_uv_version(uint id);

extern uint		*uvg_get_ref(void);
extern egreal	*uvg_get_vertex(void);

extern uint		uvg_get_vertex_length(void);
extern uint		uvg_get_polygon_length(void);

extern void		uvg_set_all_corner_select(uint id, egreal c0, egreal c1, egreal c2, egreal c3);
extern void		uvg_set_one_corner_select(uint id, uint corner, egreal select);
extern void		uvg_set_corner_select_all(egreal select);

extern void		uv_find_closest_corner(egreal *output, float x, float y, boolean selected);

extern void		uvg_get_un_colapse(uint id);

extern void		uvg_set_all_corner_uv(uint id, egreal u0, egreal v0, egreal u1, egreal v1, egreal u2, egreal v2, egreal u3, egreal v3);
extern void		uvg_set_one_corner_uv(uint id, uint corner, egreal u, egreal v);

extern boolean	uvg_node_has_uv(void);
extern void		uvg_node_create_uv(void);

extern void		uvg_action_end(void);

extern void		uvg_redo(void);
extern void		uvg_undo(void);
