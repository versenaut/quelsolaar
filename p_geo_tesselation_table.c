#include "e_includes.h"
#include "p_geo_subdivide_internal.h"

#define MAX_TESS_TABLE_GENS_PER_COPMPUTE 1000000

typedef enum {
    TESS_POLYGON_TRI = 3,
	TESS_POLYGON_QUAD = 4
} TessPolygonType;

typedef struct{
	TessPolygonType type; /*tri or quad */
	egreal			corner[4][4]; /* vertex number , andt the values it is combined by*/
	uint			edge[4]; /* what edge is it +5 = internal edge */
	uint			level_of_edge[4]; /* the tesselation level of the edges */ 
}weight_polygon;

typedef struct{
	egreal vertical0;
	egreal horizontal0;
	egreal vertical1;
	egreal horizontal1;
	egreal vertical2;
	egreal horizontal2;
	egreal vertical3;
	egreal horizontal3;
}weight_square;

PTessTableElement	*global_tess_table_tri[TESSELATION_TABLE_MAX_LEVEL];
PTessTableElement	*global_tess_table_quad[TESSELATION_TABLE_MAX_LEVEL];
uint				global_tess_level_dynamic;

uint p_get_max_tess_level(void)
{
	return global_tess_level_dynamic; 
}
void init_weight_polygon(TessPolygonType type, weight_polygon *polygon)
{
	uint i, j;
	polygon->type = type;
	for(i = 0 ; i < 4 ; i++)
	{
		for(j = 0 ; j < 4 ; j++)
		{
			if(i == j)
				polygon->corner[i][j] = 1.0;
			else
				polygon->corner[i][j] = 0.0;
		}
		polygon->edge[i] = i;
		polygon->level_of_edge[i] = 0;
	}
}


void copy_vertex(weight_polygon *read, egreal *target, uint vertex1)
{
	target[0] = read->corner[vertex1][0];
	target[1] = read->corner[vertex1][1];
	target[2] = read->corner[vertex1][2];
	target[3] = read->corner[vertex1][3];
}

void divide_edge(weight_polygon *read, egreal *target, uint vertex1, uint vertex2)
{
	target[0] = (read->corner[vertex1][0] + read->corner[vertex2][0]) / 2;
	target[1] = (read->corner[vertex1][1] + read->corner[vertex2][1]) / 2;
	target[2] = (read->corner[vertex1][2] + read->corner[vertex2][2]) / 2;
	target[3] = (read->corner[vertex1][3] + read->corner[vertex2][3]) / 2;
}

void create_middel_vertex(weight_polygon *read, egreal *target)
{
	target[0] = (egreal)(read->corner[0][0] + read->corner[1][0] + read->corner[2][0] + read->corner[3][0]) / 4.0;
	target[1] = (egreal)(read->corner[0][1] + read->corner[1][1] + read->corner[2][1] + read->corner[3][1]) / 4.0;
	target[2] = (egreal)(read->corner[0][2] + read->corner[1][2] + read->corner[2][2] + read->corner[3][2]) / 4.0;
	target[3] = (egreal)(read->corner[0][3] + read->corner[1][3] + read->corner[2][3] + read->corner[3][3]) / 4.0;
}

void tesselate_weight_polygon(weight_polygon *read, weight_polygon *write, uint level)
{
	uint i, j, k, read_count;
	read_count = 1;
	for(i = 0 ; i < level; i++)
		read_count *= 4;

	for(k = 0 ; k < read_count; k++)
	{
		if(read->type == TESS_POLYGON_TRI)
		{
			write[(k * 4) + 0].edge[0] = read[k].edge[0];	     
			write[(k * 4) + 3].edge[0] = read[k].edge[0];	     
	        
			write[(k * 4) + 3].edge[1] = read[k].edge[1];	     
			write[(k * 4) + 1].edge[1] = read[k].edge[1];

			write[(k * 4) + 1].edge[2] = read[k].edge[2];	     
			write[(k * 4) + 0].edge[2] = read[k].edge[2];

			write[(k * 4) + 0].edge[1] = 5;
			write[(k * 4) + 1].edge[0] = 5;
			write[(k * 4) + 2].edge[0] = 5;
			write[(k * 4) + 2].edge[1] = 5;
			write[(k * 4) + 2].edge[2] = 5;
			write[(k * 4) + 3].edge[2] = 5;

			copy_vertex(&read[k], write[(k * 4) + 0].corner[0], 0); /* first polygon */
			divide_edge(&read[k], write[(k * 4) + 0].corner[1], 0, 1);
			divide_edge(&read[k], write[(k * 4) + 0].corner[2], 2, 0);	  
			write[k * 4].type = TESS_POLYGON_TRI;
			divide_edge(&read[k], write[(k * 4) + 1].corner[0], 2, 0);/* second polygon */
			divide_edge(&read[k], write[(k * 4) + 1].corner[1], 1, 2);
			copy_vertex(&read[k], write[(k * 4) + 1].corner[2], 2); 
			write[(k * 4) + 1].type = TESS_POLYGON_TRI;
			divide_edge(&read[k], write[(k * 4) + 2].corner[0], 1, 2); /* third polygon */
			divide_edge(&read[k], write[(k * 4) + 2].corner[1], 2, 0);
			divide_edge(&read[k], write[(k * 4) + 2].corner[2], 0, 1);	  
			write[(k * 4) + 2].type = TESS_POLYGON_TRI;
			divide_edge(&read[k], write[(k * 4) + 3].corner[0], 0, 1);	  /* fourth polygon */
			copy_vertex(&read[k], write[(k * 4) + 3].corner[1], 1);
			divide_edge(&read[k], write[(k * 4) + 3].corner[2], 1, 2);
			write[(k * 4) + 3].type = TESS_POLYGON_TRI;
		}
		else
		{
			copy_vertex(&read[k], write[k * 4].corner[0], 0); /* first polygon */
			divide_edge(&read[k], write[k * 4].corner[1], 0, 1);
			create_middel_vertex(&read[k], write[k * 4].corner[2]);	       
			divide_edge(&read[k], write[k * 4].corner[3], 3, 0);	    
			write[k * 4].type = TESS_POLYGON_QUAD;
			divide_edge(&read[k], write[(k * 4) + 1].corner[0], 0, 1); /* second polygon */
			copy_vertex(&read[k], write[(k * 4) + 1].corner[1], 1);
			divide_edge(&read[k], write[(k * 4) + 1].corner[2], 1, 2);
			create_middel_vertex(&read[k], write[(k * 4) + 1].corner[3]);
			write[(k * 4) + 1].type = TESS_POLYGON_QUAD;
			create_middel_vertex(&read[k], write[(k * 4) + 2].corner[0]); /* third polygon */
			divide_edge(&read[k], write[(k * 4) + 2].corner[1], 1, 2);
			copy_vertex(&read[k], write[(k * 4) + 2].corner[2], 2);	       
			divide_edge(&read[k], write[(k * 4) + 2].corner[3], 2, 3);
			write[(k * 4) + 2].type = TESS_POLYGON_QUAD;
			divide_edge(&read[k], write[(k * 4) + 3].corner[0], 0, 3);	  /* fourth polygon */
			create_middel_vertex(&read[k], write[(k * 4) + 3].corner[1]);
			divide_edge(&read[k], write[(k * 4) + 3].corner[2], 2, 3);	  
			copy_vertex(&read[k], write[(k * 4) + 3].corner[3], 3);
			write[(k * 4) + 3].type = TESS_POLYGON_QUAD;
		}
		for(i = 0 ; i < 4 ; i++)
		{
			for(j = 0 ; j < 4 ; j++)
			{
				write[(k * 4) + i].level_of_edge[j] = level + 1;
			}
		}
	}
}

uint corner_split(weight_polygon *polygon, weight_square *square, uint tess0, uint tess1)
{
	uint i, poly_count = 0;
	egreal h, v;
	for(i = 0; i < tess0; i++)
	{
		polygon[poly_count].corner[0][0] = (1 - square->vertical3) * (1 - square->horizontal3);
		polygon[poly_count].corner[0][1] = (1 - square->vertical3) * (square->horizontal3);
		polygon[poly_count].corner[0][2] = (square->vertical3) * (square->horizontal3);
		polygon[poly_count].corner[0][3] = (square->vertical3) * (1 - square->horizontal3);

		h = ((tess0 - i) * square->horizontal0 + i * square->horizontal1) / tess0;
		v = ((tess0 - i) * square->vertical0 + i * square->vertical1) / tess0;
	        
		polygon[poly_count].corner[1][0] = (1 - v) * (1 - h);
		polygon[poly_count].corner[1][1] = (1 - v) * h;
		polygon[poly_count].corner[1][2] = v * h;
		polygon[poly_count].corner[1][3] = v * (1 - h);

		i++;
		h = ((tess0 - i) * square->horizontal0 + i * square->horizontal1) / tess0;
		v = ((tess0 - i) * square->vertical0 + i * square->vertical1) / tess0;
		i--;

		polygon[poly_count].corner[2][0] = (1 - v) * (1 - h);
		polygon[poly_count].corner[2][1] = (1 - v) * h;
		polygon[poly_count].corner[2][2] = v * h;
		polygon[poly_count].corner[2][3] = v * (1 - h);		       
		poly_count++;
	}
	for(i = 0; i < tess1; i++)
	{
		polygon[poly_count].corner[0][0] = (1 - square->vertical3) * (1 - square->horizontal3);
		polygon[poly_count].corner[0][1] = (1 - square->vertical3) * (square->horizontal3);
		polygon[poly_count].corner[0][2] = (square->vertical3) * (square->horizontal3);
		polygon[poly_count].corner[0][3] = (square->vertical3) * (1 - square->horizontal3);

		h = ((tess1 - i) * square->horizontal1 + i * square->horizontal2) / tess1;
		v = ((tess1 - i) * square->vertical1 + i * square->vertical2) / tess1;
	        
		polygon[poly_count].corner[1][0] = (1 - v) * (1 - h);
		polygon[poly_count].corner[1][1] = (1 - v) * h;
		polygon[poly_count].corner[1][2] = v * h;
		polygon[poly_count].corner[1][3] = v * (1 - h);

		i++;
		h = ((tess1 - i) * square->horizontal1 + i * square->horizontal2) / tess1;
		v = ((tess1 - i) * square->vertical1 + i * square->vertical2) / tess1;
		i--;

		polygon[poly_count].corner[2][0] = (1 - v) * (1 - h);
		polygon[poly_count].corner[2][1] = (1 - v) * h;
		polygon[poly_count].corner[2][2] = v * h;
		polygon[poly_count].corner[2][3] = v * (1 - h);		       
		poly_count++;
	}
	return poly_count;
}

uint middle_split(weight_polygon *polygon, weight_square *square, uint h_tess, uint v_tess)
{
	uint i, j, poly_count = 0;
	egreal v, h, v1, h1;

	h = (square->horizontal1 - square->horizontal0) / (egreal)h_tess;
	v = (square->vertical3 - square->vertical0) / (egreal)v_tess;

	for(i = 0; i < h_tess; i++) /* filling the middle of the quad with polygons*/
	{
		for(j = 0; j < v_tess; j++)
		{
			h1 = square->horizontal0 + i * h;
			v1 = square->vertical0 + j * v;
        
			polygon[poly_count].corner[0][0] = (1 - v1) * (1 - h1);
			polygon[poly_count].corner[0][1] = (1 - v1) * (h1);
			polygon[poly_count].corner[0][2] = (v1) * (h1);
			polygon[poly_count].corner[0][3] = (v1) * (1 - h1);

			h1 = square->horizontal0 + i * h + h;
			v1 = square->vertical0 + j * v;

			polygon[poly_count].corner[1][0] = (1 - v1) * (1 - h1);
			polygon[poly_count].corner[1][1] = (1 - v1) * (h1);
			polygon[poly_count].corner[1][2] = (v1) * (h1);
			polygon[poly_count].corner[1][3] = (v1) * (1 - h1);

			h1 = square->horizontal0 + i * h;
			v1 = square->vertical0 + j * v + v;

			polygon[poly_count].corner[2][0] = (1 - v1) * (1 - h1);
			polygon[poly_count].corner[2][1] = (1 - v1) * (h1);
			polygon[poly_count].corner[2][2] = (v1) * (h1);
			polygon[poly_count].corner[2][3] = (v1) * (1 - h1);

			h1 = square->horizontal0 + i * h;
			v1 = square->vertical0 + j * v + v;

			poly_count++;

			polygon[poly_count].corner[0][0] = (1 - v1) * (1 - h1);
			polygon[poly_count].corner[0][1] = (1 - v1) * (h1);
			polygon[poly_count].corner[0][2] = (v1) * (h1);
			polygon[poly_count].corner[0][3] = (v1) * (1 - h1);

			h1 = square->horizontal0 + i * h + h;
			v1 = square->vertical0 + j * v;

			polygon[poly_count].corner[1][0] = (1 - v1) * (1 - h1);
			polygon[poly_count].corner[1][1] = (1 - v1) * (h1);
			polygon[poly_count].corner[1][2] = (v1) * (h1);
			polygon[poly_count].corner[1][3] = (v1) * (1 - h1);

			h1 = square->horizontal0 + i * h + h;
			v1 = square->vertical0 + j * v + v;

			polygon[poly_count].corner[2][0] = (1 - v1) * (1 - h1);
			polygon[poly_count].corner[2][1] = (1 - v1) * (h1);
			polygon[poly_count].corner[2][2] = (v1) * (h1);
			polygon[poly_count].corner[2][3] = (v1) * (1 - h1);

			poly_count++;
		}
	}
	return poly_count;
}
uint edge_split(weight_polygon *polygon, weight_square *square, uint tess_0, uint tess_1)
{
	uint i, j, poly_count = 0;
	egreal v0, v1, h0, h1;

	v0 = square->vertical0;
	v1 = square->vertical3;
	h0 = square->horizontal0;
	h1 = square->horizontal3;

	j = tess_0 / (tess_1 * 2);

	for(i = 0; ; i++)
	{
		for(; j < tess_0 / tess_1; j++)
		{
			polygon[poly_count].corner[0][0] = (1 - v1) * (1 - h1);
			polygon[poly_count].corner[0][1] = (1 - v1) * (h1);
			polygon[poly_count].corner[0][2] = (v1) * (h1);
			polygon[poly_count].corner[0][3] = (v1) * (1 - h1);

			polygon[poly_count].corner[1][0] = (1 - v0) * (1 - h0);
			polygon[poly_count].corner[1][1] = (1 - v0) * (h0);
			polygon[poly_count].corner[1][2] = (v0) * (h0);
			polygon[poly_count].corner[1][3] = (v0) * (1 - h0);

			v0 += (square->vertical1 - square->vertical0) / tess_0;
			h0 += (square->horizontal1 - square->horizontal0) / tess_0;

			polygon[poly_count].corner[2][0] = (1 - v0) * (1 - h0);
			polygon[poly_count].corner[2][1] = (1 - v0) * (h0);
			polygon[poly_count].corner[2][2] = (v0) * (h0);
			polygon[poly_count].corner[2][3] = (v0) * (1 - h0);

			poly_count++;

			if(poly_count == tess_1 + tess_0)
				return poly_count;
		}

		polygon[poly_count].corner[0][0] = (1 - v1) * (1 - h1);
		polygon[poly_count].corner[0][1] = (1 - v1) * (h1);
		polygon[poly_count].corner[0][2] = (v1) * (h1);
		polygon[poly_count].corner[0][3] = (v1) * (1 - h1);

		polygon[poly_count].corner[1][0] = (1 - v0) * (1 - h0);
		polygon[poly_count].corner[1][1] = (1 - v0) * (h0);
		polygon[poly_count].corner[1][2] = (v0) * (h0);
		polygon[poly_count].corner[1][3] = (v0) * (1 - h0);

		v1 += (square->vertical2 - square->vertical3) / tess_1;
		h1 += (square->horizontal2 - square->horizontal3) / tess_1;

		polygon[poly_count].corner[2][0] = (1 - v1) * (1 - h1);
		polygon[poly_count].corner[2][1] = (1 - v1) * (h1);
		polygon[poly_count].corner[2][2] = (v1) * (h1);
		polygon[poly_count].corner[2][3] = (v1) * (1 - h1);

		poly_count++;
			if(poly_count == tess_1 + tess_0)
			{
				printf("error\n");
				return poly_count;
			}
		j = 0;
	}
	return poly_count;
}

uint pow_level(uint level)
{
	uint pow, i;
	pow = 1;
	for(i = 0; i < level; i++)
		pow *= 2;
	return pow;
}

uint lowest_level(uint level0, uint level1)
{
	if(level0 > level1)
		return level1;
	else
		return level0;
}

uint init_weight_quad(weight_polygon *polygon, uint level3, uint level2, uint level1, uint level0)
{
	int poly_count, h_level = 0, v_level = 0;
	weight_square square, square2;

	poly_count = 0;

	level0 = pow_level(level0);
	level1 = pow_level(level1);
	level2 = pow_level(level2);
	level3 = pow_level(level3);
        

	square.horizontal0 = 0;
	square.horizontal1 = 1;
	square.horizontal2 = 1;
	square.horizontal3 = 0;
	square.vertical0 = 0;
	square.vertical1 = 0;
	square.vertical2 = 1;
	square.vertical3 = 1;

/*	poly_count += middle_split(polygon, &square, lowest_level(level0, level2), lowest_level(level1, level3));
		return poly_count;
*/
	if(level0 > level2)
	{
		square.vertical0 = 1.0 / (egreal)lowest_level(level1, level3);
		square.vertical1 = 1.0 / (egreal)lowest_level(level1, level3);
		h_level = level2;
		v_level -= 1;
	}
	else
	{
		h_level = level0;
		if(level0 != level2)
		{
			square.vertical2 = ((egreal)lowest_level(level1, level3) - 1.0) / (egreal)lowest_level(level1, level3);
			square.vertical3 = ((egreal)lowest_level(level1, level3) - 1.0) / (egreal)lowest_level(level1, level3);
			v_level -= 1;
		}
	}

	if(level3 > level1)
	{
		square.horizontal0 = 1.0 / (egreal)lowest_level(level0, level2);
		square.horizontal3 = 1.0 / (egreal)lowest_level(level0, level2);
		v_level += level1;
		h_level -= 1;
	}
	else
	{
		v_level += level3;
		if(level3 != level1)
		{	 
			square.horizontal1 = ((egreal)lowest_level(level0, level2) - 1.0) / (egreal)lowest_level(level0, level2);
			square.horizontal2 = ((egreal)lowest_level(level0, level2) - 1.0) / (egreal)lowest_level(level0, level2);
			h_level -= 1;
		}
	}	 
	if(h_level > 0 && v_level > 0)
		poly_count += middle_split(polygon, &square, h_level, v_level);

	if(h_level != 0)
	{
		if(level0 > level2)
		{

			square2 = square;
			square2.vertical2 = square.vertical0;
			square2.vertical3 = square.vertical1;
			square2.vertical0 = 0;
			square2.vertical1 = 0;
	
			poly_count += edge_split(&polygon[poly_count], &square2, h_level * (level0 / level2), h_level);
		}
		else
		{
			if(level0 != level2)
			{
				square2 = square;
				square2.horizontal0 = square2.horizontal1;
				square2.horizontal1 = square2.horizontal3;
				square2.horizontal2 = square2.horizontal3;
				square2.horizontal3 = square2.horizontal0;
				square2.vertical0 = 1;
				square2.vertical1 = 1;

				poly_count += edge_split(&polygon[poly_count], &square2, h_level * (level2 / level0), h_level);
			}
		}
	}
	if(v_level != 0)
	{
		if(level1 > level3)
		{
			square2 = square;
			square2.horizontal0 = 1;
			square2.horizontal1 = 1;
			square2.horizontal3 = square.horizontal1;
			square2.vertical1 = square.vertical2;
			square2.vertical3 = square.vertical0;	

			poly_count += edge_split(&polygon[poly_count], &square2, v_level * (level1 / level3), v_level);
		}
		else
		{
			if(level1 != level3)
			{
				square2 = square;
				square2.horizontal0 = 0;
				square2.horizontal1 = 0;
				square2.horizontal2 = square.horizontal0;
				square2.vertical0 = square.vertical3;
				square2.vertical1 = square.vertical0;
				square2.vertical2 = square.vertical0;	     

				poly_count += edge_split(&polygon[poly_count], &square2, v_level * (level3 / level1), v_level);
			}
		}
	}

	if(level1 == level3 && level2 == level0)
		return poly_count;

	if((v_level % 2 == 1 && h_level % 2 == 1) || (v_level == 0 && h_level == 0))
	{
		if(square.horizontal0 == 0)
		{
			if(square.vertical0 == 0)
			{
				square2.horizontal0 = 1;
				square2.horizontal1 = 1;
				square2.horizontal2 = square.horizontal1;
				square2.horizontal3 = square.horizontal1;
				square2.vertical0 = square.vertical2;
				square2.vertical1 = 1;
				square2.vertical2 = 1;
				square2.vertical3 = square.vertical2;
				poly_count += corner_split(&polygon[poly_count], &square2, level1 / level3, level2 / level0);
			}
			else
			{
				square2.horizontal0 = square.horizontal1;
				square2.horizontal1 = 1;
				square2.horizontal2 = 1;
				square2.horizontal3 = square.horizontal1;
				square2.vertical0 = 0;
				square2.vertical1 = 0;
				square2.vertical2 = square.vertical1;
				square2.vertical3 = square.vertical1;
				poly_count += corner_split(&polygon[poly_count], &square2, level0 / level2, level1 / level3);
			}
		}
		else
		{
			if(square.vertical0 == 0)
			{
				square2.horizontal0 = square.horizontal0;
				square2.horizontal1 = 0;
				square2.horizontal2 = 0;
				square2.horizontal3 = square.horizontal0;
				square2.vertical0 = 1;
				square2.vertical1 = 1;
				square2.vertical2 = square.vertical2;
				square2.vertical3 = square.vertical2;
				poly_count += corner_split(&polygon[poly_count], &square2, level2 / level0, level3 / level1);
			}
			else
			{
				square2.horizontal0 = 0;
				square2.horizontal1 = 0;
				square2.horizontal2 = square.horizontal0;
				square2.horizontal3 = square.horizontal0;
				square2.vertical0 = square.vertical0;
				square2.vertical1 = 0;
				square2.vertical2 = 0;
				square2.vertical3 = square.vertical0;
				poly_count += corner_split(&polygon[poly_count], &square2, level3 / level1, level0 / level2);
			}
		}
	}
	return poly_count;
}


uint calculate_tri_polyon_count(uint edge_level0, uint edge_level1, uint edge_level2)
{
	uint polycount, edgecount, i, j;
	uint tess_level = edge_level0;

	if(edge_level1 < tess_level)
		tess_level = edge_level1;
	if(edge_level2 < tess_level)
		tess_level = edge_level2;

	edgecount = 1;
	polycount = 1;
	for(i = 0 ;i < tess_level; i++)
	{
		edgecount *= 2;
		polycount *= 4;
	}

	j = 1;
	for(i = 0 ;i < edge_level0 - tess_level; i++)
		j *= 2;
	polycount += (j * edgecount) - edgecount;

	j = 1;
	for(i = 0 ;i < edge_level1 - tess_level; i++)
		j *= 2;
	polycount += (j * edgecount) - edgecount;

	j = 1;
	for(i = 0 ;i < edge_level2 - tess_level; i++)
		j *= 2;
	polycount += (j * edgecount) - edgecount;

	return polycount;
}

void split_polygon(weight_polygon *read, weight_polygon *write, uint edge)
{
	uint vertex0 , vertex1 , vertex2;

	if(edge == 0)
	{
		vertex0 = 0;
		vertex1 = 1;
		vertex2 = 2;
	}
	else
	if(edge == 1)
	{
		vertex0 = 1;
		vertex1 = 2;
		vertex2 = 0;
	}
	else
	{
	        
		vertex0 = 2;
		vertex1 = 0;
		vertex2 = 1;
	}

	write[0].edge[vertex0] = read->edge[vertex0];		     
	write[0].edge[vertex1] = 5;
	write[0].edge[vertex2] = read->edge[vertex2];
	write[0].edge[3] = 5;

	write[1].edge[vertex0] = read->edge[vertex0];
	write[1].edge[vertex1] = read->edge[vertex1];	     
	write[1].edge[vertex2] = 5;
	write[1].edge[3] = 5;

	write[0].level_of_edge[vertex0] = read->level_of_edge[vertex0] + 1;
	write[0].level_of_edge[vertex1] = 0;
	write[0].level_of_edge[vertex2] = read->level_of_edge[vertex2];

	write[1].level_of_edge[vertex0] = read->level_of_edge[vertex0] + 1;
	write[1].level_of_edge[vertex1] = read->level_of_edge[vertex1];
	write[1].level_of_edge[vertex2] = 0;

	copy_vertex(read, write[0].corner[vertex0], vertex0); /* first polygon */
	divide_edge(read, write[0].corner[vertex1], vertex0, vertex1);
	copy_vertex(read, write[0].corner[vertex2], vertex2);	      

	divide_edge(read, write[1].corner[vertex0], vertex1, vertex0);/* second polygon */
	copy_vertex(read, write[1].corner[vertex1], vertex1); 
	copy_vertex(read, write[1].corner[vertex2], vertex2); 
}

weight_polygon *split_tri_edges(weight_polygon *read, weight_polygon *write1, uint in_count, uint out_count, uint *level)
{
	weight_polygon *temp; 
	uint current_polygon, added_polygons = 0, current_edge = 1;
	uint i;

	while(in_count < out_count)
	{
		current_polygon = 0;
		while(current_polygon < in_count )
		{
			for(i = 0 ; i < 3 ; i++)
			{
				if(read[current_polygon].edge[i] == current_edge)
				{
					if(read[current_polygon].level_of_edge[i] < level[current_edge])
					{
						split_polygon(&read[current_polygon], &write1[current_polygon + added_polygons], i);
						added_polygons++;
						i = 5;
					}
				}
			}
			if(i == 3)
				 write1[current_polygon + added_polygons] = read[current_polygon];

			current_polygon++;
		}


		in_count += added_polygons;
		added_polygons = 0;
		++current_edge;
		if(current_edge == 3)
			current_edge = 0;

		temp = read;
		read = write1;
		write1 = temp;
	}
	return read;
}



void make_table_element(PTessTableElement *table, weight_polygon *read, uint polycount, PTessTableElement *base, TessPolygonType type)
{
	uint i, j, k, corner,	found, corner_compare;
	table->index = malloc(sizeof(uint) * polycount * type);
	table->vertex_influence = malloc(sizeof(float) * polycount * type * type);
	table->reference = malloc(sizeof(uint) * polycount * type);

	table->element_count = 0;
	table->vertex_count = 0;

	corner_compare = type;
	if(base != NULL)
		corner_compare = 3;

	for(j = 0 ; j < polycount; j++)
	{
		for(corner = 0 ; corner < corner_compare ; corner++)
		{
			found = -1;
			if(type == 3)
			{
				for(i = 0 ; i < table->element_count && found == -1 ; i++) /* finding vertexes allready used by the polygon*/
					if(table->vertex_influence[i * 3] == read[j].corner[corner][0] &&
					table->vertex_influence[(i * 3) + 1] == read[j].corner[corner][1] &&
					table->vertex_influence[(i * 3) + 2] == read[j].corner[corner][2])
					found = i;
			}
			else
			{
				for(i = 0 ; i < table->element_count && found == -1 ; i++) /* finding vertexes allready used by the polygon*/
					if(table->vertex_influence[i * 4] == read[j].corner[corner][0] &&
					table->vertex_influence[(i * 4) + 1] == read[j].corner[corner][1] &&
					table->vertex_influence[(i * 4) + 2] == read[j].corner[corner][2] &&
					table->vertex_influence[(i * 4) + 3] == read[j].corner[corner][3])
					found = i;
			}
			if(found != -1) 
			{
				table->index[table->element_count] = found;
			}
			else
			{
				if(base == NULL)
				{
					table->reference[table->vertex_count] = table->element_count;
				}
				else
				{
					if(type == 3)
						for(k = 0 ;read[j].corner[corner][0] 
									!= base->vertex_influence[k * 3] ||
								   read[j].corner[corner][1] 
								   != base->vertex_influence[(k * 3) + 1] ||
								   read[j].corner[corner][2]
								   != base->vertex_influence[(k * 3) + 2] ;k++)
							;
					else
						for(k = 0 ;read[j].corner[corner][0] 
									!= base->vertex_influence[k * 4] ||
								   read[j].corner[corner][1] 
								   != base->vertex_influence[(k * 4) + 1] ||
								   read[j].corner[corner][2] 
								   != base->vertex_influence[(k * 4) + 2] ||
								   read[j].corner[corner][3]
								   != base->vertex_influence[(k * 4) + 3] ;k++)
							;
					table->reference[table->vertex_count] = base->reference[k];
				}
				table->index[table->element_count] = table->vertex_count;
				table->vertex_influence[table->vertex_count * type] = read[j].corner[corner][0];
				table->vertex_influence[(table->vertex_count * type) + 1] = read[j].corner[corner][1];
				table->vertex_influence[(table->vertex_count * type) + 2] = read[j].corner[corner][2];
				if(type == 4)
					table->vertex_influence[(table->vertex_count * type) + 3] = read[j].corner[corner][3];
				table->vertex_count++;
			}
			table->element_count++;
		}
	}
	table->reference = realloc(table->reference ,sizeof(uint) * table->element_count);
}
/*
boolean p_init_table(uint tess_level)
{
	uint i, j, k, polycount;
	static weight_polygon *write0, *write1, *write2, *temp;
	static PTessTableElement *table;
	static uint stage = 0, sub_stage = 0, edge[4];

	switch(stage)
	{
		case 0 :
			global_tess_level_dynamic = tess_level;
			tess_level++;
			write0 = malloc(sizeof(weight_polygon) * pow_level(tess_level) * pow_level(tess_level) * 2);
			write1 = malloc(sizeof(weight_polygon) * pow_level(tess_level) * pow_level(tess_level) * 2);
			write2 = malloc(sizeof(weight_polygon) * pow_level(tess_level) * pow_level(tess_level) * 2);
			table = malloc((sizeof *table) * (tess_level + 1));

			polycount = 1;
			init_weight_polygon(TESS_POLYGON_TRI, write1);
			make_table_element(table, write1, polycount, NULL, TESS_POLYGON_TRI);
			tesselate_weight_polygon(write1, write0, 0);

			for(i = 1 ; i < tess_level; i++)
			{
				polycount *= 4;
				make_table_element(&table[i], write0 , polycount, NULL, TESS_POLYGON_TRI);

				tesselate_weight_polygon(write0, write1, i);
				temp = write1;
				write1 = write0;
				write0 = temp;
				global_tess_table_tri[i] = malloc(sizeof(PTessTableElement) * (i + 1) * (i + 1) * (i + 1));
				global_tess_table_quad[i] = malloc(sizeof(PTessTableElement) * (i + 1) * (i + 1) * (i + 1) * (i + 1));
			}
			stage++;
			sub_stage = 1;
		break;
		case 1 :
			k = 0;
			tess_level = global_tess_level_dynamic;
			for(; sub_stage <= tess_level ; sub_stage++)
			{
				for(edge[0] = 0 ; edge[0] <= tess_level ; edge[0]++)
				{
					for(edge[1] = 0 ; edge[1] <= tess_level ; edge[1]++)
					{
						for(edge[2] = 0 ; edge[2] <= tess_level ; edge[2]++)
						{
							if(k++ > MAX_TESS_TABLE_GENS_PER_COPMPUTE)
								return FALSE;
							j = edge[0];
							if(j > edge[1])
								j = edge[1];
							if(j > edge[2])
								j = edge[2];

							polycount = 1;
							if(j > 0)
							{
								init_weight_polygon(TESS_POLYGON_TRI, write1);
								tesselate_weight_polygon(write1, write0, 0);
								for(i = 1 ; i < j ; i++ )
								{
									polycount *= 4;
									tesselate_weight_polygon(write0, write1, i);
									temp = write1;
									write1 = write0;
									write0 = temp;
								}
								polycount = calculate_tri_polyon_count(edge[0], edge[1], edge[2]);
								temp = split_tri_edges(write0, write1,	 calculate_tri_polyon_count(j, j, j) , polycount, edge);
							}
							else
							{
								init_weight_polygon(TESS_POLYGON_TRI, write0);
								polycount = calculate_tri_polyon_count(edge[0], edge[1], edge[2]);
								temp = split_tri_edges(write0, write1,	 1, polycount, edge);
							}

							make_table_element(&global_tess_table_tri[sub_stage][ edge[0] + (edge[1] * (sub_stage + 1))  + (edge[2] * (sub_stage + 1) * (sub_stage + 1) )], temp , polycount,  &table[sub_stage], TESS_POLYGON_TRI);
						}
					}
				}
			}
			stage++;
		break;
		case 2 :
			tess_level = global_tess_level_dynamic + 1;
			polycount = 1;
			init_weight_polygon(TESS_POLYGON_QUAD, write1);
			make_table_element(table, write1, polycount, NULL, TESS_POLYGON_QUAD);
			tesselate_weight_polygon(write1, write0, 0);

			for(i = 1 ; i < tess_level ; i++ )
			{
				polycount *= 4;

				make_table_element(&table[i], write0 , polycount, NULL, TESS_POLYGON_QUAD);
				tesselate_weight_polygon(write0, write1, i);
				temp = write1;
				write1 = write0;
				write0 = temp;
			}
			stage++;
			sub_stage = 1;
		break;
		case 3 :
			k = 0;
			tess_level = global_tess_level_dynamic;
			for(; sub_stage <= tess_level ; sub_stage++)
			{
				global_tess_table_quad[tess_level] = malloc(sizeof(PTessTableElement) * (tess_level + 1) * (tess_level + 1) * (tess_level + 1) * (tess_level + 1));
				for(edge[0] = 0 ; edge[0] <= tess_level ; edge[0]++)
				{
					for(edge[1] = 0 ; edge[1] <= tess_level ; edge[1]++)
					{
						for(edge[2] = 0 ; edge[2] <= tess_level ; edge[2]++)
						{
							for(edge[3] = 0 ; edge[3] <= tess_level ; edge[3]++)
							{
								if(k++ > MAX_TESS_TABLE_GENS_PER_COPMPUTE)
									return FALSE;
								polycount = init_weight_quad(temp, edge[0], edge[1], edge[2], edge[3]);
								make_table_element(&global_tess_table_quad[sub_stage][ edge[0] + (edge[1] * (sub_stage + 1))  + (edge[2] * (sub_stage + 1) * (sub_stage + 1) ) + (edge[3] * (sub_stage + 1) * (sub_stage + 1) * (sub_stage + 1))], temp , polycount,  &table[sub_stage], TESS_POLYGON_QUAD);
							}
						}
					}
				}
			}
		break;
		case 4 :
			free(table);
			free(write0);
			free(write1);
			free(write2);
		return TRUE;
	}
	return FALSE;
}
*/
boolean p_init_table(uint tess_level)
{
	static uint i, j, k, polycount, base_level;
	static weight_polygon *write0, *write1, *write2, *temp;
	static PTessTableElement *table;
	static uint stage[3] = {0, 0, 0}, edge[4];

	if(stage[0] != 0)
		tess_level = global_tess_level_dynamic;

	switch(stage[0])
	{
		case 0 :
		global_tess_level_dynamic = tess_level;

		tess_level++;
		write0 = malloc(sizeof(weight_polygon) * pow_level(tess_level) * pow_level(tess_level) * 2);
		write1 = malloc(sizeof(weight_polygon) * pow_level(tess_level) * pow_level(tess_level) * 2);
		write2 = malloc(sizeof(weight_polygon) * pow_level(tess_level) * pow_level(tess_level) * 2);
		table = malloc((sizeof *table) * (tess_level + 1));

		polycount = 1;
		init_weight_polygon(TESS_POLYGON_TRI, write1);
		make_table_element(table, write1, polycount, NULL, TESS_POLYGON_TRI);
		tesselate_weight_polygon(write1, write0, 0);

		for(i = 1 ; i < tess_level; i++)
		{
			polycount *= 4;
			make_table_element(&table[i], write0 , polycount, NULL, TESS_POLYGON_TRI);

			tesselate_weight_polygon(write0, write1, i);
			temp = write1;
			write1 = write0;
			write0 = temp;
		}
		tess_level--;

		for(base_level = 1 ; base_level <= tess_level ; base_level++)
			global_tess_table_tri[base_level] = malloc(sizeof(PTessTableElement) * (base_level + 1) * (base_level + 1) * (base_level + 1));
		stage[0] = 1;
		stage[1] = 1;
		stage[2] = 0;
		break;
		case 1 :
		i = 0;
		for(; stage[1] <= tess_level ; stage[1]++)
		{
			for(; stage[2] < (stage[1] + 1) * (stage[1] + 1) * (stage[1] + 1); stage[2]++)
			{
				if(i++ > 4)
					return FALSE;
				stage[1]++; 
				edge[0] = stage[2] % stage[1];
				j = stage[2] / stage[1];
				edge[1] = j % stage[1];
				j = j / stage[1];
				edge[2] = j % stage[1];
				stage[1]--;
				j = edge[0];
				if(j > edge[1])
					j = edge[1];
				if(j > edge[2])
					j = edge[2];

				polycount = 1;
				if(j > 0)
				{
					init_weight_polygon(TESS_POLYGON_TRI, write1);
					tesselate_weight_polygon(write1, write0, 0);
					for(k = 1 ; k < j; k++)
					{
						polycount *= 4;
						tesselate_weight_polygon(write0, write1, k);
						temp = write1;
						write1 = write0;
						write0 = temp;
					}
					polycount = calculate_tri_polyon_count(edge[0], edge[1], edge[2]);
					temp = split_tri_edges(write0, write1,	 calculate_tri_polyon_count(j, j, j) , polycount, edge);
				}
				else
				{
					init_weight_polygon(TESS_POLYGON_TRI, write0);
					polycount = calculate_tri_polyon_count(edge[0], edge[1], edge[2]);
					temp = split_tri_edges(write0, write1, 1, polycount, edge);
				}

				make_table_element(&global_tess_table_tri[stage[1]][ edge[0] + (edge[1] * (stage[1] + 1))  + (edge[2] * (stage[1] + 1) * (stage[1] + 1) )], temp , polycount,  &table[stage[1]], TESS_POLYGON_TRI);
			}
			stage[2] = 0;
		}
		stage[0] = 2;
		break;
		case 2 :

		tess_level++;
		polycount = 1;
		init_weight_polygon(TESS_POLYGON_QUAD, write1);
		make_table_element(table, write1, polycount, NULL, TESS_POLYGON_QUAD);
		tesselate_weight_polygon(write1, write0, 0);

		for(i = 1 ; i < tess_level ; i++ )
		{
			polycount *= 4;

			make_table_element(&table[i], write0 , polycount, NULL, TESS_POLYGON_QUAD);
			tesselate_weight_polygon(write0, write1, i);
			temp = write1;
			write1 = write0;
			write0 = temp;
		}
		tess_level--;
		for(base_level = 1 ; base_level <= tess_level ; base_level++)
			global_tess_table_quad[base_level] = malloc(sizeof(PTessTableElement) * (base_level + 1) * (base_level + 1) * (base_level + 1) * (base_level + 1));
		stage[0] = 3;
		stage[1] = 1;
		stage[2] = 0;
		break;
		case 3 :
		i = 0;
		for(; stage[1] <= tess_level ; stage[1]++)
		{
			for(; stage[2] < (stage[1] + 1) * (stage[1] + 1) * (stage[1] + 1) * (stage[1] + 1); stage[2]++)
			{
				if(i++ > 4)
					return FALSE;
				stage[1]++;
				edge[0] = stage[2] % stage[1];
				j = stage[2] / stage[1];
				edge[1] = j % stage[1];
				j = j / stage[1];
				edge[2] = j % stage[1];
				j = j / stage[1];
				edge[3] = j % stage[1];
				stage[1]--;
				polycount = init_weight_quad(temp, edge[0], edge[1], edge[2], edge[3]);
				make_table_element(&global_tess_table_quad[stage[1]][edge[0] + (edge[1] * (stage[1] + 1))  + (edge[2] * (stage[1] + 1) * (stage[1] + 1) ) + (edge[3] * (stage[1] + 1) * (stage[1] + 1) * (stage[1] + 1))], temp , polycount,  &table[stage[1]], TESS_POLYGON_QUAD);
			}
			stage[2] = 0;
		}
		stage[0] = 4;
		break;
		case 4 :
		free(table);
		free(write0);
		free(write1);
		free(write2);
		return TRUE;
	}
	return FALSE;
}

void p_init_table_old(uint tess_level)
{
	uint i, j, polycount, base_level;
	weight_polygon *write0, *write1, *write2, *temp;
	PTessTableElement *table;
	uint edge[4];
        
	base_level = tess_level;

	tess_level++;
	write0 = malloc(sizeof(weight_polygon) * pow_level(tess_level) * pow_level(tess_level) * 2);
	write1 = malloc(sizeof(weight_polygon) * pow_level(tess_level) * pow_level(tess_level) * 2);
	write2 = malloc(sizeof(weight_polygon) * pow_level(tess_level) * pow_level(tess_level) * 2);
	table = malloc((sizeof *table) * (tess_level + 1));

	polycount = 1;
	init_weight_polygon(TESS_POLYGON_TRI, write1);
	make_table_element(table, write1, polycount, NULL, TESS_POLYGON_TRI);
	tesselate_weight_polygon(write1, write0, 0);

	for(i = 1 ; i < tess_level; i++ )
	{
		polycount *= 4;
		make_table_element(&table[i], write0 , polycount, NULL, TESS_POLYGON_TRI);

		tesselate_weight_polygon(write0, write1, i);
		temp = write1;
		write1 = write0;
		write0 = temp;
	}
	tess_level--;

	for(base_level = 1 ; base_level <= tess_level ; base_level++)
	{
		global_tess_table_tri[base_level] = malloc(sizeof(PTessTableElement) * (base_level + 1) * (base_level + 1) * (base_level + 1));
		for(edge[0] = 0 ; edge[0] <= base_level ; edge[0]++)
		{
			for(edge[1] = 0 ; edge[1] <= base_level ; edge[1]++)
			{
				for(edge[2] = 0 ; edge[2] <= base_level ; edge[2]++)
				{
					j = edge[0];
					if(j > edge[1])
						j = edge[1];
					if(j > edge[2])
						j = edge[2];

					polycount = 1;
					if(j > 0)
					{
						init_weight_polygon(TESS_POLYGON_TRI, write1);
						tesselate_weight_polygon(write1, write0, 0);
						for(i = 1 ; i < j ; i++ )
						{
							polycount *= 4;
							tesselate_weight_polygon(write0, write1, i);
							temp = write1;
							write1 = write0;
							write0 = temp;
						}
						polycount = calculate_tri_polyon_count(edge[0], edge[1], edge[2]);
						temp = split_tri_edges(write0, write1,	 calculate_tri_polyon_count(j, j, j) , polycount, edge);
					}
					else
					{
						init_weight_polygon(TESS_POLYGON_TRI, write0);
						polycount = calculate_tri_polyon_count(edge[0], edge[1], edge[2]);
						temp = split_tri_edges(write0, write1,	 1, polycount, edge);
					}

					make_table_element(&global_tess_table_tri[base_level][ edge[0] + (edge[1] * (base_level + 1))  + (edge[2] * (base_level + 1) * (base_level + 1) )], temp , polycount,  &table[base_level], TESS_POLYGON_TRI);
				}
			}
		}
	}
	global_tess_level_dynamic = tess_level;

	tess_level++;

	polycount = 1;
	init_weight_polygon(TESS_POLYGON_QUAD, write1);
	make_table_element(table, write1, polycount, NULL, TESS_POLYGON_QUAD);
	tesselate_weight_polygon(write1, write0, 0);

	for(i = 1 ; i < tess_level ; i++ )
	{
		polycount *= 4;

		make_table_element(&table[i], write0 , polycount, NULL, TESS_POLYGON_QUAD);
		tesselate_weight_polygon(write0, write1, i);
		temp = write1;
		write1 = write0;
		write0 = temp;
	}
	tess_level--;

	for(base_level = 1 ; base_level <= tess_level ; base_level++)
	{
		global_tess_table_quad[base_level] = malloc(sizeof(PTessTableElement) * (base_level + 1) * (base_level + 1) * (base_level + 1) * (base_level + 1));
		for(edge[0] = 0 ; edge[0] <= base_level ; edge[0]++)
		{
			for(edge[1] = 0 ; edge[1] <= base_level ; edge[1]++)
			{
				for(edge[2] = 0 ; edge[2] <= base_level ; edge[2]++)
				{
					for(edge[3] = 0 ; edge[3] <= base_level ; edge[3]++)
					{
						polycount = init_weight_quad(temp, edge[0], edge[1], edge[2], edge[3]);
						make_table_element(&global_tess_table_quad[base_level][ edge[0] + (edge[1] * (base_level + 1))  + (edge[2] * (base_level + 1) * (base_level + 1) ) + (edge[3] * (base_level + 1) * (base_level + 1) * (base_level + 1))], temp , polycount,  &table[base_level], TESS_POLYGON_QUAD);
					}
				}
			}
		}
	}
	free(table);
	free(write0);
	free(write1);
	free(write2);
}


PTessTableElement *get_dynamic_table_tri(uint base_level, uint *edge)
{
	return &global_tess_table_tri[base_level][edge[0] + (edge[1] * (base_level + 1))  + (edge[2] * (base_level + 1) * (base_level + 1) )];
}


PTessTableElement *get_dynamic_table_quad(uint base_level, uint *edge)
{
	return &global_tess_table_quad[base_level][edge[0] + (edge[1] * (base_level + 1))  + (edge[2] * (base_level + 1) * (base_level + 1))+ (edge[3] * (base_level + 1) * (base_level + 1) * (base_level + 1))];
}
