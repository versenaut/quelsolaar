#include "la_includes.h"

#include "la_geometry_undo.h"
#include "la_projection.h"
#include "la_draw_overlay.h"
#include "la_tool.h"
#include "la_pop_up.h"


void geometry_load_obj(void *user, char *file)
{
	FILE	*obj;
	char	line[512];
	uint	i, ref_length = 0, vertex_length = 1, count, ref[4], *vertex_ids;
	float	x, y, z;
	obj = fopen(file, "r");
	if(obj != NULL)
	{
		printf("exit(0)");
		for(i = 0; i < 512; i++)
			line[i] = 0;
		while((fgets(line, sizeof line, obj)) != NULL)
			if(line[0] == 118) /* v */
				if(sscanf(line, "v %f %f %f", &x, &y, &z) == 3)
					vertex_length++;
		vertex_ids = malloc((sizeof *vertex_ids) * vertex_length);
		for(i = 0; i < vertex_length; i++)
			vertex_ids[i] = udg_find_empty_slot_vertex();

		rewind(obj); 
		i = 0;
		while((fgets(line, sizeof line, obj)) != NULL)
		{
			if(line[0] == 118) /* v */
			{
				count = sscanf(line, "v %f %f %f", &x, &y, &z);
				printf("v count = %i ref = %f %f %f\n", count, x, y, z);
				if(count == 3)
					udg_vertex_set(vertex_ids[++i], NULL, x, y, z);
			}else
			if(line[0] == 102) /* f */
			{
				count = sscanf(line, "f %u %u %u %u\n", &ref[0], &ref[1], &ref[2], &ref[3]);

				if(count == 3 && ref[0] < vertex_length && ref[1] < vertex_length && ref[2] < vertex_length)
					udg_polygon_set(udg_find_empty_slot_polygon(), vertex_ids[ref[0]], vertex_ids[ref[1]], vertex_ids[ref[2]], -1);
				if(count == 4 && ref[0] < vertex_length && ref[1] < vertex_length && ref[2] < vertex_length && ref[3] < vertex_length)
					udg_polygon_set(udg_find_empty_slot_polygon(), vertex_ids[ref[0]], vertex_ids[ref[1]], vertex_ids[ref[2]], vertex_ids[ref[3]]);
			}/*else
				printf("failed; %s\n", line);*/
		}
		fclose(obj);
	}
}

void geometry_save_obj(void *user, char *file)
{
	FILE *obj;
	double *vertex;
	uint *ref, ref_length, vertex_length, i, j, *count;
	udg_get_geometry(&vertex_length, &ref_length, &vertex, &ref, NULL);

	obj = fopen(file, "wt");
	if(obj != NULL)
	{
		count = malloc((sizeof *count) * vertex_length);
		for(i = 0; i < vertex_length; i++)
			count[i] = 0;
		for(i = 0; i < ref_length ; i++)
		{
			if(ref[i * 4] < vertex_length && ref[i * 4 + 1] < vertex_length && ref[i * 4 + 2] < vertex_length &&
				vertex[ref[i * 4] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 1] * 3] != V_REAL64_MAX && vertex[ref[i * 4 + 2] * 3] != V_REAL64_MAX)
			{
				count[ref[i * 4]]++;
				count[ref[i * 4 + 1]]++;
				count[ref[i * 4 + 2]]++;
				if(ref[i * 4 + 3] < vertex_length && vertex[ref[i * 4 + 3] * 3] != V_REAL64_MAX)
					count[ref[i * 4 + 3]]++;
			}
		}
		fprintf(obj, "# Object outputed form Loq Airou by Eskil Steenberg\n\n");
		fprintf(obj, "g\n\n");
		for(i = 0; i < vertex_length ; i++)
			if(count[i] != 0)
				fprintf(obj, "v %f %f %f\n", vertex[i * 3], vertex[i * 3 + 1], vertex[i * 3 + 2]);

		j = 0;
		for(i = 0; i < vertex_length; i++)
		{
			if(count[i] == 0)
				count[i] = j;
			else
				count[i] = j++;
		}
		fprintf(obj, "\ng SDS_Cage\n\n");
		for(i = 0; i < ref_length ; i++)
		{
			if(ref[i * 4] < vertex_length && ref[i * 4 + 1] < vertex_length && ref[i * 4 + 2] < vertex_length)
			{
				if(ref[i * 4 + 3] < vertex_length)
					fprintf(obj, "f %i %i %i %i\n", count[ref[i * 4]] + 1, count[ref[i * 4 + 1]] + 1, count[ref[i * 4 + 2]] + 1, count[ref[i * 4 + 3]] + 1);
				else
					fprintf(obj, "f %i %i %i\n", count[ref[i * 4]] + 1, count[ref[i * 4 + 1]] + 1, count[ref[i * 4 + 2]] + 1);
			}
		}
		fprintf(obj, "\ng\n\n");
		fclose(obj);
	}
}

