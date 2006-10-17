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
#define GL_CLAMP_TO_EDGE                  0x812F
/*
	VN_M_RAMP_SQUARE = 0,
	VN_M_RAMP_LINEAR,
	VN_M_RAMP_SMOOTH
*/
/*
float compute_spline(float f, float v0, float v1, float v2, float v3)
{
	return ((1 - f) * (1 - f)) * f + ((1 - f) * f + (f + 1 - f) * (1 - f)) * (1 - f);
}
*/
uint p_shader_ramp_texture_create(uint texture_size, VMatFrag *frag)
{
	uint texture_id, i, j = 0;
	float *buf, pos, scale, f;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_1D, texture_id);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	buf = malloc((sizeof *buf) * texture_size * 3);

	pos = frag->ramp.ramp[0].pos;
	scale = frag->ramp.ramp[frag->ramp.point_count - 1].pos - frag->ramp.ramp[0].pos;
	for(i = 0; i < texture_size; i++)
	{
		f = (float)i / (float)texture_size;
		f = pos + f * scale;
		if(f >= frag->ramp.ramp[j + 1].pos && j + 1 < frag->ramp.point_count)
			j++;
		if(frag->ramp.type != VN_M_RAMP_SQUARE)
		{
			f = (f - frag->ramp.ramp[j].pos) / (frag->ramp.ramp[j + 1].pos - frag->ramp.ramp[j].pos);
			if(frag->ramp.type == VN_M_RAMP_SMOOTH)
				f = ((f + 1 - f) * f + f * (1 - f)) * f + f * f * (1 - f);
		}else
			f = 0;

		buf[i * 3 + 0] = frag->ramp.ramp[j].red * (1.0 - f) + frag->ramp.ramp[j + 1].red * f;
		buf[i * 3 + 1] = frag->ramp.ramp[j].green * (1.0 - f) + frag->ramp.ramp[j + 1].green * f;
		buf[i * 3 + 2] = frag->ramp.ramp[j].blue * (1.0 - f) + frag->ramp.ramp[j + 1].blue * f;
	}
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, texture_size, 0, GL_RGB, GL_FLOAT, buf);
	free(buf);
	return texture_id;
}
