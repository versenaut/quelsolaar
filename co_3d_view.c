
#include <math.h>

#include "st_matrix_operations.h"
#include "enough.h"
#include "seduce.h"
#include "persuade.h"
#include "deceive.h"

#include "co_vn_handle.h"

void co_geometry_destroy(void *g);
void *co_geometry_draw(ENode *node, void *g, boolean fill, boolean scale, float red, float green, float blue, float alpha);
void co_vng_ring(void);

uint select_node = -1;

extern float co_get_pos_x(float x);
extern float co_get_pos_y(float y);
extern float co_get_view_x(float x);
extern float co_get_view_y(float y);
extern void co_draw_summary(ENode *node, float x, float y);
extern void p_get_projection_screen(double *output, double x, double y, double z);

extern float co_background_color[3];
extern float co_line_color[3];

boolean co_draw_3d_click_test_node(float x, float y)
{
	ENode *node;
	COVerseNode *co_node;
	node = e_ns_get_node(0, select_node);
	if(node != NULL)
	{
		co_node = e_ns_get_custom_data(node, CONNECTOR_ENOUGH_SLOT);
		co_node->hidden = FALSE;
		co_node->viewlock = TRUE;
		co_node->pos_x = co_get_pos_x(x);
		co_node->pos_y = co_get_pos_y(y);
		return TRUE;
	}
	return FALSE;
}

void co_draw_summary(ENode *node, float x, float y);

void co_draw_3d_summary(void)
{
	uint seconds, fractions;
	double pos[3], output[3];
	ENode *node;
	node = e_ns_get_node(0, select_node);
	if(node != NULL)
	{
		verse_session_get_time(&seconds, &fractions);
		e_nso_get_pos_time(node, pos, seconds, fractions);
		p_get_projection_screen(output, pos[0], pos[1], pos[2]);
		if(output[2] < 0)
			co_draw_summary(node, output[0], output[1]);
	}
}


void co_draw_3d_node_lock(ENode *node)
{
	uint seconds, fractions;
	double pos[3], output[3], f;
	COVerseNode *co_node;

	verse_session_get_time(&seconds, &fractions);
	co_node = e_ns_get_custom_data(node, CONNECTOR_ENOUGH_SLOT);
	e_nso_get_pos_time(node, pos, seconds, fractions);
	p_get_projection_screen(output, pos[0], pos[1], pos[2]);
	output[0] += co_get_view_x(co_node->pos_x);
	output[1] += co_get_view_y(co_node->pos_y);
	f = output[0] * output[0] + output[1] * output[1];
	if(f < 0.01)
		co_node->viewlock = TRUE;
}

ENode *co_draw_3d_test(float x, float y)
{
	uint seconds, fractions;
	ENode *node, *g_node, *found_node = NULL;
	double pos[3], output[3], light[3], f, best = 0.001;
	EObjLink *link;
	COVerseNode *co_node;

	verse_session_get_time(&seconds, &fractions);
	for(node = e_ns_get_node_next(0, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
	{
		co_node = e_ns_get_custom_data(node, CONNECTOR_ENOUGH_SLOT);
		if(co_node->search == TRUE)
		{
			for(link = e_nso_get_next_link(node, 0); link != NULL; link = e_nso_get_next_link(node, e_nso_get_link_id(link) + 1))
				if((g_node = e_ns_get_node(0, e_nso_get_link_node(link))) != NULL && V_NT_GEOMETRY == e_ns_get_node_type(g_node))
					break;
			e_nso_get_light(node, light);
			if(link != NULL || light[0] + light[1] + light[2] > 0.00001)
			{
				e_nso_get_pos_time(node, pos, seconds, fractions);
				p_get_projection_screen(output, pos[0], pos[1], pos[2]);
				output[0] += x;
				output[1] += y;
				f = output[0] * output[0] + output[1] * output[1];
				if(output[2] < 0)
				{
					if(f < best)
					{
						best = f;
						found_node = node;
					}
					if(!co_node->hidden && co_node->viewlock == TRUE)
					{
						co_node->pos_x = co_get_pos_x(x - output[0]);
						co_node->pos_y = co_get_pos_y(y - output[1]);
					}
				}
			}
		}
	}
	if(found_node != NULL)
	{
		select_node = e_ns_get_node_id(found_node);
		return found_node;
	}
	select_node = -1;
	return NULL;
}

extern float co_background_color[3];
extern float co_line_color[3];

void co_draw_3d_view_pass(boolean fill, float color)
{
	uint seconds, fractions;
	ENode *node, *g_node;
	double pos[3], scale[3];
	EObjLink *link;
	double matrix[16];
	COVerseNode *co_g_node, *co_node;
	boolean draw;

	verse_session_get_time(&seconds, &fractions);
	for(node = e_ns_get_node_next(0, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
	{
		co_node = e_ns_get_custom_data(node, CONNECTOR_ENOUGH_SLOT);

		if(co_node->search == TRUE && !(fill && e_nso_get_hide(node)))
		{
			draw = FALSE;
			for(link = e_nso_get_next_link(node, 0); link != NULL; link = e_nso_get_next_link(node, e_nso_get_link_id(link) + 1))
			{
				if((g_node = e_ns_get_node(0, e_nso_get_link_node(link))) != NULL && V_NT_GEOMETRY == e_ns_get_node_type(g_node))
				{
					e_nso_get_pos_time(node, pos, seconds, fractions);
					e_nso_get_scale(node, scale);
					glPushMatrix();
					e_nso_get_matrix(node, matrix, 0u, 0u);
					e_nso_get_pos_time(node, &matrix[12], 0, 0);
					glMultMatrixd(matrix);
					co_g_node = e_ns_get_custom_data(g_node, CONNECTOR_ENOUGH_SLOT);
					if((!co_node->hidden || select_node == e_ns_get_node_id(node)) && !fill)
						co_g_node->render_cash = co_geometry_draw(g_node, co_g_node->render_cash, fill, FALSE, co_node->color[0], co_node->color[1], co_node->color[2], 1.0);
					else if(/*!co_g_node->hidden &&*/ !fill)
						co_g_node->render_cash = co_geometry_draw(g_node, co_g_node->render_cash, fill, FALSE, co_node->color[0] * 0.5 + co_background_color[0] * 0.5,
											  co_node->color[1] * 0.5 + co_background_color[1] * 0.5,
											  co_node->color[2] * 0.5 + co_background_color[2] * 0.5, 1.0);
					else
						co_g_node->render_cash = co_geometry_draw(g_node, co_g_node->render_cash, fill, FALSE,
											  co_node->color[0] * (1.0 - color) + co_background_color[0] * color,
											  co_node->color[1] * (1.0 - color) + co_background_color[1] * color,
											  co_node->color[2] * (1.0 - color) + co_background_color[2] * color, 1.0);
					glPopMatrix();
					draw = TRUE;
				}
			}
		/*	if(!draw)
			{
				e_nso_get_pos_time(node, pos, seconds, fractions);
				glPushMatrix();
				p_get_projection_screen(scale, pos[0], pos[1], pos[2]);
				glTranslatef(pos[0], pos[1], pos[2]);
				glScalef(scale[2], scale[2], scale[2]);
				if(co_node->hidden)
					sui_draw_gl(GL_LINES, star, 6,  3, 0.7, 0.7, 0.7);
				else
					sui_draw_gl(GL_LINES, star, 6,  3, 0, 0, 0);
				glPopMatrix();
			}*/
		}
	}
}

extern void co_draw_symbols(ENode *node, float *color, float pos_x, float pos_y, float pos_z);

static boolean draw_3d_view = TRUE;
static boolean draw_3d_persuade = FALSE;

void co_draw_3d_icon_pass(void)
{
//	float star[3 * 6] = {0.02, 0, 0, -0.02, 0, 0, 0,  0.02, 0, 0, -0.02, 0, 0, 0, 0.02, 0, 0, -0.02};
	float shine[2 * 16] = {0, 0.02, 0, 0.04, 0, -0.02, 0, -0.04, 0.02, 0, 0.04, 0, -0.02, 0, -0.04, 0, 0.014, 0.014, 0.028, 0.028, -0.014, 0.014, -0.028, 0.028, -0.014, -0.014, -0.028, -0.028, 0.014, -0.014, 0.028, -0.028};
	float sun[2 * 16] = {0, 0.01, 0.007, 0.007, 0.01, 0, 0.007, 0.007, 0, -0.01, 0.007, -0.007, 0.01, 0, 0.007, -0.007, 0, -0.01, -0.007, -0.007, -0.01, 0, -0.007, -0.007, 0, 0.01, -0.007, 0.007, -0.01, 0, -0.007, 0.007};
	uint seconds, fractions;
	ENode *node;
	double light[3], pos[3], f;
	COVerseNode *co_node;

	if(!draw_3d_view)
		return;
	verse_session_get_time(&seconds, &fractions);
	for(node = e_ns_get_node_next(0, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
	{
		co_node = e_ns_get_custom_data(node, CONNECTOR_ENOUGH_SLOT);
		if(co_node->search == TRUE)
		{
			e_nso_get_pos_time(node, pos, seconds, fractions);
			p_get_projection_screen(pos, pos[0], pos[1], pos[2]);
			e_nso_get_light(node, light);
			if(light[0] + light[1] + light[2] > 0.00001)
			{
				if(pos[2] < 0)
				{
					f = light[0];
					if(light[1] > f)
						f = light[1];
					if(light[2] > f)
						f = light[2];
					f *= 1.1;
					glPushMatrix();
					glTranslatef(-pos[0], -pos[1], -1);
					sui_draw_gl(GL_LINES, shine, 16, 2, light[0] / f, light[1] / f, light[2] / f, 0);
					if(!co_node->hidden)
						sui_draw_gl(GL_LINES, sun, 16, 2, co_line_color[0], co_line_color[1], co_line_color[2], 1.0);
					else
						sui_draw_gl(GL_LINES, sun, 16, 2, co_line_color[0], co_line_color[1], co_line_color[2], 0.5);
					glPopMatrix();
				}
			}
			co_node->color[0] = co_line_color[0];
			co_node->color[1] = co_line_color[1];
			co_node->color[2] = co_line_color[2];
			co_draw_symbols(node, co_node->color, -pos[0], -pos[1], -pos[2]);
		}
	}
}

boolean co_draw_3d_view_get(void)
{
	return draw_3d_view;
}

boolean co_draw_3d_persuade_get(void)
{
	return draw_3d_persuade;
}

void co_draw_3d_view_set(boolean set)
{
	select_node = -1;
	draw_3d_view = set;
}

void co_draw_3d_persuade_set(boolean set)
{
	select_node = -1;
	draw_3d_persuade = set;
}


void co_draw_3d_view(float x, float y)
{

	if(!draw_3d_view)
		return;
	co_draw_3d_test(x, y);
	glEnable(GL_DEPTH_TEST);
	if(draw_3d_persuade)
	{
#if defined PERSUADE_ENABLED
		glClearColor(0.0, 0.0, 0.0, 0);
		p_draw_scene();
		{
			uint x, y;
			betray_get_screen_mode(&x, &y, NULL);
			betray_reshape_view(x, y);
		}
		sui_set_blend_gl(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//	glPushMatrix();
#endif
	}
	else
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		co_draw_3d_view_pass(TRUE, 0.95);
		glPolygonOffset(1.0, 1.0);
		glDepthFunc(GL_LEQUAL);
		glPolygonOffset(0, 0);
		co_draw_3d_view_pass(FALSE, 0.95);
		glDisable(GL_POLYGON_OFFSET_FILL);
		sui_set_blend_gl(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
//	glDepthRange(0.0, 0.5);
	glDisable(GL_DEPTH_TEST);
}
