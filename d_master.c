#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "enough.h"
#include "seduce.h"

#define MAX_ADDRESS_SIZE 64
#define MAX_MESSAGE_SIZE 256
#define MESSAGE_CHUNK_SIZE 256

typedef enum{
	D_CM_URL,
	D_CM_HISTORY,
	D_CM_FAVORITES,
	D_CM_LIST,
	D_CM_ADD_FAVORITES,
	D_CM_CONNECTING,
	D_CM_NAME_PASS
}DConnectMode;

typedef struct{
	char address[MAX_ADDRESS_SIZE];
	char message[MAX_MESSAGE_SIZE];
	char name[V_MAX_NAME_PASS_LENGTH];
	char pass[V_MAX_NAME_PASS_LENGTH];
	boolean favorites;
}DMasterAddress;

static DMasterAddress *d_address_list[4] = {NULL, NULL, NULL, NULL};
static uint d_address_count[4] = {0, 0, 0, 0};

static float d_view[6] = {0, 0, 0, 0, 0, 0};
static DConnectMode d_current_mode = D_CM_URL;

static char d_master_address[MAX_ADDRESS_SIZE] = "";

static void d_add_address(DConnectMode mode, const char *address, const char *description, const char *name, const char *pass)
{
	DMasterAddress *a;
	uint i, j, k;
	if(mode == D_CM_FAVORITES)
	{
		for(i = 0; i < 4; i++)
		{
			if(i != D_CM_FAVORITES)
			{
				for(j = 0; j < d_address_count[i]; j++)
				{
					a = &d_address_list[i][j];
					for(k = 0; a->address[k] == address[k] && address[k] != 0; k++);
					if(a->address[k] == address[k])
						a->favorites = TRUE;
				}
			}		
		}
	}else
	{
		for(j = 0; j < d_address_count[D_CM_FAVORITES]; j++)
		{
			a = &d_address_list[D_CM_FAVORITES][j];
			for(k = 0; a->address[k] == address[k] && address[k] != 0; k++);
			if(a->address[k] == address[k])
				a->favorites = TRUE;
		}
	}	

	if(d_address_count[mode] % MESSAGE_CHUNK_SIZE == 0)
		d_address_list[mode] = realloc(d_address_list[mode], (sizeof *d_address_list[mode]) * (d_address_count[mode] + MESSAGE_CHUNK_SIZE));
	a = &d_address_list[mode][d_address_count[mode]];
	for(i = 0; i < MAX_ADDRESS_SIZE - 1 && address[i] != 0;i++)
		a->address[i] = address[i];
	a->address[i] = 0;
	for(i = 0; i < MAX_MESSAGE_SIZE - 1 && description[i] != 0;i++)
		a->message[i] = description[i];
	a->message[i] = 0;

	for(i = 0; i < V_MAX_NAME_PASS_LENGTH - 1 && name[i] != 0;i++)
		a->name[i] = name[i];
	a->name[i] = 0;

	for(i = 0; i < V_MAX_NAME_PASS_LENGTH - 1 && pass[i] != 0;i++)
		a->pass[i] = pass[i];
	a->pass[i] = 0;
	a->favorites = mode == D_CM_FAVORITES;
	d_address_count[mode]++;
}

static void d_remove_favorite(const char *address, boolean specific, uint id)
{
	DMasterAddress *a;
	uint i, j, k;
	if(specific)
	{
		for(j = id; j != d_address_count[D_CM_FAVORITES] - 1; j++)
			d_address_list[D_CM_FAVORITES][j] = d_address_list[D_CM_FAVORITES][j + 1];
		d_address_count[D_CM_FAVORITES]--;
		for(i = 0; i < d_address_count[D_CM_FAVORITES]; i++)
		{
			a = &d_address_list[D_CM_FAVORITES][i];
			for(j = 0; a->address[j] == address[j] && address[j] != 0; j++);
			if(a->address[j] == address[j])
				return;
		}
	}else	
	{
		for(i = 0; i < d_address_count[D_CM_FAVORITES]; i++)
		{
			a = &d_address_list[D_CM_FAVORITES][i];
			for(j = 0; a->address[j] == address[j] && address[j] != 0; j++);
			if(a->address[j] == address[j])
			{
				for(j = i; j != d_address_count[D_CM_FAVORITES] - 1; j++)
					d_address_list[D_CM_FAVORITES][j] = d_address_list[D_CM_FAVORITES][j + 1];
				d_address_count[D_CM_FAVORITES]--;
			}
		}
	}
	for(i = 0; i < 4; i++)
	{
		if(i != D_CM_FAVORITES)
		{
			for(j = 0; j < d_address_count[i]; j++)
			{
				a = &d_address_list[i][j];
				for(k = 0; a->address[k] == address[k] && address[k] != 0; k++);
				if(a->address[k] == address[k])
					a->favorites = FALSE;
			}
		}
	}
}

static void d_add_address_to_history(const char *address, const char *description, const char *name, const char *pass)
{
	DMasterAddress *a;
	uint i, j;
	for(i = 0; i < d_address_count[D_CM_HISTORY] ;i++)
	{
		for(j = 0; j < MAX_ADDRESS_SIZE - 1 && address[j] != 0 && address[j] == d_address_list[D_CM_HISTORY][i].address[j]; j++);
		if(address[j] == d_address_list[D_CM_HISTORY][i].address[j])
			break;
	}
	if(i == d_address_count[D_CM_HISTORY])
	{
		if(d_address_count[D_CM_HISTORY] % MESSAGE_CHUNK_SIZE == 0)
			d_address_list[D_CM_HISTORY] = realloc(d_address_list[D_CM_HISTORY], (sizeof *d_address_list[D_CM_HISTORY]) * (d_address_count[D_CM_HISTORY] + MESSAGE_CHUNK_SIZE));
		d_address_count[D_CM_HISTORY]++;
	}

	for(j = i; j != 0; j--)
		d_address_list[D_CM_HISTORY][j] = d_address_list[D_CM_HISTORY][j - 1];

	a = &d_address_list[D_CM_HISTORY][0];
	for(i = 0; i < MAX_ADDRESS_SIZE - 1 && address[i] != 0;i++)
		a->address[i] = address[i];
	a->address[i] = 0;
	for(i = 0; i < MAX_MESSAGE_SIZE - 1 && address[i] != 0;i++)
		a->message[i] = description[i];
	a->message[i] = 0;
	for(i = 0; i < V_MAX_NAME_PASS_LENGTH - 1 && name[i] != 0;i++)
		a->name[i] = name[i];
	a->name[i] = 0;
	for(i = 0; i < V_MAX_NAME_PASS_LENGTH - 1 && pass[i] != 0;i++)
		a->pass[i] = pass[i];
	a->pass[i] = 0;
}

static void d_typein_done_func(void *user, char *text)
{
	d_add_address_to_history(text, text, "somename", "somepass");
	e_vc_connect(text, "somename", "somepass", NULL);
}

static void callback_send_ping(void *user, const char *address, const char *message)
{
	char *ms_list = "MS:LIST";
	char *ip = "IP=";
	char *de = "DE=\"";
	char add[24] = {0}, description[MAX_MESSAGE_SIZE] = {0};
	uint i, j;
	printf("%s\n", message);
	for(i = 0; ms_list[i] != 0 && ms_list[i] == message[i]; i++);
	if(ms_list[i] == 0)
	{
		j = i;
		for(j = 0; message[j] != 0; j++)
		{
			for(i = 0; ip[i] != 0 && ip[i] == message[i + j]; i++);
			if(ip[i] == 0)
			{
				j += i;
				if(add[0] != 0)
					d_add_address(D_CM_LIST, add, description, "name", "pass");
				for(i = 0; i < MAX_ADDRESS_SIZE - 1 && message[i + j] != 0 && message[i + j] != 32; i++)
					add[i] = message[i + j];
				add[i] = 0;
				description[0] = 0;
				j += i;
			}
			for(i = 0; de[i] != 0 && de[i] == message[i + j]; i++);
			if(de[i] == 0 && d_address_count != 0)
			{
				j += i;
				for(i = 0; i < MAX_MESSAGE_SIZE - 1 && message[i + j] != 0 && message[i + j] != '"'; i++)
					description[i] = message[i + j];
				description[i] = 0;
				j += i;
			}
		}
		if(add[0] != 0)
			d_add_address(D_CM_LIST, add, description, "name", "pass");
	}
}

static void d_save_address_settings(void)
{
	uint i, j, k;
	FILE *f;

	if((f = fopen("address.bin", "wb")) != NULL)
	{	
		fputc((char)d_current_mode, f);
		for(i = 0; d_master_address[i] != 0 && i < MAX_ADDRESS_SIZE - 1; i++)
			fputc(d_master_address[i], f);
		fputc(0, f);

		for(i = 0; i < 4; i++)
		{
			fputc(d_address_count[i] % 256, f);
			fputc(d_address_count[i] / 256, f);
			for(j = 0; j < d_address_count[i]; j++)
			{
				for(k = 0; d_address_list[i][j].address[k] != 0 && k < MAX_ADDRESS_SIZE - 1; k++)
					fputc(d_address_list[i][j].address[k], f);
				fputc(0, f);

				for(k = 0; d_address_list[i][j].message[k] != 0 && k < MAX_MESSAGE_SIZE - 1; k++)
					fputc(d_address_list[i][j].message[k], f);
				fputc(0, f);

				for(k = 0; d_address_list[i][j].name[k] != 0 && k < V_MAX_NAME_PASS_LENGTH - 1; k++)
					fputc(d_address_list[i][j].name[k], f);
				fputc(0, f);

				for(k = 0; d_address_list[i][j].pass[k] != 0 && k < V_MAX_NAME_PASS_LENGTH - 1; k++)
					fputc(d_address_list[i][j].pass[k], f);
				fputc(0, f);
			}
		}
		fclose(f);
	}
}

static void d_load_address_settings(void)
{
	char *default_master = "master.uni-verse.org:4950", a[256], b[256], c[256], d[256];
	uint i, j, k, count;
	FILE *f;

	if((f = fopen("address.bin", "rb")) != NULL)
	{	
		d_current_mode = fgetc(f);
		for(i = 0; i < sizeof d_master_address && (d_master_address[i] = fgetc(f)) != 0; i++);
		for(i = 0; i < 4; i++)
		{
			count = (uint8)fgetc(f);
			count += (uint8)fgetc(f) * 256;
			for(j = 0; j < count; j++)
			{
				for(k = 0; (a[k] = fgetc(f)) != 0; k++);
				for(k = 0; (b[k] = fgetc(f)) != 0; k++);
				for(k = 0; (c[k] = fgetc(f)) != 0; k++);
				for(k = 0; (d[k] = fgetc(f)) != 0; k++);
				d_add_address(i, a, b, c, d);
			}
		}
		fclose(f);
	}else
	{
		for(i = 0; default_master[i] != 0; i++)
			d_master_address[i] = default_master[i];
		d_master_address[i] = 0;
	}
/**/
	for(i = 0; default_master[i] != 0; i++)
		d_master_address[i] = default_master[i];
	d_master_address[i] = 0;
/**/
	verse_callback_set(verse_send_ping, callback_send_ping, NULL);
	d_view[d_current_mode] = 1.0;
}

void d_init_master(void)
{
	d_load_address_settings();
}

static void d_draw_star(float x, float y, float color)
{
	static const float star_data[40] = {0.000000, 0.015000, 0.005290, 0.007281, 0.005290, 0.007281, 0.014266, 0.004635, 0.014266, 0.004635,
		0.008560, -0.002781, 0.008560, -0.002781, 0.008817, -0.012135, 0.008817, -0.012135, 0.000000, -0.009000, 0.000000, -0.009000,
		-0.008817, -0.012135, -0.008817, -0.012135, -0.008560, -0.002781, -0.008560, -0.002781, -0.014266, 0.004635, -0.014266, 0.004635,
		-0.005290, 0.007281, -0.005290, 0.007281, -0.000000, 0.015000};
	glPushMatrix();
	glTranslatef(x, y, 0);
	sui_draw_gl(GL_LINES, star_data, 20, 2, 0.0, 0.0, 0.0, color);
	glPopMatrix();
}

extern void sui_draw_window(double x_pos, double y_pos, double width, double height, float color, char *title);

void d_draw_login(BInputState *input)
{
	static float y_size = 0, x_size = 1.0, line_start = 0, line_end = 0;
	float delta = 0.01, f, aspect;
	static char name[64] = {0};
	uint i, j;
		
	aspect = betray_get_screen_mode(NULL, NULL, NULL);
	for(i = 0; i < 6; i++)
	{
		if(d_current_mode == i)
			d_view[i] += delta * 1;
		else
			d_view[i] -= delta * 1;
		if(d_view[i] > 1.0)
			d_view[i] = 1.0;
		if(d_view[i] < 0.0)
			d_view[i] = 0.0;
	}
	y_size = d_view[D_CM_URL] * 0.050 + d_view[D_CM_HISTORY] * 0.2 + d_view[D_CM_FAVORITES] * 0.3 + d_view[D_CM_LIST] * 0.4 + d_view[D_CM_ADD_FAVORITES] * 0.1125 + d_view[D_CM_CONNECTING] * 0.050;
	x_size = d_view[D_CM_URL] * 0.600 + d_view[D_CM_HISTORY] * 0.7 + d_view[D_CM_FAVORITES] * 0.7 + d_view[D_CM_LIST] * 1.9 + d_view[D_CM_ADD_FAVORITES] * 0.600 + d_view[D_CM_CONNECTING] * 0.600;

	if(input->mouse_button[1])
	{
		x_size = input->pointer_x;
		y_size = input->pointer_y;
	}

	line_start = d_view[D_CM_URL] * -0.134 + d_view[D_CM_HISTORY] * -0.051 + d_view[D_CM_FAVORITES] * 0.088 + d_view[D_CM_LIST] * 0.26 + d_view[D_CM_ADD_FAVORITES] * 0.088; 
	line_end = d_view[D_CM_URL] * -0.086 + d_view[D_CM_HISTORY] * 0.056 + d_view[D_CM_FAVORITES] * 0.23 + d_view[D_CM_LIST] * 0.314 + d_view[D_CM_ADD_FAVORITES] * 0.23; 

	if(input->mode == BAM_DRAW)
	{
		sui_set_blend_gl(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		sui_draw_window(0, y_size, x_size, y_size * 2, 1, "CONNECT");
		if(d_view[D_CM_ADD_FAVORITES] < 0.9)
			sui_draw_2d_line_gl(line_start, y_size - 0.06, line_end, y_size - 0.06, 0, 0, 0, d_view[D_CM_CONNECTING]);
	}

	if(d_view[D_CM_ADD_FAVORITES] < 0.9 && d_view[D_CM_CONNECTING] < 0.9)
	{
		if(sw_text_button(input, -0.315, y_size - 0.05, 0, SUI_T_SIZE, SUI_T_SPACE, "LOCALHOST", 0.0, 0.0, 0.0, 1.0 - d_view[D_CM_CONNECTING]))
		{
			d_add_address_to_history("localhost", "Anonymous localhost", "anonymous", "nopass");
			e_vc_connect("localhost", "anonymous", "nopass", NULL);
			d_save_address_settings();
			d_current_mode = D_CM_CONNECTING;
		}
		if(sw_text_button(input, -0.13, y_size - 0.05, 0, SUI_T_SIZE, SUI_T_SPACE, "URL", 0.0, 0.0, 0.0, 1.0 - d_view[D_CM_CONNECTING]))
			d_current_mode = D_CM_URL;
		if(sw_text_button(input, -0.047, y_size - 0.05, 0, SUI_T_SIZE, SUI_T_SPACE, "HISTORY", 0.0, 0.0, 0.0, 1.0 - d_view[D_CM_CONNECTING]))
			d_current_mode = D_CM_HISTORY;
		if(sw_text_button(input, 0.092, y_size - 0.05, 0, SUI_T_SIZE, SUI_T_SPACE, "FAVORITES", 0.0, 0.0, 0.0, 1.0 - d_view[D_CM_CONNECTING]))
			d_current_mode = D_CM_FAVORITES;
		if(sw_text_button(input, 0.264, y_size - 0.05, 0, SUI_T_SIZE, SUI_T_SPACE, "LIST", 0.0, 0.0, 0.0, 1.0 - d_view[D_CM_CONNECTING]))
		{
			d_current_mode = D_CM_LIST;
			if(d_address_count[D_CM_LIST] == 0)
				verse_send_ping(d_master_address, "MS:GET IP=DE");
		}
	}

	if(d_view[D_CM_URL] > 0.1)
	{
		sui_draw_text(-x_size / 2 - 0.015 , -y_size, SUI_T_SIZE, SUI_T_SPACE, "URL:", 0.0, 0.0, 0.0, d_view[D_CM_URL]);
		sui_type_in(input, -0.260, -0.05, 1.0, SUI_T_SIZE, name, 64, d_typein_done_func, NULL, 0.0, 0.0, 0.0, d_view[D_CM_URL]);
		sui_draw_2d_line_gl(-x_size / 2 + 0.045, -y_size - 0.01, x_size / 2 + 0.01, -y_size - 0.01, 0.0, 0.0, 0.0, d_view[D_CM_URL]);
	}

	if(d_view[D_CM_CONNECTING] > 0.1)
	{
		sui_draw_text(-0.085, 0.0, SUI_T_SIZE, SUI_T_SPACE, "CONNECTING . . .", 0, 0, 0, 1);
		if(sw_text_button(input, x_size / 2 - 0.095, -y_size + 0.0, 0, SUI_T_SIZE, SUI_T_SPACE, "CANCEL", 0.0, 0.0, 0.0, d_view[D_CM_CONNECTING]))
		{
			d_current_mode = 0;
			e_vc_disconnect_all();
		}
//		if(e_vc_check_rejected_slot(0))
//			d_current_mode = D_CM_NAME_PASS;
	}

	if(d_view[D_CM_URL] < 0.9 && d_view[D_CM_ADD_FAVORITES] < 0.9 && d_view[D_CM_CONNECTING] < 0.9)
	{
		if(d_view[D_CM_URL] > d_view[D_CM_ADD_FAVORITES])
			f = d_view[D_CM_URL];
		else
			f = d_view[D_CM_ADD_FAVORITES];
		if(f < d_view[D_CM_CONNECTING])
			f = d_view[D_CM_CONNECTING];
		if(input->mode == BAM_DRAW)
		{
			sui_draw_2d_line_gl(x_size / 2, y_size - 0.075, -x_size / 2, y_size - 0.075, 0, 0, 0, 1 - f);
			sui_draw_2d_line_gl(x_size / 2, -y_size + 0.025, -x_size / 2, -y_size + 0.025, 0, 0, 0, 1 - f);
		}
	}
	if(d_view[D_CM_LIST] > 0.1)
	{
		if(sw_text_button(input, -x_size / 2, -y_size + 0.0, 0, SUI_T_SIZE, SUI_T_SPACE, "REFRESH", 0.0, 0.0, 0.0, d_view[D_CM_LIST]))
		{
			if(d_address_list[D_CM_LIST] != NULL)
				free(d_address_list[D_CM_LIST]);
			d_address_list[D_CM_LIST] = NULL;
			d_address_count[D_CM_LIST] = 0;
			verse_send_ping(d_master_address, "MS:GET IP=DE");
		}
/*		if(sw_text_button(input, -x_size / 2 + 0.15 , -y_size + 0.0, 0, SUI_T_SIZE, SUI_T_SPACE, "PING", 1.0 - d_view[D_CM_LIST], 1.0 - d_view[D_CM_LIST], 1.0 - d_view[D_CM_LIST]))
			;
*/		sui_draw_text(-x_size / 2 + 0.25, -y_size + 0, SUI_T_SIZE, SUI_T_SPACE, "MASTER SERVER :", 0.0, 0.0, 0.0, d_view[D_CM_LIST]);
		sui_type_in(input, -x_size / 2 + 0.47, -y_size + 0, 1.0, SUI_T_SIZE, d_master_address, 64, NULL, NULL, 0.0, 0.0, 0.0, d_view[D_CM_LIST]);
		if(input->mode == BAM_DRAW)
		{
			sui_draw_2d_line_gl(-x_size / 2 + 0.47, -y_size - 0.01, x_size / 2, -y_size - 0.01, 1.0 - d_view[D_CM_LIST] * 0.25, 1.0 - d_view[D_CM_LIST] * 0.25, 1.0 - d_view[D_CM_LIST] * 0.25, 0);

			/* Betray mouse debugging aid (render cross hair at mouse position).
			sui_draw_2d_line_gl(input->pointer_x, input->pointer_y - 0.1f, input->pointer_x, input->pointer_y + 0.1f, 1.0f, 0.0f, 0.0f, 1.0f);
			sui_draw_2d_line_gl(input->pointer_x - 0.1f, input->pointer_y, input->pointer_x + 0.1f, input->pointer_y, 1.0f, 0.0f, 0.0f, 1.0f);
*/		}
	}

	for(i = 1; i < 4; i++)
	{
		if(d_view[i] > 0.1 && i != d_current_mode)
		{
			for(j = 0; j < d_address_count[i] && j < (uint)((y_size - 0.05) / 0.025); j++)
			{
				sw_text_button(input, x_size * -0.5, y_size - 0.05 * (float)j - 0.1, 0, SUI_T_SIZE, SUI_T_SPACE, d_address_list[i][j].message, 0.0, 0.0, 0.0, d_view[i]);
				sw_text_button(input, x_size * 0.1, y_size - 0.05 * (float)j - 0.1, 0, SUI_T_SIZE, SUI_T_SPACE, d_address_list[i][j].address, 0.0, 0.0, 0.0, d_view[i]);
			}
		}
	}
	if(d_current_mode != D_CM_URL && d_current_mode != D_CM_ADD_FAVORITES && d_current_mode != D_CM_CONNECTING && d_current_mode != D_CM_NAME_PASS)
	{
		for(j = 0; j < d_address_count[d_current_mode] && j < (uint)((y_size - 0.05) / 0.025); j++)
		{
			if(input->mode == BAM_DRAW)
			{
				if(d_address_list[d_current_mode][j].favorites)
					d_draw_star(x_size * 0.5 - 0.01, y_size - 0.05 * (float)j - 0.1, 1.0);
				else
					d_draw_star(x_size * 0.5 - 0.01, y_size - 0.05 * (float)j - 0.1, 0.5);
			}else if(input->last_mouse_button[0] == FALSE && input->mouse_button[0] == TRUE)
			{
				float x, y;
				x = x_size * 0.5 - 0.01 - input->pointer_x;
				y = y_size - 0.05 * (float)j - 0.1 - input->pointer_y;
				if(x * x + y * y < 0.015 * 0.015)
				{
					if(d_address_list[d_current_mode][j].favorites)
						d_remove_favorite(d_address_list[d_current_mode][j].address, d_current_mode == D_CM_ADD_FAVORITES, j);
					else
						d_add_address(D_CM_FAVORITES, d_address_list[d_current_mode][j].address, d_address_list[d_current_mode][j].message,
							      d_address_list[d_current_mode][j].name, d_address_list[d_current_mode][j].pass);
				}
			}
			if(sw_text_button(input, x_size * -0.5, y_size - 0.05 * (float)j - 0.1, 0, SUI_T_SIZE, SUI_T_SPACE, d_address_list[d_current_mode][j].message, 0.0, 0.0, 0.0, d_view[d_current_mode]) ||
			   sw_text_button(input, x_size * 0.1, y_size - 0.05 * (float)j - 0.1, 0, SUI_T_SIZE, SUI_T_SPACE, d_address_list[d_current_mode][j].address, 0.0, 0.0, 0.0, d_view[d_current_mode]))
			{
				e_vc_connect(d_address_list[d_current_mode][j].address, d_address_list[d_current_mode][j].name, d_address_list[d_current_mode][j].pass, NULL);
				d_add_address_to_history(d_address_list[d_current_mode][j].address, d_address_list[d_current_mode][j].message, d_address_list[d_current_mode][j].name, d_address_list[d_current_mode][j].pass);
				d_save_address_settings();
				d_current_mode = D_CM_CONNECTING;
				break;	/* Don't stay in the loop, since prereq just broke. */
			}
		}
	}
	if(d_view[D_CM_HISTORY] > 0.1 && d_address_list[D_CM_HISTORY] != NULL)
	{
		if(sw_text_button(input, -x_size / 2, -y_size + 0.0, 0, SUI_T_SIZE, SUI_T_SPACE, "CLEAR", 0.0, 0.0, 0.0, d_view[D_CM_HISTORY]))
		{
			free(d_address_list[D_CM_HISTORY]);
			d_address_list[D_CM_HISTORY] = NULL;
			d_address_count[D_CM_HISTORY] = 0;
			d_save_address_settings();
		}
	}

	if(d_view[D_CM_FAVORITES] > 0.1)
	{
		if(sw_text_button(input, -x_size / 2, -y_size + 0.0, 0, SUI_T_SIZE, SUI_T_SPACE, "ADD", 0.0, 0.0, 0.0, d_view[D_CM_FAVORITES]))
			if(d_view[D_CM_FAVORITES] > 0.5)
				d_current_mode = D_CM_ADD_FAVORITES;
	}
	if(d_current_mode == D_CM_ADD_FAVORITES)
	{
		static char address[48] = {0};
		static char description[MAX_MESSAGE_SIZE] = {0};
		static char name[V_MAX_NAME_PASS_LENGTH] = {0};
		static char pass[V_MAX_NAME_PASS_LENGTH] = {0};

		sui_draw_text(-0.315, 0.075, SUI_T_SIZE, SUI_T_SPACE, "DESCRIPTION :", 0, 0, 0, d_view[D_CM_ADD_FAVORITES]);
		sui_type_in(input, -0.140, 0.075, 1.0, SUI_T_SIZE, description, MAX_MESSAGE_SIZE, NULL, NULL, 0, 0, 0, d_view[D_CM_ADD_FAVORITES]);
		sui_draw_2d_line_gl(-0.150, 0.065, 0.315, 0.065, 0, 0, 0, 0);

		sui_draw_text(-0.315, 0.025, SUI_T_SIZE, SUI_T_SPACE, "URL :", 0, 0, 0, d_view[D_CM_ADD_FAVORITES]);
		sui_type_in(input, -0.250, 0.025, 1.0, SUI_T_SIZE, address, 48, NULL, NULL, 0, 0, 0, d_view[D_CM_ADD_FAVORITES]);
		sui_draw_2d_line_gl(-0.260, 0.015, 0.315, 0.015, 0, 0, 0, 0);

		sui_draw_text(-0.315, -0.025, SUI_T_SIZE, SUI_T_SPACE, "NAME :", 0, 0, 0, d_view[D_CM_ADD_FAVORITES]);
		sui_type_in(input, -0.235, -0.025, 1.0, SUI_T_SIZE, name, MESSAGE_CHUNK_SIZE, NULL, NULL, 0, 0, 0, d_view[D_CM_ADD_FAVORITES]);
		sui_draw_2d_line_gl(-0.245, -0.035, 0.315, -0.035, 0, 0, 0, 0);

		sui_draw_text(-0.315, -0.075, SUI_T_SIZE, SUI_T_SPACE, "PASSSWOPRD :", 0, 0, 0, d_view[D_CM_ADD_FAVORITES]);
		sui_type_in(input, -0.140, -0.075, 1.0, SUI_T_SIZE, pass, MESSAGE_CHUNK_SIZE, NULL, NULL, 0, 0, 0, d_view[D_CM_ADD_FAVORITES]);
		sui_draw_2d_line_gl(-0.150, -0.085, 0.315, -0.085, 0, 0, 0, 0);

		if(sw_text_button(input, -x_size / 2, -y_size + 0.0, 0, SUI_T_SIZE, SUI_T_SPACE, "OK", 0.0, 0.0, 0.0, d_view[D_CM_ADD_FAVORITES]))
		{
			if(d_view[D_CM_ADD_FAVORITES] > 0.5)
			{
				d_add_address(D_CM_FAVORITES, address, description, name, pass);
				d_current_mode = D_CM_FAVORITES;
				address[0] = 0;
				description[0] = 0;
				name[0] = 0;
				pass[0] = 0;
				d_save_address_settings();
			}
		}
		if(sw_text_button(input, x_size / 2 - 0.095, -y_size + 0.0, 0, SUI_T_SIZE, SUI_T_SPACE, "CANCEL", 0.0, 0.0, 0.0, d_view[D_CM_ADD_FAVORITES]))
		{
			if(d_view[D_CM_ADD_FAVORITES] > 0.5)
			{
				d_current_mode = D_CM_FAVORITES;
				address[0] = 0;
				description[0] = 0;
				name[0] = 0;
				pass[0] = 0;
			}
		}
	}
}
