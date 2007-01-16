#include "la_includes.h"

#include "st_matrix_operations.h"
#include "la_geometry_undo.h"
#include "la_projection.h"


void la_t_poly_compute_normal(double *normal, egreal *vertex, uint *ref)
{
	egreal r, v0[3], v1[3];
	v0[0] = vertex[ref[1] * 3 + 0] - vertex[ref[0] * 3 + 0];
	v0[1] = vertex[ref[1] * 3 + 1] - vertex[ref[0] * 3 + 1];
	v0[2] = vertex[ref[1] * 3 + 2] - vertex[ref[0] * 3 + 2];
	r = sqrt(v0[0] * v0[0] + v0[1] * v0[1] + v0[2] * v0[2]);
	v0[0] /= r;
	v0[1] /= r;
	v0[2] /= r;
	v1[0] = vertex[ref[2] * 3 + 0] - vertex[ref[0] * 3 + 0];
	v1[1] = vertex[ref[2] * 3 + 1] - vertex[ref[0] * 3 + 1];
	v1[2] = vertex[ref[2] * 3 + 2] - vertex[ref[0] * 3 + 2];
	r = sqrt(v1[0] * v1[0] + v1[1] * v1[1] + v1[2] * v1[2]);
	v1[0] /= r;
	v1[1] /= r;
	v1[2] /= r;
	normal[0] = v0[1] * v1[2] - v0[2] * v1[1];
	normal[1] = v0[2] * v1[0] - v0[0] * v1[2];
	normal[2] = v0[0] * v1[1] - v0[1] * v1[0];
	r = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
	normal[0] /= r;
	normal[1] /= r;
	normal[2] /= r;
}

uint la_t_poly_test(BInputState *input, double *mid, double *normal)
{
	uint i, j, j2, poly, found, ref_count, *ref, vertex_count;
	double *vertex, temp[4][3], mouse[2], value;

	udg_get_geometry(&vertex_count, &ref_count, &vertex, &ref, NULL);
	for(i = 0; i < ref_count; i++)
	{
		if(ref[i * 4] < vertex_count && ref[i * 4 + 1] < vertex_count && ref[i * 4 + 2] < vertex_count && vertex[ref[i * 4] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 1] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 2] * 3] != V_REAL64_MAX)
		{
			j = ref[i * 4] * 3;
			p_get_projection_screen(&temp[0][0], vertex[j], vertex[j + 1], vertex[j + 2]);
			j = ref[i * 4 + 1] * 3;
			p_get_projection_screen(&temp[1][0], vertex[j], vertex[j + 1], vertex[j + 2]);
			j = ref[i * 4 + 2] * 3;
			p_get_projection_screen(&temp[2][0], vertex[j], vertex[j + 1], vertex[j + 2]);
			if(ref[i * 4 + 3] < vertex_count && vertex[ref[i * 4 + 3] * 3] != V_REAL64_MAX)
			{
				j = ref[i * 4 + 3] * 3;
				p_get_projection_screen(&temp[3][0], vertex[j], vertex[j + 1], vertex[j + 2]);
				poly = 4;
			}else
				poly = 3;
			found = 0;
			mouse[0] = -input->pointer_x;
			mouse[1] = -input->pointer_y;
			for(j = 0; j < poly; j++)
			{
				j2 = (j + 1) % poly;
				value = (mouse[0] - temp[j][0]) * (temp[j][1] - temp[j2][1]) + (mouse[1] - temp[j][1]) * (temp[j2][0] - temp[j][0]);
				if(value > 0 || temp[j][2] > 0)
					break;
				value = (temp[j][0] - temp[j2][0]) * (temp[j][0] - temp[j2][0]) + (temp[j][1] - temp[j2][1]) * (temp[j][1] - temp[j2][1]);
				if(value < 0.0001)
					break;
			}
			if(j == poly)
			{
				if(mid != NULL)
				{
					mid[0] = 0;
					mid[1] = 0;
					mid[2] = 0;
					for(j = 0; j < poly; j++)
					{
						mid[0] += vertex[ref[i * 4 + j] * 3 + 0] / (double)poly;
						mid[1] += vertex[ref[i * 4 + j] * 3 + 1] / (double)poly;
						mid[2] += vertex[ref[i * 4 + j] * 3 + 2] / (double)poly;
					}
				}
				if(normal != NULL)
					la_t_poly_compute_normal(normal, vertex, &ref[i * 4]);
				return i;
			}
		}
	}
	return -1;
}

void la_t_poly_select(uint polygon)
{
	uint *ref, vertex_count;
	double value = 1;
	udg_get_geometry(&vertex_count, NULL, NULL, &ref, NULL);
	ref += polygon * 4;		
	if(udg_get_select(ref[0]) > 0.5 && 	udg_get_select(ref[1]) > 0.5 &&	udg_get_select(ref[2]) > 0.5 &&	(ref[3] >= vertex_count || udg_get_select(ref[3]) > 0.5))
		value = 0;
	udg_set_select(ref[0], value);
	udg_set_select(ref[1], value);
	udg_set_select(ref[2], value);
	if(ref[3] < vertex_count)
		udg_set_select(ref[3], value);
}

void la_t_smooth_select(void)
{
	uint i, *count, ref_count, *ref, vertex_count;
	double *vertex, *value, temp;
	udg_get_geometry(&vertex_count, &ref_count, &vertex, &ref, NULL);
	ref_count *= 4;
	count = malloc((sizeof *count) * vertex_count);
	value = malloc((sizeof *value) * vertex_count);
	for(i = 0; i < vertex_count; i++)
	{
		count[i] = 1;
		value[i] = 0;
	}
	for(i = 0; i < ref_count; i += 4)
	{
		if(ref[i] < vertex_count)
		{
			temp = udg_get_select(ref[i]) + udg_get_select(ref[i + 1]) + udg_get_select(ref[i + 2]);
			if(ref[i + 3] < vertex_count)
				temp = (temp + udg_get_select(ref[i + 3])) / 4;
			else
				temp = temp / 3;
			count[ref[i]]++;
			value[ref[i]] += temp;
			count[ref[i + 1]]++;
			value[ref[i + 1]] += temp;
			count[ref[i + 2]]++;
			value[ref[i + 2]] += temp;
			if(ref[i + 3] < vertex_count)
			{
				count[ref[i + 3]]++;
				value[ref[i + 3]] += temp;
			}
		}
	}
	temp = 0;
	for(i = 0; i < vertex_count; i++)
	{
		value[i] /= (double)count[i];
		if(temp < value[i])
			temp = value[i];
	}

	for(i = 0; i < vertex_count; i++)
		if(vertex[i * 3] != V_REAL64_MAX)
			if(value[i] / temp < udg_get_select(i) - 0.0001 || value[i] / temp > udg_get_select(i) + 0.0001)
				udg_set_select(i, value[i] / temp);
	free(count);
	free(value);
}
