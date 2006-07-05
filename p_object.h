
#define LIGHTS_PER_OBJECT 8

typedef struct{
	boolean impostor;
	uint version;
	uint texture;
	float vectror[3];
	float length;
	float timer;
	float priority;
	double matrix[16];
	double size;
	float vertex_array[12];
	uint environment;
}PObjImpostor;

typedef struct{
	uint32		node_id;
	uint32		lights[LIGHTS_PER_OBJECT];
	float		lightfade;
}PObjLight;

typedef struct{
	uint32		node_id;
	PMesh		**meshes;
	uint		mesh_count;
	uint		version;
	PObjLight	light;
	void		*environment;
	PObjImpostor impostor;
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

/* handeling drawable meshas */

void	p_rm_set_eay(PMesh *mesh, egreal *eay);
void	p_rm_destroy(PMesh *mesh);
PMesh	*p_rm_service(PMesh *mesh, ENode *o_node, const egreal *vertex);
void	p_rm_unvalidate(PMesh *mesh);
boolean p_rm_drawable(PMesh *mesh);
uint	p_rm_get_geometry_node(PMesh *mesh);


/* light */


extern double	p_light_compute_brightnes(double *obj, uint32 node_id, uint32 time_s, uint32 time_f);
extern void		p_light_update(PObjLight *light, uint32 node_id, uint light_count, uint32 time_s, uint32 time_f);
extern void		p_set_light(PObjLight *light, uint light_count, uint32 time_s, uint32 time_f);
extern void		p_set_shadow_light(uint32 time_s, uint32 time_f);

