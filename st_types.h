#if !defined(TYPES_H)
#define	TYPES_H

#define ARRAY_CHUNK_SIZE 16
#define ARRAY_CHUNK_SIZE_SMALL 8
#define ARRAY_CHUNK_SIZE_LARGE 128

#if !defined(TRUE)
#define TRUE 1
#endif
#if !defined(FALSE)
#define FALSE 0
#endif
#if defined _WIN32
typedef unsigned int uint;
#else
#include <sys/types.h>
#endif
#if !defined(VERSE_TYPES)
#define VERSE_TYPES
typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef unsigned char boolean;
#endif

#define PI  3.141592653
/*
typedef struct{
	float	x;
	float	y;
	float	z;
}Point;
*/
typedef struct{
	float	x;
	float	y;
	float	z;
}DPoint;
/*
typedef struct{
	Point	pos;
	float	r;
}HomoPoint;
*/
typedef struct{
	float	r;
	float	g;
	float	b;
}Color;

typedef struct{
	Color	col;
	float	a;
}ColorAlpha;

typedef struct{
	void	**data;
	uint	allocated;
}DynLookUpTable;

extern void dv2_copy(double *a, double *b);
extern void dv3_copy(double *a, double *b);

extern double dv2_dot(double *a, double *b);
extern double dv3_dot(double *a, double *b);

extern double dv2_length(double *a);
extern double dv3_length(double *a);

extern void dv2o_sub(double *output, double *a, double *b);
extern void dv3o_sub(double *output, double *a, double *b);

extern void dv2_sub(double *a, double *b);
extern void dv3_sub(double *a, double *b);

extern void dv2o_sub(double *output, double *a, double *b);
extern void dv3o_sub(double *output, double *a, double *b);

extern void dv2_add(double *a, double *b);
extern void dv3_add(double *a, double *b);

extern void dv2o_add(double *output, double *a, double *b);
extern void dv3o_add(double *output, double *a, double *b);

extern void dv2_mult(double *a, double *b);
extern void dv3_mult(double *a, double *b);

extern void dv2o_mult(double *output, double *a, double *b);
extern void dv3o_mult(double *output, double *a, double *b);

extern void dv2_division(double *a, double b);
extern void dv3_division(double *a, double b);

extern void dv2o_division(double *output, double *a, double b);
extern void dv3o_division(double *output, double *a, double b);


extern void dv2_cross(double *a, double *b);
extern void dv3_cross(double *a, double *b);

extern void dv2o_cross(double *output, double *a, double *b);
extern void dv3o_cross(double *output, double *a, double *b);

extern void dv2_3point_cross(double *output, double *origo, double *a, double *b);
extern void dv3_3point_cross(double *output, double *origo, double *a, double *b);

extern void dv2_normalize(double *a);
extern void dv3_normalize(double *a);

extern void init_dlut(DynLookUpTable *table);
extern void free_dlut(DynLookUpTable *table);
extern void *find_dlut(DynLookUpTable *table, uint key);
extern void *add_entry_dlut(DynLookUpTable *table, uint key, void *pointer);
extern uint add_entry_in_empty_dlut(DynLookUpTable *table, void *pointer);
extern void remove_entry_dlut(DynLookUpTable *table, uint key);
extern uint count_entry_dlut(DynLookUpTable *table);
extern void clean_dlut(DynLookUpTable *table);
extern void *get_next_dlut(DynLookUpTable *table, uint key);
extern uint get_next_empty_dlut(DynLookUpTable *table, uint key);
extern void foreach_remove_dlut(DynLookUpTable *table, void (*func)(uint key, void *pointer, void *user_data), void *user_data);

extern double get_rand(uint32 index);

#endif
