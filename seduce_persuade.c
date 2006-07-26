
#include <math.h>

#include "st_matrix_operations.h"
#include "enough.h"
#include "seduce.h"
#include "persuade.h"

void sp_settings_pre(SUIViewElement *element)
{
	char text[32] = {0};
	element[0].type = S_VET_SPLIT;
	element[0].text = "geometry";

	element[1].type = S_VET_INTEGER;
	element[1].text = "Tesselation";
	element[1].param.integer = p_geo_get_sds_level();

	element[2].type = S_VET_INTEGER;
	element[2].text = "Poly Cap";
	element[2].param.integer = p_geo_get_sds_poly_cap();

	element[3].type = S_VET_SLIDER;
	element[3].text = "Complexity";
	element[3].param.slider = sqrt(p_geo_get_sds_mesh_factor() / 1000.0);

	element[4].type = S_VET_INTEGER;
	element[4].text = "Force Tesselation";
	element[4].param.integer = p_geo_get_sds_force_level();

	element[5].type = S_VET_SPLIT;
	element[5].text = "render to texture";

	element[6].type = S_VET_REAL;
	element[6].text = "Impostor Size";
	element[6].param.real = p_render_get_impostor_size();

	element[7].type = S_VET_INTEGER;
	element[7].text = "Resolution";
	element[7].param.integer = p_render_get_impostor_resolution();

	element[8].type = S_VET_BOOLEAN;
	element[8].text = "Draw Shadows";
	element[8].param.checkbox = p_render_get_shadows();

	element[9].type = S_VET_BOOLEAN;
	element[9].text = "Draw Wireframe";
	element[9].param.checkbox = p_render_get_wireframe();

	element[10].type = S_VET_BOOLEAN;
	element[10].text = "Use HDRI";
	element[10].param.checkbox = p_th_get_sds_use_hdri();

	element[11].type = S_VET_BOOLEAN;
	element[11].text = "Draw flares";
	element[11].param.checkbox = p_render_get_flare();
}

void sp_settings_post(SUIViewElement *element)
{
	p_geo_set_sds_level(element[1].param.integer);
	p_geo_set_sds_poly_cap(element[2].param.integer);
	p_geo_set_sds_mesh_factor(element[3].param.slider * element[3].param.slider * 1000.0);
	p_geo_set_sds_force_level(element[4].param.integer);
	p_render_set_impostor_size(element[6].param.real);
	p_render_set_impostor_resolution(element[7].param.integer);
	p_render_set_shadows(element[8].param.checkbox);
	p_render_set_wireframe(element[9].param.checkbox);
	if(element[10].param.checkbox != p_th_get_sds_use_hdri())
	{
		p_th_set_sds_use_hdri(element[10].param.checkbox);
		p_context_update();
	}
	p_render_set_flare(element[11].param.checkbox);
	
}
