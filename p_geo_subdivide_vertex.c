#include <stdio.h>
#include <stdlib.h>


#include <math.h>
#include "p_geo_subdivide_internal.h"

extern void add_dependencys(SubVertexDepend *dep, uint32 ref, float waight);
extern void rop_next_vertex(SubMesh *geometry);

extern uint8 *get_tri_subdivide_table(uint	generations);
extern uint8 *get_quad_subdivide_table(uint	generations);


void add_crease_dependencys(SubCreaseDependElement *dep, uint dep_count, uint32 ref, float waight, float crease)
{
	uint i;
	for(i = 0 ; i < dep_count ; i++)
	{
		if(dep[i].vertex_ref == ref)
		{
			dep[i].value += waight;
			dep[i].crease = crease;
			dep[i].edge_counts++;
			return;
		}
		if(dep[i].vertex_ref == -1)
		{
			dep[i].vertex_ref = ref;
			dep[i].crease = crease;		
			dep[i].value = waight;
			dep[i].edge_counts = 1;
			return;
		}
	}
}

float *create_vertex_crease_data(SubMesh *geometry, uint *cv_creas, uint extra_count)
{
	uint		  tri_count, quad_count, count;
	uint32 	  *read_tri = NULL, *read_quad = NULL, i, j ,k = 0, middle, ref1, ref2;
	float		  *output, mid_crease;
	SubCreaseDependElement *dep;

	if(geometry->tri_crease == NULL && geometry->quad_crease == NULL)
		return NULL;

	dep = malloc((sizeof *dep) * (extra_count * 4 + 2));

	output = malloc((sizeof *output) * ((geometry->tri_count * 16 + geometry->quad_count * 18) + geometry->vertex_count));
	for(i = 0 ; i < geometry->vertex_count; i++)
	{
		tri_count = 0;
		quad_count = 0;

		if(geometry->tri_backref != NULL)
		{
			read_tri = geometry->tri_backref[i];
			if(read_tri != NULL)
				tri_count = read_tri[0];
		}
		if(geometry->quad_backref != NULL)
		{
			read_quad = geometry->quad_backref[i];
			if(read_quad != NULL)
				quad_count = read_quad[0];
		}

		count = (tri_count * 2 + quad_count * 3 + 1);

/*		for(j = 0; j < count + 2; j++)
			dep[j].vertex_ref = -1;*/
		for(j = 0; j < extra_count * 4 + 2; j++)
			dep[j].vertex_ref = -1;
		
		
		for(j = 1; j <= tri_count; j++)
		{
			if(geometry->tri_reference[(read_tri[j] * 3)] == i)
			{
				add_crease_dependencys(dep, count, geometry->tri_reference[(read_tri[j] * 3)], 0.83333333333, 1);
				add_crease_dependencys(dep, count, geometry->tri_reference[(read_tri[j] * 3) + 1], 0.83333333333, geometry->tri_crease[read_tri[j] * 3]);
				add_crease_dependencys(dep, count, geometry->tri_reference[(read_tri[j] * 3) + 2], 0.83333333333, geometry->tri_crease[read_tri[j] * 3 + 2]);
			}
			else if(geometry->tri_reference[(read_tri[j] * 3) + 1] == i)
			{
				add_crease_dependencys(dep, count, geometry->tri_reference[(read_tri[j] * 3)], 0.83333333333, geometry->tri_crease[read_tri[j] * 3]);
				add_crease_dependencys(dep, count, geometry->tri_reference[(read_tri[j] * 3) + 1], 0.83333333333, 1);
				add_crease_dependencys(dep, count, geometry->tri_reference[(read_tri[j] * 3) + 2], 0.83333333333, geometry->tri_crease[read_tri[j] * 3 + 1]);
			}
			else if(geometry->tri_reference[(read_tri[j] * 3) + 2] == i)
			{
				add_crease_dependencys(dep, count, geometry->tri_reference[(read_tri[j] * 3)], 0.83333333333, geometry->tri_crease[read_tri[j] * 3 + 2]);
				add_crease_dependencys(dep, count, geometry->tri_reference[(read_tri[j] * 3) + 1], 0.83333333333, geometry->tri_crease[read_tri[j] * 3 + 1]);
				add_crease_dependencys(dep, count, geometry->tri_reference[(read_tri[j] * 3) + 2], 0.83333333333, 1);
			}
		}
		
		for(j = 1; j <= quad_count; j++)
		{	
			if(geometry->quad_reference[read_quad[j] * 4] == i)
			{
				add_crease_dependencys(dep, count, geometry->quad_reference[(read_quad[j] * 4)], 0.25, 1);
				add_crease_dependencys(dep, count, geometry->quad_reference[(read_quad[j] * 4) + 1], 0.75, geometry->quad_crease[read_quad[j] * 4]);
				add_crease_dependencys(dep, count, geometry->quad_reference[(read_quad[j] * 4) + 2], 0.25, 1);
				add_crease_dependencys(dep, count, geometry->quad_reference[(read_quad[j] * 4) + 3], 0.75, geometry->quad_crease[read_quad[j] * 4 + 3]);
			}
			else if(geometry->quad_reference[(read_quad[j] * 4) + 1] == i)
			{

				add_crease_dependencys(dep, count, geometry->quad_reference[(read_quad[j] * 4)], 0.75, geometry->quad_crease[read_quad[j] * 4]);
				add_crease_dependencys(dep, count, geometry->quad_reference[(read_quad[j] * 4) + 1], 0.25, 1);
				add_crease_dependencys(dep, count, geometry->quad_reference[(read_quad[j] * 4) + 2], 0.75, geometry->quad_crease[read_quad[j] * 4 + 1]);
				add_crease_dependencys(dep, count, geometry->quad_reference[(read_quad[j] * 4) + 3], 0.25, 1);
			}
			else if(geometry->quad_reference[(read_quad[j] * 4) + 2] == i)
			{

				add_crease_dependencys(dep, count, geometry->quad_reference[(read_quad[j] * 4)], 0.25, 1);
				add_crease_dependencys(dep, count, geometry->quad_reference[(read_quad[j] * 4) + 1], 0.75, geometry->quad_crease[read_quad[j] * 4 + 1]);
				add_crease_dependencys(dep, count, geometry->quad_reference[(read_quad[j] * 4) + 2], 0.25, 1);
				add_crease_dependencys(dep, count, geometry->quad_reference[(read_quad[j] * 4) + 3], 0.75, geometry->quad_crease[read_quad[j] * 4 + 2]);
			}
			else if(geometry->quad_reference[(read_quad[j] * 4) + 3] == i)
			{
				add_crease_dependencys(dep, count, geometry->quad_reference[(read_quad[j] * 4)], 0.75, geometry->quad_crease[read_quad[j] * 4 + 3]);
				add_crease_dependencys(dep, count, geometry->quad_reference[(read_quad[j] * 4) + 1], 0.25, 1);
				add_crease_dependencys(dep, count, geometry->quad_reference[(read_quad[j] * 4) + 2], 0.75, geometry->quad_crease[read_quad[j] * 4 + 2]);
				add_crease_dependencys(dep, count, geometry->quad_reference[(read_quad[j] * 4) + 3], 0.25, 1);
			}
		}

		if(count == 1)
		{
//			add_crease_dependencys(dep, 2, i, 1, 1.0);
		//	for(j = 0; dep[j].vertex_ref != -1; j++)
			{
		/*		if(dep[j].vertex_ref == i)
					output[k++] = 1;
				else*/
					output[k++] = 0;
			}
		}
		else
		{
			for(j = 0 ; dep[j].vertex_ref != -1 && (dep[j].edge_counts != 1 || dep[j].value < 0.26); j++);
			
			if(dep[j].vertex_ref != -1 && tri_count + quad_count > 1)
			{
				float temp_crease = 1; 
				for(j = 0; dep[j].vertex_ref != -1; j++)
					if(dep[j].edge_counts == 2 && dep[j].crease < temp_crease)
						temp_crease = dep[j].crease;
				for(j = 0; dep[j].vertex_ref != -1; j++)
				{
					if((dep[j].edge_counts == 1 && dep[j].value > 0.26) || dep[j].vertex_ref == i)
						dep[j].crease = 0;
					else
						dep[j].crease = temp_crease;
				}
			}
			for(j = 0; dep[j].vertex_ref != i; j++);
			middle = j;
	
			dep[middle].value += (tri_count + quad_count - 2) * (tri_count + quad_count);
		
			if(j == 0)
				ref1 = 1;
			else
				ref1 = 0;
			for(j = 1; dep[j].vertex_ref != -1 ;j++)
				if(dep[ref1].crease > dep[j].crease && j != middle)
					ref1 = j;

 			ref2 = j - 1;
			for(j = 0; dep[j].vertex_ref != -1 ;j++)
				if(dep[ref2].crease > dep[j].crease && j != ref1 && j != middle)
					ref2 = j;
				
			mid_crease = 1;
			for(j = 0; dep[j].vertex_ref != -1 ;j++)
				if(j != ref1 && j != ref2 && j != middle)
					mid_crease *= dep[j].crease; 
			for(j = 0; dep[j].vertex_ref != -1; j++)
			{
				if(j != middle)
					output[k++] = (dep[j].value - dep[j].edge_counts * 0.25) * (1 - (( 1 - dep[ref2].crease) - (1 - dep[j].crease))) * mid_crease + (dep[j].edge_counts * 0.25) * dep[ref2].crease;
				else
					output[k++] = ((1 - dep[ref2].crease) * 4) + ((dep[ref2].crease) * (tri_count + quad_count - 2) * (tri_count + quad_count)) + (dep[ref2].crease * 0.25 * quad_count) + (dep[ref2].crease * 0.33333333333333333 *  tri_count);
			}
		}
	}
	free(dep);
	return output;
}

/* ads a depending value to a vertex*/


/*takes care of vertex points*/
void rop_cv_vertexref(SubMesh *geometry, SubMesh *last_geometry, uint cv_count, float *cv_crease)
{
	uint		       tri_count, quad_count, count, crease_count = 0;
	uint32		  *read_tri = NULL, *read_quad = NULL, i, j;
	SubVertexDepend *dep;
	for(i = 0 ; i < cv_count; i++)
	{
		tri_count = 0;
		quad_count = 0;
		if(last_geometry->tri_backref != NULL)
		{
			read_tri = last_geometry->tri_backref[i];
			if(read_tri != NULL)
				tri_count = read_tri[0];
		}
		if(last_geometry->quad_backref != NULL)
		{
			read_quad = last_geometry->quad_backref[i];
			if(read_quad != NULL)
				quad_count = read_quad[0];
		}
		dep = &geometry->vertex_depend_temp[geometry->vertex_count];
		dep->length = 0;
		dep->alocated = 4;
		dep->element = malloc((sizeof *dep->element) * 4);
		for(j = 0; j < 4; j++)
		{
			dep->element[j].value = 0;
			dep->element[j].vertex_ref = 0;
		}
		for(j = 1; j <= tri_count; j++)
		{
			add_dependencys(dep, last_geometry->tri_reference[(read_tri[j] * 3)], 1);
			add_dependencys(dep, last_geometry->tri_reference[(read_tri[j] * 3) + 1], 1);
			add_dependencys(dep, last_geometry->tri_reference[(read_tri[j] * 3) + 2], 1);
		}
		for(j = 1; j <= quad_count; j++)
		{
			add_dependencys(dep, last_geometry->quad_reference[(read_quad[j] * 4)], 1);
			add_dependencys(dep, last_geometry->quad_reference[(read_quad[j] * 4) + 1], 1);
			add_dependencys(dep, last_geometry->quad_reference[(read_quad[j] * 4) + 2], 1);
			add_dependencys(dep, last_geometry->quad_reference[(read_quad[j] * 4) + 3], 1);
		}
		count = tri_count + quad_count;
		if(count == 0)
		{	if(cv_crease != NULL)
				add_dependencys(dep, i, cv_crease[crease_count++]);
			else
				add_dependencys(dep, i,1);
		}else
		if(cv_crease != NULL)
		{
			for(j = 0; j < dep->length; j++)
				dep->element[j].value = cv_crease[crease_count++];
		}
		else
			add_dependencys(dep, i, (tri_count + quad_count) * (tri_count + quad_count - 2.5));

		if(count == 1)
			for(j = 0; j < dep->length; j++)
				if(dep->element[j].vertex_ref != i)
					dep->element[j].value = 0;

		rop_next_vertex(geometry);
	}	
}

void rop_vertexref(SubMesh *geometry, SubMesh *last_geometry, uint cv_count, uint level)
{
	uint		       tri_count, quad_count, count, poly_count;
	uint32		  *read_tri = NULL, *read_quad = NULL, i, j;
	float			crease;
	uint8		      *tri_table, *quad_table;
	SubVertexDepend *dep;

	tri_table = get_tri_subdivide_table(level);
	quad_table = get_quad_subdivide_table(level);

	poly_count = 1;
	for(i = 1; i < level; i++)
		poly_count *= 4;

	for(i = cv_count ; i < last_geometry->vertex_count; i++)
	{
		tri_count = 0;
		quad_count = 0;
		if(last_geometry->tri_backref != NULL)
		{
			read_tri = last_geometry->tri_backref[i];
			if(read_tri != NULL)
				tri_count = read_tri[0];
		}

		if(last_geometry->quad_backref != NULL)
		{
			read_quad = last_geometry->quad_backref[i];
			if(read_quad != NULL)
				quad_count = read_quad[0];
		}

		dep = &geometry->vertex_depend_temp[geometry->vertex_count];

		dep->length = 0;
		dep->alocated = 4;
		dep->sum = 0;

		dep->element = malloc((sizeof *dep->element) * 4);
		dep->element[0].value = 0;
		dep->element[0].vertex_ref = 0;
		dep->element[1].value = 0;
		dep->element[1].vertex_ref = 0;
		dep->element[2].value = 0;
		dep->element[2].vertex_ref = 0;
		dep->element[3].value = 0;
		dep->element[3].vertex_ref = 0;
		for(j = 1; j <= tri_count; j++)
		{
			add_dependencys(dep, last_geometry->tri_reference[(read_tri[j] * 3)], 0.83333333333);
			add_dependencys(dep, last_geometry->tri_reference[(read_tri[j] * 3) + 1], 0.83333333333);
			add_dependencys(dep, last_geometry->tri_reference[(read_tri[j] * 3) + 2], 0.83333333333);
		}

		for(j = 1; j <= quad_count; j++)
		{
			if(last_geometry->quad_reference[read_quad[j] * 4] == i)
			{
				add_dependencys(dep, last_geometry->quad_reference[(read_quad[j] * 4)], 0.75);
				add_dependencys(dep, last_geometry->quad_reference[(read_quad[j] * 4) + 1], 0.75);
				add_dependencys(dep, last_geometry->quad_reference[(read_quad[j] * 4) + 2], 0.25);
				add_dependencys(dep, last_geometry->quad_reference[(read_quad[j] * 4) + 3], 0.75);
			}
			else if(last_geometry->quad_reference[(read_quad[j] * 4) + 1] == i)
			{
				add_dependencys(dep, last_geometry->quad_reference[(read_quad[j] * 4)], 0.75);
				add_dependencys(dep, last_geometry->quad_reference[(read_quad[j] * 4) + 1], 0.75);
				add_dependencys(dep, last_geometry->quad_reference[(read_quad[j] * 4) + 2], 0.75);
				add_dependencys(dep, last_geometry->quad_reference[(read_quad[j] * 4) + 3], 0.25);
			}
			else if(last_geometry->quad_reference[(read_quad[j] * 4) + 2] == i)
			{
				add_dependencys(dep, last_geometry->quad_reference[(read_quad[j] * 4)], 0.25);
				add_dependencys(dep, last_geometry->quad_reference[(read_quad[j] * 4) + 1], 0.75);
				add_dependencys(dep, last_geometry->quad_reference[(read_quad[j] * 4) + 2], 0.75);
				add_dependencys(dep, last_geometry->quad_reference[(read_quad[j] * 4) + 3], 0.75);
			}
			else if(last_geometry->quad_reference[(read_quad[j] * 4) + 3] == i)
			{
				add_dependencys(dep, last_geometry->quad_reference[(read_quad[j] * 4)], 0.75);
				add_dependencys(dep, last_geometry->quad_reference[(read_quad[j] * 4) + 1], 0.25);
				add_dependencys(dep, last_geometry->quad_reference[(read_quad[j] * 4) + 2], 0.75);
				add_dependencys(dep, last_geometry->quad_reference[(read_quad[j] * 4) + 3], 0.75);
			}
		}
		count = tri_count + quad_count;

		add_dependencys(dep, i, count * (count - 2.5));
		dep->sum = count * count;

		if(quad_count + tri_count < 4 || (quad_count / 2) + (tri_count / 3) > 2)
		{
			if(quad_count != 0)
			{
				if(dep->element[2].value > 1)
				{
					if(dep->element[4].value > 0.5)
					{
						dep->element[2].vertex_ref = dep->element[1].vertex_ref;
						dep->element[1].vertex_ref = dep->element[4].vertex_ref;

					}
					else
					{
						dep->element[1].vertex_ref = dep->element[5].vertex_ref;
						dep->element[2].vertex_ref = dep->element[3].vertex_ref;
					}
				}
				else
				{
					if(dep->element[0].value > 1)
					{
						dep->element[0].vertex_ref = dep->element[2].vertex_ref;
						dep->element[1].vertex_ref = dep->element[5].vertex_ref;
						dep->element[2].vertex_ref = dep->element[3].vertex_ref;
					}
					else
					{
						dep->element[0].vertex_ref = dep->element[4].vertex_ref;		
					}
				}
			}
			else 
			{
				if(dep->element[2].value < dep->element[1].value)
				{
					if(dep->element[0].vertex_ref > dep->element[1].vertex_ref)
					{
						dep->element[3].vertex_ref = dep->element[2].vertex_ref;
						dep->element[2].vertex_ref = dep->element[1].vertex_ref;
						dep->element[0].vertex_ref = dep->element[3].vertex_ref;

					}
					else
						dep->element[2].vertex_ref = dep->element[1].vertex_ref;
				}
				dep->element[1].vertex_ref = dep->element[4].vertex_ref;
			}	
			dep->element[0].value = 1;
			dep->element[1].value = 1;
			dep->element[2].value = 6;
			dep->length = 3;
		}
		else if(last_geometry->tri_crease != NULL || last_geometry->quad_crease != NULL)
		{
			if(quad_count != 4 || read_quad[1] / poly_count != read_quad[3] / poly_count)
			{
				if(tri_count != 0)
				{
					if(tri_count == 3 || read_tri[1] / (poly_count) != read_tri[5] / (poly_count))
					{
					/*	printf("i = %i compare to %i %i %i\n", i, last_geometry->tri_reference[(read_tri[3] * 3)]
							, last_geometry->tri_reference[(read_tri[3] * 3) + 1]
							, last_geometry->tri_reference[(read_tri[3] * 3) + 2]);*/
						if(last_geometry->tri_reference[(read_tri[3] * 3)] == i)
						{
							/* first edge */
							crease = last_geometry->tri_crease[(read_tri[1] / poly_count) * 3];
							dep->element[1].value = dep->element[1].value * crease + (dep->element[0].value * 8) * (1 - crease);
							dep->element[2].value *= crease;
							dep->element[3].value *= crease;
							for(j = 5; j < dep->length; j++)
								dep->element[j].value *= crease;

						}
						else if(last_geometry->tri_reference[(read_tri[3] * 3) + 2] == i)
						{
							/* second edge */
							crease = last_geometry->tri_crease[(read_tri[1] / poly_count) * 3 + 1];
							dep->element[0].value *= crease;
							dep->element[1].value = dep->element[1].value * crease + (dep->element[2].value * 8) * (1 - crease);
							dep->element[3].value *= crease;
							for(j = 5; j < dep->length; j++)
								dep->element[j].value *= crease;
						}
						else
						{
							/* third edge */								
							crease = last_geometry->tri_crease[(read_tri[1] / poly_count) * 3 + 2];
							dep->element[1].value *= crease;
							dep->element[2].value = dep->element[2].value * crease + (dep->element[0].value * 8) * (1 - crease);
							dep->element[3].value *= crease;
							for(j = 5; j < dep->length; j++)
								dep->element[j].value *= crease;
						}
					}	 
				}
				else
				{
					crease = 0;

					if(dep->element[1].value > dep->element[3].value || dep->element[2].value > dep->element[3].value)
					{
						if(dep->element[1].value > dep->element[2].value) /* edge 0 */
						{
							crease = last_geometry->quad_crease[(read_quad[1] / poly_count) * 4];
							dep->element[2].value *= crease;
							dep->element[3].value *= crease;
							for(j = 5; j < dep->length; j++)
								dep->element[j].value *= crease;
						}
						else /* edge 1 */
						{
							crease = last_geometry->quad_crease[(read_quad[1] / poly_count) * 4 + 1];
							dep->element[0].value *= crease;
							dep->element[3].value *= crease;
							for(j = 5; j < dep->length; j++)
								dep->element[j].value *= crease;
						}
					}
					else if(read_quad[1] % 4 != 0 && read_quad[2] % 4 != 0)	 /* edge 2 */
					{
						crease = last_geometry->quad_crease[(read_quad[1] / poly_count) * 4 + 2];
						dep->element[0].value *= crease;
						dep->element[1].value *= crease;
						dep->element[4].value *= crease;
						for(j = 6; j < dep->length; j++)
							dep->element[j].value *= crease;
						
					}
					else  /* edge 3 */
					{
						crease = last_geometry->quad_crease[(read_quad[1] / poly_count) * 4 + 3];
						dep->element[1].value *= crease;
						dep->element[2].value *= crease;
						dep->element[4].value *= crease;
						for(j = 6; j < dep->length; j++)
							dep->element[j].value *= crease;
					}
				}
			}
		}
		rop_next_vertex(geometry);
	}	
	for(i = 0; i < last_geometry->tri_count; i++)
	{
			geometry->tri_reference[(i * 12) + 0] = last_geometry->tri_reference[(i * 3)];
			geometry->tri_reference[(i * 12) + 10] = last_geometry->tri_reference[(i * 3) + 1];
			geometry->tri_reference[(i * 12) + 5] = last_geometry->tri_reference[(i * 3) + 2];
	}

	for(i = 0; i < last_geometry->quad_count; i++)
	{
		geometry->quad_reference[(i * 16) + 0] = last_geometry->quad_reference[i * 4];
		geometry->quad_reference[(i * 16) + 5] = last_geometry->quad_reference[(i * 4) + 1];
		geometry->quad_reference[(i * 16) + 10] = last_geometry->quad_reference[(i * 4) + 2];
		geometry->quad_reference[(i * 16) + 15] = last_geometry->quad_reference[(i * 4) + 3];
	}
}

