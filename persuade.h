/* Undef this to disable use of Persuade in the various applications. The symbol
 * actually cared about by the other code is PERSUADE_H, so that is wrapped here
 * by the PERSUADE_ENABLED symbol for ease of disabling/enabling.
 *
 * Typically, we set this externally in the Makefile, since there are linking
 * considerations too.
*/

#define PERSUADE_ENABLED

#if defined PERSUADE_ENABLED

#if !defined PERSUADE_H
#define	PERSUADE_H

#include "enough.h"

typedef void PMesh;
extern void persuade_init(uint max_tesselation_level, void *(*gl_GetProcAddress)(const char* proc)); /* initialize persuade and set the maximum SDS level*/
extern void p_task_compute(uint count); /* this function makes Persuade update everything that needs to be computedcompute */

/* settings */

extern void		p_geo_set_sds_level(uint level);
extern uint		p_geo_get_sds_level(void);
extern void		p_geo_set_sds_poly_cap(uint poly_cap);
extern uint		p_geo_get_sds_poly_cap(void);
extern void		p_geo_set_sds_compute_timer(float timer);
extern float	p_geo_get_sds_compute_timer(void);

extern void		p_geo_set_sds_force_level(uint level);
extern uint		p_geo_get_sds_force_level(void);
extern void		p_geo_set_sds_mesh_factor(float factor);
extern float	p_geo_get_sds_mesh_factor(void);

extern void		p_render_set_shadows(boolean shadows);
extern boolean	p_render_get_shadows(void);

extern void		p_render_set_wireframe(boolean wireframe);
extern boolean	p_render_get_wireframe(void);



extern void		p_render_set_impostor_size(float size);
extern float	p_render_get_impostor_size(void);
extern void		p_render_set_impostor_resolution(uint resolution);
extern uint		p_render_get_impostor_resolution(void);

/*renderable meshes*/

extern PMesh	*p_rm_create(ENode *node); /* use to create a drawabel version of a geometry node */
extern void		p_rm_destroy(PMesh *mesh); /* Destroy the same*/

extern void		*p_rm_service(PMesh *mesh, ENode *o_node, const egreal *vertex); /* service the mesh to make it drawabel */
extern void		p_rm_compute(PMesh *mesh, ENode *o_node, const egreal *vertex);
extern void		p_rm_update_shape(PMesh *mesh, egreal *vertex);
extern void		p_rm_set_eay(PMesh *mesh, egreal *eay); /* set a view point if you want view specific LOD */
extern boolean	p_rm_validate(PMesh *mesh); /* is the mesh in sync with the geometry node */
extern boolean	p_rm_drawable(PMesh *mesh); /* is the mesh drawable */
extern void		p_rm_unvalidate(PMesh *mesh); /* make mesh invalud */

extern egreal	*p_rm_get_vertex(PMesh *mesh); /* get the array of vertices to draw */
extern egreal	*p_rm_get_normal(PMesh *mesh); /* get the array of normals to draw */
extern uint		p_rm_get_vertex_length(PMesh *mesh); /* get the length of the vertex array */
extern uint		*p_rm_get_reference(PMesh *mesh); /* get the reference array of the object */
extern uint		p_rm_get_ref_length(PMesh *mesh); /* get the length of the reference array */

extern uint		p_rm_get_mat_count(PMesh *mesh); /* get tyhe number of materials this object has */
extern uint		p_rm_get_material_range(PMesh *mesh, uint mat); /* what is the last index range this material has */
extern uint		p_rm_get_material(PMesh *mesh, uint mat); /* what is the id of the material */

extern boolean	p_rm_get_shadow(PMesh *mesh);

extern void		p_lod_set_view_pos(double *view_cam);
extern void		p_lod_set_view_matrix(double *view_matrix);

/*extern egreal	*p_rm_get_param(PMesh *mesh, uint16 layer_r, uint16 layer_g, uint16 layer_b);*/

extern egreal	p_rm_compute_bounding_box(ENode *node, egreal *vertex, egreal *center, egreal *scale);

/* Texture handler */

typedef void PTextureHandle;

extern void		p_th_texture_restart(void);
extern PTextureHandle *	p_th_create_texture_handle(uint node_id, char *layer_r, char *layer_g, char *layer_b);
extern void		p_th_destroy_texture_handle(PTextureHandle *handle);
extern uint		p_th_get_texture_id(PTextureHandle *handle);
extern uint		p_th_get_texture_dimensions(PTextureHandle *handle);

extern void		p_draw_scene(void);

#endif		/* PERSUADE_H */

#endif		/* PERSUADE_ENABLED */
