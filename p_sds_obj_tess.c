#include <math.h>
#include "enough.h"
#include "p_sds_geo.h"
#include "p_sds_table.h"
#include "p_sds_obj.h"

egreal p_sds_edge_tesselation_global_func(egreal *v_0, egreal *v_1, egreal *e_0, egreal *eay)
{
	egreal length, edge, vector[3];
	length = ((v_0[0] - v_1[0]) * (v_0[0] - v_1[0])) + ((v_0[1] - v_1[1]) * (v_0[1] - v_1[1])) + ((v_0[2] - v_1[2]) * (v_0[2] - v_1[2]));

	vector[0] = (e_0[0] - ((v_0[0] + v_1[0]) * 0.5));
	vector[1] = (e_0[1] - ((v_0[1] + v_1[1]) * 0.5));
	vector[2] = (e_0[2] - ((v_0[2] + v_1[2]) * 0.5));
	edge = (vector[0] * vector[0]) + (vector[1] * vector[1]) + (vector[2] * vector[2]);
	return /*sqrt(*/edge / length/*)*/;
}


egreal p_sds_edge_tesselation_test_func(egreal *v_0, egreal *v_1, egreal *e_0, egreal *eay)
{
	egreal v_a[3], v_b[3], f;

	if(v_0[0] + v_0[1] > v_1[0] + v_1[1])
	{
		v_a[0] = (v_1[0] - v_0[0]);
		v_a[1] = (v_1[1] - v_0[1]);
		v_a[2] = (v_1[2] - v_0[2]);

		f = sqrt(v_a[0] * v_a[0] + v_a[1] * v_a[1] + v_a[2] * v_a[2]);

		v_a[0] /= f; 
		v_a[1] /= f; 
		v_a[2] /= f; 

		v_b[0] = (e_0[0] - v_0[0]);
		v_b[1] = (e_0[1] - v_0[1]);
		v_b[2] = (e_0[2] - v_0[2]);

		f = v_a[0] * v_b[0] + v_a[1] * v_b[1] + v_a[2] * v_b[2];

		v_b[0] += v_a[0] * f;
		v_b[1] += v_a[1] * f;
		v_b[2] += v_a[2] * f;

		return v_b[0] * v_b[0] + v_b[1] * v_b[1] + v_b[2] * v_b[2];
	}else
	{
		v_a[0] = (v_0[0] - v_1[0]);
		v_a[1] = (v_0[1] - v_1[1]);
		v_a[2] = (v_0[2] - v_1[2]);

		f = sqrt(v_a[0] * v_a[0] + v_a[1] * v_a[1] + v_a[2] * v_a[2]);

		v_a[0] /= f; 
		v_a[1] /= f; 
		v_a[2] /= f; 

		v_b[0] = (e_0[0] - v_1[0]);
		v_b[1] = (e_0[1] - v_1[1]);
		v_b[2] = (e_0[2] - v_1[2]);

		f = v_a[0] * v_b[0] + v_a[1] * v_b[1] + v_a[2] * v_b[2];

		v_b[0] += v_a[0] * f;
		v_b[1] += v_a[1] * f;
		v_b[2] += v_a[2] * f;

		return v_b[0] * v_b[0] + v_b[1] * v_b[1] + v_b[2] * v_b[2];
	}
}

egreal p_sds_edge_tesselation_local_func(egreal *v_0, egreal *v_1, egreal *e_0, egreal *eay)
{
	egreal length, edge, dist, vector[3];
	length = ((v_0[0] - v_1[0]) * (v_0[0] - v_1[0])) + ((v_0[1] - v_1[1]) * (v_0[1] - v_1[1])) + ((v_0[2] - v_1[2]) * (v_0[2] - v_1[2]));

	vector[0] = (e_0[0] - ((v_0[0] + v_1[0]) * 0.5));
	vector[1] = (e_0[1] - ((v_0[1] + v_1[1]) * 0.5));
	vector[2] = (e_0[2] - ((v_0[2] + v_1[2]) * 0.5));
	edge = (vector[0] * vector[0]) + (vector[1] * vector[1]) + (vector[2] * vector[2]);
	vector[0] = (e_0[0] + eay[0]);
	vector[1] = (e_0[1] + eay[1]);
	vector[2] = (e_0[2] + eay[2]);
	dist = 1 / ((vector[0] * vector[0]) + (vector[1] * vector[1]) + (vector[2] * vector[2]));
	return sqrt((edge / length) * dist);
}
/*

PTessTableElement	*p_select_tri_tesselation(const PPolyStore *mesh, const egreal *vertex, uint *index, uint polygon_times_size_times_three, egreal tess_factor, uint force_tess, egreal (*edge_tess_func)(egreal *v_0, egreal *v_1, egreal *e_0, egreal *eay), egreal *eay)
{
	PDepend *dep;
	uint i, j, pos, edge[4];
	egreal value, edge_data[3 * 6];
	for(i = 0; i < 6; i++)
	{
		dep = &mesh->vertex_dependency[mesh->reference[index[i] + polygon_times_size_times_three]];
		pos = dep->element[0].vertex_ref * 3;
		edge_data[i * 3] = value * vertex[pos + 0];
		edge_data[i * 3 + 1] = value * vertex[pos + 1];
		edge_data[i * 3 + 2] = value * vertex[pos + 2];
		for(j = 1; j < dep->length; j++)
		{
			value = dep->element[j].value;
			pos = dep->element[j].vertex_ref * 3;
			edge_data[i * 3] += value * vertex[pos + 0];
			edge_data[i * 3 + 1] += value * vertex[pos + 1];
			edge_data[i * 3 + 2] += value * vertex[pos + 2];
		}
	}
		edge[0] = edge_tess_func(&edge_data[0], &edge_data[5], &edge_data[1], eay) * tess_factor;
		edge[1] = edge_tess_func(&edge_data[5], &edge_data[4], &edge_data[3], eay) * tess_factor;
		edge[2] = edge_tess_func(&edge_data[4], &edge_data[0], &edge_data[2], eay) * tess_factor;
	if(edge[0] < force_tess)
		edge[0] = force_tess;
	if(edge[1] < force_tess)
		edge[1] = force_tess;
	if(edge[2] < force_tess)
		edge[2] = force_tess;
	if(edge[0] > mesh->base_level)
		edge[0] = mesh->base_level;
	if(edge[1] > mesh->base_level)
		edge[1] = mesh->base_level;
	if(edge[2] > mesh->base_level)
		edge[2] = mesh->base_level;
	return get_dynamic_table_tri(mesh->base_level, edge);
}

PTessTableElement	*p_select_quad_tesselation(const PPolyStore *mesh, const egreal *vertex, uint *index, uint polygon_times_size_times_four, egreal tess_factor, uint force_tess, egreal (*edge_tess_func)(egreal *v_0, egreal *v_1, egreal *e_0, egreal *eay), egreal *eay)
{
	PDepend *dep;
	uint i, j, pos, edge[4];
	egreal value, edge_data[3 * 9];
//	printf("looking for an edge\n");
	for(i = 0; i < 8; i++)
	{
		dep = &mesh->vertex_depend_temp[mesh->quad_reference[polygon_times_size_times_four + index[i]]];
		value = dep->element[0].value;
		pos = dep->element[0].vertex_ref * 3;
		edge_data[i * 3] = value * vertex[pos + 0];
		edge_data[i * 3 + 1] = value * vertex[pos + 1];
		edge_data[i * 3 + 2] = value * vertex[pos + 2];
//		printf("ref = %u %u %u ", polygon_times_size_times_four, index[i], mesh->quad_reference[polygon_times_size_times_four + index[i]]);
		for(j = 1; j < dep->length; j++)
		{
			value = dep->element[j].value;
			pos = dep->element[j].vertex_ref * 3;
			edge_data[i * 3] += value * vertex[pos + 0];
			edge_data[i * 3 + 1] += value * vertex[pos + 1];
			edge_data[i * 3 + 2] += value * vertex[pos + 2];

		}
//		printf("pos = %i, %f %f %f\n", i, edge_data[i * 3], edge_data[i * 3 + 1], edge_data[i * 3 + 2]);
	}
	edge[0] = edge_tess_func(&edge_data[0 * 3], &edge_data[3 * 3], &edge_data[2 * 3], eay) * tess_factor;
	edge[1] = edge_tess_func(&edge_data[3 * 3], &edge_data[7 * 3], &edge_data[4 * 3], eay) * tess_factor;
	edge[2] = edge_tess_func(&edge_data[7 * 3], &edge_data[5 * 3], &edge_data[6 * 3], eay) * tess_factor;
	edge[3] = edge_tess_func(&edge_data[5 * 3], &edge_data[0 * 3], &edge_data[1 * 3], eay) * tess_factor;

	if(edge[0] < force_tess)
		edge[0] = force_tess;
	if(edge[1] < force_tess)
		edge[1] = force_tess;
	if(edge[2] < force_tess)
		edge[2] = force_tess;
	if(edge[3] < force_tess)
		edge[3] = force_tess;
	if(edge[0] > mesh->base_level)
		edge[0] = mesh->base_level;
	if(edge[1] > mesh->base_level)
		edge[1] = mesh->base_level;
	if(edge[2] > mesh->base_level)
		edge[2] = mesh->base_level;
	if(edge[3] > mesh->base_level)
		edge[3] = mesh->base_level;
	return get_dynamic_table_quad(mesh->base_level, edge);
}
*/



/*
				if(mesh->sub_stages[0] == mesh->render.mat[mesh->sub_stages[1]].tri_end)
						mesh->sub_stages[1]++;
					if(mesh->sub_stages[0] < mesh->render.mat[mesh->sub_stages[1]].quad_end)
						poly = mesh->sub_stages[0] * smesh->poly_per_base * 4;
					else
						poly = smesh->quad_length / 4 + mesh->sub_stages[0] * smesh->poly_per_base * 3;

					for(j = 0; j < table->vertex_count; j++)
					{
						
						dep = &smesh->vertex_dependency[smesh->ref[table->reference[j] + poly]];
						mesh->depend.ref_count[mesh->render.vertex_count++] = dep->length;
						for(k = 0; k < dep->length; k++)
						{
							mesh->depend.reference[mesh->depend.length] = dep->element[k].vertex * 3;
							mesh->depend.weight[mesh->depend.length] = dep->element[k].value;
							mesh->depend.length++;
						}
					}
*/

double view_cam_lod_eye_pos[3];
double view_cam_lod_factor = 0.01;
double view_cam_lod_limit = 1.5;
double view_cam_lod_geometry_only = 1000;

void p_lod_set_wiev_pos(double *view_cam)
{
	view_cam_lod_eye_pos[0] = view_cam[0];
	view_cam_lod_eye_pos[1] = view_cam[1];
	view_cam_lod_eye_pos[2] = view_cam[2];
}

egreal p_lod_compute_lod_level(ENode *o_node, ENode *g_node, uint32 time_s, uint32 time_f)
{
	double tmp[3], f;
	if(o_node == NULL)
		return view_cam_lod_geometry_only;
	e_nso_get_scale(o_node, tmp);
	f = (tmp[0] + tmp[1] + tmp[2]) / 3;

	e_nso_get_pos_time(o_node, tmp, time_s, time_f);
	tmp[0] -= view_cam_lod_eye_pos[0];	
	tmp[1] -= view_cam_lod_eye_pos[1];
	tmp[2] -= view_cam_lod_eye_pos[2];

	f = (f + 1) / sqrt(tmp[0] * tmp[0] * tmp[1] * tmp[1] + tmp[2] * tmp[2]);
	return f;
}

boolean p_lod_compute_lod_update(ENode *o_node, ENode *g_node, uint32 time_s, uint32 time_f, egreal factor)
{
	double f;
	if(o_node == NULL)
		return FALSE;
	f = p_lod_compute_lod_level(o_node, g_node, time_s, time_f);
	if(f * (view_cam_lod_limit + view_cam_lod_limit - 1) < factor || f / view_cam_lod_limit > factor)
		return TRUE;
	else 
		return FALSE;
}


void p_lod_select_tesselation(PMesh *mesh, PPolyStore *smesh, egreal *cvs)
{
	egreal *vertex, *v, *v2, *v3, f, temp[3], limit[7] = {0.01, 0.05, 0.17, 0.5, 1.6, 5, 15}; // limit[6] = {2, 5, 17, 40, 80, 160};
	PDepend *dep;
	uint poly, i, j, stage;
	if(mesh->sub_stages[0] == 0)
	{
		mesh->temp = vertex = malloc((sizeof *vertex) * mesh->anim.cv_count * 3);
		mesh->sub_stages[0]++;
		mesh->sub_stages[1] = 0;
	}
	if(mesh->sub_stages[0] == 1)
	{
		vertex = mesh->temp;
		v = &vertex[mesh->sub_stages[1] * 3];
		for(i = 0; i + mesh->sub_stages[1] < mesh->anim.cv_count; i++)
		{
		//	v = &vertex[(mesh->sub_stages[1] + 1) * 3];
			v[0] = 0;
			v[1] = 0;
			v[2] = 0;
			dep = &smesh->vertex_dependency[mesh->sub_stages[1] + i];
			for(j = 0; j < dep->length; j++)
			{
				f = dep->element[j].value;
				v3 = &cvs[dep->element[j].vertex * 3];
				v[0] += v3[0] * f;
				v[1] += v3[1] * f;
				v[2] += v3[2] * f;
			}
			v += 3;
		}
		mesh->sub_stages[1] += i;
		if(i + mesh->sub_stages[1] == mesh->anim.cv_count)
		{
			mesh->sub_stages[0]++;
			mesh->sub_stages[1] = 0;
			mesh->sub_stages[2] = 0;
		}	
	}
	if(mesh->sub_stages[0] == 2)
	{
		PTessTableElement *table, *tri_table, *quad_table;
		uint level[4] = {1, 1, 1, 1};
		vertex = mesh->temp;
		tri_table = get_dynamic_table_tri(smesh->level, level);
		quad_table = get_dynamic_table_quad(smesh->level, level);
		mesh->render.mat[0].vertex_start = 0;
		for(stage = mesh->sub_stages[1]; stage < mesh->tess.tri_count + mesh->tess.quad_count; stage++)
		{
			if(stage < mesh->tess.quad_count)
			{
				poly = stage * smesh->poly_per_base * 4;
				v2 = &vertex[smesh->ref[poly + quad_table->reference[0]] * 3];
				for(i = 0; i < 4; i++)
				{
					temp[0] = 0;
					temp[1] = 0;
					temp[2] = 0;
					dep = &smesh->vertex_dependency[smesh->ref[poly + quad_table->reference[i * 2 + 1]]];
					for(j = 0; j < dep->length; j++)
					{
						f = dep->element[j].value;
						v3 = &cvs[dep->element[j].vertex * 3];
						temp[0] += v3[0] * f;
						temp[1] += v3[1] * f;
						temp[2] += v3[2] * f;
					}
					v = v2;
					v2 = &vertex[smesh->ref[poly + quad_table->reference[((i + 1) % 4) * 2]] * 3];
					f = p_sds_edge_tesselation_global_func(v, v2, temp, NULL) * mesh->tess.factor;
					for(j = 0; j < smesh->level - 1 && limit[j] < f; j++);
					level[i] = j;
					if(level[i] < mesh->tess.force)
						level[i] = mesh->tess.force;
					if(level[i] > smesh->level)
						level[i] = smesh->level;
				}
				table = get_dynamic_table_quad(smesh->level, level);
				if(smesh->crease[stage * 4] > 0.1)
					mesh->render.element_count += (table->edges[1] - table->edges[0]);
				if(smesh->crease[stage * 4 + 1] > 0.1)
					mesh->render.element_count += (table->edges[2] - table->edges[1]);
				if(smesh->crease[stage * 4 + 2] > 0.1)
					mesh->render.element_count += (table->edges[3] - table->edges[2]);
				if(smesh->crease[stage * 4 + 3] > 0.1)
					mesh->render.element_count += (table->edges[4] - table->edges[3]);
			}else
			{
				poly = smesh->quad_length + (stage - smesh->base_quad_count) * smesh->poly_per_base * 3;
				v2 = &vertex[smesh->ref[poly + tri_table->reference[0]] * 3];
				for(i = 0; i < 3; i++)
				{
					temp[0] = 0;
					temp[1] = 0;
					temp[2] = 0;
					dep = &smesh->vertex_dependency[smesh->ref[poly + tri_table->reference[i * 2 + 1]]];
					for(j = 0; j < dep->length; j++)
					{
						f = dep->element[j].value;
						v3 = &cvs[dep->element[j].vertex * 3];
						temp[0] += v3[0] * f;
						temp[1] += v3[1] * f;
						temp[2] += v3[2] * f;
					}
					v = v2;
					v2 = &vertex[smesh->ref[poly + tri_table->reference[(i + 1) % 3 * 2]] * 3];
					f = p_sds_edge_tesselation_global_func(v, v2, temp, NULL) * mesh->tess.factor;
					for(j = 0; j < smesh->level - 1 && limit[j] < f; j++);
					level[i] = j;
					if(level[i] < mesh->tess.force)
						level[i] = mesh->tess.force;
					if(level[i] > smesh->level)
						level[i] = smesh->level;
				}
				table = get_dynamic_table_tri(smesh->level, level);
				if(smesh->crease[smesh->quad_length * 4 + (stage - smesh->base_quad_count) * 3] > 0.1)
					mesh->render.element_count += (table->edges[1] - table->edges[0]);
				if(smesh->crease[smesh->quad_length * 4 + (stage - smesh->base_quad_count) * 3 + 1] > 0.1)
					mesh->render.element_count += (table->edges[2] - table->edges[1]);
				if(smesh->crease[smesh->quad_length * 4 + (stage - smesh->base_quad_count) * 3 + 2] > 0.1)
					mesh->render.element_count += (table->edges[3] - table->edges[2]);
			}
			for(j = 0; j < table->vertex_count; j++)
				mesh->depend.length += smesh->vertex_dependency[smesh->ref[poly + table->reference[j]]].length;
			mesh->tess.tess[mesh->tess.order_temp_mesh_rev[stage]] = table;
			mesh->render.element_count += table->element_count;
			mesh->render.vertex_count += table->vertex_count;
		}
		mesh->sub_stages[1] = stage;
		if(mesh->sub_stages[1] == mesh->tess.tri_count + mesh->tess.quad_count)
		{
			mesh->stage++;
			mesh->sub_stages[0] = 0;
			mesh->sub_stages[1] = 0;
			mesh->sub_stages[2] = 0;
			mesh->sub_stages[3] = 0;
			free(mesh->temp);
			mesh->temp = NULL;
		}
	}
}




void p_lod_select_tesselation_old(PMesh *mesh, PPolyStore *smesh, egreal *cvs)
{
	egreal *vertex, *v, *v2, *v3, f, temp[3], limit[7] = {0.01, 0.05, 0.17, 0.5, 1.6, 5, 15}; // limit[6] = {2, 5, 17, 40, 80, 160};
	PDepend *dep;
	uint poly, i, j, stage;
	if(mesh->sub_stages[0] == 0)
	{
		mesh->temp = vertex = malloc((sizeof *vertex) * mesh->anim.cv_count * 3);
		mesh->sub_stages[0]++;
		mesh->sub_stages[1] = 0;
	}
	if(mesh->sub_stages[0] == 1)
	{
		vertex = mesh->temp;
		v = &vertex[mesh->sub_stages[1] * 3];
		for(i = 0; i + mesh->sub_stages[1] < mesh->anim.cv_count; i++)
		{
		//	v = &vertex[(mesh->sub_stages[1] + 1) * 3];
			v[0] = 0;
			v[1] = 0;
			v[2] = 0;
			dep = &smesh->vertex_dependency[mesh->sub_stages[1] + i];
			for(j = 0; j < dep->length; j++)
			{
				f = dep->element[j].value;
				v3 = &cvs[dep->element[j].vertex * 3];
				v[0] += v3[0] * f;
				v[1] += v3[1] * f;
				v[2] += v3[2] * f;
			}
			v += 3;
		}
		mesh->sub_stages[1] += i;
		if(i + mesh->sub_stages[1] == mesh->anim.cv_count)
		{
			mesh->sub_stages[0]++;
			mesh->sub_stages[1] = 0;
			mesh->sub_stages[2] = 0;
		}	
	}
	if(mesh->sub_stages[0] == 2)
	{
		PTessTableElement *table, *tri_table, *quad_table;
		uint level[4] = {1, 1, 1, 1};
		vertex = mesh->temp;
		tri_table = get_dynamic_table_tri(smesh->level, level);
		quad_table = get_dynamic_table_quad(smesh->level, level);
		mesh->render.mat[0].vertex_start = 0;
		for(stage = mesh->sub_stages[1]; stage < mesh->tess.tri_count + mesh->tess.quad_count; stage++)
		{
			if(stage == mesh->render.mat[mesh->sub_stages[2]].tri_end)
			{
				if(mesh->render.mat_count > 1)
					printf("new material\n");
				mesh->sub_stages[2]++;
				mesh->render.mat[mesh->sub_stages[2]].vertex_start = mesh->render.vertex_count;
			}
			if(stage < mesh->render.mat[mesh->sub_stages[2]].quad_end)
			{
				if(mesh->render.mat_count > 1)
					printf("*quad %u %u\n", stage, mesh->tess.order_temp_mesh[stage]);
				poly = mesh->tess.order_temp_mesh[stage] * smesh->poly_per_base * 4;
				v2 = &vertex[smesh->ref[poly + quad_table->reference[0]] * 3];
				for(i = 0; i < 4; i++)
				{
					temp[0] = 0;
					temp[1] = 0;
					temp[2] = 0;
					dep = &smesh->vertex_dependency[smesh->ref[poly + quad_table->reference[i * 2 + 1]]];
					for(j = 0; j < dep->length; j++)
					{
						f = dep->element[j].value;
						v3 = &cvs[dep->element[j].vertex * 3];
						temp[0] += v3[0] * f;
						temp[1] += v3[1] * f;
						temp[2] += v3[2] * f;
					}
					v = v2;
					v2 = &vertex[smesh->ref[poly + quad_table->reference[((i + 1) % 4) * 2]] * 3];
					f = p_sds_edge_tesselation_global_func(v, v2, temp, NULL) * mesh->tess.factor;
					for(j = 0; j < smesh->level - 1 && limit[j] < f; j++);
					level[i] = j;
					if(level[i] < mesh->tess.force)
						level[i] = mesh->tess.force;
				}
				table = get_dynamic_table_quad(smesh->level, level);
			}else
			{
				if(mesh->render.mat_count > 1)
					printf("*tri %u %u\n", stage, mesh->tess.order_temp_mesh[stage]);
				poly = smesh->quad_length + (mesh->tess.order_temp_mesh[stage] - smesh->base_quad_count) * smesh->poly_per_base * 3;
				v2 = &vertex[smesh->ref[poly + tri_table->reference[0]] * 3];
				for(i = 0; i < 3; i++)
				{
					temp[0] = 0;
					temp[1] = 0;
					temp[2] = 0;
					dep = &smesh->vertex_dependency[smesh->ref[poly + tri_table->reference[i * 2 + 1]]];
					for(j = 0; j < dep->length; j++)
					{
						f = dep->element[j].value;
						v3 = &cvs[dep->element[j].vertex * 3];
						temp[0] += v3[0] * f;
						temp[1] += v3[1] * f;
						temp[2] += v3[2] * f;
					}
					v = v2;
					v2 = &vertex[smesh->ref[poly + tri_table->reference[(i + 1) % 3 * 2]] * 3];
					f = p_sds_edge_tesselation_global_func(v, v2, temp, NULL) * mesh->tess.factor;
					for(j = 0; j < smesh->level - 1 && limit[j] < f; j++);
					level[i] = j;
					if(level[i] < mesh->tess.force)
						level[i] = mesh->tess.force;
				}
				table = get_dynamic_table_tri(smesh->level, level);
			}
			for(j = 0; j < table->vertex_count; j++)
				mesh->depend.length += smesh->vertex_dependency[smesh->ref[poly + table->reference[j]]].length;
			mesh->tess.tess[stage] = table;
			mesh->render.element_count += table->element_count;
			mesh->render.vertex_count += table->vertex_count;
		}
		mesh->sub_stages[1] = stage;
		if(mesh->sub_stages[1] == mesh->tess.tri_count + mesh->tess.quad_count)
		{
			mesh->stage++;
			mesh->sub_stages[0] = 0;
			mesh->sub_stages[1] = 0;
			mesh->sub_stages[2] = 0;
			mesh->sub_stages[3] = 0;
			free(mesh->temp);
			mesh->temp = NULL;
		}
	}
}

