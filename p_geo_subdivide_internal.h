#if !defined(SUBDIVIDE_H)
#define	SUBDIVIDE_H

//#include "ngl.h"
#include "enough.h"
#include "st_types.h"

#define	TESSELATION_TABLE_MAX_LEVEL 8

typedef struct{	
	ebreal	*vertex_influence; /*[array] vertex number , andt the values it is combined by in series of 3 or 4*/
	uint	*index; /*[array] the reference offset for the final index */
	uint	*reference; /*[array] the vertex order of ref form the rop data */
	uint	vertex_count; /* number of vertex in the tesselated polygon */
	uint	element_count; /* number of elements in the tesselated polygon */
}PTessTableElement;

extern void init_tess_tables(uint tess_level);
extern void init_tri_tess_tables(uint tess_level);
extern PTessTableElement	*get_dynamic_table_tri(uint base, uint *edge);
extern PTessTableElement	*get_dynamic_table_quad(uint base, uint *edge);
extern uint p_get_max_tess_level(void);
/*
typedef struct{
	SubPrecision x;
	SubPrecision y;
	SubPrecision z;
}SubPoint;

typedef struct{
	PTessTableElement	**tri_tess;
	PTessTableElement	**quad_tess;
	uint				tri_count;
	uint				quad_count;
	uint32				*depend_reference;
	SubPrecision		*depend_weight;
	uint32				*depend_ref_count;
	uint32				*reference;
	uint32				element_count;
	SubPrecision		*vertex_array;
	uint32				vertex_count;
	float				**param_array;			
	uint				param_count;
}DrawMesh;
*/
typedef struct{
	void		*array;
	ebreal		*output;
	uint		array_type;
	uint		ofset;	
	uint		groop_size;
}ParamArrayCreate;

typedef struct{
	ebreal						value;
	ebreal						crease;
	uint						edge_counts;
	uint32						vertex_ref;
}SubCreaseDependElement;

typedef struct{
	ebreal						value;
	uint32						vertex_ref;
}SubVertexDependElement;

typedef struct{
	uint					length;
	uint					alocated;
	ebreal					sum;
	SubVertexDependElement	*element;
}SubVertexDepend;

typedef struct{
	uint			base_level;
	uint32			tri_count;
	uint32			quad_count;

	SubVertexDepend	*vertex_depend_temp;
	float			*vertex_crease;
	float			*tri_crease;
	float			*quad_crease;
	uint32			vertex_count;
	uint32			cv_count;

	uint32			*tri_reference;
	uint32			*quad_reference;

	uint32			**tri_backref; /* temp pointers */
	uint32			**quad_backref; /* temp pointers */

	uint32			*normal_tri_ref;
	uint32			*normal_quad_ref;
	uint32			group_count_list;
	uint32			cross_product_list;
	uint32			version;
	void			*next;
}SubMesh;

extern SubMesh *p_geometry_get_sds(ENode *node);

extern SubMesh *mech_cleanup(unsigned int *reference, unsigned int reference_count, unsigned int vertex_count, unsigned int *vertex_crease, unsigned int *edge_crease, unsigned int default_vertex_crease, unsigned int default_edge_crease, unsigned int level);
extern SubMesh *allocate_mech(SubMesh *input, uint level);
extern void clear_mesh(SubMesh *mesh);
extern void p_geo_destroy_sub_mesh(SubMesh *mesh);
extern uint32 tri_backref(SubMesh *mesh);
extern uint32 quad_backref(SubMesh *mesh);
extern void create_vertex_normal_data(SubMesh *geometry, uint extra_tri, uint extra_quad);

typedef struct{
	void			**tri_tess; /* the selected tri tesselation */
	void			**quad_tess; /* the selected quad tesselation */
	uint			tri_count; /* number of control tris */
	uint			quad_count; /* number of control quads */
//	NGLArray		depend_reference;
//	NGLArray		depend_weight;
//	NGLArray		depend_ref_count;
//	NGLArray		normal_depend_reference;
//	NGLArray		normal_depend_weight;
//	NGLArray		normal_depend_ref_count;*/
	uint32			cv_count;
//	NGLArray 		cv_array; /* control vertexes */
//	NGLArray 		cv_normal; /* control normals */
//	NGLArray 		displacement; /* legth of eatch vertexes displacement */
//	NGLArray 		vertex_array; /* the subdivided vertex array */
//	NGLArray 		normal_array; /* the subdivided normal array */
	uint32			vertex_count; /* the number of subdivided vertex array */
//	NGLArray 		reference; /* the final reference array */
	uint32			element_count; /* the length of the reference array */
	SubMesh			*sub_mesh;
	void			*anim;
} EOPMesh;

typedef struct{
	uint geometry_id;
	uint geometry_version;
	uint stage;
	uint sub_stage;
	struct{
		void **tri_tess; /* the selected tri tesselation */
		void **quad_tess; /* the selected quad tesselation */
		uint tri_count; /* number of control tris */
		uint quad_count; /* number of control quads */
		egreal factor;
		uint force;
		egreal eay[3];
		egreal (*edge_tess_func)(egreal *v_0, egreal *v_1, egreal *e_0, egreal *eay);
	}tess;
	struct{
		egreal *vertex_array; /* the subdivided vertex array */
		egreal *normal_array; /* the subdivided normal array */
		uint32 vertex_count; /* the number of subdivided vertex array */
		uint32 *reference; /* the final reference array */
		uint32 element_count; /* the length of the reference array */
	}render;
	struct{
		uint32	*reference;
		egreal	*weight;
		uint32	*ref_count;
		uint	length;
	}depend;
	struct{
		uint32	*reference;
		uint32	*count;
		egreal	*weight;
	}normal;
}PMesh;


//extern EOPMesh *make_ngl_mesh(SubMesh *mesh, NGLArray cv_array, uint cv_count, float tess_factor, uint force_tess, Point *eay, Point *scale);
//extern void create_material_ngl_array(EOPMesh *mesh, NGLArray id, uint32 *reference, uint slot, void *data, uint type);
extern void create_material_array(EOPMesh *mesh, float *output, uint32 *reference, uint slot, void *data, uint type);
//extern NGLArray create_ngl_mesh_normals(EOPMesh *draw_mesh);
//extern NGLArray get_ngl_mesh_displacement(EOPMesh *mesh);
//extern void free_ngl_mesh(EOPMesh *mesh);



extern float *create_vertex_crease_data(SubMesh *geometry, uint *cv_creas, uint extra_count);
extern void rop_cv_vertexref(SubMesh *geometry, SubMesh *last_geometry, uint cv_count, float *cv_crease);
extern void rop_vertexref(SubMesh *geometry, SubMesh *last_geometry, uint cv_count, uint level);
extern void rop_faceref(SubMesh *geometry, SubMesh *last_geometry);
extern void rop_edgeref(SubMesh *geometry, SubMesh *last_geometry, uint level);
extern void rop_sub_re_depend(SubMesh *geometry, SubMesh *last_geometry);

extern void e_create_material_array(EOPMesh *mesh, float *output, uint32 *reference, uint slot, void *data, uint type);

#endif

