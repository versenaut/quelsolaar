
#include "verse.h"
#include "enough.h"

#include "seduce.h"
#include "p_sds_geo.h"
extern double get_rand(uint32 index);


void p_sds_add_depend(PDepend *dep, PDepend *add, egreal mult)
{
	uint i, j, k;
	float f;
	if(mult < 0.001)
		return;
	for(i = 0; i < add->length; i++)
	{
		for(j = 0; j < dep->length && dep->element[j].vertex != add->element[i].vertex && dep->element[j].vertex != -1; j++);
		if(j == dep->length)
		{
			dep->length += 32; // FIX ME should be 8
			dep->element = realloc(dep->element, (sizeof *dep->element) * dep->length);
			for(k = j; k < dep->length; k++)
			{
				dep->element[k].value = 0;
				dep->element[k].vertex = -1;
			}
		}
		f = (add->element[i].value / add->sum) * mult;
		dep->element[j].value += f;
		dep->element[j].vertex = add->element[i].vertex;
		dep->sum += f;
	}
}

void p_sds_print_depend(PDepend *dep, char *text)
{
	uint i;
	printf("PRINT DEP: %s \ndep->length = %u sum = %f\n", text, dep->length, dep->sum);
	for(i = 0; i < dep->length; i++)
		printf("X    ref = %u value = %f\n", dep->element[i].vertex, dep->element[i].value);
}

PDepend *p_sds_allocate_depend_first(uint length)
{
	PDependElement *e;
	PDepend *d;
	uint i;
	length++;
	e = malloc((sizeof *e) * length);
	d = malloc((sizeof *d) * length);
	for(i = 0; i < length; i++)
	{
		d[i].sum = 1;
		d[i].length = 1;
		d[i].element = &e[i];
		e[i].value = 1;
		e[i].vertex = i;
	}
	d[length - 1].length = (unsigned short)~0;
	return d;
}

PDepend *p_sds_allocate_depend(uint length)
{
	PDepend *d;
	uint i;
	length++;
	d = malloc((sizeof *d) * length);
	for(i = 0; i < length; i++)
	{
		d[i].length = 0;
		d[i].sum = 0;
		d[i].element = NULL;
	}
	return d;
}


void p_sds_free_depend(PDepend *dep, uint length)
{
	uint i;
	if(dep[length].length != (unsigned short) ~0)
	{
		for(i = 0; i < length; i++)
			if(dep[i].element != NULL);
				free(dep[i].element);
	}else
		free(dep[0].element);
	free(dep);
}
/*
typedef struct{
	uint *ref;
	uint *neighbor;
	uint *crease;
	uint tri_length;
	uint quad_length;			
	uint *base_neighbor;	
	uint base_tri_count;
	uint base_quad_count;
	uint poly_per_base;
	uint open_edges;
	PDepend *vertex_dependency;
	uint vertex_count;
	uint geometry_version;
	void *next;	
	uint level;
	uint stage[2];
}PPolyStore;
*/

float p_sds_compute_neighbor(PPolyStore *poly)
{
	uint i, j, temp, count, cor, other_count, clear = 0, *n, *v, a, b, *ref;
	uint counter = 0, laps = 0;
	ref = poly->ref;
	n = malloc((sizeof *n) * (poly->quad_length + poly->tri_length));
	for(i = 0; i < (poly->quad_length + poly->tri_length); i++)
		n[i] = -1;
	v = malloc((sizeof *v) * poly->vertex_count);
	for(i = 0; i < poly->vertex_count; i++)
		v[i] = -1;
	while(clear < poly->quad_length + poly->tri_length)
	{
		for(i = 0; i < poly->quad_length && clear < poly->quad_length + poly->tri_length; i++)
		{
			counter++;
			cor = v[ref[i]];
			if(cor == -1)
			{
				if(n[i] == -1 || n[(i / 4) * 4 + (i + 3) % 4] == -1)
					v[ref[i]] = i;
		//		else
		//			printf("jump!");
			}
			else if(cor == i)
				v[ref[i]] = -1;
			else
			{
				if(cor >= poly->quad_length)
				{	/* other poly is a tri */
					a = (i / 4) * 4;
					b = poly->quad_length + ((cor - poly->quad_length) / 3) * 3;
					if((n[cor] == -1 || n[a + (i + 3) % 4] == -1) && ref[a + (i + 3) % 4] == ref[b + (cor - b + 1) % 3])
					{
						n[a + (i + 3) % 4] = cor;
						n[cor] = a + (i + 3) % 4;
//						printf("i = %u clear = %u\n", i, clear); 
						clear = 0;
						if(n[b + (cor - b + 2) % 3] != -1)
						{
							if(n[i] == -1)
								v[ref[i]] = i;
							else
								v[ref[i]] = -1;
						}
					}
					if((n[i] == -1 || n[b + (cor - b + 2) % 3] == -1) && ref[a + (i + 1) % 4] == ref[b + (cor - b + 2) % 3])
					{
						n[i] = b + (cor - b + 2) % 3;						
						n[b + (cor - b + 2) % 3] = i;
//						printf("i = %u clear = %u\n", i, clear); 
						clear = 0;
						if(n[cor] != -1)
						{
							if(n[a + (i + 3) % 4] == -1)
								v[ref[i]] = i;
							else
								v[ref[i]] = -1;
						}
//						v[ref[i]] = -1;
					}
				}else
				{	
					/* other poly is a quad */
					a = (i / 4) * 4;
					b = (cor / 4) * 4;
					if((n[cor] == -1 || n[a + (i + 3) % 4] == -1) && ref[a + (i + 3) % 4] == ref[b + (cor + 1) % 4])
					{
						n[a + (i + 3) % 4] = cor;
						n[cor] = a + (i + 3) % 4;
//						printf("i = %u clear = %u\n", i, clear); 
						clear = 0;	
//						v[ref[i]] = -1;
						if(n[b + (cor + 3) % 4] != -1)
						{
							if(n[i] == -1)
								v[ref[i]] = i;
							else
								v[ref[i]] = -1;
						}
					}
					if((n[i] == -1 || n[b + (cor + 3) % 4] == -1) && ref[a + (i + 1) % 4] == ref[b + (cor + 3) % 4])
					{
						n[i] = b + (cor + 3) % 4;
						n[b + (cor + 3) % 4] = i;
//						printf("i = %u clear = %u\n", i, clear); 
						clear = 0;	
						if(n[cor] != -1)
						{
							if(n[a + (i + 3) % 4] == -1)
								v[ref[i]] = i;
							else
								v[ref[i]] = -1;
						}
					}
				}						
			}
			clear++;
		}
		for(; i < poly->quad_length + poly->tri_length && clear < poly->quad_length + poly->tri_length; i++)
		{
			cor = v[ref[i]];
			if(cor == -1)
			{
			//	if(ncor == -1)
				v[ref[i]] = i;
			}
			else if(cor == i)
				v[ref[i]] = -1;
			else 
			{
				if(cor >= poly->quad_length)
				{	/* other poly is a tri */
					a = poly->quad_length + ((i - poly->quad_length) / 3) * 3;
					b = poly->quad_length + ((cor - poly->quad_length) / 3) * 3;
					if((n[cor] == -1 || n[a + (i - a + 2) % 3] == -1) && ref[a + (i - a + 2) % 3] == ref[b + (cor - b + 1) % 3])
					{
						n[a + (i - a + 2) % 3] = cor;
						n[cor] = a + (i - a + 2) % 3;
		//				printf("i = %u clear = %u\n", i, clear); 
						clear = 0;
						if(n[b + (cor - b + 2) % 3] != -1)
						{
							if(n[i] == -1)
								v[ref[i]] = i;
							else
								v[ref[i]] = -1;
						}
//						v[ref[i]] = -1;
					}
					if((n[i] == -1 || n[b + (cor - b + 2) % 3] == -1) && ref[a + (i - a + 1) % 3] == ref[b + (cor - b + 2) % 3])
					{
						n[i] = b + (cor - b + 2) % 3;						
						n[b + (cor - b + 2) % 3] = i;
//						printf("i = %u clear = %u\n", i, clear); 
						clear = 0;
						if(n[cor] != -1)
						{
							if(n[a + (i - a + 2) % 3] == -1)
								v[ref[i]] = i;
							else
								v[ref[i]] = -1;
						}
//						v[ref[i]] = -1;
					}
				}else
				{
					/* other poly is a quad */
					a = poly->quad_length + ((i - poly->quad_length) / 3) * 3;
					b = (cor / 4) * 4;
					if((n[cor] == -1 || n[a + (i - a + 2) % 3] == -1) && ref[a + (i - a + 2) % 3] == ref[b + (cor + 1) % 4])
					{
						n[a + (i - a + 2) % 3] = cor;
						n[cor] = a + (i - a + 2) % 3;
//						printf("i = %u clear = %u\n", i, clear); 
						clear = 0;
						if(n[b + (cor + 3) % 4] != -1)
						{
							if(n[i] == -1)
								v[ref[i]] = i;
							else
								v[ref[i]] = -1;
						}
//						v[ref[i]] = -1;
					}
					if((n[i] == -1 || n[(cor - b + 3) % 4] == -1) && ref[a + (i - a + 1) % 3] == ref[b + (cor + 3) % 4])
					{
						n[i] = b + (cor + 3) % 4;
						n[b + (cor + 3) % 4] = i;				
//						printf("i = %u clear = %u\n", i, clear); 						
						clear = 0;
						if(n[cor] != -1)
						{
							if(n[a + (i - a + 2) % 3] == -1)
								v[ref[i]] = i;
							else
								v[ref[i]] = -1;
						}
//						v[ref[i]] = -1;
					}
				}						
			}
			counter++;
			clear++;
		}
		laps++;
		
	}
	counter = 0;
	for(i = 0; i < (poly->quad_length + poly->tri_length); i++)
		if(n[i] == -1)
			counter++;
	poly->open_edges = counter;
	free(v);
	poly->neighbor = poly->base_neighbor = n;
	return 100; 
}

PPolyStore *p_sds_create(uint *ref, uint ref_count, egreal *vertex, uint vertex_count, uint version)
{
	PPolyStore *mesh;
	mesh = malloc(sizeof *mesh);
	mesh->ref = NULL;
	mesh->crease = NULL;
	mesh->neighbor = NULL;
	mesh->tri_length = 0;
	mesh->quad_length = 0;			
	mesh->base_neighbor = 0;	
	mesh->base_tri_count = 0;
	mesh->base_quad_count = 0;
	mesh->base_neighbor = NULL;
	mesh->poly_per_base = 1;
	mesh->vertex_count = vertex_count;
	mesh->geometry_version = version;
	mesh->vertex_dependency = p_sds_allocate_depend_first(vertex_count);
	mesh->next = NULL;
	mesh->level = 0;
	mesh->stage[0] = 0;	
	mesh->stage[1] = 0;
	mesh->version = version;
	return mesh;
}

void p_sds_free(PPolyStore *mesh, boolean limited)
{
	if(!limited && mesh->next != NULL)
		p_sds_free(mesh->next, TRUE);
	if(mesh->ref != NULL)
		free(mesh->ref);
	if(!limited && mesh->crease != NULL)
		free(mesh->crease);
	if(mesh->neighbor != NULL && mesh->level != 0)
		free(mesh->neighbor);
	if(!limited && mesh->base_neighbor != NULL)
		free(mesh->base_neighbor);
	if(mesh->vertex_dependency != NULL)
	{
		uint i;
		if(mesh->level != 0)
		{
			for(i = 0; i < mesh->vertex_count; i++)
				if(mesh->vertex_dependency[i].element != NULL)
					free(mesh->vertex_dependency[i].element);
		}
		else
			free(mesh->vertex_dependency->element);
		free(mesh->vertex_dependency);
	}
	free(mesh);
}


float p_sds_stage_count_poly(PPolyStore *mesh, uint *ref, uint ref_count, egreal *vertex, uint vertex_count, egreal default_crease)
{
	uint stage, i = 0;
	ref_count *= 4;
	for(stage = mesh->stage[1]; stage < ref_count && i < MAX_COUNT_STAGE_LOOPS ; stage += 4)
	{
//		printf("ref%u = %u %u %u %u\n", stage / 4, ref[stage], ref[stage + 1], ref[stage + 2], ref[stage + 3]);
		if(ref[stage] < vertex_count && ref[stage + 1] < vertex_count &&  ref[stage + 2] < vertex_count && vertex[ref[stage] * 3] != E_REAL_MAX && vertex[ref[stage + 1] * 3] != E_REAL_MAX && vertex[ref[stage + 2] * 3] != E_REAL_MAX)
		{
			if(ref[stage + 3] < vertex_count && vertex[ref[stage + 3] * 3] != E_REAL_MAX)
				mesh->base_quad_count++;
			else
				mesh->base_tri_count++;				
		}
		i++;
	}
	if(stage == ref_count)
	{
		mesh->ref = malloc((sizeof *mesh->ref) * (3 * mesh->base_tri_count + 4 * mesh->base_quad_count));
		mesh->crease = malloc((sizeof *mesh->crease) * (3 * mesh->base_tri_count + 4 * mesh->base_quad_count));
		for(stage = 0; stage < (3 * mesh->base_tri_count + 4 * mesh->base_quad_count); stage++)
			mesh->crease[stage] = default_crease;
		mesh->stage[1] = 0;
		mesh->stage[0]++;
	}else
		mesh->stage[1] = stage;
	return 1;
}

float p_sds_stage_clean_poly(PPolyStore *mesh, uint *ref, uint ref_count, egreal *vertex, uint vertex_count)
{
	uint stage, i = 0;
	mesh->base_quad_count *= 4;
	ref_count *= 4;
	for(stage = mesh->stage[1]; stage < ref_count && i < MAX_CLEAN_STAGE_LOOPS ; stage += 4)
	{
		if(ref[stage] < vertex_count && ref[stage + 1] < vertex_count &&  ref[stage + 2] < vertex_count && vertex[ref[stage] * 3] != E_REAL_MAX && vertex[ref[stage + 1] * 3] != E_REAL_MAX && vertex[ref[stage + 2] * 3] != E_REAL_MAX)
		{
			if(ref[stage + 3] < vertex_count && vertex[ref[stage + 3] * 3] != E_REAL_MAX)
			{
				mesh->ref[mesh->quad_length++] = ref[stage];
				mesh->ref[mesh->quad_length++] = ref[stage + 1];
				mesh->ref[mesh->quad_length++] = ref[stage + 2];
				mesh->ref[mesh->quad_length++] = ref[stage + 3];												
			}
			else
			{
				mesh->ref[mesh->base_quad_count + mesh->tri_length++] = ref[stage];
				mesh->ref[mesh->base_quad_count + mesh->tri_length++] = ref[stage + 1];
				mesh->ref[mesh->base_quad_count + mesh->tri_length++] = ref[stage + 2];												
			}					
		}
		i++;
	}
	mesh->base_quad_count /= 4;	
	if(stage == ref_count)
	{
 		mesh->stage[1] = 0;
		mesh->stage[0]++;		
	}else
		mesh->stage[1] = stage;
	return 1;
}

float p_sds_stage_clean_poly_cerease(PPolyStore *mesh, uint *ref, uint ref_count, egreal *vertex, uint vertex_count, uint *crease)
{
	uint stage, i = 0;
	ref_count *= 4;
	mesh->base_quad_count *= 4;
	for(stage = mesh->stage[1]; stage < ref_count && i < MAX_CLEAN_STAGE_LOOPS ; stage += 4)
	{
		if(ref[stage] < vertex_count && ref[stage + 1] < vertex_count &&  ref[stage + 2] < vertex_count && vertex[ref[stage] * 3] != E_REAL_MAX && vertex[ref[stage + 1] * 3] != E_REAL_MAX && vertex[ref[stage + 2] * 3] != E_REAL_MAX)
		{
			if(ref[stage + 3] < vertex_count && vertex[ref[stage + 3] * 3] != E_REAL_MAX)
			{
				mesh->crease[mesh->quad_length] = 1 - ((egreal)crease[stage + 0] / 4294967295.0);
				mesh->ref[mesh->quad_length++] = ref[stage];	
				mesh->crease[mesh->quad_length] = 1 - ((egreal)crease[stage + 1] / 4294967295.0);							
				mesh->ref[mesh->quad_length++] = ref[stage + 1];
				mesh->crease[mesh->quad_length] = 1 - ((egreal)crease[stage + 2] / 4294967295.0);	
				mesh->ref[mesh->quad_length++] = ref[stage + 2];
				mesh->crease[mesh->quad_length] = 1 - ((egreal)crease[stage + 3] / 4294967295.0);	
				mesh->ref[mesh->quad_length++] = ref[stage + 3];
/*				printf("crease = %f %f %f %f\n",
					1 - ((egreal)crease[stage + 0] / 4294967295.0),
					1 - ((egreal)crease[stage + 1] / 4294967295.0),
					1 - ((egreal)crease[stage + 2] / 4294967295.0),
					1 - ((egreal)crease[stage + 3] / 4294967295.0));
*/			}
			else
			{
				mesh->crease[mesh->base_quad_count + mesh->tri_length] = 1 - ((egreal)crease[stage] / 4294967295.0);
				mesh->ref[mesh->base_quad_count + mesh->tri_length++] = ref[stage];
				mesh->crease[mesh->base_quad_count + mesh->tri_length] = 1 - ((egreal)crease[stage + 1] / 4294967295.0);
				mesh->ref[mesh->base_quad_count + mesh->tri_length++] = ref[stage + 1];
				mesh->crease[mesh->base_quad_count + mesh->tri_length] = 1 - ((egreal)crease[stage + 2] / 4294967295.0);
				mesh->ref[mesh->base_quad_count + mesh->tri_length++] = ref[stage + 2];
/*				printf("crease = %f %f %f\n",
					1 - ((egreal)crease[stage + 0] / 4294967295.0),
					1 - ((egreal)crease[stage + 1] / 4294967295.0),
					1 - ((egreal)crease[stage + 2] / 4294967295.0));	
*/			}					
		}
		i++;
	}
	mesh->base_quad_count /= 4;	
	if(stage == ref_count)
	{
 		mesh->stage[1] = 0;
		mesh->stage[0]++;		
	}else
		mesh->stage[1] = stage;
	return 1;
}

egreal p_sds_get_crease(PPolyStore *mesh, uint edge)
{
	uint id;
	id = mesh->neighbor[edge];
	if(id == -1)
		return 0;	
	if(mesh->crease == NULL)
		return 1;
	if(edge <  mesh->quad_length)
	{
		if(id / (mesh->poly_per_base * 4) == edge / (mesh->poly_per_base * 4))
			return 1;
		return mesh->crease[(edge / (mesh->poly_per_base * 4)) * 4 + edge % 4];
		return mesh->crease[(edge - edge % (mesh->poly_per_base * 4)) / mesh->poly_per_base + edge % 4];
	}else
	{
		edge -= mesh->quad_length;
		if((id - mesh->quad_length) / (mesh->poly_per_base * 3) == edge / (mesh->poly_per_base * 3))
			return 1;

		return mesh->crease[mesh->base_quad_count * 4 + (edge / (mesh->poly_per_base * 3)) * 3 + edge % 3];	
	//	return mesh->crease[mesh->base_quad_count * 4 + (edge - edge % (mesh->poly_per_base * 3)) / mesh->poly_per_base + edge % 3];
	}
}
//		return mesh->crease[edge / mesh->poly_per_base + (edge - mesh->quad_length * mesh->poly_per_base) % 3];



void p_sds_final_clean(PPolyStore *mesh)
{
	PDepend *dep;
	uint i, j;
	for(i = 0; i < mesh->vertex_count; i++)
	{
		dep = &mesh->vertex_dependency[i];
		for(j = 0; j < dep->length && dep->element[j].vertex != -1; j++)
			dep->element[j].value /= dep->sum;
		dep->length = j;
	}
	mesh->stage[0]++;
}

PPolyStore *p_sds_allocate_next(PPolyStore *pre)
{
	PPolyStore *mesh;
	uint i, count;
	mesh = malloc(sizeof *mesh);
	mesh->tri_length = pre->tri_length * 4;
	mesh->quad_length = pre->quad_length * 4;
	mesh->ref = malloc((sizeof *mesh->ref) * (mesh->tri_length + mesh->quad_length));
	mesh->neighbor = malloc((sizeof *mesh->neighbor) * (mesh->tri_length + mesh->quad_length));
	mesh->crease = pre->crease;
	mesh->base_neighbor = pre->base_neighbor;	
	mesh->base_tri_count = pre->base_tri_count;
	mesh->base_quad_count = pre->base_quad_count;
	mesh->geometry_version = pre->geometry_version;
	mesh->poly_per_base = pre->poly_per_base * 4;
	mesh->open_edges = pre->open_edges * 2; 
	mesh->vertex_count = pre->vertex_count;
	mesh->vertex_dependency = p_sds_allocate_depend(2 * (pre->vertex_count + (mesh->tri_length + mesh->quad_length - mesh->open_edges) / 2 + mesh->open_edges + (mesh->quad_length / 4)));
	mesh->next = NULL;
	mesh->level = pre->level + 1;
	mesh->stage[0] = 0;
	mesh->stage[1] = 0;
	pre->next = mesh;
	mesh->version = pre->version;
	return mesh;
}
