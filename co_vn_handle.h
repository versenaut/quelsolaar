
#define CO_ENOUGH_NODE_SLOT 2
#define CO_ENOUGH_DATA_SLOT 1

extern void co_init_handle_verse_node();

extern uint co_vn_mouse_over(BInputState *input);
extern void co_vn_unhide_node(uint node_id, float pos_x, float pos_y);
extern void co_vn_unhide_all(VNodeType type);
extern void co_vn_hide_all(VNodeType type);
extern void co_vn_hide_node(uint node_id);
extern boolean co_vn_hiden_test(uint node_id);
extern void *co_vn_node_get(uint node_id);

extern void co_vn_handle(BInputState *input);


extern void *co_o_node_create(uint node_id);
extern void *co_g_node_create(uint node_id);
extern void *co_m_node_create(uint node_id);
extern void *co_b_node_create(uint node_id);
extern void *co_c_node_create(uint node_id);
extern void *co_e_node_create(uint node_id);

extern void co_o_node_destroy(void *node);
extern void co_g_node_destroy(void *node);
extern void co_m_node_destroy(void *node);
extern void co_b_node_destroy(void *node);
extern void co_c_node_destroy(void *node);
extern void co_e_node_destroy(void *node);

extern float co_o_node_handle(BInputState *input, uint node_id, void *node, float pos_x, float pos_y);
extern float co_g_node_handle(BInputState *input, uint node_id, void *node, float pos_x, float pos_y);
extern float co_m_node_handle(BInputState *input, uint node_id, void *node, float pos_x, float pos_y);
extern float co_b_node_handle(BInputState *input, uint node_id, void *node, float pos_x, float pos_y);
extern float co_c_node_handle(BInputState *input, uint node_id, void *node, float pos_x, float pos_y);
extern float co_e_node_handle(BInputState *input, uint node_id, void *node, float pos_x, float pos_y);

extern boolean co_handle_object(BInputState *input, ENode *node);
extern boolean co_handle_bitmap(BInputState *input, ENode *node);
extern boolean co_handle_geometry(BInputState *input, ENode *node);
extern boolean co_handle_material(BInputState *input, ENode *node);
extern boolean co_handle_text(BInputState *input, ENode *node);
extern boolean co_handle_curve(BInputState *input, ENode *node);

extern boolean co_handle_return(BInputState *input);
extern void sui_draw_symb_close(float pos_x, float pos_y, float red, float green, float blue);

extern void co_draw_material(ENode *node);

extern void co_o_node_draw(uint node_id);
extern void co_g_node_draw(uint node_id);
extern void co_m_node_draw(uint node_id);
extern void co_b_node_draw(uint node_id);
extern void co_c_node_draw(uint node_id);
extern void co_e_node_draw(uint node_id);

extern uint co_vn_mouse_test_type(float pos_x, float pos_y, VNodeType type);
extern uint co_vn_mouse_test(float pos_x, float pos_y);
extern boolean co_vn_position_get(uint node_id, float *pos);

extern void material_func(ENode *node, VNMFragmentID frag, ECustomDataCommand command);

extern void co_vn_link_init(void);
extern void co_vn_link_start(uint node_id,	VNodeType node_type, uint fragment);
extern void co_vn_link_continue(BInputState *input, float pos_x, float pos_y);
boolean co_vn_link_active(void);
