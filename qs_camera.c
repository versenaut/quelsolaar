
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

#include "enough.h"
#include "st_types.h"
#include "betray.h"
#include "persuade.h"
#include "seduce.h"

static struct{
	double	matrix[16];
	double	pos[3];
	double	pitch;
	double	jaw;
	double	pitch_goal;
	double	jaw_goal;
	double	speed;
	double	side_speed;
} GView;


void qs_camera_init(void)
{
	uint i;
	for(i = 0; i < 16; i++)
	{
		if(i % 5 == 0)
			GView.matrix[i] = 1;
		else
			GView.matrix[i] = 0;
	}
	GView.pos[0] = 0;
	GView.pos[1] = 0;
	GView.pos[2] = 0;
	GView.pitch = 0;
	GView.jaw = 0;
	GView.pitch_goal = 0;
	GView.jaw_goal = 0;
	GView.pitch_goal = 0;
	GView.jaw_goal = 0;
}

double  *qs_get_cam_matrix(void)
{
	return GView.matrix;
}

double  *qs_get_cam_pos(void)
{
	return GView.pos;
}

void qs_compute_matrix(double *matrix)
{
	double r;
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

void qs_set_camera(void)
{
	double matrix[16];
	matrix[0] = GView.matrix[0];
	matrix[1] = GView.matrix[4];
	matrix[2] = GView.matrix[8];
	matrix[3] = GView.matrix[12];
	matrix[4] = GView.matrix[1];
	matrix[5] = GView.matrix[5];
	matrix[6] = GView.matrix[9];
	matrix[7] = GView.matrix[13];
	matrix[8] = GView.matrix[2];
	matrix[9] = GView.matrix[6];
	matrix[10] = GView.matrix[10];
	matrix[11] = GView.matrix[14];
	matrix[12] = GView.matrix[3];
	matrix[13] = GView.matrix[7];
	matrix[14] = GView.matrix[11];
	matrix[15] = GView.matrix[15];

	glMultMatrixd(matrix);
	glTranslatef(-GView.pos[0], -GView.pos[1], -GView.pos[2]);
}

static void eulers_to_quat(VNQuat64 *quat, real32 heading, real32 attitude)
{
	real32	c1 = cosf(heading / 2.0f), c2 = cosf(attitude / 2.0f), c3 = 1.0f;
	real32	s1 = sinf(heading / 2.0f), s2 = sinf(attitude / 2.0f), s3 = 0.0f;

	quat->x = s1 * s2 * c3 + c1 * c2 * s3;
	quat->y = s1 * c2 * c3 + c1 * s2 * s3;
	quat->z = c1 * s2 * c3 - s1 * c2 * s3;
	quat->w = c1 * c2 * c3 - s1 * s2 * s3;
}

void qs_compute_camera(BInputState *input, float delta_time)
{
	static real64 old_pos[3] = { 0.0f };
	static boolean w = FALSE, lw = FALSE, s = FALSE, ls = FALSE, a = FALSE, la = FALSE, d = FALSE, ld = FALSE;
	double x, y, z;

	if(input->mouse_button[0])
		betray_set_mouse_warp(TRUE);

	if(input->mouse_button[0] && input->last_mouse_button[0])
	{
		VNQuat64	rot;

		betray_set_mouse_warp(TRUE);
		GView.matrix[4] = 0;
		GView.matrix[5] = 1;
		GView.matrix[6] = 0;
		GView.pitch_goal -= input->delta_pointer_y * 4;
		GView.jaw_goal -= input->delta_pointer_x * 4;

		GView.pitch = GView.pitch_goal * 0.3 + GView.pitch * 0.7;
		GView.jaw = GView.jaw_goal * 0.3 + GView.jaw * 0.7;

/*		GView.jaw -= input->delta_pointer_x * 4;
		if(GView.jaw > PI)
			GView.jaw -= PI * 2.0;
		if(GView.jaw < -PI)
			GView.jaw += PI * 2.0;
		GView.pitch -= input->delta_pointer_y * 4;
*/		if(GView.pitch > PI * 0.499)
			GView.pitch = PI * 0.499;
		if(GView.pitch < -PI * 0.499)
			GView.pitch = -PI * 0.499;

		y = cos(GView.pitch);
		GView.matrix[8] = sin(GView.jaw) * y;
		GView.matrix[9] = sin(GView.pitch);
		GView.matrix[10] = cos(GView.jaw) * y;

		qs_compute_matrix(GView.matrix);

		eulers_to_quat(&rot, GView.jaw + M_PI/ 2 , -GView.pitch);
		verse_send_o_transform_rot_real64(e_ns_get_node_id(e_ns_get_node_avatar(0)),
						  0, 0, &rot, NULL, NULL, NULL, 0.0f);
	}
	betray_get_key_up_down(&w, &lw, 'w');
	betray_get_key_up_down(&s, &ls, 's');
	betray_get_key_up_down(&a, &la, 'a');
	betray_get_key_up_down(&d, &ld, 'd');
	x = 0;
	y = 0;
	if(w || input->mouse_button[1])
		GView.speed++;
	if(s)
		GView.speed--;
	if(a)
		GView.side_speed++;
	if(d)
		GView.side_speed--;

	GView.speed *= 0.9;
	GView.side_speed *= 0.9;

	GView.pos[0] -= GView.matrix[8] * delta_time * GView.speed;
	GView.pos[1] -= GView.matrix[9] * delta_time * GView.speed;
	GView.pos[2] -= GView.matrix[10] * delta_time * GView.speed;
	GView.pos[0] -= GView.matrix[0] * delta_time * GView.side_speed;
	GView.pos[1] -= GView.matrix[1] * delta_time * GView.side_speed;
	GView.pos[2] -= GView.matrix[2] * delta_time * GView.side_speed;

	if(GView.pos[0] != old_pos[0] ||
	   GView.pos[1] != old_pos[1] ||
	   GView.pos[2] != old_pos[2])
	{
		verse_send_o_transform_pos_real64(e_ns_get_node_id(e_ns_get_node_avatar(0)),
						  0, 0, GView.pos, NULL, NULL, NULL, 0.0f);
		memcpy(old_pos, GView.pos, sizeof old_pos);
	}

	if(!input->mouse_button[0] && !input->last_mouse_button[0])
		betray_set_mouse_warp(FALSE);
}
