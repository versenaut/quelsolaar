
extern void la_pfx_init(uint particle_count, uint texture_size);
extern void la_pfx_create_spark(double *pos);
extern void la_pfx_create_intro_spark(double *pos);
extern void la_pfx_create_aches(double *pos);
extern void la_pfx_create_dust_line(double *pos_a, double *pos_b);
extern void la_pfx_create_dust_selected_vertexes(double *mid);
extern void la_pfx_create_aches_surface(double *v0, double *v1, double *v2, double *v3);
extern void la_pfx_create_bright(double *pos);
extern void la_pfx_video_flare(void);
extern void la_pfx_select_vertex(void);
extern void la_pfx_draw(boolean intro);
extern void la_pfx_draw_intro(void);
