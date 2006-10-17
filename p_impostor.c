/*
 * 
*/

#include <stdio.h>
#include <math.h>

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
#include <math.h>

#include "enough.h"
#include "st_types.h"
#include "p_task.h"
#include "p_sds_geo.h"
#include "p_sds_table.h"
#include "p_sds_obj.h"
#include "p_extension.h"
#include "p_object.h"
#include "st_matrix_operations.h"

#define GL_TEXTURE_CUBE_MAP_EXT             0x8513 
#define GL_TEXTURE_BINDING_CUBE_MAP_EXT     0x8514 
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT  0x8515 
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT  0x8516 
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT  0x8517 
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT  0x8518 
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT  0x8519 
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT  0x851A 
#define GL_PROXY_TEXTURE_CUBE_MAP_EXT       0x851B 
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE_EXT    0x851C 
#define GL_CLAMP_TO_EDGE					0x812F

#define GL_RGB_FLOAT32                  0x8815


extern void p_texture_render_bind(uint texture, uint size, uint target);
extern void p_texture_render_unbind(void);
extern void p_render_set_transform(ENode *node);

extern uint p_create_renderable_texture(uint size, uint format);
extern void p_pre_fbo_draw(float fov);
extern void p_post_fbo_draw(void);
extern void p_render_object(ENode *node, boolean transparency, boolean test);
extern uint p_shader_create(char *vertex, char *fragment);
extern uint p_th_get_hdri_token(boolean alpha);
extern void p_shader_bind_texture(uint program, char *name, uint unit, uint texture_id);
extern void p_shader_unbind_texture(uint unit);

static uint p_impostor_resolution = 128; 
static float p_render_impostor_size = 0.03;
static uint p_blur_program = -1; 

uint create_environment(uint size, float *buf)
{
	uint environment;
	uint format = p_th_get_hdri_token(FALSE);
	glEnable(GL_TEXTURE_CUBE_MAP_EXT);
	glGenTextures(1, &environment);
	glBindTexture(GL_TEXTURE_CUBE_MAP_EXT, environment);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT, 0, format, size, size, 0, GL_RGB, GL_FLOAT, buf); 
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT, 0, format, size, size, 0, GL_RGB, GL_FLOAT, buf); 
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT, 0, format, size, size, 0, GL_RGB, GL_FLOAT, buf); 
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT, 0, format, size, size, 0, GL_RGB, GL_FLOAT, buf); 
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT, 0, format, size, size, 0, GL_RGB, GL_FLOAT, buf); 
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT, 0, format, size, size, 0, GL_RGB, GL_FLOAT, buf);
	glDisable(GL_TEXTURE_CUBE_MAP_EXT);
	return environment;
}

void print_blur_matrix(void)
{
	double point[3];
	uint i;
	for(i = 0; i < 20; i++)
	{

		point[0] = (get_rand(i * 6 + 0) * 2 - 1) * 0.6;
		point[1] = (get_rand(i * 6 + 1) * 2 - 1) * 0.6;
		point[2] = (get_rand(i * 6 + 2) * 2 - 1) * 0.6;
		printf("color += textureCube(environment, normal + vec3(%f, %f, %f) * vec3(scale, scale, scale)\n", point[0], point[1], point[2]);
	}
}

void p_create_cube_blur_shader(void)
{
	char vertex[300] =
	"varying vec3 r_normal;"
	"void main()"
	"{"
//	"	r_normal = gl_NormalMatrix * gl_Vertex.xyz;"
	"	r_normal = gl_Vertex.xyz;"
	"	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
	"}";
	char fragment[6000] =
	"uniform samplerCube environment;\n"
	"uniform samplerCube my_noise;\n"
	"varying vec3 r_normal;\n"
	"void main()\n"
	"{\n"
	"   vec3 normal;\n"
	"   vec4 color;\n"
	"   float scale = 0.6;\n"
	"	normal = normalize(r_normal);\n"
	"	color = textureCube(environment, normal + vec3(0.169075, 0.135824, -0.176180) * vec3(scale, scale, scale));\n"
	"	color += textureCube(environment, normal + vec3(-0.186778, -0.073838, 0.487833) * vec3(scale, scale, scale));\n"
	"	color += textureCube(environment, normal + vec3(0.446765, 0.270796, 0.071040) * vec3(scale, scale, scale));\n"
	"	color += textureCube(environment, normal + vec3(-0.540902, -0.240882, -0.437164) * vec3(scale, scale, scale));\n"

	"	color += textureCube(environment, normal + vec3(0.239617, 0.580526, -0.047384) * vec3(scale, scale, scale));\n"
	"	color += textureCube(environment, normal + vec3(-0.177717, 0.124415, -0.170225) * vec3(scale, scale, scale));\n"
	"	color += textureCube(environment, normal + vec3(0.262794, 0.580178, 0.183709) * vec3(scale, scale, scale));\n"
	"	color += textureCube(environment, normal + vec3(-0.284242, 0.537213, -0.396187) * vec3(scale, scale, scale));\n"

	"	color += textureCube(environment, normal + vec3(0.031456, -0.215087, 0.279483) * vec3(scale, scale, scale));\n"
	"	color += textureCube(environment, normal + vec3(-0.493833, 0.512675, -0.399887) * vec3(scale, scale, scale));\n"
	"	color += textureCube(environment, normal + vec3(0.260768, 0.109893, -0.244902) * vec3(scale, scale, scale));\n"
	"	color += textureCube(environment, normal + vec3(-0.321655, -0.434036, 0.533835) * vec3(scale, scale, scale));\n"

	"	color += textureCube(environment, normal + vec3(0.465889, -0.129719, 0.525718) * vec3(scale, scale, scale));\n"
	"	color += textureCube(environment, normal + vec3(-0.482868, -0.387759, -0.479857) * vec3(scale, scale, scale));\n"
	"	color += textureCube(environment, normal + vec3(0.161984, -0.218762, -0.293495) * vec3(scale, scale, scale));\n"
	"	color += textureCube(environment, normal + vec3(-0.492634, 0.166669, -0.325802) * vec3(scale, scale, scale));\n"
	"	gl_FragColor += color / vec4(40.0, 40.0, 40.0, 1.0);\n"
//	"	gl_FragColor = vec4(normal, 1.0);\n"
	"}\n";

	if(p_blur_program == -1)
		p_blur_program = p_shader_create(vertex, fragment);
//	print_blur_matrix();
}

/*
color += textureCube(environment, normal + vec3(0.064213, 0.557927, 0.412619) * vec3(scale, scale, scale));\n"
color += textureCube(environment, normal + vec3(-0.133883, -0.455591, 0.511163) * vec3(scale, scale, scale)
color += textureCube(environment, normal + vec3(-0.312261, 0.515510, -0.240775) * vec3(scale, scale, scale)
color += textureCube(environment, normal + vec3(0.518477, -0.339376, 0.346199) * vec3(scale, scale, scale)
color += textureCube(environment, normal + vec3(0.169075, 0.135824, -0.176180) * vec3(scale, scale, scale)
color += textureCube(environment, normal + vec3(-0.186778, -0.073838, 0.487833) * vec3(scale, scale, scale)
color += textureCube(environment, normal + vec3(0.446765, 0.270796, 0.071040) * vec3(scale, scale, scale)
color += textureCube(environment, normal + vec3(-0.540902, -0.240882, -0.437164) * vec3(scale, scale, scale)
color += textureCube(environment, normal + vec3(0.239617, 0.580526, -0.047384) * vec3(scale, scale, scale)
color += textureCube(environment, normal + vec3(0.177717, 0.124415, -0.170225) * vec3(scale, scale, scale)
color += textureCube(environment, normal + vec3(0.262794, 0.580178, 0.183709) * vec3(scale, scale, scale)
*/
extern void p_shader_use(uint program);
extern double get_rand(uint32 index);

uint p_blur_object_environment(ENode *node, uint blur, uint texture, uint size, uint side)
{
	float draw[72] = {
	1.0, 1.0, 1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0,
	1.0, 1.0, -1.0, -1.0, 1.0, -1.0, -1.0, -1.0, -1.0, 1.0, -1.0, -1.0,
	1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0, -1.0, -1.0, 1.0, -1.0, 1.0,
	-1.0, 1.0, 1.0, -1.0, 1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 1.0,
	1.0, 1.0, 1.0, -1.0, 1.0, 1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0,
	1.0, -1.0, 1.0, -1.0, -1.0, 1.0, -1.0, -1.0, -1.0, 1.0, -1.0, -1.0};
	double matrix[16], objpos[3], point_a[3], point_b[3];
	double fov_v, fov_h;
	PObjImpostor *imp;
	ENode *n;
	PObject *o;
	uint i;

	p_create_cube_blur_shader();
	e_nso_get_pos_time(node, objpos, 0, 0);

	if(blur == -1)
	{
		blur = create_environment(size, NULL);
	}
	glLoadIdentity();
	glPushMatrix();
//	glClearColor(0, 0, 0, 0);
	switch(side)
	{
		case 0 :
			p_texture_render_bind(blur, size, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT);
		break;
		case 1 :
			p_texture_render_bind(blur, size, GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT);
			glRotatef(180, 0, 1, 0);
		break;
		case 2 :
			p_texture_render_bind(blur, size, GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT);
			glRotatef(90, 0, 1, 0);
		break;
		case 3 :
			p_texture_render_bind(blur, size, GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT);
			glRotatef(-90, 0, 1, 0);
		break;
		case 4 :
			p_texture_render_bind(blur, size, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT);
			glRotatef(180, 0, 1, 0);
			glRotatef(-90, 1, 0, 0);
		break;
		case 5 :
			p_texture_render_bind(blur, size, GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT);
			glRotatef(180, 0, 1, 0);
			glRotatef(90, 1, 0, 0);
		break;
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(0.01, -0.01, 0.01, -0.01, 0.01, 100.0);
	glViewport(0, 0, size, size);
	glMatrixMode(GL_MODELVIEW);
	p_shader_use(p_blur_program);
	p_shader_bind_texture(p_blur_program, "environment", 0, texture);
//	p_shader_bind_texture(p_blur_program, "noise", 1, noise);

	glDisableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT , 0, draw);
	glDrawArrays(GL_QUADS, 0, 24);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	p_shader_unbind_texture(0);
	p_shader_unbind_texture(1);
	p_shader_use(0);
	glPopMatrix();
	p_texture_render_unbind();
	return blur;
}

void p_render_set_impostor_size(float size)
{
	p_render_impostor_size = size;
}

float p_render_get_impostor_size(void)
{
	return p_render_impostor_size;
}

void p_render_set_impostor_resolution(uint resolution)
{
	p_impostor_resolution = resolution;
}

uint p_render_get_impostor_resolution(void)
{
	return p_impostor_resolution;
}



void p_init_impostor(PObjImpostor *imp)
{
	static uint i = 0;
	i++;
	imp->impostor = FALSE;
	imp->version = 0;
	imp->texture = -1;
	imp->vector[0] = imp->vector[1] = imp->vector[2] = 0.0;
	imp->length = 0;
	imp->timer = (float)((i * i) % 20) / 20.0;
	imp->priority = 0.4;
	imp->size = 1;
	imp->environment = -1;
	imp->blur = -1;
}
void p_render_lit_and_transformed_object(ENode *node, boolean transparency);
double *p_lod_get_view_pos(void);
double *p_lod_get_view_matrix(void);

void create_matrix_normalized_c(double *matrix, double *origo, double *point_a, double *point_b);

void draw_fov(ENode *node, double *matrix, PObject *o)
{
	egreal high_x, low_x, high_y, low_y, high_z, low_z, high_x2, low_x2, high_y2, low_y2, high_z2, low_z2;
	double x, y, z, f, fov = 0, scale[3];
	float vertex[8 * 3];
	ENode *g_node;
	uint i;

	high_x = 0.25;
	low_x = -0.25;
	high_y = 0.25;
	low_y = -0.25;
	high_z = 0.25;
	low_z = -0.25;

	for(i = 0; i < o->mesh_count && o->meshes[i] != NULL; i++)
	{	
		g_node = e_ns_get_node(0, o->meshes[i]->geometry_id);
		if(g_node != NULL && V_NT_GEOMETRY == e_ns_get_node_type(g_node))
		{
			e_nsg_get_bounding_box(g_node, &high_x, &low_x, &high_y, &low_y, &high_z, &low_z);
			break;
		}
	}
	for(; i < o->mesh_count && o->meshes[i] != NULL; i++)
	{
		g_node = e_ns_get_node(0, o->meshes[i]->geometry_id);
		if(g_node != NULL && V_NT_GEOMETRY == e_ns_get_node_type(g_node))
		{
			e_nsg_get_bounding_box(g_node, &high_x2, &low_x2, &high_y2, &low_y2, &high_z2, &low_z2);


			if(high_x2 > high_x)
				high_x = high_x2;
			if(low_x2 < low_x)
				low_x = low_x2;
			if(high_y2 > high_y)
				high_y = high_y2;
			if(low_y2 < low_y)
				low_y = low_y2;
			if(high_z2 > high_z)
				high_z = high_z2;
			if(low_z2 < low_z)
				low_z = low_z2;
		}
	}

	e_nso_get_scale(node, scale);
	high_x *= scale[0];
	low_x *= scale[0];
	high_y *= scale[1];
	low_y *= scale[1];
	high_z *= scale[2];
	low_z *= scale[2];
	for(i = 0; i < 8; i++)
	{
		if(i % 2 == 0)
			vertex[i * 3] = high_x;
		else
			vertex[i * 3] = low_x;
		if((i / 2) % 2 == 0)
			vertex[i * 3 + 1] = high_y;
		else
			vertex[i * 3 + 1] = low_y;
		if((i / 4) % 2 == 0)
			vertex[i * 3 + 2] = high_z;
		else
			vertex[i * 3 + 2] = low_z;
	}
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT , 0, vertex);
	glColor4f(1, 1, 1, 0);
	glDrawArrays(GL_LINES, 0, 8);
}

void compute_fov(ENode *node, double *matrix, PObject *o, double *fov_v, double *fov_h)
{
	egreal high_x, low_x, high_y, low_y, high_z, low_z, high_x2, low_x2, high_y2, low_y2, high_z2, low_z2;
	double x, y, z, f, scale[3];
	ENode *g_node;
	uint i;

	high_x = 0.25;
	low_x = -0.25;
	high_y = 0.25;
	low_y = -0.25;
	high_z = 0.25;
	low_z = -0.25;

	for(i = 0; i < o->mesh_count && o->meshes[i] != NULL; i++)
	{
		g_node = e_ns_get_node(0, o->meshes[i]->geometry_id);
		if(g_node != NULL && V_NT_GEOMETRY == e_ns_get_node_type(g_node))
		{
			e_nsg_get_bounding_box(g_node, &high_x, &low_x, &high_y, &low_y, &high_z, &low_z);
			break;
		}
	}
	for(; i < o->mesh_count && o->meshes[i] != NULL; i++)
	{
		g_node = e_ns_get_node(0, ((PMesh *)o->meshes)[i].geometry_id);
		if(g_node != NULL && V_NT_GEOMETRY == e_ns_get_node_type(g_node))
		{
			e_nsg_get_bounding_box(g_node, &high_x2, &low_x2, &high_y2, &low_y2, &high_z2, &low_z2);
			if(high_x2 > high_x)
				high_x = high_x2;
			if(low_x2 < low_x)
				low_x = low_x2;
			if(high_y2 > high_y)
				high_y = high_y2;
			if(low_y2 < low_y)
				low_y = low_y2;
			if(high_z2 > high_z)
				high_z = high_z2;
			if(low_z2 < low_z)
				low_z = low_z2;
		}
	}

	e_nso_get_scale(node, scale);
	
	high_x *= scale[0];
	low_x *= scale[0];
	high_y *= scale[1];
	low_y *= scale[1];
	high_z *= scale[2];
	low_z *= scale[2];
	for(i = 0; i < 8; i++)
	{
		if(i % 2 == 0)
			x = high_x;
		else
			x = low_x;
		if((i / 2) % 2 == 0)
			y = high_y;
		else
			y = low_y;
		if((i / 4) % 2 == 0)
			z = high_z;
		else
			z = low_z;
		point_threw_matrix3(matrix, &x, &y, &z);
		f = x / z;
		if(f > *fov_h)
			*fov_h = f;
		if(-f > *fov_h)
			*fov_h = -f;
		f = y / z;
		if(f > *fov_v)
			*fov_v = f;
		if(-f > *fov_v)
			*fov_v = -f;
	}
}
// uint p_env_get_environment(PObjEnv *env)




void p_draw_reflection_flares(double *view);

uint update_object_environment(ENode *node, uint texture, uint size, uint side)
{
	double matrix[16], objpos[3], point_a[3], point_b[3];
	double fov_v, fov_h;
	PObjImpostor *imp;
	ENode *n;
	PObject *o;
	uint i;
	e_nso_get_pos_time(node, objpos, 0, 0);

	if(texture == -1)
		texture = create_environment(size, NULL);
	glPushMatrix();
	glLoadIdentity();
	glClearColor(0, 0, 0, 0);
	switch(side)
	{
		case 0 :
			p_texture_render_bind(texture, size, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT);
		break;
		case 1 :
			p_texture_render_bind(texture, size, GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT);
			glRotatef(180, 0, 1, 0);
		break;
		case 2 :
			p_texture_render_bind(texture, size, GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT);
			glRotatef(90, 0, 1, 0);
		break;
		case 3 :
			p_texture_render_bind(texture, size, GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT);
			glRotatef(-90, 0, 1, 0);
		break;
		case 4 :
			p_texture_render_bind(texture, size, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT);
			glRotatef(180, 0, 1, 0);
			glRotatef(-90, 1, 0, 0);
		break;
		case 5 :
			p_texture_render_bind(texture, size, GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT);
			glRotatef(180, 0, 1, 0);
			glRotatef(90, 1, 0, 0);
		break;
	}
//	glClearColor(1.0, 0, 0, 0); /*KILL ME*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glTranslatef(-objpos[0], -objpos[1], -objpos[2]);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(0.01, -0.01, 0.01, -0.01, 0.01, 100.0);
	glViewport(0, 0, size, size);
	glMatrixMode(GL_MODELVIEW);

	for(n = e_ns_get_node_next(0, 0, V_NT_OBJECT); n != NULL; n = e_ns_get_node_next(e_ns_get_node_id(n) + 1, 0, V_NT_OBJECT))
	{
		if(node != n)
		{
			float light[3] = {10, 10, 10};
			glEnable(GL_DEPTH_TEST);
			glPushMatrix();
			o = e_ns_get_custom_data(n, P_ENOUGH_SLOT);
			p_set_light(&o->light, 3, 0, 0);
			p_render_set_transform(n);
			p_render_object(n, FALSE, TRUE);
			p_render_object(n, TRUE, TRUE);
			glPopMatrix();
			glDisable(GL_DEPTH_TEST);
		}	
	}
	p_draw_reflection_flares(objpos);
	glPopMatrix();
	p_texture_render_unbind();
	return texture;
}

void update_object_impostor(ENode *node, double *pos, uint texture)
{
	double matrix[16], objpos[3], point_a[3], point_b[3];
	double fov_v, fov_h;
	static float time = 0;
	PObjImpostor *imp;
	PObject *o;
	uint i;
	o = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
	point_b[0] = pos[0];
	point_b[1] = pos[1] + 1;
	point_b[2] = pos[2];
	e_nso_get_pos_time(node, objpos, 0, 0);
	point_a[0] = objpos[0] - pos[0] * 2;
	point_a[1] = objpos[1] - pos[1] * 2;
	point_a[2] = objpos[2] - pos[2] * 2;
	create_matrix_normalized_c(matrix, pos, objpos, point_b);

	p_texture_render_bind(texture, p_impostor_resolution, GL_TEXTURE_2D);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	time += 0.01;
	reverse_matrix(matrix);
	matrix[0] = -matrix[0];
	matrix[1] = -matrix[1];
	matrix[2] = -matrix[2];
	matrix[4] = -matrix[4];
	matrix[5] = -matrix[5];
	matrix[6] = -matrix[6];
	matrix[8] = -matrix[8];
	matrix[9] = -matrix[9];
	matrix[10] = -matrix[10];
	matrix[12] = 0;
	matrix[13] = 0;
	matrix[14] = 0;
	glPushMatrix();
	glMultMatrixd(matrix);
	glTranslated(-pos[0], -pos[1], -pos[2]);
	glPushMatrix();
	p_set_light(&o->light, 3, 0, 0);
	p_render_set_transform(node);
	glGetDoublev(GL_MODELVIEW_MATRIX, matrix);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	compute_fov(node, matrix, o, &fov_v, &fov_h);
/*	if(fov_v > fov_h)
		fov_h = fov_v;
	fov_v = fov_h;*/
	o->impostor.size = fov_h;
	o->impostor.impostor = fov_v < p_render_impostor_size && fov_h < p_render_impostor_size;
	o->impostor.size = sqrt((objpos[0] - pos[0]) * (objpos[0] - pos[0]) + (objpos[1] - pos[1]) * (objpos[1] - pos[1]) + (objpos[2] - pos[2]) * (objpos[2] - pos[2]));
	glFrustum(-fov_h * 0.1, fov_h * 0.1, -fov_v * 0.1, fov_v * 0.1, 0.05, 100.0);
	glViewport(0, 0, p_impostor_resolution, p_impostor_resolution);
	glMatrixMode(GL_MODELVIEW);

	if(o->impostor.impostor)
	{
		p_render_object(node, FALSE, FALSE);
		p_render_object(node, TRUE, FALSE);
	}
	glPopMatrix();
	glPopMatrix();
	p_texture_render_unbind();

	reverse_matrix(matrix);
	o->impostor.vertex_array[0] = matrix[0] * o->impostor.size * fov_h + matrix[4] * o->impostor.size * fov_v;
	o->impostor.vertex_array[1] = matrix[1] * o->impostor.size * fov_h + matrix[5] * o->impostor.size * fov_v;
	o->impostor.vertex_array[2] = matrix[2] * o->impostor.size * fov_h + matrix[6] * o->impostor.size * fov_v;
	o->impostor.vertex_array[3] = matrix[0] * o->impostor.size * fov_h - matrix[4] * o->impostor.size * fov_v;
	o->impostor.vertex_array[4] = matrix[1] * o->impostor.size * fov_h - matrix[5] * o->impostor.size * fov_v;
	o->impostor.vertex_array[5] = matrix[2] * o->impostor.size * fov_h - matrix[6] * o->impostor.size * fov_v;
	o->impostor.vertex_array[6] = -o->impostor.vertex_array[0];
	o->impostor.vertex_array[7] = -o->impostor.vertex_array[1];
	o->impostor.vertex_array[8] = -o->impostor.vertex_array[2];
	o->impostor.vertex_array[9] = -o->impostor.vertex_array[3];
	o->impostor.vertex_array[10] = -o->impostor.vertex_array[4];
	o->impostor.vertex_array[11] = -o->impostor.vertex_array[5];
	reverse_matrix(matrix);
}

boolean p_render_to_texture_suported(void);
uint p_blur_intermidiet = -1;
uint p_blur_intermidiet2 = -1;

void p_update_object_impostors(void)
{
	static uint node_id = 0, stage = 0;
	uint counter = 0;
	ENode *node;
	PObject *o;
	double matrix[16];

	if(!p_render_to_texture_suported())
		return;
	for(node = e_ns_get_node_next(node_id, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
	{		
		o = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
		if(o != NULL && o->mesh_count != 0 && o->meshes[0] != NULL)
		{
			o->impostor.timer += o->impostor.priority;
			if(o->impostor.timer > 1)
			{
				o->impostor.timer = 0;
				if(o->impostor.texture == -1)
					o->impostor.texture = p_create_renderable_texture(p_impostor_resolution, p_th_get_hdri_token(TRUE));
				if(stage == 0)
					update_object_impostor(node, p_lod_get_view_pos(), o->impostor.texture);
				else if(stage > 0 && stage < 7)
					o->impostor.environment = update_object_environment(node, o->impostor.environment, 256, stage - 1);
			/*	else if(stage < 13)
					o->impostor.blur = p_blur_object_environment(node, o->impostor.blur, o->impostor.environment, 256, stage - 7);
			
				else if(stage < 13)
					intermidiet = p_blur_object_environment(node, intermidiet, o->impostor.environment, 32, stage - 7);
				else if(stage < (13 + 6))
					o->impostor.blur = p_blur_object_environment(node, o->impostor.blur, intermidiet, 32, stage - 13);
*/
				else if(stage < 8)
				{
					p_blur_intermidiet = p_blur_object_environment(node, p_blur_intermidiet, o->impostor.environment, 64, 0);
					p_blur_intermidiet = p_blur_object_environment(node, p_blur_intermidiet, o->impostor.environment, 64, 1);
					p_blur_intermidiet = p_blur_object_environment(node, p_blur_intermidiet, o->impostor.environment, 64, 2);
					p_blur_intermidiet = p_blur_object_environment(node, p_blur_intermidiet, o->impostor.environment, 64, 3);
					p_blur_intermidiet = p_blur_object_environment(node, p_blur_intermidiet, o->impostor.environment, 64, 4);
					p_blur_intermidiet = p_blur_object_environment(node, p_blur_intermidiet, o->impostor.environment, 64, 5);
					p_blur_intermidiet2 = p_blur_object_environment(node, p_blur_intermidiet2, p_blur_intermidiet, 64, 0);
					p_blur_intermidiet2 = p_blur_object_environment(node, p_blur_intermidiet2, p_blur_intermidiet, 64, 1);
					p_blur_intermidiet2 = p_blur_object_environment(node, p_blur_intermidiet2, p_blur_intermidiet, 64, 2);
					p_blur_intermidiet2 = p_blur_object_environment(node, p_blur_intermidiet2, p_blur_intermidiet, 64, 3);
					p_blur_intermidiet2 = p_blur_object_environment(node, p_blur_intermidiet2, p_blur_intermidiet, 64, 4);
					p_blur_intermidiet2 = p_blur_object_environment(node, p_blur_intermidiet2, p_blur_intermidiet, 64, 5);
					o->impostor.blur = p_blur_object_environment(node, o->impostor.blur, p_blur_intermidiet2, 64, 0);
					o->impostor.blur = p_blur_object_environment(node, o->impostor.blur, p_blur_intermidiet2, 64, 1);
					o->impostor.blur = p_blur_object_environment(node, o->impostor.blur, p_blur_intermidiet2, 64, 2);
					o->impostor.blur = p_blur_object_environment(node, o->impostor.blur, p_blur_intermidiet2, 64, 3);
					o->impostor.blur = p_blur_object_environment(node, o->impostor.blur, p_blur_intermidiet2, 64, 4);
					o->impostor.blur = p_blur_object_environment(node, o->impostor.blur, p_blur_intermidiet2, 64, 5);
				}

	/*			else if(stage < 13)
					intermidiet = p_blur_object_environment(node, intermidiet, o->impostor.environment, 32, stage - 7);
				else if(stage < (13 + 6))
					intermidiet2 = p_blur_object_environment(node, intermidiet2, intermidiet, 32, stage - (7 + 6));
				else if(stage < (13 + 6 + 6))
					o->impostor.blur = p_blur_object_environment(node, o->impostor.blur, intermidiet2, 32, stage - (7 + 6 + 6));
			*/
				else
				{
					uint i;
				/*	intermidiet2 = o->impostor.environment;
					o->impostor.environment = intermidiet;
					intermidiet = intermidiet2;
					o->impostor.blur = o->impostor.environment;
					o->impostor.blur = o->impostor.environment;
			*/		node_id = e_ns_get_node_id(node) + 1;
					stage = 0;
					return;
				}
				stage++;
				return;
			}
		}
		node_id = e_ns_get_node_id(node) + 1;
		if(counter++ > 10)
			return;
	}
	node_id = 0;
}

boolean p_draw_object_as_impostor(ENode *node)
{
	PObject *o;
	o = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
	return (o != NULL && o->impostor.texture != -1 && o->impostor.impostor == TRUE);
}

void p_draw_object_impostor(ENode *node)
{
	float color_array[8] = {1, 1, 1, 0, 0, 0, 0, 1};
	float vertex_array[12] = {-1, -1, 0, 1, -1, 0, 1, 1, 0, -1, 1, 0};
	double *matrix, pos[3];
	PObject *o;

	matrix = p_lod_get_view_matrix();
	o = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
	if(o == NULL || o->impostor.texture == -1 || o->impostor.impostor != TRUE)
		return;
	e_nso_get_pos_time(node, pos, 0, 0);
	glPushMatrix();
	glTranslated(pos[0], pos[1], pos[2]);
//	sui_set_blend_gl(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_BLEND);
//	sui_set_texture2D_array_gl(color_array, 4, 2, o->impostor.texture);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0 , color_array);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, o->impostor.texture);

	glColor4f(1, 1, 1, 1);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT , 0, o->impostor.vertex_array);
	glDrawArrays(GL_QUADS, 0, 4);
//	glDisable(GL_TEXTURE);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
//	glBlendFunc(GL_ZERO, GL_ONE);
//	glDisable(GL_BLEND);
	glPopMatrix();
}

void p_draw_all_impostors(void)
{
	ENode *node;
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	for(node = e_ns_get_node_next(0, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
		p_draw_object_impostor(node);
	glDisable(GL_ALPHA_TEST);
	glBlendFunc(GL_ONE, GL_ZERO);
}

void p_impostor_context_update(void)
{
	ENode *node;
	PObject *o;
	p_blur_intermidiet = -1;
	p_blur_intermidiet2 = -1;
	for(node = e_ns_get_node_next(0, 0, V_NT_OBJECT); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_OBJECT))
	{		
		o = e_ns_get_custom_data(node, P_ENOUGH_SLOT);
		if(o != NULL)
		{
			o->impostor.texture = -1;
			o->impostor.environment = -1;
			o->impostor.blur = -1;
		}
	}
}
