
extern void la_do_init(void);
extern void la_do_edge_select(double *vertex_a, double *vertex_b);
extern void la_do_edge_split(double *vertex_a, double *vertex_b, double pos);
extern void la_do_edge_delete(float ax, float ay, float az, float bx, float by, float bz, float cx, float cy, float cz);
extern void la_do_edge_delete_air(double *vertex_a, double *vertex_b);

extern void la_do_active_vertex(double *vertex, boolean move);
extern void la_do_xyz_lines(double *start, boolean snap);
extern void la_do_draw(double *start, double *end, boolean snap, double *closest);
extern void la_do_owerlay(void);
extern void la_do_draw_closest_edge(uint *edge, double x, double y, boolean snap);
