void co_vng_init(void);

void co_vng_update_time(void);

void co_vng_saturn_ring(void);
void co_vng_geo_planet(void);
void co_vng_dust_planet(void);
void co_vng_sun(void);
void co_vng_gas_planet(void);
void co_vng_black_hole(void);
void co_vng_sunrays(void);
void co_vng_ring(void);
void co_vng_fragment(float pos_x, float pos_y, float length, float color);

void co_vng_color(float red, float green, float blue);

float *co_vng_get_sparks(uint *length);

boolean co_vng_render_link(BInputState *input, uint id, boolean connected, float pos_x, float pos_y, float link_x, float link_y, char *name);
//void co_vng_render_name(BInputState *input, ENode *node, float pos_x, float pos_y);
uint co_vng_render_name(BInputState *input, const char *name, float pos_x, float pos_y, float pointer_x, float pointer_y);

extern void co_vng_divider(BInputState *input, float pos_x, float pos_y, float *rotate, float *color, float *soft_color, boolean *on, char *text);


