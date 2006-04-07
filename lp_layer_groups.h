
extern uint lp_layer_current_get();
extern void lp_layer_current_set(uint group);

extern EGeoLayer *lp_layer_get_red(uint group);
extern EGeoLayer *lp_layer_get_green(uint group);
extern EGeoLayer *lp_layer_get_blue(uint group);

extern char *lp_layer_get_name(uint group);

extern uint lp_layer_get_group_count();

extern void lp_update_layer_groups(ENode *node);
