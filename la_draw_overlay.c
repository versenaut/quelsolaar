#include "la_includes.h"

#include "la_geometry_undo.h"
#include "la_projection.h"
#include "la_tool.h"
#include "st_matrix_operations.h"

#include "persuade.h"

struct{
	float		*active_vertex;
	float		*active_vertex_shadow;
	float		*active_vertex_color;
	float		*move_vertex;
	float		*move_vertex_shadow;
	float		*move_vertex_color;
	float		*tag_select;
	float		*tag_select_shadow;
	float		*tag_select_color;
	float		*tag_unselect;
	float		*tag_unselect_shadow;
	float		*tag_unselect_color;
	float		*tri;
	float		*tri_normal;
	uint		tri_count;
	float		*quad;
	float		*quad_normal;
	float		*wire;
	uint		wire_count;
	uint		quad_count;
	uint		surface_version;
}GlobalOverlay;

void la_do_init(void)
{
	uint i;
	float temp, square[8] = {0.006, 0.01, 0.003, 0.038, -0.003, 0.038, -0.006, 0.01};
	GlobalOverlay.active_vertex = malloc((sizeof * GlobalOverlay.active_vertex) * 4 * 4 * 2);
	GlobalOverlay.active_vertex_shadow = malloc((sizeof * GlobalOverlay.active_vertex_shadow) * 16 * 4 * 2);
	GlobalOverlay.active_vertex_color = malloc((sizeof * GlobalOverlay.active_vertex_color) * 16 * 4 * 4);
	for(i = 0; i < 4; i++)
	{
		square[0] = 0.013 * sin((float)i * 3.14 * 0.5 - 0.4);
		square[1] = 0.013 * cos((float)i * 3.14 * 0.5 - 0.4);
		square[2] = 0.045 * sin((float)i * 3.14 * 0.5 - 0.05);
		square[3] = 0.045 * cos((float)i * 3.14 * 0.5 - 0.05);
		square[4] = 0.045 * sin((float)i * 3.14 * 0.5 + 0.05);
		square[5] = 0.045 * cos((float)i * 3.14 * 0.5 + 0.05);
		square[6] = 0.013 * sin((float)i * 3.14 * 0.5 + 0.4);
		square[7] = 0.013 * cos((float)i * 3.14 * 0.5 + 0.4);
		sui_draw_set_vec2(GlobalOverlay.active_vertex, i * 4 + 0, square[0], square[1]);
		sui_draw_set_vec2(GlobalOverlay.active_vertex, i * 4 + 1, square[2], square[3]);
		sui_draw_set_vec2(GlobalOverlay.active_vertex, i * 4 + 2, square[4], square[5]);
		sui_draw_set_vec2(GlobalOverlay.active_vertex, i * 4 + 3, square[6], square[7]);
		sui_create_shadow_edge(0.005, 4, &GlobalOverlay.active_vertex_shadow[i * 4 * 2], &GlobalOverlay.active_vertex_color[i * 4 * 4], square);
	}
	GlobalOverlay.move_vertex = malloc((sizeof * GlobalOverlay.move_vertex) * 4 * 8 * 2);
	GlobalOverlay.move_vertex_shadow = malloc((sizeof * GlobalOverlay.move_vertex_shadow) * 16 * 8 * 2);
	GlobalOverlay.move_vertex_color = malloc((sizeof * GlobalOverlay.move_vertex_color) * 16 * 8 * 4);
	for(i = 0; i < 8; i++)
	{
		square[0] = 0.02 * sin((float)i * 3.14 * 0.25);
		square[1] = 0.02 * cos((float)i * 3.14 * 0.25);
		square[2] = 0.025 * sin((float)i * 3.14 * 0.25);
		square[3] = 0.025 * cos((float)i * 3.14 * 0.25);
		square[4] = 0.025 * sin((float)i * 3.14 * 0.25 + 0.3);
		square[5] = 0.025 * cos((float)i * 3.14 * 0.25 + 0.3);
		square[6] = 0.017 * sin((float)i * 3.14 * 0.25 + 0.3);
		square[7] = 0.017 * cos((float)i * 3.14 * 0.25 + 0.3);
		sui_draw_set_vec2(GlobalOverlay.move_vertex, i * 4 + 0, square[0], square[1]);
		sui_draw_set_vec2(GlobalOverlay.move_vertex, i * 4 + 1, square[2], square[3]);
		sui_draw_set_vec2(GlobalOverlay.move_vertex, i * 4 + 2, square[4], square[5]);
		sui_draw_set_vec2(GlobalOverlay.move_vertex, i * 4 + 3, square[6], square[7]);
		sui_create_shadow_edge(0.005, 4, &GlobalOverlay.move_vertex_shadow[i * 4 * 2], &GlobalOverlay.move_vertex_color[i * 4 * 4], square);
	}

	GlobalOverlay.tag_select = malloc((sizeof * GlobalOverlay.tag_select) * 4 * 6 * 2);
	GlobalOverlay.tag_select_shadow = malloc((sizeof * GlobalOverlay.tag_select_shadow) * 16 * 6 * 2);
	GlobalOverlay.tag_select_color = malloc((sizeof * GlobalOverlay.tag_select_color) * 16 * 6 * 4);
	for(i = 0; i < 6; i++)
	{
		square[0] = 0.01 * sin((float)i * 3.14 * 0.33333333);
		square[1] = 0.01 * cos((float)i * 3.14 * 0.33333333);
		square[2] = 0.02 * sin((float)i * 3.14 * 0.33333333 + 0.2);
		square[3] = 0.02 * cos((float)i * 3.14 * 0.33333333 + 0.2);
		square[4] = 0.02 * sin((float)i * 3.14 * 0.33333333 + 0.3);
		square[5] = 0.02 * cos((float)i * 3.14 * 0.33333333 + 0.3);
		square[6] = 0.01 * sin((float)i * 3.14 * 0.33333333 + 0.5);
		square[7] = 0.01 * cos((float)i * 3.14 * 0.33333333 + 0.5);
		sui_draw_set_vec2(GlobalOverlay.tag_select, i * 4 + 0, square[0], square[1]);
		sui_draw_set_vec2(GlobalOverlay.tag_select, i * 4 + 1, square[2], square[3]);
		sui_draw_set_vec2(GlobalOverlay.tag_select, i * 4 + 2, square[4], square[5]);
		sui_draw_set_vec2(GlobalOverlay.tag_select, i * 4 + 3, square[6], square[7]);
		sui_create_shadow_edge(0.005, 4, &GlobalOverlay.tag_select_shadow[i * 4 * 2], &GlobalOverlay.tag_select_color[i * 4 * 4], square);
	}

	GlobalOverlay.tag_unselect = malloc((sizeof * GlobalOverlay.tag_unselect) * 4 * 6 * 2);
	GlobalOverlay.tag_unselect_shadow = malloc((sizeof * GlobalOverlay.tag_unselect_shadow) * 16 * 6 * 2);
	GlobalOverlay.tag_unselect_color = malloc((sizeof * GlobalOverlay.tag_unselect_color) * 16 * 6 * 4);
	for(i = 0; i < 6; i++)
	{
		square[0] = 0.01 * sin((float)i * 3.14 * 0.33333333);
		square[1] = 0.01 * cos((float)i * 3.14 * 0.33333333);
		square[2] = 0.013 * sin((float)i * 3.14 * 0.33333333);
		square[3] = 0.013 * cos((float)i * 3.14 * 0.33333333);
		square[4] = 0.013 * sin((float)i * 3.14 * 0.33333333 + 0.5);
		square[5] = 0.013 * cos((float)i * 3.14 * 0.33333333 + 0.5);
		square[6] = 0.01 * sin((float)i * 3.14 * 0.33333333 + 0.5);
		square[7] = 0.01 * cos((float)i * 3.14 * 0.33333333 + 0.5);
		sui_draw_set_vec2(GlobalOverlay.tag_unselect, i * 4 + 0, square[0], square[1]);
		sui_draw_set_vec2(GlobalOverlay.tag_unselect, i * 4 + 1, square[2], square[3]);
		sui_draw_set_vec2(GlobalOverlay.tag_unselect, i * 4 + 2, square[4], square[5]);
		sui_draw_set_vec2(GlobalOverlay.tag_unselect, i * 4 + 3, square[6], square[7]);
		sui_create_shadow_edge(0.005, 4, &GlobalOverlay.tag_unselect_shadow[i * 4 * 2], &GlobalOverlay.tag_unselect_color[i * 4 * 4], square);
	}
	
	GlobalOverlay.tri_normal = NULL;
	GlobalOverlay.tri_count = 0;
	GlobalOverlay.quad = NULL;
	GlobalOverlay.quad_normal = NULL;
	GlobalOverlay.quad_count = 0;
	GlobalOverlay.wire_count = 0;
}

void la_do_edge_select(double *vertex_a, double *vertex_b)
{
	float edge[16], a[2], b[2];
	a[0] = -vertex_a[0];
	a[1] = -vertex_a[1];
	b[0] = vertex_a[0] - vertex_b[0];
	b[1] = vertex_a[1] - vertex_b[1];
	sui_draw_set_vec2(edge, 0, a[0] + b[1] * 0.04, a[1] - b[0] * 0.04);
	sui_draw_set_vec2(edge, 1, a[0] - b[1] * 0.04, a[1] + b[0] * 0.04);
	sui_draw_set_vec2(edge, 2, a[0] + (b[0] * 0.4) - b[1] * 0.06, a[1] + (b[1] * 0.4) + b[0] * 0.06);
	sui_draw_set_vec2(edge, 3, a[0] + (b[0] * 0.4) + b[1] * 0.06, a[1] + (b[1] * 0.4) - b[0] * 0.06);
	sui_draw_set_vec2(edge, 4, a[0] + (b[0] * 0.6) + b[1] * 0.06, a[1] + (b[1] * 0.6) - b[0] * 0.06);
	sui_draw_set_vec2(edge, 5, a[0] + (b[0] * 0.6) - b[1] * 0.06, a[1] + (b[1] * 0.6) + b[0] * 0.06);
	sui_draw_set_vec2(edge, 6, a[0] + b[0] - b[1] * 0.04, a[1] + b[1] + b[0] * 0.04);
	sui_draw_set_vec2(edge, 7, a[0] + b[0] + b[1] * 0.04, a[1] + b[1] - b[0] * 0.04);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	sui_draw_gl(GL_QUADS, edge, 8, 2, 0.2, 0.8, 1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void la_do_edge_delete_air(double *vertex_a, double *vertex_b)
{
	float edge[12], a[2], b[2];
	a[0] = -vertex_a[0];
	a[1] = -vertex_a[1];
	b[0] = vertex_a[0] - vertex_b[0];
	b[1] = vertex_a[1] - vertex_b[1];
	sui_draw_set_vec2(edge, 0, a[0] + b[0] * 0.1, a[1] + b[1] * 0.1);
	sui_draw_set_vec2(edge, 1, a[0] + b[0] * 0.2 + b[1] * 0.06, a[1] + b[1] * 0.2 + b[0] * 0.06);
	sui_draw_set_vec2(edge, 2, a[0] + b[0] * 0.2 + b[1] * 0.06, a[1] + b[1] * 0.2 + b[0] * 0.06);
	sui_draw_set_vec2(edge, 3, a[0] + b[0] * 0.8 + b[1] * 0.06, a[1] + b[1] * 0.8 + b[0] * 0.06);
	sui_draw_set_vec2(edge, 4, a[0] + b[0] * 0.8 + b[1] * 0.06, a[1] + b[1] * 0.8 + b[0] * 0.06);
	sui_draw_set_vec2(edge, 5, a[0] + b[0] * 0.9, a[1] + b[1] * 0.9);
	sui_draw_gl(GL_LINES, edge, 6, 2, 1, 0.1, 0.4);
	sui_draw_set_vec2(edge, 1, a[0] + b[0] * 0.2 - b[1] * 0.06, a[1] + b[1] * 0.2 - b[0] * 0.06);
	sui_draw_set_vec2(edge, 2, a[0] + b[0] * 0.2 - b[1] * 0.06, a[1] + b[1] * 0.2 - b[0] * 0.06);
	sui_draw_set_vec2(edge, 3, a[0] + b[0] * 0.8 - b[1] * 0.06, a[1] + b[1] * 0.8 - b[0] * 0.06);
	sui_draw_set_vec2(edge, 4, a[0] + b[0] * 0.8 - b[1] * 0.06, a[1] + b[1] * 0.8 - b[0] * 0.06);
	sui_draw_gl(GL_LINES, edge, 4, 2, 1, 0.1, 0.4);
}


void la_do_edge_split(double *vertex_a, double *vertex_b, double pos)
{
	float edge[32], a[2], b[2];
	a[0] = -vertex_a[0];
	a[1] = -vertex_a[1];
	b[0] = vertex_a[0] - vertex_b[0];
	b[1] = vertex_a[1] - vertex_b[1];
	sui_draw_set_vec2(edge, 0, a[0] + (b[0] * -0.03) - b[1] * 0.06, a[1] + (b[1] * -0.03) + b[0] * 0.06);
	sui_draw_set_vec2(edge, 1, a[0] + (b[0] * -0.03) - b[1] * 0.02, a[1] + (b[1] * -0.03) + b[0] * 0.02);
	sui_draw_set_vec2(edge, 2, a[0] + (b[0] * (pos - 0.01)) - b[1] * 0.02, a[1] + (b[1] * (pos - 0.01)) + b[0] * 0.02);
	sui_draw_set_vec2(edge, 3, a[0] + (b[0] * (pos - 0.03)) - b[1] * 0.06, a[1] + (b[1] * (pos - 0.03)) + b[0] * 0.06);

	sui_draw_set_vec2(edge, 4, a[0] + (b[0] * -0.03) + b[1] * 0.06, a[1] + (b[1] * -0.03) - b[0] * 0.06);
	sui_draw_set_vec2(edge, 5, a[0] + (b[0] * -0.03) + b[1] * 0.02, a[1] + (b[1] * -0.03) - b[0] * 0.02);
	sui_draw_set_vec2(edge, 6, a[0] + (b[0] * (pos - 0.01)) + b[1] * 0.02, a[1] + (b[1] * (pos - 0.01)) - b[0] * 0.02);
	sui_draw_set_vec2(edge, 7, a[0] + (b[0] * (pos - 0.03)) + b[1] * 0.06, a[1] + (b[1] * (pos - 0.03)) - b[0] * 0.06);

	sui_draw_set_vec2(edge, 8, a[0] + (b[0] * 1.03) - b[1] * 0.06, a[1] + (b[1] * 1.03) + b[0] * 0.06);
	sui_draw_set_vec2(edge, 9, a[0] + (b[0] * 1.03) - b[1] * 0.02, a[1] + (b[1] * 1.03) + b[0] * 0.02);
	sui_draw_set_vec2(edge, 10, a[0] + (b[0] * (pos + 0.01)) - b[1] * 0.02, a[1] + (b[1] * (pos + 0.01)) + b[0] * 0.02);
	sui_draw_set_vec2(edge, 11, a[0] + (b[0] * (pos + 0.03)) - b[1] * 0.06, a[1] + (b[1] * (pos + 0.03)) + b[0] * 0.06);

	sui_draw_set_vec2(edge, 12, a[0] + (b[0] * 1.03) + b[1] * 0.06, a[1] + (b[1] * 1.03) - b[0] * 0.06);
	sui_draw_set_vec2(edge, 13, a[0] + (b[0] * 1.03) + b[1] * 0.02, a[1] + (b[1] * 1.03) - b[0] * 0.02);
	sui_draw_set_vec2(edge, 14, a[0] + (b[0] * (pos + 0.01)) + b[1] * 0.02, a[1] + (b[1] * (pos + 0.01)) - b[0] * 0.02);
	sui_draw_set_vec2(edge, 15, a[0] + (b[0] * (pos + 0.03)) + b[1] * 0.06, a[1] + (b[1] * (pos + 0.03)) - b[0] * 0.06);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	sui_draw_gl(GL_QUADS, edge, 16, 2, 1, 0.1, 0.4);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void la_do_edge_delete(float ax, float ay, float az, float bx, float by, float bz, float cx, float cy, float cz)
{
	float edge[18];
	sui_draw_set_vec3(edge, 0, ax * 0.9 + bx * 0.1, ay * 0.9 + by * 0.1, az * 0.9 + bz * 0.1);
	sui_draw_set_vec3(edge, 1, ax * 0.75 + bx * 0.15 + cx * 0.1, ay * 0.75 + by * 0.15 + cy * 0.1, az * 0.75 + bz * 0.15 + cz * 0.1);
	sui_draw_set_vec3(edge, 2, ax * 0.75 + bx * 0.15 + cx * 0.1, ay * 0.75 + by * 0.15 + cy * 0.1, az * 0.75 + bz * 0.15 + cz * 0.1);
	sui_draw_set_vec3(edge, 3, bx * 0.75 + ax * 0.15 + cx * 0.1, by * 0.75 + ay * 0.15 + cy * 0.1, bz * 0.75 + az * 0.15 + cz * 0.1);
	sui_draw_set_vec3(edge, 4, bx * 0.75 + ax * 0.15 + cx * 0.1, by * 0.75 + ay * 0.15 + cy * 0.1, bz * 0.75 + az * 0.15 + cz * 0.1);
	sui_draw_set_vec3(edge, 5, bx * 0.9 + ax * 0.1, by * 0.9 + ay * 0.1, bz * 0.9 + az * 0.1);

	sui_draw_gl(GL_LINES, edge, 6, 3, 1, 0.1, 0.4);
}

void la_do_active_vertex(double *vertex, boolean move)
{
	static double t = 0;
	double pos[3];
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();
	p_get_projection_screen(pos, vertex[0], vertex[1], vertex[2]);
	glDisable(GL_DEPTH_TEST);
	glTranslated(-pos[0], -pos[1], -1);
	glRotated(t++, 0, 0, 1);
	if(move)
	{
		sui_set_blend_gl(GL_ADD, GL_ADD);
		sui_draw_gl(GL_QUADS, GlobalOverlay.move_vertex, 32, 2, 0.8, 0.8, 0.8);
		sui_set_blend_gl(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		sui_set_color_array_gl(GlobalOverlay.move_vertex_color, 32 * 4, 4);
		sui_draw_gl(GL_QUADS, GlobalOverlay.move_vertex_shadow, 32 * 4, 2, 0.8, 0.8, 0.8);
	}
	else
	{
		sui_set_blend_gl(GL_ADD, GL_ADD);
		sui_draw_gl(GL_QUADS, GlobalOverlay.active_vertex, 16, 2, 0.8, 0.8, 0.8);
		sui_set_blend_gl(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		sui_set_color_array_gl(GlobalOverlay.active_vertex_color, 16 * 4, 4);
		sui_draw_gl(GL_QUADS, GlobalOverlay.active_vertex_shadow, 16 * 4, 2, 0.8, 0.8, 0.8);
	}
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	glPushMatrix();
	p_view_set();
	glPushMatrix();
}

void la_do_xyz_lines(double *start, boolean snap)
{
	float xyz_lines[24 * 3], xyz_color[24 * 3];
	double camera[3], r, r2, x, y, z, scale[3] = {0, 0, 0};
	uint i, axis;
	glPushMatrix();
	p_get_view_camera(camera);
	camera[0] -= start[0];
	camera[1] -= start[1];
	camera[2] -= start[2];
	r = sqrt(camera[0] * camera[0] + camera[1] * camera[1] + camera[2] * camera[2]);
	r2 = sqrt(camera[1] * camera[1] + camera[2] * camera[2]);
	r *= 0.004;
	x = r * 20;
	y = camera[2] / r2 * r;
	z = camera[1] / r2 * r;
	sui_draw_set_vec3(xyz_lines, 0, x, y, -z);
	sui_draw_set_vec3(xyz_lines, 1, x * 40, y, -z);
	sui_draw_set_vec3(xyz_lines, 2, x * 40, -y, z);
	sui_draw_set_vec3(xyz_lines, 3, x, -y, z);
	sui_draw_set_vec3(xyz_lines, 4, -x, y, -z);
	sui_draw_set_vec3(xyz_lines, 5, -x * 40, y, -z);
	sui_draw_set_vec3(xyz_lines, 6, -x * 40, -y, z);
	sui_draw_set_vec3(xyz_lines, 7, -x, -y, z);
	r2 = sqrt(camera[0] * camera[0] + camera[2] * camera[2]);
	x = camera[2] / r2 * r;
	y = r * 20;
	z = camera[0] / r2 * r;
	sui_draw_set_vec3(xyz_lines, 8, -x, y, z);
	sui_draw_set_vec3(xyz_lines, 9, -x, y * 40, z);
	sui_draw_set_vec3(xyz_lines, 10, x, y * 40, -z);
	sui_draw_set_vec3(xyz_lines, 11, x, y, -z);
	sui_draw_set_vec3(xyz_lines, 12, -x, -y, z);
	sui_draw_set_vec3(xyz_lines, 13, -x, -y * 40, z);
	sui_draw_set_vec3(xyz_lines, 14, x, -y * 40, -z);
	sui_draw_set_vec3(xyz_lines, 15, x, -y, -z);
	r2 = sqrt(camera[1] * camera[1] + camera[0] * camera[0]);
	x = camera[1] / r2 * r;
	y = camera[0] / r2 * r;
	z = r * 20;
	sui_draw_set_vec3(xyz_lines, 16, x, -y, z);
	sui_draw_set_vec3(xyz_lines, 17, x, -y, z * 40);
	sui_draw_set_vec3(xyz_lines, 18, -x, y, z * 40);
	sui_draw_set_vec3(xyz_lines, 19, -x, y, z);
	sui_draw_set_vec3(xyz_lines, 20, x, -y, -z);
	sui_draw_set_vec3(xyz_lines, 21, x, -y, -z * 40);
	sui_draw_set_vec3(xyz_lines, 22, -x, y, -z * 40);
	sui_draw_set_vec3(xyz_lines, 23, -x, y, -z);
	for(i = 0; i < 24; i++)
	{
		if(((i + 1) / 2) % 2)
			sui_draw_set_vec3(xyz_color, i, 0, 0, 0);
		else
			sui_draw_set_vec3(xyz_color, i, 0.05, 0.15, 0.25);
	}
	glTranslated(start[0], start[1], start[2]);
	glDisable(GL_DEPTH_TEST);
	sui_set_blend_gl(GL_ONE, GL_ONE);
	sui_set_color_array_gl(xyz_color, 24, 3);
	sui_draw_gl(GL_LINES, xyz_lines, 24, 3, 1, 1, 1);
	if(snap)
	{
		sui_set_blend_gl(GL_ONE, GL_ONE);
		sui_set_color_array_gl(xyz_color, 24, 3);
		sui_draw_gl(GL_QUADS, xyz_lines, 24, 3, 1, 1, 1);
		axis = p_get_projection_axis();
		if(axis < 3)
		{
			float grid_points[300];
			udg_get_grid_snap(camera, &scale[axis]);
			glScaled(scale[0], scale[1], scale[2]);
			scale[axis] *= camera[axis];
			for(i = 0; i < 100; i++)
				sui_draw_set_vec3(grid_points, i, (float)i - 50.0, (float)i - 50.0, (float)i - 50.0);
			sui_draw_gl(GL_LINES, xyz_lines, 100, 3, 1, 1, 1);
		}
	}
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
}

void la_do_draw(double *start, double *end, boolean snap, double *closest)
{
	double camera[3], r, r2, x, y, z;
	glDisable(GL_DEPTH_TEST);
	sui_draw_3d_line_gl(start[0], start[1], start[2], end[0], end[1], end[2], 0.7, 0.7, 0.7);
	glEnable(GL_DEPTH_TEST);
	la_do_xyz_lines(start, snap);
}

void la_do_draw_closest_edge(uint *edge, double x, double y, boolean snap)
{
	double *vertex, a[3], b[3];
	if(edge[0] != -1 && edge[1] != -1)
	{
		udg_get_geometry(NULL, NULL, &vertex, NULL, NULL);
		glPopMatrix();
		glPopMatrix();
		glPushMatrix();
		glTranslated(0, 0, -1);
		glDisable(GL_DEPTH_TEST);
		p_get_projection_screen(a, vertex[edge[0] * 3], vertex[edge[0] * 3 + 1], vertex[edge[0] * 3 + 2]);
		p_get_projection_screen(b, vertex[edge[1] * 3], vertex[edge[1] * 3 + 1], vertex[edge[1] * 3 + 2]);
		if(snap)
		{
			la_do_edge_select(a, b);
		}else
		{
			if(udg_get_select(edge[0]) + udg_get_select(edge[1]) > 0.99)
				la_do_edge_select(a, b);
			else
			{
				double r;
				r = sqrt((a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]));
				r = (((a[0] - b[0]) / r) * (a[0] + x) + ((a[1] - b[1]) / r) * (a[1] + y)) / r;
				la_do_edge_split(a, b, r);
			}
		}
		glEnable(GL_DEPTH_TEST);
		glPopMatrix();
		glPushMatrix();
		p_view_set();
		glPushMatrix();
	}
}

void draw_persuade_surface(ENode *node)
{
#ifdef PERSUADE_H
	static ENode *old_node = NULL;
	static PMesh *mesh = NULL, *next = NULL;
	static uint version;

	if(old_node != NULL && old_node != node && mesh != NULL)
	{
		p_rm_destroy(mesh);
		mesh = NULL;
	}
	old_node = node;
	if(node == NULL)
		return;

	if(mesh == NULL)
		mesh = p_rm_create(node);
	if(mesh != NULL)
		mesh = p_rm_service(mesh, NULL, e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node,  0)));
	if(mesh != NULL && p_rm_drawable(mesh))
	{
	
		glPushMatrix();
		glPolygonMode(GL_FRONT, GL_LINE);
		if(e_nsg_get_layer_version(e_nsg_get_layer_by_id(node,  0)) != version)
		{
			p_rm_update_shape(mesh, e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node,  0)));
			version = e_nsg_get_layer_version(e_nsg_get_layer_by_id(node,  0));
		}
//		p_rm_service(mesh, NULL, e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node,  0)));
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_LIGHTING);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glBindTexture(GL_TEXTURE_2D, la_pfx_surface_material());
		glDisable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_NORMALIZE);
//		glDisable(GL_LIGHTING);
//		glDisable(GL_TEXTURE_2D);
//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


#ifdef E_GEOMETRY_REAL_PRECISION_64_BIT
		glVertexPointer(3, GL_DOUBLE, 0, p_rm_get_vertex(mesh));
		glNormalPointer(GL_DOUBLE, 0 , p_rm_get_normal(mesh));
//		glNormalPointer(GL_DOUBLE, 0 , p_rm_get_vertex(mesh));
#endif
#ifdef E_GEOMETRY_REAL_PRECISION_32_BIT
		glVertexPointer(3, GL_FLOAT, 0, p_rm_get_vertex(mesh));
		glNormalPointer(GL_FLOAT, 0 , p_rm_get_normal(mesh));
#endif
//		glEnable(GL_LIGHTING);
		glColor4f(1, 1, 1, 1);

		glDrawElements(GL_TRIANGLES, p_rm_get_ref_length(mesh), GL_UNSIGNED_INT, p_rm_get_reference(mesh));
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_NORMALIZE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glPopMatrix();
	}
#endif
}
void draw_persuade_surface_old(ENode *node)
{
#ifdef PERSUADE_H
	static PMesh	*mesh = NULL, *next = NULL;
	static uint version;

	if((mesh == NULL || !p_rm_validate(mesh)) && next == NULL)
		next = p_rm_create(node);
	if(next != NULL)
	{
		if(p_rm_validate(next))
			next = p_rm_service(next, NULL, e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node,  0)));
		else
		{
			p_rm_destroy(next);
			next = NULL;
		}
	}
	if(next != NULL && p_rm_drawable(next))
	{
		if(mesh != NULL)
			p_rm_destroy(mesh);
		mesh = next;
		next = NULL; 
	}
	if(mesh != NULL)
	{
		glPushMatrix();
		glPolygonMode(GL_FRONT, GL_LINE);
		if(e_nsg_get_layer_version(e_nsg_get_layer_by_id(node,  0)) != version)
		{
			p_rm_update_shape(mesh, e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node,  0)));
			version = e_nsg_get_layer_version(e_nsg_get_layer_by_id(node,  0));
		}
//		p_rm_service(mesh, NULL, e_nsg_get_layer_data(node, e_nsg_get_layer_by_id(node,  0)));
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_LIGHTING);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glBindTexture(GL_TEXTURE_2D, la_pfx_surface_material());
		glDisable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_NORMALIZE);
//		glDisable(GL_LIGHTING);
//		glDisable(GL_TEXTURE_2D);
//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


#ifdef E_GEOMETRY_REAL_PRECISION_64_BIT
		glVertexPointer(3, GL_DOUBLE, 0, p_rm_get_vertex(mesh));
		glNormalPointer(GL_DOUBLE, 0 , p_rm_get_normal(mesh));
//		glNormalPointer(GL_DOUBLE, 0 , p_rm_get_vertex(mesh));
#endif
#ifdef E_GEOMETRY_REAL_PRECISION_32_BIT
		glVertexPointer(3, GL_FLOAT, 0, p_rm_get_vertex(mesh));
		glNormalPointer(GL_FLOAT, 0 , p_rm_get_normal(mesh));
#endif
//		glEnable(GL_LIGHTING);
		glColor4f(1, 1, 1, 1);

		glDrawElements(GL_TRIANGLES, p_rm_get_ref_length(mesh), GL_UNSIGNED_INT, p_rm_get_reference(mesh));
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_NORMALIZE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glPopMatrix();
	}
#endif
}
void draw_owerlay_tags(void)
{
	double pos[3], f, color;
	UNDOTag	*tag;
	uint count, i;
	tag = udg_get_tags(&count);
	for(i = 0; i < count; i++)
	{
		glPopMatrix();
		glPopMatrix();
		glPushMatrix();
		p_get_projection_screen(pos, tag[i].vec[0], tag[i].vec[1], tag[i].vec[2]);
		glDisable(GL_DEPTH_TEST);
		glTranslated(-pos[0], -pos[1], -1);
	//	glRotated(t++, 0, 0, 1);
		if(tag[i].select < 0.01)
		{
			sui_set_blend_gl(GL_ADD, GL_ADD);
			sui_draw_gl(GL_QUADS, GlobalOverlay.tag_unselect, 24, 2, 0.8, 0.8, 0.8);
		/*	sui_set_blend_gl(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			sui_set_color_array_gl(GlobalOverlay.tag_unselect_color, 24 * 4, 4);
			sui_draw_gl(GL_QUADS, GlobalOverlay.tag_unselect_shadow, 24 * 4, 2, 0.8, 0.8, 0.8);*/
		}else
		{
			sui_set_blend_gl(GL_ADD, GL_ADD);
			sui_draw_gl(GL_QUADS, GlobalOverlay.tag_select, 24, 2, 0.8, 0.8, 0.8);
		/*	sui_set_blend_gl(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			sui_set_color_array_gl(GlobalOverlay.tag_select_color, 24 * 4, 4);
			sui_draw_gl(GL_QUADS, GlobalOverlay.tag_select_shadow, 24 * 4, 2, 0.8, 0.8, 0.8);*/
		}
		color = tag[i].select * 0.5 + 0.25;
		sui_draw_text(0.03, SUI_T_SIZE * -0.5, SUI_T_SIZE, SUI_T_SPACE, tag[i].group, color, color, color);
		f = sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, tag[i].group);
		sui_draw_text(0.05 + f, SUI_T_SIZE * -0.5, SUI_T_SIZE, SUI_T_SPACE, tag[i].tag, color, color, color);
		sui_draw_2d_line_gl(0.04 + f, 0.03, 0.04 + f, -0.03, color, color, color);
		glEnable(GL_DEPTH_TEST);
		glPopMatrix();
		glPushMatrix();
		p_view_set();
		glPushMatrix();
	}
}

extern void test_draw();

void draw_owerlay_surface(void)
{
	static uint version, draw;
	double *vertex;
	uint *ref, *crease, ref_length, length, i, j, temp[4];

	float x, y, z, x2, y2, z2, x3, y3, z3, r;

	if(GlobalOverlay.surface_version != udg_get_version(TRUE, TRUE, FALSE, FALSE, FALSE))
	{	
		udg_get_geometry(&length, &ref_length, &vertex, &ref, &crease);
		if(GlobalOverlay.tri_count != 0)
		{
			free(GlobalOverlay.tri);
			free(GlobalOverlay.tri_normal);
			GlobalOverlay.tri = NULL;
			GlobalOverlay.tri_normal = NULL;
		}
		if(GlobalOverlay.quad_count != 0)
		{
			free(GlobalOverlay.quad);
			free(GlobalOverlay.quad_normal);
			GlobalOverlay.quad = NULL;
			GlobalOverlay.quad_normal = NULL;
		}	
		if(GlobalOverlay.wire_count != 0)
		{
			free(GlobalOverlay.wire);
			GlobalOverlay.wire = NULL;
		}		
		GlobalOverlay.wire_count = 0;
		GlobalOverlay.tri_count = 0;
		GlobalOverlay.quad_count = 0;
		
		for(i = 0; i < ref_length; i++)
		{
			if(ref[i * 4] < length && ref[i * 4 + 1] < length && ref[i * 4 + 2] < length && vertex[ref[i * 4] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 1] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 2] * 3] != V_REAL64_MAX)
			{
				if(ref[i * 4 + 3] < length && vertex[ref[i * 4 + 3] * 3] != V_REAL64_MAX)
				{
					GlobalOverlay.quad_count++;
					if(crease != NULL)
						for(j = 0; j < 4; j++)
							if(crease[i * 4 + j] != 0)
								GlobalOverlay.wire_count++;
				}else
				{
					GlobalOverlay.tri_count++;
					if(crease != NULL)
						for(j = 0; j < 3; j++)
							if(crease[i * 4 + j] != 0)
								GlobalOverlay.wire_count++;
				}
			}
		}

		if(GlobalOverlay.tri_count != 0)
		{
			GlobalOverlay.tri = malloc((sizeof *GlobalOverlay.tri) * GlobalOverlay.tri_count * 9);
			GlobalOverlay.tri_normal = malloc((sizeof *GlobalOverlay.tri_normal) * GlobalOverlay.tri_count * 9);
		}

		if(GlobalOverlay.quad_count != 0)
		{
			GlobalOverlay.quad = malloc((sizeof *GlobalOverlay.quad) * GlobalOverlay.quad_count * 12);
			GlobalOverlay.quad_normal = malloc((sizeof *GlobalOverlay.quad_normal) * GlobalOverlay.quad_count * 12);
		}	

		if(GlobalOverlay.wire_count != 0)
			GlobalOverlay.wire = malloc((sizeof *GlobalOverlay.wire) * GlobalOverlay.wire_count * 2 * 3);
	}

	if((GlobalOverlay.tri_count != 0 || GlobalOverlay.quad_count != 0) && GlobalOverlay.surface_version != udg_get_version(TRUE, TRUE, TRUE, FALSE, FALSE))
	{
		GlobalOverlay.tri_count = 0;
		GlobalOverlay.quad_count = 0;
		GlobalOverlay.wire_count = 0;
		udg_get_geometry(&length, &ref_length, &vertex, &ref, &crease);
	
		#ifdef PERSUADE_H
			draw = TRUE;
		#else
	//	if(sui_test_setting_version(&version))
			draw = FALSE; /*sui_get_setting_int("RENDER_AS_SDS", TRUE);*/
		#endif


		for(i = 0; i < ref_length; i++)
		{
			if(ref[i * 4] < length && ref[i * 4 + 1] < length && ref[i * 4 + 2] < length && vertex[ref[i * 4] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 1] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 2] * 3] != V_REAL64_MAX)
			{
				temp[0] = ref[i * 4] * 3;
				temp[1] = ref[i * 4 + 1] * 3;
				temp[2] = ref[i * 4 + 2] * 3;
				temp[3] = ref[i * 4 + 3] * 3;
				x2 = vertex[temp[0] + 0] - vertex[temp[1] + 0];
				y2 = vertex[temp[0] + 1] - vertex[temp[1] + 1];
				z2 = vertex[temp[0] + 2] - vertex[temp[1] + 2];
				r = sqrt(x2 * x2 + y2 * y2 + z2 * z2);
				x2 = x2 / r;
				y2 = y2 / r;
				z2 = z2 / r;

				x3 = vertex[temp[2] + 0] - vertex[temp[1] + 0];
				y3 = vertex[temp[2] + 1] - vertex[temp[1] + 1];
				z3 = vertex[temp[2] + 2] - vertex[temp[1] + 2];
				r = sqrt(x3 * x3 + y3 * y3 + z3 * z3);
				x3 = x3 / r;
				y3 = y3 / r;
				z3 = z3 / r;

				x = y2 * z3 - z2 * y3;
				y = z2 * x3 - x2 * z3;
				z = x2 * y3 - y2 * x3;
	
		//		x = ((vertex[temp[1] + 1] - vertex[temp[0] + 1]) * (vertex[temp[2] + 2] - vertex[temp[0] + 2]) - (vertex[temp[1] + 2] - vertex[temp[0] + 2]) * (vertex[temp[2] + 1] - vertex[temp[0] + 1]));
		//		y = ((vertex[temp[1] + 2] - vertex[temp[0] + 2]) * (vertex[temp[2]] - vertex[temp[0]]) - (vertex[temp[1]] - vertex[temp[0]]) * (vertex[temp[2] + 2] - vertex[temp[0] + 2]));
		//		z = ((vertex[temp[1]] - vertex[temp[0]]) * (vertex[temp[2] + 2] - vertex[temp[0] + 1]) - (vertex[temp[1] + 1] - vertex[temp[0] + 1]) * (vertex[temp[2]] - vertex[temp[0]]));
				r = sqrt(x * x + y * y + z * z);
				x = x / r;
				y = y / r;
				z = z / r;
				r = x * 0.06 + y * 0.06 + z * 0.06;
				
				if(r < 0)
					r = -r;
			//	r += 0.01;
				r *= 0.5;
				x = r;
				y = r;
				z = r;
				if(draw != TRUE)
				{ 
					x = r * 3.1;
					y = r * 5.3;
					z = r * 10.5;
				}
				if(temp[3] < length * 3 && vertex[temp[3]] != V_REAL64_MAX)
				{
					if(udg_get_select(temp[0] / 3) > 0.01 && udg_get_select(temp[1] / 3) > 0.01 && udg_get_select(temp[2] / 3) > 0.01 && udg_get_select(temp[3] / 3) > 0.01)
					{
						x += x * 1.2;
						y += y * 1.22;
						z += z * 1.24;
					}
					sui_draw_set_vec3(GlobalOverlay.quad_normal, GlobalOverlay.quad_count, x, y, z);
					sui_draw_set_vec3(GlobalOverlay.quad, GlobalOverlay.quad_count++, vertex[temp[0]], vertex[temp[0] + 1], vertex[temp[0] + 2]);
					if(crease != NULL && crease[i * 4] != 0)
					{
						sui_draw_set_vec3(GlobalOverlay.wire, GlobalOverlay.wire_count++, vertex[temp[0]], vertex[temp[0] + 1], vertex[temp[0] + 2]);
						sui_draw_set_vec3(GlobalOverlay.wire, GlobalOverlay.wire_count++, vertex[temp[1]], vertex[temp[1] + 1], vertex[temp[1] + 2]);
					}
					sui_draw_set_vec3(GlobalOverlay.quad_normal, GlobalOverlay.quad_count, x, y, z);
					sui_draw_set_vec3(GlobalOverlay.quad, GlobalOverlay.quad_count++, vertex[temp[1]], vertex[temp[1] + 1], vertex[temp[1] + 2]);
					if(crease != NULL && crease[i * 4 + 1] != 0)
					{
						sui_draw_set_vec3(GlobalOverlay.wire, GlobalOverlay.wire_count++, vertex[temp[1]], vertex[temp[1] + 1], vertex[temp[1] + 2]);
						sui_draw_set_vec3(GlobalOverlay.wire, GlobalOverlay.wire_count++, vertex[temp[2]], vertex[temp[2] + 1], vertex[temp[2] + 2]);
					}
					sui_draw_set_vec3(GlobalOverlay.quad_normal, GlobalOverlay.quad_count, x, y, z);
					sui_draw_set_vec3(GlobalOverlay.quad, GlobalOverlay.quad_count++, vertex[temp[2]], vertex[temp[2] + 1], vertex[temp[2] + 2]);	
					if(crease != NULL && crease[i * 4 + 2] != 0)
					{
						sui_draw_set_vec3(GlobalOverlay.wire, GlobalOverlay.wire_count++, vertex[temp[2]], vertex[temp[2] + 1], vertex[temp[2] + 2]);
						sui_draw_set_vec3(GlobalOverlay.wire, GlobalOverlay.wire_count++, vertex[temp[3]], vertex[temp[3] + 1], vertex[temp[3] + 2]);
					}
					sui_draw_set_vec3(GlobalOverlay.quad_normal, GlobalOverlay.quad_count, x, y, z);
					sui_draw_set_vec3(GlobalOverlay.quad, GlobalOverlay.quad_count++, vertex[temp[3]], vertex[temp[3] + 1], vertex[temp[3] + 2]);	
					if(crease != NULL && crease[i * 4 + 3] != 0)
					{
						sui_draw_set_vec3(GlobalOverlay.wire, GlobalOverlay.wire_count++, vertex[temp[3]], vertex[temp[3] + 1], vertex[temp[3] + 2]);
						sui_draw_set_vec3(GlobalOverlay.wire, GlobalOverlay.wire_count++, vertex[temp[0]], vertex[temp[0] + 1], vertex[temp[0] + 2]);
					}
				}else
				{
					if(udg_get_select(temp[0] / 3) > 0.01 && udg_get_select(temp[1] / 3) > 0.01 && udg_get_select(temp[2] / 3) > 0.01)
					{
						x += x * 1.2;
						y += y * 1.22;
						z += z * 1.24;
					}
					sui_draw_set_vec3(GlobalOverlay.tri_normal, GlobalOverlay.tri_count, x, y, z);
					sui_draw_set_vec3(GlobalOverlay.tri, GlobalOverlay.tri_count++, vertex[temp[0]], vertex[temp[0] + 1], vertex[temp[0] + 2]);
					if(crease != NULL && crease[i * 4] != 0)
					{
						sui_draw_set_vec3(GlobalOverlay.wire, GlobalOverlay.wire_count++, vertex[temp[0]], vertex[temp[0] + 1], vertex[temp[0] + 2]);
						sui_draw_set_vec3(GlobalOverlay.wire, GlobalOverlay.wire_count++, vertex[temp[1]], vertex[temp[1] + 1], vertex[temp[1] + 2]);
					}
					sui_draw_set_vec3(GlobalOverlay.tri_normal, GlobalOverlay.tri_count, x, y, z);
					sui_draw_set_vec3(GlobalOverlay.tri, GlobalOverlay.tri_count++, vertex[temp[1]], vertex[temp[1] + 1], vertex[temp[1] + 2]);
					if(crease != NULL && crease[i * 4 + 1] != 0)
					{
						sui_draw_set_vec3(GlobalOverlay.wire, GlobalOverlay.wire_count++, vertex[temp[1]], vertex[temp[1] + 1], vertex[temp[1] + 2]);
						sui_draw_set_vec3(GlobalOverlay.wire, GlobalOverlay.wire_count++, vertex[temp[2]], vertex[temp[2] + 1], vertex[temp[2] + 2]);
					}
					sui_draw_set_vec3(GlobalOverlay.tri_normal, GlobalOverlay.tri_count, x, y, z);
					sui_draw_set_vec3(GlobalOverlay.tri, GlobalOverlay.tri_count++, vertex[temp[2]], vertex[temp[2] + 1], vertex[temp[2] + 2]);					
					if(crease != NULL && crease[i * 4 + 2] != 0)
					{
						sui_draw_set_vec3(GlobalOverlay.wire, GlobalOverlay.wire_count++, vertex[temp[2]], vertex[temp[2] + 1], vertex[temp[2] + 2]);
						sui_draw_set_vec3(GlobalOverlay.wire, GlobalOverlay.wire_count++, vertex[temp[0]], vertex[temp[0] + 1], vertex[temp[0] + 2]);
					}
				}
			}
		}
	}
	GlobalOverlay.surface_version = udg_get_version(TRUE, TRUE, FALSE, FALSE, FALSE);
	draw_persuade_surface(e_ns_get_node(0, udg_get_modeling_node()));
	test_draw();
	glDisable(GL_DEPTH_TEST);
/*	glEnable(GL_CULL_FACE);
	if(GlobalOverlay.tri_count != 0)
	{

		glCullFace(GL_FRONT);
		sui_set_blend_gl(GL_ONE, GL_ONE);
		sui_set_color_array_gl(GlobalOverlay.tri_normal, GlobalOverlay.tri_count, 3);
		sui_draw_gl(GL_TRIANGLES, GlobalOverlay.tri, GlobalOverlay.tri_count, 3, 1, 1, 1);
	}

	if(GlobalOverlay.quad_count != 0)
	{
		glCullFace(GL_FRONT);
		sui_set_blend_gl(GL_ONE, GL_ONE);
		sui_set_color_array_gl(GlobalOverlay.quad_normal, GlobalOverlay.quad_count, 3);
		sui_draw_gl(GL_QUADS, GlobalOverlay.quad, GlobalOverlay.quad_count, 3, 1, 1, 1);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if(GlobalOverlay.tri_count != 0)
	{
		glCullFace(GL_FRONT);
		sui_set_blend_gl(GL_ONE, GL_ONE);
		sui_draw_gl(GL_TRIANGLES, GlobalOverlay.tri, GlobalOverlay.tri_count, 3, 0.1, 0.05, 0.15);
		glCullFace(GL_BACK);
		sui_draw_gl(GL_TRIANGLES, GlobalOverlay.tri, GlobalOverlay.tri_count, 3, 0.05, 0.05, 0.05);
	}	
	if(GlobalOverlay.quad_count != 0)
	{
		sui_set_blend_gl(GL_ONE, GL_ONE);
		glCullFace(GL_FRONT);
		sui_draw_gl(GL_QUADS, GlobalOverlay.quad, GlobalOverlay.quad_count, 3, 0.1, 0.05, 0.15);
		glCullFace(GL_BACK);
		sui_draw_gl(GL_QUADS, GlobalOverlay.quad, GlobalOverlay.quad_count, 3, 0.05, 0.05, 0.05);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
*/
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	if(GlobalOverlay.tri_count != 0)
	{

		glCullFace(GL_FRONT);
		sui_set_blend_gl(GL_ONE, GL_ONE);
		sui_set_color_array_gl(GlobalOverlay.tri_normal, GlobalOverlay.tri_count, 3);
		sui_draw_gl(GL_TRIANGLES, GlobalOverlay.tri, GlobalOverlay.tri_count, 3, 1, 1, 1);
	}

	if(GlobalOverlay.quad_count != 0)
	{
		glCullFace(GL_FRONT);
		sui_set_blend_gl(GL_ONE, GL_ONE);
		sui_set_color_array_gl(GlobalOverlay.quad_normal, GlobalOverlay.quad_count, 3);
		sui_draw_gl(GL_QUADS, GlobalOverlay.quad, GlobalOverlay.quad_count, 3, 1, 1, 1);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if(GlobalOverlay.tri_count != 0)
	{
		glCullFace(GL_FRONT);
		sui_set_blend_gl(GL_ONE, GL_ONE);
		sui_draw_gl(GL_TRIANGLES, GlobalOverlay.tri, GlobalOverlay.tri_count, 3, 0.05, 0.1, 0.15);
		glCullFace(GL_BACK);
		sui_draw_gl(GL_TRIANGLES, GlobalOverlay.tri, GlobalOverlay.tri_count, 3, 0.05, 0.05, 0.05);
	}	
	if(GlobalOverlay.quad_count != 0)
	{
		sui_set_blend_gl(GL_ONE, GL_ONE);
		glCullFace(GL_FRONT);
		sui_draw_gl(GL_QUADS, GlobalOverlay.quad, GlobalOverlay.quad_count, 3, 0.05, 0.1, 0.15);
		glCullFace(GL_BACK);
		sui_draw_gl(GL_QUADS, GlobalOverlay.quad, GlobalOverlay.quad_count, 3, 0.05, 0.05, 0.05);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	if(GlobalOverlay.wire_count != 0)
		sui_draw_gl(GL_LINES, GlobalOverlay.wire, GlobalOverlay.wire_count, 3, 0.5, 0.4, 0.5);

	glDisable(GL_CULL_FACE);
	draw_owerlay_tags();
	glEnable(GL_DEPTH_TEST);
}

void draw_owerlay_vertex(void)
{
	static float *base_lines = NULL, *vertex_array = NULL, *vertex_color = NULL;
	static uint vertex_version = 0, vertex_count = 0;
	static boolean no_select = TRUE;
	double *vertex, *base;
	double select, manip_pos[3] ={0, 0, 0}, sum = 0;
	uint length, count, i, j;
	boolean manip = TRUE;
	UNDOTag	*tag;
	tag = udg_get_tags(&count);
	udg_get_geometry(&length, NULL, &vertex, NULL, NULL);
	if(vertex_version != udg_get_version(TRUE, TRUE, TRUE, FALSE, TRUE))
	{
		j = 0;
		for(i = 0; i < length; i++)
			if(vertex[i * 3] != V_REAL64_MAX)
				j++;

		if(vertex_array != NULL)
		{
			free(vertex_array);
			free(vertex_color);
		}
		if(base_lines != NULL)
			free(base_lines);

		for(i = 0; i < count && tag[i].select < 0.001; i++);	

		if(j == 0 /*|| i == count*/)
		{
			vertex_array = NULL;
			vertex_color = NULL;
			base_lines = NULL;
			la_t_tm_hide(TRUE);
			no_select = TRUE;
		}else
		{
			vertex_count = j;
			vertex_array = malloc((sizeof *base_lines) * vertex_count * 3);
			vertex_color = malloc((sizeof *base_lines) * vertex_count * 3);
			if((base = udg_get_base_layer()) != NULL)
				base_lines = malloc((sizeof *base_lines) * vertex_count * 2 * 3);
			j = 0;
			for(i = 0; i < length; i++)
			{
				if(vertex[i * 3] != V_REAL64_MAX)
				{
					sui_draw_set_vec3(vertex_array, j, vertex[i * 3], vertex[i * 3 + 1], vertex[i * 3 + 2]);
					select = udg_get_select(i);
					sui_draw_set_vec3(vertex_color, j, 0.2 + select * 0.8, 0.6 + select * 0.4, 1);
					j++;
					if(select > 0.0001)
					{
						manip = FALSE;
						sum += select;
						manip_pos[0] += vertex[i * 3] * select;
						manip_pos[1] += vertex[i * 3 + 1] * select;
						manip_pos[2] += vertex[i * 3 + 2] * select;
					}
				}
			}
			for(i = 0; i < count; i++)
			{
				if(tag[i].select > 0.0001)
				{
					manip = FALSE;
					sum += tag[i].select;
					manip_pos[0] += tag[i].vec[0] * tag[i].select;
					manip_pos[1] += tag[i].vec[1] * tag[i].select;
					manip_pos[2] += tag[i].vec[2] * tag[i].select;
				}
			}
			vertex_version = udg_get_version(TRUE, TRUE, TRUE, FALSE, FALSE);
			if(manip == FALSE && no_select == TRUE)
				la_t_tm_place(manip_pos[0] / sum, manip_pos[1] / sum, manip_pos[2] / sum);
			la_t_tm_hide(manip);
			no_select = manip;
		}
		if((base = udg_get_base_layer()) != NULL)
		{
			j = 0;
			for(i = 0; i < length; i++)
			{	
				if(vertex[i * 3] != V_REAL64_MAX)
				{
					sui_draw_set_vec3(base_lines, j++, vertex[i * 3], vertex[i * 3 + 1], vertex[i * 3 + 2]);
					sui_draw_set_vec3(base_lines, j++, base[i * 3], base[i * 3 + 1], base[i * 3 + 2]);
				}
			}
		}
	}
	if(vertex_array != NULL)
	{
		sui_set_color_array_gl(vertex_color, vertex_count, 3);
		sui_draw_gl(GL_POINTS, vertex_array, vertex_count, 3, 1, 1, 1);
	}
	if(base_lines != NULL)
	{
		sui_draw_gl(GL_POINTS, vertex_array, vertex_count, 3, 0.1, 0.1, 0.1 );
	}
}
void draw_owerlay_edge(void)
{
	static float *edge_array = NULL, *edge_color = NULL;
	static uint *edge_ref = NULL, el = 0, ev = 0;
	double *vertex, *vertex0, *vertex1, *vertex2;
	uint length, edge_length, i, j,  *edge;

	udg_get_geometry(&length, NULL, &vertex, NULL, NULL);
	edge = udg_get_edge_data(&edge_length);
	if(el != edge_length || ev != udg_get_version(TRUE, TRUE, FALSE, TRUE, FALSE))
	{
		if(edge_array != 0)
		{
			free(edge_array);
			free(edge_color);
			free(edge_ref);
			edge_array = 0;
			edge_color = 0;
			edge_ref = 0;
		}
		if(edge_length != 0)
		{
			edge_array = malloc((sizeof *edge_array) * edge_length * 5 * 3);
			edge_color = malloc((sizeof *edge_color) * edge_length * 5 * 3);
			edge_ref = malloc((sizeof *edge_ref) * edge_length * 4 * 2);
			for(i = 0; i < edge_length; i++)
			{
				if(edge[i * 2] < length && edge[i * 2 + 1] < length)
				{
					vertex0 = &vertex[edge[i * 2] * 3];
					vertex1 = &vertex[edge[i * 2 + 1] * 3];
					sui_draw_set_vec3(edge_array, i * 5, vertex0[0] * 0.95 + vertex1[0] * 0.05, vertex0[1] * 0.95 + vertex1[1] * 0.05, vertex0[2] * 0.95 + vertex1[2] * 0.05);
					sui_draw_set_vec3(edge_array, i * 5 + 1, vertex0[0] * 0.75 + vertex1[0] * 0.25, vertex0[1] * 0.75 + vertex1[1] * 0.25, vertex0[2] * 0.75 + vertex1[2] * 0.25);
					sui_draw_set_vec3(edge_array, i * 5 + 2, vertex0[0] * 0.50 + vertex1[0] * 0.50, vertex0[1] * 0.50 + vertex1[1] * 0.50, vertex0[2] * 0.50 + vertex1[2] * 0.50);
					sui_draw_set_vec3(edge_array, i * 5 + 3, vertex0[0] * 0.25 + vertex1[0] * 0.75, vertex0[1] * 0.25 + vertex1[1] * 0.75, vertex0[2] * 0.25 + vertex1[2] * 0.75);
					sui_draw_set_vec3(edge_array, i * 5 + 4, vertex0[0] * 0.05 + vertex1[0] * 0.95, vertex0[1] * 0.05 + vertex1[1] * 0.95, vertex0[2] * 0.05 + vertex1[2] * 0.95);
					sui_draw_set_vec3(edge_color, i * 5 + 0, 0.5, 0.5, 0.5);
					sui_draw_set_vec3(edge_color, i * 5 + 1, 0.3, 0.3, 0.3);
					sui_draw_set_vec3(edge_color, i * 5 + 2, 0.2, 0.2, 0.2);
					sui_draw_set_vec3(edge_color, i * 5 + 3, 0.3, 0.3, 0.3);
					sui_draw_set_vec3(edge_color, i * 5 + 4, 0.5, 0.5, 0.5);
				}else for(j = 0; j < 5; j++)
				{
					sui_draw_set_vec3(edge_array, i * 5 + j, 0, 0, 0);
					sui_draw_set_vec3(edge_color, i * 5 + j, 0, 0, 0);
				}
				sui_draw_set_ivec2(edge_ref, i * 4 + 0, i * 5, i * 5 + 1);
				sui_draw_set_ivec2(edge_ref, i * 4 + 1, i * 5 + 1, i * 5 + 2);
				sui_draw_set_ivec2(edge_ref, i * 4 + 2, i * 5 + 2, i * 5 + 3);
				sui_draw_set_ivec2(edge_ref, i * 4 + 3, i * 5 + 3, i * 5 + 4);
			}
		}
		el = edge_length;
		ev = udg_get_version(TRUE, TRUE, FALSE, TRUE, FALSE);
	}
	if(el != 0)
	{
		sui_set_blend_gl(GL_ONE, GL_ONE);
		sui_set_color_array_gl(edge_color, el * 5, 3);
		sui_draw_elements_gl(GL_LINES, edge_array, edge_ref, el * 8, 3, 1, 1, 1);
	}
}


extern draw_view_cage(void);

void la_do_owerlay(void)
{
	static uint version, draw = FALSE;
//	if(sui_test_setting_version(&version))
//		draw = sui_get_setting_int("RENDER_AS_SDS", TRUE);
	if(draw_view_cage() || draw != TRUE)
		draw_owerlay_surface();
	draw_owerlay_edge();
	draw_owerlay_vertex();
}


