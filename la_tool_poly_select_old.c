#include "la_includes.h"

#include "st_matrix_operations.h"
#include "la_geometry_undo.h"
#include "la_tool.h"

double *convert_to_poly_selection(uint *ref, uint ref_length, uint vertex_length, uint *edge_size)
{
	uint i, polys = 0, count = 0;
	double *new_select;
	new_select = malloc((sizeof *new_select) * vertex_length);
	for(i = 0; i < vertex_length; i++)
		new_select[i] = 0;
	for(i = 0; i < ref_length; i++)
	{
		if(ref[i * 4] < vertex_length && ref[i * 4 + 1] < vertex_length && ref[i * 4 + 2] < vertex_length)
		{
			if(ref[i * 4 + 3] < vertex_length)
			{
				if(udg_get_select(ref[i * 4]) > 0.01 && udg_get_select(ref[i * 4 + 1]) > 0.01 && udg_get_select(ref[i * 4 + 2]) > 0.01 && udg_get_select(ref[i * 4 + 3]) > 0.01)
				{
					new_select[ref[i * 4]] = udg_get_select(ref[i * 4]);
					new_select[ref[i * 4 + 1]] = udg_get_select(ref[i * 4 + 1]);
					new_select[ref[i * 4 + 2]] = udg_get_select(ref[i * 4 + 2]);
					new_select[ref[i * 4 + 3]] = udg_get_select(ref[i * 4 + 3]);
					polys++;
				}else
				{
					if(udg_get_select(ref[i * 4]) > 0.01)
						count++;
					if(udg_get_select(ref[i * 4 + 1]) > 0.01)
						count++;
					if(udg_get_select(ref[i * 4 + 2]) > 0.01)
						count++;
					if(udg_get_select(ref[i * 4 + 3]) > 0.01)
						count++;
				}
			}else
			{
				if(udg_get_select(ref[i * 4]) > 0.01 && udg_get_select(ref[i * 4 + 1]) > 0.01 && udg_get_select(ref[i * 4 + 2]) > 0.01)
				{
					new_select[ref[i * 4]] = udg_get_select(ref[i * 4]);
					new_select[ref[i * 4 + 1]] = udg_get_select(ref[i * 4 + 1]);
					new_select[ref[i * 4 + 2]] = udg_get_select(ref[i * 4 + 2]);
					polys++;
				}else
				{
					if(udg_get_select(ref[i * 4]) > 0.01)
						count++;
					if(udg_get_select(ref[i * 4 + 1]) > 0.01)
						count++;
					if(udg_get_select(ref[i * 4 + 2]) > 0.01)
						count++;
				}
			}
		}
	}
	if(edge_size != NULL)
		*edge_size = count;
	if(polys != 0)
		return new_select;
	else
		free(new_select);
	return NULL;
}

typedef struct{
	uint	*ref;
	uint	ref_length;
	uint	vertex_length;
	uint32	*crease;
	uint	*pairs;
	uint	pair_count;
	void	(*func)(double *output, uint vertex_id, void *data);
	void	*data;
	double	*select;
}ExtrudeParams;

void  copy_vertex_func(double *output, uint vertex_id, void *data)
{
	udg_get_vertex_pos(output, vertex_id);
}

uint create_vertex_copy(ExtrudeParams *extrude, uint vertex)
{
	uint i;
	double buf[3];
	for(i = 0; i < extrude->pair_count && extrude->pairs[i * 2] != vertex ;i++);
	if(i == extrude->pair_count)
	{
		extrude->pairs[i * 2 + 1] = udg_find_empty_slot_vertex();
		extrude->func(buf, vertex, extrude->data);
		udg_vertex_set(extrude->pairs[i * 2 + 1], NULL, buf[0], buf[1], buf[2]);
		extrude->pairs[i * 2] = vertex;
		extrude->pair_count++;
	}
	return extrude->pairs[i * 2 + 1];
}

void la_t_extrude(uint vertex_count, void (*func)(double *output, uint vertex_id, void *data), void *data)
{
	ExtrudeParams param;
	uint i, j, k, poly, edge_id, change, ref[4];
	double vertex[3];
	param.func = func;
	param.data = data;
	udg_get_geometry(&param.vertex_length, &param.ref_length, NULL, &param.ref, &param.crease);
	param.pair_count = 0;
	param.pairs = malloc((sizeof *param.pairs) * param.vertex_length);
	for(i = 0; i < param.vertex_length; i++)
		param.pairs[i] = 0;
	for(i = 0; i < param.ref_length * 4; i += 4)
	{
		if(param.ref[i] < param.vertex_length)
		{
			if(param.ref[i + 3] < param.vertex_length)
				poly = 4;
			else
				poly = 3;
			for(j = 0; j < poly; j++)
			{
				param.pairs[param.ref[i + j]] += param.ref[i + (j + 1) % poly];
				param.pairs[param.ref[i + (j + 1) % poly]] -= param.ref[i + j];
			}
		}
	}
	for(i = 0; i < param.vertex_length; i++)
		if(param.pairs[i] != 0)
			param.pairs[i] = -1;
			
	for(i = 0; i < param.ref_length * 4; i += 4)
	{
		if(param.ref[i] < param.vertex_length)
		{
			if(param.ref[i + 3] < param.vertex_length)
				poly = 4;
			else
				poly = 3;
			for(j = 0; j < poly; j++)
			{
				if(param.pairs[param.ref[i + j]] != 0 && udg_get_select(param.ref[i + j]) > 0.01 && param.pairs[param.ref[i + (j + 1) % poly]] != 0 && udg_get_select(param.ref[i + (j + 1) % poly]) > 0.01)
				{
					for(k = 0; k < 2; k++)
					{
						if(param.pairs[param.ref[i + (j + k) % poly]] == -1)
						{
							param.pairs[param.ref[i + (j + k) % poly]] = edge_id = udg_find_empty_slot_vertex();
							func(vertex, param.ref[i + (j + k) % poly], data);
							udg_vertex_set(param.pairs[param.ref[i + (j + k) % poly]], NULL, vertex[0], vertex[1], vertex[2]);
						}
					}
					udg_polygon_set(udg_find_empty_slot_polygon(), param.ref[i + (j + 1) % poly], param.ref[i + j], param.pairs[param.ref[i + j]], param.pairs[param.ref[i + (j + 1) % poly]]);
				}
			}
		}
	}			
/*	for(i = 0; i < param.ref_length * 4; i += 4)
	{
		if(param.ref[i] < param.vertex_length)
		{
			if(param.ref[i + 3] < param.vertex_length)
				poly = 4;
			else
				poly = 3;
			for(j = 0; j < poly && udg_get_select(param.ref[i + j]) > 0.01; j++);
			if(j != poly)
			{
				change = FALSE;
				for(j = 0; j < poly; j++)
				{
					if(udg_get_select(param.ref[i + j]) > 0.01 && udg_get_select(param.ref[i + (j + 1) % poly]) > 0.01)
					{
						if(param.pairs[param.ref[i + j]] > edge_id || param.pairs[param.ref[i + j]] == 0 || param.pairs[param.ref[i + (j + 1) % poly]] > edge_id || param.pairs[param.ref[i + (j + 1) % poly]] == 0)
						{
							for(k = 0; k < 2; k++)
							{
								if(param.pairs[param.ref[i + (j + k) % poly]] == -1 || param.pairs[param.ref[i + (j + k) % poly]] == 0)
								{
									param.pairs[param.ref[i + (j + k) % poly]] = udg_find_empty_slot_vertex();
									func(vertex, param.ref[i + (j + k) % poly], data);
									udg_vertex_set(param.pairs[param.ref[i + (j + k) % poly]], NULL, vertex[0], vertex[1], vertex[2]);
								}
							}
							udg_polygon_set(udg_find_empty_slot_polygon(), param.ref[i + j], param.ref[i + (j + 1) % poly], param.pairs[param.ref[i + (j + 1) % poly]], param.pairs[param.ref[i + j]]);
						}
					}
				}
			}
		}	
	}*/
	for(i = 0; i < param.ref_length * 4; i += 4)
	{
		if(param.ref[i] < param.vertex_length)
		{
			if(param.ref[i + 3] < param.vertex_length)
				poly = 4;
			else
				poly = 3;
			ref[3] = -1;
			for(j = 0; j < poly && udg_get_select(param.ref[i + j]) > 0.01; j++);
			if(j < poly)
			{
				for(j = 0; j < poly; j++)
				{
					if(param.pairs[param.ref[i + j]] != -1 && param.pairs[param.ref[i + j]] != 0)
					{
						ref[j] = param.pairs[param.ref[i + j]];
						change = TRUE;
					}
					else
						ref[j] = param.ref[i + j];
				}
				if(change)
					udg_polygon_set(i / 4, ref[0], ref[1], ref[2], ref[3]);
			}
		}
	}
	free(param.pairs);
}

void detach_tri(ExtrudeParams *extrude, uint *ref, uint ref_id, uint *crease)
{
	uint new_tri[4], create;
	if(extrude->select[ref[0]] > 0.01 && extrude->select[ref[1]] > 0.01 && extrude->select[ref[2]] > 0.01)
		return;
	if(extrude->select[ref[0]] < 0.01 && extrude->select[ref[1]] < 0.01 && extrude->select[ref[2]] < 0.01)
		return;
	new_tri[0] = ref[0];
	new_tri[1] = ref[1];
	new_tri[2] = ref[2];
	if(extrude->select[ref[0]] > 0.01)
		new_tri[0] = create_vertex_copy(extrude, new_tri[0]);
	if(extrude->select[ref[1]] > 0.01)
		new_tri[1] = create_vertex_copy(extrude, new_tri[1]);
	if(extrude->select[ref[2]] > 0.01)
		new_tri[2] = create_vertex_copy(extrude, new_tri[2]);
	udg_polygon_set(ref_id, new_tri[0], new_tri[1], new_tri[2], -1);
}

void detach_quad(ExtrudeParams *extrude, uint *ref, uint ref_id, uint *crease)
{
	uint new_quad[4], create;

	if(extrude->select[ref[0]] > 0.01 && extrude->select[ref[1]] > 0.01 && extrude->select[ref[2]] > 0.01 && extrude->select[ref[3]] > 0.01)
		return;
	if(extrude->select[ref[0]] < 0.01 && extrude->select[ref[1]] < 0.01 && extrude->select[ref[2]] < 0.01 && extrude->select[ref[3]] < 0.01)
		return;
	new_quad[0] = ref[0];
	new_quad[1] = ref[1];
	new_quad[2] = ref[2];
	new_quad[3] = ref[3];
	if(extrude->select[ref[0]] > 0.01)
		new_quad[0] = create_vertex_copy(extrude, new_quad[0]);
	if(extrude->select[ref[1]] > 0.01)
		new_quad[1] = create_vertex_copy(extrude, new_quad[1]);
	if(extrude->select[ref[2]] > 0.01)
		new_quad[2] = create_vertex_copy(extrude, new_quad[2]);
	if(extrude->select[ref[3]] > 0.01)
		new_quad[3] = create_vertex_copy(extrude, new_quad[3]);
	udg_polygon_set(ref_id, new_quad[0], new_quad[1], new_quad[2], new_quad[3]);
}


void la_t_detach_selected_polygons()
{
	ExtrudeParams param;
	double matrix[16];
	uint i;
	param.func = copy_vertex_func;
	udg_get_geometry(&param.vertex_length, &param.ref_length, NULL, &param.ref, &param.crease);
	param.pair_count = 0;
	param.select = convert_to_poly_selection(param.ref, param.ref_length, param.vertex_length, &i);
	if(param.select == NULL)
		return;
	param.pairs = malloc((sizeof *param.pairs) * i * 2);
	for(i = 0; i < param.ref_length * 4; i += 4)
	{
		if(param.ref[i] < param.vertex_length && param.ref[i + 1] < param.vertex_length && param.ref[i + 2] < param.vertex_length)
		{
			if(param.ref[i + 3] > param.vertex_length)
				detach_tri(&param, &param.ref[i], i / 4, &param.crease[i]);
			else
				detach_quad(&param, &param.ref[i], i / 4, &param.crease[i]);
		}
	}
//	set_extrude_selection(&param);
	free(param.select);
	free(param.pairs);
	return;
}

/*
	if(extrude->select[ref[0]] > 0.01 && extrude->select[ref[1]] > 0.01 && extrude->select[ref[2]] > 0.01 && (ref[3] > extrude->vertex_length || extrude->select[ref[3]] > 0.01))
	{
		new_poly[0] = create_vertex_copy(extrude, ref[0]);
		new_poly[1] = create_vertex_copy(extrude, ref[1]);
		new_poly[2] = create_vertex_copy(extrude, ref[2]);
		if(ref[3] > extrude->vertex_length)
			new_poly[3] = -1;
		else
			new_poly[3] = create_vertex_copy(extrude, ref[3]);
		create = udg_find_empty_slot_polygon();
		udg_polygon_set(create, new_poly[0], new_poly[1], new_poly[2], new_poly[3]);
		udg_crease_set(create, crease[0], crease[1], crease[2], crease[3]);
	}*/

void duplicate_polygon(ExtrudeParams *extrude, uint *ref, uint ref_id, uint *crease)
{
	uint new_poly[4], create;

	if(extrude->select[ref[0]] < 0.01 && extrude->select[ref[1]] < 0.01 && extrude->select[ref[2]] < 0.01 && extrude->select[ref[3]] < 0.01 && (ref[3] > extrude->vertex_length || extrude->select[ref[3]] < 0.01))
		return;
	
	if(extrude->select[ref[0]] > 0.01 && extrude->select[ref[1]] > 0.01 && extrude->select[ref[2]] > 0.01 && (ref[3] > extrude->vertex_length || extrude->select[ref[3]] > 0.01))
		create = udg_find_empty_slot_polygon();
	else
		create = ref_id;
	
	new_poly[0] = ref[0];
	new_poly[1] = ref[1];
	new_poly[2] = ref[2];
	new_poly[3] = ref[3];
	if(extrude->select[ref[0]] > 0.01)
		new_poly[0] = create_vertex_copy(extrude, new_poly[0]);
	if(extrude->select[ref[1]] > 0.01)
		new_poly[1] = create_vertex_copy(extrude, new_poly[1]);
	if(extrude->select[ref[2]] > 0.01)
		new_poly[2] = create_vertex_copy(extrude, new_poly[2]);
	if(extrude->select[ref[3]] > 0.01 && ref[3] < extrude->vertex_length)
		new_poly[3] = create_vertex_copy(extrude, new_poly[3]);
	udg_polygon_set(create, new_poly[0], new_poly[1], new_poly[2], new_poly[3]);

}

void la_t_duplicate_selected_polygons()
{
	ExtrudeParams param;
	double matrix[16], select;
	uint i;
	param.func = copy_vertex_func;
	udg_get_geometry(&param.vertex_length, &param.ref_length, NULL, &param.ref, &param.crease);
	param.pair_count = 0;
	param.select = convert_to_poly_selection(param.ref, param.ref_length, param.vertex_length, NULL);
	if(param.select == NULL)
		return;
	param.pairs = malloc((sizeof *param.pairs) * param.vertex_length * 2);
	for(i = 0; i < param.ref_length * 4; i += 4)
		if(param.ref[i] < param.vertex_length && param.ref[i + 1] < param.vertex_length && param.ref[i + 2] < param.vertex_length)
			duplicate_polygon(&param, &param.ref[i], i / 4, &param.crease[i]);
//	set_extrude_selection(&param);
	free(param.select);
	free(param.pairs);
}

void la_t_flip_selected_polygons()
{
	double *select;
	uint i, vertex_length, ref_length, *ref, *crease;

	udg_get_geometry(&vertex_length, &ref_length, NULL, &ref, &crease);
	select = convert_to_poly_selection(ref, ref_length, vertex_length, NULL);
	ref_length *= 4;
	for(i = 0; i < ref_length; i += 4)
	{
		if(ref[i] < vertex_length && (select[ref[i]] > 0.01 || select[ref[i + 1]] > 0.01 || select[ref[i + 2]] > 0.01 || (ref[i + 3] < vertex_length && select[ref[i + 3]] > 0.01)))
		{
			if(select[ref[i]] > 0.01 && select[ref[i + 1]] > 0.01 && select[ref[i + 2]] > 0.01 && (ref[i + 3] > vertex_length || select[ref[i + 3]] > 0.01))
				udg_polygon_set(i / 4, ref[i + 2], ref[i + 1], ref[i], ref[i + 3]);
			else
				udg_polygon_delete(i / 4);
		}
	}

	free(select);
}

void la_t_mirror_axis(double *origo, double *vector)
{
	double vertex[3], r;
	uint i, vertex_length;
	udg_get_geometry(&vertex_length, NULL, NULL, NULL, NULL);

	for(i = 0; i < vertex_length; i++)
	{
		if(udg_get_select(i) > 0.01)
		{
			udg_get_vertex_pos(vertex, i);
			if(vertex[0] != V_REAL64_MAX)
			{
				r = vector[0] * (vertex[0] - origo[0]) + vector[1] * (vertex[1] - origo[1]) + vector[2] * (vertex[2] - origo[2]);
				udg_vertex_set(i, vertex, vertex[0] - r * vector[0] * 2, vertex[1] - r * vector[1] * 2, vertex[2] - r * vector[2] * 2);
			}
		}
	}
}

void la_t_mirror_selection(uint v0, uint v1, uint v2)
{
	double a[3], b[3], origo[3], vector[3], r;
	udg_get_vertex_pos(a, v0);
	udg_get_vertex_pos(origo, v1);
	udg_get_vertex_pos(b, v2);
	a[0] -= origo[0];
	a[1] -= origo[1];
	a[2] -= origo[2];
	r = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
	a[0] = a[0] / r;
	a[1] = a[1] / r;
	a[2] = a[2] / r;
	b[0] -= origo[0];
	b[1] -= origo[1];
	b[2] -= origo[2];
	r = sqrt(b[0] * b[0] + b[1] * b[1] + b[2] * b[2]);
	b[0] = b[0] / r;
	b[1] = b[1] / r;
	b[2] = b[2] / r;
	vector[0] = (a[1] * b[2] - a[2] * b[1]);
	vector[1] = (a[2] * b[0] - a[0] * b[2]);
	vector[2] = (a[0] * b[1] - a[1] * b[0]);
	r = sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
	vector[0] = vector[0] / r;
	vector[1] = vector[1] / r;
	vector[2] = vector[2] / r;
	la_t_mirror_axis(origo, vector);
}

void test_for_better_normal(uint base_id, uint selected_id, double *normal, double *vector)
{
	double a[3], b[3], r;
	udg_get_vertex_pos(a, selected_id);
	udg_get_vertex_pos(b, base_id);
	b[0] -= a[0];
	b[1] -= a[1];
	b[2] -= a[2];
	if(b[0] * vector[0] + b[1] * vector[1] + b[2] * vector[2] > 0)
		return;
	r = sqrt(b[0] * b[0] + b[1] * b[1] + b[2] * b[2]);
	b[0] = b[0] / r;
	b[1] = b[1] / r;
	b[2] = b[2] / r;
	r = b[0] * vector[0] + b[1] * vector[1] + b[2] * vector[2];
	if(r > -0.4)
		return;
	if(r < normal[0] * vector[0] + normal[1] * vector[1] + normal[2] * vector[2])
		return;
	normal[0] = b[0];
	normal[1] = b[1];
	normal[2] = b[2];
}


void la_t_flatten_selection_normal(double *origo, double *vector)
{
	double a[3], b[3], r, *normal;
	uint i, vertex_length, ref_length, *ref;
	udg_get_geometry(&vertex_length, &ref_length, NULL, &ref, NULL);
	normal = malloc((sizeof *normal) * vertex_length * 3);
	for(i = 0; i < vertex_length; i++)
	{
		normal[i * 3] = -vector[0];
		normal[i * 3 + 1] = -vector[1];
		normal[i * 3 + 2] = -vector[2];
	}
	ref_length *= 4;
	for(i = 0; i < ref_length; i += 4)
	{
		if(ref[i] < vertex_length)
		{
			if(ref[i + 0] < vertex_length && ref[i + 1] < vertex_length && udg_get_select(ref[i + 0]) < 0.01 && udg_get_select(ref[i + 1]) > 0.01)
				test_for_better_normal(ref[i + 0], ref[i + 1], &normal[ref[i + 1] * 3], vector);
			if(ref[i + 1] < vertex_length && ref[i + 2] < vertex_length && udg_get_select(ref[i + 1]) < 0.01 && udg_get_select(ref[i + 2]) > 0.01)
				test_for_better_normal(ref[i + 1], ref[i + 2], &normal[ref[i + 2] * 3], vector);
			if(ref[i + 2] < vertex_length && ref[i + 3] < vertex_length && udg_get_select(ref[i + 2]) < 0.01 && udg_get_select(ref[i + 3]) > 0.01)
				test_for_better_normal(ref[i + 2], ref[i + 3], &normal[ref[i + 3] * 3], vector);
			if(ref[i + 3] < vertex_length && ref[i + 0] < vertex_length && udg_get_select(ref[i + 3]) < 0.01 && udg_get_select(ref[i + 0]) > 0.01)
				test_for_better_normal(ref[i + 3], ref[i + 0], &normal[ref[i + 0] * 3], vector);
			if(ref[i + 3] >= vertex_length && ref[i + 2] < vertex_length && ref[i + 0] < vertex_length && udg_get_select(ref[i + 2]) < 0.01 && udg_get_select(ref[i + 3]) > 0.01)
				test_for_better_normal(ref[i + 2], ref[i + 0], &normal[ref[i + 0] * 3], vector);
		}
	}
		
	for(i = 0; i < vertex_length; i++)
	{
		if(udg_get_select(i) > 0.01)
		{
			udg_get_vertex_pos(a, i);
			if(a[0] != V_REAL64_MAX)
			{
				r = normal[i * 3] * vector[0] + normal[i * 3 + 1] * vector[1] + normal[i * 3 + 2] * vector[2];
				r = (vector[0] * (a[0] - origo[0]) + vector[1] * (a[1] - origo[1]) + vector[2] * (a[2] - origo[2])) / r;
				udg_vertex_set(i, a, a[0] - r * normal[i * 3] * 1, a[1] - r * normal[i * 3 + 1] * 1, a[2] - r * normal[i * 3 + 2] * 1);
			}
		}
	}
	free(normal);
}


void la_t_flatten_selection_face(uint v0, uint v1, uint v2)
{
	double a[3], b[3], origo[3], vector[3], r, *normal;

	udg_get_vertex_pos(a, v0);
	udg_get_vertex_pos(origo, v1);
	udg_get_vertex_pos(b, v2);
	a[0] -= origo[0];
	a[1] -= origo[1];
	a[2] -= origo[2];
	r = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
	a[0] = a[0] / r;
	a[1] = a[1] / r;
	a[2] = a[2] / r;
	b[0] -= origo[0];
	b[1] -= origo[1];
	b[2] -= origo[2];
	r = sqrt(b[0] * b[0] + b[1] * b[1] + b[2] * b[2]);
	b[0] = b[0] / r;
	b[1] = b[1] / r;
	b[2] = b[2] / r;
	vector[0] = (a[1] * b[2] - a[2] * b[1]);
	vector[1] = (a[2] * b[0] - a[0] * b[2]);
	vector[2] = (a[0] * b[1] - a[1] * b[0]);
	r = sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
	vector[0] = vector[0] / r;
	vector[1] = vector[1] / r;
	vector[2] = vector[2] / r;
	la_t_flatten_selection_normal(origo, vector);
}

void la_t_delete_selection(void)
{
	uint i, vertex_length, ref_length, *ref;
	udg_get_geometry(&vertex_length, &ref_length, NULL, &ref, NULL);
	ref_length *= 4;
	for(i = 0; i < ref_length; i++)
		if(ref[i] < vertex_length && udg_get_select(ref[i]) > 0.01 && udg_get_select(ref[i + 1]) > 0.01 && udg_get_select(ref[i + 2]) > 0.01 && (ref[i + 3] > vertex_length || udg_get_select(ref[i + 3]) > 0.01))
			udg_polygon_delete(i / 4);
}

/* Old Extrude


void extrude_tri(ExtrudeParams *extrude, uint *ref, uint ref_id, uint *crease)
{
	uint new_tri[4], create;
	if(extrude->select[ref[0]] > 0.01 && extrude->select[ref[1]] > 0.01 && extrude->select[ref[2]] > 0.01)
		return;
	if(extrude->select[ref[0]] < 0.01 && extrude->select[ref[1]] < 0.01 && extrude->select[ref[2]] < 0.01)
		return;
	new_tri[0] = ref[0];
	new_tri[1] = ref[1];
	new_tri[2] = ref[2];
	if(extrude->select[ref[0]] > 0.01)
		new_tri[0] = create_vertex_copy(extrude, new_tri[0]);
	if(extrude->select[ref[1]] > 0.01)
		new_tri[1] = create_vertex_copy(extrude, new_tri[1]);
	if(extrude->select[ref[2]] > 0.01)
		new_tri[2] = create_vertex_copy(extrude, new_tri[2]);

	if(ref[0] != new_tri[0] && ref[1] != new_tri[1])
	{
		create = udg_find_empty_slot_polygon();
		udg_polygon_set(create, ref[0], ref[1], new_tri[1], new_tri[0]);
		udg_crease_set(create, crease[0], 0, crease[0], 0);
	}
	if(ref[1] != new_tri[1] && ref[2] != new_tri[2])
	{
		create = udg_find_empty_slot_polygon();
		udg_polygon_set(create, ref[1], ref[2], new_tri[2], new_tri[1]);
		udg_crease_set(create, crease[1], 0, crease[1], 0);
	}
	if(ref[2] != new_tri[2] && ref[0] != new_tri[0])
	{
		create = udg_find_empty_slot_polygon();
		udg_polygon_set(create, ref[2], ref[0], new_tri[0], new_tri[2]);
		udg_crease_set(create, crease[2], 0, crease[2], 0);
	}

	udg_polygon_set(ref_id, new_tri[0], new_tri[1], new_tri[2], -1);
}

void extrude_quad(ExtrudeParams *extrude, uint *ref, uint ref_id, uint *crease)
{
	uint new_quad[4], create;

	printf("quad select = %f %f %f %f\n", extrude->select[ref[0]], extrude->select[ref[1]], extrude->select[ref[2]], extrude->select[ref[3]]);
	if(extrude->select[ref[0]] > 0.01 && extrude->select[ref[1]] > 0.01 && extrude->select[ref[2]] > 0.01 && extrude->select[ref[3]] > 0.01)
		return;
	if(extrude->select[ref[0]] < 0.01 && extrude->select[ref[1]] < 0.01 && extrude->select[ref[2]] < 0.01 && extrude->select[ref[3]] < 0.01)
		return;
	new_quad[0] = ref[0];
	new_quad[1] = ref[1];
	new_quad[2] = ref[2];
	new_quad[3] = ref[3];
	if(extrude->select[ref[0]] > 0.01)
		new_quad[0] = create_vertex_copy(extrude, new_quad[0]);
	if(extrude->select[ref[1]] > 0.01)
		new_quad[1] = create_vertex_copy(extrude, new_quad[1]);
	if(extrude->select[ref[2]] > 0.01)
		new_quad[2] = create_vertex_copy(extrude, new_quad[2]);
	if(extrude->select[ref[3]] > 0.01)
		new_quad[3] = create_vertex_copy(extrude, new_quad[3]);
	
	printf("quad select = %i -> %i %i -> %i %i -> %i %i -> %i\n", new_quad[0], ref[0],	new_quad[1], ref[1], new_quad[2], ref[2], new_quad[3], ref[3]);

	if(ref[0] != new_quad[0] && ref[1] != new_quad[1])
	{
		create = udg_find_empty_slot_polygon();
		udg_polygon_set(create, ref[0], ref[1], new_quad[1], new_quad[0]);
		udg_crease_set(create, crease[0], 0, crease[0], 0);
	}
	if(ref[1] != new_quad[1] && ref[2] != new_quad[2])
	{
		create = udg_find_empty_slot_polygon();
		udg_polygon_set(create, ref[1], ref[2], new_quad[2], new_quad[1]);
		udg_crease_set(create, crease[1], 0, crease[1], 0);
	}
	if(ref[2] != new_quad[2] && ref[3] != new_quad[3])
	{
		create = udg_find_empty_slot_polygon();
		udg_polygon_set(create, ref[2], ref[3], new_quad[3], new_quad[2]);
		udg_crease_set(create, crease[2], 0, crease[2], 0);
	}
	if(ref[3] != new_quad[3] && ref[0] != new_quad[0])
	{
		create = udg_find_empty_slot_polygon();
		udg_polygon_set(create, ref[3], ref[0], new_quad[0], new_quad[3]);
		udg_crease_set(create, crease[3], 0, crease[3], 0);
	}
	udg_polygon_set(ref_id, new_quad[0], new_quad[1], new_quad[2], new_quad[3]);
}


void la_t_extrude(uint vertex_count, void (*func)(double *output, uint vertex_id, void *data), void *data)
{
	ExtrudeParams param;
	uint i;
	param.func = func;
	param.data = data;
	udg_get_geometry(&param.vertex_length, &param.ref_length, NULL, &param.ref, &param.crease);
	param.pair_count = 0;
	param.select = convert_to_poly_selection(param.ref, param.ref_length, param.vertex_length, &i);
	if(param.select == NULL)
		return;
	param.pairs = malloc((sizeof *param.pairs) * i * 2);

	for(i = 0; i < param.ref_length * 4; i += 4)
	{
		if(param.ref[i] < param.vertex_length && param.ref[i + 1] < param.vertex_length && param.ref[i + 2] < param.vertex_length)
		{
			if(param.ref[i + 3] > param.vertex_length)
				extrude_tri(&param, &param.ref[i], i / 4, &param.crease[i]);
			else
				extrude_quad(&param, &param.ref[i], i / 4, &param.crease[i]);
		}
	}
	set_extrude_selection(&param);
	free(param.select);
	free(param.pairs);
	return;
}

*/