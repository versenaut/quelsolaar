#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "verse.h"
#include "enough.h"
#include "seduce.h"
#include "co_vn_graphics.h"
#include "co_vn_handle.h"
#include "co_widgets.h"

#define LIGHT_VEC_X (0.4 / 0.938083)
#define LIGHT_VEC_Y (0.6 / 0.938083)
#define LIGHT_VEC_Z (0.6 / 0.938083)
#define LIGHT_DIST 5
#define LIGHT_MULT 4

#define LIGHT_SAMPLES 256

#define TEXTURE_RESOLUTION 128

void co_compute_vec(float *vec, float size, uint x, uint y)
{
	float f;
	f = (float)size * 0.5;
	vec[0] = (float)x / f - 1.0;
	vec[1] = (float)y / f - 1.0;
	f = (vec[0] * vec[0] + vec[1] * vec[1]);
	if(f < 1)
		vec[2] = sqrt(1 - f);
	else
		vec[2] = 0;
}

void co_compute_view_vec(float *vec, float size, uint x, uint y)
{
	float f;
	f = (float)size * 0.5;
	vec[0] = ((float)x / f - 1.0) * 0.2;
	vec[1] = ((float)y / f - 1.0) * 0.2;
	f = (vec[0] * vec[0] + vec[1] * vec[1]);
	if(f < 1)
		vec[2] = sqrt(1 - f);
	else
		vec[2] = 0;
}

void co_compute_stocastic_vector(float *vec, float *normal, uint index, float falloff)
{
	float r;
	index *= 3;
	vec[0] = normal[0] + (get_rand(index++) - 0.5) * falloff;
	vec[1] = normal[1] + (get_rand(index++) - 0.5) * falloff;
	vec[2] = normal[2] + (get_rand(index++) - 0.5) * falloff;
	r = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	vec[0] = vec[0] / r;
	vec[1] = vec[1] / r;
	vec[2] = vec[2] / r;
}

void co_compute_refraction(float *vec, float size, uint x, uint y)
{
	float f;
	f = (float)size * 0.5;
	vec[0] = (float)x / f - 1.0;
	vec[1] = (float)y / f - 1.0;
	vec[2] = sqrt(1 - (vec[0] * vec[0] + vec[1] * vec[1]));
//	vec[2] = 0;
}

void co_compute_reflect_color(float *vec, float *color)
{
	float f, checker, x, y;

	if(vec[1] < -0.1)
	{
		x = 2.5 * vec[0] / -vec[1];
		y = 2.5 * vec[2] / -vec[1];
		checker = (float)(((int)y + (int)x + 1000) % 2) * 0.5 + 0.5;

		f = (y - LIGHT_VEC_Z * LIGHT_DIST) * (y - LIGHT_VEC_Z * LIGHT_DIST) + (x - LIGHT_VEC_X * LIGHT_DIST) * (x - LIGHT_VEC_X * LIGHT_DIST) + (1 - LIGHT_VEC_Y * LIGHT_DIST) * (1 - LIGHT_VEC_Y * LIGHT_DIST);
		color[0] += checker * LIGHT_MULT * 0.7 / f;
		color[1] += checker * LIGHT_MULT * 0.7 / f;
		color[2] += checker * LIGHT_MULT * 0.7 / f;

		f = (y + LIGHT_VEC_Z * LIGHT_DIST) * (y + LIGHT_VEC_Z * LIGHT_DIST) + (x - LIGHT_VEC_X * LIGHT_DIST) * (x - LIGHT_VEC_X * LIGHT_DIST) + (1 - LIGHT_VEC_Y * LIGHT_DIST) * (1 - LIGHT_VEC_Y * LIGHT_DIST);
		color[0] += checker * LIGHT_MULT * 0.5 / f;
		color[1] += checker * LIGHT_MULT * 0.4 / f;
		color[2] += checker * LIGHT_MULT * 0.3 / f;

		f = (y + LIGHT_VEC_Z * LIGHT_DIST) * (y + LIGHT_VEC_Z * LIGHT_DIST) + (x + LIGHT_VEC_X * LIGHT_DIST) * (x + LIGHT_VEC_X * LIGHT_DIST) + (1 - LIGHT_VEC_Y * LIGHT_DIST) * (1 - LIGHT_VEC_Y * LIGHT_DIST);
		color[0] += checker * LIGHT_MULT * 0.6 / f;
		color[1] += checker * LIGHT_MULT * 0.8 / f;
		color[2] += checker * LIGHT_MULT * 1.0 / f;

		color[0] += checker * 0.05;
		color[1] += checker * 0.15;
		color[2] += checker * 0.2;
	}else
	{
		f = 0.9 - vec[1];
		color[0] += f * f * f + 0.1;
		color[1] += f * f + 0.3;
		color[2] += f + 0.4;
	}
}


void co_compute_reflect_hightlight(float *vec, float *color, float fall_off)
{
	float f;
	fall_off = fall_off * 0.5 + 0.01;
	fall_off = (fall_off * fall_off);
	f = vec[0] * 0.264906 + vec[1] * 0.264906 + vec[2] *  0.927173 - 1 + fall_off;
	if(f > 0)
	{
		f = f / fall_off;
		f = 0.1 * f * f / fall_off;
		color[0] += f;
		color[1] += f;
		color[2] += f;
	}
}

void co_compute_reflect_vector(float *vec)
{
	float r;
	vec[0] = 0 + vec[0] * 2.0 * vec[2];
	vec[1] = 0 + vec[1] * 2.0 * vec[2];
	vec[2] = 1 + vec[2] * 2.0 * vec[2];
	r = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	vec[0] = vec[0] / r;
	vec[1] = vec[1] / r;
	vec[2] = vec[2] / r;
}

extern uint co_material_get_texture_id(ENode *node, VNMFragmentID id);

float co_noise_function(float *vec, uint level)
{
	int i, j, k, iadd, jadd, kadd;
	float fi, fj, fk, f, values[8]; 
	fi = vec[0] * (egreal)level;
	fj = vec[1] * (egreal)level;
	fk = vec[2] * (egreal)level;
	i = fi;
	j = fj;
	k = fk;
	fi -= (egreal)i;
	fj -= (egreal)j;
	fk -= (egreal)k;

	iadd = (i + 1) % level;
	jadd = ((j + 1) % level) * level;
	kadd = ((k + 1) % level) * level * level;
	j = j * level;
	k = k * level * level;
	
	values[0] = get_rand(i + j + k);
	values[1] = get_rand(iadd + j + k);
	values[2] = get_rand(i + jadd + k);
	values[3] = get_rand(iadd + jadd + k);
	values[4] = get_rand(i + j + kadd);
	values[5] = get_rand(iadd + j + kadd);
	values[6] = get_rand(i + jadd + kadd);
	values[7] = get_rand(iadd + jadd + kadd);
	f = 1.0 - fi;
	values[0] = values[0] * f + values[1] * fi;
	values[2] = values[2] * f + values[3] * fi;
	values[4] = values[4] * f + values[5] * fi;
	values[6] = values[6] * f + values[7] * fi;
	f = 1.0 - fj;
	values[0] = values[0] * f + values[2] * fj;
	values[4] = values[4] * f + values[6] * fj;
	f = 1.0 - fk;
	return values[0] * f + values[4] * fk;
}

float co_noise_multi_function(float *vec)
{
	return co_noise_function(vec, 7) * 0.7 + co_noise_function(vec, 17) * 0.3 + co_noise_function(vec, 31) * 0.15;
}

#define CO_POINT_NOISE_LEVEL 100

float co_noice_point_function(float *vec)
{
	int i, j, k, start, pos, found;
	float fi, fj, fk, f, fbest = 100000, vector[3]; 
	fi = vec[0];
	fj = vec[1];
	fk = vec[2];
	i = fi;
	j = fj;
	k = fk;
	fi -= (egreal)i;
	fj -= (egreal)j;
	fk -= (egreal)k;
	start = i + j * CO_POINT_NOISE_LEVEL + k * CO_POINT_NOISE_LEVEL * CO_POINT_NOISE_LEVEL;
	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < 3; j++)
		{
			for(k = 0; k < 3; k++)
			{
				pos = start + i + j * CO_POINT_NOISE_LEVEL + k * CO_POINT_NOISE_LEVEL * CO_POINT_NOISE_LEVEL;
				vector[0] = get_rand(pos * 3 + 0) + (float)i - (1.0 + fi);
				vector[1] = get_rand(pos * 3 + 1) + (float)j - (1.0 + fj);
				vector[2] = get_rand(pos * 3 + 2) + (float)k - (1.0 + fk);
				f = vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2];	
				if(f < fbest)
				{
					fbest = f;
					found = pos;
				}
			}	
		}
	}
	return get_rand(found);
}

uint render_material(ENode *node, VNMFragmentID id, uint size, uint line, float *buffer)
{
	uint i, samples = 0;
	VMatFrag *frag;

	frag = e_nsm_get_fragment(node, id);
	if(frag == NULL || !e_nsm_enter_fragment(node, id))
	{
		for(i = 0; i < size * 3; i++)
			buffer[i] = 0;
		return 1;
	}

	switch(e_nsm_get_fragment_type(node, id))
	{	
		case VN_M_FT_COLOR :
		{
			uint j;
			float r, g, b;
			r = frag->color.red;
			g = frag->color.green;
			b = frag->color.blue;
			j = 0;
			for(i = 0; i < size * 3;)
			{
				buffer[i++] = r;
				buffer[i++] = g;
				buffer[i++] = b;
			}
			samples = 1;
			break;
		}
		case VN_M_FT_LIGHT :
		{
			uint k = 0, l;
			float normal[3], vec[3], f, light[3];
			if(frag->light.normal_falloff > 0.001)
				samples = LIGHT_SAMPLES;
			else
				samples = 4;
			for(i = 0; i < size; i++)
			{
				co_compute_vec(normal, size, line, i);

				if(frag->light.type > VN_M_LIGHT_DIRECT_AND_AMBIENT)
				{
					normal[0] = -normal[0];
					normal[1] = -normal[1];
					normal[2] = -normal[2];
				}
				if(frag->light.normal_falloff > 0.001)
				{
					light[0] = 0;
					light[1] = 0;
					light[2] = 0;
					for(l = 0; l < LIGHT_SAMPLES; l++)
					{
						co_compute_stocastic_vector(vec, normal, line * LIGHT_SAMPLES * size + i * LIGHT_SAMPLES + l, frag->light.normal_falloff);
			
						if(frag->light.type % 3 != 1)
						{
							f = vec[0] * LIGHT_VEC_X + vec[1] * LIGHT_VEC_Y + vec[2] * -LIGHT_VEC_Z;
							if(f > 0)
							{
								light[0] += f * 0.5;
								light[1] += f * 0.4;
								light[2] += f * 0.3;
							}
							f = vec[0] * -LIGHT_VEC_X + vec[1] * LIGHT_VEC_Y + vec[2] * -LIGHT_VEC_Z;
							if(f > 0)
							{
								light[0] += f * 0.6;
								light[1] += f * 0.8;
								light[2] += f * 1;
							}
							f = vec[0] * LIGHT_VEC_X + vec[1] * LIGHT_VEC_Y + vec[2] * LIGHT_VEC_Z;
							if(f > 0)
							{
								light[0] += f * 0.7;
								light[1] += f * 0.7;
								light[2] += f * 0.7;
							}
						}
						if(frag->light.type % 3 > 0)
						{
							f = 0.1 + 0.1 * vec[0];
							light[0] += f;
							light[1] += f;
							light[2] += f;
						}
					}
					buffer[k++] = light[0] / LIGHT_SAMPLES;
					buffer[k++] = light[1] / LIGHT_SAMPLES;
					buffer[k++] = light[2] / LIGHT_SAMPLES;
				}else
				{	
					buffer[k + 0] = 0;
					buffer[k + 1] = 0;
					buffer[k + 2] = 0;
					if(frag->light.type % 3 != 1)
					{
						f = normal[0] * LIGHT_VEC_X + normal[1] * LIGHT_VEC_Y + normal[2] * -LIGHT_VEC_Z;
						if(f > 0)
						{
							buffer[k + 0] += f * 0.5;
							buffer[k + 1] += f * 0.4;
							buffer[k + 2] += f * 0.3;
						}
						f = normal[0] * -LIGHT_VEC_X + normal[1] * LIGHT_VEC_Y + normal[2] * -LIGHT_VEC_Z;
						if(f > 0)
						{
							buffer[k + 0] += f * 0.6;
							buffer[k + 1] += f * 0.8;
							buffer[k + 2] += f * 1;
						}
						f = normal[0] * LIGHT_VEC_X + normal[1] * LIGHT_VEC_Y + normal[2] * LIGHT_VEC_Z;
						if(f > 0)
						{
							buffer[k + 0] += f * 0.7;
							buffer[k + 1] += f * 0.7;
							buffer[k + 2] += f * 0.7;
						}
					}
					if(frag->light.type % 3 > 0)
					{
						f = 0.1 + 0.1 * normal[0];
						buffer[k + 0] += f;
						buffer[k + 1] += f;
						buffer[k + 2] += f;
					}
					k += 3;
				}
			}
			break;
		}
		case VN_M_FT_REFLECTION :
		{
			uint k = 0, l;
			float vec[3], normal[3], light[3];
			if(frag->reflection.normal_falloff > 0.001)
				samples = LIGHT_SAMPLES;
			else
				samples = 1;
			for(i = 0; i < size; i++)
			{
				co_compute_vec(normal, size, line, i);
				co_compute_reflect_vector(normal);
				if(frag->reflection.normal_falloff > 0.001)
				{
					light[0] = 0;
					light[1] = 0;
					light[2] = 0;
					for(l = 0; l < LIGHT_SAMPLES; l++)
					{
						co_compute_stocastic_vector(vec, normal, line * LIGHT_SAMPLES * size + i * LIGHT_SAMPLES + l, frag->reflection.normal_falloff);
						co_compute_reflect_color(vec, light);
					}
					buffer[k] = light[0] / LIGHT_SAMPLES;
					buffer[k + 1] = light[1] / LIGHT_SAMPLES;
					buffer[k + 2] = light[2] / LIGHT_SAMPLES;
				}else
				{
					buffer[k] = 0;
					buffer[k + 1] = 0;
					buffer[k + 2] = 0;
					co_compute_reflect_color(normal, &buffer[k]);
				}
				co_compute_reflect_hightlight(normal, &buffer[k], frag->reflection.normal_falloff);
				k += 3;
			}
			break;
		}	
		case VN_M_FT_TRANSPARENCY :
		{
			uint j, k = 0, l;
			float f, vec[3], normal[3], light[3];
			if(frag->transparency.normal_falloff > 0.001)
				samples = LIGHT_SAMPLES;
			else
				samples = 1;
			for(i = 0; i < size; i++)
			{
				co_compute_vec(vec, size, line, i);
				co_compute_view_vec(normal, size, line, i);
				f = (vec[0] * normal[0] + vec[1] * normal[1] + vec[2] * normal[2]) * (frag->transparency.refraction_index - 1.0);
				normal[0] -= vec[0] * f;
				normal[1] -= vec[1] * f;
				normal[2] -= vec[2] * f;
				f = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
				normal[0] /= f;
				normal[1] /= f;
				normal[2] /= f;


				normal[2] = -normal[2];
				if(frag->transparency.normal_falloff > 0.001)
				{
					light[0] = 0;
					light[1] = 0;
					light[2] = 0;
					for(l = 0; l < LIGHT_SAMPLES; l++)
					{
						co_compute_stocastic_vector(vec, normal, line * LIGHT_SAMPLES * size + i * LIGHT_SAMPLES + l, frag->transparency.normal_falloff);
						co_compute_reflect_color(vec, light);
					}
					buffer[k] = light[0] / LIGHT_SAMPLES;
					buffer[k + 1] = light[1] / LIGHT_SAMPLES;
					buffer[k + 2] = light[2] / LIGHT_SAMPLES;
				}else
				{
					buffer[k] = 0;
					buffer[k + 1] = 0;
					buffer[k + 2] = 0;
					co_compute_reflect_color(normal, &buffer[k]);
				}
				k += 3;
			}
			break;
		}
		case VN_M_FT_VOLUME :
		{
			uint k = 0, l;
			float normal[3],  f, f2, light[3];
			for(i = 0; i < size; i++)
			{
				co_compute_vec(normal, size, line, i);
				light[0] = 0;
				light[1] = 0;
				light[2] = 0;

				f = normal[0] * LIGHT_VEC_X + normal[1] * LIGHT_VEC_Y + normal[2] * -LIGHT_VEC_Z;
				
				f2 = (frag->volume.col_r + f) / (frag->volume.col_r + 1);
				if(f2 > 0)
					light[0] += f2 * 0.5;
				f2 = (frag->volume.col_g + f) / (frag->volume.col_g + 1);
				if(f2 > 0)
					light[1] += f2 * 0.4;
				f2 = (frag->volume.col_b + f) / (frag->volume.col_b + 1);
				if(f2 > 0)
					light[2] += f2 * 0.3;

				f = normal[0] * -LIGHT_VEC_X + normal[1] * LIGHT_VEC_Y + normal[2] * -LIGHT_VEC_Z;

				f2 = (frag->volume.col_r + f) / (frag->volume.col_r + 1);
				if(f2 > 0)
					light[0] += f2 * 0.6;
				f2 = (frag->volume.col_g + f) / (frag->volume.col_g + 1);
				if(f2 > 0)
					light[1] += f2 * 0.8;
				f2 = (frag->volume.col_b + f) / (frag->volume.col_b + 1);
				if(f2 > 0)
					light[2] += f2 * 1;

				f = normal[0] * LIGHT_VEC_X + normal[1] * LIGHT_VEC_Y + normal[2] * LIGHT_VEC_Z;

				f2 = (frag->volume.col_r + f) / (frag->volume.col_r + 1);
				if(f2 > 0)
					light[0] += f2 * 0.7;
				f2 = (frag->volume.col_g + f) / (frag->volume.col_g + 1);
				if(f2 > 0)
					light[1] += f2 * 0.7;
				f2 = (frag->volume.col_b + f) / (frag->volume.col_b + 1);
				if(f2 > 0)
					light[2] += f2 * 0.7;

				buffer[k++] = light[0];
				buffer[k++] = light[1];
				buffer[k++] = light[2];
			}
			samples = 4;
			break;
		}
		case VN_M_FT_VIEW :
		{
			uint j, k = 0;
			float vec[3], light;
			for(i = 0; i < size; i++)
			{
				co_compute_vec(vec, size, line, i);
				buffer[k++] = vec[0];
				buffer[k++] = vec[1];
				buffer[k++] = vec[2];
			}
			break;
			samples = 1;
		}
		break;
		case VN_M_FT_GEOMETRY :
		{
			uint j, k = 0;
			float vec[3], light;
			for(i = 0; i < size; i++)
			{
				co_compute_vec(vec, size, line, i);
				buffer[k++] = (vec[0] + 1.0) * 0.5;
				buffer[k++] = (vec[1] + 1.0) * 0.5;
				buffer[k++] = (vec[2] + 1.0) * 0.5;
			}
			samples = 1;
			break;
		}
		case VN_M_FT_TEXTURE :
		{
			uint k = 0;
			ebreal output[3];
			EBMHandle	*handle;
			handle = e_nsb_get_image_handle(frag->texture.bitmap, frag->texture.layer_r, frag->texture.layer_g, frag->texture.layer_b);
			samples = 4 + render_material(node, frag->texture.mapping, size, line, buffer);
			for(i = 0; i < size; i++)
			{
				e_nsb_evaluate_image_handle_tile(handle, output, buffer[k], buffer[k + 1], buffer[k + 2]);
				buffer[k++] = output[0];
				buffer[k++] = output[1];
				buffer[k++] = output[2];
			}
			e_nsb_destroy_image_handle(handle);
			break;
		}
		case VN_M_FT_ANIMATION :
		{
			for(i = 0; i < size * size * 3; i++)
				buffer[i] = 0.5;
			samples = 1;
			break;
		}
		case VN_M_FT_NOISE :
		{
			uint k = 0;
			float f;
			render_material(node, frag->noise.mapping, size, line, buffer);
			switch(frag->noise.type)
			{
				case VN_M_NOISE_PERLIN_ZERO_TO_ONE :
					for(i = 0; i < size; i++)
					{
						f = co_noise_multi_function(&buffer[k]);
						buffer[k++] = f;
						buffer[k++] = f;
						buffer[k++] = f;
					}
				break;
				case VN_M_NOISE_PERLIN_MINUS_ONE_TO_ONE :
					for(i = 0; i < size; i++)
					{
						f = co_noise_multi_function(&buffer[k]) * 2 - 1;
						buffer[k++] = f;
						buffer[k++] = f;
						buffer[k++] = f;
					}
				break;
				case VN_M_NOISE_POINT_ZERO_TO_ONE :
					for(i = 0; i < size; i++)
					{
						f = co_noice_point_function(&buffer[k]);
						buffer[k++] = f;
						buffer[k++] = f;
						buffer[k++] = f;
					}
				break;
				case VN_M_NOISE_POINT_MINUS_ONE_TO_ONE :
					for(i = 0; i < size; i++)
					{
						f = co_noice_point_function(&buffer[k]) * 2 - 1;
						buffer[k++] = f;
						buffer[k++] = f;
						buffer[k++] = f;
					}
				break;

			}
			samples = 4;
			break;
		}
		case VN_M_FT_BLENDER :
		{
			float *buf, *control, f;
			buf = malloc((sizeof *buf) * size * 3);
			samples = 1;
			samples += render_material(node, frag->blender.data_a, size, line, buffer);
			samples += render_material(node, frag->blender.data_b, size, line, buf);
			
			switch(frag->blender.type)
			{
				case VN_M_BLEND_FADE :
				control = malloc((sizeof *buf) * size * 3);
				samples += render_material(node, frag->blender.control, size, line, control);
				for(i = 0; i < size * 3; i++)
					buffer[i] = buffer[i] * control[i] + buf[i] * (1 - control[i]);
				free(control);
				break;
				case VN_M_BLEND_ADD :
				for(i = 0; i < size * 3; i++)
					buffer[i] += buf[i];
				break;
				case VN_M_BLEND_SUBTRACT :
				for(i = 0; i < size * 3; i++)
					buffer[i] -= buf[i];
				break;
				case VN_M_BLEND_MULTIPLY :
				for(i = 0; i < size * 3; i++)
					buffer[i] *= buf[i];
				break;
				case VN_M_BLEND_DIVIDE :
				for(i = 0; i < size * 3; i++)
					buffer[i] /= buf[i];
				break;
			}
			free(buf);
		}
		break;
		case VN_M_FT_CLAMP :
		{
			render_material(node, frag->clamp.data, size, line, buffer);
			if(frag->clamp.min)
			{
				for(i = 0; i < size * 3; i += 3)
				{
					if(buffer[i] > frag->clamp.red)
						buffer[i] = frag->clamp.red;
					if(buffer[i + 1] > frag->clamp.green)
						buffer[i + 1] = frag->clamp.green;
					if(buffer[i + 2] > frag->clamp.blue)
						buffer[i + 2] = frag->clamp.blue;
				}
			}else
			{
				for(i = 0; i < size * 3; i += 3)
				{
					if(buffer[i] < frag->clamp.red)
						buffer[i] = frag->clamp.red;
					if(buffer[i + 1] < frag->clamp.green)
						buffer[i + 1] = frag->clamp.green;
					if(buffer[i + 2] < frag->clamp.blue)
						buffer[i + 2] = frag->clamp.blue;
				}
			}
			samples = 1;
		}
		break;
		case VN_M_FT_MATRIX :
		{
			float tmp[3], m[16];
			for(i = 0; i < 16; i++)
				m[i] = frag->matrix.matrix[i];
			samples = 1 + render_material(node, frag->matrix.data, size, line, buffer);
			for(i = 0; i < size * 3; i += 3)
			{
				tmp[0] = buffer[i] * m[0] + buffer[i + 1] * m[4] + buffer[i + 2] * m[8] + m[12];
				tmp[1] = buffer[i] * m[1] + buffer[i + 1] * m[5] + buffer[i + 2] * m[9] + m[13];
				tmp[2] = buffer[i] * m[2] + buffer[i + 1] * m[6] + buffer[i + 2] * m[10] + m[14];
				buffer[i] = tmp[0];
				buffer[i + 1] = tmp[1];
				buffer[i + 2] = tmp[2];
			}	
		}
		break;
		case VN_M_FT_RAMP :
		{
			uint j;
			float f;
			samples = 4 + render_material(node, frag->ramp.mapping, size, line, buffer);
			for(i = 0; i < size * 3; i += 3)
			{
				f = buffer[i + frag->ramp.channel];
				if(f < frag->ramp.ramp[0].pos)
				{
					buffer[i] = frag->ramp.ramp[0].red;
					buffer[i + 1] = frag->ramp.ramp[0].green;
					buffer[i + 2] = frag->ramp.ramp[0].blue;
				}else
				{
					for(j = 1; j < frag->ramp.point_count; j++)
					{
						if(f < frag->ramp.ramp[j].pos)
						{
							
							if(frag->ramp.type != VN_M_RAMP_SQUARE)
							{
								f = (f - frag->ramp.ramp[j - 1].pos) / (frag->ramp.ramp[j].pos - frag->ramp.ramp[j - 1].pos);
								if(frag->ramp.type == VN_M_RAMP_SMOOTH)
									f = ((f + 1 - f) * f + f * (1 - f)) * f + f * f * (1 - f);
							}else
								f = 0;

							buffer[i] = frag->ramp.ramp[j].red * f + frag->ramp.ramp[j - 1].red * (1 - f);
							buffer[i + 1] = frag->ramp.ramp[j].green * f + frag->ramp.ramp[j - 1].green * (1 - f);
							buffer[i + 2] = frag->ramp.ramp[j].blue * f + frag->ramp.ramp[j - 1].blue * (1 - f);
							break;
						}
					}
					if(j == frag->ramp.point_count)
					{
						--j;
						buffer[i] = frag->ramp.ramp[j].red;
						buffer[i + 1] = frag->ramp.ramp[j].green;
						buffer[i + 2] = frag->ramp.ramp[j].blue;
					}
				}
			}	
		}
		break;
		case VN_M_FT_ALTERNATIVE :
		{
			uint j, k;
			float *buf;
			buf = malloc((sizeof *buf) * size * 3);
			samples = render_material(node, frag->alternative.alt_a, size, line, buffer);
			samples += render_material(node, frag->alternative.alt_b, size, line, buf);
			for(i = 0; i < line; i++)
			{
				k = i * 3;
				buffer[k] = buf[k];
				k++;
				buffer[k] = buf[k];
				k++;
				buffer[k] = buf[k];
			}
			free(buf);
			break;
		}
		case VN_M_FT_OUTPUT :
			samples = render_material(node, frag->output.front, size, line, buffer);
		break;
	}
	glBindTexture(GL_TEXTURE_2D, co_material_get_texture_id(node, id));
	{
		float *f;
		f = malloc((sizeof *f) * size * 3);
		for(i = 0; i < size * 3; i++)
		{
			if(buffer[i] > 1)
				f[i] = 1;
			else if(buffer[i] < 0)
				f[i] = 0;
			else
				f[i] = buffer[i];
		}
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, line, size, 1, GL_RGB, GL_FLOAT, f);
		free(f);
	}
	e_nsm_leave_fragment(node, id);
	return samples;
}


extern uint co_material_get_version(ENode *node, VNMFragmentID id);
extern void co_material_set_version(ENode *node, VNMFragmentID id, uint version);


uint co_get_fragment_version(ENode *node, uint16 fragment)
{
	uint i, output, *version;
	VMatFrag *frag;
	frag = e_nsm_get_fragment(node, fragment);
	if(frag == NULL)
		return 0;

	output = e_nsm_get_fragment_version(node, fragment);

	if(!e_nsm_enter_fragment(node, fragment))
		return 0;
	switch(e_nsm_get_fragment_type(node, fragment))
	{	
		case VN_M_FT_TEXTURE :
			output += co_get_fragment_version(node, frag->texture.mapping);
			break;
		case VN_M_FT_NOISE :
			output += co_get_fragment_version(node, frag->noise.mapping);
			break;
		case VN_M_FT_BLENDER :
			if(frag->blender.type == VN_M_BLEND_FADE)
				output += co_get_fragment_version(node, frag->blender.control);
			output += co_get_fragment_version(node, frag->blender.data_a);
			output += co_get_fragment_version(node, frag->blender.data_b);
			break;
		case VN_M_FT_CLAMP :
			output += co_get_fragment_version(node, frag->clamp.data);
			break;
		case VN_M_FT_MATRIX :
			output += co_get_fragment_version(node, frag->matrix.data);
			break;
		case VN_M_FT_RAMP :
			output += co_get_fragment_version(node, frag->ramp.mapping);
			break;
		case VN_M_FT_ALTERNATIVE :
			output += co_get_fragment_version(node, frag->alternative.alt_a);
			output += co_get_fragment_version(node, frag->alternative.alt_b);
			break;
		case VN_M_FT_OUTPUT :
			output += co_get_fragment_version(node, frag->output.front);
			break;
	}
	e_nsm_leave_fragment(node, fragment);
	co_material_set_version(node, fragment, output);
	return output;
}

VNMFragmentID co_get_compute_fragment(ENode *node)
{
	VNMFragmentID id;
	for(id = e_nsm_get_fragment_next(node, 0); id != (VNMFragmentID)-1; id = e_nsm_get_fragment_next(node, id + 1))
		if(e_nsm_get_fragment_type(node, id) == VN_M_FT_OUTPUT)
			if(co_material_get_version(node, id) != co_get_fragment_version(node, id))
				return id;

	for(id = e_nsm_get_fragment_next(node, 0); id != (VNMFragmentID)-1; id = e_nsm_get_fragment_next(node, id + 1))
		if(e_nsm_get_fragment_type(node, id) == VN_M_FT_BLENDER)
			if(co_material_get_version(node, id) != co_get_fragment_version(node, id))
				return id;

	for(id = e_nsm_get_fragment_next(node, 0); id != (VNMFragmentID)-1; id = e_nsm_get_fragment_next(node, id + 1))
		if( e_nsm_get_fragment_type(node, id) != VN_M_FT_LIGHT && e_nsm_get_fragment_type(node, id) != VN_M_FT_TRANSPARENCY && e_nsm_get_fragment_type(node, id) != VN_M_FT_REFLECTION)
			if(co_material_get_version(node, id) != co_get_fragment_version(node, id))
				return id;
	for(id = e_nsm_get_fragment_next(node, 0); id != (VNMFragmentID)-1; id = e_nsm_get_fragment_next(node, id + 1))
		if( e_nsm_get_fragment_type(node, id) == VN_M_FT_LIGHT || e_nsm_get_fragment_type(node, id) == VN_M_FT_TRANSPARENCY || e_nsm_get_fragment_type(node, id) == VN_M_FT_REFLECTION)
			if(co_material_get_version(node, id) != co_get_fragment_version(node, id))
				return id;
	return (VNMFragmentID)-1;
}

void co_material_compute(uint lines)
{
	float buf[TEXTURE_RESOLUTION * 3];
	static uint32 node_id = 0;
	static VNMFragmentID frag_id = -1;
	static uint line = 0;
	ENode *node = NULL;
	uint i, j;
	if((node = e_ns_get_node_next(node_id, 0, V_NT_MATERIAL)) == NULL)
	{
		node = e_ns_get_node_next(0, 0, V_NT_MATERIAL);
		line = 0;
		if(node == NULL)
			return;
		node_id = e_ns_get_node_id(node);
	}
	if(e_nsm_get_fragment(node, frag_id) == NULL)
	{
		line = 0;
		frag_id = co_get_compute_fragment(node);
		if(frag_id == (VNMFragmentID)-1)
		{
			node_id = e_ns_get_node_id(node) + 1;
			if((node = e_ns_get_node_next(node_id, 0, V_NT_MATERIAL)) == NULL)
				node_id = 0;
		}
		return;
	}
	for(i = 0; i < 512 && line != TEXTURE_RESOLUTION;)
	{
		j =  render_material(node, frag_id, TEXTURE_RESOLUTION, line++, buf);
		i += j;
	}
	if(line == TEXTURE_RESOLUTION)
	{
		frag_id = co_get_compute_fragment(node);
		line = 0;
	}
}
