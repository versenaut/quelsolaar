
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

boolean qs_draw_settings(BInputState *input)
{
	SUIViewElement element[14];
	boolean output;
	char text[32] = {0};
	sp_settings_pre(element);
	element[13].type = S_VET_COLOR;
	element[13].text = "background";
	element[13].param.color[0] = background_color[0];
	element[13].param.color[1] = background_color[1];
	element[13].param.color[2] = background_color[2];
	output = !sui_draw_setting_view(input, 0, 0.3, 0.5, element, 14, "SETTINGS", 0.0);
	sp_settings_post(element);
	background_color[0] = element[13].param.color[0];
	background_color[1] = element[13].param.color[1];
	background_color[2] = element[13].param.color[2];
	return output;
}
