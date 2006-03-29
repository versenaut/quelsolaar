#include <stdlib.h>
#include <math.h>

#include "seduce.h"

void	sui_draw_line3(float pos_x, float pos_y, float dest_x, float dest_y, float red, float green, float blue)
{
	float array[48], vec_x, vec_y, r;
	vec_x = pos_x - dest_x;
	vec_y = dest_y - pos_y;
	r = sqrt(vec_x * vec_x + vec_y * vec_y);
	vec_x /= r;
	vec_y /= r;
	array[0] = pos_x - -0.000000 * vec_x - 0.005000 * vec_y;
	array[1] = pos_y + -0.000000 * vec_y - 0.005000 * vec_x;
	array[2] = dest_x - 0.000000 * vec_x - 0.005000 * vec_y;
	array[3] = dest_y + 0.000000 * vec_y - 0.005000 * vec_x;
	array[4] = dest_x - 0.000000 * vec_x - -0.005000 * vec_y;
	array[5] = dest_y + 0.000000 * vec_y - -0.005000 * vec_x;
	array[6] = pos_x - -0.000000 * vec_x - -0.005000 * vec_y;
	array[7] = pos_y + -0.000000 * vec_y - -0.005000 * vec_x;
	array[8] = dest_x - 0.000000 * vec_x - -0.005000 * vec_y;
	array[9] = dest_y + 0.000000 * vec_y - -0.005000 * vec_x;
	array[10] = dest_x - 0.001768 * vec_x - -0.004268 * vec_y;
	array[11] = dest_y + 0.001768 * vec_y - -0.004268 * vec_x;
	array[12] = dest_x - 0.001768 * vec_x - -0.004268 * vec_y;
	array[13] = dest_y + 0.001768 * vec_y - -0.004268 * vec_x;
	array[14] = dest_x - 0.002500 * vec_x - -0.002500 * vec_y;
	array[15] = dest_y + 0.002500 * vec_y - -0.002500 * vec_x;
	array[16] = dest_x - 0.000000 * vec_x - 0.005000 * vec_y;
	array[17] = dest_y + 0.000000 * vec_y - 0.005000 * vec_x;
	array[18] = dest_x - 0.001768 * vec_x - 0.004268 * vec_y;
	array[19] = dest_y + 0.001768 * vec_y - 0.004268 * vec_x;
	array[20] = dest_x - 0.001768 * vec_x - 0.004268 * vec_y;
	array[21] = dest_y + 0.001768 * vec_y - 0.004268 * vec_x;
	array[22] = dest_x - 0.002500 * vec_x - 0.002500 * vec_y;
	array[23] = dest_y + 0.002500 * vec_y - 0.002500 * vec_x;
	array[24] = dest_x - 0.002500 * vec_x - 0.002500 * vec_y;
	array[25] = dest_y + 0.002500 * vec_y - 0.002500 * vec_x;
	array[26] = dest_x - 0.002500 * vec_x - -0.002500 * vec_y;
	array[27] = dest_y + 0.002500 * vec_y - -0.002500 * vec_x;
	array[28] = pos_x - -0.002500 * vec_x - 0.002500 * vec_y;
	array[29] = pos_y + -0.002500 * vec_y - 0.002500 * vec_x;
	array[30] = pos_x - -0.002500 * vec_x - -0.002500 * vec_y;
	array[31] = pos_y + -0.002500 * vec_y - -0.002500 * vec_x;
	array[32] = pos_x - -0.000000 * vec_x - 0.005000 * vec_y;
	array[33] = pos_y + -0.000000 * vec_y - 0.005000 * vec_x;
	array[34] = pos_x - -0.001768 * vec_x - 0.004268 * vec_y;
	array[35] = pos_y + -0.001768 * vec_y - 0.004268 * vec_x;
	array[36] = pos_x - -0.001768 * vec_x - 0.004268 * vec_y;
	array[37] = pos_y + -0.001768 * vec_y - 0.004268 * vec_x;
	array[38] = pos_x - -0.002500 * vec_x - 0.002500 * vec_y;
	array[39] = pos_y + -0.002500 * vec_y - 0.002500 * vec_x;
	array[40] = pos_x - -0.000000 * vec_x - -0.005000 * vec_y;
	array[41] = pos_y + -0.000000 * vec_y - -0.005000 * vec_x;
	array[42] = pos_x - -0.001768 * vec_x - -0.004268 * vec_y;
	array[43] = pos_y + -0.001768 * vec_y - -0.004268 * vec_x;
	array[44] = pos_x - -0.001768 * vec_x - -0.004268 * vec_y;
	array[45] = pos_y + -0.001768 * vec_y - -0.004268 * vec_x;
	array[46] = pos_x - -0.002500 * vec_x - -0.002500 * vec_y;
	array[47] = pos_y + -0.002500 * vec_y - -0.002500 * vec_x;
	sui_draw_gl(GL_LINES, array, 24, 2, red, green, blue);
}

void	sui_draw_line(float pos_x, float pos_y, float dest_x, float dest_y, float red, float green, float blue)
{
	float array[76], vec_x, vec_y, r;
	vec_x = pos_x - dest_x;
	vec_y = dest_y - pos_y;
	r = sqrt(vec_x * vec_x + vec_y * vec_y);
	vec_x /= r;
	vec_y /= r;
	array[0] = dest_x - -0.007500 * vec_x - 0.007500 * vec_y;
	array[1] = dest_y + -0.007500 * vec_y - 0.007500 * vec_x;
	array[2] = pos_x - 0.007500 * vec_x - 0.007500 * vec_y;
	array[3] = pos_y + 0.007500 * vec_y - 0.007500 * vec_x;
	array[4] = pos_x - 0.017500 * vec_x - -0.007500 * vec_y;
	array[5] = pos_y + 0.017500 * vec_y - -0.007500 * vec_x;
	array[6] = pos_x - 0.007500 * vec_x - -0.007500 * vec_y;
	array[7] = pos_y + 0.007500 * vec_y - -0.007500 * vec_x;
	array[8] = pos_x - 0.017500 * vec_x - -0.007500 * vec_y;
	array[9] = pos_y + 0.017500 * vec_y - -0.007500 * vec_x;
	array[10] = pos_x - 0.020000 * vec_x - -0.005000 * vec_y;
	array[11] = pos_y + 0.020000 * vec_y - -0.005000 * vec_x;
	array[12] = dest_x - -0.020000 * vec_x - -0.005000 * vec_y;
	array[13] = dest_y + -0.020000 * vec_y - -0.005000 * vec_x;
	array[14] = dest_x - -0.017500 * vec_x - -0.007500 * vec_y;
	array[15] = dest_y + -0.017500 * vec_y - -0.007500 * vec_x;
	array[16] = dest_x - -0.017500 * vec_x - -0.007500 * vec_y;
	array[17] = dest_y + -0.017500 * vec_y - -0.007500 * vec_x;
	array[18] = dest_x - -0.007500 * vec_x - -0.007500 * vec_y;
	array[19] = dest_y + -0.007500 * vec_y - -0.007500 * vec_x;
	array[20] = dest_x - -0.007500 * vec_x - 0.007500 * vec_y;
	array[21] = dest_y + -0.007500 * vec_y - 0.007500 * vec_x;
	array[22] = dest_x - -0.003750 * vec_x - 0.006495 * vec_y;
	array[23] = dest_y + -0.003750 * vec_y - 0.006495 * vec_x;
	array[24] = dest_x - -0.003750 * vec_x - 0.006495 * vec_y;
	array[25] = dest_y + -0.003750 * vec_y - 0.006495 * vec_x;
	array[26] = dest_x - -0.001005 * vec_x - 0.003750 * vec_y;
	array[27] = dest_y + -0.001005 * vec_y - 0.003750 * vec_x;
	array[28] = dest_x - -0.001005 * vec_x - 0.003750 * vec_y;
	array[29] = dest_y + -0.001005 * vec_y - 0.003750 * vec_x;
	array[30] = dest_x - 0.000000 * vec_x - 0.000000 * vec_y;
	array[31] = dest_y + 0.000000 * vec_y - 0.000000 * vec_x;
	array[32] = pos_x - 0.020000 * vec_x - -0.005000 * vec_y;
	array[33] = pos_y + 0.020000 * vec_y - -0.005000 * vec_x;
	array[34] = dest_x - -0.020000 * vec_x - -0.005000 * vec_y;
	array[35] = dest_y + -0.020000 * vec_y - -0.005000 * vec_x;
	array[36] = pos_x - 0.007500 * vec_x - 0.007500 * vec_y;
	array[37] = pos_y + 0.007500 * vec_y - 0.007500 * vec_x;
	array[38] = pos_x - 0.003750 * vec_x - 0.006495 * vec_y;
	array[39] = pos_y + 0.003750 * vec_y - 0.006495 * vec_x;
	array[40] = pos_x - 0.003750 * vec_x - 0.006495 * vec_y;
	array[41] = pos_y + 0.003750 * vec_y - 0.006495 * vec_x;
	array[42] = pos_x - 0.001005 * vec_x - 0.003750 * vec_y;
	array[43] = pos_y + 0.001005 * vec_y - 0.003750 * vec_x;
	array[44] = pos_x - 0.001005 * vec_x - 0.003750 * vec_y;
	array[45] = pos_y + 0.001005 * vec_y - 0.003750 * vec_x;
	array[46] = pos_x - -0.000000 * vec_x - 0.000000 * vec_y;
	array[47] = pos_y + -0.000000 * vec_y - 0.000000 * vec_x;
	array[48] = pos_x - 0.007500 * vec_x - -0.007500 * vec_y;
	array[49] = pos_y + 0.007500 * vec_y - -0.007500 * vec_x;
	array[50] = pos_x - 0.003750 * vec_x - -0.006495 * vec_y;
	array[51] = pos_y + 0.003750 * vec_y - -0.006495 * vec_x;
	array[52] = pos_x - 0.003750 * vec_x - -0.006495 * vec_y;
	array[53] = pos_y + 0.003750 * vec_y - -0.006495 * vec_x;
	array[54] = pos_x - 0.001005 * vec_x - -0.003750 * vec_y;
	array[55] = pos_y + 0.001005 * vec_y - -0.003750 * vec_x;
	array[56] = pos_x - 0.001005 * vec_x - -0.003750 * vec_y;
	array[57] = pos_y + 0.001005 * vec_y - -0.003750 * vec_x;
	array[58] = pos_x - -0.000000 * vec_x - 0.000000 * vec_y;
	array[59] = pos_y + -0.000000 * vec_y - 0.000000 * vec_x;
	array[60] = dest_x - -0.007500 * vec_x - -0.007500 * vec_y;
	array[61] = dest_y + -0.007500 * vec_y - -0.007500 * vec_x;
	array[62] = dest_x - -0.003750 * vec_x - -0.006495 * vec_y;
	array[63] = dest_y + -0.003750 * vec_y - -0.006495 * vec_x;
	array[64] = dest_x - -0.003750 * vec_x - -0.006495 * vec_y;
	array[65] = dest_y + -0.003750 * vec_y - -0.006495 * vec_x;
	array[66] = dest_x - -0.001005 * vec_x - -0.003750 * vec_y;
	array[67] = dest_y + -0.001005 * vec_y - -0.003750 * vec_x;
	array[68] = dest_x - -0.001005 * vec_x - -0.003750 * vec_y;
	array[69] = dest_y + -0.001005 * vec_y - -0.003750 * vec_x;
	array[70] = dest_x - 0.000000 * vec_x - 0.000000 * vec_y;
	array[71] = dest_y + 0.000000 * vec_y - 0.000000 * vec_x;
	array[72] = dest_x - -0.022500 * vec_x - -0.007500 * vec_y;
	array[73] = dest_y + -0.022500 * vec_y - -0.007500 * vec_x;
	array[74] = pos_x - 0.022500 * vec_x - -0.007500 * vec_y;
	array[75] = pos_y + 0.022500 * vec_y - -0.007500 * vec_x;
	sui_draw_gl(GL_LINES, array, 38, 2, red, green, blue);
}

void	sui_draw_slider_line(float pos_x, float pos_y, float red, float green, float blue)
{
	static float array[] = {0.005000, 0.015000, 0.005000, 0.027500, -0.005000, 0.027500, -0.005000, 0.015000, -0.000000, 0.010000, 0.005000, 0.015000, -0.005000, -0.027500, -0.005000, -0.015000, 0.005000, -0.015000, 0.005000, -0.027500, -0.005000, -0.015000, -0.000000, -0.010000, 0.005000, -0.027500, -0.005000, -0.027500, -0.000000, -0.010000, 0.005000, -0.015000, 0.005000, 0.027500, -0.005000, 0.027500, -0.005000, 0.015000, -0.000000, 0.010000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	sui_draw_gl(GL_LINES, array, 20, 2, red, green, blue);
	glPopMatrix();
}

void sui_draw_check_box_unchecked(float pos_x, float pos_y, float red, float green, float blue)
{
	static float array[] = {-0.010000, -0.017500, 0.010000, -0.017500, 0.017500, 0.012500, 0.017500, -0.010000, -0.017500, 0.010000, -0.017500, -0.010000, 0.027500, 0.032500, -0.000000, 0.005000, -0.010000, -0.017500, -0.013750, -0.016495, -0.013750, -0.016495, -0.016495, -0.013750, -0.016495, -0.013750, -0.017500, -0.010000, -0.010000, 0.012500, -0.015000, 0.005000, 0.027500, 0.032500, 0.005000, -0.012500, 0.012500, 0.017500, -0.010000, 0.017500, -0.000000, 0.005000, -0.010000, 0.012500, -0.005000, -0.012500, -0.015000, 0.005000, -0.010000, 0.017500, -0.013750, 0.016495, -0.013750, 0.016495, -0.016495, 0.013750, -0.016495, 0.013750, -0.017500, 0.010000, 0.010000, -0.017500, 0.013750, -0.016495, 0.013750, -0.016495, 0.016495, -0.013750, 0.016495, -0.013750, 0.017500, -0.010000, 0.005000, -0.012500, -0.005000, -0.012500};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	sui_draw_gl(GL_LINES, array, 38, 2, red, green, blue);
	glPopMatrix();
}

void sui_draw_check_box_checked(float pos_x, float pos_y, float red, float green, float blue)
{
	static float array[] = {-0.017500, 0.010000, -0.017500, -0.010000, -0.010000, 0.017500, -0.013750, 0.016495, -0.013750, 0.016495, -0.016495, 0.013750, -0.016495, 0.013750, -0.017500, 0.010000, -0.010000, -0.017500, -0.013750, -0.016495, -0.013750, -0.016495, -0.016495, -0.013750, -0.016495, -0.013750, -0.017500, -0.010000, 0.010000, -0.017500, 0.013750, -0.016495, 0.013750, -0.016495, 0.016495, -0.013750, 0.016495, -0.013750, 0.017500, -0.010000, 0.010000, 0.017500, 0.013750, 0.016495, 0.013750, 0.016495, 0.016495, 0.013750, 0.016495, 0.013750, 0.017500, 0.010000, -0.010000, 0.017500, 0.010000, 0.017500, 0.017500, 0.010000, 0.017500, -0.010000, 0.010000, -0.017500, -0.010000, -0.017500};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	sui_draw_gl(GL_LINES, array, 32, 2, red, green, blue);
	glPopMatrix();
}

boolean co_w_checkbox(BInputState *input, float pos_x, float pos_y, boolean *state, float red, float green, float blue)
{
	pos_y += 0.01;
	if(input->mode == BAM_DRAW)
	{
		if(*state)
			sui_draw_check_box_checked(pos_x, pos_y, red, green, blue);
		else
			sui_draw_check_box_unchecked(pos_x, pos_y, red, green, blue);
	}else if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
	{
		if(sui_box_click_test(pos_x - 0.02, pos_y - 0.02, 0.04, 0.04) && sui_box_down_click_test(pos_x - 0.02, pos_y - 0.02, 0.04, 0.04))
		{
			*state = !(*state);
			return TRUE;
		}
	}
	return FALSE;
}

void	sui_draw_symb_slider(float pos_x, float pos_y, float red, float green, float blue)
{
	static float array[] = {-0.010000, -0.030000, -0.000000, -0.020000, -0.000000, 0.020000, -0.010000, 0.030000, -0.000000, 0.020000, 0.010000, 0.030000, -0.010000, 0.060000, 0.010000, 0.060000, -0.010000, -0.060000, -0.010000, -0.030000, -0.000000, -0.020000, 0.010000, -0.030000, 0.010000, -0.030000, 0.010000, -0.060000, 0.010000, -0.060000, -0.010000, -0.060000, -0.010000, 0.060000, -0.010000, 0.030000, 0.010000, 0.060000, 0.010000, 0.030000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	sui_draw_gl(GL_LINES, array, 20, 2, red, green, blue);
	glPopMatrix();
}

void	co_draw_type_in(float pos_x, float pos_y, float dest_x, float dest_y, float red, float green, float blue)
{
	float array[76], vec_x, vec_y, r;
	vec_x = pos_x - dest_x;
	vec_y = dest_y - pos_y;
	r = sqrt(vec_x * vec_x + vec_y * vec_y);
	vec_x /= r;
	vec_y /= r;
	array[0] = dest_x - -0.030000 * vec_x - 0.030000 * vec_y;
	array[1] = dest_y + -0.030000 * vec_y - 0.030000 * vec_x;
	array[2] = pos_x - 0.030000 * vec_x - 0.030000 * vec_y;
	array[3] = pos_y + 0.030000 * vec_y - 0.030000 * vec_x;
	array[4] = pos_x - 0.070000 * vec_x - -0.030000 * vec_y;
	array[5] = pos_y + 0.070000 * vec_y - -0.030000 * vec_x;
	array[6] = pos_x - 0.030000 * vec_x - -0.030000 * vec_y;
	array[7] = pos_y + 0.030000 * vec_y - -0.030000 * vec_x;
	array[8] = pos_x - 0.070000 * vec_x - -0.030000 * vec_y;
	array[9] = pos_y + 0.070000 * vec_y - -0.030000 * vec_x;
	array[10] = pos_x - 0.080000 * vec_x - -0.020000 * vec_y;
	array[11] = pos_y + 0.080000 * vec_y - -0.020000 * vec_x;
	array[12] = dest_x - -0.080000 * vec_x - -0.020000 * vec_y;
	array[13] = dest_y + -0.080000 * vec_y - -0.020000 * vec_x;
	array[14] = dest_x - -0.070000 * vec_x - -0.030000 * vec_y;
	array[15] = dest_y + -0.070000 * vec_y - -0.030000 * vec_x;
	array[16] = dest_x - -0.070000 * vec_x - -0.030000 * vec_y;
	array[17] = dest_y + -0.070000 * vec_y - -0.030000 * vec_x;
	array[18] = dest_x - -0.030000 * vec_x - -0.030000 * vec_y;
	array[19] = dest_y + -0.030000 * vec_y - -0.030000 * vec_x;
	array[20] = dest_x - -0.030000 * vec_x - 0.030000 * vec_y;
	array[21] = dest_y + -0.030000 * vec_y - 0.030000 * vec_x;
	array[22] = dest_x - -0.015000 * vec_x - 0.025981 * vec_y;
	array[23] = dest_y + -0.015000 * vec_y - 0.025981 * vec_x;
	array[24] = dest_x - -0.015000 * vec_x - 0.025981 * vec_y;
	array[25] = dest_y + -0.015000 * vec_y - 0.025981 * vec_x;
	array[26] = dest_x - -0.004019 * vec_x - 0.015000 * vec_y;
	array[27] = dest_y + -0.004019 * vec_y - 0.015000 * vec_x;
	array[28] = dest_x - -0.004019 * vec_x - 0.015000 * vec_y;
	array[29] = dest_y + -0.004019 * vec_y - 0.015000 * vec_x;
	array[30] = dest_x - 0.000000 * vec_x - 0.000000 * vec_y;
	array[31] = dest_y + 0.000000 * vec_y - 0.000000 * vec_x;
	array[32] = pos_x - 0.080000 * vec_x - -0.020000 * vec_y;
	array[33] = pos_y + 0.080000 * vec_y - -0.020000 * vec_x;
	array[34] = dest_x - -0.080000 * vec_x - -0.020000 * vec_y;
	array[35] = dest_y + -0.080000 * vec_y - -0.020000 * vec_x;
	array[36] = pos_x - 0.030000 * vec_x - 0.030000 * vec_y;
	array[37] = pos_y + 0.030000 * vec_y - 0.030000 * vec_x;
	array[38] = pos_x - 0.015000 * vec_x - 0.025981 * vec_y;
	array[39] = pos_y + 0.015000 * vec_y - 0.025981 * vec_x;
	array[40] = pos_x - 0.015000 * vec_x - 0.025981 * vec_y;
	array[41] = pos_y + 0.015000 * vec_y - 0.025981 * vec_x;
	array[42] = pos_x - 0.004019 * vec_x - 0.015000 * vec_y;
	array[43] = pos_y + 0.004019 * vec_y - 0.015000 * vec_x;
	array[44] = pos_x - 0.004019 * vec_x - 0.015000 * vec_y;
	array[45] = pos_y + 0.004019 * vec_y - 0.015000 * vec_x;
	array[46] = pos_x - -0.000000 * vec_x - 0.000000 * vec_y;
	array[47] = pos_y + -0.000000 * vec_y - 0.000000 * vec_x;
	array[48] = pos_x - 0.030000 * vec_x - -0.030000 * vec_y;
	array[49] = pos_y + 0.030000 * vec_y - -0.030000 * vec_x;
	array[50] = pos_x - 0.015000 * vec_x - -0.025981 * vec_y;
	array[51] = pos_y + 0.015000 * vec_y - -0.025981 * vec_x;
	array[52] = pos_x - 0.015000 * vec_x - -0.025981 * vec_y;
	array[53] = pos_y + 0.015000 * vec_y - -0.025981 * vec_x;
	array[54] = pos_x - 0.004019 * vec_x - -0.015000 * vec_y;
	array[55] = pos_y + 0.004019 * vec_y - -0.015000 * vec_x;
	array[56] = pos_x - 0.004019 * vec_x - -0.015000 * vec_y;
	array[57] = pos_y + 0.004019 * vec_y - -0.015000 * vec_x;
	array[58] = pos_x - -0.000000 * vec_x - 0.000000 * vec_y;
	array[59] = pos_y + -0.000000 * vec_y - 0.000000 * vec_x;
	array[60] = dest_x - -0.030000 * vec_x - -0.030000 * vec_y;
	array[61] = dest_y + -0.030000 * vec_y - -0.030000 * vec_x;
	array[62] = dest_x - -0.015000 * vec_x - -0.025981 * vec_y;
	array[63] = dest_y + -0.015000 * vec_y - -0.025981 * vec_x;
	array[64] = dest_x - -0.015000 * vec_x - -0.025981 * vec_y;
	array[65] = dest_y + -0.015000 * vec_y - -0.025981 * vec_x;
	array[66] = dest_x - -0.004019 * vec_x - -0.015000 * vec_y;
	array[67] = dest_y + -0.004019 * vec_y - -0.015000 * vec_x;
	array[68] = dest_x - -0.004019 * vec_x - -0.015000 * vec_y;
	array[69] = dest_y + -0.004019 * vec_y - -0.015000 * vec_x;
	array[70] = dest_x - 0.000000 * vec_x - 0.000000 * vec_y;
	array[71] = dest_y + 0.000000 * vec_y - 0.000000 * vec_x;
	array[72] = dest_x - -0.090000 * vec_x - -0.030000 * vec_y;
	array[73] = dest_y + -0.090000 * vec_y - -0.030000 * vec_x;
	array[74] = pos_x - 0.090000 * vec_x - -0.030000 * vec_y;
	array[75] = pos_y + 0.090000 * vec_y - -0.030000 * vec_x;
	sui_draw_gl(GL_LINES, array, 38, 2, red, green, blue);
}


void	sui_draw_line_char_254(float pos_x, float pos_y, float dest_x, float dest_y, float red, float green, float blue)
{
	float array[100], vec_x, vec_y, r;
	vec_x = pos_x - dest_x;
	vec_y = dest_y - pos_y;
	r = sqrt(vec_x * vec_x + vec_y * vec_y);
	vec_x /= r;
	vec_y /= r;
	array[0] = pos_x - 0.030000 * vec_x - 0.030000 * vec_y;
	array[1] = pos_y + 0.030000 * vec_y - 0.030000 * vec_x;
	array[2] = pos_x - 0.015000 * vec_x - 0.025981 * vec_y;
	array[3] = pos_y + 0.015000 * vec_y - 0.025981 * vec_x;
	array[4] = pos_x - 0.015000 * vec_x - 0.025981 * vec_y;
	array[5] = pos_y + 0.015000 * vec_y - 0.025981 * vec_x;
	array[6] = pos_x - 0.004019 * vec_x - 0.015000 * vec_y;
	array[7] = pos_y + 0.004019 * vec_y - 0.015000 * vec_x;
	array[8] = pos_x - 0.004019 * vec_x - 0.015000 * vec_y;
	array[9] = pos_y + 0.004019 * vec_y - 0.015000 * vec_x;
	array[10] = pos_x - -0.000000 * vec_x - 0.000000 * vec_y;
	array[11] = pos_y + -0.000000 * vec_y - 0.000000 * vec_x;
	array[12] = pos_x - 0.030000 * vec_x - -0.030000 * vec_y;
	array[13] = pos_y + 0.030000 * vec_y - -0.030000 * vec_x;
	array[14] = pos_x - 0.015000 * vec_x - -0.025981 * vec_y;
	array[15] = pos_y + 0.015000 * vec_y - -0.025981 * vec_x;
	array[16] = pos_x - 0.015000 * vec_x - -0.025981 * vec_y;
	array[17] = pos_y + 0.015000 * vec_y - -0.025981 * vec_x;
	array[18] = pos_x - 0.004019 * vec_x - -0.015000 * vec_y;
	array[19] = pos_y + 0.004019 * vec_y - -0.015000 * vec_x;
	array[20] = pos_x - 0.004019 * vec_x - -0.015000 * vec_y;
	array[21] = pos_y + 0.004019 * vec_y - -0.015000 * vec_x;
	array[22] = pos_x - -0.000000 * vec_x - 0.000000 * vec_y;
	array[23] = pos_y + -0.000000 * vec_y - 0.000000 * vec_x;
	array[24] = dest_x - -0.030000 * vec_x - -0.030000 * vec_y;
	array[25] = dest_y + -0.030000 * vec_y - -0.030000 * vec_x;
	array[26] = dest_x - -0.015000 * vec_x - -0.025981 * vec_y;
	array[27] = dest_y + -0.015000 * vec_y - -0.025981 * vec_x;
	array[28] = dest_x - -0.015000 * vec_x - -0.025981 * vec_y;
	array[29] = dest_y + -0.015000 * vec_y - -0.025981 * vec_x;
	array[30] = dest_x - -0.004019 * vec_x - -0.015000 * vec_y;
	array[31] = dest_y + -0.004019 * vec_y - -0.015000 * vec_x;
	array[32] = dest_x - -0.004019 * vec_x - -0.015000 * vec_y;
	array[33] = dest_y + -0.004019 * vec_y - -0.015000 * vec_x;
	array[34] = dest_x - 0.000000 * vec_x - 0.000000 * vec_y;
	array[35] = dest_y + 0.000000 * vec_y - 0.000000 * vec_x;
	array[36] = dest_x - -0.030000 * vec_x - 0.030000 * vec_y;
	array[37] = dest_y + -0.030000 * vec_y - 0.030000 * vec_x;
	array[38] = dest_x - -0.015000 * vec_x - 0.025981 * vec_y;
	array[39] = dest_y + -0.015000 * vec_y - 0.025981 * vec_x;
	array[40] = dest_x - -0.015000 * vec_x - 0.025981 * vec_y;
	array[41] = dest_y + -0.015000 * vec_y - 0.025981 * vec_x;
	array[42] = dest_x - -0.004019 * vec_x - 0.015000 * vec_y;
	array[43] = dest_y + -0.004019 * vec_y - 0.015000 * vec_x;
	array[44] = dest_x - -0.004019 * vec_x - 0.015000 * vec_y;
	array[45] = dest_y + -0.004019 * vec_y - 0.015000 * vec_x;
	array[46] = dest_x - 0.000000 * vec_x - 0.000000 * vec_y;
	array[47] = dest_y + 0.000000 * vec_y - 0.000000 * vec_x;
	array[48] = pos_x - 0.030000 * vec_x - -0.030000 * vec_y;
	array[49] = pos_y + 0.030000 * vec_y - -0.030000 * vec_x;
	array[50] = pos_x - 0.040000 * vec_x - -0.020000 * vec_y;
	array[51] = pos_y + 0.040000 * vec_y - -0.020000 * vec_x;
	array[52] = pos_x - 0.040000 * vec_x - -0.020000 * vec_y;
	array[53] = pos_y + 0.040000 * vec_y - -0.020000 * vec_x;
	array[54] = pos_x - 0.210000 * vec_x - -0.020000 * vec_y;
	array[55] = pos_y + 0.210000 * vec_y - -0.020000 * vec_x;
	array[56] = pos_x - 0.210000 * vec_x - -0.020000 * vec_y;
	array[57] = pos_y + 0.210000 * vec_y - -0.020000 * vec_x;
	array[58] = pos_x - 0.220000 * vec_x - -0.030000 * vec_y;
	array[59] = pos_y + 0.220000 * vec_y - -0.030000 * vec_x;
	array[60] = pos_x - 0.250000 * vec_x - 0.010000 * vec_y;
	array[61] = pos_y + 0.250000 * vec_y - 0.010000 * vec_x;
	array[62] = pos_x - 0.230000 * vec_x - 0.030000 * vec_y;
	array[63] = pos_y + 0.230000 * vec_y - 0.030000 * vec_x;
	array[64] = dest_x - -0.090000 * vec_x - 0.010000 * vec_y;
	array[65] = dest_y + -0.090000 * vec_y - 0.010000 * vec_x;
	array[66] = pos_x - 0.250000 * vec_x - 0.010000 * vec_y;
	array[67] = pos_y + 0.250000 * vec_y - 0.010000 * vec_x;
	array[68] = pos_x - 0.250000 * vec_x - -0.010000 * vec_y;
	array[69] = pos_y + 0.250000 * vec_y - -0.010000 * vec_x;
	array[70] = dest_x - -0.090000 * vec_x - -0.010000 * vec_y;
	array[71] = dest_y + -0.090000 * vec_y - -0.010000 * vec_x;
	array[72] = dest_x - -0.090000 * vec_x - -0.010000 * vec_y;
	array[73] = dest_y + -0.090000 * vec_y - -0.010000 * vec_x;
	array[74] = dest_x - -0.070000 * vec_x - -0.030000 * vec_y;
	array[75] = dest_y + -0.070000 * vec_y - -0.030000 * vec_x;
	array[76] = dest_x - -0.070000 * vec_x - -0.030000 * vec_y;
	array[77] = dest_y + -0.070000 * vec_y - -0.030000 * vec_x;
	array[78] = dest_x - -0.030000 * vec_x - -0.030000 * vec_y;
	array[79] = dest_y + -0.030000 * vec_y - -0.030000 * vec_x;
	array[80] = dest_x - -0.030000 * vec_x - 0.030000 * vec_y;
	array[81] = dest_y + -0.030000 * vec_y - 0.030000 * vec_x;
	array[82] = dest_x - -0.070000 * vec_x - 0.030000 * vec_y;
	array[83] = dest_y + -0.070000 * vec_y - 0.030000 * vec_x;
	array[84] = dest_x - -0.070000 * vec_x - 0.030000 * vec_y;
	array[85] = dest_y + -0.070000 * vec_y - 0.030000 * vec_x;
	array[86] = dest_x - -0.090000 * vec_x - 0.010000 * vec_y;
	array[87] = dest_y + -0.090000 * vec_y - 0.010000 * vec_x;
	array[88] = pos_x - 0.250000 * vec_x - -0.010000 * vec_y;
	array[89] = pos_y + 0.250000 * vec_y - -0.010000 * vec_x;
	array[90] = pos_x - 0.230000 * vec_x - -0.030000 * vec_y;
	array[91] = pos_y + 0.230000 * vec_y - -0.030000 * vec_x;
	array[92] = pos_x - 0.230000 * vec_x - -0.030000 * vec_y;
	array[93] = pos_y + 0.230000 * vec_y - -0.030000 * vec_x;
	array[94] = pos_x - 0.220000 * vec_x - -0.030000 * vec_y;
	array[95] = pos_y + 0.220000 * vec_y - -0.030000 * vec_x;
	array[96] = pos_x - 0.230000 * vec_x - 0.030000 * vec_y;
	array[97] = pos_y + 0.230000 * vec_y - 0.030000 * vec_x;
	array[98] = pos_x - 0.030000 * vec_x - 0.030000 * vec_y;
	array[99] = pos_y + 0.030000 * vec_y - 0.030000 * vec_x;
	sui_draw_gl(GL_LINES, array, 50, 2, red, green, blue);
}


boolean co_w_slider(BInputState *input, float pos_x, float pos_y, float length, double *nr, float red, float green, float blue)
{
	if(input->mode == BAM_DRAW)
	{
		float f;
		sui_draw_line3(pos_x, pos_y + 0.01, pos_x + length, pos_y + 0.01, red, green, blue);
		f = *nr;
		if(f > 1)
			f = 1;
		if(f < 0)
			f = 0;
		sui_draw_slider_line(pos_x + length * f, pos_y + 0.01, red, green, blue);
	}else
	{
		static double *drag = NULL;
		if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
			if(sui_box_click_test(pos_x/* + 0.22*/, pos_y + -0.025, (length/* - 0.04 - 0.22*/), 0.05))
				drag = nr;
		if(input->mouse_button[0] != TRUE)
			drag = NULL;
		if(drag == nr)
		{
			if(input->pointer_x < pos_x)
				*nr = 0;
			else if(input->pointer_x > pos_x + length)
				*nr = 1;
			else
				*nr = (input->pointer_x - pos_x) / length;
			return TRUE;
		}
	}
	return FALSE;
}



boolean co_w_type_in(BInputState *input, float pos_x, float pos_y, float length, float size, char *text, uint buffer_size, void (*done_func)(void *user, char *text), void* user, float text_color, float line_color)
{
	if(input->mode == BAM_DRAW)
		sui_draw_2d_line_gl(pos_x, pos_y - size * 0.25, pos_x + length, pos_y - size * 0.25,line_color, line_color, line_color);
	return sui_type_in(input, pos_x, pos_y, length, size, text, buffer_size, done_func, user, text_color, text_color, text_color);
}

void sui_draw_rounded_square(float pos_x, float pos_y, float size_x, float size_y, float red, float green, float blue)
{
	float array[64];
	size_x += pos_x;
	size_y += pos_y;
	array[0] = size_x + -0.005000;
	array[1] = pos_y + 0.000000;
	array[2] = pos_x + 0.005000;
	array[3] = pos_y + 0.000000;
	array[4] = pos_x + -0.000000;
	array[5] = pos_y + -0.005000;
	array[6] = pos_x + -0.000000;
	array[7] = size_y + 0.005000;
	array[8] = pos_x + 0.005000;
	array[9] = size_y + -0.000000;
	array[10] = size_x + -0.005000;
	array[11] = size_y + -0.000000;
	array[12] = size_x + 0.000000;
	array[13] = size_y + 0.005000;
	array[14] = size_x + 0.000000;
	array[15] = pos_y + -0.005000;
	array[16] = pos_x + 0.005000;
	array[17] = pos_y + 0.000000;
	array[18] = pos_x + 0.002500;
	array[19] = pos_y + -0.000670;
	array[20] = pos_x + 0.002500;
	array[21] = pos_y + -0.000670;
	array[22] = pos_x + 0.000670;
	array[23] = pos_y + -0.002500;
	array[24] = pos_x + 0.000670;
	array[25] = pos_y + -0.002500;
	array[26] = pos_x + -0.000000;
	array[27] = pos_y + -0.005000;
	array[28] = size_x + -0.005000;
	array[29] = pos_y + 0.000000;
	array[30] = size_x + -0.002500;
	array[31] = pos_y + -0.000670;
	array[32] = size_x + -0.002500;
	array[33] = pos_y + -0.000670;
	array[34] = size_x + -0.000670;
	array[35] = pos_y + -0.002500;
	array[36] = size_x + -0.000670;
	array[37] = pos_y + -0.002500;
	array[38] = size_x + 0.000000;
	array[39] = pos_y + -0.005000;
	array[40] = size_x + -0.005000;
	array[41] = size_y + -0.000000;
	array[42] = size_x + -0.002500;
	array[43] = size_y + 0.000670;
	array[44] = size_x + -0.002500;
	array[45] = size_y + 0.000670;
	array[46] = size_x + -0.000670;
	array[47] = size_y + 0.002500;
	array[48] = size_x + -0.000670;
	array[49] = size_y + 0.002500;
	array[50] = size_x + 0.000000;
	array[51] = size_y + 0.005000;
	array[52] = pos_x + 0.005000;
	array[53] = size_y + -0.000000;
	array[54] = pos_x + 0.002500;
	array[55] = size_y + 0.000670;
	array[56] = pos_x + 0.002500;
	array[57] = size_y + 0.000670;
	array[58] = pos_x + 0.000670;
	array[59] = size_y + 0.002500;
	array[60] = pos_x + 0.000670;
	array[61] = size_y + 0.002500;
	array[62] = pos_x + -0.000000;
	array[63] = size_y + 0.005000;
	sui_draw_gl(GL_LINES, array, 32, 2, red, green, blue);
}


void	sui_draw_symb_close(float pos_x, float pos_y, float red, float green, float blue)
{
	static float array[] = {-0.002500, -0.002500, -0.010000, -0.010000, -0.000000, -0.017500, 0.005408, -0.016643, 0.005408, -0.016643, 0.010286, -0.014158, 0.010286, -0.014158, 0.014158, -0.010286, 0.014158, -0.010286, 0.016643, -0.005408, 0.016643, -0.005408, 0.017500, 0.000000, -0.000000, -0.017500, -0.005408, -0.016643, -0.005408, -0.016643, -0.010286, -0.014158, -0.010286, -0.014158, -0.014158, -0.010286, -0.014158, -0.010286, -0.016643, -0.005408, -0.016643, -0.005408, -0.017500, 0.000000, -0.000000, 0.017500, -0.005408, 0.016643, -0.005408, 0.016643, -0.010286, 0.014158, -0.010286, 0.014158, -0.014158, 0.010286, -0.014158, 0.010286, -0.016643, 0.005408, -0.016643, 0.005408, -0.017500, 0.000000, 0.002500, 0.002500, 0.010000, 0.010000, -0.000000, 0.017500, 0.005408, 0.016643, 0.005408, 0.016643, 0.010286, 0.014158, 0.010286, 0.014158, 0.014158, 0.010286, 0.014158, 0.010286, 0.016643, 0.005408, 0.016643, 0.005408, 0.017500, 0.000000, 0.002500, -0.002500, 0.010000, -0.010000, -0.002500, 0.002500, -0.010000, 0.010000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	glScalef(0.5, 0.5, 0.5);
	sui_draw_gl(GL_LINES, array, 48, 2, red, green, blue);
	glPopMatrix();
}

boolean co_w_close_button(BInputState *input, float pos_x, float pos_y, float red, float green, float blue)
{
	pos_x += 0.025;
	if(input->mode == BAM_DRAW)
		sui_draw_symb_close(pos_x, pos_y, red, green, blue);
	else if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE)
		if(0.025 * 0.025 > (pos_x - input->pointer_x) * (pos_x - input->pointer_x) + (pos_y - input->pointer_y) * (pos_y - input->pointer_y))
			if(0.025 * 0.025 > (pos_x - input->click_pointer_x) * (pos_x - input->click_pointer_x) + (pos_y - input->click_pointer_y) * (pos_y - input->click_pointer_y))
				return TRUE;
	return FALSE;
}


float co_w_scroll(BInputState *input, float pos, float size)
{
	static boolean drag = FALSE;
	float aspect, p;
	aspect = betray_get_screen_mode(NULL, NULL, NULL);
	p = (1 - pos / -size) * aspect;
/*	if(size < aspect)
		return pos;
	pos /= aspect;
	size /= aspect;*/
	if(input->mode == BAM_DRAW)
	{
		sui_draw_2d_line_gl(0.98, p, 0.98, p - 5.0 / (size * aspect), 0.5, 0.5, 0.5);
		sui_draw_2d_line_gl(0.97, p, 0.97, p - 5.0 / (size * aspect), 0.5, 0.5, 0.5);
		sui_draw_2d_line_gl(0.96, p, 0.96, p - 5.0 / (size * aspect), 0.5, 0.5, 0.5);
	}
	else
	{
		if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE)
		{
			if(input->pointer_x > 0.95 && input->pointer_y < p)
				pos += (5.0 / (size * aspect)) / aspect * -size;
			if(input->pointer_x > 0.95 && input->pointer_y > p - 4.0 / (size * aspect))
				pos -= (5.0 / (size * aspect)) / aspect * -size;
		}
		if(input->mouse_button[0] == TRUE && input->click_pointer_x > 0.95 && input->pointer_y < p && input->pointer_y > p - 4.0 / (size * aspect))
			drag = TRUE;
		if(drag)
			pos += input->delta_pointer_y * size / aspect;
		if(input->mouse_button[0] == FALSE || input->last_mouse_button[0] == FALSE)
			drag = FALSE;
	//	pos = (-1 + (p / aspect) * -1) * -size;
	//	p = (1 - -pos / size) * aspect;
	}
	return pos;
}
