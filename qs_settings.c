
#include <math.h>

#include "st_matrix_operations.h"
#include "enough.h"
#include "seduce.h"
#include "persuade.h"

static float background_color[3] = {0.0, 0.0, 0.0};

void qs_settings_get_background_color(float *color)
{
	color[0] = background_color[0];
	color[1] = background_color[1];
	color[2] = background_color[2];
}

void sp_settings_pre(SUIViewElement *element);
void sp_settings_post(SUIViewElement *element);

void qs_draw_settings(BInputState *input)
{
	SUIViewElement element[13];
	char text[32] = {0};
	sp_settings_pre(element);
	element[12].type = S_VET_COLOR;
	element[12].text = "background";
	element[12].param.color[0] = background_color[0];
	element[12].param.color[1] = background_color[1];
	element[12].param.color[2] = background_color[2];
	sui_draw_setting_view(input, 0, 0.3, 0.5, element, 13, "SETTINGS", 0.0);
	sp_settings_post(element);
	background_color[0] = element[12].param.color[0];
	background_color[1] = element[12].param.color[1];
	background_color[2] = element[12].param.color[2];
}
