
#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
	#include <windows.h>
	#include <GL/gl.h>
#else
	#include <gl.h>
#endif

#include "enough.h"
#include "p_task.h"
#include "persuade.h"

#include "p_object.h"
#include "p_shader.h"


void p_render_object_old(ENode *node)
{
	double matrix[16], dpos[3], dlight[3];
	float pos[4] = {0, 1, 0, 1}, light[4] = {1, 1, 1, 1};
	ENode *light_node;
	PRenderSets *set;
	PObject *o;
	uint32 i, j = 0;
	o = e_ns_get_custom_data(node, P_ENOUGH_SLOT);

	for(i = 0; i < LIGHTS_PER_OBJECT; i++)
		glDisable(GL_LIGHT0 + i);

	/*just adding a standard light */
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0 + j, GL_POSITION, pos);
	glLightfv(GL_LIGHT0 + j, GL_DIFFUSE, light);
	j++;

	for(i = 0; i < LIGHTS_PER_OBJECT && j < 12; i++)
	{
		light_node = e_ns_get_node(0, o->lights[i]);
		if(light_node != NULL && V_NT_OBJECT == e_ns_get_node_type(light_node))
		{
		//	e_nso_get_position(light_node, dpos);
			e_nso_get_light(light_node, dlight);
			if(dlight[0] > 0.00001 || dlight[1] > 0.00001 || dlight[2] > 0.00001)
			{
				glEnable(GL_LIGHT0 + j);
				pos[0] = dpos[0];
				pos[1] = dpos[1];
				pos[2] = dpos[2];
				light[0] = dlight[0];
				light[1] = dlight[1];
				light[2] = dlight[2];
				glLightfv(GL_LIGHT0 + j, GL_POSITION, pos);
				glLightfv(GL_LIGHT0 + j, GL_DIFFUSE, light);
				j++;
			}
		}
	}
	glPushMatrix();
//	e_nso_get_matrix(node, matrix);

	for(i = 0; i < o->set_count; i++)
	{
		set = &o->sets[i];
		if(set->draw != NULL)
		{
			p_shader_bind(set->material);
			glEnableClientState(GL_NORMAL_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, p_rm_get_vertex(set->draw));
			glNormalPointer(GL_FLOAT, 0, p_rm_get_normal(set->draw));
			glDrawElements(GL_TRIANGLES, p_rm_get_ref_length(set->draw), GL_UNSIGNED_INT, p_rm_get_reference(set->draw));
			glDisableClientState(GL_NORMAL_ARRAY);
		}
	}
	glPopMatrix();
}