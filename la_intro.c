#include "la_includes.h"

#include "la_geometry_undo.h"

#include "la_tool.h"
#include "st_matrix_operations.h"
#include "la_particle_fx.h"
#include "la_projection.h"


struct{
	float	*blade;
	float	*ring;
}GlobalIntro;

#define CIRCLE_SEGMENTS 24
#define PI  3.141592653

void la_intro(void)
{
	uint i;
	GlobalIntro.blade = malloc((sizeof * GlobalIntro.blade) * 28 * 2);
	GlobalIntro.ring = malloc((sizeof * GlobalIntro.ring) * CIRCLE_SEGMENTS * 4 * 2);
	for(i = 0; i < CIRCLE_SEGMENTS; i++)
	{
		sui_draw_set_vec2(GlobalIntro.ring, i * 4 + 0, 0.09 * sin((float)i * PI * 2 / CIRCLE_SEGMENTS), 0.09 * cos((float)i * PI * 2 / CIRCLE_SEGMENTS));
		sui_draw_set_vec2(GlobalIntro.ring, i * 4 + 1, 0.11 * sin((float)i * PI * 2 / CIRCLE_SEGMENTS), 0.11 * cos((float)i * PI * 2 / CIRCLE_SEGMENTS));
		sui_draw_set_vec2(GlobalIntro.ring, i * 4 + 2, 0.11 * sin((float)(i + 1) * PI * 2 / CIRCLE_SEGMENTS), 0.11 * cos((float)(i + 1) * PI * 2 / CIRCLE_SEGMENTS));
		sui_draw_set_vec2(GlobalIntro.ring, i * 4 + 3, 0.09 * sin((float)(i + 1) * PI * 2 / CIRCLE_SEGMENTS), 0.09 * cos((float)(i + 1) * PI * 2 / CIRCLE_SEGMENTS));
	}

	sui_draw_set_vec2(GlobalIntro.blade, 0, 0.01, 0.4175);
	sui_draw_set_vec2(GlobalIntro.blade, 1, -0.01, 0.4175);
	sui_draw_set_vec2(GlobalIntro.blade, 2, -0.02, 0.415);
	sui_draw_set_vec2(GlobalIntro.blade, 3, 0.02, 0.415);

	sui_draw_set_vec2(GlobalIntro.blade, 4, 0.02, 0.415);
	sui_draw_set_vec2(GlobalIntro.blade, 5, -0.02, 0.415);
	sui_draw_set_vec2(GlobalIntro.blade, 6, -0.03, 0.41);
	sui_draw_set_vec2(GlobalIntro.blade, 7, 0.03, 0.41);

	sui_draw_set_vec2(GlobalIntro.blade, 8, 0.03, 0.41);
	sui_draw_set_vec2(GlobalIntro.blade, 9, -0.03, 0.41);
	sui_draw_set_vec2(GlobalIntro.blade, 10, -0.06, 0.03);
	sui_draw_set_vec2(GlobalIntro.blade, 11, 0.06, 0.03);

	sui_draw_set_vec2(GlobalIntro.blade, 12, 0.06, 0.03);
	sui_draw_set_vec2(GlobalIntro.blade, 13, -0.06, 0.03);
	sui_draw_set_vec2(GlobalIntro.blade, 14, -0.057, 0.025);
	sui_draw_set_vec2(GlobalIntro.blade, 15, 0.057, 0.025);

	sui_draw_set_vec2(GlobalIntro.blade, 16, 0.057, 0.025);
	sui_draw_set_vec2(GlobalIntro.blade, 17, -0.057, 0.025);
	sui_draw_set_vec2(GlobalIntro.blade, 18, -0.05, 0.02);
	sui_draw_set_vec2(GlobalIntro.blade, 19, 0.05, 0.02);

	sui_draw_set_vec2(GlobalIntro.blade, 20, -0.05, 0.02);
	sui_draw_set_vec2(GlobalIntro.blade, 21, -0.04, 0.016);
	sui_draw_set_vec2(GlobalIntro.blade, 22, -0.035, 0.014);
	sui_draw_set_vec2(GlobalIntro.blade, 23, -0.01, 0.02);


	sui_draw_set_vec2(GlobalIntro.blade, 24, 0.035, 0.014);
	sui_draw_set_vec2(GlobalIntro.blade, 25, 0.04, 0.016);
	sui_draw_set_vec2(GlobalIntro.blade, 26, 0.05, 0.02);
	sui_draw_set_vec2(GlobalIntro.blade, 27, 0.01, 0.02);
}


void draw_la_star_wire(double open)
{
	uint i;
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPushMatrix();
	sui_draw_text(0.1, 0.1, SUI_T_SIZE, SUI_T_SPACE, "LOQ AIROU", 0, 0, 0);
	glRotated(20 * open, 0, 0, 1);
	sui_draw_gl(GL_QUADS, GlobalIntro.ring, CIRCLE_SEGMENTS * 4, 2, 0, 0, 0);
	glPopMatrix();
	for(i = 0; i < 4; i++)
	{
		glPushMatrix();
		glRotated(90 * (double)i, 0, 0, 1);
		glTranslated(0, 0.1, 0);
		glRotated(open * 100, 1, 0, 0);
		sui_draw_gl(GL_QUADS, GlobalIntro.blade, 28, 2, 0, 0, 0);
		glPopMatrix();
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void draw_la_star(double open)
{
	uint i;
	glPushMatrix();
	sui_draw_gl(GL_QUADS, GlobalIntro.ring, CIRCLE_SEGMENTS * 4, 2, 0, 0, 0);
	glPopMatrix();
	for(i = 0; i < 4; i++)
	{
		glPushMatrix();
		glRotated(90 * (double)i, 0, 0, 1);
		glTranslated(0, 0.1, 0);
		glRotated(open * 100, 1, 0, 0);
		sui_draw_gl(GL_QUADS, GlobalIntro.blade, 28, 2, 0, 0, 0);
		glPopMatrix();
	}

}

void draw_la_star_drop(double x, double y, double z, double timer, boolean wire, boolean spark)
{
	double t2, pos[3];
	glPushMatrix();
	t2 = timer;
	if(timer > 1)
	{
		timer = 1;
		if(!wire)
			spark = FALSE;
	}
	timer = 1 - timer;
	pos[0] = x;
	pos[1] = y; 
	pos[2] = z - timer * timer * timer * 1;
	glTranslated(pos[0], pos[1], pos[2]);
	if(spark)
		la_pfx_create_intro_spark(pos);
	glRotated(20 * t2, 0, 0, 1);
	glRotated(timer * sin(timer * 20), 0, 1, 0);
	glRotated(timer * cos(timer * 20), 1, 0, 0);
	if(wire)
		draw_la_star(1 - (1 / (1 + timer * timer * timer * timer)));
	else
		draw_la_star_wire(1 - (1 / (1 + timer * timer * timer * timer)));
	glPopMatrix();
}


void la_intro_draw(void)
{
	uint i;
	static double t = -100, rot = 1;
	static uint spark = 0;
	double r;

	glPushMatrix();
	t++;
	spark++;
//	sw_draw_background_square();
	r = t - 400;
	if(r > 0)
	r = 0;
//	sui_draw_text("LOQ", SUITM_SPACED, NULL, NULL, 0, -0.28 + r * 0.002, 0, 0.5, sui_get_material(SUIM_BLACK_FONT));
//	sui_draw_text(-0.44 + r * 0.002, 0, SUI_T_SIZE * 4, SUI_T_SPACE, "LOQ", 0.5, 0.5, 0.5);
//	sui_draw_text("AIROU", SUITM_SPACED, NULL, NULL, 0, 0.2 - r * 0.002, 0, 0.5, sui_get_material(SUIM_BLACK_FONT));
//	sui_draw_text(0.3 - r * 0.002, 0, SUI_T_SIZE * 4, SUI_T_SPACE, "AIROU", 0.5, 0.5, 0.5);
	glScaled(0.2, 0.2, 0.2);
	if(t > 100)
		rot *= 0.992;
	glTranslated(0, 0, 2 - (2 * rot));
	glRotated(rot * 30, 0, 0, 1);
	glRotated(rot * -90, 0, 1, 0);

	p_projection_update();
	for(i = 1; i < 40; i++)
		draw_la_star_drop(sin(i * 4) * 1.5, cos(i * 343.6) * 1.5, 1 + cos(i * 656.3) * 4.6 + t * 0.01 - 1, t * 0.02 + ((cos(i * 7) * 2) - 3), FALSE, spark % 10 == i % 10);
	draw_la_star_drop(0, 0, 1.5, t * 0.01, TRUE, spark % 4 == 1);
	la_pfx_draw(TRUE);
	glPopMatrix();

}

