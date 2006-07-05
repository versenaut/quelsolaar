#include <stdio.h>
#include <stdlib.h>


#include <math.h>


typedef struct{
	float x;
	float y;
	float z;
}point;


void clear_matrix(double *matrix)
{
	matrix[0] = 1;
	matrix[1] = 0;
	matrix[2] = 0;
	matrix[3] = 0;
	matrix[4] = 0;
	matrix[5] = 1;
	matrix[6] = 0;
	matrix[7] = 0;
	matrix[8] = 0;
	matrix[9] = 0;
	matrix[10] = 1;
	matrix[11] = 0;
	matrix[12] = 0;
	matrix[13] = 0;
	matrix[14] = 0;
	matrix[15] = 1;
}

void clear_matrix_edge(double *matrix)
{
	matrix[3] = 0;
	matrix[7] = 0;
	matrix[11] = 0;
	matrix[12] = 0;
	matrix[13] = 0;
	matrix[14] = 0;
	matrix[15] = 1;
}

void negate_matrix(double *matrix)
{
	double swap;

	swap = matrix[1];
	matrix[1] = matrix[4];
	matrix[4] = swap;

	swap = matrix[2];
	matrix[2] = matrix[8];
	matrix[8] = swap;

	swap = matrix[3];
	matrix[3] = matrix[12];
	matrix[12] = swap;

	swap = matrix[6];
	matrix[6] = matrix[9];
	matrix[9] = swap;

	swap = matrix[7];
	matrix[7] = matrix[13];
	matrix[13] = swap;

	swap = matrix[11];
	matrix[11] = matrix[14];
	matrix[14] = swap;
}

void reverse_matrix(double *matrix)
{
	double swap;

	swap = matrix[1];
	matrix[1] = matrix[4];
	matrix[4] = swap;

	swap = matrix[2];
	matrix[2] = matrix[8];
	matrix[8] = swap;


	swap = matrix[6];
	matrix[6] = matrix[9];
	matrix[9] = swap;

	matrix[3] = -matrix[3];
	matrix[7] = -matrix[7];
	matrix[11] = -matrix[11];
}


void point_threw_matrix4(double *matrix, double *pos_x, double *pos_y, double *pos_z, double *pos_w)
{
	double x, y, z, w;
	x = *pos_x;
	y = *pos_y;
	z = *pos_z;
	w = *pos_w;

	*pos_x = (matrix[0] * x) + (matrix[4] * y) + (matrix[8] * z) + (matrix[12] * w);
	*pos_y = (matrix[1] * x) + (matrix[5] * y) + (matrix[9] * z) + (matrix[13] * w);
	*pos_z = (matrix[2] * x) + (matrix[6] * y) + (matrix[10] * z) + (matrix[14] * w);
	*pos_w = (matrix[3] * x) + (matrix[7] * y) + (matrix[11] * z) + (matrix[15] * w);
}
void point_threw_matrix3(double *matrix, double *pos_x, double *pos_y, double *pos_z)
{
	double x, y, z;
	x = *pos_x;
	y = *pos_y;
	z = *pos_z;

	*pos_x = (matrix[0] * x) + (matrix[4] * y) + (matrix[8] * z) + matrix[12];
	*pos_y = (matrix[1] * x) + (matrix[5] * y) + (matrix[9] * z) + matrix[13];
	*pos_z = (matrix[2] * x) + (matrix[6] * y) + (matrix[10] * z) + matrix[14];
}
void matrix_multiply(const double *a,const  double *b, double *output)
{
	output[0] = (b[0] * a[0]) + (b[1] * a[4]) + (b[2] * a[8]) + (b[3] * a[12]);
	output[4] = (b[4] * a[0]) + (b[5] * a[4]) + (b[6] * a[8]) + (b[7] * a[12]);
	output[8] = (b[8] * a[0]) + (b[9] * a[4]) + (b[10] * a[8]) + (b[11] * a[12]);
	output[12] = (b[12] * a[0]) + (b[13] * a[4]) + (b[14] * a[8]) + (b[15] * a[12]);

	output[1] = (b[0] * a[1]) + (b[1] * a[5]) + (b[2] * a[9]) + (b[3] * a[13]);
	output[5] = (b[4] * a[1]) + (b[5] * a[5]) + (b[6] * a[9]) + (b[7] * a[13]);
	output[9] = (b[8] * a[1]) + (b[9] * a[5]) + (b[10] * a[9]) + (b[11] * a[13]);
	output[13] = (b[12] * a[1]) + (b[13] * a[5]) + (b[14] * a[9]) + (b[15] * a[13]);

	output[2] = (b[0] * a[2]) + (b[1] * a[6]) + (b[2] * a[10]) + (b[3] * a[14]);
	output[6] = (b[4] * a[2]) + (b[5] * a[6]) + (b[6] * a[10]) + (b[7] * a[14]);
	output[10] = (b[8] * a[2]) + (b[9] * a[6]) + (b[10] * a[10]) + (b[11] * a[14]);
	output[14] = (b[12] * a[2]) + (b[13] * a[6]) + (b[14] * a[10]) + (b[15] * a[14]);

	output[3] = (b[0] * a[3]) + (b[1] * a[7]) + (b[2] * a[11]) + (b[3] * a[15]);
	output[7] = (b[4] * a[3]) + (b[5] * a[7]) + (b[6] * a[11]) + (b[7] * a[15]);
	output[11] = (b[8] * a[3]) + (b[9] * a[7]) + (b[10] * a[11]) + (b[11] * a[15]);
	output[15] = (b[12] * a[3]) + (b[13] * a[7]) + (b[14] * a[11]) + (b[15] * a[15]);
}

void matrix_print(const double *matrix)
{
	printf("[%f %f %f %f]\n", matrix[0], matrix[1], matrix[2], matrix[3]);
	printf("[%f %f %f %f]\n", matrix[4], matrix[5], matrix[6], matrix[7]);
	printf("[%f %f %f %f]\n", matrix[8], matrix[9], matrix[10], matrix[11]);
	printf("[%f %f %f %f]\n", matrix[12], matrix[13], matrix[14], matrix[15]);
}
/*
glLoadMatrixf(const float *m)
*/
void transform_matrix(double *matrix, double x, double y, double z)
{
	matrix[12] += x;
	matrix[13] += y;
	matrix[14] += z;
}
void scale_matrix(double *matrix, double x, double y, double z)
{
	matrix[0] *= x;
	matrix[5] *= y;
	matrix[10] *= z;
}

void matrix_rotate_x(double *a, double degree)
{
	double temp;

	temp = a[4];
	a[4] = (cos(degree) * a[4]) + (sin(degree) * a[8]);
	a[8] = (-sin(degree) * temp) + (cos(degree) * a[8]);

	temp = a[5];
	a[5] = (cos(degree) * a[5]) + (sin(degree) * a[9]);
	a[9] = (-sin(degree) * temp) + (cos(degree) * a[9]);

	temp = a[6];
	a[6] = (cos(degree) * a[6]) + (sin(degree) * a[10]);
	a[10] = (-sin(degree) * temp) + (cos(degree) * a[10]);

	temp = a[7];
	a[7] = (cos(degree) * a[7]) + (sin(degree) * a[11]);
	a[11] = (-sin(degree) * temp) + (cos(degree) * a[11]);
}

void matrix_rotate_y(double *a, double degree)
{
	double temp;

	temp = a[0];
	a[0] = (cos(degree) * a[0]) + (-sin(degree) * a[8]);
	a[8] = (sin(degree) * temp) + (cos(degree) * a[8]);

	temp = a[1];
	a[1] = (cos(degree) * a[1]) + (-sin(degree) * a[9]);
	a[9] = (sin(degree) * temp) + (cos(degree) * a[9]);

	temp = a[2];
	a[2] = (cos(degree) * a[2]) + (-sin(degree) * a[10]);
	a[10] = (sin(degree) * temp) + (cos(degree) * a[10]);

	temp = a[3];
	a[3] = (cos(degree) * a[3]) + (-sin(degree) * a[11]);
	a[11] = (sin(degree) * temp) + (cos(degree) * a[11]);

}


void matrix_rotate_z(double *a, double degree)
{
	double temp;

	temp = a[0];
	a[0] = (cos(degree) * a[0]) + (sin(degree) * a[4]);
	a[4] = (-sin(degree) * temp) + (cos(degree) * a[4]);

	temp = a[1];
	a[1] = (cos(degree) * a[1]) + (sin(degree) * a[5]);
	a[5] = (-sin(degree) * temp) + (cos(degree) * a[5]);

	temp = a[2];
	a[2] = (cos(degree) * a[2]) + (sin(degree) * a[6]);
	a[6] = (-sin(degree) * temp) + (cos(degree) * a[6]);

	temp = a[3];
	a[3] = (cos(degree) * a[3]) + (sin(degree) * a[7]);
	a[7] = (-sin(degree) * temp) + (cos(degree) * a[7]);
}

void matrix_normalize_size(double *matrix)
{
	double scalar;

	scalar = sqrt(matrix[0] * matrix[0] + matrix[4] * matrix[4] + matrix[8] * matrix[8]);
	matrix[0] *= scalar; 
	matrix[4] *= scalar; 
	matrix[8] *= scalar;
	scalar = sqrt(matrix[1] * matrix[1] + matrix[5] * matrix[5] + matrix[9] * matrix[9]);
	matrix[1] *= scalar; 
	matrix[5] *= scalar; 
	matrix[9] *= scalar;
	scalar = sqrt(matrix[2] * matrix[2] + matrix[6] * matrix[6] + matrix[10] * matrix[10]);
	matrix[2] *= scalar; 
	matrix[6] *= scalar; 
	matrix[10] *= scalar;
}

void matrix_rotate_zo(double *matrix, double degree)
{
	matrix[0] = cos(degree);
	matrix[1] = sin(degree);
	matrix[2] = 0;
	matrix[3] = 0;
	matrix[4] = -sin(degree);
	matrix[5] = cos(degree);
	matrix[6] = 0;
	matrix[7] = 0;
	matrix[8] = 0;
	matrix[9] = 0;
	matrix[10] = 1;
	matrix[11] = 0;	
	matrix[12] = 0;
	matrix[13] = 0;
	matrix[14] = 0;
	matrix[15] = 1;
}

void matrix_rotate_yo(double *matrix, double degree)
{
	matrix[0] = cos(degree);
	matrix[1] = 0;
	matrix[2] = -sin(degree);
	matrix[3] = 0;
	matrix[4] = 0;
	matrix[5] = 1;
	matrix[6] = 0;
	matrix[7] = 0;
	matrix[8] = sin(degree);
	matrix[9] = 0;
	matrix[10] = cos(degree);
	matrix[11] = 0;	
	matrix[12] = 0;
	matrix[13] = 0;
	matrix[14] = 0;
	matrix[15] = 1;
}


void create_matrix_normalized(double *matrix, double *origo, double *point_a, double *point_b)
{
	double r;

	point_a[0] -= origo[0];
	point_a[1] -= origo[1];
	point_a[2] -= origo[2];
	r = sqrt(point_a[0] * point_a[0] + point_a[1] * point_a[1] + point_a[2] * point_a[2]);
	matrix[8] = point_a[0] / r;
	matrix[9] = point_a[1] / r;
	matrix[10] = point_a[2] / r;
	matrix[12] = origo[0];
	matrix[13] = origo[1];
	matrix[14] = origo[2];
	point_b[0] -= origo[0];
	point_b[1] -= origo[1];
	point_b[2] -= origo[2];
	matrix[4] = matrix[9] * point_b[2] - matrix[10] * point_b[1];
	matrix[5] = matrix[10] * point_b[0] - matrix[8] * point_b[2];
	matrix[6] = matrix[8] * point_b[1] - matrix[9] * point_b[0];
	r = sqrt(matrix[4] * matrix[4] + matrix[5] * matrix[5] + matrix[6] * matrix[6]);
	matrix[4] = matrix[4] / r;
	matrix[5] = matrix[5] / r;
	matrix[6] = matrix[6] / r;
	matrix[0] = matrix[9] * matrix[6] - matrix[10] * matrix[5];
	matrix[1] = matrix[10] * matrix[4] - matrix[8] * matrix[6];
	matrix[2] = matrix[8] * matrix[5] - matrix[9] * matrix[4];
	r = sqrt(matrix[0] * matrix[0] + matrix[1] * matrix[1] + matrix[2] * matrix[2]);
	matrix[0] = matrix[0] / r;
	matrix[1] = matrix[1] / r;
	matrix[2] = matrix[2] / r;
	matrix[3] = 0;
	matrix[7] = 0;
	matrix[11] = 0;
	matrix[15] = 1;
}



void create_matrix_normalized_b(double *matrix, double *origo, double *point_x, double *point_y)
{
	double r;

	point_x[0] -= origo[0];
	point_x[1] -= origo[1];
	point_x[2] -= origo[2];
	r = sqrt(point_x[0] * point_x[0] + point_x[1] * point_x[1] + point_x[2] * point_x[2]);
	matrix[0] = point_x[0] / r;
	matrix[1] = point_x[1] / r;
	matrix[2] = point_x[2] / r;
	matrix[12] = 0;
	matrix[13] = 0;
	matrix[14] = 0;
	point_y[0] -= origo[0];
	point_y[1] -= origo[1];
	point_y[2] -= origo[2];
	r = matrix[0] * point_y[0] + matrix[1] * point_y[1] + matrix[2] * point_y[2];
	matrix[4] = point_y[0] - matrix[0] * r;
	matrix[5] = point_y[1] - matrix[1] * r;
	matrix[6] = point_y[2] - matrix[2] * r;
	r = sqrt(matrix[4] * matrix[4] + matrix[5] * matrix[5] + matrix[6] * matrix[6]);
	matrix[4] = matrix[4] / r;
	matrix[5] = matrix[5] / r;
	matrix[6] = matrix[6] / r;
	matrix[8] = matrix[1] * matrix[6] - matrix[2] * matrix[5];
	matrix[9] = matrix[2] * matrix[4] - matrix[0] * matrix[6];
	matrix[10] = matrix[0] * matrix[5] - matrix[1] * matrix[4];
	r = sqrt(matrix[8] * matrix[8] + matrix[9] * matrix[9] + matrix[10] * matrix[10]);
	matrix[8] = matrix[8] / r;
	matrix[9] = matrix[9] / r;
	matrix[10] = matrix[10] / r;
	matrix[3] = origo[0];
	matrix[7] = origo[1];
	matrix[11] = origo[2];
	matrix[15] = 1;
}

void create_matrix_normalized_c(double *matrix, double *origo, double *point_a, double *point_b)
{
	double r;
	point_a[0] -= origo[0];
	point_a[1] -= origo[1];
	point_a[2] -= origo[2];
	r = sqrt(point_a[0] * point_a[0] + point_a[1] * point_a[1] + point_a[2] * point_a[2]);
	matrix[8] = point_a[0] / r;
	matrix[9] = point_a[1] / r;
	matrix[10] = point_a[2] / r;
	matrix[12] = origo[0];
	matrix[13] = origo[1];
	matrix[14] = origo[2];
	point_b[0] -= origo[0];
	point_b[1] -= origo[1];
	point_b[2] -= origo[2];
	matrix[0] = matrix[9] * point_b[2] - matrix[10] * point_b[1];
	matrix[1] = matrix[10] * point_b[0] - matrix[8] * point_b[2];
	matrix[2] = matrix[8] * point_b[1] - matrix[9] * point_b[0];
	r = sqrt(matrix[0] * matrix[0] + matrix[1] * matrix[1] + matrix[2] * matrix[2]);
	matrix[0] = matrix[0] / r;
	matrix[1] = matrix[1] / r;
	matrix[2] = matrix[2] / r;
	matrix[4] = matrix[9] * matrix[2] - matrix[10] * matrix[1];
	matrix[5] = matrix[10] * matrix[0] - matrix[8] * matrix[2];
	matrix[6] = matrix[8] * matrix[1] - matrix[9] * matrix[0];
	r = sqrt(matrix[4] * matrix[4] + matrix[5] * matrix[5] + matrix[6] * matrix[6]);
	matrix[4] = matrix[4] / r;
	matrix[5] = matrix[5] / r;
	matrix[6] = matrix[6] / r;
	matrix[3] = 0;
	matrix[7] = 0;
	matrix[11] = 0;
	matrix[15] = 1;
}
