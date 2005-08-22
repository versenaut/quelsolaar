
#include "verse.h"
#include "enough.h"

#include "seduce.h"
#include "p_sds_geo.h"
extern double get_rand(uint32 index);


#define MAX_COUNT_STAGE_LOOPS 1000000
#define MAX_CLEAN_STAGE_LOOPS 1000000

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
			dep->length += 8;
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
	e = malloc((sizeof *e) * length);

	d = malloc((sizeof *d) * length);
	d[0].length = -1;
	for(i = 0; i < length; i++)
	{
		d[i].sum = 1;
		d[i].length = 1;
		d[i].element = &e[i];
		e[i].value = 1;
		e[i].vertex = i;
	}
	return d;
}

PDepend *p_sds_allocate_depend(uint length)
{
	PDepend *d;
	uint i;
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
	if(dep[0].length != (unsigned short) ~0)
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
				if(cor > poly->quad_length)
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
				if(cor > poly->quad_length)
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
	printf("counter = %u laps = %u corner = %u\n", counter, laps, poly->quad_length + poly->tri_length);
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
/*		free(mesh->vertex_dependency);*/
	}
	free(mesh);

}


float p_sds_stage_count_poly(PPolyStore *mesh, uint *ref, uint ref_count, egreal *vertex, uint vertex_count)
{
	uint stage;
	ref_count *= 4;
	for(stage = mesh->stage[1]; stage < ref_count && stage < MAX_COUNT_STAGE_LOOPS ; stage += 4)
	{
//		printf("ref%u = %u %u %u %u\n", stage / 4, ref[stage], ref[stage + 1], ref[stage + 2], ref[stage + 3]);
		if(ref[stage] < vertex_count && ref[stage + 1] < vertex_count &&  ref[stage + 2] < vertex_count && vertex[ref[stage] * 3] != E_REAL_MAX && vertex[ref[stage + 1] * 3] != E_REAL_MAX && vertex[ref[stage + 2] * 3] != E_REAL_MAX)
		{
			if(ref[stage + 3] < vertex_count && vertex[ref[stage + 3] * 3] != E_REAL_MAX)
				mesh->base_quad_count++;
			else
				mesh->base_tri_count++;				
		}
	}
	if(stage == ref_count)
	{
		mesh->ref = malloc((sizeof *mesh->ref) * (3 * mesh->base_tri_count + 4 * mesh->base_quad_count));
		mesh->crease = malloc((sizeof *mesh->crease) * (3 * mesh->base_tri_count + 4 * mesh->base_quad_count));
		for(stage = 0; stage < (3 * mesh->base_tri_count + 4 * mesh->base_quad_count); stage++)
			mesh->crease[stage] = 1;
		mesh->stage[1] = 0;
		mesh->stage[0]++;
	}else
		mesh->stage[1] = stage;
	return 1;
}

float p_sds_stage_clean_poly(PPolyStore *mesh, uint *ref, uint ref_count, egreal *vertex, uint vertex_count)
{
	uint stage;
	mesh->base_quad_count *= 4;
	ref_count *= 4;
	for(stage = mesh->stage[1]; stage < ref_count && stage < MAX_CLEAN_STAGE_LOOPS ; stage += 4)
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
	uint stage;
	ref_count *= 4;
	mesh->base_quad_count *= 4;
	for(stage = mesh->stage[1]; stage < ref_count && stage < MAX_CLEAN_STAGE_LOOPS ; stage += 4)
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

float p_sds_stage(uint *naighbour, uint ref_count, egreal *crease)
{
	egreal f;
	uint i;
	for(i = 0; i < ref_count * 4; i++)
	{
		if(*naighbour < i)
		{
			f = (crease[i] + *naighbour) * 0.5;
			crease[i] = f;
			*naighbour = f;
		}
		*naighbour++;
	}
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


void sds_test_draw(PPolyStore *s, egreal *vertex)
{
	float array[16];
	uint i = 0, j, color;
	for(i = 0; i < 4 * s->base_quad_count; i += 4)
	{
		array[0] = (vertex[s->ref[i] * 3] + vertex[s->ref[i + 1] * 3] + vertex[s->ref[i + 2] * 3] + vertex[s->ref[i + 3] * 3]) / 4.0;
		array[1] = (vertex[s->ref[i] * 3 + 1] + vertex[s->ref[i + 1] * 3 + 1] + vertex[s->ref[i + 2] * 3 + 1] + vertex[s->ref[i + 3] * 3 + 1]) / 4.0;
		array[2] = (vertex[s->ref[i] * 3 + 2] + vertex[s->ref[i + 1] * 3 + 2] + vertex[s->ref[i + 2] * 3 + 2] + vertex[s->ref[i + 3] * 3 + 2]) / 4.0;
		for(j = 0; j < 4; j++)
		{	
			array[3] = vertex[s->ref[i + j] * 3] * 0.9 + array[0] * 0.1;	
			array[4] = vertex[s->ref[i + j] * 3 + 1] * 0.9 + array[1] * 0.1;	
			array[5] = vertex[s->ref[i + j] * 3 + 2] * 0.9 + array[2] * 0.1;
			array[6] = vertex[s->ref[i + (j + 1) % 4] * 3] * 0.9 + array[0] * 0.1;	
			array[7] = vertex[s->ref[i + (j + 1) % 4] * 3 + 1] * 0.9 + array[1] * 0.1;	
			array[8] = vertex[s->ref[i + (j + 1) % 4] * 3 + 2] * 0.9 + array[2] * 0.1;
			color = i + j;
	//		printf("s->base_neighbor[color] %u\n", s->base_neighbor[color]);
			if(s->base_neighbor[color] < color)
				color = s->base_neighbor[color];

			if(s->base_neighbor[color] == -1)
				sui_draw_gl(GL_TRIANGLES, array, 3, 3, 0, 0, 1);		
			else
				sui_draw_gl(GL_TRIANGLES, array, 3, 3, get_rand(color), get_rand(color + 1), 0);
		}

	}
	for(; i < 3 * s->base_tri_count + 4 * s->base_quad_count; i += 3)
	{
		array[0] = (vertex[s->ref[i] * 3] + vertex[s->ref[i + 1] * 3] + vertex[s->ref[i + 2] * 3]) / 3.0;
		array[1] = (vertex[s->ref[i] * 3 + 1] + vertex[s->ref[i + 1] * 3 + 1] + vertex[s->ref[i + 2] * 3 + 1]) / 3.0;
		array[2] = (vertex[s->ref[i] * 3 + 2] + vertex[s->ref[i + 1] * 3 + 2] + vertex[s->ref[i + 2] * 3 + 2]) / 3.0;
		for(j = 0; j < 3; j++)
		{	
			array[3] = vertex[s->ref[i + j] * 3] * 0.9 + array[0] * 0.1;	
			array[4] = vertex[s->ref[i + j] * 3 + 1] * 0.9 + array[1] * 0.1;	
			array[5] = vertex[s->ref[i + j] * 3 + 2] * 0.9 + array[2] * 0.1;
			array[6] = vertex[s->ref[i + (j + 1) % 3] * 3] * 0.9 + array[0] * 0.1;	
			array[7] = vertex[s->ref[i + (j + 1) % 3] * 3 + 1] * 0.9 + array[1] * 0.1;	
			array[8] = vertex[s->ref[i + (j + 1) % 3] * 3 + 2] * 0.9 + array[2] * 0.1;
			color = i + j;
	//		printf("s->base_neighbor[color] %u\n", s->base_neighbor[color]);
			if(s->base_neighbor[color] < color)
				color = s->base_neighbor[color];
			if(s->base_neighbor[color] == -1)
				sui_draw_gl(GL_TRIANGLES, array, 3, 3, 0, 0, 1);		
			else
			sui_draw_gl(GL_TRIANGLES, array, 3, 3, get_rand(color), get_rand(color + 1), 0);
		}
	}
//	printf("count prim = %u %u\n", s->base_tri_count, s->base_quad_count);

}


void sds_test_draw_2(PPolyStore *s, egreal *vertex)
{
	static float t = 0;
	float array[12], center[3];
	PDepend *dep;
	uint i = 0, j, k, color, pos;
//	vertex[4] = sin(t++ * 0.1) * 0.001;
//	vertex[4] -= 0.001;
	for(i = 0; i < s->quad_length; i += 4)
	{
		for(j = 0; j < 4 * 3; j++)
			array[j] = 0;
		for(j = 0; j < 4; j++)
		{	
			dep = &s->vertex_dependency[s->ref[i + j]];
			for(k = 0; k < dep->length; k++)
			{
				if(dep->element[k].vertex != -1)
				{

					array[j * 3 + 0] += vertex[3 * dep->element[k].vertex] * dep->element[k].value / dep->sum;
					array[j * 3 + 1] += vertex[3 * dep->element[k].vertex + 1] * dep->element[k].value / dep->sum;
					array[j * 3 + 2] += vertex[3 * dep->element[k].vertex + 2] * dep->element[k].value / dep->sum;
				}
			}
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		sui_draw_gl(GL_QUADS, array, 4, 3, 0.3, 0.3, 0.3);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		{
			float array2[12], color[12];
			for(j = 0; j < 4; j++)
			{
				array2[j * 3 + 0] = (array[j * 3 + 0] + array[((j + 1) % 4) * 3 + 0]) * 0.5;
				array2[j * 3 + 1] = (array[j * 3 + 1] + array[((j + 1) % 4) * 3 + 1]) * 0.5;
				array2[j * 3 + 2] = (array[j * 3 + 2] + array[((j + 1) % 4) * 3 + 2]) * 0.5;
				
				k = i + j;
				if(s->neighbor[i + j] < k)
					k = s->neighbor[k];

				color[j * 3 + 0] = get_rand(k);
				color[j * 3 + 1] = get_rand(k + 1);
				color[j * 3 + 2] = get_rand(k + 2);
			}

			sui_set_color_array_gl(color, 4, 3);
			sui_draw_gl(GL_QUADS, array2, 4, 3, get_rand(i), get_rand(i + 1), get_rand(i + 2));
		}
	}
//	printf("s->base_tri_count = %u\n", s->base_tri_count);
	for(; i < s->tri_length + s->quad_length; i += 3)
	{
		for(j = 0; j < 3 * 3; j++)
			array[j] = 0;
		for(j = 0; j < 3; j++)
		{	
			dep = &s->vertex_dependency[s->ref[i + j]];
			for(k = 0; k < dep->length; k++)
			{
				if(dep->element[k].vertex != -1)
				{

					array[j * 3 + 0] += vertex[3 * dep->element[k].vertex] * dep->element[k].value / dep->sum;
					array[j * 3 + 1] += vertex[3 * dep->element[k].vertex + 1] * dep->element[k].value / dep->sum;
					array[j * 3 + 2] += vertex[3 * dep->element[k].vertex + 2] * dep->element[k].value / dep->sum;
				}
			}
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		sui_draw_gl(GL_TRIANGLES, array, 3, 3, 0.3, 0.3, 0.3);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		{
			float array2[12], color[12];
			for(j = 0; j < 3; j++)
			{
				array2[j * 3 + 0] = (array[j * 3 + 0] + array[((j + 1) % 3) * 3 + 0]) * 0.5;
				array2[j * 3 + 1] = (array[j * 3 + 1] + array[((j + 1) % 3) * 3 + 1]) * 0.5;
				array2[j * 3 + 2] = (array[j * 3 + 2] + array[((j + 1) % 3) * 3 + 2]) * 0.5;
				
				k = i + j;
				if(s->neighbor[i + j] < k)
					k = s->neighbor[k];

				color[j * 3 + 0] = get_rand(k);
				color[j * 3 + 1] = get_rand(k + 1);
				color[j * 3 + 2] = get_rand(k + 2);
			}

			sui_set_color_array_gl(color, 3, 3);
			sui_draw_gl(GL_TRIANGLES, array2, 3, 3, get_rand(i), get_rand(i + 1), get_rand(i + 2));
		}
	}
}

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
