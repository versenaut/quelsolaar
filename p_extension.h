
extern void		p_extension_init(void *(*gl_GetProcAddress)(const char* proc));
extern void		*p_extension_get_address(const char* proc);
extern boolean	p_extension_test(const char *string);
