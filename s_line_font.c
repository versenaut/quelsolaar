#include "seduce.h"

void sui_draw_letter(uint8 letter, float red, float green, float blue, float alpha)
{
	static float font_33[] = {(float)0.000000, (float)0.000000, (float)0.000000, (float)-0.060000, (float)0.000000, (float)0.080000, (float)0.000000, (float)0.420000};
	static float font_34[] = {(float)0.080000, (float)0.240000, (float)0.060000, (float)0.240000, (float)0.020000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.100000, (float)0.300000, (float)0.080000, (float)0.240000, (float)0.040000, (float)0.300000, (float)0.020000, (float)0.240000};
	static float font_35[] = {(float)0.220000, (float)0.080000, (float)0.000000, (float)0.080000, (float)0.220000, (float)0.160000, (float)0.000000, (float)0.160000, (float)0.040000, (float)0.000000, (float)0.080000, (float)0.240000, (float)0.140000, (float)0.000000, (float)0.180000, (float)0.240000};
	static float font_36[] = {(float)0.120000, (float)0.120000, (float)0.180000, (float)0.111962, (float)0.180000, (float)0.111962, (float)0.223923, (float)0.090000, (float)0.223923, (float)0.090000, (float)0.240000, (float)0.060000, (float)0.160000, (float)-0.040000, (float)0.160000, (float)0.280000
			, (float)0.120000, (float)0.120000, (float)0.060000, (float)0.128038, (float)0.060000, (float)0.128038, (float)0.016077, (float)0.150000, (float)0.016077, (float)0.150000, (float)0.000000, (float)0.180000, (float)0.080000, (float)0.240000, (float)0.040000, (float)0.231962
			, (float)0.040000, (float)0.231962, (float)0.010718, (float)0.210000, (float)0.010718, (float)0.210000, (float)0.000000, (float)0.180000, (float)0.080000, (float)0.000000, (float)0.080000, (float)-0.040000, (float)0.080000, (float)0.280000, (float)0.080000, (float)-0.040000
			, (float)0.160000, (float)0.000000, (float)0.200000, (float)0.008038, (float)0.200000, (float)0.008038, (float)0.229282, (float)0.030000, (float)0.229282, (float)0.030000, (float)0.240000, (float)0.060000, (float)0.080000, (float)0.000000, (float)0.040000, (float)0.008038
			, (float)0.040000, (float)0.008038, (float)0.010718, (float)0.030000, (float)0.010718, (float)0.030000, (float)0.000000, (float)0.060000, (float)0.160000, (float)0.240000, (float)0.200000, (float)0.231962, (float)0.200000, (float)0.231962, (float)0.229282, (float)0.210000
			, (float)0.229282, (float)0.210000, (float)0.240000, (float)0.180000, (float)0.160000, (float)0.240000, (float)0.080000, (float)0.240000, (float)0.080000, (float)0.000000, (float)0.160000, (float)0.000000};
	static float font_37[] = {(float)0.200000, (float)0.100000, (float)0.220000, (float)0.094641, (float)0.220000, (float)0.094641, (float)0.234641, (float)0.080000, (float)0.234641, (float)0.080000, (float)0.240000, (float)0.060000, (float)0.180000, (float)0.000000, (float)0.160000, (float)0.005359
			, (float)0.160000, (float)0.005359, (float)0.145359, (float)0.020000, (float)0.145359, (float)0.020000, (float)0.140000, (float)0.040000, (float)0.000000, (float)0.000000, (float)0.240000, (float)0.240000, (float)0.060000, (float)0.240000, (float)0.030000, (float)0.231962
			, (float)0.030000, (float)0.231962, (float)0.008038, (float)0.210000, (float)0.008038, (float)0.210000, (float)0.000000, (float)0.180000, (float)0.040000, (float)0.140000, (float)0.020000, (float)0.145359, (float)0.020000, (float)0.145359, (float)0.005359, (float)0.160000
			, (float)0.005359, (float)0.160000, (float)0.000000, (float)0.180000, (float)0.040000, (float)0.140000, (float)0.070000, (float)0.148038, (float)0.070000, (float)0.148038, (float)0.091962, (float)0.170000, (float)0.091962, (float)0.170000, (float)0.100000, (float)0.200000
			, (float)0.060000, (float)0.240000, (float)0.080000, (float)0.234641, (float)0.080000, (float)0.234641, (float)0.094641, (float)0.220000, (float)0.094641, (float)0.220000, (float)0.100000, (float)0.200000, (float)0.200000, (float)0.100000, (float)0.170000, (float)0.091962
			, (float)0.170000, (float)0.091962, (float)0.148038, (float)0.070000, (float)0.148038, (float)0.070000, (float)0.140000, (float)0.040000, (float)0.180000, (float)0.000000, (float)0.210000, (float)0.008038, (float)0.210000, (float)0.008038, (float)0.231962, (float)0.030000
			, (float)0.231962, (float)0.030000, (float)0.240000, (float)0.060000};
	static float font_38[] = {(float)0.120000, (float)0.140000, (float)0.060000, (float)0.129282, (float)0.060000, (float)0.129282, (float)0.016077, (float)0.100000, (float)0.016077, (float)0.100000, (float)0.000000, (float)0.060000, (float)0.140000, (float)0.240000, (float)0.160000, (float)0.234641
			, (float)0.160000, (float)0.234641, (float)0.174641, (float)0.220000, (float)0.174641, (float)0.220000, (float)0.180000, (float)0.200000, (float)0.200000, (float)0.060000, (float)0.200000, (float)-0.040000, (float)0.240000, (float)0.000000, (float)0.120000, (float)0.000000
			, (float)0.120000, (float)0.000000, (float)0.060000, (float)0.008038, (float)0.060000, (float)0.008038, (float)0.016077, (float)0.030000, (float)0.016077, (float)0.030000, (float)0.000000, (float)0.060000, (float)0.140000, (float)0.140000, (float)0.170000, (float)0.129282
			, (float)0.170000, (float)0.129282, (float)0.191962, (float)0.100000, (float)0.191962, (float)0.100000, (float)0.200000, (float)0.060000, (float)0.120000, (float)0.140000, (float)0.150000, (float)0.148038, (float)0.150000, (float)0.148038, (float)0.171962, (float)0.170000
			, (float)0.171962, (float)0.170000, (float)0.180000, (float)0.200000, (float)0.140000, (float)0.140000, (float)0.100000, (float)0.145359, (float)0.100000, (float)0.145359, (float)0.070718, (float)0.160000, (float)0.070718, (float)0.160000, (float)0.060000, (float)0.180000
			, (float)0.140000, (float)0.240000, (float)0.100000, (float)0.231962, (float)0.100000, (float)0.231962, (float)0.070718, (float)0.210000, (float)0.070718, (float)0.210000, (float)0.060000, (float)0.180000};
	static float font_39[] = {(float)0.020000, (float)0.300000, (float)0.010000, (float)0.297321, (float)0.010000, (float)0.297321, (float)0.002679, (float)0.290000, (float)0.002679, (float)0.290000, (float)0.000000, (float)0.280000, (float)0.020000, (float)0.300000, (float)0.040000, (float)0.300000
			, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.280000};
	static float font_40[] = {(float)0.060000, (float)0.280000, (float)0.030000, (float)0.258564, (float)0.030000, (float)0.258564, (float)0.008038, (float)0.200000, (float)0.008038, (float)0.200000, (float)0.000000, (float)0.120000, (float)0.060000, (float)-0.040000, (float)0.030000, (float)-0.018564
			, (float)0.030000, (float)-0.018564, (float)0.008038, (float)0.040000, (float)0.008038, (float)0.040000, (float)0.000000, (float)0.120000};
	static float font_41[] = {(float)0.000000, (float)-0.040000, (float)0.030000, (float)-0.018564, (float)0.030000, (float)-0.018564, (float)0.051962, (float)0.040000, (float)0.051962, (float)0.040000, (float)0.060000, (float)0.120000, (float)0.000000, (float)0.280000, (float)0.030000, (float)0.258564
			, (float)0.030000, (float)0.258564, (float)0.051962, (float)0.200000, (float)0.051962, (float)0.200000, (float)0.060000, (float)0.120000};
	static float font_43[] = {(float)0.000000, (float)0.120000, (float)0.240000, (float)0.120000, (float)0.120000, (float)0.240000, (float)0.120000, (float)0.000000};
	static float font_44[] = {(float)0.060000, (float)0.000000, (float)0.030000, (float)-0.010718, (float)0.030000, (float)-0.010718, (float)0.008038, (float)-0.040000, (float)0.008038, (float)-0.040000, (float)0.000000, (float)-0.080000};
	static float font_45[] = {(float)0.000000, (float)0.120000, (float)0.240000, (float)0.120000};
	static float font_46[] = {(float)0.020000, (float)-0.060000, (float)0.010000, (float)-0.057321, (float)0.010000, (float)-0.057321, (float)0.002679, (float)-0.050000, (float)0.002679, (float)-0.050000, (float)0.000000, (float)-0.040000, (float)0.040000, (float)0.000000, (float)0.020000, (float)-0.005359
			, (float)0.020000, (float)-0.005359, (float)0.005359, (float)-0.020000, (float)0.005359, (float)-0.020000, (float)0.000000, (float)-0.040000, (float)0.040000, (float)0.000000, (float)0.050000, (float)-0.002679, (float)0.050000, (float)-0.002679, (float)0.057321, (float)-0.010000
			, (float)0.057321, (float)-0.010000, (float)0.060000, (float)-0.020000, (float)0.020000, (float)-0.060000, (float)0.040000, (float)-0.054641, (float)0.040000, (float)-0.054641, (float)0.054641, (float)-0.040000, (float)0.054641, (float)-0.040000, (float)0.060000, (float)-0.020000};
	static float font_47[] = {(float)0.240000, (float)0.280000, (float)0.000000, (float)-0.040000};
	static float font_48[] = {(float)0.120000, (float)0.320000, (float)0.060000, (float)0.303923, (float)0.060000, (float)0.303923, (float)0.016077, (float)0.260000, (float)0.016077, (float)0.260000, (float)0.000000, (float)0.200000, (float)0.120000, (float)0.320000, (float)0.180000, (float)0.303923
			, (float)0.180000, (float)0.303923, (float)0.223923, (float)0.260000, (float)0.223923, (float)0.260000, (float)0.240000, (float)0.200000, (float)0.120000, (float)0.000000, (float)0.060000, (float)0.016077, (float)0.060000, (float)0.016077, (float)0.016077, (float)0.060000
			, (float)0.016077, (float)0.060000, (float)0.000000, (float)0.120000, (float)0.120000, (float)0.000000, (float)0.180000, (float)0.016077, (float)0.180000, (float)0.016077, (float)0.223923, (float)0.060000, (float)0.223923, (float)0.060000, (float)0.240000, (float)0.120000
			, (float)0.000000, (float)0.200000, (float)0.000000, (float)0.120000, (float)0.240000, (float)0.120000, (float)0.240000, (float)0.200000};
	static float font_49[] = {(float)0.100000, (float)0.320000, (float)0.100000, (float)0.000000, (float)0.000000, (float)0.240000, (float)0.050000, (float)0.250718, (float)0.050000, (float)0.250718, (float)0.086603, (float)0.280000, (float)0.086603, (float)0.280000, (float)0.100000, (float)0.320000};
	static float font_50[] = {(float)0.240000, (float)0.000000, (float)0.000000, (float)0.000000, (float)0.120000, (float)0.320000, (float)0.060000, (float)0.309282, (float)0.060000, (float)0.309282, (float)0.016077, (float)0.280000, (float)0.016077, (float)0.280000, (float)0.000000, (float)0.240000
			, (float)0.120000, (float)0.320000, (float)0.180000, (float)0.309282, (float)0.180000, (float)0.309282, (float)0.223923, (float)0.280000, (float)0.223923, (float)0.280000, (float)0.240000, (float)0.240000, (float)0.120000, (float)0.160000, (float)0.180000, (float)0.170718
			, (float)0.180000, (float)0.170718, (float)0.223923, (float)0.200000, (float)0.223923, (float)0.200000, (float)0.240000, (float)0.240000, (float)0.120000, (float)0.160000, (float)0.060000, (float)0.138564, (float)0.060000, (float)0.138564, (float)0.016077, (float)0.080000
			, (float)0.016077, (float)0.080000, (float)0.000000, (float)0.000000};
	static float font_51[] = {(float)0.120000, (float)0.320000, (float)0.170000, (float)0.309282, (float)0.170000, (float)0.309282, (float)0.206603, (float)0.280000, (float)0.206603, (float)0.280000, (float)0.220000, (float)0.240000, (float)0.120000, (float)0.320000, (float)0.070000, (float)0.309282
			, (float)0.070000, (float)0.309282, (float)0.033397, (float)0.280000, (float)0.033397, (float)0.280000, (float)0.020000, (float)0.240000, (float)0.120000, (float)0.000000, (float)0.180000, (float)0.013397, (float)0.180000, (float)0.013397, (float)0.223923, (float)0.050000
			, (float)0.223923, (float)0.050000, (float)0.240000, (float)0.100000, (float)0.120000, (float)0.000000, (float)0.060000, (float)0.013397, (float)0.060000, (float)0.013397, (float)0.016077, (float)0.050000, (float)0.016077, (float)0.050000, (float)0.000000, (float)0.100000
			, (float)0.180000, (float)0.180000, (float)0.210000, (float)0.169282, (float)0.210000, (float)0.169282, (float)0.231962, (float)0.140000, (float)0.231962, (float)0.140000, (float)0.240000, (float)0.100000, (float)0.180000, (float)0.180000, (float)0.200000, (float)0.188038
			, (float)0.200000, (float)0.188038, (float)0.214641, (float)0.210000, (float)0.214641, (float)0.210000, (float)0.220000, (float)0.240000, (float)0.180000, (float)0.180000, (float)0.120000, (float)0.180000};
	static float font_52[] = {(float)0.180000, (float)0.000000, (float)0.180000, (float)0.320000, (float)0.240000, (float)0.060000, (float)0.000000, (float)0.060000, (float)0.000000, (float)0.060000, (float)0.180000, (float)0.320000};
	static float font_53[] = {(float)0.000000, (float)0.320000, (float)0.240000, (float)0.320000, (float)0.100000, (float)0.000000, (float)0.140000, (float)0.000000, (float)0.000000, (float)0.220000, (float)0.000000, (float)0.320000, (float)0.120000, (float)0.220000, (float)0.180000, (float)0.206603
			, (float)0.180000, (float)0.206603, (float)0.223923, (float)0.170000, (float)0.223923, (float)0.170000, (float)0.240000, (float)0.120000, (float)0.100000, (float)0.000000, (float)0.050000, (float)0.010718, (float)0.050000, (float)0.010718, (float)0.013397, (float)0.040000
			, (float)0.013397, (float)0.040000, (float)0.000000, (float)0.080000, (float)0.140000, (float)0.000000, (float)0.190000, (float)0.016077, (float)0.190000, (float)0.016077, (float)0.226603, (float)0.060000, (float)0.226603, (float)0.060000, (float)0.240000, (float)0.120000
			, (float)0.120000, (float)0.220000, (float)0.000000, (float)0.220000};
	static float font_54[] = {(float)0.240000, (float)0.320000, (float)0.120000, (float)0.293205, (float)0.120000, (float)0.293205, (float)0.032154, (float)0.220000, (float)0.032154, (float)0.220000, (float)0.000000, (float)0.120000, (float)0.120000, (float)0.000000, (float)0.180000, (float)0.016077
			, (float)0.180000, (float)0.016077, (float)0.223923, (float)0.060000, (float)0.223923, (float)0.060000, (float)0.240000, (float)0.120000, (float)0.120000, (float)0.000000, (float)0.060000, (float)0.016077, (float)0.060000, (float)0.016077, (float)0.016077, (float)0.060000
			, (float)0.016077, (float)0.060000, (float)0.000000, (float)0.120000, (float)0.120000, (float)0.220000, (float)0.060000, (float)0.206603, (float)0.060000, (float)0.206603, (float)0.016077, (float)0.170000, (float)0.016077, (float)0.170000, (float)0.000000, (float)0.120000
			, (float)0.120000, (float)0.220000, (float)0.180000, (float)0.206603, (float)0.180000, (float)0.206603, (float)0.223923, (float)0.170000, (float)0.223923, (float)0.170000, (float)0.240000, (float)0.120000};
	static float font_55[] = {(float)0.000000, (float)0.320000, (float)0.240000, (float)0.320000, (float)0.240000, (float)0.320000, (float)0.020000, (float)0.000000, (float)0.060000, (float)0.200000, (float)0.240000, (float)0.200000};
	static float font_56[] = {(float)0.120000, (float)0.000000, (float)0.060000, (float)0.013397, (float)0.060000, (float)0.013397, (float)0.016077, (float)0.050000, (float)0.016077, (float)0.050000, (float)0.000000, (float)0.100000, (float)0.120000, (float)0.200000, (float)0.160000, (float)0.208038
			, (float)0.160000, (float)0.208038, (float)0.189282, (float)0.230000, (float)0.189282, (float)0.230000, (float)0.200000, (float)0.260000, (float)0.120000, (float)0.200000, (float)0.080000, (float)0.208038, (float)0.080000, (float)0.208038, (float)0.050718, (float)0.230000
			, (float)0.050718, (float)0.230000, (float)0.040000, (float)0.260000, (float)0.120000, (float)0.000000, (float)0.180000, (float)0.013397, (float)0.180000, (float)0.013397, (float)0.223923, (float)0.050000, (float)0.223923, (float)0.050000, (float)0.240000, (float)0.100000
			, (float)0.120000, (float)0.320000, (float)0.080000, (float)0.311962, (float)0.080000, (float)0.311962, (float)0.050718, (float)0.290000, (float)0.050718, (float)0.290000, (float)0.040000, (float)0.260000, (float)0.120000, (float)0.320000, (float)0.160000, (float)0.311962
			, (float)0.160000, (float)0.311962, (float)0.189282, (float)0.290000, (float)0.189282, (float)0.290000, (float)0.200000, (float)0.260000, (float)0.120000, (float)0.200000, (float)0.180000, (float)0.186603, (float)0.180000, (float)0.186603, (float)0.223923, (float)0.150000
			, (float)0.223923, (float)0.150000, (float)0.240000, (float)0.100000, (float)0.120000, (float)0.200000, (float)0.060000, (float)0.186603, (float)0.060000, (float)0.186603, (float)0.016077, (float)0.150000, (float)0.016077, (float)0.150000, (float)0.000000, (float)0.100000};
	static float font_57[] = {(float)0.120000, (float)0.320000, (float)0.060000, (float)0.309282, (float)0.060000, (float)0.309282, (float)0.016077, (float)0.280000, (float)0.016077, (float)0.280000, (float)0.000000, (float)0.240000, (float)0.200000, (float)0.320000, (float)0.120000, (float)0.320000
			, (float)0.080000, (float)0.160000, (float)0.040000, (float)0.170718, (float)0.040000, (float)0.170718, (float)0.010718, (float)0.200000, (float)0.010718, (float)0.200000, (float)0.000000, (float)0.240000, (float)0.200000, (float)0.320000, (float)0.200000, (float)0.000000
			, (float)0.120000, (float)0.160000, (float)0.160000, (float)0.170718, (float)0.160000, (float)0.170718, (float)0.189282, (float)0.200000, (float)0.189282, (float)0.200000, (float)0.200000, (float)0.240000, (float)0.080000, (float)0.160000, (float)0.120000, (float)0.160000};
	static float font_58[] = {(float)0.000000, (float)0.080000, (float)0.000000, (float)0.040000, (float)0.000000, (float)0.200000, (float)0.000000, (float)0.160000};
	static float font_59[] = {(float)0.040000, (float)0.200000, (float)0.040000, (float)0.160000, (float)0.040000, (float)0.080000, (float)0.000000, (float)-0.020000};
	static float font_60[] = {(float)0.000000, (float)0.120000, (float)0.240000, (float)0.000000, (float)0.000000, (float)0.120000, (float)0.240000, (float)0.240000};
	static float font_61[] = {(float)0.000000, (float)0.080000, (float)0.160000, (float)0.080000, (float)0.000000, (float)0.160000, (float)0.160000, (float)0.160000};
	static float font_62[] = {(float)0.240000, (float)0.120000, (float)0.000000, (float)0.240000, (float)0.240000, (float)0.120000, (float)0.000000, (float)0.000000};
	static float font_63[] = {(float)0.160000, (float)0.220000, (float)0.130000, (float)0.209282, (float)0.130000, (float)0.209282, (float)0.108038, (float)0.180000, (float)0.108038, (float)0.180000, (float)0.100000, (float)0.140000, (float)0.120000, (float)0.420000, (float)0.060000, (float)0.403923
			, (float)0.060000, (float)0.403923, (float)0.016077, (float)0.360000, (float)0.016077, (float)0.360000, (float)0.000000, (float)0.300000, (float)0.120000, (float)0.420000, (float)0.180000, (float)0.403923, (float)0.180000, (float)0.403923, (float)0.223923, (float)0.360000
			, (float)0.223923, (float)0.360000, (float)0.240000, (float)0.300000, (float)0.160000, (float)0.220000, (float)0.200000, (float)0.230718, (float)0.200000, (float)0.230718, (float)0.229282, (float)0.260000, (float)0.229282, (float)0.260000, (float)0.240000, (float)0.300000
			, (float)0.120000, (float)-0.040000, (float)0.090000, (float)-0.031962, (float)0.090000, (float)-0.031962, (float)0.068038, (float)-0.010000, (float)0.068038, (float)-0.010000, (float)0.060000, (float)0.020000, (float)0.100000, (float)0.060000, (float)0.080000, (float)0.054641
			, (float)0.080000, (float)0.054641, (float)0.065359, (float)0.040000, (float)0.065359, (float)0.040000, (float)0.060000, (float)0.020000, (float)0.100000, (float)0.060000, (float)0.130000, (float)0.051962, (float)0.130000, (float)0.051962, (float)0.151962, (float)0.030000
			, (float)0.151962, (float)0.030000, (float)0.160000, (float)0.000000, (float)0.120000, (float)-0.040000, (float)0.140000, (float)-0.034641, (float)0.140000, (float)-0.034641, (float)0.154641, (float)-0.020000, (float)0.154641, (float)-0.020000, (float)0.160000, (float)-0.000000};
	static float font_64[] = {(float)0.080000, (float)0.040000, (float)0.060000, (float)0.045359, (float)0.060000, (float)0.045359, (float)0.045359, (float)0.060000, (float)0.045359, (float)0.060000, (float)0.040000, (float)0.080000, (float)0.100000, (float)0.000000, (float)0.050000, (float)0.013397
			, (float)0.050000, (float)0.013397, (float)0.013397, (float)0.050000, (float)0.013397, (float)0.050000, (float)0.000000, (float)0.100000, (float)0.100000, (float)0.000000, (float)0.220000, (float)0.000000, (float)0.080000, (float)0.040000, (float)0.130000, (float)0.053397
			, (float)0.130000, (float)0.053397, (float)0.166603, (float)0.090000, (float)0.166603, (float)0.090000, (float)0.180000, (float)0.140000, (float)0.220000, (float)0.180000, (float)0.220000, (float)0.100000, (float)0.140000, (float)0.200000, (float)0.180000, (float)0.200000
			, (float)0.140000, (float)0.240000, (float)0.070000, (float)0.221244, (float)0.070000, (float)0.221244, (float)0.018756, (float)0.170000, (float)0.018756, (float)0.170000, (float)0.000000, (float)0.100000, (float)0.140000, (float)0.240000, (float)0.180000, (float)0.231962
			, (float)0.180000, (float)0.231962, (float)0.209282, (float)0.210000, (float)0.209282, (float)0.210000, (float)0.220000, (float)0.180000, (float)0.140000, (float)0.200000, (float)0.090000, (float)0.183923, (float)0.090000, (float)0.183923, (float)0.053397, (float)0.140000
			, (float)0.053397, (float)0.140000, (float)0.040000, (float)0.080000, (float)0.180000, (float)0.040000, (float)0.180000, (float)0.200000, (float)0.180000, (float)0.040000, (float)0.200000, (float)0.048038, (float)0.200000, (float)0.048038, (float)0.214641, (float)0.070000
			, (float)0.214641, (float)0.070000, (float)0.220000, (float)0.100000};
	static float font_65[] = {(float)0.000000, (float)0.000000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.120000, (float)0.000000, (float)0.120000};
	static float font_66[] = {(float)0.000000, (float)0.240000, (float)0.000000, (float)0.000000, (float)0.200000, (float)0.000000, (float)0.000000, (float)0.000000, (float)0.200000, (float)0.120000, (float)0.240000, (float)0.080000, (float)0.200000, (float)0.240000, (float)0.240000, (float)0.200000
			, (float)0.240000, (float)0.040000, (float)0.200000, (float)0.000000, (float)0.240000, (float)0.160000, (float)0.200000, (float)0.120000, (float)0.000000, (float)0.240000, (float)0.200000, (float)0.240000, (float)0.240000, (float)0.200000, (float)0.240000, (float)0.160000
			, (float)0.240000, (float)0.080000, (float)0.240000, (float)0.040000, (float)0.200000, (float)0.120000, (float)0.000000, (float)0.120000};
	static float font_67[] = {(float)0.240000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.000000, (float)0.000000, (float)0.000000, (float)0.240000, (float)0.000000};
	static float font_68[] = {(float)0.200000, (float)0.240000, (float)0.240000, (float)0.200000, (float)0.200000, (float)0.000000, (float)0.000000, (float)0.000000, (float)0.000000, (float)0.000000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.200000, (float)0.240000
			, (float)0.240000, (float)0.200000, (float)0.240000, (float)0.040000, (float)0.240000, (float)0.040000, (float)0.200000, (float)0.000000};
	static float font_69[] = {(float)0.000000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.000000, (float)0.000000, (float)0.120000, (float)0.120000, (float)0.120000};
	static float font_70[] = {(float)0.000000, (float)0.000000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.120000, (float)0.000000, (float)0.120000};
	static float font_71[] = {(float)0.240000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.000000, (float)0.000000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.120000
			, (float)0.240000, (float)0.120000, (float)0.120000, (float)0.120000};
	static float font_72[] = {(float)0.240000, (float)0.240000, (float)0.240000, (float)0.000000, (float)0.000000, (float)0.000000, (float)0.000000, (float)0.240000, (float)0.240000, (float)0.120000, (float)0.000000, (float)0.120000};
	static float font_73[] = {(float)0.000000, (float)0.240000, (float)0.000000, (float)0.000000};
	static float font_74[] = {(float)0.240000, (float)0.000000, (float)0.000000, (float)0.000000, (float)0.000000, (float)0.120000, (float)0.000000, (float)0.000000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.000000};
	static float font_75[] = {(float)0.000000, (float)0.240000, (float)0.000000, (float)0.000000, (float)0.200000, (float)0.120000, (float)0.000000, (float)0.120000, (float)0.200000, (float)0.120000, (float)0.240000, (float)0.160000, (float)0.200000, (float)0.120000, (float)0.240000, (float)0.080000
			, (float)0.240000, (float)0.080000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.160000, (float)0.240000, (float)0.240000};
	static float font_76[] = {(float)0.000000, (float)0.240000, (float)0.000000, (float)0.020000, (float)0.240000, (float)0.000000, (float)0.000000, (float)0.000000};
	static float font_77[] = {(float)0.000000, (float)0.240000, (float)0.000000, (float)0.000000, (float)0.000000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.000000, (float)0.120000, (float)0.240000, (float)0.120000, (float)0.080000};
	static float font_78[] = {(float)0.240000, (float)0.000000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.000000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.000000, (float)0.000000, (float)0.240000};
	static float font_79[] = {(float)0.000000, (float)0.240000, (float)0.000000, (float)0.000000, (float)0.000000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.000000, (float)0.240000};
	static float font_80[] = {(float)0.000000, (float)0.000000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.120000, (float)0.000000, (float)0.120000, (float)0.240000, (float)0.120000, (float)0.240000, (float)0.240000};
	static float font_81[] = {(float)0.240000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.000000, (float)0.000000, (float)0.240000, (float)0.240000, (float)0.000000, (float)0.160000, (float)0.000000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.000000
			, (float)0.120000, (float)0.080000, (float)0.120000, (float)-0.080000, (float)0.000000, (float)0.000000, (float)0.080000, (float)0.000000};
	static float font_82[] = {(float)0.200000, (float)0.120000, (float)0.240000, (float)0.080000, (float)0.240000, (float)0.080000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.160000, (float)0.200000, (float)0.120000, (float)0.240000, (float)0.240000, (float)0.000000, (float)0.240000
			, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.000000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.160000, (float)0.000000, (float)0.120000, (float)0.200000, (float)0.120000};
	static float font_83[] = {(float)0.240000, (float)0.120000, (float)0.000000, (float)0.120000, (float)0.000000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.120000
			, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.120000};
	static float font_84[] = {(float)0.000000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.120000, (float)0.240000, (float)0.120000, (float)0.000000};
	static float font_85[] = {(float)0.000000, (float)0.240000, (float)0.000000, (float)0.000000, (float)0.000000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.240000};
	static float font_86[] = {(float)0.240000, (float)0.120000, (float)0.240000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.120000, (float)0.120000, (float)0.000000, (float)0.240000, (float)0.120000, (float)0.000000, (float)0.120000, (float)0.120000, (float)0.000000};
	static float font_87[] = {(float)0.200000, (float)0.000000, (float)0.240000, (float)0.040000, (float)0.160000, (float)0.000000, (float)0.200000, (float)0.000000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.040000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.040000
			, (float)0.000000, (float)0.040000, (float)0.040000, (float)0.000000, (float)0.120000, (float)0.040000, (float)0.080000, (float)0.000000, (float)0.080000, (float)0.000000, (float)0.040000, (float)0.000000, (float)0.120000, (float)0.040000, (float)0.160000, (float)0.000000
			, (float)0.120000, (float)0.040000, (float)0.120000, (float)0.240000};
	static float font_88[] = {(float)0.240000, (float)0.240000, (float)0.000000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.000000, (float)0.240000};
	static float font_89[] = {(float)0.120000, (float)0.120000, (float)0.120000, (float)0.000000, (float)0.000000, (float)0.160000, (float)0.040000, (float)0.120000, (float)0.240000, (float)0.160000, (float)0.200000, (float)0.120000, (float)0.000000, (float)0.160000, (float)0.000000, (float)0.240000
			, (float)0.040000, (float)0.120000, (float)0.120000, (float)0.120000, (float)0.120000, (float)0.120000, (float)0.200000, (float)0.120000, (float)0.240000, (float)0.160000, (float)0.240000, (float)0.240000};
	static float font_90[] = {(float)0.000000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.000000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.240000, (float)0.000000, (float)0.000000};
	static float font_91[] = {(float)0.000000, (float)0.280000, (float)0.000000, (float)-0.040000, (float)0.000000, (float)-0.040000, (float)0.040000, (float)-0.040000, (float)0.040000, (float)0.280000, (float)0.000000, (float)0.280000};
	static float font_92[] = {(float)0.000000, (float)0.240000, (float)0.200000, (float)-0.040000, (float)0.000000, (float)0.260000, (float)0.010000, (float)0.262679, (float)0.010000, (float)0.262679, (float)0.017321, (float)0.270000, (float)0.017321, (float)0.270000, (float)0.020000, (float)0.280000
			, (float)0.240000, (float)-0.020000, (float)0.230000, (float)-0.022679, (float)0.230000, (float)-0.022679, (float)0.222679, (float)-0.030000, (float)0.222679, (float)-0.030000, (float)0.220000, (float)-0.040000, (float)0.240000, (float)0.000000, (float)0.040000, (float)0.280000
			, (float)0.240000, (float)0.000000, (float)0.240000, (float)-0.020000, (float)0.200000, (float)-0.040000, (float)0.220000, (float)-0.040000, (float)0.020000, (float)0.280000, (float)0.040000, (float)0.280000, (float)0.000000, (float)0.260000, (float)0.000000, (float)0.240000};
	static float font_93[] = {(float)0.000000, (float)0.280000, (float)0.040000, (float)0.280000, (float)0.040000, (float)0.280000, (float)0.040000, (float)-0.040000, (float)0.040000, (float)-0.040000, (float)0.000000, (float)-0.040000};
	static float font_94[] = {(float)0.000000, (float)0.240000, (float)0.060000, (float)0.320000, (float)0.060000, (float)0.320000, (float)0.120000, (float)0.240000};
	static float font_95[] = {(float)0.000000, (float)-0.040000, (float)0.020000, (float)-0.020000, (float)0.020000, (float)-0.020000, (float)0.120000, (float)-0.020000, (float)0.120000, (float)-0.020000, (float)0.140000, (float)-0.040000};
	static float font_96[] = {(float)0.000000, (float)0.300000, (float)0.000000, (float)0.240000};
	static float font_97[] = {(float)0.180000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.080000, (float)0.000000, (float)0.160000, (float)0.018756, (float)0.160000, (float)0.018756, (float)0.218564, (float)0.070000, (float)0.218564, (float)0.070000, (float)0.240000, (float)0.140000
			, (float)0.180000, (float)0.240000, (float)0.090000, (float)0.215885, (float)0.090000, (float)0.215885, (float)0.024115, (float)0.150000, (float)0.024115, (float)0.150000, (float)0.000000, (float)0.060000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.240000
			, (float)0.080000, (float)0.000000, (float)0.040000, (float)0.008038, (float)0.040000, (float)0.008038, (float)0.010718, (float)0.030000, (float)0.010718, (float)0.030000, (float)0.000000, (float)0.060000};
	static float font_98[] = {(float)0.140000, (float)0.240000, (float)0.190000, (float)0.226603, (float)0.190000, (float)0.226603, (float)0.226603, (float)0.190000, (float)0.226603, (float)0.190000, (float)0.240000, (float)0.140000, (float)0.000000, (float)0.420000, (float)0.000000, (float)0.000000
			, (float)0.140000, (float)0.240000, (float)0.070000, (float)0.221244, (float)0.070000, (float)0.221244, (float)0.018756, (float)0.170000, (float)0.018756, (float)0.170000, (float)0.000000, (float)0.100000, (float)0.080000, (float)0.000000, (float)0.040000, (float)0.008038
			, (float)0.040000, (float)0.008038, (float)0.010718, (float)0.030000, (float)0.010718, (float)0.030000, (float)0.000000, (float)0.060000, (float)0.120000, (float)0.000000, (float)0.180000, (float)0.018756, (float)0.180000, (float)0.018756, (float)0.223923, (float)0.070000
			, (float)0.223923, (float)0.070000, (float)0.240000, (float)0.140000, (float)0.080000, (float)0.000000, (float)0.120000, (float)0.000000};
	static float font_99[] = {(float)0.140000, (float)0.240000, (float)0.070000, (float)0.223923, (float)0.070000, (float)0.223923, (float)0.018756, (float)0.180000, (float)0.018756, (float)0.180000, (float)0.000000, (float)0.120000, (float)0.120000, (float)0.000000, (float)0.180000, (float)0.010718
			, (float)0.180000, (float)0.010718, (float)0.223923, (float)0.040000, (float)0.223923, (float)0.040000, (float)0.240000, (float)0.080000, (float)0.140000, (float)0.240000, (float)0.190000, (float)0.231962, (float)0.190000, (float)0.231962, (float)0.226603, (float)0.210000
			, (float)0.226603, (float)0.210000, (float)0.240000, (float)0.180000, (float)0.120000, (float)0.000000, (float)0.060000, (float)0.016077, (float)0.060000, (float)0.016077, (float)0.016077, (float)0.060000, (float)0.016077, (float)0.060000, (float)0.000000, (float)0.120000};
	static float font_100[] = {(float)0.240000, (float)0.420000, (float)0.240000, (float)0.000000, (float)0.100000, (float)0.000000, (float)0.050000, (float)0.010718, (float)0.050000, (float)0.010718, (float)0.013397, (float)0.040000, (float)0.013397, (float)0.040000, (float)0.000000, (float)0.080000
			, (float)0.160000, (float)0.240000, (float)0.080000, (float)0.218564, (float)0.080000, (float)0.218564, (float)0.021436, (float)0.160000, (float)0.021436, (float)0.160000, (float)0.000000, (float)0.080000, (float)0.160000, (float)0.240000, (float)0.200000, (float)0.229282
			, (float)0.200000, (float)0.229282, (float)0.229282, (float)0.200000, (float)0.229282, (float)0.200000, (float)0.240000, (float)0.160000, (float)0.100000, (float)0.000000, (float)0.170000, (float)0.010718, (float)0.170000, (float)0.010718, (float)0.221244, (float)0.040000
			, (float)0.221244, (float)0.040000, (float)0.240000, (float)0.080000};
	static float font_101[] = {(float)0.140000, (float)0.000000, (float)0.070000, (float)0.013397, (float)0.070000, (float)0.013397, (float)0.018756, (float)0.050000, (float)0.018756, (float)0.050000, (float)0.000000, (float)0.100000, (float)0.140000, (float)0.240000, (float)0.070000, (float)0.221244
			, (float)0.070000, (float)0.221244, (float)0.018756, (float)0.170000, (float)0.018756, (float)0.170000, (float)0.000000, (float)0.100000, (float)0.140000, (float)0.240000, (float)0.190000, (float)0.229282, (float)0.190000, (float)0.229282, (float)0.226603, (float)0.200000
			, (float)0.226603, (float)0.200000, (float)0.240000, (float)0.160000, (float)0.140000, (float)0.000000, (float)0.190000, (float)0.008038, (float)0.190000, (float)0.008038, (float)0.226603, (float)0.030000, (float)0.226603, (float)0.030000, (float)0.240000, (float)0.060000
			, (float)0.000000, (float)0.120000, (float)0.240000, (float)0.120000, (float)0.240000, (float)0.120000, (float)0.240000, (float)0.160000};
	static float font_102[] = {(float)0.000000, (float)0.160000, (float)0.120000, (float)0.160000, (float)0.060000, (float)0.000000, (float)0.060000, (float)0.340000, (float)0.140000, (float)0.420000, (float)0.100000, (float)0.409282, (float)0.100000, (float)0.409282, (float)0.070718, (float)0.380000
			, (float)0.070718, (float)0.380000, (float)0.060000, (float)0.340000, (float)0.140000, (float)0.420000, (float)0.170000, (float)0.411962, (float)0.170000, (float)0.411962, (float)0.191962, (float)0.390000, (float)0.191962, (float)0.390000, (float)0.200000, (float)0.360000};
	static float font_103[] = {(float)0.040000, (float)-0.320000, (float)0.130000, (float)-0.277128, (float)0.130000, (float)-0.277128, (float)0.195885, (float)-0.160000, (float)0.195885, (float)-0.160000, (float)0.220000, (float)-0.000000, (float)0.040000, (float)-0.320000, (float)0.020000, (float)-0.314641
			, (float)0.020000, (float)-0.314641, (float)0.005359, (float)-0.300000, (float)0.005359, (float)-0.300000, (float)0.000000, (float)-0.280000, (float)0.240000, (float)-0.020000, (float)0.120000, (float)-0.054833, (float)0.120000, (float)-0.054833, (float)0.032154, (float)-0.150000
			, (float)0.032154, (float)-0.150000, (float)0.000000, (float)-0.280000, (float)0.060000, (float)0.000000, (float)0.030000, (float)0.008038, (float)0.030000, (float)0.008038, (float)0.008038, (float)0.030000, (float)0.008038, (float)0.030000, (float)0.000000, (float)0.060000
			, (float)0.220000, (float)0.000000, (float)0.220000, (float)0.240000, (float)0.060000, (float)0.000000, (float)0.140000, (float)0.016077, (float)0.140000, (float)0.016077, (float)0.198564, (float)0.060000, (float)0.198564, (float)0.060000, (float)0.220000, (float)0.120000
			, (float)0.160000, (float)0.240000, (float)0.080000, (float)0.215885, (float)0.080000, (float)0.215885, (float)0.021436, (float)0.150000, (float)0.021436, (float)0.150000, (float)0.000000, (float)0.060000, (float)0.160000, (float)0.240000, (float)0.220000, (float)0.240000};
	static float font_104[] = {(float)0.100000, (float)0.240000, (float)0.140000, (float)0.229282, (float)0.140000, (float)0.229282, (float)0.169282, (float)0.200000, (float)0.169282, (float)0.200000, (float)0.180000, (float)0.160000, (float)0.100000, (float)0.240000, (float)0.050000, (float)0.223923
			, (float)0.050000, (float)0.223923, (float)0.013397, (float)0.180000, (float)0.013397, (float)0.180000, (float)0.000000, (float)0.120000, (float)0.000000, (float)0.000000, (float)0.000000, (float)0.420000, (float)0.180000, (float)0.160000, (float)0.180000, (float)0.000000};
	static float font_105[] = {(float)0.000000, (float)0.420000, (float)0.000000, (float)0.360000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.000000};
	static float font_106[] = {(float)0.000000, (float)0.360000, (float)0.000000, (float)0.420000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.000000, (float)-0.120000, (float)-0.200000, (float)-0.060000, (float)-0.173205, (float)-0.060000, (float)-0.173205, (float)-0.016077, (float)-0.100000
			, (float)-0.016077, (float)-0.100000, (float)0.000000, (float)-0.000000, (float)-0.120000, (float)-0.200000, (float)-0.150000, (float)-0.191962, (float)-0.150000, (float)-0.191962, (float)-0.171962, (float)-0.170000, (float)-0.171962, (float)-0.170000, (float)-0.180000, (float)-0.140000};
	static float font_107[] = {(float)0.000000, (float)0.120000, (float)0.160000, (float)0.240000, (float)0.000000, (float)0.420000, (float)0.000000, (float)0.000000, (float)0.160000, (float)0.000000, (float)0.000000, (float)0.120000};
	static float font_108[] = {(float)0.000000, (float)0.420000, (float)0.000000, (float)0.000000};
	static float font_109[] = {(float)0.000000, (float)0.000000, (float)0.000000, (float)0.240000, (float)0.100000, (float)0.240000, (float)0.050000, (float)0.231962, (float)0.050000, (float)0.231962, (float)0.013397, (float)0.210000, (float)0.013397, (float)0.210000, (float)0.000000, (float)0.180000
			, (float)0.100000, (float)0.240000, (float)0.130000, (float)0.231962, (float)0.130000, (float)0.231962, (float)0.151962, (float)0.210000, (float)0.151962, (float)0.210000, (float)0.160000, (float)0.180000, (float)0.260000, (float)0.240000, (float)0.210000, (float)0.231962
			, (float)0.210000, (float)0.231962, (float)0.173397, (float)0.210000, (float)0.173397, (float)0.210000, (float)0.160000, (float)0.180000, (float)0.260000, (float)0.240000, (float)0.290000, (float)0.231962, (float)0.290000, (float)0.231962, (float)0.311962, (float)0.210000
			, (float)0.311962, (float)0.210000, (float)0.320000, (float)0.180000, (float)0.160000, (float)0.180000, (float)0.160000, (float)0.000000, (float)0.320000, (float)0.180000, (float)0.320000, (float)0.000000};
	static float font_110[] = {(float)0.000000, (float)0.240000, (float)0.000000, (float)0.000000, (float)0.180000, (float)0.180000, (float)0.180000, (float)0.000000, (float)0.100000, (float)0.240000, (float)0.140000, (float)0.231962, (float)0.140000, (float)0.231962, (float)0.169282, (float)0.210000
			, (float)0.169282, (float)0.210000, (float)0.180000, (float)0.180000, (float)0.100000, (float)0.240000, (float)0.050000, (float)0.223923, (float)0.050000, (float)0.223923, (float)0.013397, (float)0.180000, (float)0.013397, (float)0.180000, (float)0.000000, (float)0.120000};
	static float font_111[] = {(float)0.140000, (float)0.240000, (float)0.070000, (float)0.221244, (float)0.070000, (float)0.221244, (float)0.018756, (float)0.170000, (float)0.018756, (float)0.170000, (float)0.000000, (float)0.100000, (float)0.100000, (float)0.000000, (float)0.160000, (float)0.018756
			, (float)0.160000, (float)0.018756, (float)0.203923, (float)0.070000, (float)0.203923, (float)0.070000, (float)0.220000, (float)0.140000, (float)0.140000, (float)0.240000, (float)0.180000, (float)0.226603, (float)0.180000, (float)0.226603, (float)0.209282, (float)0.190000
			, (float)0.209282, (float)0.190000, (float)0.220000, (float)0.140000, (float)0.100000, (float)0.000000, (float)0.050000, (float)0.013397, (float)0.050000, (float)0.013397, (float)0.013397, (float)0.050000, (float)0.013397, (float)0.050000, (float)0.000000, (float)0.100000};
	static float font_112[] = {(float)0.120000, (float)0.240000, (float)0.160000, (float)0.231962, (float)0.160000, (float)0.231962, (float)0.189282, (float)0.210000, (float)0.189282, (float)0.210000, (float)0.200000, (float)0.180000, (float)0.120000, (float)0.000000, (float)0.060000, (float)0.013397
			, (float)0.060000, (float)0.013397, (float)0.016077, (float)0.050000, (float)0.016077, (float)0.050000, (float)0.000000, (float)0.100000, (float)0.120000, (float)0.240000, (float)0.060000, (float)0.231962, (float)0.060000, (float)0.231962, (float)0.016077, (float)0.210000
			, (float)0.016077, (float)0.210000, (float)0.000000, (float)0.180000, (float)0.120000, (float)0.000000, (float)0.160000, (float)0.013397, (float)0.160000, (float)0.013397, (float)0.189282, (float)0.050000, (float)0.189282, (float)0.050000, (float)0.200000, (float)0.100000
			, (float)0.200000, (float)0.180000, (float)0.200000, (float)0.120000, (float)0.000000, (float)0.240000, (float)0.000000, (float)-0.320000};
	static float font_113[] = {(float)0.240000, (float)-0.320000, (float)0.240000, (float)0.240000, (float)0.160000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.100000, (float)0.000000, (float)0.170000, (float)0.013397, (float)0.170000, (float)0.013397, (float)0.221244, (float)0.050000
			, (float)0.221244, (float)0.050000, (float)0.240000, (float)0.100000, (float)0.320000, (float)-0.160000, (float)0.120000, (float)-0.160000, (float)0.160000, (float)0.240000, (float)0.080000, (float)0.226603, (float)0.080000, (float)0.226603, (float)0.021436, (float)0.190000
			, (float)0.021436, (float)0.190000, (float)0.000000, (float)0.140000, (float)0.100000, (float)0.000000, (float)0.050000, (float)0.018756, (float)0.050000, (float)0.018756, (float)0.013397, (float)0.070000, (float)0.013397, (float)0.070000, (float)0.000000, (float)0.140000};
	static float font_114[] = {(float)0.100000, (float)0.240000, (float)0.130000, (float)0.234641, (float)0.130000, (float)0.234641, (float)0.151962, (float)0.220000, (float)0.151962, (float)0.220000, (float)0.160000, (float)0.200000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.000000
			, (float)0.100000, (float)0.240000, (float)0.050000, (float)0.226603, (float)0.050000, (float)0.226603, (float)0.013397, (float)0.190000, (float)0.013397, (float)0.190000, (float)0.000000, (float)0.140000};
	static float font_115[] = {(float)0.140000, (float)0.000000, (float)0.190000, (float)0.010718, (float)0.190000, (float)0.010718, (float)0.226603, (float)0.040000, (float)0.226603, (float)0.040000, (float)0.240000, (float)0.080000, (float)0.120000, (float)0.000000, (float)0.140000, (float)0.000000
			, (float)0.140000, (float)0.140000, (float)0.190000, (float)0.131962, (float)0.190000, (float)0.131962, (float)0.226603, (float)0.110000, (float)0.226603, (float)0.110000, (float)0.240000, (float)0.080000, (float)0.120000, (float)0.240000, (float)0.070000, (float)0.231962
			, (float)0.070000, (float)0.231962, (float)0.033397, (float)0.210000, (float)0.033397, (float)0.210000, (float)0.020000, (float)0.180000, (float)0.100000, (float)0.140000, (float)0.060000, (float)0.145359, (float)0.060000, (float)0.145359, (float)0.030718, (float)0.160000
			, (float)0.030718, (float)0.160000, (float)0.020000, (float)0.180000, (float)0.120000, (float)0.000000, (float)0.060000, (float)0.010718, (float)0.060000, (float)0.010718, (float)0.016077, (float)0.040000, (float)0.016077, (float)0.040000, (float)0.000000, (float)0.080000
			, (float)0.120000, (float)0.240000, (float)0.170000, (float)0.234641, (float)0.170000, (float)0.234641, (float)0.206603, (float)0.220000, (float)0.206603, (float)0.220000, (float)0.220000, (float)0.200000, (float)0.140000, (float)0.140000, (float)0.100000, (float)0.140000};
	static float font_116[] = {(float)0.100000, (float)0.000000, (float)0.070000, (float)0.010718, (float)0.070000, (float)0.010718, (float)0.048038, (float)0.040000, (float)0.048038, (float)0.040000, (float)0.040000, (float)0.080000, (float)0.100000, (float)0.000000, (float)0.140000, (float)0.013397
			, (float)0.140000, (float)0.013397, (float)0.169282, (float)0.050000, (float)0.169282, (float)0.050000, (float)0.180000, (float)0.100000, (float)0.000000, (float)0.200000, (float)0.120000, (float)0.200000, (float)0.040000, (float)0.080000, (float)0.040000, (float)0.420000};
	static float font_117[] = {(float)0.120000, (float)0.000000, (float)0.060000, (float)0.018756, (float)0.060000, (float)0.018756, (float)0.016077, (float)0.070000, (float)0.016077, (float)0.070000, (float)0.000000, (float)0.140000, (float)0.120000, (float)0.000000, (float)0.150000, (float)0.010718
			, (float)0.150000, (float)0.010718, (float)0.171962, (float)0.040000, (float)0.171962, (float)0.040000, (float)0.180000, (float)0.080000, (float)0.000000, (float)0.000000, (float)0.000000, (float)0.240000, (float)0.180000, (float)0.080000, (float)0.180000, (float)0.240000};
	static float font_118[] = {(float)0.060000, (float)0.000000, (float)0.240000, (float)0.240000, (float)0.000000, (float)0.240000, (float)0.060000, (float)0.000000};
	static float font_119[] = {(float)0.000000, (float)0.240000, (float)0.020000, (float)0.000000, (float)0.120000, (float)0.240000, (float)0.140000, (float)0.000000, (float)0.020000, (float)0.000000, (float)0.120000, (float)0.240000, (float)0.140000, (float)0.000000, (float)0.240000, (float)0.240000};
	static float font_120[] = {(float)0.160000, (float)0.000000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.000000, (float)0.160000, (float)0.240000};
	static float font_121[] = {(float)0.180000, (float)0.000000, (float)0.180000, (float)0.240000, (float)0.120000, (float)0.000000, (float)0.150000, (float)0.008038, (float)0.150000, (float)0.008038, (float)0.171962, (float)0.030000, (float)0.171962, (float)0.030000, (float)0.180000, (float)0.060000
			, (float)0.120000, (float)0.000000, (float)0.060000, (float)0.021436, (float)0.060000, (float)0.021436, (float)0.016077, (float)0.080000, (float)0.016077, (float)0.080000, (float)0.000000, (float)0.160000, (float)0.000000, (float)-0.320000, (float)0.090000, (float)-0.277128
			, (float)0.090000, (float)-0.277128, (float)0.155885, (float)-0.160000, (float)0.155885, (float)-0.160000, (float)0.180000, (float)-0.000000, (float)0.000000, (float)0.160000, (float)0.000000, (float)0.240000};
	static float font_122[] = {(float)0.000000, (float)0.000000, (float)0.240000, (float)0.000000, (float)0.000000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.000000, (float)0.000000, (float)0.240000, (float)0.240000};
	static float font_123[] = {(float)0.020000, (float)0.140000, (float)0.020000, (float)0.240000, (float)0.020000, (float)0.100000, (float)0.020000, (float)0.000000, (float)0.000000, (float)0.120000, (float)0.010000, (float)0.117321, (float)0.010000, (float)0.117321, (float)0.017321, (float)0.110000
			, (float)0.017321, (float)0.110000, (float)0.020000, (float)0.100000, (float)0.000000, (float)0.120000, (float)0.010000, (float)0.122679, (float)0.010000, (float)0.122679, (float)0.017321, (float)0.130000, (float)0.017321, (float)0.130000, (float)0.020000, (float)0.140000
			, (float)0.060000, (float)0.280000, (float)0.040000, (float)0.274641, (float)0.040000, (float)0.274641, (float)0.025359, (float)0.260000, (float)0.025359, (float)0.260000, (float)0.020000, (float)0.240000, (float)0.060000, (float)-0.040000, (float)0.040000, (float)-0.034641
			, (float)0.040000, (float)-0.034641, (float)0.025359, (float)-0.020000, (float)0.025359, (float)-0.020000, (float)0.020000, (float)-0.000000};
	static float font_124[] = {(float)0.000000, (float)0.420000, (float)0.000000, (float)-0.180000};
	static float font_125[] = {(float)0.040000, (float)0.000000, (float)0.040000, (float)0.100000, (float)0.040000, (float)0.140000, (float)0.040000, (float)0.240000, (float)0.060000, (float)0.120000, (float)0.050000, (float)0.122679, (float)0.050000, (float)0.122679, (float)0.042679, (float)0.130000
			, (float)0.042679, (float)0.130000, (float)0.040000, (float)0.140000, (float)0.060000, (float)0.120000, (float)0.050000, (float)0.117321, (float)0.050000, (float)0.117321, (float)0.042679, (float)0.110000, (float)0.042679, (float)0.110000, (float)0.040000, (float)0.100000
			, (float)0.000000, (float)0.280000, (float)0.020000, (float)0.274641, (float)0.020000, (float)0.274641, (float)0.034641, (float)0.260000, (float)0.034641, (float)0.260000, (float)0.040000, (float)0.240000, (float)0.000000, (float)-0.040000, (float)0.020000, (float)-0.034641
			, (float)0.020000, (float)-0.034641, (float)0.034641, (float)-0.020000, (float)0.034641, (float)-0.020000, (float)0.040000, (float)-0.000000};
	static float font_126[] = {(float)0.080000, (float)0.260000, (float)0.040000, (float)0.254641, (float)0.040000, (float)0.254641, (float)0.010718, (float)0.240000, (float)0.010718, (float)0.240000, (float)0.000000, (float)0.220000, (float)0.080000, (float)0.240000, (float)0.120000, (float)0.245359
			, (float)0.120000, (float)0.245359, (float)0.149282, (float)0.260000, (float)0.149282, (float)0.260000, (float)0.160000, (float)0.280000};
	static uint font_size[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
			, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
			, 0, 4, 8, 8, 46, 50, 46, 10, 12, 12, 0, 4, 6, 2, 24, 2
			, 28, 8, 26, 38, 6, 26, 30, 6, 48, 24, 4, 4, 4, 4, 4, 48
			, 50, 8, 20, 6, 12, 8, 6, 10, 6, 2, 6, 12, 4, 8, 6, 8
			, 8, 12, 14, 10, 4, 6, 8, 18, 4, 14, 6, 6, 24, 6, 4, 6
			, 2, 22, 28, 24, 26, 28, 16, 40, 16, 4, 16, 6, 2, 30, 16, 24
			, 28, 24, 14, 40, 16, 16, 4, 8, 4, 22, 6, 28, 2, 28, 12, 0
			, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
			, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
			, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
			, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
			, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
			, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
			, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
			, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	static float *font_array[256];
	static boolean init = FALSE;
	if(init == FALSE)
	{
		init = TRUE;
		font_array[33] = font_33;
		font_array[34] = font_34;
		font_array[35] = font_35;
		font_array[36] = font_36;
		font_array[37] = font_37;
		font_array[38] = font_38;
		font_array[39] = font_39;
		font_array[40] = font_40;
		font_array[41] = font_41;
		font_array[43] = font_43;
		font_array[44] = font_44;
		font_array[45] = font_45;
		font_array[46] = font_46;
		font_array[47] = font_47;
		font_array[48] = font_48;
		font_array[49] = font_49;
		font_array[50] = font_50;
		font_array[51] = font_51;
		font_array[52] = font_52;
		font_array[53] = font_53;
		font_array[54] = font_54;
		font_array[55] = font_55;
		font_array[56] = font_56;
		font_array[57] = font_57;
		font_array[58] = font_58;
		font_array[59] = font_59;
		font_array[60] = font_60;
		font_array[61] = font_61;
		font_array[62] = font_62;
		font_array[63] = font_63;
		font_array[64] = font_64;
		font_array[65] = font_65;
		font_array[66] = font_66;
		font_array[67] = font_67;
		font_array[68] = font_68;
		font_array[69] = font_69;
		font_array[70] = font_70;
		font_array[71] = font_71;
		font_array[72] = font_72;
		font_array[73] = font_73;
		font_array[74] = font_74;
		font_array[75] = font_75;
		font_array[76] = font_76;
		font_array[77] = font_77;
		font_array[78] = font_78;
		font_array[79] = font_79;
		font_array[80] = font_80;
		font_array[81] = font_81;
		font_array[82] = font_82;
		font_array[83] = font_83;
		font_array[84] = font_84;
		font_array[85] = font_85;
		font_array[86] = font_86;
		font_array[87] = font_87;
		font_array[88] = font_88;
		font_array[89] = font_89;
		font_array[90] = font_90;
		font_array[91] = font_91;
		font_array[92] = font_92;
		font_array[93] = font_93;
		font_array[94] = font_94;
		font_array[95] = font_95;
		font_array[96] = font_96;
		font_array[97] = font_97;
		font_array[98] = font_98;
		font_array[99] = font_99;
		font_array[100] = font_100;
		font_array[101] = font_101;
		font_array[102] = font_102;
		font_array[103] = font_103;
		font_array[104] = font_104;
		font_array[105] = font_105;
		font_array[106] = font_106;
		font_array[107] = font_107;
		font_array[108] = font_108;
		font_array[109] = font_109;
		font_array[110] = font_110;
		font_array[111] = font_111;
		font_array[112] = font_112;
		font_array[113] = font_113;
		font_array[114] = font_114;
		font_array[115] = font_115;
		font_array[116] = font_116;
		font_array[117] = font_117;
		font_array[118] = font_118;
		font_array[119] = font_119;
		font_array[120] = font_120;
		font_array[121] = font_121;
		font_array[122] = font_122;
		font_array[123] = font_123;
		font_array[124] = font_124;
		font_array[125] = font_125;
		font_array[126] = font_126;
	}
	if(font_size[letter] != 0)
		sui_draw_gl(GL_LINES, font_array[letter], font_size[letter], 2, red, green, blue, 1.0f);
}

float sui_get_letter_size(unsigned char letter)
{
	static const float font_width[] = {0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000
			, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000
			, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.220000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.040000, (float)0.060000, (float)0.060000, (float)0.100000, (float)0.240000, (float)0.030000, (float)0.240000, (float)0.060000, (float)0.100000
			, (float)0.240000, (float)0.100000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.200000, (float)0.100000, (float)0.040000, (float)0.240000, (float)0.160000, (float)0.240000, (float)0.240000
			, (float)0.220000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.100000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000
			, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.040000, (float)0.240000, (float)0.040000, (float)0.120000, (float)0.140000
			, (float)0.100000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.240000, (float)0.200000, (float)0.240000, (float)0.180000, (float)0.100000, (float)0.100000, (float)0.160000, (float)0.100000, (float)0.320000, (float)0.180000, (float)0.220000
			, (float)0.200000, (float)0.320000, (float)0.160000, (float)0.240000, (float)0.180000, (float)0.180000, (float)0.240000, (float)0.240000, (float)0.160000, (float)0.180000, (float)0.240000, (float)0.060000, (float)0.100000, (float)0.060000, (float)0.160000, (float)0.100000
			, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000
			, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000
			, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000
			, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000
			, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000
			, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000
			, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000
			, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000, (float)0.100000};
	return font_width[letter];
}
