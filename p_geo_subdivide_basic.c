/*
**
*/

#include "e_includes.h"

//#include "ngl.h"
#include "p_geo_subdivide_internal.h"

/* Enough is developed on Win32, using glib 1.3.x, which has a few more
** symbols than the stable 1.2.7 I currently use on Linux. The need for
** this should go away, once the later releases become available for Linux.
** /Emil
*/
#if !defined(G_MAXUINT)
#define	G_MAXUINT	UINT_MAX
#endif

/* selects what objects to needing geometry*/

void p_geo_destroy_sub_mesh(SubMesh *mesh)
{
	uint i;
	if(mesh != NULL)
	{
		if(mesh->tri_reference != NULL)
			free(mesh->tri_reference);
		if(mesh->quad_reference != NULL)
			free(mesh->quad_reference);

		if(mesh->vertex_depend_temp != NULL)
		{
			for(i = 0; i < mesh->vertex_count; i++)
				if(mesh->vertex_depend_temp[i].element != NULL)
					free(mesh->vertex_depend_temp[i].element);
			free(mesh->vertex_depend_temp);
		}
/*		if(mesh->normal_tri_ref != NULL)
			free(mesh->normal_tri_ref);
		if(mesh->normal_quad_ref != NULL)
			free(mesh->normal_quad_ref);*/
	/*	if(mesh->group_count_list != 0)
			nglFreeArray(mesh->group_count_list);
		if(mesh->cross_product_list != 0)
			nglFreeArray(mesh->cross_product_list);*/
		free(mesh);
	}
}

SubMesh *mech_cleanup(unsigned int *reference, unsigned int reference_count, unsigned int vertex_count, unsigned int *vertex_crease, unsigned int *edge_crease, unsigned int default_vertex_crease, unsigned int default_edge_crease, unsigned int level)
{
	uint32			t_cur, q_cur, i;
	SubMesh			*mesh;
 
	mesh = malloc(sizeof *mesh);

	mesh->tri_count = 0;
	mesh->quad_count = 0;
	mesh->vertex_count = 0;
	mesh->tri_crease = NULL;
	mesh->quad_crease = NULL;
	mesh->tri_backref = NULL;
	mesh->quad_backref = NULL;
	mesh->normal_tri_ref = NULL;
	mesh->normal_quad_ref = NULL;
	mesh->group_count_list = 0;
	mesh->cross_product_list = 0;

	/* get the number of polygons and quads in the object*/
	for(i = 0; i < reference_count ; i++)
	{
		if(reference[i * 4] < vertex_count && reference[(i * 4) + 1] < vertex_count && reference[(i * 4) + 2] < vertex_count)
		{
			if(reference[(i * 4) + 3] > vertex_count)
				mesh->tri_count++;
			else
				mesh->quad_count++;
		}
	}
	for(i = 1; i < level; i += 2)
	{
		mesh->tri_count *= 16;
		mesh->quad_count *= 16; 
	}

	if(mesh->tri_count != 0)
	{
		mesh->tri_reference = malloc((sizeof *mesh->tri_reference) * 3 * mesh->tri_count);
		for(i = 0; i < 3 * mesh->tri_count; i++)
			mesh->tri_reference[i] = -1;
	}
	else
		mesh->tri_reference = NULL;

	if(mesh->quad_count != 0)
	{
		mesh->quad_reference = malloc((sizeof *mesh->quad_reference) * 4 * mesh->quad_count);
		for(i = 0; i < 4 * mesh->quad_count; i++)
			mesh->quad_reference[i] = -1;
	}
	else
		mesh->quad_reference = NULL;
	mesh->vertex_depend_temp = malloc((sizeof *mesh->vertex_depend_temp) * (vertex_count + (mesh->quad_count * 5) + (mesh->tri_count * 3)));

	t_cur = 0;
	q_cur = 0;

	if(edge_crease != NULL)
	{
		mesh->tri_crease = malloc((sizeof *mesh->tri_crease) * 3 * mesh->tri_count);
		mesh->quad_crease = malloc((sizeof *mesh->quad_crease) * 4 * mesh->quad_count);

		for(i = 0; i < reference_count ; i++)
		{
			if(reference[i * 4] < vertex_count && reference[i * 4 + 1] < vertex_count && reference[i * 4 + 2] < vertex_count)
			{
				if(reference[i * 4 + 3] > vertex_count)
				{
					mesh->tri_reference[t_cur] = reference[i * 4];
					mesh->tri_crease[t_cur++] = 1 - ((float)edge_crease[i * 4] / 4294967295.0);
					mesh->tri_reference[t_cur] = reference[i * 4 + 1];
					mesh->tri_crease[t_cur++] = 1 - ((float)edge_crease[i * 4 + 1] / 4294967295.0);
					mesh->tri_reference[t_cur] = reference[i * 4 + 2];
					mesh->tri_crease[t_cur++] = 1 - ((float)edge_crease[i * 4 + 2] / 4294967295.0);
				}
				else
				{
					mesh->quad_reference[q_cur] = reference[i * 4];
					mesh->quad_crease[q_cur++] = 1 - ((float)edge_crease[i * 4] / 4294967295.0);
					mesh->quad_reference[q_cur] = reference[i * 4 + 1];
					mesh->quad_crease[q_cur++] = 1 - ((float)edge_crease[i * 4 + 1] / 4294967295.0);
					mesh->quad_reference[q_cur] = reference[i * 4 + 2];
					mesh->quad_crease[q_cur++] = 1 - ((float)edge_crease[i * 4 + 2] / 4294967295.0);
					mesh->quad_reference[q_cur] = reference[i * 4 + 3];
					mesh->quad_crease[q_cur++] = 1 - ((float)edge_crease[i * 4 + 3] / 4294967295.0);
				}
			}
		}
	}
	else
	{
		for(i = 0; i < reference_count ; i++)
		{
			if(reference[i * 4] < vertex_count && reference[(i * 4) + 1] < vertex_count && reference[(i * 4) + 2] < vertex_count)
			{
				if(reference[(i * 4) + 3] > vertex_count)
				{
					mesh->tri_reference[t_cur++] = reference[i * 4];
					mesh->tri_reference[t_cur++] = reference[(i * 4) + 1];
					mesh->tri_reference[t_cur++] = reference[(i * 4) + 2];
				}
				else
				{
					mesh->quad_reference[q_cur++] = reference[i * 4];
					mesh->quad_reference[q_cur++] = reference[(i * 4) + 1];
					mesh->quad_reference[q_cur++] = reference[(i * 4) + 2];
					mesh->quad_reference[q_cur++] = reference[(i * 4) + 3];
				}
			}
		}
		if(default_edge_crease > 0)
		{
			mesh->tri_crease = malloc((sizeof *mesh->tri_crease) * t_cur);
			mesh->quad_crease = malloc((sizeof *mesh->quad_crease) * q_cur);
			for(i = 0; i < t_cur; i++)
				mesh->tri_crease[i] = ((float)default_edge_crease / 4294967295.0);
			for(i = 0; i < q_cur; i++)
				mesh->quad_crease[i] = ((float)default_edge_crease / 4294967295.0);
		}
		else
		{
			mesh->tri_crease = NULL;
			mesh->quad_crease = NULL;
		}
	}

	mesh->vertex_count = vertex_count;

	if(vertex_crease != NULL)
	{
		mesh->vertex_crease = malloc((sizeof *mesh->vertex_crease) * vertex_count);
		for(i = 0; i < vertex_count; i++)
			mesh->vertex_crease[i] = 1.0 - ((float)vertex_crease[i * 4 + 3] / 4294967295.0);
	}
	else
		mesh->vertex_crease = NULL;

	mesh->tri_count = t_cur / 3;
	mesh->quad_count = q_cur / 4;
	if(t_cur == 0 && q_cur == 0)
	{
		free(mesh->vertex_depend_temp);
		mesh->vertex_depend_temp = NULL;
		return NULL;
	}
	return mesh;
}

void clear_mesh(SubMesh *mesh)
{
	uint i;

	if(mesh != NULL)
	{

		if(mesh->quad_backref != NULL)
			for(i = 0; i < mesh->vertex_count; i++)
				if(mesh->quad_backref[i] != NULL)
					free(mesh->quad_backref[i]);
		free(mesh->quad_backref);
		mesh->quad_backref = NULL;


		if(mesh->tri_reference != NULL)
			for(i = 0; i < mesh->tri_count * 3; i++)
				mesh->tri_reference[i] =  -1;
		mesh->tri_count = 0;

		if(mesh->quad_reference != NULL)
			for(i = 0; i < (mesh->quad_count * 4); i++)
				mesh->quad_reference[i] = -1;
		mesh->quad_count = 0;

		if(mesh->tri_backref != NULL)
			for(i = 0; i < mesh->vertex_count; i++)
			    if(mesh->tri_backref[i] != NULL)
				    free(mesh->tri_backref[i]);

		free(mesh->tri_backref);
		mesh->tri_backref = NULL;
		mesh->vertex_count = 0;
	}
}

SubMesh *allocate_mech(SubMesh *input, uint level)
{
	uint32			tri_count, quad_count, i;
	SubMesh			*mesh;

	mesh = malloc(sizeof *mesh);

	tri_count = input->tri_count * 4;
	quad_count = input->quad_count * 4;

	for(i = 2; i < level; i += 2)
	{
		tri_count *= 16;
		quad_count *= 16; 
	}

	if(tri_count != 0)
	{
		mesh->tri_reference = malloc(sizeof(uint32) * tri_count * 3);
		for(i = 0; i < tri_count * 3; i++)
			mesh->tri_reference[i] = -1;
	}
	else 
		mesh->tri_reference = NULL;
	if(quad_count != 0)	      
	{
		mesh->quad_reference = malloc(sizeof(uint32) * quad_count * 4);
		for(i = 0; i < quad_count * 4; i++)
			mesh->quad_reference[i] = -1;
	}
	else
		mesh->quad_reference = NULL;

	mesh->vertex_depend_temp = malloc((sizeof *mesh->vertex_depend_temp) * (input->vertex_count + (quad_count * 5) + (tri_count * 3)));

	mesh->tri_count = 0;
	mesh->quad_count = 0;

	mesh->vertex_count = 0;

	mesh->tri_crease = input->tri_crease;
	mesh->quad_crease = input->quad_crease;

	mesh->tri_backref = NULL;
	mesh->quad_backref = NULL;
	mesh->vertex_crease = input->vertex_crease;
	return mesh;
}
