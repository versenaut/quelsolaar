extern void uvo_draw_square();

extern void get_tri_edge(egreal *output, uint edge, egreal *uv);
extern void get_quad_edge(egreal *output, uint edge, egreal *uv);

extern void get_tri_corner(egreal *output, uint corner, egreal *uv, egreal select);
extern void get_quad_corner(egreal *output, uint corner, egreal *uv, egreal select);

extern void get_inner_corner(egreal *output, uint id, uint corner);

extern void uvo_draw_overlay(BInputState *input);
extern void uvo_set_overlay_motion(float move);

/* view */

extern void uv_handle_view(BInputState *input, float *pos, boolean active);

extern float uv_get_pos_x(float x);
extern float uv_get_pos_y(float y);
extern float uv_get_view_x(float x);
extern float uv_get_view_y(float y);

extern void uv_center(float x, float y);
extern float *get_cam_pos();

extern boolean uv_test_tri(egreal *uv, float x, float y);
extern boolean uv_test_quad(egreal *uv, float x, float y);

extern uint uv_test_corner(uint id, float x, float y);
extern uint uv_test_edge(uint id, float x, float y);

extern boolean uv_test_polygon(uint id, float x, float y);
extern uint uv_test_all_polygons(float x, float y);

extern void uv_find_closest_corner(egreal *output, float x, float y, boolean selected);



/* draw */

void uv_init_draw_line(void);
void uv_new_draw_line(void);
void uv_draw_line_add(float x, float y, boolean add);
boolean uv_test_draw_line_point(egreal *pos);
void uv_draw_line_test_select(void);

/* transform */


extern void uv_trans_init();

extern void uv_trans_move_corner(BInputState *input, uint poly_id, uint corner);
extern void uv_trans_move_edge(BInputState *input, uint poly_id, uint corner);
extern void uv_trans_draw();

extern boolean uv_trans_begin(BInputState *input);
extern void uv_trans_continiue(BInputState *input);
extern void uv_trans_end();

extern void uv_trans_unhide();
extern void uv_trans_hide();

extern void uv_trans_get_pos(egreal *pos);

extern boolean uv_trans_test_manip(BInputState *input);

/* pop up */

extern void uv_pu_empty(BInputState *input);
extern void uv_pu_polygon(BInputState *input, uint poly);
extern void uv_pu_corner(BInputState *input, uint poly, uint corner);
extern void uv_pu_edge(BInputState *input, uint poly, uint edge);
extern void uv_pu_manip(BInputState *input);

/* menu */

extern void uv_draw_menu(BInputState *input);
extern boolean uv_test_menu(BInputState *input);

/* corner tools */

extern void uv_tool_corner_colaps(uint poly_id, uint corner);
extern void uv_tool_corner_select(uint poly_id, uint corner);
extern void uv_tool_edge_straight_uv(uint poly_id, uint corner);

/* edge tools */

extern void uv_tool_edge_streight(uint poly_id, uint edge);
extern void uv_tool_edge_select_streight(uint poly_id, uint edge);
extern void uv_tool_edge_poly_streight(uint poly_id, uint edge);

/* poly tools */

extern void uv_tool_poly_square(uint poly_id);
extern void uv_tool_poly_rectangle(uint poly_id);
extern void uv_tool_project(uint u, uint v, boolean unselected);
extern void uv_tool_poly_normal_select(uint poly_id);
extern void uv_tool_poly_project(uint poly);
extern void uv_tool_poly_plane_select(uint poly);
/* select tools */

extern void uv_tool_flip_x();
extern void uv_tool_flip_y();
extern void uv_fit_selection(boolean preserve_aspect);

/* unfold */

extern uint uv_polygon_unfold(uint id, float x, float y);
extern void uv_start_polygon_unfold(uint id, float x, float y);


extern void uve_init();
extern void uve_update();
extern void uve_draw();
extern boolean uve_test(float x, float y);

/* strips */

void uv_strip(uint polygon, uint edge);
void uv_strip_horizontal(uint polygon);
void uv_strip_vertical(uint polygon);
void uv_strip_plane(uint polygon);
