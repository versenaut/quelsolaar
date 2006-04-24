
extern void p_extension_init(void *(*gl_GetProcAddress)(const char* proc));
extern boolean p_extension_test(const char *string);

extern void p_shader_init(void);
extern void p_shader_param_load(ENode *parent, uint32 node_id, void *array, uint count, uint environment, uint diffuse_environment);

extern uint p_shader_get_param_count(ENode *node);
extern VMatFrag *p_shader_get_param(ENode *node, uint nr);

extern char	p_shader_get_source_vertex(ENode *node);
extern char	p_shader_get_source_frag(ENode *node);
extern void p_shader_bind(uint32 node_id);
extern void p_shader_unbind(uint32 node_id);

extern boolean p_shaders_supported(void);
extern boolean p_shader_shadow_bind(void);
