#include "la_includes.h"

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
	ProjectionData.distance = 2;
	ProjectionData.distance_target = 2;
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
				dv3_copy(ProjectionData.target, ProjectionData.position);
			}else if(input->mouse_button[0] == TRUE)
			{
				a = ProjectionData.distance + ((input->pointer_x - ProjectionData.grab[0]) - (input->pointer_y - ProjectionData.grab[1])) * 2;
				if(a > 90)
					a = 90;
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
	boolean rotate = FALSE;
	double *vertex, normal[3] = {0, 0, 0}, a[3], temp, size = 0.001;
	ProjectionData.speed = 0.95;
	dv3_copy(ProjectionData.target, center);
	udg_get_geometry(&vertex_count, &polygon_count, &vertex, &ref, NULL);
	for(i = 0; i < polygon_count; i++)
	{
		if(ref[i * 4] < vertex_count && 0.01 < udg_get_select(ref[i * 4]) && ref[i * 4 + 1] < vertex_count && 0.01 < udg_get_select(ref[i * 4 + 1]) && ref[i * 4 + 2] < vertex_count && 0.01 < udg_get_select(ref[i * 4 + 2]) && (ref[i * 4 + 3] > vertex_count || 0.01 < udg_get_select(ref[i * 4 + 3])))
		{		
			dv3_3point_cross(a, &vertex[ref[i * 4 + 1] * 3], &vertex[ref[i * 4] * 3], &vertex[ref[i * 4 + 2] * 3]);
			dv3_add(normal, a);
			rotate = TRUE;
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

		temp = dv3_length(normal);
		ProjectionData.pitch_target = 360 * atan(normal[1] / temp) / 6.28;
	}

	for(i = 0; i < vertex_count; i++)
	{
		if(vertex[i * 3] != V_REAL64_MAX && 0.01 < udg_get_select(i))
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
	dv3_copy(center, ProjectionData.position);
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

void p_op_lod_set_camers_pos(double *cam);

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
	{
		double pos[3];
		pos[0] = ProjectionData.model[2] * ProjectionData.distance + ProjectionData.position[0];
		pos[1] = ProjectionData.model[6] * ProjectionData.distance + ProjectionData.position[1];
		pos[2] = ProjectionData.model[10] * ProjectionData.distance + ProjectionData.position[2];
		p_op_lod_set_camers_pos(pos);
	}
}


void p_get_projection(double *output, float x, float y)
{
	output[0] = ProjectionData.model[0] * x * ProjectionData.distance;
	output[1] = ProjectionData.model[4] * x * ProjectionData.distance;
	output[2] = ProjectionData.model[8] * x * ProjectionData.distance;
	output[0] += ProjectionData.model[1] * y * ProjectionData.distance;
	output[1] += ProjectionData.model[5] * y * ProjectionData.distance;
	output[2] += ProjectionData.model[9] * y * ProjectionData.distance;
	dv3_add(output, ProjectionData.position);
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
	dv3_add(output, ProjectionData.position);

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
	dv3_add(a, ProjectionData.position);
	b[0] = ProjectionData.model[2] * ProjectionData.distance;
	b[1] = ProjectionData.model[6] * ProjectionData.distance;
	b[2] = ProjectionData.model[10] * ProjectionData.distance;
	dv3_add(b, ProjectionData.position);
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
	dv3_add(a, b);
	dv3_add(b, ProjectionData.position);
	dv3_add(a, pos);
	r = sqrt(b[(axis + 1) % 3] * b[(axis + 1) % 3] + b[(axis + 2) % 3] * b[(axis + 2) % 3]);
	r2 = sqrt(a[(axis + 1) % 3] * a[(axis + 1) % 3] + a[(axis + 2) % 3] * a[(axis + 2) % 3]);
	r3 = b[axis] + (a[axis] * r / r2);
	if(dist != NULL)
	{
		dv3_copy(a, pos);
		a[axis] += r3;
		point_threw_matrix3(ProjectionData.model, &a[0], &a[1], &a[2]);
		a[0] = (a[0] / a[2]) + pointer_x;
		a[1] = (a[1] / a[2]) + pointer_y;
		*dist = dv2_dot(a, a);
	}
	return r3;
}

void p_get_projection_vertex_with_axis(double *output, double *start, double pointer_x, double pointer_y, boolean snap, double *closest)
{
	double dist, best_dist, pos, best_pos;
	//static uint axis; 
	uint i; 
	if(snap != TRUE)
	{
		best_pos = p_get_projection_line(&best_dist, 0, pointer_x, pointer_y, start);
		ProjectionData.axis = 0;
		for(i = 1; i < 3; i++)
		{
			pos = p_get_projection_line(&dist, i, pointer_x, pointer_y, start);
			if(dist < best_dist)
			{
				best_pos = pos;
				best_dist = dist;
				ProjectionData.axis = i;
			}
		}
		if(ProjectionData.view_axis < 3)
		{	
			printf("ProjectionData.view_axis %u\n", ProjectionData.view_axis);
			p_get_projection_plane(output, ProjectionData.view_axis, pointer_x, pointer_y, start[ProjectionData.view_axis]);
			return;
		}
		if(best_dist > 0.0001)
		{
			p_get_projection_vertex(output, start, pointer_x, pointer_y);
			return;
		}
	}
	dv2_copy(output, start);
	if(snap != TRUE)
		output[ProjectionData.axis] += best_pos;
	else
	{
        best_pos = p_get_projection_line(&best_dist, ProjectionData.axis, pointer_x, pointer_y, start);
		if(best_dist < 0.0001)
			output[ProjectionData.axis] += (double)((int)(best_pos * ProjectionData.grid_size)) / ProjectionData.grid_size;
		else
			output[ProjectionData.axis] = closest[ProjectionData.axis];
	}
}

uint p_get_projection_axis(void)
{
	return ProjectionData.axis;
}

/*
typedef struct{
	uint	*vertex;
	double	*distance;
	double	x;
	double	y;
}GNFindClosestParam;


void find_cosest_func(uint id, Point *vertex, GNFindClosestParam *user)
{
	double distance;
	p_get_projection_screen(pos, vertex->x, vertex->y, vertex->z);
	distance = (pos[0] + user->x) * (pos[0] + user->x) + (pos[1] + user->y) * (pos[1] + user->y);
	if(*user->distance > distance)
	{
		*user->distance = distance;
		*user->vertex = id;
	}
}

*/
void p_find_closest_vertex(uint *closest, uint *selected, double *distance, double *selected_distance, double x, double y)
{
	uint32 vertex_count, i;
	double	*vertex;
	double	pos[3], r;
	udg_get_geometry(&vertex_count, NULL, &vertex, NULL, NULL);

	for(i = 0; i < vertex_count; i++)
	{
		if(vertex[i * 3] < V_REAL64_MAX)
		{
			p_get_projection_screen(pos, vertex[i * 3], vertex[i * 3 + 1], vertex[i * 3 + 2]);
			if(pos[2] < 0)
			{
				pos[0] += x;
				pos[1] += y;
				r = dv2_dot(pos, pos);
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
	if(temp > 0.008 || temp < -0.008)
		return FALSE;
	return TRUE;
}

void p_find_closest_edge(uint *edge, double x, double y)
{
	double *vertex, a[3], b[3], c[3], d[3];
	uint32 vertex_count, ref_count, *ref, i;
	udg_get_geometry(&vertex_count, &ref_count, &vertex, &ref, NULL);
	ref_count *= 4;

	edge[0] = -1;
	edge[1] = -1;

	for(i = 0; i < ref_count; i += 4)
	{
		if(ref[i] < vertex_count && ref[i + 1] < vertex_count && ref[i + 2] < vertex_count)
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
					return;
				}
				if(p_find_closest_edge_test(b, c, x, y))
				{
					edge[0] = ref[i + 1];
					edge[1] = ref[i + 2];
					return;
				}
				if(ref[i + 3] < vertex_count)
				{
					p_get_projection_screen(d, vertex[ref[i + 3] * 3], vertex[ref[i + 3] * 3 + 1], vertex[ref[i + 3] * 3 + 2]);
					if(p_find_closest_edge_test(c, d, x, y))
					{
						edge[0] = ref[i + 2];
						edge[1] = ref[i + 3];
						return;
					}
					if(p_find_closest_edge_test(a, d, x, y))
					{
						edge[0] = ref[i + 3];
						edge[1] = ref[i + 0];
						return;
					}
				}else if(p_find_closest_edge_test(c, a, x, y))
				{
					edge[0] = ref[i + 2];
					edge[1] = ref[i + 0];
					return;
				}
			}
		}
	}
	ref = udg_get_edge_data(&ref_count);
	for(i = 0; i < ref_count; i++)
	{
		p_get_projection_screen(a, vertex[ref[i * 2] * 3], vertex[ref[i * 2] * 3 + 1], vertex[ref[i * 2] * 3 + 2]);
		p_get_projection_screen(b, vertex[ref[i * 2 + 1] * 3], vertex[ref[i * 2 + 1] * 3 + 1], vertex[ref[i * 2 + 1] * 3 + 2]);
		if(p_find_closest_edge_test(a, b, x, y))
		{
			edge[0] = ref[i * 2];
			edge[1] = ref[i * 2 + 1];
			return;
		}
	}
}