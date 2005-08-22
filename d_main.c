#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "enough.h"
#include "seduce.h"

typedef struct{
	char address[48];
	char name[48];
	char pass[48];
}DLoginLink;

DLoginLink *d_login_list = NULL;
uint d_login_list_length = 0;

void deceve_add_login(const char *address, const char *name, const char *pass)
{
	uint i, j;
	for(i = 0; i < d_login_list_length; i++)
	{
		for(j = 0; d_login_list[i].address[j] == address[j] && address[j] != 0; j++);
		if(d_login_list[i].address[j] == address[j])
			return;
	}
	if(d_login_list_length % 16 == 0)
		d_login_list = malloc((sizeof *d_login_list) * (d_login_list_length + 16));
	for(i = 0; i < 47 && address[i] != 0; i++)
		d_login_list[d_login_list_length].address[i] = address[i];
	d_login_list[d_login_list_length].address[i] = 0;

	for(i = 0; i < 47 && name[i] != 0; i++)
		d_login_list[d_login_list_length].name[i] = name[i];
	d_login_list[d_login_list_length].name[i] = 0;

	for(i = 0; i < 47 && pass[i] != 0; i++)
		d_login_list[d_login_list_length].pass[i] = pass[i];
	d_login_list[d_login_list_length].pass[i] = 0;
	d_login_list_length++;
}

static char deceive_select_node[64] = { 0 };

void deceive_set_arg(int argc, char **argv)
{
	char address[64] = {0}, name[64] = {0}, pass[64] = {0}, node[64] = {0}, *input;
	uint i, j;
	if(argc < 2)
		return;
	input = argv[1];

	for(i = 0; input[i] != '@' && input[i] != 0; i++)
	if(input[i] == '@')
	{
		for(i = 0; input[i] != '@' && input[i] != ':'; i++)
			name[i] = input[i];
		name[i] = 0;
		if(input[i] == ':')
		{
			j = 0;
			for(i++; input[i] != '@'; i++)
				pass[j++] = input[i];
			pass[j] = 0;
		}
	}
	i++;
	for(j = 0; input[i] != '/' && input[i] != 0; i++)
		address[j++] = input[i];
	address[j] = 0;
	j = 0;
	if(input[i] == '/')
		for(i++; input[i] != 0; i++)
			deceive_select_node[j++] = input[i];
	deceive_select_node[j] = 0;
	deceve_add_login(address, name, pass);
	e_vc_connect(address, name, pass, NULL);
}


void deceive_save_bookmarks(const char *file_name)
{
	FILE *bookmarks;
	uint i;

	if(file_name == NULL)
		file_name = "bookmarks.dbm";
	bookmarks = fopen(file_name, "w");
	if(bookmarks == NULL)
		return;
	for(i = 0; i < d_login_list_length; i++)
		fprintf(bookmarks, "%s %s %s\n", 
				d_login_list[i].address,
				d_login_list[i].name,
				d_login_list[i].pass);
	fclose(bookmarks);
}

void deceive_load_bookmarks(const char *file_name)
{
	char line[256], address[256], name[256], pass[256];
	FILE *bookmarks;

	if(file_name == NULL)
		file_name = "bookmarks.dbm";
	if((bookmarks = fopen(file_name, "r")) != NULL)
	{
		while((fgets(line, sizeof line, bookmarks)) != NULL)
			if(sscanf(line, "%s %s %s", address, name, pass) == 3)
				deceve_add_login(address, name, pass);
		fclose(bookmarks);
	}
}

ENode * deceive_get_edit_node(void)
{
	ENode *node;
	char *name;
	uint i, j;
	if(deceive_select_node[0] == 0)
		return NULL;
	for(i = 0; i < V_NT_NUM_TYPES; i++)
	{
		for(node = e_ns_get_node_next(0, 0, i); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, i))
		{
			name = e_ns_get_node_name(node);
			for(j = 0; name[j] == deceive_select_node[j] && deceive_select_node[j] != 0; j++);
			if(name[j] == deceive_select_node[j])
			{
				deceive_select_node[0] = 0;
				return node;
			}
		}
	}
	return NULL;
}

void deceive_draw_symb_close(float pos_x, float pos_y)
{
	static float array[] = {-0.002500, -0.002500, -0.010000, -0.010000, -0.000000, -0.017500, 0.005408, -0.016643, 0.005408, -0.016643, 0.010286, -0.014158, 0.010286, -0.014158, 0.014158, -0.010286, 0.014158, -0.010286, 0.016643, -0.005408, 0.016643, -0.005408, 0.017500, 0.000000, -0.000000, -0.017500, -0.005408, -0.016643, -0.005408, -0.016643, -0.010286, -0.014158, -0.010286, -0.014158, -0.014158, -0.010286, -0.014158, -0.010286, -0.016643, -0.005408, -0.016643, -0.005408, -0.017500, 0.000000, -0.000000, 0.017500, -0.005408, 0.016643, -0.005408, 0.016643, -0.010286, 0.014158, -0.010286, 0.014158, -0.014158, 0.010286, -0.014158, 0.010286, -0.016643, 0.005408, -0.016643, 0.005408, -0.017500, 0.000000, 0.002500, 0.002500, 0.010000, 0.010000, -0.000000, 0.017500, 0.005408, 0.016643, 0.005408, 0.016643, 0.010286, 0.014158, 0.010286, 0.014158, 0.014158, 0.010286, 0.014158, 0.010286, 0.016643, 0.005408, 0.016643, 0.005408, 0.017500, 0.000000, 0.002500, -0.002500, 0.010000, -0.010000, -0.002500, 0.002500, -0.010000, 0.010000};
	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	glScalef(0.5, 0.5, 0.5);
	sui_draw_gl(GL_LINES, array, 48, 2, 0, 0, 0);
	glPopMatrix();
}

void hidden_type_in(BInputState *input, float pos_x, float pos_y, float length, float size, char *text, uint buffer_size, float red, float green, float blue)
{
	static uint cursor;
	char zeros[256];
	int i;
	float pos;
	
	if(input->mode == BAM_DRAW)
	{
		for(i = 0; i < 255 && text[i] != 0; i++)
			zeros[i] = '0';
		zeros[i] = 0;
		sui_draw_text(pos_x, pos_y, size, SUI_T_SPACE, zeros, red, green, blue);
	}
	else
	{
		if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
		{
			if(sui_box_click_test(pos_x, pos_y - size, length, size * 3.0))
			{
				text[0] = 0;
				betray_start_type_in(text, buffer_size, NULL, &cursor, NULL);
			}
		}
	}
}

void (*deceive_draw_func)(void *user_pointer) = NULL;
void *deceive_user_pointer = NULL;

void deceive_set_intro_draw_func(void (*draw_func)(void *user_pointer), void *user_pointer)
{
	deceive_draw_func = draw_func;
	deceive_user_pointer = user_pointer;
}

void deceive_intro_handler(BInputState *input, void *application_handler_func)
{
	DLoginLink *link = NULL;
	static boolean active = FALSE, init = FALSE;
	static char connect_type_in[48];
	uint i, j;
	if(init == FALSE)
	{
		deceive_load_bookmarks(NULL);
		init = TRUE;
	}
	connect_type_in[0] = 0;

	if(e_vc_check_connected() && e_vc_check_accepted_slot(0))
	{
		betray_set_action_func(application_handler_func, NULL);
		return;
	}

	if(input->mode == BAM_MAIN)
	{
		verse_callback_update(3000);
		return;
	}

	if(input->mode == BAM_DRAW)
	{
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glPopMatrix();
		glPopMatrix();
		glPushMatrix();
		glClearColor(1, 1, 1, 1);
		glTranslatef(0, 0, -1);
		glDisable(GL_DEPTH_TEST);
		if(deceive_draw_func != NULL)
			deceive_draw_func(deceive_user_pointer);
	}

	if((input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE) || (input->mouse_button[1] == FALSE && input->last_mouse_button[1] == TRUE) || (input->mouse_button[2] == FALSE && input->last_mouse_button[2] == TRUE))
		active = TRUE;
	
	if(active)
	{
		if(e_vc_check_connected_slot(0))
		{
			if(input->mode == BAM_DRAW)
				sui_draw_text(sui_compute_text_length(0.02, 2, "CONNECTING...") * -0.5, -0.3, 0.02, 2, "CONNECTING...", 0, 0, 0);    
			if(sw_text_button(input, sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, "Cancel") * -0.5, -0.35, 0, SUI_T_SIZE, SUI_T_SPACE, "Cancel", 0, 0, 0))
			{
				e_vc_disconnect(0);
			}
		}
		else
		{
			static boolean text_init = FALSE;
			static boolean list = FALSE;
			static char address[64] = {0}, name[64] = {0}, pass[64] = {0}, hidden[64] = {0};
			static float scroll = 0;
			float color = 0;
			if(list)
				color = 0.9;
			else
				scroll = 0;
			if(!text_init)
			{
				char *t;
				uint i;
				t = sui_get_setting_text("address", "localhost");
				for(i = 0; i < 64 && t[i] != 0 ; i++)
					address[i] = t[i];
				address[i] = 0;
				t = sui_get_setting_text("name", "name");
				for(i = 0; i < 64 && t[i] != 0 ; i++)
					name[i] = t[i];
				name[i] = 0;
				t = sui_get_setting_text("pass", "pass");
				for(i = 0; i < 64 && t[i] != 0 ; i++)
					pass[i] = t[i];
				pass[i] = 0;
				text_init = TRUE;
			}
			if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
				if(sui_box_click_test(-0.2, -0.2 - SUI_T_SIZE, 0.4, SUI_T_SIZE * 3.0))
					list = TRUE;

			if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == FALSE)
				list = FALSE;

			if(input->mode == BAM_DRAW)
				sui_draw_text(sui_compute_text_length(0.02, 2, "CONNECT") * -0.5, 0.35, 0.02, 2, "CONNECT", color, color, color);

			if(sw_text_button(input, -0.2, 0.25 - 2 * 0.02, 0, 0.02, 0.3, "Localhost", color, color, color))
				e_vc_connect("localhost", "anonymous", "nopass", NULL);
			if(sw_text_button(input, 0.2, 0.25 - 2 * 0.02, 1, 0.02, 0.3, "Exit", color, color, color))
				exit(0);
	
			if(address[0] != 0)
			{
				for(i = 0; i < d_login_list_length; i++)
				{
					for(j = 0; d_login_list[i].address[j] == address[j] && d_login_list[i].address[j] != 0; j++);
					if(j > 0 && address[j] == 0)
						link = &d_login_list[i];
				}
			}

			if(link != NULL)
				sui_draw_text(-0.2, -0.2, SUI_T_SIZE, SUI_T_SPACE, link->address, 0.8, 0.8, 0.8);
			sui_type_in(input, -0.2, -0.2, 0.4, SUI_T_SIZE, address, 64, NULL, NULL, color, color, color);
			sui_draw_2d_line_gl(-0.2, -0.2, 0.2, -0.2, color, color, color);
			sui_type_in(input, -0.2, -0.25, 0.4, SUI_T_SIZE, name, 64, NULL, NULL, color, color, color);
			sui_draw_2d_line_gl(-0.2, -0.25, 0.2, -0.25, color, color, color);
			hidden_type_in(input, -0.2, -0.3, 0.4, SUI_T_SIZE, pass, 64, color, color, color);
			sui_draw_2d_line_gl(-0.2, -0.3, 0.2, -0.3, color, color, color);


			if(sw_text_button(input, -0.2, -0.325 - 2 * 0.02, 0, 0.02, 0.3, "OK", color, color, color))
			{
				e_vc_connect(address, name, pass, NULL);
				deceive_save_bookmarks(NULL);
			}
			if(sw_text_button(input, 0.2, -0.325 - 2 * 0.02, 1, 0.02, 0.3, "Clear", color, color, color))
			{
				address[0] = 0;
				name[0] = 0;
				pass[0] = 0;
			}
			if(list == TRUE)
			{
				if(input->pointer_y > 0.45)
					scroll -= 0.4 * betray_get_delta_time();
				if(input->pointer_y < -0.45)
					scroll += 0.4 * betray_get_delta_time();
				if(scroll < 0)
					scroll = 0;
				if(scroll > d_login_list_length * 0.05 - 0.05)
					scroll = d_login_list_length * 0.05 - 0.05;
				
				if(input->mode == BAM_DRAW)
				{
					for(i = 0; i < d_login_list_length; i++)
					{
						sui_draw_text(-0.2, scroll - 0.2 + (float)i * -0.05, SUI_T_SIZE, SUI_T_SPACE, d_login_list[i].address, 0, 0, 0);		
						deceive_draw_symb_close(0.2, scroll - 0.19 + (float)i * -0.05);
					}
				}else if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE)
				{
					i = -(input->pointer_y + 0.175 - scroll) / 0.05;
					if(i < d_login_list_length)
					{
						if(input->pointer_x > 0.19 && input->pointer_x < 0.21)
							d_login_list[i] = d_login_list[--d_login_list_length];
						else
						{
							for(j = 0; j < 47; j++)
								address[j] = d_login_list[i].address[j];
							for(j = 0; j < 47; j++)
								name[j] = d_login_list[i].name[j];
							for(j = 0; j < 47; j++)
								pass[j] = d_login_list[i].pass[j];
						}
					}
				}
			}
		}
	}

	if(input->mode == BAM_DRAW)
	{
		float border[16] = {-1, 10, 1, 10, 1, 0.45, -1, 0.45, -1, -10, 1, -10, 1, -0.45, -1, -0.45};
		sui_draw_gl(GL_QUADS, border, 8, 2, 0, 0, 0);
		glEnable(GL_DEPTH_TEST);
		glPopMatrix();
		glPushMatrix();
		glPushMatrix();
	}
}
