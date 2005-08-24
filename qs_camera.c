
#include <math.h>
#include <stdlib.h>

#include "persuade.h"
#include "seduce.h"
#include "st_types.h"

struct{
	float	matrix[16];
	float	pos[3];
	float	vector[3];
	uint	entity_id;
	float	pointer[2];
	float	speed;
}GDroppShip;


void g_dropp_ship_init(void)
{
	uint i;
	for(i = 0; i < 16; i++)
	{
		if(i % 5 == 0)
			GDroppShip.matrix[i] = 1;
		else
			GDroppShip.matrix[i] = 0;
	}
	GDroppShip.pointer[0] = 0;
	GDroppShip.pointer[1] = 0;
	GDroppShip.speed = 0;
	GDroppShip.pos[0] = 0;
	GDroppShip.pos[1] = 0;
	GDroppShip.pos[2] = 1;
}

float *g_get_cam_matrix(void)
{
	return GDroppShip.matrix;
}

float *g_get_cam_pos(void)
{
	return GDroppShip.pos;
}

void reverse_camera_matrix(float *matrix)
{
	float swap;
	swap = matrix[1];
	matrix[1] = matrix[4];
	matrix[4] = swap;
	swap = matrix[2];
	matrix[2] = matrix[8];
	matrix[8] = swap;
	swap = matrix[6];
	matrix[6] = matrix[9];
	matrix[9] = swap;
}


float ge_compute_matrix_read(float *pos, float *goal, uint axis, float *matrix)
{
	float vec[3], r;
	vec[0] = (goal[0] - pos[0]);
	vec[1] = (goal[1] - pos[1]);
	vec[2] = (goal[2] - pos[2]);
	r = matrix[8] * vec[0] + matrix[9] * vec[1] + matrix[10] * vec[2];
	if(r < 1)
		r = 1;
	axis *= 4;
	return matrix[axis + 0] * vec[0] / r + matrix[axis + 1] * vec[1] / r + matrix[axis + 2] * vec[2] / r;
}


void ge_rotate_matrix(float *matrix, float sides, float heigt)
{
	float r;
	matrix[8] += matrix[4] * heigt + matrix[0] * sides;
	matrix[9] += matrix[5] * heigt + matrix[1] * sides;
	matrix[10] += matrix[6] * heigt + matrix[2] * sides;
}

void ge_straighten_matrix(float *matrix, float factor, float *vec, float bank)
{
	matrix[4] += (vec[0] + matrix[0] * bank) * factor;
	matrix[5] += (vec[1] + matrix[1] * bank) * factor;
	matrix[6] += (vec[2] + matrix[2] * bank) * factor;
}

void ge_compute_matrix(float *matrix)
{
	float r;
	r = sqrt(matrix[8] * matrix[8] + matrix[9] * matrix[9] + matrix[10] * matrix[10]);
	matrix[8] = matrix[8] / r;
	matrix[9] = matrix[9] / r;
	matrix[10] = matrix[10] / r;
/*	matrix[0] = matrix[9] * matrix[6] - matrix[10] * matrix[5];
	matrix[1] = matrix[10] * matrix[4] - matrix[8] * matrix[6];
	matrix[2] = matrix[8] * matrix[5] - matrix[9] * matrix[4];*/
	matrix[0] = matrix[5] * matrix[10] - matrix[6] * matrix[9];
	matrix[1] = matrix[6] * matrix[8] - matrix[4] * matrix[10];
	matrix[2] = matrix[4] * matrix[9] - matrix[5] * matrix[8];
	r = sqrt(matrix[0] * matrix[0] + matrix[1] * matrix[1] + matrix[2] * matrix[2]);
	matrix[0] = matrix[0] / r;
	matrix[1] = matrix[1] / r;
	matrix[2] = matrix[2] / r;
/*	matrix[4] = matrix[1] * matrix[10] - matrix[2] * matrix[9];
	matrix[5] = matrix[2] * matrix[8] - matrix[0] * matrix[10];
	matrix[6] = matrix[0] * matrix[9] - matrix[1] * matrix[8];*/
	matrix[4] = matrix[9] * matrix[2] - matrix[10] * matrix[1];
	matrix[5] = matrix[10] * matrix[0] - matrix[8] * matrix[2];
	matrix[6] = matrix[8] * matrix[1] - matrix[9] * matrix[0];
}

void ge_move_matrix(float *pos, float *goal, float *matrix, float speed, float delta_time)
{
	float f;
	f = ge_compute_matrix_read(pos, goal, 2, matrix);
	f *= f;
	speed *= delta_time;
	if(f < 1 && f > 0)
		speed *= 0.1 + 0.9 * f;
	pos[0] += matrix[8] * speed;
	pos[1] += matrix[9] * speed;
	pos[2] += matrix[10] * speed;
}

void ge_move_matrix_axis(float *pos, uint axis, float *matrix, float speed, float delta_time)
{
	speed *= delta_time;
	axis *= 4;
	pos[0] += matrix[axis] * speed;
	pos[1] += matrix[axis + 1] * speed;
	pos[2] += matrix[axis + 2] * speed;
}

void g_just_set_camera(void)
{
	glMultMatrixf(GDroppShip.matrix);
	glTranslatef(-GDroppShip.pos[0], -GDroppShip.pos[1], -GDroppShip.pos[2]);
}

void g_get_projection_screen(float *output, float x, float y, float z)
{
	x = GDroppShip.pos[0] - x;
	y = GDroppShip.pos[1] - y;
	z = GDroppShip.pos[2] - z;
	output[0] = (GDroppShip.matrix[0] * x) + (GDroppShip.matrix[4] * y) + (GDroppShip.matrix[8] * z) + GDroppShip.matrix[12];
	output[1] = (GDroppShip.matrix[1] * x) + (GDroppShip.matrix[5] * y) + (GDroppShip.matrix[9] * z) + GDroppShip.matrix[13];
	output[2] = (GDroppShip.matrix[2] * x) + (GDroppShip.matrix[6] * y) + (GDroppShip.matrix[10] * z) + GDroppShip.matrix[14];
	output[0] = -output[0] / output[2];
	output[1] = -output[1] / output[2];
}
void g_get_projection_screen_no_presp(float *output, float x, float y, float z)
{
	x = GDroppShip.pos[0] - x;
	y = GDroppShip.pos[1] - y;
	z = GDroppShip.pos[2] - z;
	output[0] = (GDroppShip.matrix[0] * x) + (GDroppShip.matrix[4] * y) + (GDroppShip.matrix[8] * z) + GDroppShip.matrix[12];
	output[1] = (GDroppShip.matrix[1] * x) + (GDroppShip.matrix[5] * y) + (GDroppShip.matrix[9] * z) + GDroppShip.matrix[13];
	output[2] = (GDroppShip.matrix[2] * x) + (GDroppShip.matrix[6] * y) + (GDroppShip.matrix[10] * z) + GDroppShip.matrix[14];
}

void g_set_ship_camera(BInputState *input, float delta_time)
{
	float	vec[3] = {0, 1, 0};
	float speed = 0;

	reverse_camera_matrix(GDroppShip.matrix);
//	GDroppShip.pointer[0] = (GDroppShip.pointer[0] + input->pointer_x * 0.3) * 0.9;
//	GDroppShip.pointer[1] = (GDroppShip.pointer[1] + input->pointer_y * 0.3) * 0.9;
	if(input->mouse_button[0])
	{
		GDroppShip.pointer[0] = input->delta_pointer_x;
		GDroppShip.pointer[1] = input->delta_pointer_y;
	}else
	{
		GDroppShip.pointer[0] = 0;
		GDroppShip.pointer[1] = 0;
	}
	ge_rotate_matrix(GDroppShip.matrix, -GDroppShip.pointer[0], -GDroppShip.pointer[1]);
//	ge_rotate_matrix(matrix, 0.01, 0);
//	ge_rotate_matrix(matrix, 0, 0.1);
	ge_straighten_matrix(GDroppShip.matrix, delta_time, vec, GDroppShip.pointer[0] * GDroppShip.speed * 4);
	ge_compute_matrix(GDroppShip.matrix);
	if(input->mouse_button[0])
		GDroppShip.speed += 0.05;
	if(input->mouse_button[0] && input->last_mouse_button[0] == FALSE && GDroppShip.speed > 0 && GDroppShip.speed < 1)
		GDroppShip.speed += (2 - GDroppShip.speed) * 0.6;
	if(input->mouse_button[1])
		GDroppShip.speed -= 0.05;
	GDroppShip.speed *= 0.9;
//	ge_move_matrix(e, GDroppShip.matrix, GDroppShip.speed, delta_time);
	vec[0] = GDroppShip.matrix[8] * -GDroppShip.speed * 0.2;
	vec[1] = GDroppShip.matrix[9] * -GDroppShip.speed * 0.2;
	vec[2] = GDroppShip.matrix[10] * -GDroppShip.speed * 0.2;
	GDroppShip.vector[0] = (GDroppShip.vector[0] + vec[0] * 0.3) * 0.9;
	GDroppShip.vector[1] = (GDroppShip.vector[1] + vec[1] * 0.3) * 0.9;
	GDroppShip.vector[2] = (GDroppShip.vector[2] + vec[2] * 0.3) * 0.9;
	GDroppShip.pos[0] += (GDroppShip.vector[0] + vec[0] + GDroppShip.matrix[8] * GDroppShip.pointer[0]) * delta_time;
	GDroppShip.pos[1] += (GDroppShip.vector[1] + vec[1] + GDroppShip.matrix[9] * GDroppShip.pointer[0]) * delta_time;
	GDroppShip.pos[2] += (GDroppShip.vector[2] + vec[2] + GDroppShip.matrix[10] * GDroppShip.pointer[0]) * delta_time;
//		glGetFloatv(GL_MODELVIEW_MATRIX, GDroppShip.matrix);
	reverse_camera_matrix(GDroppShip.matrix);
//		glRotatef(90, 0, 1, 0);
	glMultMatrixf(GDroppShip.matrix);
	glTranslatef(-GDroppShip.pos[0], -GDroppShip.pos[1], -GDroppShip.pos[2]);
	{
		double view_cam[3];
		view_cam[0] = GDroppShip.pos[0]; 
		view_cam[1] = GDroppShip.pos[1]; 
		view_cam[2] = GDroppShip.pos[2]; 
		p_lod_set_view_pos(view_cam);
	}
}
