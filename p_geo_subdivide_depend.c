#include <stdio.h>
#include <stdlib.h>


#include <math.h>
#include "p_geo_subdivide_internal.h"


void add_dependencys(SubVertexDepend *dep, uint32 ref, float waight)
{
	uint i;
	for(i = 0 ; i < dep->length ; i++)
	{
		if(dep->element[i].vertex_ref == ref)
		{
			dep->element[i].value += waight;
			dep->sum += waight;
			return;
		}
	}
	if(dep->alocated == dep->length)
	{

		dep->alocated += 2;
		dep->element = realloc(dep->element, (sizeof *dep->element) * dep->alocated);
		dep->element[dep->length].value = 0;
	}
	dep->element[dep->length].value = waight;
	dep->element[dep->length].vertex_ref = ref;
	dep->length++;
	dep->sum += waight;
}

void rop_next_vertex(SubMesh *geometry)
{
	uint32				i;
	float				sum = 0;
	SubVertexDepend		*dep;
	dep = &geometry->vertex_depend_temp[geometry->vertex_count++];
	for(i = 0; i < dep->length ; i++)
		sum += dep->element[i].value;
	for(i = 0; i < dep->length ; i++)
		dep->element[i].value /= sum;
}

void rop_sub_re_depend(SubMesh *geometry, SubMesh *last_geometry)
{
	int		i;
	uint32	j, k;
	float	value, sum;
	SubVertexDepend	 *dep, *last_dep, new_dep;

	for(i = geometry->vertex_count - 1; i >= 0; i--)
	{
		sum = 0;

		new_dep.length = 0;
		new_dep.alocated = 10;
		new_dep.sum = 0;
		new_dep.element = malloc((sizeof *new_dep.element) * 10);
		for(j = 0; j < 10; j++)
		{
			new_dep.element[j].value = 0;
			new_dep.element[j].vertex_ref = 0;
		}

		dep = &geometry->vertex_depend_temp[i];
		for(j = 0; j < dep->length ; j++)
		{
			value = dep->element[j].value;
			last_dep = &last_geometry->vertex_depend_temp[dep->element[j].vertex_ref];
			if(value > 0.0001)
			{
				for(k = 0; k < last_dep->length ; k++)
				{
					if(last_dep->element[k].value > 0.0001)
					{
						add_dependencys(&new_dep, last_dep->element[k].vertex_ref, (value * last_dep->element[k].value));
						sum += (value * last_dep->element[k].value);
					}
				}
			}
		}

		free(dep->element);
		*dep = new_dep;
	}
}


