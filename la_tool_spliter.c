#include "la_includes.h"

#include "st_matrix_operations.h"
#include "la_geometry_undo.h"
#include "la_projection.h"
#include "la_particle_fx.h"

#define MAX_SPLITS 32
#define MAX_SPLITS_PLUS_TWO 34

struct{
    uint 	division;
    uint 	division_old;
	uint	split_count;
	uint	ref[2][4];
	uint	crease[2][4];
	uint	new_vertex;
	uint	poly_id[2][MAX_SPLITS_PLUS_TWO];
	uint	vertex_id[MAX_SPLITS];
    uint	vertex_old[MAX_SPLITS];
    uint	edge[2];
	uint	vertex_create;
	uint	poly_create;
	double	pos[3];
	boolean	first;
	boolean	free;
}GlobalSplitData;


void splitt_edge_possition(uint edge0, uint edge1, float x, float y)
{
	double other[3], output[3], a[3], b[3], r;
	udg_get_vertex_pos(output, edge0);
	udg_get_vertex_pos(other, edge1);
	p_get_projection_screen(a, output[0], output[1], output[2]);
	p_get_projection_screen(b, other[0], other[1], other[2]);
	r = sqrt((a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]));
	r = (((a[0] - b[0]) / r) * (a[0] + x) + ((a[1] - b[1]) / r) * (a[1] + y)) / r;
	GlobalSplitData.pos[0] = other[0] * r + output[0] * (1 - r);
	GlobalSplitData.pos[1] = other[1] * r + output[1] * (1 - r);
	GlobalSplitData.pos[2] = other[2] * r + output[2] * (1 - r);
}

void create_vertex_edge(uint edge0, uint edge1, float x, float y, boolean add, boolean subtract)
{
	uint i;
    if(add)
    {
		GlobalSplitData.vertex_id[GlobalSplitData.division - 0] = GlobalSplitData.vertex_id[GlobalSplitData.division - 1];
		GlobalSplitData.vertex_id[GlobalSplitData.division - 1] = GlobalSplitData.vertex_create = udg_find_empty_slot_vertex();
    }
	else if(subtract)
    {
		udg_vertex_delete(GlobalSplitData.vertex_id[GlobalSplitData.division]);
		GlobalSplitData.vertex_id[GlobalSplitData.division] = GlobalSplitData.vertex_id[GlobalSplitData.division + 1];
    }
	else
    {
        for(i = 0; i < GlobalSplitData.division || i == 0; i++)
            GlobalSplitData.vertex_id[i] = GlobalSplitData.vertex_create = udg_find_empty_slot_vertex();
    }
    
    if(GlobalSplitData.division == 1 && GlobalSplitData.free)
        udg_vertex_set(GlobalSplitData.vertex_id[0], NULL, GlobalSplitData.pos[0], GlobalSplitData.pos[1], GlobalSplitData.pos[2]);
    else
    {
		double a[3], b[3], temp;
        udg_get_vertex_pos(a, edge0);
        udg_get_vertex_pos(b, edge1);
		for(i = 0; i < GlobalSplitData.division; i++)
		{
			temp =  (i + 1) / (double)(GlobalSplitData.division + 1);
			udg_vertex_set(GlobalSplitData.vertex_id[i], NULL, a[0] * temp + b[0] * (1 - temp), a[1] * temp + b[1] * (1 - temp), a[2] * temp + b[2] * (1 - temp));
		}
    }
}

void polygon_split_end(uint *ref, uint *crease, uint *poly_id, /*uint vertex_count,*/ double *vertex_pos, boolean turn)
{
	double vertex[3], r;
	uint i;
	if(ref[3] != -1)
	{
		if(GlobalSplitData.division < 2)
		{
			udg_get_vertex_pos(vertex, ref[3]);
			r = (vertex[0] - vertex_pos[0]) * (vertex[0] - vertex_pos[0]) + (vertex[1] - vertex_pos[1]) * (vertex[1] - vertex_pos[1]) + (vertex[2] - vertex_pos[2]) * (vertex[2] - vertex_pos[2]);
			udg_get_vertex_pos(vertex, ref[2]);
			if(r < (vertex[0] - vertex_pos[0]) * (vertex[0] - vertex_pos[0]) + (vertex[1] - vertex_pos[1]) * (vertex[1] - vertex_pos[1]) + (vertex[2] - vertex_pos[2]) * (vertex[2] - vertex_pos[2]))
			{
				udg_polygon_set(poly_id[0], ref[0], GlobalSplitData.vertex_id[0], ref[3], -1);
				udg_crease_set(poly_id[0], crease[0], 0, crease[3], 0);
				udg_polygon_set(poly_id[1], GlobalSplitData.vertex_id[0], ref[1], ref[2], ref[3]);
				udg_crease_set(poly_id[1], crease[0], crease[1], crease[2], 0);
			}else
			{
				udg_polygon_set(poly_id[0], ref[0], GlobalSplitData.vertex_id[0], ref[2], ref[3]);
				udg_crease_set(poly_id[0], crease[0], 0, crease[2], crease[3]);
				udg_polygon_set(poly_id[1], GlobalSplitData.vertex_id[0], ref[1], ref[2], -1);
				udg_crease_set(poly_id[1], crease[0], crease[1], 0, 0);
			}
		}else
		{
			if(turn)
			{
				udg_polygon_set(poly_id[0], GlobalSplitData.vertex_id[0], ref[1],  ref[2], -1);
				udg_crease_set(poly_id[0], crease[0], 0, crease[2], 0);
				for(i = 1; i < GlobalSplitData.division / 2; i++)
					udg_polygon_set(poly_id[i], GlobalSplitData.vertex_id[i], GlobalSplitData.vertex_id[i - 1], ref[2], -1);
				udg_polygon_set(poly_id[i], GlobalSplitData.vertex_id[i], GlobalSplitData.vertex_id[i - 1], ref[2], ref[3]);
				for(i++; i < GlobalSplitData.division; i++)
					udg_polygon_set(poly_id[i], GlobalSplitData.vertex_id[i], GlobalSplitData.vertex_id[i - 1], ref[3], -1);
				udg_polygon_set(poly_id[i], ref[0], GlobalSplitData.vertex_id[i - 1], ref[3], -1);
				udg_crease_set(poly_id[i], crease[0], crease[1], 0, 0);
			}else
			{
				udg_polygon_set(poly_id[0], ref[0], GlobalSplitData.vertex_id[0], ref[3], -1);
				udg_crease_set(poly_id[0], crease[0], 0, crease[2], 0);
				for(i = 1; i < GlobalSplitData.division / 2; i++)
					udg_polygon_set(poly_id[i], GlobalSplitData.vertex_id[i - 1], GlobalSplitData.vertex_id[i], ref[3], -1);
				udg_polygon_set(poly_id[i], GlobalSplitData.vertex_id[i - 1], GlobalSplitData.vertex_id[i], ref[2], ref[3]);
				for(i++; i < GlobalSplitData.division; i++)
					udg_polygon_set(poly_id[i], GlobalSplitData.vertex_id[i - 1], GlobalSplitData.vertex_id[i], ref[2], -1);
				udg_polygon_set(poly_id[i], ref[2], GlobalSplitData.vertex_id[i - 1], ref[1], -1);
				udg_crease_set(poly_id[i], crease[0], crease[1], 0, 0);
			}
		}
	}else
	{
		if(turn)
		{
			udg_polygon_set(poly_id[0], GlobalSplitData.vertex_id[0], ref[1],  ref[2], -1);
			udg_crease_set(poly_id[0], crease[0], 0, crease[2], 0);
			for(i = 1; i < GlobalSplitData.division; i++)
				udg_polygon_set(poly_id[i], GlobalSplitData.vertex_id[i], GlobalSplitData.vertex_id[i - 1], ref[2], -1);
			udg_polygon_set(poly_id[i], ref[0], GlobalSplitData.vertex_id[i - 1], ref[2], -1);
			udg_crease_set(poly_id[i], crease[0], crease[1], 0, 0);
		}else
		{
			udg_polygon_set(poly_id[0], ref[0], GlobalSplitData.vertex_id[0], ref[2], -1);
			udg_crease_set(poly_id[0], crease[0], 0, crease[2], 0);
			for(i = 1; i < GlobalSplitData.division; i++)
				udg_polygon_set(poly_id[i], GlobalSplitData.vertex_id[i - 1], GlobalSplitData.vertex_id[i], ref[2], -1);
			udg_polygon_set(poly_id[i], GlobalSplitData.vertex_id[i - 1], ref[1], ref[2], -1);
			udg_crease_set(poly_id[i], crease[0], crease[1], 0, 0);
		}
	}
}

void find_edge_spliter(uint edge0, uint edge1, float x, float y)
{
	uint i, j, ref_count, *ref, vertex_count, *crease;
	double vertex[3];
	udg_get_geometry(&vertex_count, &ref_count, NULL, &ref, &crease);
	ref_count *= 4;
	for(i = 0; i < ref_count; i++)
	{
		if(ref[i] == edge0)
		{
			uint temp, next, poly = 4;
			temp = (i / 4) * 4;
			if(ref[temp + 3] > vertex_count)
				poly = 3;
			next = ref[temp + (i + 1 - temp) % poly];
			if(next == edge1)
			{
				GlobalSplitData.ref[GlobalSplitData.split_count][0] = ref[temp + (i - temp) % poly];
				GlobalSplitData.ref[GlobalSplitData.split_count][1] = ref[temp + (i + 1 - temp) % poly];
				GlobalSplitData.ref[GlobalSplitData.split_count][2] = ref[temp + (i + 2 - temp) % poly];
				if(poly == 3)
					GlobalSplitData.ref[GlobalSplitData.split_count][3] = -1;
				else
					GlobalSplitData.ref[GlobalSplitData.split_count][3] = ref[temp + (i + 3 - temp) % poly];
				if(crease != NULL)
				{
					GlobalSplitData.crease[GlobalSplitData.split_count][0] = crease[temp + (i - temp) % poly];
					GlobalSplitData.crease[GlobalSplitData.split_count][1] = crease[temp + (i + 1 - temp) % poly];
					GlobalSplitData.crease[GlobalSplitData.split_count][2] = crease[temp + (i + 2 - temp) % poly];
					GlobalSplitData.crease[GlobalSplitData.split_count][3] = crease[temp + (i + 3 - temp) % poly];
				}else for(j = 0; j < 4; j++)
					GlobalSplitData.crease[GlobalSplitData.split_count][j] = 0;
				GlobalSplitData.poly_id[GlobalSplitData.split_count][0] = i / 4;
				for(j = 0; j < GlobalSplitData.division || j == 0; j++)
					GlobalSplitData.poly_id[GlobalSplitData.split_count][j + 1] = GlobalSplitData.poly_create = udg_find_empty_slot_polygon();
				polygon_split_end(&GlobalSplitData.ref[GlobalSplitData.split_count][0], &GlobalSplitData.crease[GlobalSplitData.split_count][0], &GlobalSplitData.poly_id[GlobalSplitData.split_count][0], vertex, GlobalSplitData.split_count == 0);
				GlobalSplitData.split_count++;
			}
		}
	}
}

void la_t_edge_spliter_start(BInputState *input, uint *edge)
{
	GlobalSplitData.division = 1;
	GlobalSplitData.division_old = 0;
	GlobalSplitData.split_count = 0;
	GlobalSplitData.vertex_create = 0;
	GlobalSplitData.poly_create = 0;
	GlobalSplitData.edge[0] = edge[0];
	GlobalSplitData.edge[1] = edge[1];
	GlobalSplitData.first = TRUE;
	GlobalSplitData.free = TRUE;
	splitt_edge_possition(edge[0], edge[1], input->pointer_x, input->pointer_y);
	create_vertex_edge(edge[0], edge[1], input->pointer_x, input->pointer_y, FALSE, FALSE);
//	GlobalSplitData.vertex_id[0] = GlobalSplitData.vertex_create = udg_find_empty_slot_id(VN_G_FRAGMENT_VERTEX, GlobalSplitData.vertex_create + 1);
	find_edge_spliter(edge[0], edge[1], input->pointer_x, input->pointer_y);
	/*if( DO THE TURN TEST) */
	find_edge_spliter(edge[1], edge[0], input->pointer_x, input->pointer_y);

	if(GlobalSplitData.poly_create == 0)
	{
		uint *ref, ref_count, i;
		ref = udg_get_edge_data(&ref_count);
		for(i = 0; i < ref_count; i++)
		{
			if((ref[i * 2] == edge[0] && ref[i * 2 + 1] == edge[1]) || (ref[i * 2] == edge[1] && ref[i * 2 + 1] == edge[0]))
			{
				udg_create_edge(GlobalSplitData.vertex_id[0], ref[i * 2 + 1]);
				udg_create_edge(GlobalSplitData.vertex_id[0], ref[i * 2]);
				udg_destroy_edge(i);
				return;
			}
		}
	}
}
void final_edge_split(uint poly, uint edge)
{
	uint poly_id;
	if(GlobalSplitData.ref[poly][3] != -1)
	{
		if(edge == 0)
		{
			udg_polygon_set(GlobalSplitData.poly_id[poly][0], GlobalSplitData.ref[poly][2], GlobalSplitData.ref[poly][3], GlobalSplitData.ref[poly][0], -1);
			GlobalSplitData.poly_create = udg_find_empty_slot_polygon();
			udg_polygon_set(GlobalSplitData.poly_create, GlobalSplitData.vertex_old[0],  GlobalSplitData.ref[poly][1], GlobalSplitData.vertex_id[0], -1);	
			GlobalSplitData.vertex_id[GlobalSplitData.division] = GlobalSplitData.ref[poly][2];
		}
		if(edge == 1)
		{
			udg_polygon_set(GlobalSplitData.poly_id[poly][0], GlobalSplitData.vertex_old[0],  GlobalSplitData.ref[poly][1], GlobalSplitData.ref[poly][2], GlobalSplitData.vertex_id[0]);
/*			udg_crease_set(GlobalSplitData.poly_id[poly][0], GlobalSplitData.crease[poly][0], GlobalSplitData.crease[poly][1], GlobalSplitData.crease[poly][2], 0);
*/			GlobalSplitData.vertex_id[GlobalSplitData.division] = GlobalSplitData.ref[poly][3];
		}
		if(edge == 2)
		{
			udg_polygon_set(GlobalSplitData.poly_id[poly][0], GlobalSplitData.vertex_old[0],  GlobalSplitData.ref[poly][1], GlobalSplitData.ref[poly][3], GlobalSplitData.vertex_id[0]);
			GlobalSplitData.poly_create = udg_find_empty_slot_polygon();
			udg_polygon_set(GlobalSplitData.poly_create, GlobalSplitData.ref[poly][1],  GlobalSplitData.ref[poly][2], GlobalSplitData.ref[poly][3], -1);
			GlobalSplitData.vertex_id[GlobalSplitData.division] = -1;
		}
	}else
	{
		if(edge == 0)
		{
	//		udg_polygon_set(GlobalSplitData.poly_id[poly][0], GlobalSplitData.ref[poly][2], GlobalSplitData.ref[poly][3], GlobalSplitData.ref[poly][0], -1);
	//		GlobalSplitData.poly_create = udg_find_empty_slot_id(VN_G_FRAGMENT_POLYGON, GlobalSplitData.poly_create + 1);
			udg_polygon_set(GlobalSplitData.poly_id[poly][0], GlobalSplitData.vertex_old[0],  GlobalSplitData.ref[poly][1], GlobalSplitData.vertex_id[0], -1);	
			GlobalSplitData.vertex_id[GlobalSplitData.division] = GlobalSplitData.ref[poly][2];
		}
		if(edge == 1)
		{
	//		udg_polygon_set(GlobalSplitData.poly_id[poly][0], GlobalSplitData.vertex_old[0],  GlobalSplitData.ref[poly][1], GlobalSplitData.ref[poly][3], GlobalSplitData.vertex_id[0]);
	//		GlobalSplitData.poly_create = udg_find_empty_slot_id(VN_G_FRAGMENT_POLYGON, GlobalSplitData.poly_create + 1);
			udg_polygon_set(GlobalSplitData.poly_id[poly][0], GlobalSplitData.ref[poly][1],  GlobalSplitData.ref[poly][2], GlobalSplitData.vertex_id[0], GlobalSplitData.vertex_old[0]);
			GlobalSplitData.vertex_id[GlobalSplitData.division] = -1;
		}
	}
}

void final_middle_split(uint poly)
{
	uint i, last = 0;
//	udg_polygon_set(GlobalSplitData.poly_id[poly][1], GlobalSplitData.vertex_old[0], GlobalSplitData.vertex_old[1],  GlobalSplitData.vertex_id[GlobalSplitData.division - 1],GlobalSplitData.vertex_id[GlobalSplitData.division]);
	if(GlobalSplitData.division > GlobalSplitData.division_old)
	{
		for(i = 1; i < GlobalSplitData.division + 1; i++)
		{
			if(last != (i * GlobalSplitData.division_old) / GlobalSplitData.division)
			{
				udg_polygon_set(GlobalSplitData.poly_id[poly][i], GlobalSplitData.vertex_old[last + 1], GlobalSplitData.vertex_old[last],  GlobalSplitData.vertex_id[i - 1], GlobalSplitData.vertex_id[i]);
				last++;
			}else
				udg_polygon_set(GlobalSplitData.poly_id[poly][i], GlobalSplitData.vertex_old[last], GlobalSplitData.vertex_id[i - 1], GlobalSplitData.vertex_id[i], -1);
		}
	}else
	{
		for(i = 1; i < GlobalSplitData.division_old + 1; i++)
		{
			if(last != (i * GlobalSplitData.division) / GlobalSplitData.division_old)
			{
				udg_polygon_set(GlobalSplitData.poly_id[poly][i],  GlobalSplitData.vertex_old[i], GlobalSplitData.vertex_old[i - 1], GlobalSplitData.vertex_id[last], GlobalSplitData.vertex_id[last + 1]);
				last++;
			}else
				udg_polygon_set(GlobalSplitData.poly_id[poly][i], GlobalSplitData.vertex_id[last], GlobalSplitData.vertex_old[i], GlobalSplitData.vertex_old[i - 1], -1);
		}
	}
}

void la_t_edge_spliter(BInputState *input)
{
	uint i, j, k, j2, new_vertex, poly; 
	double vertex[4][3], start[2], mouse[2], a, b;
	la_pfx_create_spark(GlobalSplitData.pos);

    if(input->mouse_button[2] == FALSE && input->last_mouse_button[2] == TRUE)
    {
		if(GlobalSplitData.division == 1)
		{
			GlobalSplitData.free = TRUE;
			udg_vertex_set(GlobalSplitData.vertex_id[0], NULL, GlobalSplitData.pos[0], GlobalSplitData.pos[1], GlobalSplitData.pos[2]);
		}
		else
		{
			udg_polygon_delete(GlobalSplitData.poly_id[0][GlobalSplitData.division]);
			GlobalSplitData.division--;
			create_vertex_edge(GlobalSplitData.edge[0], GlobalSplitData.edge[1], input->pointer_x, input->pointer_y, FALSE, TRUE);
			if(GlobalSplitData.split_count > 0)
				polygon_split_end(&GlobalSplitData.ref[0][0], &GlobalSplitData.crease[0][0], &GlobalSplitData.poly_id[0][0], GlobalSplitData.pos, TRUE);
			if(GlobalSplitData.split_count == 2)
			{
				udg_polygon_delete(GlobalSplitData.poly_id[1][GlobalSplitData.division - 1]);
				polygon_split_end(&GlobalSplitData.ref[1][0], &GlobalSplitData.crease[1][0], &GlobalSplitData.poly_id[1][0], GlobalSplitData.pos, FALSE);
			}else if(GlobalSplitData.first == FALSE)
			{
				if(GlobalSplitData.division >= GlobalSplitData.division_old)
					udg_polygon_delete(GlobalSplitData.poly_id[1][GlobalSplitData.division + 1]);
				final_middle_split(1);
			}
		}
	}
	if(input->mouse_button[1] == FALSE && input->last_mouse_button[1] == TRUE && GlobalSplitData.division != MAX_SPLITS)
    {
		GlobalSplitData.division++;
		create_vertex_edge(GlobalSplitData.edge[0], GlobalSplitData.edge[1], input->pointer_x, input->pointer_y, TRUE, FALSE);
		if(GlobalSplitData.split_count > 0)
		{
			GlobalSplitData.poly_id[0][GlobalSplitData.division] = GlobalSplitData.poly_create = udg_find_empty_slot_polygon();
			polygon_split_end(&GlobalSplitData.ref[0][0], &GlobalSplitData.crease[0][0], &GlobalSplitData.poly_id[0][0], GlobalSplitData.pos, TRUE);
		}
		if(GlobalSplitData.split_count == 2)
		{
			GlobalSplitData.poly_id[1][GlobalSplitData.division] = GlobalSplitData.poly_create = udg_find_empty_slot_polygon();
			polygon_split_end(&GlobalSplitData.ref[1][0], &GlobalSplitData.crease[1][0], &GlobalSplitData.poly_id[1][0], GlobalSplitData.pos, FALSE);
		}else if(GlobalSplitData.first == FALSE)
		{
			if(GlobalSplitData.division > GlobalSplitData.division_old)
				GlobalSplitData.poly_id[1][GlobalSplitData.division] = GlobalSplitData.poly_create = udg_find_empty_slot_polygon();
			final_middle_split(1);
		}
	}

	for(i = 0; i < GlobalSplitData.split_count; i++)
	{
		udg_get_vertex_pos(&vertex[0][0], GlobalSplitData.ref[i][0]);
		p_get_projection_screen(&vertex[0][0], vertex[0][0], vertex[0][1], vertex[0][2]);
		udg_get_vertex_pos(&vertex[1][0], GlobalSplitData.ref[i][1]);
		p_get_projection_screen(&vertex[1][0], vertex[1][0], vertex[1][1], vertex[1][2]);
		udg_get_vertex_pos(&vertex[2][0], GlobalSplitData.ref[i][2]);
		p_get_projection_screen(&vertex[2][0], vertex[2][0], vertex[2][1], vertex[2][2]);
		if(GlobalSplitData.ref[i][3] != -1)
		{
			udg_get_vertex_pos(&vertex[3][0], GlobalSplitData.ref[i][3]);
			p_get_projection_screen(&vertex[3][0], vertex[3][0], vertex[3][1], vertex[3][2]);
			poly = 4;
		}else
			poly = 3;
		start[0] = (vertex[0][0] + vertex[1][0]) * 0.5;
		start[1] = (vertex[0][1] + vertex[1][1]) * 0.5;
		mouse[0] = -input->pointer_x;
		mouse[1] = -input->pointer_y;
		for(j = 1; j < poly; j++)
		{
			j2 = (j + 1) % poly;
			a = (vertex[j][0] - vertex[j2][0]) * (start[1] - vertex[j2][1]) + (vertex[j][1] - vertex[j2][1]) * (vertex[j2][0] - start[0]);
			b = (vertex[j][0] - vertex[j2][0]) * (mouse[1] - vertex[j2][1]) + (vertex[j][1] - vertex[j2][1]) * (vertex[j2][0] - mouse[0]);
			if((a > 0 && b < 0) || (a < 0 && b > 0))
			{
				a = (start[0] - mouse[0]) * (vertex[j][1] - mouse[1]) + (start[1] - mouse[1]) * (mouse[0] - vertex[j][0]);
				b = (start[0] - mouse[0]) * (vertex[j2][1] - mouse[1]) + (start[1] - mouse[1]) * (mouse[0] - vertex[j2][0]);
				if((a > 0 && b < 0) || (a < 0 && b > 0))
				{
					GlobalSplitData.split_count = 0;
					GlobalSplitData.first = FALSE;
					if(i == 0)
						for(k = 0; k < GlobalSplitData.division; k++)
							GlobalSplitData.vertex_old[k] = GlobalSplitData.vertex_id[k];
					else
						for(k = 0; k < GlobalSplitData.division; k++)
							GlobalSplitData.vertex_old[k] = GlobalSplitData.vertex_id[GlobalSplitData.division - 1 - k];
					GlobalSplitData.vertex_old[k] = GlobalSplitData.ref[i][0];
					splitt_edge_possition(GlobalSplitData.ref[i][j2], GlobalSplitData.ref[i][j], input->pointer_x, input->pointer_y);
					create_vertex_edge(GlobalSplitData.ref[i][j2], GlobalSplitData.ref[i][j], mouse[0], mouse[1], FALSE, FALSE);
			//		GlobalSplitData.vertex_id[k] = GlobalSplitData.ref[i][0];
					GlobalSplitData.edge[0] = GlobalSplitData.ref[i][j2];
					GlobalSplitData.edge[1] = GlobalSplitData.ref[i][j];
					if(i == 0)
					{
						for (k = 0; k < GlobalSplitData.division + 1; k++)
							GlobalSplitData.poly_id[1][k] = GlobalSplitData.poly_id[0][k];
						for (k = 0; k < 4; k++)
							GlobalSplitData.ref[1][k] = GlobalSplitData.ref[0][k];
					}
					GlobalSplitData.division_old = GlobalSplitData.division;
					
					find_edge_spliter(GlobalSplitData.ref[i][j2], GlobalSplitData.ref[i][j], input->pointer_x, input->pointer_y);
					
					final_edge_split(1, j - 1);
					final_middle_split(1);
				}
			}
		}
	}
}
