
#define CO_ENOUGH_NODE_SLOT 2
#define CO_ENOUGH_DATA_SLOT 1
#define CONNECTOR_ENOUGH_SLOTT 1



typedef struct{
	uint		node_id;
	float		pos_x;
	float		pos_y;
	boolean		search;	
	boolean		viewlock;	
	boolean		hidden;
	void		*render_cash;
	void		*persuade;
}COVerseNode;

extern void co_init_handle_verse_node(void);

extern uint co_vn_mouse_over(BInputState *input);
extern void co_vn_unhide_node(uint node_id, float pos_x, float pos_y);
extern void co_vn_unhide_all(VNodeType type);
extern void co_vn_hide_all(VNodeType type);
extern void co_vn_hide_node(uint node_id);
extern boolean co_vn_hidden_test(uint node_id);
extern void *co_vn_node_get(uint node_id);

extern void co_vn_handle(BInputState *input);

extern boolean co_handle_object(BInputState *input, ENode *node);
extern boolean co_handle_bitmap(BInputState *input, ENode *node);
extern boolean co_handle_geometry(BInputState *input, ENode *node);
extern boolean co_handle_material(BInputState *input, ENode *node);
extern boolean co_handle_text(BInputState *input, ENode *node);
extern boolean co_handle_curve(BInputState *input, ENode *node);
extern boolean co_handle_audio(BInputState *input, ENode *node);

extern boolean co_handle_return(BInputState *input);
extern void sui_draw_symb_close(float pos_x, float pos_y, float red, float green, float blue);

extern void co_draw_material(ENode *node);

extern uint co_vn_mouse_test_type(float pos_x, float pos_y, VNodeType type);
extern uint co_vn_mouse_test(float pos_x, float pos_y);
extern boolean co_vn_position_get(uint node_id, float *pos);

extern void material_func(ENode *node, VNMFragmentID frag, ECustomDataCommand command);

extern void co_vn_link_init(void);
extern void co_vn_link_start(uint node_id, VNodeType node_type, uint fragment);
extern void co_vn_link_continue(BInputState *input, float pos_x, float pos_y);
extern boolean co_vn_link_active(void);
