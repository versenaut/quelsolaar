#include <stdio.h>
#include <stdlib.h>


#include <math.h>
#include "p_geo_subdivide_internal.h"

extern void add_dependencys(SubVertexDepend *dep, uint32 ref, float waight);
extern void rop_next_vertex(SubMesh *geometry);

/*takes care of face points on quads*/

void rop_faceref(SubMesh *geometry, SubMesh *last_geometry)
{
	uint32	quad_count, max_uint;
	uint i;
	SubVertexDepend	*dep;
	max_uint =  0;
	max_uint--;

	quad_count = last_geometry->quad_count;

	for(i = 0; i < quad_count; i++) 
	{
		if(geometry->quad_reference[(i * 16) + 2] == -1)
		{
			dep = &geometry->vertex_depend_temp[geometry->vertex_count]; /* setting quad middle and creating nev vertex */

			dep->length = 4;
			dep->alocated = 4;
			dep->sum = 4;

			dep->element = malloc((sizeof *dep->element) * 4);
			dep->element[0].value = 1;
			dep->element[0].vertex_ref = last_geometry->quad_reference[(i * 4)];
			dep->element[1].value = 1;
			dep->element[1].vertex_ref = last_geometry->quad_reference[(i * 4) + 1];
			dep->element[2].value = 1;
			dep->element[2].vertex_ref = last_geometry->quad_reference[(i * 4) + 2];
			dep->element[3].value = 1;
			dep->element[3].vertex_ref = last_geometry->quad_reference[(i * 4) + 3];

			geometry->quad_reference[(i * 16) + 2] = geometry->vertex_count;
			geometry->quad_reference[(i * 16) + 7] = geometry->vertex_count;        
			geometry->quad_reference[(i * 16) + 8] = geometry->vertex_count;        
			geometry->quad_reference[(i * 16) + 13] = geometry->vertex_count;	 
			rop_next_vertex(geometry);
		}
	}
}
