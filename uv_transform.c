#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"
#include "seduce.h"
#include "uv.h"
#include "uv_geometry.h"
#include <math.h>

void uv_trans_move_corner(BInputState *input, uint poly_id, uint corner)
{
	uint vertex, *ref, length, i;
	egreal *select, pos[2];

	select = uvg_get_corner_select(poly_id);
	if(select[corner] < 0.01)
		return;
	pos[0] = uv_get_pos_x(input->pointer_x);
	pos[1] = uv_get_pos_y(input->pointer_y);
	if(input->mouse_button[1] == TRUE)
	{
		uv_find_closest_corner(pos, pos[0], pos[1], FALSE);
	}
	ref = uvg_get_ref();
	vertex = ref[poly_id * 4 + corner];
	length = uvg_get_polygon_length() * 4;


	for(i = 0; i < length; i++)
	{
		if(uvg_get_polygon_select(i / 4))
		{
			if(ref[i] == vertex && uvg_get_next_polygon(i / 4) == i / 4 && uvg_get_polygon_select(i / 4))
			{
				select = uvg_get_corner_select(i / 4);
				if(uvg_get_polygon_select(i / 4) && select[i % 4] > 0.01)
					uvg_set_one_corner_uv(i / 4, i % 4, pos[0], pos[1]);
			}
		}
	}
}

void uv_trans_move_edge(BInputState *input, uint poly_id, uint corner)
{
	uint  id, i, found, sides;
	egreal uv[8], uv2[8], temp[2], mid[2], x, y, f, shortest = 10000000;

	if(0.001 > (input->pointer_x - input->click_pointer_x) * (input->pointer_x - input->click_pointer_x) + (input->pointer_y - input->click_pointer_y) * (input->pointer_y - input->click_pointer_y))
		return;
	found = poly_id * 4 + corner; 
	x = uv_get_pos_x(input->pointer_x);
	y = uv_get_pos_y(input->pointer_y);

	for(id = uvg_get_next_polygon(0); id != ~0u; id = uvg_get_next_polygon(id + 1))
	{
		if(id != poly_id)
		{
			uvg_get_uve(uv, id);
			sides = uvg_get_sides(id);
			mid[0] = uv[0];
			mid[1] = uv[1];
			for(i = 1; i < sides; i++)
			{
				mid[0] += uv[i * 2];
				mid[1] += uv[i * 2 + 1];
			}
			mid[0] *= 0.2 / sides;
			mid[1] *= 0.2 / sides;


			for(i = 0; i < sides; i++)
			{
				temp[0] = (uv[i * 2] + uv[((i + 1) % sides) * 2]) * 0.4 + mid[0] - x;
				temp[1] = (uv[i * 2 + 1] + uv[((i + 1) % sides) * 2 + 1]) * 0.4 + mid[1] - y;
				f = temp[0] * temp[0] + temp[1] * temp[1];
				if(f < shortest)
				{
					found = id * 4 + i;
					shortest = f;
				}
			}
		}
	}
	id = found / 4;
	uvg_get_uve(uv, id);
	uvg_get_uve(uv2, poly_id);
	sides = uvg_get_sides(id);
	i = found % 4;

	uv2[corner * 2] = uv[i * 2];
	uv2[corner * 2 + 1] = uv[i * 2 + 1];
	uv2[((corner + uvg_get_sides(poly_id) - 1) % uvg_get_sides(poly_id)) * 2] = uv[((i + 1) % sides) * 2];
	uv2[((corner + uvg_get_sides(poly_id) - 1) % uvg_get_sides(poly_id)) * 2 + 1] = uv[((i + 1) % sides) * 2 + 1];
	uvg_set_all_corner_uv(poly_id, uv2[0], uv2[1], uv2[2], uv2[3], uv2[4], uv2[5], uv2[6], uv2[7]);
}

typedef enum{
	TM_IDLE,
	TM_MOVE,
	TM_TRANSLATE,
	TM_TRANSLATE_X,
	TM_TRANSLATE_Y,
	TM_SCALE,
	TM_SCALE_X,
	TM_SCALE_Y,
	TM_ROTATE
}UVTMode;

struct{
	egreal *locked_uv;
	uint	locked_length;
	egreal	manip_pos[2];
	egreal	manip_start[2];
	float	*ring;
	UVTMode	mode;
	boolean display;
}UVTGlobal;


#define RING_SECTIONS 17
#define DRAW_CLOSE_RANGE 0.05

void uv_trans_init()
{
	uint i;
	UVTGlobal.mode = TM_IDLE;
	UVTGlobal.ring = malloc((sizeof *UVTGlobal.ring) * RING_SECTIONS * 8 * 2);
	for(i = 0; i < RING_SECTIONS; i++)
	{
		sui_draw_set_vec2(UVTGlobal.ring, i * 8 + 0, 1.0 * DRAW_CLOSE_RANGE * sin(((double)i + 0.0) * PI / RING_SECTIONS * 2), 1.0 * DRAW_CLOSE_RANGE * cos(((double)i + 0.0) * PI / RING_SECTIONS * 2));
		sui_draw_set_vec2(UVTGlobal.ring, i * 8 + 1, 1.0 * DRAW_CLOSE_RANGE * sin(((double)i + 0.5) * PI / RING_SECTIONS * 2), 1.0 * DRAW_CLOSE_RANGE * cos(((double)i + 0.5) * PI / RING_SECTIONS * 2));
		sui_draw_set_vec2(UVTGlobal.ring, i * 8 + 2, 1.0 * DRAW_CLOSE_RANGE * sin(((double)i + 0.5) * PI / RING_SECTIONS * 2), 1.0 * DRAW_CLOSE_RANGE * cos(((double)i + 0.5) * PI / RING_SECTIONS * 2));
		sui_draw_set_vec2(UVTGlobal.ring, i * 8 + 3, 0.9 * DRAW_CLOSE_RANGE * sin(((double)i + 0.5) * PI / RING_SECTIONS * 2), 0.9 * DRAW_CLOSE_RANGE * cos(((double)i + 0.5) * PI / RING_SECTIONS * 2));
		sui_draw_set_vec2(UVTGlobal.ring, i * 8 + 4, 0.9 * DRAW_CLOSE_RANGE * sin(((double)i + 0.5) * PI / RING_SECTIONS * 2), 0.9 * DRAW_CLOSE_RANGE * cos(((double)i + 0.5) * PI / RING_SECTIONS * 2));
		sui_draw_set_vec2(UVTGlobal.ring, i * 8 + 5, 0.9 * DRAW_CLOSE_RANGE * sin(((double)i + 1.0) * PI / RING_SECTIONS * 2), 0.9 * DRAW_CLOSE_RANGE * cos(((double)i + 1.0) * PI / RING_SECTIONS * 2));
		sui_draw_set_vec2(UVTGlobal.ring, i * 8 + 6, 0.9 * DRAW_CLOSE_RANGE * sin(((double)i + 1.0) * PI / RING_SECTIONS * 2), 0.9 * DRAW_CLOSE_RANGE * cos(((double)i + 1.0) * PI / RING_SECTIONS * 2));
		sui_draw_set_vec2(UVTGlobal.ring, i * 8 + 7, 1.0 * DRAW_CLOSE_RANGE * sin(((double)i + 1.0) * PI / RING_SECTIONS * 2), 1.0 * DRAW_CLOSE_RANGE * cos(((double)i + 1.0) * PI / RING_SECTIONS * 2));
	}
	UVTGlobal.manip_pos[0] = 0;
	UVTGlobal.manip_pos[1] = 0;
	UVTGlobal.display = FALSE;
}


void uv_trans_lock(void)
{
	uint id;
	UVTGlobal.locked_uv = malloc((sizeof *UVTGlobal.locked_uv) * uvg_get_polygon_length() * 8);
	for(id = uvg_get_next_polygon(0); id != ~0u; id = uvg_get_next_polygon(id + 1))
		uvg_get_uve(&UVTGlobal.locked_uv[id * 8], id);
	UVTGlobal.locked_length = uvg_get_polygon_length();
}

void uv_trans_unlock(void)
{
	free(UVTGlobal.locked_uv);
	UVTGlobal.locked_length = 0;
}

void uv_trans_form(egreal pos_x, egreal pos_y, egreal *matrix)
{
	uint id, i;
	egreal *select;
//	printf("UVTGlobal.locked_length %u\n", UVTGlobal.locked_length);
	for(id = uvg_get_next_polygon(0); id != ~0u && id < UVTGlobal.locked_length; id = uvg_get_next_polygon(id + 1))
	{
		if(uvg_get_polygon_select(id))
		{
			select = uvg_get_corner_select(id);
			for(i = 0; i < uvg_get_sides(id) && select[i] < 0.01; i++);
			if(i < uvg_get_sides(id))
			{
/*				for(i = 0; i < 8; i++)
					printf("%f", UVTGlobal.locked_uv[id * 8 + i]);
				printf("\n");
*/				
				uvg_set_all_corner_uv(id, UVTGlobal.locked_uv[id * 8 + 0] * (1 - select[0]) + select[0] * (pos_x + (UVTGlobal.locked_uv[id * 8 + 0] - UVTGlobal.manip_start[0]) * matrix[0] + (UVTGlobal.locked_uv[id * 8 + 1] - UVTGlobal.manip_start[1]) * matrix[1]),
											UVTGlobal.locked_uv[id * 8 + 1] * (1 - select[0]) + select[0] * (pos_y + (UVTGlobal.locked_uv[id * 8 + 0] - UVTGlobal.manip_start[0]) * matrix[2] + (UVTGlobal.locked_uv[id * 8 + 1] - UVTGlobal.manip_start[1]) * matrix[3]),
											UVTGlobal.locked_uv[id * 8 + 2] * (1 - select[1]) + select[1] * (pos_x + (UVTGlobal.locked_uv[id * 8 + 2] - UVTGlobal.manip_start[0]) * matrix[0] + (UVTGlobal.locked_uv[id * 8 + 3] - UVTGlobal.manip_start[1]) * matrix[1]),
											UVTGlobal.locked_uv[id * 8 + 3] * (1 - select[1]) + select[1] * (pos_y + (UVTGlobal.locked_uv[id * 8 + 2] - UVTGlobal.manip_start[0]) * matrix[2] + (UVTGlobal.locked_uv[id * 8 + 3] - UVTGlobal.manip_start[1]) * matrix[3]),
											UVTGlobal.locked_uv[id * 8 + 4] * (1 - select[2]) + select[2] * (pos_x + (UVTGlobal.locked_uv[id * 8 + 4] - UVTGlobal.manip_start[0]) * matrix[0] + (UVTGlobal.locked_uv[id * 8 + 5] - UVTGlobal.manip_start[1]) * matrix[1]),
											UVTGlobal.locked_uv[id * 8 + 5] * (1 - select[2]) + select[2] * (pos_y + (UVTGlobal.locked_uv[id * 8 + 4] - UVTGlobal.manip_start[0]) * matrix[2] + (UVTGlobal.locked_uv[id * 8 + 5] - UVTGlobal.manip_start[1]) * matrix[3]),
											UVTGlobal.locked_uv[id * 8 + 6] * (1 - select[3]) + select[3] * (pos_x + (UVTGlobal.locked_uv[id * 8 + 6] - UVTGlobal.manip_start[0]) * matrix[0] + (UVTGlobal.locked_uv[id * 8 + 7] - UVTGlobal.manip_start[1]) * matrix[1]),
											UVTGlobal.locked_uv[id * 8 + 7] * (1 - select[3]) + select[3] * (pos_y + (UVTGlobal.locked_uv[id * 8 + 6] - UVTGlobal.manip_start[0]) * matrix[2] + (UVTGlobal.locked_uv[id * 8 + 7] - UVTGlobal.manip_start[1]) * matrix[3]));

			}
		}
	}
}


void uv_trans_set_manip_pos(egreal pos_x, egreal pos_y, egreal *matrix)
{
	UVTGlobal.manip_pos[0] = pos_x;
	UVTGlobal.manip_pos[1] = pos_y;
}

UVTMode uv_trans_test_manip_area(BInputState *input)
{
	egreal x, y;
	UVTMode mode;
	x = input->pointer_x - uv_get_view_x(UVTGlobal.manip_pos[0]);
	y = input->pointer_y - uv_get_view_y(UVTGlobal.manip_pos[1]);
	mode = TM_IDLE;
	if(UVTGlobal.display != TRUE)
		return TM_IDLE;
	if(0.02 * 0.02 > x * x + y * y)
		mode = TM_MOVE;
	if(0.035 * 0.035 > (x + 0.21) * (x + 0.21) + (y + 0.01) * (y + 0.01))
		mode = TM_SCALE_X;
	if(0.035 * 0.035 > (x - 0.01) * (x - 0.01) + (y - 0.21) * (y - 0.21))
		mode = TM_SCALE_Y;
	if(0.035 * 0.035 > (x + 0.15) * (x + 0.15) + (y - 0.15) * (y - 0.15))
		mode = TM_SCALE;
	if(x > -0.19 && x < -0.04  && y > -0.035 && y < 0.035)
		mode = TM_TRANSLATE_X;
	if(y < 0.19 && y > 0.04  && x > -0.035 && x < 0.035)
		mode = TM_TRANSLATE_Y;
	if(x > -0.04 && y < 0.04 && 0.1 * 0.1 > (x + 0.02) * (x + 0.02) + (y - 0.02) * (y - 0.02) && 0.06 * 0.06 < (x + 0.02) * (x + 0.02) + (y - 0.02) * (y - 0.02))
		mode = TM_TRANSLATE;
	if(0.165 * 0.165 > x * x + y * y && x < -0.035 && y > 0.035)
		mode = TM_ROTATE;
	return mode;
}

boolean uv_trans_test_manip(BInputState *input)
{
	return TM_IDLE != uv_trans_test_manip_area(input);
}

boolean uv_trans_begin(BInputState *input)
{
	UVTGlobal.mode = uv_trans_test_manip_area(input);
	if(UVTGlobal.mode == TM_IDLE)
		return FALSE;
	UVTGlobal.manip_start[0] = UVTGlobal.manip_pos[0];
	UVTGlobal.manip_start[1] = UVTGlobal.manip_pos[1];
	if(UVTGlobal.mode != TM_MOVE)
		uv_trans_lock();
	return TRUE;
}

void uv_trans_continiue(BInputState *input)
{
	egreal matrix[4] = {1, 0, 0, 1}, r, pos[2];
	pos[0] = uv_get_pos_x(input->pointer_x);
	pos[1] = uv_get_pos_y(input->pointer_y);
	if(input->mouse_button[1] == TRUE)
		uv_find_closest_corner(pos, pos[0], pos[1], FALSE);
	switch(UVTGlobal.mode)
	{
		case TM_MOVE :
			uv_find_closest_corner(UVTGlobal.manip_pos, pos[0], pos[1], TRUE);
			break;
		case TM_TRANSLATE :
			uv_trans_form(pos[0], pos[1], matrix);
			UVTGlobal.manip_pos[0] = pos[0];
			UVTGlobal.manip_pos[1] = pos[1];
		break;
		case TM_TRANSLATE_X :
			uv_trans_form(pos[0], UVTGlobal.manip_start[1], matrix);
			UVTGlobal.manip_pos[0] = pos[0];
		break;		
		case TM_TRANSLATE_Y :
			uv_trans_form(UVTGlobal.manip_start[0], pos[1], matrix);
			UVTGlobal.manip_pos[1] = pos[1];
		break;
		case TM_SCALE :
			matrix[0] = matrix[3] = sqrt((uv_get_view_x(UVTGlobal.manip_start[0]) - uv_get_view_x(pos[0])) * (uv_get_view_x(UVTGlobal.manip_start[0]) - uv_get_view_x(pos[0]))) + (uv_get_view_y(pos[1]) - uv_get_view_y(UVTGlobal.manip_start[1]) * (uv_get_view_y(pos[1]) - uv_get_view_y(UVTGlobal.manip_start[1]))) / 0.19;
			uv_trans_form(UVTGlobal.manip_start[0], UVTGlobal.manip_start[1], matrix);
		break;
		case TM_SCALE_X :
			matrix[0] = (uv_get_view_x(UVTGlobal.manip_start[0]) - uv_get_view_x(pos[0])) / 0.21;
			uv_trans_form(UVTGlobal.manip_start[0], UVTGlobal.manip_start[1], matrix);
		break;		
		case TM_SCALE_Y :
			matrix[3] = (uv_get_view_y(pos[1]) - uv_get_view_y(UVTGlobal.manip_start[1])) / 0.21;
			uv_trans_form(UVTGlobal.manip_start[0], UVTGlobal.manip_start[1], matrix);
		break;
		case TM_ROTATE :
			matrix[0] = (UVTGlobal.manip_start[0] - pos[0]);
			matrix[1] = (pos[1] - UVTGlobal.manip_start[1]);
			r = sqrt(matrix[0] * matrix[0] + matrix[1] * matrix[1]);
			matrix[0] /= r; 
			matrix[1] /= r; 
			matrix[2] = -matrix[1]; 
			matrix[3] = matrix[0]; 
			uv_trans_form(UVTGlobal.manip_start[0], UVTGlobal.manip_start[1], matrix);
		break;
		default:
		break;
	}
}

void uv_trans_end()
{
	if(UVTGlobal.mode != TM_MOVE)
	uv_trans_unlock();
	UVTGlobal.mode = TM_IDLE;
}

void uv_trans_hide(void)
{
	UVTGlobal.display = FALSE;
}

void uv_trans_unhide(void)
{
	if(UVTGlobal.display != TRUE)
	{
		egreal uv[8],  *s, pos[2] = {0, 0}, sum = 0;
		uint id, i, poly;
		for(id = uvg_get_next_polygon(0); id != ~0u; id = uvg_get_next_polygon(id + 1))
		{
			if(uvg_get_polygon_select(id))
			{
				poly = uvg_get_sides(id);
				s = uvg_get_corner_select(id);
				uvg_get_uve(uv, id);
				for(i = 0; i < poly; i++)
				{
					pos[0] += uv[i * 2] * s[i];
					pos[1] += uv[i * 2 + 1] * s[i];
					sum += s[i];
				}
			}	
		}
		if(sum > 0.1f)
		{
			UVTGlobal.manip_pos[0] = pos[0] / sum;
			UVTGlobal.manip_pos[1] = pos[1] / sum;
			UVTGlobal.display = TRUE;
		}
	}
}

void uv_trans_get_pos(egreal *pos)
{
	pos[0] = UVTGlobal.manip_pos[0];
	pos[1] = UVTGlobal.manip_pos[1];
}

void uv_trans_draw()
{
	static float array[] = {-0.020000, 0.220000, -0.020000, 0.200000, -0.160000, 0.000000, -0.040000, 0.020000, 0.040000, 0.220000, 0.040000, 0.200000, 0.020000, 0.240000, -0.000000, 0.240000, -0.000000, 0.180000, -0.010000, 0.182679, -0.010000, 0.182679, -0.017321, 0.190000, -0.017321, 0.190000, -0.020000, 0.200000, 0.060000, 0.040000, 0.050000, 0.037320, 0.050000, 0.037320, 0.042680, 0.030000, 0.042680, 0.030000, 0.040000, 0.020000, -0.160000, 0.180000, -0.140000, 0.180000, -0.180000, 0.000000, -0.180000, -0.020000, -0.160000, -0.020000, -0.160000, 0.000000, -0.020000, 0.040000, 0.040000, 0.040000, 0.060000, 0.040000, 0.070000, 0.037320, 0.070000, 0.037320, 0.077321, 0.030000, 0.077321, 0.030000, 0.080000, 0.020000, -0.160000, 0.120000, -0.170000, 0.122679, -0.170000, 0.122679, -0.177320, 0.130000, -0.177320, 0.130000, -0.180000, 0.140000, -0.180000, 0.160000, -0.180000, 0.140000, -0.000000, 0.180000, 0.020000, 0.180000, -0.000000, -0.020000, -0.010000, -0.017320, -0.010000, -0.017320, -0.017321, -0.010000, -0.017321, -0.010000, -0.020000, 0.000000, -0.000000, 0.020000, 0.010000, 0.017321, 0.010000, 0.017321, 0.017320, 0.010000, 0.017320, 0.010000, 0.020000, 0.000000, -0.020000, 0.160000, -0.036875, 0.158979, -0.036875, 0.158979, -0.053504, 0.155932, -0.053504, 0.155932, -0.069645, 0.150902, -0.069645, 0.150902, -0.085061, 0.143964, -0.085061, 0.143964, -0.099529, 0.135218, -0.099529, 0.135218, -0.112837, 0.124792, -0.112837, 0.124792, -0.124792, 0.112837, -0.124792, 0.112837, -0.135218, 0.099529, -0.135218, 0.099529, -0.143964, 0.085061, -0.143964,
 0.085061, -0.150902, 0.069645, -0.150902, 0.069645, -0.155932, 0.053504, -0.155932, 0.053504, -0.158979, 0.036875, -0.158979, 0.036875, -0.160000, 0.020000, -0.020000, -0.040000, -0.030000, -0.042680, -0.030000, -0.042680, -0.037320, -0.050000, -0.037320, -0.050000, -0.040000, -0.060000, -0.160000, 0.180000, -0.170000, 0.177320, -0.170000, 0.177320, -0.177320, 0.170000, -0.177320, 0.170000, -0.180000, 0.160000, -0.200000, 0.020000, -0.190000, 0.017321, -0.190000, 0.017321, -0.182679, 0.010000, -0.182679, 0.010000, -0.180000, 0.000000, -0.200000, -0.040000, -0.190000, -0.037321, -0.190000, -0.037321, -0.182679, -0.030000, -0.182679, -0.030000, -0.180000, -0.020000, -0.000000, 0.240000, -0.010000, 0.237321, -0.010000, 0.237321, -0.017321, 0.230000, -0.017321, 0.230000, -0.020000, 0.220000, -0.220000, 0.020000, -0.230000, 0.017321, -0.230000, 0.017321, -0.237321, 0.010000, -0.237321, 0.010000, -0.240000, 0.000000, -0.220000, -0.040000, -0.230000, -0.037321, -0.230000, -0.037321, -0.237321, -0.030000, -0.237321, -0.030000, -0.240000, -0.020000, -0.140000, 0.180000, -0.130000, 0.177320, -0.130000, 0.177320, -0.122679, 0.170000, -0.122679, 0.170000, -0.120000, 0.160000, -0.160000, 0.120000, -0.147639, 0.121958, -0.147639, 0.121958, -0.136489, 0.127639, -0.136489, 0.127639, -0.127639, 0.136489, -0.127639, 0.136489, -0.121958, 0.147639, -0.121958, 0.147639, -0.120000, 0.160000, -0.000000, 0.160000, 0.020000, 0.160000, -0.020000, 0.040000, -0.000000, 0.160000, 0.020000, 0.160000, 0.040000, 0.040000, -0.040000, 0.020000, -0.040000, -0.040000, -0.040000, -0.040000,
 -0.160000, -0.020000, -0.020000, -0.080000, -0.004357, -0.078769, -0.004357, -0.078769, 0.010902, -0.075106, 0.010902, -0.075106, 0.025399, -0.069101, 0.025399, -0.069101, 0.038779, -0.060902, 0.038779, -0.060902, 0.050711, -0.050711, 0.050711, -0.050711, 0.060902, -0.038779, 0.060902, -0.038779, 0.069101, -0.025399, 0.069101, -0.025399, 0.075106, -0.010902, 0.075106, -0.010902, 0.078769, 0.004357, 0.078769, 0.004357, 0.080000, 0.020000, -0.000000, -0.020000, 0.010000, -0.017320, 0.010000, -0.017320, 0.017320, -0.010000, 0.017320, -0.010000, 0.020000, 0.000000, -0.020000, -0.040000, -0.006649, -0.038496, -0.006649, -0.038496, 0.006033, -0.034058, 0.006033, -0.034058, 0.017409, -0.026910, 0.017409, -0.026910, 0.026910, -0.017409, 0.026910, -0.017409, 0.034058, -0.006033, 0.034058, -0.006033, 0.038496, 0.006649, 0.038496, 0.006649, 0.040000, 0.020000, -0.000000, 0.020000, -0.010000, 0.017321, -0.010000, 0.017321, -0.017321, 0.010000, -0.017321, 0.010000, -0.020000, 0.000000, -0.020000, -0.080000, -0.030000, -0.077321, -0.030000, -0.077321, -0.037320, -0.070000, -0.037320, -0.070000, -0.040000, -0.060000, -0.220000, -0.040000, -0.200000, -0.040000, -0.240000, 0.000000, -0.240000, -0.020000, 0.020000, 0.240000, 0.030000, 0.237321, 0.030000, 0.237321, 0.037321, 0.230000, 0.037321, 0.230000, 0.040000, 0.220000, 0.020000, 0.180000, 0.030000, 0.182679, 0.030000, 0.182679, 0.037321, 0.190000, 0.037321, 0.190000, 0.040000, 0.200000, -0.200000, 0.020000, -0.220000, 0.020000};
	float *cam;
	if(UVTGlobal.display != TRUE)
		return;
	glPushMatrix();
	cam = get_cam_pos();
	glTranslatef(uv_get_view_x(UVTGlobal.manip_pos[0]), uv_get_view_y(UVTGlobal.manip_pos[1]), 0);
	glTranslatef(-cam[0], -cam[1], cam[2] - 1);
	sui_draw_gl(GL_LINES, array, 220, 2, 1, 1, 1, 1.0f);
	glPopMatrix();
}
