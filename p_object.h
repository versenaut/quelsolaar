
#define LIGHTS_PER_OBJECT 8

typedef struct{
	uint32	id;
	uint32	material;
	uint32	target;
	uint	vertex_version;
	PMesh	*draw;	
	PMesh	*progress;
	void	*param;
}PRenderSets;

typedef struct{
	uint32		node_id;
	PRenderSets *sets;
	uint		set_count;
	PRenderSets temp;
	uint32		lights[LIGHTS_PER_OBJECT];
	void		*environment;
	boolean		task;
}PObject;

extern void *p_env_compute(void *env);
extern void p_env_destroy(void *env);
extern void p_env_init(unsigned int size);
extern uint p_env_get_environment(void *env);
extern uint p_env_get_diffuse(void *env);

extern void *p_param_array_allocate(ENode *g_node, ENode *m_node);
extern void p_param_array_destroy(void *p);
extern boolean p_array_update(void *p, PMesh *mesh, ENode *g_node, ENode *m_node);
extern egreal **p_param_get_array(void *p);
