
#include <math.h>

#include "st_matrix_operations.h"
#include "enough.h"
#include "seduce.h"
#include "persuade.h"
#include "deceive.h"

#include "co_vn_handle.h"

void co_geometry_destroy(void *g);
void *co_geometry_draw(ENode *node, void *g, boolean fill, float red, float green, float blue);
void co_vng_ring(void);

uint select_node = -1;

extern float co_get_pos_x(float x);
extern float co_get_pos_y(float y);
extern float co_get_view_x(float x);
extern float co_get_view_y(float y);
extern void co_draw_sumary(ENode *node, float x, float y);

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

void co_draw_sumary(ENode *node, float x, float y);

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
			co_draw_sumary(node, output[0], output[1]);
	}
}


void co_draw_3d_node_lock(ENode *node)
{
	uint seconds, fractions;
	double pos[3], output[3], f;
	EObjLink *link;
	VNQuat64 rot;
	double matrix[16];
	COVerseNode *co_g_node, *co_node;
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
	ENode *node, *found_node = NULL;
	double pos[3], output[3], f, best = 0.001;
	EObjLink *link;
	VNQuat64 rot;
	double matrix[16];
	COVerseNode *co_g_node, *co_node;
	verse_session_get_time(&seconds, &fractions);

	for(node = e_ns_get_node_next(0, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
	{
		co_node = e_ns_get_custom_data(node, CONNECTOR_ENOUGH_SLOT);
		if(co_node->search == TRUE)
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
	if(found_node != NULL)
	{
		select_node = e_ns_get_node_id(found_node);
		return found_node;
	}
	select_node = -1;
	return NULL;
}

void co_draw_3d_view_pass(boolean fill, float color)
{
	uint seconds, fractions;
	ENode *node, *g_node;
	double pos[3], scale[3];
	EObjLink *link;
	VNQuat64 rot;
	double matrix[16];
	COVerseNode *co_g_node, *co_node;
	verse_session_get_time(&seconds, &fractions);
	for(node = e_ns_get_node_next(0, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
	{
		co_node = e_ns_get_custom_data(node, CONNECTOR_ENOUGH_SLOT);

		if(co_node->search == TRUE)
		{
			for(link = e_nso_get_next_link(node, 0); link != NULL; link = e_nso_get_next_link(node, e_nso_get_link_id(link) + 1))
			{
				if((g_node = e_ns_get_node(0, e_nso_get_link_node(link))) != NULL && V_NT_GEOMETRY == e_ns_get_node_type(g_node))
				{
					e_nso_get_pos_time(node, pos, seconds, fractions);
					e_nso_get_scale(node, scale);
					glPushMatrix();
					glTranslatef(pos[0], pos[1], pos[2]);
					glScalef(scale[0], scale[1], scale[2]);
					e_nso_get_rot(node, &rot, NULL, NULL, NULL, NULL, NULL);
					rot.w = sqrt(1 - (rot.x * rot.x + rot.y * rot.y + rot.z * rot.z));
					e_quaternions_to_matrixd(matrix, &rot);
					glMultMatrixd(matrix);

					co_g_node = e_ns_get_custom_data(g_node, CONNECTOR_ENOUGH_SLOT);
			
					if((!co_node->hidden || select_node == e_ns_get_node_id(node)) && !fill)
						co_g_node->render_cash = co_geometry_draw(g_node, co_g_node->render_cash, fill, 0, 0, 0);
					else if(!co_g_node->hidden && !fill)
						co_g_node->render_cash = co_geometry_draw(g_node, co_g_node->render_cash, fill, 0.7, 0.7, 0.7);
					else
						co_g_node->render_cash = co_geometry_draw(g_node, co_g_node->render_cash, fill, color, color, color);
					glPopMatrix();
				}
			}
		}
	}
}

static boolean draw_3d_view = TRUE;

boolean co_draw_3d_view_get(void)
{
	return draw_3d_view;
}

void co_draw_3d_view_set(boolean set)
{
	select_node = -1;
	draw_3d_view = set;
}

void co_draw_3d_view(float x, float y)
{
	if(!draw_3d_view)
		return;
	co_draw_3d_test(x, y);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glDepthRange(0.5, 1.0);
	glPolygonOffset(1.0, 1.0);
	co_draw_3d_view_pass(TRUE, 0.99);
	glPolygonOffset(0, 0);
	co_draw_3d_view_pass(FALSE, 0.9);
	glDisable(GL_DEPTH_TEST);
	glDepthRange(0.0, 0.5);
}
