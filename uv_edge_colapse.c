#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"
#include "seduce.h"

#include "uv.h"
#include "uv_geometry.h"

#define VERTEX_MIN_MAX_DIST 0.0001

typedef struct{
	uint poly[2];
	uint edge[2];
}UVEdge;


struct{
	uint	node_id;
	uint	edge_count;
	uint	edge_allocated;
	UVEdge	*edges;
	uint	polygon;
}UVEdgeGlobal;

void uve_init()
{
	UVEdgeGlobal.node_id = 0;
	UVEdgeGlobal.edge_count = 0;
	UVEdgeGlobal.edge_allocated = 64;
	UVEdgeGlobal.edges = malloc((sizeof *UVEdgeGlobal.edges) * UVEdgeGlobal.edge_allocated);
	UVEdgeGlobal.polygon = 0;
}



void uve_edge_dust(egreal *dist, uint poly1, uint poly2, uint edge1, uint edge2)
{
	egreal uv[8], uv2[8];
	uint sides[2];
	uvg_get_uve(uv, poly1);
	uvg_get_uve(uv2, poly2);
	sides[0] = 2 * uvg_get_sides(poly1);
	sides[1] = 2 * uvg_get_sides(poly2);
	edge1 *= 2;
	edge2 *= 2;
	dist[0] = (uv[edge1] - uv2[(edge2 + 2) % sides[1]]) * (uv[edge1] - uv2[(edge2 + 2) % sides[1]]) +
				(uv[edge1 + 1] - uv2[(edge2 + 2) % sides[1] + 1]) * (uv[edge1 + 1] - uv2[(edge2 + 2) % sides[1] + 1]);

	dist[1] = (uv[(edge1 + 2) % sides[0]] - uv2[edge2]) * (uv[(edge1 + 2) % sides[0]] - uv2[edge2]) +
				(uv[(edge1 + 2) % sides[0] + 1] - uv2[edge2 + 1]) * (uv[(edge1 + 2) % sides[0] + 1] - uv2[edge2 + 1]);
}

void uve_update_edge(uint polygon, uint edge)
{
	uint *ref, id, i, j, sides[2];
	egreal dist[2];
	ref = uvg_get_ref();
	sides[0] = uvg_get_sides(polygon);
	for(id = uvg_get_next_polygon(0); id != -1; id = uvg_get_next_polygon(id + 1))
	{
		sides[1] = uvg_get_sides(id);
		for(i = 0; i < sides[1]; i++)
		{
			if(ref[id * 4 + i] == ref[polygon * 4 + (edge + 1) % sides[0]] && ref[id * 4 + (i + 1) % sides[1]] == ref[polygon * 4 + edge])
			{
				uve_edge_dust(dist, id, polygon, i, edge);
				if((dist[0] > VERTEX_MIN_MAX_DIST && dist[1] < VERTEX_MIN_MAX_DIST) || (dist[0] < VERTEX_MIN_MAX_DIST && dist[1] > VERTEX_MIN_MAX_DIST))
				{
					for(j = 0; j < UVEdgeGlobal.edge_count; j++)
					{					
						if(UVEdgeGlobal.edges[j].poly[1] == id &&
							UVEdgeGlobal.edges[j].poly[0] == polygon &&
							UVEdgeGlobal.edges[j].edge[1] == i &&
							UVEdgeGlobal.edges[j].edge[0] == edge)
							break;
						if(UVEdgeGlobal.edges[j].poly[0] == id &&
							UVEdgeGlobal.edges[j].poly[1] == polygon &&
							UVEdgeGlobal.edges[j].edge[0] == i &&
							UVEdgeGlobal.edges[j].edge[1] == edge)
							break;
					}
					if(j == UVEdgeGlobal.edge_count)
					{
	//					printf("add %u %u %u %u\n", id, polygon, edge, i);
	//					printf("create %u dist = %f %f border %f\n", UVEdgeGlobal.edge_count, dist[0], dist[1], VERTEX_MIN_MAX_DIST);
						UVEdgeGlobal.edges[UVEdgeGlobal.edge_count].poly[0] = polygon;
						UVEdgeGlobal.edges[UVEdgeGlobal.edge_count].poly[1] = id;
						UVEdgeGlobal.edges[UVEdgeGlobal.edge_count].edge[0] = edge;
						UVEdgeGlobal.edges[UVEdgeGlobal.edge_count].edge[1] = i;
						UVEdgeGlobal.edge_count++;
					}
				}
			}
		}
	}
}

void uve_update()
{
	egreal dist[2];
	uint i;
	if(UVEdgeGlobal.node_id != uvg_get_node())
	{
		UVEdgeGlobal.node_id = uvg_get_node();
		UVEdgeGlobal.edge_count = 0;
	}
	UVEdgeGlobal.polygon = uvg_get_next_polygon(UVEdgeGlobal.polygon + 1);
	if(UVEdgeGlobal.polygon == -1)
		UVEdgeGlobal.polygon = uvg_get_next_polygon(0);

	for(i = 0; i < UVEdgeGlobal.edge_count; i++)
	{
		uve_edge_dust(dist, UVEdgeGlobal.edges[i].poly[0], UVEdgeGlobal.edges[i].poly[1], UVEdgeGlobal.edges[i].edge[0], UVEdgeGlobal.edges[i].edge[1]);
//		printf("sub %u %u %u %u\n", UVEdgeGlobal.edges[i].poly[0], UVEdgeGlobal.edges[i].poly[1], UVEdgeGlobal.edges[i].edge[0], UVEdgeGlobal.edges[i].edge[1]);
//		printf("delete %u dist = %f %f border %f\n", i, dist[0], dist[1], VERTEX_MIN_MAX_DIST);
		if((dist[0] > VERTEX_MIN_MAX_DIST && dist[1] > VERTEX_MIN_MAX_DIST) || (dist[0] < VERTEX_MIN_MAX_DIST && dist[1] < VERTEX_MIN_MAX_DIST))
			UVEdgeGlobal.edges[i] = UVEdgeGlobal.edges[--UVEdgeGlobal.edge_count];
	}

	if(UVEdgeGlobal.edge_count < UVEdgeGlobal.edge_allocated)
	if(UVEdgeGlobal.polygon != -1)
		for(i = 0; i < uvg_get_sides(UVEdgeGlobal.polygon); i++)
			uve_update_edge(UVEdgeGlobal.polygon, i);
}


void uve_find_intersection(egreal *output, egreal *e1start, egreal *e1end, egreal *e2start, egreal *e2end)
{
	egreal vec1[2], vec2[2], r;

	sui_draw_2d_line_gl(e1start[0], e1start[1], e1start[0] + (e1start[0] - e1end[0]) * 0.2, e1start[1] + (e1start[1] - e1end[1]) * 0.2, 1, 0.9, 0);
	sui_draw_2d_line_gl(e2start[0], e2start[1], e2start[0] + (e2start[0] - e2end[0]) * 0.2, e2start[1] + (e2start[1] - e2end[1]) * 0.2, 1, 0.5, 0);

	vec1[0] = e2end[0] - e2start[0];
	vec1[1] = e2end[1] - e2start[1];

	r =	sqrt(vec1[0] * vec1[0] + vec1[1] * vec1[1]);

	vec1[0] /= r;
	vec1[1] /= r;

	vec2[0] = e1start[0] - e2start[0];
	vec2[1] = e1start[1] - e2start[1];

	r =	vec1[0] * vec2[0] + vec1[1] * vec2[1];

/*	{
		output[0] = (e2start[0] + vec1[0] * r);
		output[1] = (e2start[1] + vec1[1] * r);
		return;
	}*/

	vec1[0] = (e2start[0] + vec1[0] * r - e1start[0]);
	vec1[1] = (e2start[1] + vec1[1] * r - e1start[1]);

/*	r =	sqrt(vec1[0] * vec1[0] + vec1[1] * vec1[1]);

	vec1[0] /= r;
	vec1[1] /= r;*/

	vec2[0] = e1end[0] - e1start[0];
	vec2[1] = e1end[1] - e1start[1];

	r =	sqrt(vec2[0] * vec2[0] + vec2[1] * vec2[1]);

	vec2[0] /= r;
	vec2[1] /= r;

	r =	vec1[0] * vec2[0] + vec1[1] * vec2[1];

	output[0] = e1start[0] + vec2[0] * r;
	output[1] = e1start[1] + vec2[1] * r;
}




void	sui_draw_edge_colapse(float pos_x, float pos_y, float red, float green, float blue)
{
	static float array[] = {0.018000, 0.000000, 0.011112, 0.001370, 0.011112, 0.001370, 0.005272, 0.005272, 0.005272, 0.005272, 0.001370, 0.011112, 0.001370, 0.011112, -0.000000, 0.018000, -0.000000, 0.024000, 0.007416, 0.022825, 0.007416, 0.022825, 0.014107, 0.019416, 0.014107, 0.019416, 0.019416, 0.014107, 0.019416, 0.014107, 0.022825, 0.007416, 0.022825, 0.007416, 0.024000, 0.000000, -0.000000, -0.024000, -0.007416, -0.022825, -0.007416, -0.022825, -0.014107, -0.019416, -0.014107, -0.019416, -0.019416, -0.014107, -0.019416, -0.014107, -0.022825, -0.007416, -0.022825, -0.007416, -0.024000, 0.000000, -0.000000, 0.018000, -0.006888, 0.016630, -0.006888, 0.016630, -0.012728, 0.012728, -0.012728, 0.012728, -0.016630, 0.006888, -0.016630, 0.006888, -0.018000, 0.000000, -0.018000, 0.000000, -0.011112, -0.001370, -0.011112, -0.001370, -0.005272, -0.005272, -0.005272, -0.005272, -0.001370, -0.011112, -0.001370, -0.011112, -0.000000, -0.018000, -0.000000, -0.018000, 0.006888, -0.016630, 0.006888, -0.016630, 0.012728, -0.012728, 0.012728, -0.012728, 0.016630, -0.006888, 0.016630, -0.006888, 0.018000, 0.000000, -0.000000, 0.024000, -0.000000, 0.018000, 0.024000, 0.000000, 0.018000, 0.000000, -0.000000, -0.018000, -0.000000, -0.024000, -0.018000, 0.000000, -0.024000, 0.000000};
	static float t = 0;
	glPushMatrix();
//	glRotatef(t++, 0, 0, 1);
	glTranslatef(pos_x, pos_y, 0);
	sui_draw_gl(GL_LINES, array, 60, 2, red, green, blue);
	glPopMatrix();
}

void uve_draw()
{
	egreal uv[8], uv2[8], pos[2];
	UVEdge *e;
	uint i, v[4];

	glPushMatrix();
	glTranslatef(0, 0, -1);
	for(i = 0; i < UVEdgeGlobal.edge_count; i++)
	{
		e = &UVEdgeGlobal.edges[i];
		uvg_get_uve(uv, e->poly[0]);
		uvg_get_uve(uv2, e->poly[1]);
		v[0] = e->edge[0] * 2;
		v[1] = ((e->edge[0] + 1) % uvg_get_sides(e->poly[0])) * 2;
		v[2] = e->edge[1] * 2;
		v[3] = ((e->edge[1] + 1) % uvg_get_sides(e->poly[1])) * 2;

		pos[0] = uv_get_view_x((uv[v[0]] + uv[v[1]] + uv2[v[2]] + uv2[v[3]]) * 0.25);
		pos[1] = uv_get_view_y((uv[v[0] + 1] + uv[v[1] + 1] + uv2[v[2] + 1] + uv2[v[3] + 1]) * 0.25);

		sui_draw_2d_line_gl(uv_get_view_x((uv[v[0]] + uv[v[1]]) * 0.46 + (uv2[v[2]] + uv2[v[3]]) * 0.04), 
					uv_get_view_y((uv[v[0] + 1] + uv[v[1] + 1]) * 0.46 + (uv2[v[2] + 1] + uv2[v[3] + 1]) * 0.04), 
					uv_get_view_x((uv2[v[2]] + uv2[v[3]]) * 0.1 + (uv[v[0]] + uv[v[1]]) * 0.4), 
					uv_get_view_y((uv2[v[2] + 1] + uv2[v[3] + 1]) * 0.1 + (uv[v[0] + 1] + uv[v[1] + 1]) * 0.4), 0.3, 0.1, 0);

		sui_draw_2d_line_gl(uv_get_view_x((uv[v[0]] + uv[v[1]]) * 0.1 + (uv2[v[2]] + uv2[v[3]]) * 0.4), 
					uv_get_view_y((uv[v[0] + 1] + uv[v[1] + 1]) * 0.1 + (uv2[v[2] + 1] + uv2[v[3] + 1]) * 0.4), 
					uv_get_view_x((uv2[v[2]] + uv2[v[3]]) * 0.46 + (uv[v[0]] + uv[v[1]]) * 0.04), 
					uv_get_view_y((uv2[v[2] + 1] + uv2[v[3] + 1]) * 0.46 + (uv[v[0] + 1] + uv[v[1] + 1]) * 0.04), 0.3, 0.1, 0);


		sui_draw_edge_colapse(pos[0], pos[1], 0.6, 0.2, 0);
	}
	glPopMatrix();
}

boolean uve_test(float pointer_x, float pointer_y)
{
	egreal uv[8], uv2[8], pos[2];
	boolean output = FALSE;
	UVEdge *e;
	uint i, v[4];
	for(i = 0; i < UVEdgeGlobal.edge_count; i++)
	{
		e = &UVEdgeGlobal.edges[i];
		uvg_get_uve(uv, e->poly[0]);
		uvg_get_uve(uv2, e->poly[1]);
		v[0] = e->edge[0] * 2;
		v[1] = ((e->edge[0] + 1) % uvg_get_sides(e->poly[0])) * 2;
		v[2] = e->edge[1] * 2;
		v[3] = ((e->edge[1] + 1) % uvg_get_sides(e->poly[1])) * 2;

		pos[0] = uv_get_view_x((uv[v[0]] + uv[v[1]] + uv2[v[2]] + uv2[v[3]]) * 0.25) - pointer_x;
		pos[1] = uv_get_view_y((uv[v[0] + 1] + uv[v[1] + 1] + uv2[v[2] + 1] + uv2[v[3] + 1]) * 0.25) - pointer_y;

		if(pos[0] * pos[0] + pos[1] * pos[1] < 0.02 * 0.02)
		{
			egreal pos2[2];
			uve_find_intersection(pos, &uv[e->edge[0] * 2],
									&uv[((e->edge[0] + uvg_get_sides(e->poly[0]) - 1) % uvg_get_sides(e->poly[0])) * 2],
								&uv2[((e->edge[1] + 1) % uvg_get_sides(e->poly[1])) * 2],
									&uv2[((e->edge[1] + 2) % uvg_get_sides(e->poly[1])) * 2]);
			
			uv[v[0]] = uv2[v[3]] = pos[0];
			uv[v[0] + 1] = uv2[v[3] + 1] = pos[1];

			uve_find_intersection(pos, &uv2[e->edge[1] * 2],
									&uv2[((e->edge[1] + uvg_get_sides(e->poly[1]) - 1) % uvg_get_sides(e->poly[1])) * 2],
								&uv[((e->edge[0] + 1) % uvg_get_sides(e->poly[0])) * 2],
									&uv[((e->edge[0] + 2) % uvg_get_sides(e->poly[0])) * 2]);

			uv[v[1]] = uv2[v[2]] = pos[0];
			uv[v[1] + 1] = uv2[v[2] + 1] = pos[1];
			uvg_set_all_corner_uv(e->poly[0], uv[0], uv[1], uv[2], uv[3], uv[4], uv[5], uv[6], uv[7]);
			uvg_set_all_corner_uv(e->poly[1], uv2[0], uv2[1], uv2[2], uv2[3], uv2[4], uv2[5], uv2[6], uv2[7]);
			return TRUE;
		}
	}
	return FALSE;
}