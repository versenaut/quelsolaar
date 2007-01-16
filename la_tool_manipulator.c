#include "la_includes.h"

#include "st_matrix_operations.h"
#include "la_geometry_undo.h"
#include "la_projection.h"
#include "la_draw_overlay.h"
#include "la_tool.h"
#include "la_particle_fx.h"

#define PI  3.141592653

#define ROTATE_GRID_SPLITS  32
#define ROTATE_GRID_DEGREES  36

typedef enum{
	TMM_IDLE,
	TMM_PLACE,
	TMM_TRANSFORM,
	TMM_ROTATE,
	TMM_SCALE,
	TMM_NORMAL,
	TMM_TANGENT,
	TMM_SMOOTH
}TManipMode;

struct{
	float		*manipulator_array;
	float		*manipulator_normal;
	float		*manipulator_color;
	float		*manipulator_circle;
	float		*manipulator_grid;
	float		*manipulator_normal_array;
	uint		manipulator_normal_array_length;
	float		*manipulator_rings;
	float		*manipulator_rings_normals;
	float		*manipulator_rings_color;
	float		*manipulator_rings_shadow;
	double		manipulator_pos[3];
	double		manipulator_start[3];
	double		manipulator_size;
	double		manipulator_scale[3];
	double		manipulator_rotate_scale;
	TManipMode	mode;
	double		grab_pos;
	double		grab_manip_pos;
	uint		grab_axis;
	double		*data;
	double		*normal;
	uint		data_length;
	double		*tags;
	uint		tag_length;
	boolean		hide;
	double		min_scale[3];
	double		max_scale[3];
	uint		update_rotate;
}GlobalTransformManipulator;

void la_t_tm_init(void)
{
	uint i, j, k;
	float square[8];
	GlobalTransformManipulator.manipulator_normal_array = NULL;
	GlobalTransformManipulator.manipulator_normal_array_length = 0;
	GlobalTransformManipulator.manipulator_circle = malloc((sizeof *GlobalTransformManipulator.manipulator_circle) * (ROTATE_GRID_DEGREES * 6 + 24) * 3);
	GlobalTransformManipulator.manipulator_grid = malloc((sizeof *GlobalTransformManipulator.manipulator_grid) * (ROTATE_GRID_DEGREES * 4 + ROTATE_GRID_SPLITS * 4) * 3);
	k = 0;
	for(i = 0; i < ROTATE_GRID_DEGREES; i++)
	{
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_grid, k++, 0.96 * sin(2 * PI * (float)i / ROTATE_GRID_DEGREES + 0.01), 0.96 * cos(2 * PI * (float)i / ROTATE_GRID_DEGREES + 0.01), -0.002);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_grid, k++, 0.99 * sin(2 * PI * (float)i / ROTATE_GRID_DEGREES + 0.006), 0.99 * cos(2 * PI * (float)i / ROTATE_GRID_DEGREES + 0.006), -0.018);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_grid, k++, 0.99 * sin(2 * PI * (float)i / ROTATE_GRID_DEGREES - 0.006), 0.99 * cos(2 * PI * (float)i / ROTATE_GRID_DEGREES - 0.006), -0.018);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_grid, k++, 0.96 * sin(2 * PI * (float)i / ROTATE_GRID_DEGREES - 0.01), 0.96 * cos(2 * PI * (float)i / ROTATE_GRID_DEGREES - 0.01), -0.002);


		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_circle, i * 6 + 0, 1 * sin(2 * PI * (float)i / ROTATE_GRID_DEGREES), 1 * cos(2 * PI * (float)i / ROTATE_GRID_DEGREES), -0.01);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_circle, i * 6 + 1, 1 * sin(2 * PI * (float)(i + 1) / ROTATE_GRID_DEGREES), 1 * cos(2 * PI * (float)(i + 1) / ROTATE_GRID_DEGREES), -0.01);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_circle, i * 6 + 2, 0.95 * sin(2 * PI * (float)i / ROTATE_GRID_DEGREES), 0.95 * cos(2 * PI * (float)i / ROTATE_GRID_DEGREES), -0.0);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_circle, i * 6 + 3, 0.95 * sin(2 * PI * (float)(i + 1) / ROTATE_GRID_DEGREES), 0.95 * cos(2 * PI * (float)(i + 1) / ROTATE_GRID_DEGREES), -0.0);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_circle, i * 6 + 4, 0.9 * sin(2 * PI * (float)i / ROTATE_GRID_DEGREES), 0.9 * cos(2 * PI * (float)i / ROTATE_GRID_DEGREES), -0.015);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_circle, i * 6 + 5, 0.9 * sin(2 * PI * (float)(i + 1) / ROTATE_GRID_DEGREES), 0.9 * cos(2 * PI * (float)(i + 1) / ROTATE_GRID_DEGREES), -0.015);
	}

	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_circle, i * 6 + 0, 0, 0, 1);
	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_circle, i * 6 + 1, 0, 0, 0.01);
	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_circle, i * 6 + 2, 0, 0, -0.01);
	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_circle, i * 6 + 3, 0, 0, -1);

	for(i = 0; i < ROTATE_GRID_SPLITS; i++)
	{
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_grid, k++, 0.94 * sin(2 * PI * (float)i / ROTATE_GRID_SPLITS + 0.02), 0.94 * cos(2 * PI * (float)i / ROTATE_GRID_SPLITS + 0.02), -0.002);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_grid, k++, 0.94 * sin(2 * PI * (float)i / ROTATE_GRID_SPLITS - 0.02), 0.94 * cos(2 * PI * (float)i / ROTATE_GRID_SPLITS - 0.02), -0.002);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_grid, k++, 0.91 * sin(2 * PI * (float)i / ROTATE_GRID_SPLITS - 0.01), 0.91 * cos(2 * PI * (float)i / ROTATE_GRID_SPLITS - 0.01), -0.028);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_grid, k++, 0.91 * sin(2 * PI * (float)i / ROTATE_GRID_SPLITS + 0.01), 0.91 * cos(2 * PI * (float)i / ROTATE_GRID_SPLITS + 0.01), -0.028);
	}
	k = 0;
	GlobalTransformManipulator.manipulator_array = malloc((sizeof *GlobalTransformManipulator.manipulator_array) * (12 + 24 * 6) * 3);
	GlobalTransformManipulator.manipulator_normal = malloc((sizeof *GlobalTransformManipulator.manipulator_normal) * (12 + 24 * 6) * 3);

	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal, k, 0, 1, 1);
	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_array, k++, 0.3, 0, 0.01);
	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal, k, 0, 1, 1);
	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_array, k++, 0.1, 0, 0.014);
	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal, k, 0, 1, 1);
	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_array, k++, 0.1, 0.014, 0);
	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal, k, 0, 1, 1);
	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_array, k++, 0.3, 0.01, 0);

	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal, k, 1, 0, 1);
	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_array, k++, 0, 0.1, 0.014);
	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal, k, 1, 0, 1);
	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_array, k++, 0, 0.3, 0.01);
	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal, k, 1, 0, 1);
	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_array, k++, 0.01, 0.3, 0);
	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal, k, 1, 0, 1);
	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_array, k++, 0.014, 0.1, 0);

	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal, k, 1, 12, 0);
	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_array, k++, 0.014, 0, 0.1);
	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal, k, 1, 1, 0);
	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_array, k++, 0.01, 0, 0.3);
	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal, k, 1, 1, 0);
	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_array, k++, 0, 0.01, 0.3);
	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal, k, 1, 1, 0);
	sui_draw_set_vec3(GlobalTransformManipulator.manipulator_array, k++, 0, 0.014, 0.1);

	for(i = 1; i < 13; i++)
	{
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal, k, 1, 0, 0);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_array, k++, 0, 0.3 * sin(0.5 * PI * (float)i / 14), 0.3 * cos(0.5 * PI * (float)i / 14));
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal, k, 1, 0, 0);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_array, k++, 0, 0.3 * sin(0.5 * PI * (float)(i + 1) / 14), 0.3 * cos(0.5 * PI * (float)(i + 1) / 14));
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal, k, 1, 0, 0);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_array, k++, 0, 0.29 * sin(0.5 * PI * (float)(i + 1) / 14), 0.29 * cos(0.5 * PI * (float)(i + 1) / 14));
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal, k, 1, 0, 0);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_array, k++, 0, 0.29 * sin(0.5 * PI * (float)i / 14), 0.29 * cos(0.5 * PI * (float)i / 14));

		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal, k, 0, 1, 0);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_array, k++, 0.3 * sin(0.5 * PI * (float)i / 14), 0, 0.3 * cos(0.5 * PI * (float)i / 14));
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal, k, 0, 1, 0);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_array, k++, 0.3 * sin(0.5 * PI * (float)(i + 1) / 14), 0, 0.3 * cos(0.5 * PI * (float)(i + 1) / 14));
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal, k, 0, 1, 0);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_array, k++, 0.29 * sin(0.5 * PI * (float)(i + 1) / 14), 0, 0.29 * cos(0.5 * PI * (float)(i + 1) / 14));
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal, k, 0, 1, 0);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_array, k++, 0.29 * sin(0.5 * PI * (float)i / 14), 0, 0.29 * cos(0.5 * PI * (float)i / 14));

		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal, k, 0, 0, 1);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_array, k++, 0.3 * sin(0.5 * PI * (float)i / 14), 0.3 * cos(0.5 * PI * (float)i / 14), 0);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal, k, 0, 0, 1);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_array, k++, 0.3 * sin(0.5 * PI * (float)(i + 1) / 14), 0.3 * cos(0.5 * PI * (float)(i + 1) / 14), 0);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal, k, 0, 0, 1);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_array, k++, 0.29 * sin(0.5 * PI * (float)(i + 1) / 14), 0.29 * cos(0.5 * PI * (float)(i + 1) / 14), 0);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal, k, 0, 0, 1);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_array, k++, 0.29 * sin(0.5 * PI * (float)i / 14), 0.29 * cos(0.5 * PI * (float)i / 14), 0);	
	}

	GlobalTransformManipulator.manipulator_rings = malloc((sizeof *GlobalTransformManipulator.manipulator_array) * 4 * (ROTATE_GRID_DEGREES + 6) * 2);
	GlobalTransformManipulator.manipulator_rings_normals =  malloc((sizeof *GlobalTransformManipulator.manipulator_array) * 4 * (ROTATE_GRID_DEGREES + 6) * 3);
	GlobalTransformManipulator.manipulator_rings_shadow = malloc((sizeof *GlobalTransformManipulator.manipulator_array) *  16 * (ROTATE_GRID_DEGREES + 6) * 2);
	GlobalTransformManipulator.manipulator_rings_color = malloc((sizeof *GlobalTransformManipulator.manipulator_array) * 16 * (ROTATE_GRID_DEGREES + 6) * 4);
	for(i = 0; i < ROTATE_GRID_DEGREES; i++)
	{
		square[0] =  0.35 * sin(2 * PI * (float)i / ROTATE_GRID_DEGREES);
		square[1] = 0.35 * cos(2 * PI * (float)i / ROTATE_GRID_DEGREES);
		square[2] = 0.33 * sin(2 * PI * (float)i / ROTATE_GRID_DEGREES);
		square[3] = 0.33 * cos(2 * PI * (float)i / ROTATE_GRID_DEGREES);
		square[4] = 0.33 * sin(2 * PI * ((float)i + 0.5) / ROTATE_GRID_DEGREES);
		square[5] = 0.33 * cos(2 * PI * ((float)i + 0.5) / ROTATE_GRID_DEGREES);
		square[6] = 0.35 * sin(2 * PI * ((float)i + 0.5) / ROTATE_GRID_DEGREES);
		square[7] = 0.35 * cos(2 * PI * ((float)i + 0.5) / ROTATE_GRID_DEGREES);

		sui_draw_set_vec2(GlobalTransformManipulator.manipulator_rings, i * 4 + 0, square[0], square[1]);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_rings_normals, i * 4 + 0, square[0], square[1], -5);
		sui_draw_set_vec2(GlobalTransformManipulator.manipulator_rings, i * 4 + 1, square[2], square[3]);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_rings_normals, i * 4 + 1, square[2], square[3], -5);
		sui_draw_set_vec2(GlobalTransformManipulator.manipulator_rings, i * 4 + 2, square[4], square[5]);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_rings_normals, i * 4 + 2, square[4], square[5], -5);
		sui_draw_set_vec2(GlobalTransformManipulator.manipulator_rings, i * 4 + 3, square[6], square[7]);
		sui_draw_set_vec3(GlobalTransformManipulator.manipulator_rings_normals, i * 4 + 3, square[6], square[7], -5);
		sui_create_shadow_edge(-0.02, 4, &GlobalTransformManipulator.manipulator_rings_shadow[16 * i * 2], &GlobalTransformManipulator.manipulator_rings_color[16 * i * 4], square);
	}

	for(; i < ROTATE_GRID_DEGREES + 6; i++)
	{
		for(j = 0; j < 2; j++)
		{
			square[0] = -0.4 * cos(2 * PI * ((float)i - 2.4 + (float)j * 0.8) / 18);
			square[1] = 0.4 * sin(2 * PI * ((float)i - 2.4 + (float)j * 0.8) / 18);
			square[2] = -0.5 * cos(2 * PI * ((float)i - 2.2 + (float)j * 0.4) / 18);
			square[3] = 0.5 * sin(2 * PI * ((float)i - 2.2 + (float)j * 0.4) / 18);
			square[4] = -0.5 * cos(2 * PI * ((float)i - 2.1 + (float)j * 0.2) / 18);
			square[5] = 0.5 * sin(2 * PI * ((float)i - 2.1 + (float)j * 0.2) / 18);
			square[6] = -0.4 * cos(2 * PI * ((float)i - 2.2 + (float)j * 0.4) / 18);
			square[7] = 0.4 * sin(2 * PI * ((float)i - 2.2 + (float)j * 0.4) / 18);
			i += j; 
			sui_draw_set_vec2(GlobalTransformManipulator.manipulator_rings, i * 4 + 0, square[0], square[1]);
			sui_draw_set_vec3(GlobalTransformManipulator.manipulator_rings_normals, i * 4 + 0, square[0], square[1], -5);
			sui_draw_set_vec2(GlobalTransformManipulator.manipulator_rings, i * 4 + 1, square[2], square[3]);
			sui_draw_set_vec3(GlobalTransformManipulator.manipulator_rings_normals, i * 4 + 1, square[2], square[3], -5);
			sui_draw_set_vec2(GlobalTransformManipulator.manipulator_rings, i * 4 + 2, square[4], square[5]);
			sui_draw_set_vec3(GlobalTransformManipulator.manipulator_rings_normals, i * 4 + 2, square[4], square[5], -5);
			sui_draw_set_vec2(GlobalTransformManipulator.manipulator_rings, i * 4 + 3, square[6], square[7]);
			sui_draw_set_vec3(GlobalTransformManipulator.manipulator_rings_normals, i * 4 + 3, square[6], square[7], -5);
			sui_create_shadow_edge(0.02 - (float)j * 0.04, 4, &GlobalTransformManipulator.manipulator_rings_shadow[16 * i * 2], &GlobalTransformManipulator.manipulator_rings_color[16 * i * 4], square);
		}
	}


	GlobalTransformManipulator.manipulator_pos[0] = 0;
	GlobalTransformManipulator.manipulator_pos[1] = 0;
	GlobalTransformManipulator.manipulator_pos[2] = 0;
	GlobalTransformManipulator.manipulator_size = 1;
	GlobalTransformManipulator.manipulator_rotate_scale = 1;
	GlobalTransformManipulator.mode = TMM_IDLE;
	GlobalTransformManipulator.data = NULL;
	GlobalTransformManipulator.hide = TRUE;
	GlobalTransformManipulator.tags = NULL;
	GlobalTransformManipulator.tag_length = 0;
}

void la_t_tm_place(double x, double y, double z)
{
	GlobalTransformManipulator.manipulator_pos[0] = x;
	GlobalTransformManipulator.manipulator_pos[1] = y;
	GlobalTransformManipulator.manipulator_pos[2] = z;
}
void la_t_tm_get_pos(double *pos)
{
	pos[0] = GlobalTransformManipulator.manipulator_pos[0];
	pos[1] = GlobalTransformManipulator.manipulator_pos[1];
	pos[2] = GlobalTransformManipulator.manipulator_pos[2];
}
void la_t_tm_get_vector(double *vector)
{
	double r;

	p_get_view_camera(vector);
	dv3_sub(vector, GlobalTransformManipulator.manipulator_pos);
	r = sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
	vector[0] = (double)((uint)(vector[0] * 1.6 / r));
	vector[1] = (double)((uint)(vector[1] * 1.6 / r));
	vector[2] = (double)((uint)(vector[2] * 1.6 / r));
	dv3_normalize(vector);
}

void la_t_tm_hide(boolean hide)
{
	GlobalTransformManipulator.hide = hide;
}

void la_t_tm_draw(BInputState *input, boolean active)
{
	uint i;
	if(GlobalTransformManipulator.hide)
		return;
	if(active != TRUE)
	{
		double camera[3];
		p_get_projection_screen(camera, GlobalTransformManipulator.manipulator_pos[0], GlobalTransformManipulator.manipulator_pos[1], GlobalTransformManipulator.manipulator_pos[2]);
		GlobalTransformManipulator.manipulator_size = 0.5 * camera[2]; /*sqrt(camera[0] * camera[0] + camera[1] * camera[1] + camera[2] * camera[2]);*/
		p_get_view_camera(camera);
		camera[0] =	camera[0] - GlobalTransformManipulator.manipulator_pos[0];
		camera[1] = camera[1] - GlobalTransformManipulator.manipulator_pos[1];
		camera[2] = camera[2] - GlobalTransformManipulator.manipulator_pos[2];
		for(i = 0; i < 3; i++)
		{
			if(0 > camera[i])
				GlobalTransformManipulator.manipulator_scale[i] = GlobalTransformManipulator.manipulator_size;
			else
				GlobalTransformManipulator.manipulator_scale[i] = -GlobalTransformManipulator.manipulator_size;
		}
		for(i = 0; i < 3; i++)
		{
			double dist[3];
			p_get_projection_plane(dist, i, input->pointer_x, input->pointer_y, GlobalTransformManipulator.manipulator_pos[i]);
		}
	}
	glDisable(GL_DEPTH_TEST);
	if(GlobalTransformManipulator.mode == TMM_SCALE)
		la_do_xyz_lines(GlobalTransformManipulator.manipulator_pos, input->mouse_button[1]);
	glPushMatrix();	
	glTranslated(GlobalTransformManipulator.manipulator_pos[0], GlobalTransformManipulator.manipulator_pos[1], GlobalTransformManipulator.manipulator_pos[2]);
	glScaled(GlobalTransformManipulator.manipulator_scale[0], GlobalTransformManipulator.manipulator_scale[1], GlobalTransformManipulator.manipulator_scale[2]);

	if(GlobalTransformManipulator.mode == TMM_ROTATE)
	{
		if(GlobalTransformManipulator.grab_axis == 0)
			glRotated(90, 0, 1, 0);
		if(GlobalTransformManipulator.grab_axis == 1)
			glRotated(90, 1, 0, 0);
		sui_draw_gl(GL_LINES, GlobalTransformManipulator.manipulator_circle, (ROTATE_GRID_DEGREES * 6 + 24), 3, 1, 1, 1, 0.0);
		glScaled(1, 1, GlobalTransformManipulator.manipulator_rotate_scale);
		sui_draw_gl(GL_QUADS, GlobalTransformManipulator.manipulator_grid, (ROTATE_GRID_DEGREES * 4 + ROTATE_GRID_SPLITS * 4), 3, 1, 1, 1, 0.0);
		glScaled(-8 * GlobalTransformManipulator.manipulator_size / GlobalTransformManipulator.manipulator_scale[0], -8 * GlobalTransformManipulator.manipulator_size / GlobalTransformManipulator.manipulator_scale[1], -8 * GlobalTransformManipulator.manipulator_size / GlobalTransformManipulator.manipulator_scale[2]);
	}

	
	if(GlobalTransformManipulator.mode == TMM_IDLE || GlobalTransformManipulator.mode == TMM_PLACE)	
	{
		double matrix[16];
//		sui_set_normal_array_gl(GlobalTransformManipulator.manipulator_normal, 12 + 24 * 6);
		sui_draw_gl(GL_QUADS, GlobalTransformManipulator.manipulator_array, 12 + 24 * 6, 3, 1, 1, 1, 0.0);
		p_get_model_matrix(matrix);
		matrix[3] = 0;
		matrix[7] = 0;
		matrix[11] = 0;
		matrix[12] = 0;
		matrix[13] = 0;
		matrix[14] = 0;
		glPopMatrix();
		glPushMatrix();
		reverse_matrix(matrix);
		glTranslated(GlobalTransformManipulator.manipulator_pos[0], GlobalTransformManipulator.manipulator_pos[1], GlobalTransformManipulator.manipulator_pos[2]);
		glMultMatrixd(matrix);
		glScaled(GlobalTransformManipulator.manipulator_size, GlobalTransformManipulator.manipulator_size, GlobalTransformManipulator.manipulator_size);
//		sui_set_normal_array_gl(GlobalTransformManipulator.manipulator_normal, 12 + 24 * 6);
		sui_draw_gl(GL_QUADS, GlobalTransformManipulator.manipulator_rings, 4 * (ROTATE_GRID_DEGREES + 6), 2, 1, 1, 1, 0.0);
		sui_set_blend_gl(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		sui_set_color_array_gl(GlobalTransformManipulator.manipulator_rings_color, 16 * (ROTATE_GRID_DEGREES + 6), 4);
		sui_draw_gl(GL_QUADS, GlobalTransformManipulator.manipulator_rings_shadow, 16 * (ROTATE_GRID_DEGREES + 6), 2, 1, 1, 1, 0.0);
		glPopMatrix();
		glPushMatrix();
	}


	{
		double vector[3], origo[3] = {0, 0, 0}, point[3] = {0.1, 0.1, 1}, matrix[16];
		la_t_tm_get_vector(vector);
		create_matrix_normalized(matrix, origo, vector, point);
		glPopMatrix();
		glPushMatrix();
		glTranslated(GlobalTransformManipulator.manipulator_pos[0], GlobalTransformManipulator.manipulator_pos[1], GlobalTransformManipulator.manipulator_pos[2]);
		glMultMatrixd(matrix);
		glScaled(GlobalTransformManipulator.manipulator_size, GlobalTransformManipulator.manipulator_size, GlobalTransformManipulator.manipulator_size);
		la_pfx_video_flare();
//		sui_draw_gl(GL_QUADS, GlobalTransformManipulator.manipulator_rings, (ROTATE_GRID_DEGREES * 4 + 8), 3, 1, 1, 1);
		glPopMatrix();
		glPushMatrix();
	}
	glPopMatrix();
	if(GlobalTransformManipulator.mode == TMM_NORMAL || GlobalTransformManipulator.mode == TMM_TANGENT || GlobalTransformManipulator.mode == TMM_SMOOTH)	
	{
		sui_draw_gl(GL_LINES, GlobalTransformManipulator.manipulator_normal_array, GlobalTransformManipulator.manipulator_normal_array_length * 2, 3, 1, 1, 1, 0.0);
	}
	glEnable(GL_DEPTH_TEST);
}

void compute_tangent(double *vertex, uint edge_a, uint edge_b, uint other)
{
	float r;
	double normal[3], temp[3];
	edge_a *= 3;
	edge_b *= 3;
	other *= 3;
	dv3o_sub(normal, &vertex[edge_a], &vertex[other]);
	dv3o_sub(temp, &vertex[edge_a], &vertex[edge_b]);
	dv3_normalize(temp);
	r = dv3_dot(normal, temp);
	normal[0] -= temp[0] * r;
	normal[1] -= temp[1] * r;
	normal[2] -= temp[2] * r;
	dv3_normalize(normal);
	dv3_add(&GlobalTransformManipulator.normal[edge_a], normal);
	r = dv3_dot(&GlobalTransformManipulator.normal[edge_a], normal);
	dv3_division(&GlobalTransformManipulator.normal[edge_a], r);
	dv3_add(&GlobalTransformManipulator.normal[edge_b], normal);
	r = dv3_dot(&GlobalTransformManipulator.normal[edge_b], normal);
	dv3_division(&GlobalTransformManipulator.normal[edge_b], r);
}


void lock_transform_vertexes(BInputState *input, boolean normal, boolean tangent, boolean smooth)
{
	UNDOTag *tag;
	uint i, j, *ref, ref_count, vertex_count;
	double select, *vertex;

	udg_get_geometry(&vertex_count, &ref_count, &vertex, &ref, NULL);

	GlobalTransformManipulator.data_length = vertex_count;
	GlobalTransformManipulator.data = malloc((sizeof *GlobalTransformManipulator.data) * GlobalTransformManipulator.data_length * 3);

	for(i = 0; i < 3; i++)
	{
		GlobalTransformManipulator.min_scale[i] = GlobalTransformManipulator.manipulator_pos[i];
		GlobalTransformManipulator.max_scale[i] = GlobalTransformManipulator.manipulator_pos[i];
	}
	for(i = 0; i < GlobalTransformManipulator.data_length; i++)
	{
		GlobalTransformManipulator.data[i * 3] = vertex[i * 3];
		GlobalTransformManipulator.data[i * 3 + 1] = vertex[i * 3 + 1];
		GlobalTransformManipulator.data[i * 3 + 2] = vertex[i * 3 + 2];
		select = udg_get_select(i);
		if(select > 0.0001)
		{
			if(vertex[i * 3] > GlobalTransformManipulator.max_scale[0])
				GlobalTransformManipulator.max_scale[0] = vertex[i * 3];
			if(vertex[i * 3] < GlobalTransformManipulator.min_scale[0])
				GlobalTransformManipulator.min_scale[0] = vertex[i * 3];

			if(vertex[i * 3 + 1] > GlobalTransformManipulator.max_scale[1])
				GlobalTransformManipulator.max_scale[1] = vertex[i * 3 + 1];
			if(vertex[i * 3 + 1] < GlobalTransformManipulator.min_scale[1])
				GlobalTransformManipulator.min_scale[1] = vertex[i * 3 + 1];

			if(vertex[i * 3 + 2] > GlobalTransformManipulator.max_scale[2])
				GlobalTransformManipulator.max_scale[2] = vertex[i * 3 + 2];
			if(vertex[i * 3 + 2] < GlobalTransformManipulator.min_scale[2])
				GlobalTransformManipulator.min_scale[2] = vertex[i * 3 + 2];					
		}
	}
	GlobalTransformManipulator.normal = NULL;
	if(normal)
	{
		double x, y, z, r;
		uint *pos, poly;
		GlobalTransformManipulator.normal = malloc((sizeof *GlobalTransformManipulator.normal) * vertex_count * 3);
		for(i = 0; i < vertex_count; i++)
		{
			GlobalTransformManipulator.normal[i * 3] = 0;
			GlobalTransformManipulator.normal[i * 3 + 1] = 0;
			GlobalTransformManipulator.normal[i * 3 + 2] = 0;
		}
		pos = ref;
		for(i = 0; i < ref_count; pos += 4)
		{
			i++;
			if(pos[0] < vertex_count &&
			   pos[1] < vertex_count &&
			   pos[2] < vertex_count &&
			   vertex[pos[0] * 3] != V_REAL64_MAX &&
			   vertex[pos[1] * 3] != V_REAL64_MAX &&
			   vertex[pos[2] * 3] != V_REAL64_MAX)
			{
				if(pos[3] < vertex_count && vertex[pos[3] * 3] != V_REAL64_MAX)
					poly = 4;
				else
					poly = 3;
				if(udg_get_select(pos[0]) > 0.01 && udg_get_select(pos[1]) > 0.01 && udg_get_select(pos[2]) > 0.01 && (poly == 3 || udg_get_select(pos[3]) > 0.01))
				{
					x = ((vertex[pos[0] * 3 + 1] - vertex[pos[1] * 3 + 1]) * (vertex[pos[2] * 3 + 2] - vertex[pos[1] * 3 + 2]) - (vertex[pos[0] * 3 + 2] - vertex[pos[1] * 3 + 2]) * (vertex[pos[2] * 3 + 1] - vertex[pos[1] * 3 + 1]));
					y = ((vertex[pos[0] * 3 + 2] - vertex[pos[1] * 3 + 2]) * (vertex[pos[2] * 3 + 0] - vertex[pos[1] * 3 + 0]) - (vertex[pos[0] * 3 + 0] - vertex[pos[1] * 3 + 0]) * (vertex[pos[2] * 3 + 2] - vertex[pos[1] * 3 + 2]));
					z = ((vertex[pos[0] * 3 + 0] - vertex[pos[1] * 3 + 0]) * (vertex[pos[2] * 3 + 1] - vertex[pos[1] * 3 + 1]) - (vertex[pos[0] * 3 + 1] - vertex[pos[1] * 3 + 1]) * (vertex[pos[2] * 3 + 0] - vertex[pos[1] * 3 + 0]));
					r = sqrt(x * x + y * y + z * z);
					x = x / r;
					y = y / r;
					z = z / r;
					for(j = 0; j < poly; j++)
					{
						GlobalTransformManipulator.normal[pos[j] * 3] += x;
						GlobalTransformManipulator.normal[pos[j] * 3 + 1] += y;
						GlobalTransformManipulator.normal[pos[j] * 3 + 2] += z;
					}
				}
			}
		}
		pos = ref;
		for(i = 0; i < vertex_count; i++)
		{
			x = GlobalTransformManipulator.normal[i * 3];
			y = GlobalTransformManipulator.normal[i * 3 + 1];
			z = GlobalTransformManipulator.normal[i * 3 + 2];
			r = x * x + y * y + z * z;
			if(r > 0.001)
			{
				r = sqrt(r);
				GlobalTransformManipulator.normal[i * 3] = x / r;
				GlobalTransformManipulator.normal[i * 3 + 1] = y / r;
				GlobalTransformManipulator.normal[i * 3 + 2] = z / r;
			}
		}
	}
	if(tangent)
	{
		uint *pos;
		boolean *edge;
		GlobalTransformManipulator.normal = malloc((sizeof *GlobalTransformManipulator.normal) * vertex_count * 3);
		edge = malloc((sizeof *edge) * vertex_count);
		for(i = 0; i < vertex_count; i++)
		{
			GlobalTransformManipulator.normal[i * 3] = 0;
			GlobalTransformManipulator.normal[i * 3 + 1] = 0;
			GlobalTransformManipulator.normal[i * 3 + 2] = 0;
			edge[i] = FALSE;
		}
		pos = ref;
		for(i = 0; i < ref_count; i++)
		{
			if(pos[3] < vertex_count)
				if(udg_get_select(pos[0]) < 0.01 || udg_get_select(pos[1]) < 0.01 || udg_get_select(pos[2]) < 0.01 || (pos[3] < vertex_count && udg_get_select(pos[3]) < 0.01))
					for(j = 0; j < 4; j++)
						if(pos[j] < vertex_count && udg_get_select(pos[j]) > 0.01)
							edge[pos[j]] = TRUE;	
			pos += 4;
		}
		pos = ref;
		for(i = 0; i < ref_count; i++)
		{
			if(pos[3] < vertex_count)
			{
				if(udg_get_select(pos[0]) > 0.01 && udg_get_select(pos[1]) > 0.01 && udg_get_select(pos[2]) > 0.01 && (pos[3] > vertex_count || udg_get_select(pos[3]) > 0.01))
				{
					if(edge[pos[0]] && edge[pos[1]])
						compute_tangent(vertex, pos[0], pos[1], pos[2]);
					if(edge[pos[1]] && edge[pos[2]])
						compute_tangent(vertex, pos[1], pos[2], pos[0]);
					if(pos[3] >= vertex_count && edge[pos[2]] && edge[pos[0]])
						compute_tangent(vertex, pos[2], pos[0], pos[1]);
					if(pos[3] < vertex_count && edge[pos[2]] && edge[pos[3]])
						compute_tangent(vertex, pos[2], pos[3], pos[0]);
					if(pos[3] < vertex_count && edge[pos[3]] && edge[pos[0]])
						compute_tangent(vertex, pos[3], pos[0], pos[1]);
				}
			}
			pos += 4;
		}
		j = 0;
		for(i = 0; i < vertex_count; i++)
			if(GlobalTransformManipulator.normal[i * 3] != 0 || GlobalTransformManipulator.normal[i * 3 + 1] != 0 || GlobalTransformManipulator.normal[i * 3 + 2] != 0)
				j++;
/*		for(i = 0; i < vertex_count; i++)
		{
			printf("tangent output %f %f %f edge %u\n",GlobalTransformManipulator.normal[i * 3],
			GlobalTransformManipulator.normal[i * 3 + 1],
			GlobalTransformManipulator.normal[i * 3 + 2],
			edge[i]);
		}
*/		free(edge);
	}
	if(smooth)
	{
		uint *count, poly;
		double vertex[3], sum[3];
		GlobalTransformManipulator.normal = malloc((sizeof *GlobalTransformManipulator.normal) * vertex_count * 3);
		count = malloc((sizeof *count) * vertex_count);
		for(i = 0; i < vertex_count; i++)
		{
			GlobalTransformManipulator.normal[i * 3] = 0;
			GlobalTransformManipulator.normal[i * 3 + 1] = 0;
			GlobalTransformManipulator.normal[i * 3 + 2] = 0;
			count[i] = 0;
		}
		for(i = 0; i < ref_count * 4; i += 4)
		{
			if(ref[i] < vertex_count && ref[i + 1] < vertex_count && ref[i + 2] < vertex_count && vertex[ref[i] * 3] != V_REAL64_MAX && vertex[ref[i + 1] * 3] != V_REAL64_MAX && vertex[ref[i + 2] * 3] != V_REAL64_MAX)
			{
				if(ref[i + 3] < vertex_count && vertex[ref[i + 3] * 3] != V_REAL64_MAX)
					poly = 4;
				else
					poly = 3;
				sum[0] = 0;
				sum[1] = 0;
				sum[2] = 0;
				for(j = 0; j < poly; j++)
				{
					udg_get_vertex_pos(vertex, ref[i + j]);
					dv3_add(sum, vertex);
				}
				for(j = 0; j < poly; j++)
				{
					udg_get_vertex_pos(vertex, ref[i + j]);
					GlobalTransformManipulator.normal[ref[i + j] * 3] += sum[0] - vertex[0];
					GlobalTransformManipulator.normal[ref[i + j] * 3 + 1] += sum[1] - vertex[1];
					GlobalTransformManipulator.normal[ref[i + j] * 3 + 2] += sum[2] - vertex[2];
					count[ref[i + j]] += poly - 1;
				}
			}
		}
		for(i = 0; i < vertex_count; i++)
		{
			if(count[i] != 0)
			{
				udg_get_vertex_pos(vertex, i);
				GlobalTransformManipulator.normal[i * 3] = (vertex[0] - (GlobalTransformManipulator.normal[i * 3] / count[i])) / (GlobalTransformManipulator.manipulator_size * 0.4);
				GlobalTransformManipulator.normal[i * 3 + 1] = (vertex[1] - (GlobalTransformManipulator.normal[i * 3 + 1] / count[i])) / (GlobalTransformManipulator.manipulator_size * 0.4);
				GlobalTransformManipulator.normal[i * 3 + 2] = (vertex[2] - (GlobalTransformManipulator.normal[i * 3 + 2] / count[i])) / (GlobalTransformManipulator.manipulator_size * 0.4);
			}
		}
		free(count);
	}
	if(tangent || normal || smooth)
	{
		j = 0;
		for(i = 0; i < vertex_count; i++)
			if(GlobalTransformManipulator.normal[i * 3] != 0 || GlobalTransformManipulator.normal[i * 3 + 1] != 0 || GlobalTransformManipulator.normal[i * 3 + 2] != 0)
				if(udg_get_select(i) > 0.01)
					j++;
		GlobalTransformManipulator.manipulator_normal_array_length = j;
		GlobalTransformManipulator.manipulator_normal_array = malloc((sizeof *GlobalTransformManipulator.manipulator_normal_array) * j * 2 * 3);
		j = 0;
		for(i = 0; i < vertex_count; i++)
		{
			if(GlobalTransformManipulator.normal[i * 3] != 0 || GlobalTransformManipulator.normal[i * 3 + 1] != 0 || GlobalTransformManipulator.normal[i * 3 + 2] != 0)
			{
				if(udg_get_select(i) > 0.01)
				{
					sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal_array, j * 2 + 0, vertex[i * 3] - GlobalTransformManipulator.normal[i * 3] * GlobalTransformManipulator.manipulator_size * 0.4, vertex[i * 3 + 1] - GlobalTransformManipulator.normal[i * 3 + 1] * GlobalTransformManipulator.manipulator_size * 0.4, vertex[i * 3 + 2] - GlobalTransformManipulator.normal[i * 3 + 2] * GlobalTransformManipulator.manipulator_size * 0.4);
					sui_draw_set_vec3(GlobalTransformManipulator.manipulator_normal_array, j * 2 + 1, vertex[i * 3] + GlobalTransformManipulator.normal[i * 3] * GlobalTransformManipulator.manipulator_size * 0.4, vertex[i * 3 + 1] + GlobalTransformManipulator.normal[i * 3 + 1] * GlobalTransformManipulator.manipulator_size * 0.4, vertex[i * 3 + 2] + GlobalTransformManipulator.normal[i * 3 + 2] * GlobalTransformManipulator.manipulator_size * 0.4);
					j++;
				}
			}
		}
	}else
	{
		tag = udg_get_tags(&GlobalTransformManipulator.tag_length);
		for(i = 0; i < GlobalTransformManipulator.tag_length && tag[i].select < 0.01; i++);
		if(i == GlobalTransformManipulator.tag_length)
			return;
		GlobalTransformManipulator.tags = malloc((sizeof *GlobalTransformManipulator.tags) * GlobalTransformManipulator.tag_length * 3);
		for(i = 0; i < GlobalTransformManipulator.tag_length; i++)
		{
			GlobalTransformManipulator.tags[i * 3] = tag[i].vec[0];
			GlobalTransformManipulator.tags[i * 3 + 1] = tag[i].vec[1];
			GlobalTransformManipulator.tags[i * 3 + 2] = tag[i].vec[2];
		}
	}
}

boolean la_t_tm_grab(BInputState *input)
{
	double dist[3], pos, distance;
	uint i;
	if(GlobalTransformManipulator.hide)
		return FALSE;
	for(i = 0; i < 3; i++)
	{
		pos = p_get_projection_line(&distance, i, input->pointer_x, input->pointer_y, GlobalTransformManipulator.manipulator_pos);
		if(pos / GlobalTransformManipulator.manipulator_scale[i] > 0.1 && pos / GlobalTransformManipulator.manipulator_scale[i] < 0.3 && distance < 0.02 * 0.02)
		{
			GlobalTransformManipulator.grab_axis = i;
			GlobalTransformManipulator.grab_pos = pos;
			GlobalTransformManipulator.grab_manip_pos = GlobalTransformManipulator.manipulator_pos[i];
			GlobalTransformManipulator.mode = TMM_TRANSFORM;
			lock_transform_vertexes(input, FALSE, FALSE, FALSE);
			la_pfx_create_bright(GlobalTransformManipulator.manipulator_pos);
			return TRUE;
		}	
	}

	distance = p_get_projection_screen_distance(GlobalTransformManipulator.manipulator_pos[0], GlobalTransformManipulator.manipulator_pos[1], GlobalTransformManipulator.manipulator_pos[2], input->pointer_x, input->pointer_y);
	if(distance < 0.05 * 0.05)
	{
		GlobalTransformManipulator.mode = TMM_PLACE;
		la_pfx_create_bright(GlobalTransformManipulator.manipulator_pos);
		return TRUE;
	}

	for(i = 0; i < 3; i++)
	{
		double a, b,radius;
		p_get_projection_plane(dist, i, input->pointer_x, input->pointer_y, GlobalTransformManipulator.manipulator_pos[i]);
		if((dist[(i + 1) % 3] - GlobalTransformManipulator.manipulator_pos[(i + 1) % 3]) * GlobalTransformManipulator.manipulator_scale[(i + 1) % 3] > 0 &&
			(dist[(i + 2) % 3] - GlobalTransformManipulator.manipulator_pos[(i + 2) % 3]) * GlobalTransformManipulator.manipulator_scale[(i + 2) % 3] > 0)
		{
			a = (dist[(i + 1) % 3] - GlobalTransformManipulator.manipulator_pos[(i + 1) % 3]) / (GlobalTransformManipulator.manipulator_size * 0.3);
			b = (dist[(i + 2) % 3] - GlobalTransformManipulator.manipulator_pos[(i + 2) % 3]) / (GlobalTransformManipulator.manipulator_size * 0.3);
			radius = a * a + b * b;
			if(radius < 1)
			{
				GlobalTransformManipulator.grab_axis = i;
				GlobalTransformManipulator.mode = TMM_ROTATE;
				GlobalTransformManipulator.grab_pos = atan2(dist[(GlobalTransformManipulator.grab_axis + 1) % 3] - GlobalTransformManipulator.manipulator_pos[(GlobalTransformManipulator.grab_axis + 1) % 3], dist[(GlobalTransformManipulator.grab_axis + 2) % 3] - GlobalTransformManipulator.manipulator_pos[(GlobalTransformManipulator.grab_axis + 2) % 3]);
				GlobalTransformManipulator.grab_pos = 0.5 * PI * (double)((int)(GlobalTransformManipulator.grab_pos / (0.5 * PI) + 4.5));
				lock_transform_vertexes(input, FALSE, FALSE, FALSE);
				la_pfx_create_bright(GlobalTransformManipulator.manipulator_pos);
				return TRUE;
			}
		}	
	}
	if(distance < 0.15 * 0.15)
	{
		GlobalTransformManipulator.mode = TMM_SCALE;
		lock_transform_vertexes(input, FALSE, FALSE, FALSE);
		la_pfx_create_bright(GlobalTransformManipulator.manipulator_pos);
		return TRUE;
	}

	p_get_projection_screen(dist, GlobalTransformManipulator.manipulator_pos[0], GlobalTransformManipulator.manipulator_pos[1], GlobalTransformManipulator.manipulator_pos[2]);
	pos = (-input->pointer_x - dist[0]) - (-input->pointer_y - dist[1]);
	if(pos > -0.35 && pos < 0)
	{
		pos = (-input->pointer_x - dist[0]) + (-input->pointer_y - dist[1]);
		if(pos > -0.07 && pos < 0.07)
		{
			GlobalTransformManipulator.mode = TMM_NORMAL;
			GlobalTransformManipulator.grab_manip_pos = ((input->pointer_x - dist[0]) - (input->pointer_y - dist[1])) * GlobalTransformManipulator.manipulator_size;
			lock_transform_vertexes(input, TRUE, FALSE, FALSE);
			return TRUE;
		}
	}
	pos = (-input->pointer_x - dist[0]) + (-input->pointer_y - dist[1]);
	if(pos > -0.35 && pos < 0)
	{
		pos = (-input->pointer_x - dist[0]) - (-input->pointer_y - dist[1]);
		if(pos > -0.07 && pos < 0.07)
		{
			GlobalTransformManipulator.mode = TMM_SMOOTH;
			GlobalTransformManipulator.grab_manip_pos = ((input->pointer_x - dist[0]) + (input->pointer_y - dist[1])) * GlobalTransformManipulator.manipulator_size;
			lock_transform_vertexes(input, FALSE, FALSE, TRUE);
			return TRUE;
		}
	}
	pos = (-input->pointer_x - dist[0]);
	if(pos > -0.25 && pos < 0)
	{
		pos = (-input->pointer_y - dist[1]);
		if(pos > -0.03 && pos < 0.03)
		{
			GlobalTransformManipulator.mode = TMM_TANGENT;
			GlobalTransformManipulator.grab_manip_pos = (input->pointer_x - dist[0]) * -GlobalTransformManipulator.manipulator_size;
			lock_transform_vertexes(input, FALSE, TRUE, FALSE);
			return TRUE;
		}
	}

	GlobalTransformManipulator.mode = TMM_IDLE;
	return FALSE;
}

void la_t_tm_view_center(void)
{
	p_set_view_center(GlobalTransformManipulator.manipulator_pos);
}

boolean la_t_tm_test_center(BInputState *input)
{
	double manip[3], pos;
	if(GlobalTransformManipulator.hide == TRUE)
		return FALSE;
	p_get_projection_screen(manip, GlobalTransformManipulator.manipulator_pos[0], GlobalTransformManipulator.manipulator_pos[1], GlobalTransformManipulator.manipulator_pos[2]);
	if((manip[0] + input->pointer_x) * (manip[0] + input->pointer_x) + (manip[1] + input->pointer_y) * (manip[1] + input->pointer_y) < 0.165 * 0.165)
		return TRUE;
	pos = (-input->pointer_x - manip[0]) - (-input->pointer_y - manip[1]);
	if(pos > 0 && pos < 0.4)
	{
		pos = (-input->pointer_x - manip[0]) + (-input->pointer_y - manip[1]);
		if(pos > -0.05 && pos < 0.05)
			return TRUE;
	}
	return FALSE;
}

void  matrix_func(double *output, uint vertex_id, void *data)
{
	double vertex[3], select;
	select = udg_get_select(vertex_id);
	vertex[0] = GlobalTransformManipulator.data[vertex_id * 3];
	vertex[1] = GlobalTransformManipulator.data[vertex_id * 3 + 1];
	vertex[2] = GlobalTransformManipulator.data[vertex_id * 3 + 2];
	point_threw_matrix3((double *)data, &vertex[0], &vertex[1], &vertex[2]);
	output[0] = vertex[0] * select + GlobalTransformManipulator.data[vertex_id * 3] * (1 - select);
	output[1] = vertex[1] * select + GlobalTransformManipulator.data[vertex_id * 3 + 1] * (1 - select);
	output[2] = vertex[2] * select + GlobalTransformManipulator.data[vertex_id * 3 + 2] * (1 - select);
}
 
void  normal_func(double *output, uint vertex_id, void *data)
{
	double select;
	select = udg_get_select(vertex_id);
	output[0] = GlobalTransformManipulator.data[vertex_id * 3] + (select * *(double *)data * GlobalTransformManipulator.normal[vertex_id * 3]);
	output[1] = GlobalTransformManipulator.data[vertex_id * 3 + 1] + (select * *(double *)data * GlobalTransformManipulator.normal[vertex_id * 3 + 1]);
	output[2] = GlobalTransformManipulator.data[vertex_id * 3 + 2] + (select * *(double *)data * GlobalTransformManipulator.normal[vertex_id * 3 + 2]);
}

void la_t_tm_manipulate(BInputState *input, double *snap, uint snap_type)
{
	ENode *node;
	double select, delta, vertex[3], state[3], matrix[16], normal_tangent;
	uint i, j, update_length = -1;
	void  (*func)(double *output, uint vertex_id, void *data) = NULL;
	void *data;

	node = e_ns_get_node_selected(0, V_NT_GEOMETRY);
	if(input->mouse_button[0] == TRUE)
		update_length = 50;
	else
		GlobalTransformManipulator.update_rotate = 0;

	if(GlobalTransformManipulator.mode == TMM_IDLE)
	{
		GlobalTransformManipulator.manipulator_start[0] = GlobalTransformManipulator.manipulator_pos[0];
		GlobalTransformManipulator.manipulator_start[1] = GlobalTransformManipulator.manipulator_pos[1];
		GlobalTransformManipulator.manipulator_start[2] = GlobalTransformManipulator.manipulator_pos[2];
		return;
	}
	if(GlobalTransformManipulator.mode == TMM_PLACE)
	{
		GlobalTransformManipulator.manipulator_pos[0] = snap[0];
		GlobalTransformManipulator.manipulator_pos[1] = snap[1];
		GlobalTransformManipulator.manipulator_pos[2] = snap[2];
		if(input->mouse_button[0] != TRUE)
			GlobalTransformManipulator.mode = TMM_IDLE;
		return;
	}
	if(GlobalTransformManipulator.mode == TMM_NORMAL)
	{
		double output[3];
		p_get_projection_screen(output, GlobalTransformManipulator.manipulator_pos[0], GlobalTransformManipulator.manipulator_pos[1], GlobalTransformManipulator.manipulator_pos[2]);
		normal_tangent = ((input->pointer_x - output[0]) - (input->pointer_y - output[1])) * GlobalTransformManipulator.manipulator_size - GlobalTransformManipulator.grab_manip_pos;
		func = normal_func;
		data = &normal_tangent;
	}
	if(GlobalTransformManipulator.mode == TMM_SMOOTH)
	{
		double output[3];
		p_get_projection_screen(output, GlobalTransformManipulator.manipulator_pos[0], GlobalTransformManipulator.manipulator_pos[1], GlobalTransformManipulator.manipulator_pos[2]);
		normal_tangent = ((input->pointer_x - output[0]) + (input->pointer_y - output[1])) * GlobalTransformManipulator.manipulator_size - GlobalTransformManipulator.grab_manip_pos;
		if(normal_tangent > GlobalTransformManipulator.manipulator_size * -0.4)
			normal_tangent = GlobalTransformManipulator.manipulator_size * -0.4;
		func = normal_func;
		data = &normal_tangent;
	}
	if(GlobalTransformManipulator.mode == TMM_TANGENT)
	{
		double output[3];
		p_get_projection_screen(output, GlobalTransformManipulator.manipulator_pos[0], GlobalTransformManipulator.manipulator_pos[1], GlobalTransformManipulator.manipulator_pos[2]);
		normal_tangent = (input->pointer_x - output[0]) * -GlobalTransformManipulator.manipulator_size - GlobalTransformManipulator.grab_manip_pos;
		func = normal_func;
		data = &normal_tangent;
	}

	clear_matrix(matrix);
	if(GlobalTransformManipulator.mode == TMM_TRANSFORM)
	{
		if(input->mouse_button[1])
		{
		//	GlobalTransformManipulator.manipulator_pos[GlobalTransformManipulator.grab_axis] = snap[GlobalTransformManipulator.grab_axis];
			p_get_projection_line_snap(GlobalTransformManipulator.manipulator_pos, GlobalTransformManipulator.grab_axis, GlobalTransformManipulator.manipulator_pos[GlobalTransformManipulator.grab_axis] - GlobalTransformManipulator.manipulator_start[GlobalTransformManipulator.grab_axis], GlobalTransformManipulator.manipulator_start, snap, snap_type);
		}
		else
			GlobalTransformManipulator.manipulator_pos[GlobalTransformManipulator.grab_axis] += p_get_projection_line(NULL, GlobalTransformManipulator.grab_axis, input->pointer_x, input->pointer_y, GlobalTransformManipulator.manipulator_pos) - GlobalTransformManipulator.grab_pos;
		
		delta = GlobalTransformManipulator.manipulator_pos[GlobalTransformManipulator.grab_axis] - GlobalTransformManipulator.grab_manip_pos;
		matrix[12 + GlobalTransformManipulator.grab_axis] += delta;
		func = matrix_func;
		data = matrix;
	}
	if(GlobalTransformManipulator.mode == TMM_ROTATE)
	{
		double r, dist[3], matrix2[16], matrix3[16];
		
		if(input->mouse_button[1])
		{
			dist[0] = snap[0];
			dist[1] = snap[1];
			dist[2] = snap[2];
		}
		else
		{
			p_get_projection_plane(dist, GlobalTransformManipulator.grab_axis, input->pointer_x, input->pointer_y, GlobalTransformManipulator.manipulator_pos[GlobalTransformManipulator.grab_axis]);
		}
		delta = atan2(dist[(GlobalTransformManipulator.grab_axis + 1) % 3] - GlobalTransformManipulator.manipulator_pos[(GlobalTransformManipulator.grab_axis + 1) % 3], dist[(GlobalTransformManipulator.grab_axis + 2) % 3] - GlobalTransformManipulator.manipulator_pos[(GlobalTransformManipulator.grab_axis + 2) % 3]) - GlobalTransformManipulator.grab_pos;
		r = (dist[(GlobalTransformManipulator.grab_axis + 1) % 3] - GlobalTransformManipulator.manipulator_pos[(GlobalTransformManipulator.grab_axis + 1) % 3]) * (dist[(GlobalTransformManipulator.grab_axis + 1) % 3] - GlobalTransformManipulator.manipulator_pos[(GlobalTransformManipulator.grab_axis + 1) % 3]) + (dist[(GlobalTransformManipulator.grab_axis + 2) % 3] - GlobalTransformManipulator.manipulator_pos[(GlobalTransformManipulator.grab_axis + 2) % 3]) * (dist[(GlobalTransformManipulator.grab_axis + 2) % 3] - GlobalTransformManipulator.manipulator_pos[(GlobalTransformManipulator.grab_axis + 2) % 3]);
		if(r < GlobalTransformManipulator.manipulator_size * GlobalTransformManipulator.manipulator_size * 1 * 1 && r > GlobalTransformManipulator.manipulator_size * GlobalTransformManipulator.manipulator_size * 0.9 * 0.9)
		{
			if(r < GlobalTransformManipulator.manipulator_size * GlobalTransformManipulator.manipulator_size * 0.96 * 0.96)
				delta = (double)((uint)(ROTATE_GRID_SPLITS * delta / (2 * PI))) * 2 * PI / ROTATE_GRID_SPLITS; 
			else 
				delta = (double)((uint)(ROTATE_GRID_DEGREES * delta / (2 * PI))) * 2 * PI / ROTATE_GRID_DEGREES;
			GlobalTransformManipulator.manipulator_rotate_scale *= 0.9;
		}else
			GlobalTransformManipulator.manipulator_rotate_scale = GlobalTransformManipulator.manipulator_rotate_scale * 0.9 + 0.1;

		clear_matrix(matrix2);
		clear_matrix(matrix3);
		transform_matrix(matrix2, GlobalTransformManipulator.manipulator_pos[0], GlobalTransformManipulator.manipulator_pos[1], GlobalTransformManipulator.manipulator_pos[2]);
		if(GlobalTransformManipulator.grab_axis == 0)
			matrix_rotate_x(matrix, -delta);
		else if(GlobalTransformManipulator.grab_axis == 1)
			matrix_rotate_y(matrix, -delta);
		else if(GlobalTransformManipulator.grab_axis == 2)
			matrix_rotate_z(matrix, -delta);
		matrix_multiply(matrix2, matrix, matrix3);
		clear_matrix(matrix2);
		transform_matrix(matrix2, -GlobalTransformManipulator.manipulator_pos[0], -GlobalTransformManipulator.manipulator_pos[1], -GlobalTransformManipulator.manipulator_pos[2]);
		matrix_multiply(matrix3, matrix2, matrix);
		func = matrix_func;
		data = matrix;
	}


	if(GlobalTransformManipulator.mode == TMM_SCALE)
	{
		double scale[3], vertex[3], length;
		p_get_projection_vertex_with_axis(vertex, GlobalTransformManipulator.manipulator_pos, input->pointer_x, input->pointer_y, input->mouse_button[1], snap, LA_ST_VERTEX);
		
		scale[0] = 0;
		scale[1] = 0;
		scale[2] = 0;

		for(i = 0; i < 3; i++)
		{
			if(vertex[i] > GlobalTransformManipulator.manipulator_pos[i])
			{
				if(GlobalTransformManipulator.max_scale[i] > GlobalTransformManipulator.manipulator_pos[i] + 0.001)
					scale[i] = (vertex[i] - GlobalTransformManipulator.manipulator_pos[i]) / (GlobalTransformManipulator.max_scale[i] - GlobalTransformManipulator.manipulator_pos[i]);
			}
			else
				if(GlobalTransformManipulator.min_scale[i] + 0.001 < GlobalTransformManipulator.manipulator_pos[i])
					scale[i] = (vertex[i] - GlobalTransformManipulator.manipulator_pos[i]) / (GlobalTransformManipulator.min_scale[i] - GlobalTransformManipulator.manipulator_pos[i]);
		}
		length = sqrt(scale[0] * scale[0] + scale[1] * scale[1] + scale[2] * scale[2]);
		for(i = 0; i < 3; i++)
		{
			if(scale[i] == 1 || (vertex[i] - GlobalTransformManipulator.manipulator_pos[i] > -0.0001 && vertex[i] - GlobalTransformManipulator.manipulator_pos[i] < 0.0001))
				scale[i] = 1;
			else
				scale[i] = length;
		}
		transform_matrix(matrix, (scale[0] - 1) * -GlobalTransformManipulator.manipulator_pos[0], (scale[1] - 1) * -GlobalTransformManipulator.manipulator_pos[1], (scale[2] - 1) * -GlobalTransformManipulator.manipulator_pos[2]);
		scale_matrix(matrix, scale[0], scale[1], scale[2]);
		func = matrix_func;
		data = matrix;
	}

	if(func == matrix_func && GlobalTransformManipulator.tags != NULL)
	{	
		UNDOTag *tag;
		tag = udg_get_tags(&i);
		if(i == GlobalTransformManipulator.tag_length)
		{
			for(i = 0; i < GlobalTransformManipulator.tag_length; i++)
			{
				vertex[0] = GlobalTransformManipulator.tags[i * 3];
				vertex[1] = GlobalTransformManipulator.tags[i * 3 + 1];
				vertex[2] = GlobalTransformManipulator.tags[i * 3 + 2];
	

				point_threw_matrix3(matrix, &vertex[0], &vertex[1], &vertex[2]);
				vertex[0] = vertex[0] * tag[i].select + GlobalTransformManipulator.tags[i * 3] * (1 - tag[i].select);
				vertex[1] = vertex[1] * tag[i].select + GlobalTransformManipulator.tags[i * 3 + 1] * (1 - tag[i].select);
				vertex[2] = vertex[2] * tag[i].select + GlobalTransformManipulator.tags[i * 3 + 2] * (1 - tag[i].select);
				udg_move_tag(i, vertex);
			}
		}
	}


	if(data != NULL)
	{
		j = 0;
		if(GlobalTransformManipulator.update_rotate >= GlobalTransformManipulator.data_length)
			GlobalTransformManipulator.update_rotate = 0;
		for(i = GlobalTransformManipulator.update_rotate; i < GlobalTransformManipulator.data_length; i++)
		{
			select = udg_get_select(i);
			if(select > 0.0001)
			{
				state[0] = GlobalTransformManipulator.data[i * 3];
				state[1] = GlobalTransformManipulator.data[i * 3 + 1];
				state[2] = GlobalTransformManipulator.data[i * 3 + 2];
				func(vertex, i, data);
				udg_vertex_set(i, state, vertex[0], vertex[1], vertex[2]);
				if(++j > update_length)
				{
					GlobalTransformManipulator.update_rotate = i;
					break;
				}
			}
		}
		if(i == GlobalTransformManipulator.data_length)
			GlobalTransformManipulator.update_rotate = 0;
		if(input->mouse_button[2] != TRUE && input->last_mouse_button[2] == TRUE)
		{
			undo_event_done();
			la_t_extrude(GlobalTransformManipulator.data_length, func, data);
		}
	}else
	{
		for(i = 0; i < GlobalTransformManipulator.data_length; i++)
		{
			select = udg_get_select(i);
			if(select > 0.0001)
			{
				func(vertex, i, data);
				udg_vertex_move(i, vertex[0], vertex[1], vertex[2]);
			}
		}
	}


	if(input->mouse_button[0] != TRUE && input->last_mouse_button[0] != TRUE)
	{
		if(GlobalTransformManipulator.data != NULL);
			free(GlobalTransformManipulator.data);
		if(GlobalTransformManipulator.normal != NULL);
		{
			free(GlobalTransformManipulator.manipulator_normal_array);
			free(GlobalTransformManipulator.normal);
		}
		if(GlobalTransformManipulator.tags != NULL)
		{
			free(GlobalTransformManipulator.tags);
			GlobalTransformManipulator.tags = NULL;
		}
		GlobalTransformManipulator.mode = TMM_IDLE;
	}
}

void grab_one_vertex(BInputState *input, uint id, double *pos)
{
    static int direction, recursion;
	static double *select;
    uint i, j;
    double delta[3], base[3];
    if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
    {
        direction = 1;
        recursion = 0;
        lock_transform_vertexes(input, FALSE, FALSE, FALSE);
        select = malloc((sizeof *select) * GlobalTransformManipulator.data_length);
        for(i = 0; i < GlobalTransformManipulator.data_length; i++)
            select[i] = 0;
        select[id] = 1;
    }
    delta[0] = pos[0] - GlobalTransformManipulator.data[id * 3];
    delta[1] = pos[1] - GlobalTransformManipulator.data[id * 3 + 1];
    delta[2] = pos[2] - GlobalTransformManipulator.data[id * 3 + 2];     
    if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE)
    {
        for(i = 0; i < GlobalTransformManipulator.data_length; i++)
        {
            if(select[i] > 0.0001)
            {
                base[0] = GlobalTransformManipulator.data[i * 3];
                base[1] = GlobalTransformManipulator.data[i * 3 + 1];
                base[2] = GlobalTransformManipulator.data[i * 3 + 2];
                udg_vertex_set(i, base, base[0] + delta[0] * select[i], base[1] + delta[1] * select[i], base[2] + delta[2] * select[i]);
            }
        }

        free(select);
        free(GlobalTransformManipulator.data);
        return;
    }
    if(input->mouse_button[2] == FALSE && input->last_mouse_button[2] == TRUE)
    {
        recursion += direction;
        if(direction < 0)
            for(i = 0; i < GlobalTransformManipulator.data_length; i++)
                if(select[i] > 0.0001)
                    udg_vertex_move(i, GlobalTransformManipulator.data[i * 3], GlobalTransformManipulator.data[i * 3 + 1], GlobalTransformManipulator.data[i * 3 + 2]);
        if(recursion == 0 || recursion == 5)
            direction = 0 - direction;
        for(i = 0; i < GlobalTransformManipulator.data_length; i++)
            select[i] = 0;
        select[id] = 1;            
        if(recursion != 0)
        {
            uint  *count, ref_count, *ref;
            double *value, temp;
            udg_get_geometry(NULL, &ref_count, NULL, &ref, NULL);
            ref_count *= 4;  
            count = malloc((sizeof *count) * GlobalTransformManipulator.data_length);
            value = malloc((sizeof *value) * GlobalTransformManipulator.data_length);

            for(j = 0; j < recursion; j++)
            {
            	for(i = 0; i < GlobalTransformManipulator.data_length; i++)
                {
                    count[i] = 0;
                    value[i] = 0;
                }
                for(i = 0; i < ref_count; i += 4)
                {
                    if(ref[i] < GlobalTransformManipulator.data_length)
                    {
                        temp = select[ref[i]] + select[ref[i + 1]] + select[ref[i + 2]];
                        if(ref[i + 3] < GlobalTransformManipulator.data_length)
                            temp = (temp + select[ref[i + 3]]) / 4;
                        else
                            temp = temp / 3;
                        count[ref[i]]++;
                        value[ref[i]] += temp;
                        count[ref[i + 1]]++;
                        value[ref[i + 1]] += temp;
                        count[ref[i + 2]]++;
                        value[ref[i + 2]] += temp;
                        if(ref[i + 3] < GlobalTransformManipulator.data_length)
                        {
                            count[ref[i + 3]]++;
                            value[ref[i + 3]] += temp;
                        }
                    }
                }
                for(i = 0; i < GlobalTransformManipulator.data_length; i++)
					if(count[i] > 0)
	                    select[i] = value[i] / (double)count[i];
            }
            temp = 0;
            for(i = 0; i < GlobalTransformManipulator.data_length; i++)
                if(temp < select[i])
                    temp = select[i];
            for(i = 0; i < GlobalTransformManipulator.data_length; i++)
                select[i] = select[i] / temp;
/*		for(i = 0; i < GlobalTransformManipulator.data_length; i++)
			printf("select[%u] = %f\n", i, select[i]);
*/
            free(count);
            free(value);
        }
    }
    for(i = 0; i < GlobalTransformManipulator.data_length; i++)
        if(select[i] > 0.0001)
            udg_vertex_move(i, GlobalTransformManipulator.data[i * 3] + delta[0] * select[i],
			    GlobalTransformManipulator.data[i * 3 + 1] + delta[1] * select[i],
			    GlobalTransformManipulator.data[i * 3 + 2] + delta[2] * select[i]);
}
