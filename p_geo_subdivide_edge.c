#include <stdio.h>
#include <stdlib.h>


#include <math.h>
#include "p_geo_subdivide_internal.h"

uint8 *global_tri_edge_table = NULL;
uint8 *global_quad_edge_table = NULL;
uint global_subdivide_generartion_count[16];


extern void add_dependencys(SubVertexDepend *dep, uint32 ref, float waight);
extern void rop_next_vertex(SubMesh *geometry);

uint8 *get_tri_subdivide_table(uint	generations)
{
	uint i, j, index;
	uint length, last_length;
	uint static generations_generated = 0;

	if(generations <= generations_generated)
	{
		length = 0;
		for(i = 0; i < generations; i++)
			length += global_subdivide_generartion_count[i];
		return &global_tri_edge_table[length];
	}

	generations_generated = 1;
	global_subdivide_generartion_count[0] = 1;
        
	for(i = generations_generated; i <= generations; i++)
		global_subdivide_generartion_count[i] = global_subdivide_generartion_count[i - 1] * 4;

	length = 0;
	for(i = 0; i < generations; i++)
		length += global_subdivide_generartion_count[i];

	if(generations_generated == 0)
		global_tri_edge_table = malloc(sizeof(uint8) * 3 * length);
	else
		global_tri_edge_table = realloc(global_tri_edge_table, sizeof(uint8) * 3 * length);

	global_tri_edge_table[0] = 0;
	global_tri_edge_table[1] = 1;
	global_tri_edge_table[2] = 2;
        
	last_length = 0;
	for(i = 0; i < generations_generated; i++)
		last_length += global_subdivide_generartion_count[i];

	length = last_length;

	for(i = generations_generated; i < generations; i++)
	{
		length += global_subdivide_generartion_count[i];
		for(j = 0; j < global_subdivide_generartion_count[i]; i++)
		{
			index = (12 * j) + length;
			global_tri_edge_table[index] = global_tri_edge_table[3 * j + last_length];
			global_tri_edge_table[index + 1] = 255;
			global_tri_edge_table[index + 2] = global_tri_edge_table[3 * j + 2 + last_length];
			global_tri_edge_table[index + 3] = 255;
			global_tri_edge_table[index + 4] = global_tri_edge_table[3 * j + 1 + last_length];
			global_tri_edge_table[index + 5] = global_tri_edge_table[3 * j + 2 + last_length];
			global_tri_edge_table[index + 6] = 255;
			global_tri_edge_table[index + 7] = 255;
			global_tri_edge_table[index + 8] = 255;
			global_tri_edge_table[index + 9] = global_tri_edge_table[3 * j + last_length];
			global_tri_edge_table[index + 10] = global_tri_edge_table[3 * j + 1 + last_length];
			global_tri_edge_table[index + 11] = 255;
		}
		last_length = length;
	}
	generations_generated = generations; 
	return &global_tri_edge_table[length];
}

uint8 *get_quad_subdivide_table(uint	generations)
{
	uint i, j, index;
	uint length, last_length;
	uint static generations_generated = 0;

	if(generations < generations_generated)
	{
		length = 0;
		for(i = 0; i < generations; i++)
			length += global_subdivide_generartion_count[i];
		return &global_quad_edge_table[length];
	}


	generations_generated = 1;
	global_subdivide_generartion_count[0] = 1;
        
	for(i = generations_generated; i <= generations; i++)
		global_subdivide_generartion_count[i] = global_subdivide_generartion_count[i - 1] * 4;

	length = 0;
	for(i = 0; i < generations; i++)
		length += global_subdivide_generartion_count[i];

	if(generations_generated == 0)
		global_quad_edge_table = malloc(sizeof(uint8) * 4 * length);
	else
		global_quad_edge_table = realloc(global_quad_edge_table, sizeof(uint8) * 4 * length);
        
	global_quad_edge_table[0] = 0;
	global_quad_edge_table[1] = 1;
	global_quad_edge_table[2] = 2;
	global_quad_edge_table[3] = 3;

	last_length = 0;
	for(i = 0; i < generations_generated; i++)
		last_length += global_subdivide_generartion_count[i];

	length = last_length;

	for(i = generations_generated; i < generations; i++)
	{
		length += global_subdivide_generartion_count[i];
		for(j = 0; j < global_subdivide_generartion_count[i]; i++)
		{
			index = (16 * j) + length;
			global_quad_edge_table[index] = global_quad_edge_table[4 * j + last_length]; 
			global_quad_edge_table[index + 1] = 255;
			global_quad_edge_table[index + 2] = 255;
			global_quad_edge_table[index + 3] = global_quad_edge_table[4 * j + 3 + last_length];
			global_quad_edge_table[index + 4] = global_quad_edge_table[4 * j + last_length];
			global_quad_edge_table[index + 5] = global_quad_edge_table[4 * j + 1 + last_length];
			global_quad_edge_table[index + 6] = 255;
			global_quad_edge_table[index + 7] = 255;
			global_quad_edge_table[index + 8] = 255;
			global_quad_edge_table[index + 9] = global_quad_edge_table[4 * j + 1 + last_length];
			global_quad_edge_table[index + 10] = global_quad_edge_table[4 * j + 2 + last_length];
			global_quad_edge_table[index + 11] = 255;
			global_quad_edge_table[index + 12] = 255;
			global_quad_edge_table[index + 13] = 255;
			global_quad_edge_table[index + 14] = global_quad_edge_table[4 * j + 2 + last_length];
			global_quad_edge_table[index + 15] = global_quad_edge_table[4 * j + 3 + last_length];
		}
		last_length = length;
	}
	generations_generated = generations; 
	return &global_quad_edge_table[length];
}

/* Creates a dependency on a tri edge. */
void add_tri_dependency(uint32 polygon, uint32 vertex1, uint32 vertex2, SubMesh *geometry, SubMesh *last_geometry)
{
	uint32			 *cv_polygon, vertex_count;
	SubVertexDepend *dep;

	cv_polygon = &geometry->tri_reference[(polygon * 12)];
	vertex_count = geometry->vertex_count;

	if(((last_geometry->tri_reference[polygon * 3] == vertex1) && (last_geometry->tri_reference[(polygon * 3) + 1] == vertex2)) ||
		((last_geometry->tri_reference[polygon * 3] == vertex2) && (last_geometry->tri_reference[(polygon * 3) + 1] == vertex1)))
	{
		cv_polygon[1] = vertex_count;
		cv_polygon[8] = vertex_count;
		cv_polygon[9] = vertex_count;	      
	}
	else
	if(((last_geometry->tri_reference[polygon * 3] == vertex1) && (last_geometry->tri_reference[(polygon * 3) + 2] == vertex2)) ||
		((last_geometry->tri_reference[polygon * 3] == vertex2) && (last_geometry->tri_reference[(polygon * 3) + 2] == vertex1)))
	{
		cv_polygon[2] = vertex_count;
		cv_polygon[3] = vertex_count;
		cv_polygon[7] = vertex_count;		     
	}
	else
	{
		cv_polygon[4] = vertex_count;
		cv_polygon[6] = vertex_count;
		cv_polygon[11] = vertex_count;		     
	}

	dep = &geometry->vertex_depend_temp[geometry->vertex_count];
	add_dependencys(dep, last_geometry->tri_reference[polygon * 3], 1);
	add_dependencys(dep, last_geometry->tri_reference[(polygon * 3) + 1], 1);
	add_dependencys(dep, last_geometry->tri_reference[(polygon * 3) + 2], 1);

}
/*creates a dependency on a quad edge */
void add_quad_dependency(uint32 polygon, uint32 vertex1, uint32 vertex2, SubMesh *geometry, SubMesh *last_geometry)
{
	uint32			 *cv_polygon, vertex_count;
	SubVertexDepend *dep;

	cv_polygon = &geometry->quad_reference[(polygon * 16)];
	vertex_count = geometry->vertex_count;

	if(((last_geometry->quad_reference[polygon * 4] == vertex1) && (last_geometry->quad_reference[(polygon * 4) + 1] == vertex2)) ||
		((last_geometry->quad_reference[polygon * 4] == vertex2) && (last_geometry->quad_reference[(polygon * 4) + 1] == vertex1)))
	{
		cv_polygon[1] = vertex_count;
		cv_polygon[4] = vertex_count;		     
	}
	else if(((last_geometry->quad_reference[polygon * 4] == vertex1) && (last_geometry->quad_reference[(polygon * 4) + 3] == vertex2)) ||
		((last_geometry->quad_reference[polygon * 4] == vertex2) && (last_geometry->quad_reference[(polygon * 4) + 3] == vertex1)))
	{
		cv_polygon[3] = vertex_count;
		cv_polygon[12] = vertex_count;		      
	}
	else if(((last_geometry->quad_reference[(polygon * 4) + 1] == vertex1) && (last_geometry->quad_reference[(polygon * 4) + 2] == vertex2)) ||
		((last_geometry->quad_reference[(polygon * 4) + 1] == vertex2) && (last_geometry->quad_reference[(polygon * 4) + 2] == vertex1)))
	{
		cv_polygon[6] = vertex_count;
		cv_polygon[9] = vertex_count;			     
	}
	else if(((last_geometry->quad_reference[(polygon * 4) + 2] == vertex1) && (last_geometry->quad_reference[(polygon * 4) + 3] == vertex2)) ||
		((last_geometry->quad_reference[(polygon * 4) + 2] == vertex2) && (last_geometry->quad_reference[(polygon * 4) + 3] == vertex1)))
	{
		cv_polygon[11] = vertex_count;
		cv_polygon[14] = vertex_count;			      
	}
	else
	{
		cv_polygon[2] = vertex_count;
		cv_polygon[7] = vertex_count;
		cv_polygon[8] = vertex_count;
		cv_polygon[13] = vertex_count;	
		return;	      
	}


	dep = &geometry->vertex_depend_temp[geometry->vertex_count];
	add_dependencys(dep, last_geometry->quad_reference[polygon * 4], 0.75);
	add_dependencys(dep, last_geometry->quad_reference[(polygon * 4) + 1], 0.75);
	add_dependencys(dep, last_geometry->quad_reference[(polygon * 4) + 2], 0.75);
	add_dependencys(dep, last_geometry->quad_reference[(polygon * 4) + 3], 0.75);
}

/* finds polygons sharing a edge, tris to tris and tris to quads*/
void polygon_edgetest(uint32 vertex1, uint32 vertex2, SubMesh *geometry, SubMesh *last_geometry, float crease)
{
	uint			i, j, count = 0;
	uint32			 *polylist1, *polylist2;
	SubVertexDepend *dep;

	dep = &geometry->vertex_depend_temp[geometry->vertex_count];

	dep->length = 2;
	dep->alocated = 9;
	dep->sum = 6;

	dep->element = malloc((sizeof *dep->element) * 9);
	dep->element[0].value = 3;
	dep->element[0].vertex_ref = vertex1;
	dep->element[1].value = 3;
	dep->element[1].vertex_ref = vertex2;
	for(i = 2; i < 9; i++)
	{
		dep->element[i].value = 0;
		dep->element[i].vertex_ref = 0;
	}
	if(last_geometry->tri_backref != NULL)
	{
		polylist1 = last_geometry->tri_backref[vertex1];
		polylist2 = last_geometry->tri_backref[vertex2];
		if(polylist1 != NULL && polylist2 != NULL)
		{
			for(i = 1; i <= polylist1[0]; i++)
			{
				for(j = 1; j <= polylist2[0]; j++)
				{
					if(polylist1[i] == polylist2[j])
					{
						add_tri_dependency(polylist2[j], vertex1, vertex2, geometry, last_geometry);
						count++;
					}
				}
			}
		}
	}
	if(last_geometry->quad_backref != NULL)
	{
		polylist1 = last_geometry->quad_backref[vertex1];
		polylist2 = last_geometry->quad_backref[vertex2];

		if(polylist1 != NULL && polylist2 != NULL)
		{	
			for(i = 1; i <= polylist1[0]; i++)
			{	
				for(j = 1; j <= polylist2[0]; j++)
				{
					if(polylist1[i] == polylist2[j])
					{
						add_quad_dependency(polylist2[j], vertex1, vertex2, geometry, last_geometry);
						count++;
					}
				}
			}
		}
	}
	if(count != 2)
		crease = 0;
	if(crease < 0.999)
		for(i = 2; i < dep->length; i++)
			dep->element[i].value *= crease;
	rop_next_vertex(geometry);
}

/*gose threw alledges and cheks for edges whitout vertexes*/
void rop_edgeref(SubMesh *geometry, SubMesh *last_geometry, uint level)
{
	uint32	i, max_uint, polygon;
	uint8	*corner_table, ref;
        
	max_uint = 0;
	max_uint--;

	if(last_geometry->tri_crease != NULL)
	{
		corner_table = get_tri_subdivide_table(level);
		for(i = 0; i < last_geometry->tri_count; i++) 
		{
			polygon = (i / global_subdivide_generartion_count[level]);
			if(geometry->tri_reference[(i * 12) + 1] == max_uint)
			{
				ref = corner_table[(i % global_subdivide_generartion_count[level - 1]) * 3];
				if(ref == 255)
					polygon_edgetest(last_geometry->tri_reference[(i * 3)], last_geometry->tri_reference[(i * 3) + 1], geometry, last_geometry, 1);
				else
					polygon_edgetest(last_geometry->tri_reference[(i * 3)], last_geometry->tri_reference[(i * 3) + 1], geometry, last_geometry, last_geometry->tri_crease[polygon * 3]);
			}
			if(geometry->tri_reference[(i * 12) + 4] == max_uint)
			{			 
				ref = corner_table[(i % global_subdivide_generartion_count[level - 1]) * 3 + 1];
				if(ref == 255)
					polygon_edgetest(last_geometry->tri_reference[(i * 3) + 1], last_geometry->tri_reference[(i * 3) + 2], geometry, last_geometry, 1);
				else
					polygon_edgetest(last_geometry->tri_reference[(i * 3) + 1],	last_geometry->tri_reference[(i * 3) + 2], geometry, last_geometry, last_geometry->tri_crease[polygon * 3 + 1]);			
			}
			if(geometry->tri_reference[(i * 12) + 2] == max_uint)
			{
				ref = corner_table[(i % global_subdivide_generartion_count[level - 1]) * 3 + 2];
				if(ref == 255)
					polygon_edgetest(last_geometry->tri_reference[(i * 3) + 2],	last_geometry->tri_reference[i * 3], geometry, last_geometry, 1);
				else
					polygon_edgetest(last_geometry->tri_reference[(i * 3) + 2],	last_geometry->tri_reference[i * 3], geometry, last_geometry, last_geometry->tri_crease[polygon * 3 + 2]);
			}
		}
	}
	else
	{
		for(i = 0; i < last_geometry->tri_count; i++) 
		{
			if(geometry->tri_reference[(i * 12) + 1] == max_uint)
				polygon_edgetest(last_geometry->tri_reference[(i * 3)],	last_geometry->tri_reference[(i * 3) + 1], geometry, last_geometry, 1);
			if(geometry->tri_reference[(i * 12) + 4] == max_uint)
				polygon_edgetest(last_geometry->tri_reference[(i * 3) + 1], last_geometry->tri_reference[(i * 3) + 2], geometry, last_geometry, 1);
			if(geometry->tri_reference[(i * 12) + 2] == max_uint)
				polygon_edgetest(last_geometry->tri_reference[(i * 3) + 2], last_geometry->tri_reference[(i * 3)], geometry, last_geometry, 1);
		}
	}

	if(last_geometry->quad_crease != NULL)
	{
		corner_table = get_quad_subdivide_table(level);
		for(i = 0; i < last_geometry->quad_count; i++) 
		{			 
			polygon = (i / global_subdivide_generartion_count[level - 1]);
			if(geometry->quad_reference[(i * 16) + 1] == max_uint)
			{
				ref = corner_table[(i % global_subdivide_generartion_count[level - 1]) * 4];
				if(ref == 255)
					polygon_edgetest(last_geometry->quad_reference[(i * 4)], last_geometry->quad_reference[(i * 4) + 1], geometry, last_geometry, 1);
				else
					polygon_edgetest(last_geometry->quad_reference[(i * 4)], last_geometry->quad_reference[(i * 4) + 1], geometry, last_geometry, last_geometry->quad_crease[polygon * 4]);
			}
			if(geometry->quad_reference[(i * 16) + 6] == max_uint)
			{			 
				ref = corner_table[(i % global_subdivide_generartion_count[level - 1]) * 4 + 1];
				if(ref == 255)
					polygon_edgetest(last_geometry->quad_reference[(i * 4) + 1], last_geometry->quad_reference[(i * 4) + 2], geometry, last_geometry, 1);
				else
					polygon_edgetest(last_geometry->quad_reference[(i * 4) + 1], last_geometry->quad_reference[(i * 4) + 2], geometry, last_geometry, last_geometry->quad_crease[polygon * 4 + 1]);
			}
			if(geometry->quad_reference[(i * 16) + 11] == max_uint)
			{
				ref = corner_table[(i % global_subdivide_generartion_count[level - 1]) * 4 + 2];
				if(ref == 255)
					polygon_edgetest(last_geometry->quad_reference[(i * 4) + 2], last_geometry->quad_reference[(i * 4) + 3], geometry, last_geometry, 1);
				else
					polygon_edgetest(last_geometry->quad_reference[(i * 4) + 2], last_geometry->quad_reference[(i * 4) + 3], geometry, last_geometry, last_geometry->quad_crease[polygon * 4 + 2]);
			}
			if(geometry->quad_reference[(i * 16) + 3] == max_uint)
			{
				ref = corner_table[(i % global_subdivide_generartion_count[level - 1]) * 4 + 3];
				if(ref == 255)
					polygon_edgetest(last_geometry->quad_reference[(i * 4) + 3], last_geometry->quad_reference[i * 4], geometry, last_geometry, 1);
				else
					polygon_edgetest(last_geometry->quad_reference[(i * 4) + 3], last_geometry->quad_reference[i * 4], geometry, last_geometry, last_geometry->quad_crease[polygon * 4 + 3]);
			}
		}
	}
	else
	{
		for(i = 0; i < last_geometry->quad_count; i++) 
		{
			if(geometry->quad_reference[(i * 16) + 1] == max_uint)
				polygon_edgetest(last_geometry->quad_reference[(i * 4)], last_geometry->quad_reference[(i * 4) + 1], geometry, last_geometry, 1);  /* looking for naighbours*/
			if(geometry->quad_reference[(i * 16) + 6] == max_uint)
				polygon_edgetest(last_geometry->quad_reference[(i * 4) + 1], last_geometry->quad_reference[(i * 4) + 2], geometry, last_geometry, 1);
			if(geometry->quad_reference[(i * 16) + 11] == max_uint)
				polygon_edgetest(last_geometry->quad_reference[(i * 4) + 2], last_geometry->quad_reference[(i * 4) + 3], geometry, last_geometry, 1);
			if(geometry->quad_reference[(i * 16) + 3] == max_uint)
				polygon_edgetest(last_geometry->quad_reference[(i * 4) + 3], last_geometry->quad_reference[(i * 4)], geometry, last_geometry, 1);
		}
	}
}
