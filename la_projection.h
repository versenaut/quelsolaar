
extern void p_init(void);
extern void p_view_change_start(BInputState *input);
extern void p_view_change(BInputState *input);

extern void p_projection_update(void);

extern void p_set_view_center(double *center);
extern void p_get_view_center(double *center);
extern void p_get_view_camera(double *camera);
extern double p_get_distance_camera(void);
extern void p_set_grid_size(double grid_size);

extern void p_get_model_matrix(double *matrix);
extern void p_view_set(void);

extern void p_get_projection(double *output, float x, float y);
extern void p_get_projection_vertex(double *output, double *vertex, double x, double y);
extern void p_get_projection_screen(double *output, double x, double y, double z);
extern double p_get_projection_screen_distance(double space_x, double space_y, double space_z, double screen_x, double screen_y);
extern void p_get_projection_plane(double *dist, uint axis, double pointer_x, double pointer_y , double depth);
extern double p_get_projection_line(double *dist, uint axis, double pointer_x, double pointer_y, double *pos);


typedef enum{
	LA_ST_VERTEX,
	LA_ST_EDGE,
	LA_ST_SURFACE
}LASnapType;

extern void p_get_projection_vertex_with_axis(double *output, double *start, double pointer_x, double pointer_y, boolean snap, double *closest, LASnapType snap_type);
extern uint p_get_projection_axis(void);
extern void p_get_projection_line_snap(double *output, uint axis, double direction, double *start, double *snap, LASnapType snap_type);
extern void p_find_closest_vertex(uint *closest, uint *selected, double *distance, double *selected_distance, double x, double y);
extern boolean p_find_closest_edge(uint *edge, double *distance, double x, double y);
extern boolean p_find_closest_tag(double *pos, double distance, double x, double y);
extern uint p_find_click_tag(double x, double y);
extern boolean p_find_click_tag_lable(double x, double y);
