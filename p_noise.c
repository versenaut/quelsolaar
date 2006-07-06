 #include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
	#include <windows.h>
	#include <GL/gl.h>
#else
#if defined(__APPLE__) || defined(MACOSX)
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif
#endif
#include "verse.h"
#include "enough.h"

extern double get_rand(uint32 index);

egreal p_noise_function(egreal *vec, uint level)
{
	int i, j, k, iadd, jadd, kadd;
	egreal fi, fj, fk, f, values[8]; 
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

egreal p_noise_multi_function(egreal *vec)
{
	return p_noise_function(vec, 7) * 0.7 + p_noise_function(vec, 17) * 0.3 + p_noise_function(vec, 31) * 0.15;
}

#define P_POINT_NOISE_LEVEL 128

egreal p_noise_point_function(egreal *vec)
{
	int i, j, k, start, pos, found;
	egreal fi, fj, fk, f, fbest = 100000, vector[3]; 
	fi = vec[0];
	fj = vec[1];
	fk = vec[2];
	i = fi;
	j = fj;
	k = fk;
	fi -= (egreal)i;
	fj -= (egreal)j;
	fk -= (egreal)k;
	start = i + j * P_POINT_NOISE_LEVEL + k * P_POINT_NOISE_LEVEL * P_POINT_NOISE_LEVEL;
	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < 3; j++)
		{
			for(k = 0; k < 3; k++)
			{
				pos = start + i + j * P_POINT_NOISE_LEVEL + k * P_POINT_NOISE_LEVEL * P_POINT_NOISE_LEVEL;
				vector[0] = get_rand(pos * 3 + 0) + (float)i - (1.0 + fi);
				vector[1] = get_rand(pos * 3 + 1) + (float)j - (1.0 + fj);
				vector[2] = get_rand(pos * 3 + 2) + (float)k - (1.0 + fk);
				f = vector[0] * vector[0] + vector[0] * vector[0] + vector[0] * vector[0];	
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