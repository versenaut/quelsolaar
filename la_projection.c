#include "la_includes.h"

#include "la_projection.h"
#include "la_geometry_undo.h"

 


extern void clear_matrix(double *matrix);
/*
void point_threw_matrix(float *matrix, float *vertex)
*/
extern void point_threw_matrix4(double *matrix, double *pos_x, double *pos_y, double *pos_z, double *pos_w);
extern void point_threw_matrix3(double *matrix, double *pos_x, double *pos_y, double *pos_z);

extern void matrix_multiply(const double *a,const  double *b, double *output);

extern void matrix_rotate_x(double *matrix, double degree);
extern void matrix_rotate_y(double *matrix, double degree);
extern void matrix_rotate_z(double *matrix, double degree);
extern void transform_matrix(double *matrix, double x, double y, double z);
extern void scale_matrix(double *matrix, double x, double y, double z);
extern void negate_matrix(double *matrix);
extern void reverse_matrix(double *matrix);
extern void matrix_print(const double *matrix);
extern void la_draw_force_update_persuade(void);

struct{
	double			matrix[16];
	double			model[16];
	double			pitch;
	double			yaw;
	double			pitch_target;
	double			yaw_target;
	double			target[3];
	double			position[3];
	double			speed;
	double			distance;
	double			distance_target;
    double			grid_size;
	float			grab[2];
	uint			view_axis;
	uint			axis;
	double			direction;
}ProjectionData;

void p_init(void)
{
	ProjectionData.pitch = 0;
	ProjectionData.yaw = 0;
	ProjectionData.pitch_target = 0;
	ProjectionData.yaw_target = 0;
	ProjectionData.position[0] = 0;
	ProjectionData.position[1] = 0;
	ProjectionData.position[2] = 0;
	ProjectionData.target[0] = 0;
	ProjectionData.target[1] = 0;
	ProjectionData.target[2] = 0;
	ProjectionData.distance = 1.0;
	ProjectionData.distance_target = 1.0;
	ProjectionData.view_axis = 3;
	ProjectionData.speed = 0.8;
    ProjectionData.grid_size = 1000;
	ProjectionData.axis = 3;
}

void p_view_change_start(BInputState *input)
{
	ProjectionData.grab[0] = input->pointer_x;
	ProjectionData.grab[1] = input->pointer_y;
	ProjectionData.speed = 0.8;
}

void p_view_change(BInputState *input)
{
	float a, b;
	if(input->mode == BAM_EVENT)
	{
		if(input->last_mouse_button[1] == TRUE)
		{
			if(input->mouse_button[2] == TRUE && input->mouse_button[0] == TRUE)
			{
				p_init();
			}else if(input->mouse_button[2] == TRUE)
			{
				ProjectionData.position[0] += ProjectionData.model[0] * ProjectionData.distance * 2 * (ProjectionData.grab[0] - input->pointer_x);
				ProjectionData.position[1] += ProjectionData.model[4] * ProjectionData.distance * 2 * (ProjectionData.grab[0] - input->pointer_x);
				ProjectionData.position[2] += ProjectionData.model[8] * ProjectionData.distance * 2 * (ProjectionData.grab[0] - input->pointer_x);

				ProjectionData.position[0] += ProjectionData.model[1] * ProjectionData.distance * 2 * (ProjectionData.grab[1] - input->pointer_y);
				ProjectionData.position[1] += ProjectionData.model[5] * ProjectionData.distance * 2 * (ProjectionData.grab[1] - input->pointer_y);
				ProjectionData.position[2] += ProjectionData.model[9] * ProjectionData.distance * 2 * (ProjectionData.grab[1] - input->pointer_y);
				ProjectionData.target[0] = ProjectionData.position[0];
				ProjectionData.target[1] = ProjectionData.position[1];
				ProjectionData.target[2] = ProjectionData.position[2];
			}else if(input->mouse_button[0] == TRUE)
			{
				a = ProjectionData.distance * (1 + (input->delta_pointer_x - input->delta_pointer_y) * 4);
				if(a > 90000)
					a = 90000;
				if(a < 0.01)
					a = 0.01;
				ProjectionData.distance_target = a;
			}else
			{
				a = ProjectionData.yaw_target + (input->pointer_x - ProjectionData.grab[0]) * 400; 
				b = ProjectionData.pitch_target - (input->pointer_y - ProjectionData.grab[1]) * 400;
				if(b > 90)
					b = 90;
				if(b < -90)
					b = -90;
		//		ProjectionData.pitch = b;
				ProjectionData.pitch_target = b;
		//		ProjectionData.yaw = a;
				ProjectionData.yaw_target = a;
				if(ProjectionData.yaw > 180 && ProjectionData.yaw_target > 180)
				{
					ProjectionData.yaw -= 360;
					ProjectionData.yaw_target -= 360;
				}else if(ProjectionData.yaw < -180 && ProjectionData.yaw_target < -180)
				{
					ProjectionData.yaw += 360;
					ProjectionData.yaw_target += 360;
				}
			}
		}
		ProjectionData.grab[0] = input->pointer_x;
		ProjectionData.grab[1] = input->pointer_y;

	/*	a = sqrt(ProjectionData.model[2] * ProjectionData.model[2] + ProjectionData.model[6] * ProjectionData.model[6] + ProjectionData.model[10] * ProjectionData.model[10]);
		if(ProjectionData.model[2] / a > 0.95 || ProjectionData.model[2] / a < -0.95)
			ProjectionData.view_axis = 0;
		else if(ProjectionData.model[6] / a > 0.95 || ProjectionData.model[6] / a < -0.95)
			ProjectionData.view_axis = 1;
		else if(ProjectionData.model[10] / a > 0.95 || ProjectionData.model[10] / a < -0.95)
			ProjectionData.view_axis = 2;
		else*/
			ProjectionData.view_axis = 3;

	/*	if(input->mouse_button[1] == TRUE && (input->delta_pointer_x > 0.0001 || input->delta_pointer_x < 0.0001 || input->delta_pointer_y > 0.0001 || input->delta_pointer_y < 0.0001))
			la_draw_force_update_persuade();*/
		if(input->mouse_button[1] == FALSE && input->last_mouse_button[1] == TRUE)
			la_draw_force_update_persuade();
	}
}

void p_projection_update(void)
{
	double	proj[16];
	glGetDoublev(GL_PROJECTION_MATRIX, ProjectionData.matrix);
	glGetDoublev(GL_MODELVIEW_MATRIX, ProjectionData.model);
}
void p_set_grid_size(double grid_size)
{
    ProjectionData.grid_size = 1.0 / grid_size;
}

void p_set_view_center(double *center)
{
	uint32 vertex_count, polygon_count, *ref, i;
	double *vertex;
	boolean rotate = FALSE;
	double normal[3] = {0, 0, 0}, temp, size = 0.001;
	ProjectionData.speed = 0.95;
	ProjectionData.target[0] = center[0];
	ProjectionData.target[1] = center[1];
	ProjectionData.target[2] = center[2];
	udg_get_geometry(&vertex_count, &polygon_count, &vertex, &ref, NULL);
	for(i = 0; i < polygon_count; i++)
	{
		if(ref[i * 4] < vertex_count && vertex[ref[i * 4] * 3] != V_REAL64_MAX && 0.01 < udg_get_select(ref[i * 4]) && ref[i * 4 + 1] < vertex_count && vertex[ref[i * 4 + 1] * 3] != V_REAL64_MAX && 0.01 < udg_get_select(ref[i * 4 + 1]) && ref[i * 4 + 2] < vertex_count  && vertex[ref[i * 4 + 2] * 3] != V_REAL64_MAX && 0.01 < udg_get_select(ref[i * 4 + 2]) && (ref[i * 4 + 3] >= vertex_count || vertex[ref[i * 4 + 3] * 3] == V_REAL64_MAX || 0.01 < udg_get_select(ref[i * 4 + 3])))
		{	
			normal[0] += ((vertex[ref[i * 4] * 3 + 1] - vertex[ref[i * 4 + 1] * 3 + 1]) * (vertex[ref[i * 4 + 2] * 3 + 2] - vertex[ref[i * 4 + 1] * 3 + 2]) - (vertex[ref[i * 4] * 3 + 2] - vertex[ref[i * 4 + 1] * 3 + 2]) * (vertex[ref[i * 4 + 2] * 3 + 1] - vertex[ref[i * 4 + 1] * 3 + 1]));
			normal[1] += ((vertex[ref[i * 4] * 3 + 2] - vertex[ref[i * 4 + 1] * 3 + 2]) * (vertex[ref[i * 4 + 2] * 3 + 0] - vertex[ref[i * 4 + 1] * 3 + 0]) - (vertex[ref[i * 4] * 3 + 0] - vertex[ref[i * 4 + 1] * 3 + 0]) * (vertex[ref[i * 4 + 2] * 3 + 2] - vertex[ref[i * 4 + 1] * 3 + 2]));
			normal[2] += ((vertex[ref[i * 4] * 3 + 0] - vertex[ref[i * 4 + 1] * 3 + 0]) * (vertex[ref[i * 4 + 2] * 3 + 1] - vertex[ref[i * 4 + 1] * 3 + 1]) - (vertex[ref[i * 4] * 3 + 1] - vertex[ref[i * 4 + 1] * 3 + 1]) * (vertex[ref[i * 4 + 2] * 3 + 0] - vertex[ref[i * 4 + 1] * 3 + 0]));
			rotate = TRUE;
		}
	}
	if(rotate != TRUE)
	{
		for(i = 0; i < polygon_count; i++)
		{
			if(ref[i * 4] < vertex_count && vertex[ref[i * 4] * 3] != V_REAL64_MAX && ref[i * 4 + 1] < vertex_count && vertex[ref[i * 4 + 1] * 3] != V_REAL64_MAX && ref[i * 4 + 2] < vertex_count && vertex[ref[i * 4 + 2] * 3] != V_REAL64_MAX)
			{	
				if(0.01 < udg_get_select(ref[i * 4]) || 0.01 < udg_get_select(ref[i * 4 + 1]) || 0.01 < udg_get_select(ref[i * 4 + 2]) || (ref[i * 4 + 3] < vertex_count && vertex[ref[i * 4 + 3] * 3] != V_REAL64_MAX && 0.01 < udg_get_select(ref[i * 4 + 3])))
				{
					normal[0] += ((vertex[ref[i * 4] * 3 + 1] - vertex[ref[i * 4 + 1] * 3 + 1]) * (vertex[ref[i * 4 + 2] * 3 + 2] - vertex[ref[i * 4 + 1] * 3 + 2]) - (vertex[ref[i * 4] * 3 + 2] - vertex[ref[i * 4 + 1] * 3 + 2]) * (vertex[ref[i * 4 + 2] * 3 + 1] - vertex[ref[i * 4 + 1] * 3 + 1]));
					normal[1] += ((vertex[ref[i * 4] * 3 + 2] - vertex[ref[i * 4 + 1] * 3 + 2]) * (vertex[ref[i * 4 + 2] * 3 + 0] - vertex[ref[i * 4 + 1] * 3 + 0]) - (vertex[ref[i * 4] * 3 + 0] - vertex[ref[i * 4 + 1] * 3 + 0]) * (vertex[ref[i * 4 + 2] * 3 + 2] - vertex[ref[i * 4 + 1] * 3 + 2]));
					normal[2] += ((vertex[ref[i * 4] * 3 + 0] - vertex[ref[i * 4 + 1] * 3 + 0]) * (vertex[ref[i * 4 + 2] * 3 + 1] - vertex[ref[i * 4 + 1] * 3 + 1]) - (vertex[ref[i * 4] * 3 + 1] - vertex[ref[i * 4 + 1] * 3 + 1]) * (vertex[ref[i * 4 + 2] * 3 + 0] - vertex[ref[i * 4 + 1] * 3 + 0]));
					rotate = TRUE;
				}
			}
		}
	}
	if(rotate == TRUE)
	{
		temp = 360 * atan2(-normal[0], normal[2]) / 6.28;
		if(temp > 0 &&  ProjectionData.yaw < temp - 180)
			ProjectionData.yaw += 360;
		if(temp < 0 &&  ProjectionData.yaw > temp + 180)
			ProjectionData.yaw -= 360;
		ProjectionData.yaw_target = temp;
		temp = (normal[0] * normal[0]) + (normal[1] * normal[1]) + (normal[2] * normal[2]);
		if(0.0 != temp)
			ProjectionData.pitch_target = 360 * atan(normal[1] / sqrt(temp)) / 6.28;
	}

	for(i = 0; i < vertex_count; i++)
	{
		if(vertex[i * 3] != V_REAL64_MAX  && 0.01 < udg_get_select(i))
		{	
			temp = (vertex[i * 3] - center[0]) * (vertex[i * 3] - center[0]) + (vertex[i * 3 + 1] - center[1]) * (vertex[i * 3 + 1] - center[1]) + (vertex[i * 3 + 2] - center[2]) * (vertex[i * 3 + 2] - center[2]);
			if(temp > size)
				size = temp;
		}
	}
	size = sqrt(size) * 2;
	ProjectionData.distance_target = size;
}

void p_get_view_center(double *center)
{
	center[0] = ProjectionData.position[0];
	center[1] = ProjectionData.position[1];
	center[2] = ProjectionData.position[2];
}

void p_get_view_camera(double *camera)
{
	camera[0] = ProjectionData.model[2] * ProjectionData.distance + ProjectionData.position[0];
	camera[1] = ProjectionData.model[6] * ProjectionData.distance + ProjectionData.position[1];
	camera[2] = ProjectionData.model[10] * ProjectionData.distance + ProjectionData.position[2];
}

double p_get_distance_camera(void)
{
	return ProjectionData.distance;
}
void p_get_model_matrix(double *matrix)
{
	uint i;
	for(i = 0; i < 16; i++)
		matrix[i] = ProjectionData.model[i];
}

void p_view_set(void)
{
	static float rotate = 0;
	rotate++;

	ProjectionData.position[0] = ProjectionData.target[0] * (1 - ProjectionData.speed) + ProjectionData.position[0] * ProjectionData.speed;
	ProjectionData.position[1] = ProjectionData.target[1] * (1 - ProjectionData.speed) + ProjectionData.position[1] * ProjectionData.speed;
	ProjectionData.position[2] = ProjectionData.target[2] * (1 - ProjectionData.speed) + ProjectionData.position[2] * ProjectionData.speed;

	ProjectionData.pitch = ProjectionData.pitch_target * (1 - ProjectionData.speed) + ProjectionData.pitch * ProjectionData.speed;
	ProjectionData.yaw = ProjectionData.yaw_target * (1 - ProjectionData.speed) + ProjectionData.yaw * ProjectionData.speed;
	ProjectionData.distance = ProjectionData.distance_target * (1 - ProjectionData.speed) + ProjectionData.distance * ProjectionData.speed;
	glTranslated(0, 0, -ProjectionData.distance);
	glRotated(ProjectionData.pitch, 1, 0, 0); /* the rotate functions now handels radians too */
	glRotated(ProjectionData.yaw, 0, 1, 0); /* the rotate functions now handels radians too */
	glTranslated(-ProjectionData.position[0], -ProjectionData.position[1], -ProjectionData.position[2]);
	p_projection_update();

}


void p_get_projection(double *output, float x, float y)
{
	output[0] = ProjectionData.model[0] * x * ProjectionData.distance;
	output[1] = ProjectionData.model[4] * x * ProjectionData.distance;
	output[2] = ProjectionData.model[8] * x * ProjectionData.distance;
	output[0] += ProjectionData.model[1] * y * ProjectionData.distance;
	output[1] += ProjectionData.model[5] * y * ProjectionData.distance;
	output[2] += ProjectionData.model[9] * y * ProjectionData.distance;
	output[0] += ProjectionData.position[0];
	output[1] += ProjectionData.position[1];
	output[2] += ProjectionData.position[2];
}

void p_get_projection_vertex(double *output, double *vertex, double x, double y)
{
	double dist, z;
	z = ProjectionData.model[2] * (vertex[0] - ProjectionData.position[0]) + ProjectionData.model[6] * (vertex[1] - ProjectionData.position[1]) + ProjectionData.model[10] * (vertex[2] - ProjectionData.position[2]);
	dist = (ProjectionData.distance - z);
	if(dist < 0)
		dist = ProjectionData.distance;
	output[0] = ProjectionData.model[0] * x * dist;
	output[1] = ProjectionData.model[4] * x * dist;
	output[2] = ProjectionData.model[8] * x * dist;
	output[0] += ProjectionData.model[1] * y * dist;
	output[1] += ProjectionData.model[5] * y * dist;
	output[2] += ProjectionData.model[9] * y * dist;
	output[0] += ProjectionData.model[2] * z;
	output[1] += ProjectionData.model[6] * z;
	output[2] += ProjectionData.model[10] * z;
	output[0] += ProjectionData.position[0];
	output[1] += ProjectionData.position[1];
	output[2] += ProjectionData.position[2];
}

void p_get_projection_screen(double *output, double x, double y, double z)
{
	point_threw_matrix3(ProjectionData.model, &x, &y, &z);
	output[0] = x / z;
	output[1] = y / z;
	output[2] = z;
}
/*
			p_get_projection_screen(pos, vertex[i].x, vertex[i].y, vertex[i].z);
			pos[0] += x;
			pos[1] += y;
			r = pos[0] * pos[0] + pos[1] * pos[1];
			if(r < *selected_distance)
*/
double p_get_projection_screen_distance(double space_x, double space_y, double space_z, double screen_x, double screen_y)
{
	point_threw_matrix3(ProjectionData.model, &space_x, &space_y, &space_z);
	space_x = (space_x / space_z) + screen_x ;
	space_y = (space_y / space_z) + screen_y;
	return space_x * space_x + space_y * space_y;
}
/* r = (b[axis] - depth) / (b[axis] - a[axis]); */
void p_get_projection_plane(double *dist, uint axis, double pointer_x, double pointer_y , double depth)
{
	double a[3], b[3], r;
	a[0] = ProjectionData.model[0] * ProjectionData.distance * pointer_x;
	a[1] = ProjectionData.model[4] * ProjectionData.distance * pointer_x;
	a[2] = ProjectionData.model[8] * ProjectionData.distance * pointer_x;
	a[0] += ProjectionData.model[1] * ProjectionData.distance * pointer_y;
	a[1] += ProjectionData.model[5] * ProjectionData.distance * pointer_y;
	a[2] += ProjectionData.model[9] * ProjectionData.distance * pointer_y;
	a[0] += ProjectionData.position[0];
	a[1] += ProjectionData.position[1];
	a[2] += ProjectionData.position[2];
	b[0] = ProjectionData.model[2] * ProjectionData.distance;
	b[1] = ProjectionData.model[6] * ProjectionData.distance;
	b[2] = ProjectionData.model[10] * ProjectionData.distance;
	b[0] += ProjectionData.position[0];
	b[1] += ProjectionData.position[1];
	b[2] += ProjectionData.position[2];
	r = (b[axis] - depth) / (b[axis] - a[axis]);
	dist[0] = b[0] - (b[0] - a[0]) * r;
	dist[1] = b[1] - (b[1] - a[1]) * r;
	dist[2] = b[2] - (b[2] - a[2]) * r;
	dist[axis] = depth;
}

double p_get_projection_line(double *dist, uint axis, double pointer_x, double pointer_y, double *pos)
{
	double a[3], b[3], r, r2, r3;
	a[0] = ProjectionData.model[0] * ProjectionData.distance * pointer_x;
	a[1] = ProjectionData.model[4] * ProjectionData.distance * pointer_x;
	a[2] = ProjectionData.model[8] * ProjectionData.distance * pointer_x;
	a[0] += ProjectionData.model[1] * ProjectionData.distance * pointer_y;
	a[1] += ProjectionData.model[5] * ProjectionData.distance * pointer_y;
	a[2] += ProjectionData.model[9] * ProjectionData.distance * pointer_y;
	b[0] = ProjectionData.model[2] * ProjectionData.distance;
	b[1] = ProjectionData.model[6] * ProjectionData.distance;
	b[2] = ProjectionData.model[10] * ProjectionData.distance;
	a[0] -= b[0];
	a[1] -= b[1];
	a[2] -= b[2];
	b[0] += ProjectionData.position[0] - pos[0];
	b[1] += ProjectionData.position[1] - pos[1];
	b[2] += ProjectionData.position[2] - pos[2];
	r = sqrt(b[(axis + 1) % 3] * b[(axis + 1) % 3] + b[(axis + 2) % 3] * b[(axis + 2) % 3]);
	r2 = sqrt(a[(axis + 1) % 3] * a[(axis + 1) % 3] + a[(axis + 2) % 3] * a[(axis + 2) % 3]);
	r3 = b[axis] + (a[axis] * r / r2);
	if(dist != NULL)
	{
		a[0] = pos[0];
		a[1] = pos[1];
		a[2] = pos[2];
		a[axis] += r3;
		point_threw_matrix3(ProjectionData.model, &a[0], &a[1], &a[2]);
		a[0] = (a[0] / a[2]) + pointer_x;
		a[1] = (a[1] / a[2]) + pointer_y;
		*dist = a[0] * a[0] + a[1] * a[1];
	}

	return r3;
}

void p_get_projection_line_snap(double *output, uint axis, double direction, double *start, double *snap, LASnapType snap_type)
{
	output[0] = start[0];
	output[1] = start[1];
	output[2] = start[2];
	if(snap_type == LA_ST_VERTEX)
	{
		output[axis] = snap[axis];
	}else if(snap_type == LA_ST_EDGE)
	{
		if(direction > 0)
			output[axis] += snap[3];
		else
			output[axis] -= snap[3];
		printf("HH p_get_projection_line_snap %u %f \n", axis, snap[3]);
		printf("HH start %f %f %f\n", start[0], start[1], start[2]);
		printf("HH out %f %f %f\n", output[0], output[1], output[2]);
	}else if(snap_type == LA_ST_SURFACE)
	{
		double r, vector[3] = {0, 0, 0};
		vector[axis] = 1;
		
		r = vector[0] * snap[3] + vector[1] * snap[4] + vector[2] * snap[5];
		if(r < 0.1 && r > -0.1)
			;//output[axis] += direction;
		else
		{
			r = (snap[3] * (start[0] - snap[0]) + snap[4] * (start[1] - snap[1]) + snap[5] * (start[2] - snap[2])) / r;
			output[0] = start[0] - r * vector[0];
			output[1] = start[1] - r * vector[1];
			output[2] = start[2] - r * vector[2];
		}
	}
}

void p_get_projection_vertex_with_axis(double *output, double *start, double pointer_x, double pointer_y, boolean snap, double *closest, LASnapType snap_type)
{
	double dist, best_dist, pos;
	//static uint axis; 
	uint i; 
//	printf("*closest = %f\n", *closest);
	if(snap != TRUE)
	{
		ProjectionData.direction = p_get_projection_line(&best_dist, 0, pointer_x, pointer_y, start);
		ProjectionData.axis = 0;
		for(i = 1; i < 3; i++)
		{
			pos = p_get_projection_line(&dist, i, pointer_x, pointer_y, start);
			if(dist < best_dist)
			{
				ProjectionData.direction = pos;
				best_dist = dist;
				ProjectionData.axis = i;
			}
		}
		if(ProjectionData.view_axis < 3)
		{	
			p_get_projection_plane(output, ProjectionData.view_axis, pointer_x, pointer_y, start[ProjectionData.view_axis]);
			return;
		}
		if(best_dist > 0.0001)
		{
			p_get_projection_vertex(output, start, pointer_x, pointer_y);
			return;
		}
	}
	output[0] = start[0];
	output[1] = start[1];
	output[2] = start[2];
	if(snap != TRUE)
		output[ProjectionData.axis] += ProjectionData.direction;
	else
	{
      /*  p_get_projection_line(&best_dist, 0, pointer_x, pointer_y, start);
		if(best_dist < 0.0001)
		{
			output[0] = p_get_projection_line(&best_dist, ProjectionData.axis, pointer_x, pointer_y, start);
			output[ProjectionData.axis] += (double)((int)(output[0] * ProjectionData.grid_size)) / ProjectionData.grid_size;
		}
		else
		{*/
			p_get_projection_line_snap(output, ProjectionData.axis, ProjectionData.direction, start, closest, snap_type);
	/*	}*/
	}
}

uint p_get_projection_axis(void)
{
	return ProjectionData.axis;
}

void p_find_closest_vertex(uint *closest, uint *selected, double *distance, double *selected_distance, double x, double y)
{
	uint32 vertex_count, i;
	double	*vertex;
	double	pos[3], r;
	udg_get_geometry(&vertex_count, NULL, &vertex, NULL, NULL);

	for(i = 0; i < vertex_count; i++)
	{
		if(vertex[i * 3] != V_REAL64_MAX)
		{
			p_get_projection_screen(pos, vertex[i * 3], vertex[i * 3 + 1], vertex[i * 3 + 2]);
			if(pos[2] < 0)
			{
				pos[0] += x;
				pos[1] += y;
				r = pos[0] * pos[0] + pos[1] * pos[1];
				if(r < *selected_distance)
				{
					*selected_distance = r;
					*selected = i;
					if(r <= *distance && udg_get_select(i) < 0.001)
					{
						*distance = r;
						*closest = i;
					}
				}
			}
		}
	}
}

boolean p_find_closest_tag(double *pos, double distance, double x, double y)
{
	boolean output = FALSE;
	uint32 count, i;
	double r;
	UNDOTag	*tag;
	tag = udg_get_tags(&count);
	for(i = 0; i < count; i++)
	{
		p_get_projection_screen(pos, tag[i].vec[0], tag[i].vec[1], tag[i].vec[2]);
		if(pos[2] < 0)
		{
			pos[0] += x;
			pos[1] += y;
			r = pos[0] * pos[0] + pos[1] * pos[1];
			if(r < distance && tag[i].select < 0.001)
			{
				distance = r;
				pos[0] = tag[i].vec[0];
				pos[1] = tag[i].vec[1];
				pos[2] = tag[i].vec[2];
				output = TRUE;
			}
		}
	}
	return output;
}

uint p_find_click_tag(double x, double y)
{
	uint32 count, i, output = -1;
	double r, pos[3];
	UNDOTag	*tag;
	tag = udg_get_tags(&count);
	for(i = 0; i < count; i++)
	{
		p_get_projection_screen(pos, tag[i].vec[0], tag[i].vec[1], tag[i].vec[2]);
		if(pos[2] < 0)
		{
			pos[0] += x;
			pos[1] += y;
			r = pos[0] * pos[0] + pos[1] * pos[1];
			if(r < 0.001)
			{
				output = i;
			}
		}
	}
	return output;
}

boolean p_find_click_tag_lable(double x, double y)
{
	uint32 count, i;
	double f, f2, pos[3];
	UNDOTag	*tag;
	tag = udg_get_tags(&count);
	for(i = 0; i < count; i++)
	{
		p_get_projection_screen(pos, tag[i].vec[0], tag[i].vec[1], tag[i].vec[2]);
		if(pos[2] < 0 && pos[1] - 0.02 < -y && pos[1] + 0.02 > -y)
		{
			f = sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, tag[i].group);
			f2 = sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, tag[i].tag);
			if(pos[0] > -x && pos[0] - (f + f2 + 0.06) < -x)

			
//			if(sui_box_click_test(pos[0] - f + 0.04, pos[1] + 0.03, -f - 0.04 , -0.06))
		//	if(sui_box_click_test(pos[0] - f + 0.04, pos[1] + 0.03, -f - 0.04 , -0.06))
			{
				udg_rename_tag(i);
				return TRUE;
			}
/*			if(sui_box_click_test(pos[0] + f + 0.04, pos[1] + 0.03, sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, tag[i].tag) + 0.02 , -0.06))
			{
				exit(0);
				udg_rename_tag_group(i);
				return TRUE;
			}*/
		}
	}
	return FALSE;
}

/*
						udg_rename_tag(uint tag);
extern void		udg_rename_group(uint tag);
*/
boolean p_find_closest_edge_test(double *a, double *b, double x, double y)
{
	double temp, r;
	x = -x;
	y = -y;
	if(a[2] > 0 || b[2] > 0)
		return FALSE;
	if((a[0] - b[0]) * (x - b[0]) + (a[1] - b[1]) * (y - b[1]) < 0)
		return FALSE;
	if((b[0] - a[0]) * (x - a[0]) + (b[1] - a[1]) * (y - a[1]) < 0)
		return FALSE;
	r = sqrt((b[1] - a[1]) * (b[1] - a[1]) + -(b[0] - a[0]) * -(b[0] - a[0]));
	temp = (x - a[0]) * ((b[1] - a[1]) / r) + (y - a[1]) * (-(b[0] - a[0]) / r);
	if(temp > 0.008 || temp < -0.008 || r < 0.0001)
		return FALSE;
	return TRUE;
}

boolean p_find_closest_edge(uint *edge, double *snap, double x, double y)
{
	double *vertex;
	double a[3], b[3], c[3], d[3];
	uint32 vertex_count, ref_count, *ref, i;
	udg_get_geometry(&vertex_count, &ref_count, &vertex, &ref, NULL);
	ref_count *= 4;

	edge[0] = -1;
	edge[1] = -1;

	for(i = 0; i < ref_count; i += 4)
	{
		if(ref[i] < vertex_count && vertex[ref[i] * 3] != V_REAL64_MAX && ref[i + 1] < vertex_count && vertex[ref[i + 1] * 3] != V_REAL64_MAX && ref[i + 2] < vertex_count && vertex[ref[i + 2] * 3] != V_REAL64_MAX)
		{
			p_get_projection_screen(a, vertex[ref[i] * 3], vertex[ref[i] * 3 + 1], vertex[ref[i] * 3 + 2]);
			p_get_projection_screen(b, vertex[ref[i + 1] * 3], vertex[ref[i + 1] * 3 + 1], vertex[ref[i + 1] * 3 + 2]);
			p_get_projection_screen(c, vertex[ref[i + 2] * 3], vertex[ref[i + 2] * 3 + 1], vertex[ref[i + 2] * 3 + 2]);
			if((a[0] - b[0]) * (c[1] - b[1]) + (a[1] - b[1]) * (b[0] - c[0]) > 0)
			{
				if(p_find_closest_edge_test(a, b, x, y))
				{
					edge[0] = ref[i];
					edge[1] = ref[i + 1];
					break;
				}
				if(p_find_closest_edge_test(b, c, x, y))
				{
					edge[0] = ref[i + 1];
					edge[1] = ref[i + 2];
					break;
				}
				if(ref[i + 3] < vertex_count && vertex[ref[i + 3] * 3] != V_REAL64_MAX)
				{
					p_get_projection_screen(d, vertex[ref[i + 3] * 3], vertex[ref[i + 3] * 3 + 1], vertex[ref[i + 3] * 3 + 2]);
					if(p_find_closest_edge_test(c, d, x, y))
					{
						edge[0] = ref[i + 2];
						edge[1] = ref[i + 3];
						break;
					}
					if(p_find_closest_edge_test(a, d, x, y))
					{
						edge[0] = ref[i + 3];
						edge[1] = ref[i + 0];
						break;
					}
				}else if(p_find_closest_edge_test(c, a, x, y))
				{
					edge[0] = ref[i + 2];
					edge[1] = ref[i + 0];
					break;
				}
			}
		}
	}
	if(edge[0] == -1)
	{
		ref = udg_get_edge_data(&ref_count);
		for(i = 0; i < ref_count; i++)
		{
			p_get_projection_screen(a, vertex[ref[i * 2] * 3], vertex[ref[i * 2] * 3 + 1], vertex[ref[i * 2] * 3 + 2]);
			p_get_projection_screen(b, vertex[ref[i * 2 + 1] * 3], vertex[ref[i * 2 + 1] * 3 + 1], vertex[ref[i * 2 + 1] * 3 + 2]);
			if(p_find_closest_edge_test(a, b, x, y))
			{
				edge[0] = ref[i * 2];
				edge[1] = ref[i * 2 + 1];
				break;
			}
		}
	}
	if(edge[0] != -1)
	{
		snap[0] = (vertex[edge[0] * 3 + 0] + vertex[edge[1] * 3 + 0]) * 0.5;
		snap[1] = (vertex[edge[0] * 3 + 1] + vertex[edge[1] * 3 + 1]) * 0.5;
		snap[2] = (vertex[edge[0] * 3 + 2] + vertex[edge[1] * 3 + 2]) * 0.5;
		a[0] = vertex[edge[0] * 3 + 0] - vertex[edge[1] * 3 + 0];
		a[1] = vertex[edge[0] * 3 + 1] - vertex[edge[1] * 3 + 1];
		a[2] = vertex[edge[0] * 3 + 2] - vertex[edge[1] * 3 + 2];
		snap[3] = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);

		printf("v 1 = %f %f %f\n", vertex[edge[1] * 3 + 0],
					vertex[edge[1] * 3 + 1],
					vertex[edge[1] * 3 + 2]);
		printf("v 2 = %f %f %f\n", vertex[edge[0] * 3 + 0],
					vertex[edge[0] * 3 + 1],
					vertex[edge[0] * 3 + 2]);
		printf("a = %f %f %f\n", a[0], a[1], a[2]);
		printf("snap[3] = %f\n", snap[3]);
		return TRUE;
	}
	return FALSE;
}
