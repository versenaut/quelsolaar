#include <stdio.h>
#include <stdlib.h>


#include "p_geo_subdivide_internal.h"

uint32 tri_backref(SubMesh *mesh)
{
	uint32		ref, i, *write;
	uint32		**tri_backref, large = 1;

	if(mesh->tri_count > 0)
	{
		tri_backref = malloc((sizeof *tri_backref) * (mesh->vertex_count + 1));
		for(i = 0 ; i < mesh->vertex_count; i++)
			tri_backref[i] = NULL;
		mesh->tri_backref = tri_backref;

		for(i = 0 ; i < mesh->tri_count * 3; i++)
		{
			ref = mesh->tri_reference[i];
			if(tri_backref[ref] == NULL)
			{
				tri_backref[ref] = malloc(sizeof(uint32 *) * 7);
				write = tri_backref[ref];
				write[0] = 1;
				write[1] = i / 3;
			}
			else
			{
				write = tri_backref[ref];
				if(++write[0] % 6 == 0)
					tri_backref[ref] = realloc(tri_backref[ref], sizeof(uint32 *) * (write[0] + 7));
				write = tri_backref[ref];
				write[write[0]] = i / 3;
				if(write[0] > large)
					large = write[0];
			}
		}
		return large;
	}
	mesh->tri_backref = NULL;
	return 0;
}

uint32 quad_backref(SubMesh *mesh)
{
	uint32		ref, i, *write;
	uint32		**quad_backref, large = 1;

	if(mesh->quad_count > 0)
	{
		quad_backref = malloc((sizeof *quad_backref) * (mesh->vertex_count + 1));
		for(i = 0 ; i < mesh->vertex_count; i++)
			quad_backref[i] = NULL;
		mesh->quad_backref = quad_backref;
		for(i = 0 ; i < mesh->quad_count * 4; i++)
		{
			ref = mesh->quad_reference[i];
			if(quad_backref[ref] == NULL)
			{
				quad_backref[ref] = malloc(sizeof(uint32 *) * 5);
				write = quad_backref[ref];
				write[0] = 1;
				write[1] = i / 4;
			}
			else
			{
				write = quad_backref[ref];
				if(++write[0] % 4 == 0)
					quad_backref[ref] = realloc(quad_backref[ref], sizeof(uint32 *) * (write[0] + 5));
				write = quad_backref[ref];
				write[write[0]] = i / 4;
				if(write[0] > large)
					large = write[0];
			}
		}
		return large;
	}
	mesh->quad_backref = NULL;
	return 0;	
}


