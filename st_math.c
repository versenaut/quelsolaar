#include <math.h>

void dv2_copy(double *a, double *b)
{
	a[0] = b[0];
	a[1] = b[1];
}

void dv3_copy(double *a, double *b)
{
	a[0] = b[0];
	a[1] = b[1];
	a[2] = b[2];
}


double dv2_dot(double *a, double *b)
{
	return a[0] * b[0] + a[1] * b[1];
}

double dv3_dot(double *a, double *b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

double dv2_length(double *a)
{
	return sqrt(a[0] * a[0] + a[1] * a[1]);
}

double dv3_length(double *a)
{
	return sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
}

void dv2o_sub(double *output, double *a, double *b)
{
	output[0] = a[0] - b[0];
	output[1] = a[1] - b[1];
}

void dv3o_sub(double *output, double *a, double *b)
{
	output[0] = a[0] - b[0];
	output[1] = a[1] - b[1];
	output[2] = a[2] - b[2];
}

void dv2_sub(double *a, double *b)
{
	a[0] -= b[0];
	a[1] -= b[1];
}

void dv3_sub(double *a, double *b)
{
	a[0] -= b[0];
	a[1] -= b[1];
	a[2] -= b[2];
}

void dv2_add(double *a, double *b)
{
	a[0] += b[0];
	a[1] += b[1];
}

void dv3_add(double *a, double *b)
{
	a[0] += b[0];
	a[1] += b[1];
	a[2] += b[2];
}

void dv2o_add(double *output, double *a, double *b)
{
	output[0] = a[0] - b[0];
	output[1] = a[1] - b[1];
}

void dv3o_add(double *output, double *a, double *b)
{
	output[0] = a[0] - b[0];
	output[1] = a[1] - b[1];
	output[2] = a[2] - b[2];
}

void dv2_mult(double *a, double *b)
{
	a[0] += b[0];
	a[1] += b[1];
}

void dv3_mult(double *a, double *b)
{
	a[0] += b[0];
	a[1] += b[1];
	a[2] += b[2];
}

void dv2o_mult(double *output, double *a, double *b)
{
	output[0] = a[0] * b[0];
	output[1] = a[1] * b[1];
}

void dv3o_mult(double *output, double *a, double *b)
{
	output[0] = a[0] * b[0];
	output[1] = a[1] * b[1];
	output[2] = a[2] * b[2];
}

void dv2_division(double *a, double b)
{
	a[0] /= b;
	a[1] /= b;
	a[2] /= b;
}

void dv3_division(double *a, double b)
{
	a[0] /= b;
	a[1] /= b;
	a[2] /= b;
}

void dv2o_division(double *output, double *a, double b)
{
	output[0] = a[0] / b;
	output[1] = a[1] / b;
}

void dv3o_division(double *output, double *a, double b)
{
	output[0] = a[0] / b;
	output[1] = a[1] / b;
	output[2] = a[2] / b;
}

extern void dv2o_cross(double *output, double *a, double *b);

void dv3o_cross(double *output, double *a, double *b)
{
	output[0] = a[1] * b[2] - a[2] * b[1];
	output[1] = a[2] * b[0] - a[0] * b[2];
	output[2] = a[0] * b[1] - a[1] * b[0];
}

void dv2_3point_cross(double *output, double *origo, double *a, double *b)
{
}

void dv3_3point_cross(double *output, double *origo, double *a, double *b)
{
	output[0] = (a[1] - origo[1]) * (b[2] - origo[2]) - (a[2] - origo[2]) * (b[1] - origo[1]);
	output[1] = (a[2] - origo[2]) * (b[0] - origo[0]) - (a[0] - origo[0]) * (b[2] - origo[2]);
	output[2] = (a[0] - origo[0]) * (b[1] - origo[1]) - (a[1] - origo[1]) * (b[0] - origo[0]);
}

void dv2_normalize(double *a)
{
	double r;
	r = sqrt(a[0] * a[0] + a[1] * a[1]);
	a[0] /= r;
	a[1] /= r;
}

void dv3_normalize(double *a)
{
	double r;
	r = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
	a[0] /= r;
	a[1] /= r;
	a[2] /= r;
}
