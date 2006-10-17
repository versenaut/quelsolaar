/*
 * 
*/

extern boolean co_w_checkbox(BInputState *input, float pos_x, float pos_y, boolean *state, float alpha);
extern boolean co_w_type_in(BInputState *input, float pos_x, float pos_y, float length, float size, char *text, uint buffer_size, void (*done_func)(void *user, char *text), void* user, float text_color, float line_color);
extern boolean co_w_slider(BInputState *input, float pos_x, float pos_y, float length, double *nr, double min, double max, float alpha);

extern void sui_draw_rounded_square(float pos_x, float pos_y, float size_x, float size_y, float red, float green, float blue, float alpha);
extern boolean co_w_close_button(BInputState *input, float pos_x, float pos_y, float alpha);

extern float co_w_scroll(BInputState *input, float pos, float size);
