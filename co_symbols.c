
#include <math.h>

#include "st_matrix_operations.h"
#include "enough.h"
#include "seduce.h"
#include "persuade.h"
#include "deceive.h"
#include <stdio.h>
#include "co_vn_handle.h"

typedef enum{
	CO_SM_COLOR,
	CO_SM_PRINT,
	CO_SM_ICON
}COSymbMode;


typedef struct{
	boolean		active;
	float		color[3];
	boolean		flash;
	char		tag[16];
	char		group[16];
	char		string[64];
	float		size;
	boolean		scaling;
	uint		icon;
	COSymbMode mode;
}COSymbSetting;

extern float co_line_color[3];
extern void se_pack_binary_uint32(FILE *f, const uint32 data);

static COSymbSetting *co_symbols = NULL;
static uint  co_symbol_count;
static uint  co_symbol_allocated = 0;
static uint co_current_edit_cymbol = 0;


static SDrawing *co_symbol_icons = NULL;
static uint  co_symbol_icon_count = 0;
static uint  co_symbol_icon_allocated = 0;

void co_pack_binary_uint32(FILE *f, const uint32 data)
{
	fputc((data >> 24) & 0xFF, f);
	fputc((data >> 16) & 0xFF, f);
	fputc((data >> 8)  & 0xFF, f);
	fputc(data & 0xFF, f);
}

void co_pack_binary_real32(FILE *f, float data)
{
	union { uint32 integer; float real; } punt;
	punt.real = data;
	se_pack_binary_uint32(f, punt.integer);
}

void co_pack_binary_string(FILE *f, char *text, uint length)
{
	uint i;
	for(i = 0; i < length && text[i] != 0; i++)
		fputc(text[i], f);
	fputc(0, f);
}

boolean co_save_symb_settings(char *file_name)
{
	uint i;
	FILE *f;
	f = fopen(file_name, "wb");
	if(f == NULL)
		return FALSE;
	co_pack_binary_uint32(f, co_symbol_count);
	for(i = 0; i < co_symbol_count; i++)
	{
		co_pack_binary_uint32(f, co_symbols[i].active);
		co_pack_binary_real32(f, co_symbols[i].color[0]);
		co_pack_binary_real32(f, co_symbols[i].color[1]);
		co_pack_binary_real32(f, co_symbols[i].color[2]);
		co_pack_binary_uint32(f, co_symbols[i].flash);
		co_pack_binary_string(f, co_symbols[i].tag, 16);
		co_pack_binary_string(f, co_symbols[i].group, 16);
		co_pack_binary_string(f, co_symbols[i].string, 64);
		co_pack_binary_real32(f, co_symbols[i].size);
		co_pack_binary_uint32(f, co_symbols[i].scaling);
		co_pack_binary_uint32(f, co_symbols[i].icon);
		co_pack_binary_uint32(f, co_symbols[i].mode);
	}
	se_save_drawing_binary(f, co_symbol_icons, co_symbol_icon_count);

	return TRUE;
}

uint32 co_unpack_binary_uint32(FILE *f)
{
	uint32 data;
	data = ((uint32) fgetc(f)) << 24;
	data |= ((uint32) fgetc(f)) << 16;
	data |= ((uint32) fgetc(f)) << 8;
	data |= ((uint32) fgetc(f));
	return data;
}

float co_unpack_binary_real32(FILE *f)
{
	union { uint integer; float real; } data;
	data.integer = ((uint32) fgetc(f)) << 24;
	data.integer |= ((uint32) fgetc(f)) << 16;
	data.integer |= ((uint32) fgetc(f)) << 8;
	data.integer |= ((uint32) fgetc(f));
	return data.real;
}

void co_unpack_binary_string(FILE *f, char *text)
{
	while((*text++ = fgetc(f)) != 0);
}

static boolean co_load_symb_settings(char *file_name)
{
	uint i;
	FILE *f;

	f = fopen(file_name, "rb");
	if(f == NULL)
		return FALSE;
	co_symbol_count = co_unpack_binary_uint32(f);
	if(feof(f))	/* After first read, check if file is already over. Catches 0-length file, at least. */
		return FALSE;
	if(co_symbols != NULL)
		free(co_symbols);
	co_symbol_allocated = co_symbol_count + 16;
	co_symbols = malloc((sizeof *co_symbols) * co_symbol_allocated);
	for(i = 0; i < co_symbol_count; i++)
	{
		co_symbols[i].active = co_unpack_binary_uint32(f);
		co_symbols[i].color[0] = co_unpack_binary_real32(f);
		co_symbols[i].color[1] = co_unpack_binary_real32(f);
		co_symbols[i].color[2] = co_unpack_binary_real32(f);
		co_symbols[i].flash = co_unpack_binary_uint32(f);
		co_unpack_binary_string(f, co_symbols[i].tag);
		co_unpack_binary_string(f, co_symbols[i].group);
		co_unpack_binary_string(f, co_symbols[i].string);
		co_symbols[i].size = co_unpack_binary_real32(f);
		co_symbols[i].scaling = co_unpack_binary_uint32(f);
		co_symbols[i].icon = co_unpack_binary_uint32(f);
		co_symbols[i].mode = co_unpack_binary_uint32(f);
	}
	co_symbol_icons = se_load_drawing_binary(f, &co_symbol_icon_count);
	co_symbol_icon_allocated = co_symbol_icon_count;
	return TRUE;
}

void icon_editor_func(BInputState *input, void *user, double x_pos, double y_pos, double width, double length)
{
/*	float color_drawing[3] = {1.0, 1.0, 1.0},  color_interface[3] = {0.2, 0.6, 1.0}; 
	static boolean init = FALSE;
	if(!init)
	{
		init = TRUE;
		co_symbol_icons[0].lines = malloc((sizeof *co_symbol_icon[0].lines) * co_symbol_icon[0].line_allocate);
		co_symbol_icons[0].line_allocate = 256;
		co_symbol_icons[0].line_count = 0;
		co_symbol_icons[0].name[0] = 0;
	}*/
	se_editor(input, &co_symbol_icons[co_symbols[co_current_edit_cymbol].icon], x_pos, y_pos - length, width, length, co_symbols[co_current_edit_cymbol].color, co_line_color, SE_LT_LINE);
}

void icon_draw(SDrawing *d, double x_pos, double y_pos, float scale, float red, float green, float blue)
{
	uint	i;
	for(i = 0; i < d->line_count; i++)
		sui_draw_2d_line_gl(d->lines[i].pos_one[0] * scale,
		d->lines[i].pos_one[1] * scale,
		d->lines[i].pos_two[0] * scale,
		d->lines[i].pos_two[1] * scale, red, green, blue, 1.0);

}

void co_set_values(COSymbSetting *s, boolean active, float color_r, float color_g, float color_b, boolean flash, char *group, char *tag, char *string, COSymbMode mode)
{
	uint i;
	s->active = active;
	s->color[0] = color_r;
	s->color[1] = color_g;
	s->color[2] = color_b;
	s->flash = flash;
	for(i = 0; i < 15 && tag[i] != 0; i++)
		s->tag[i] = tag[i];
	s->tag[i] = 0;
	for(i = 0; i < 15 && group[i] != 0; i++)
		s->group[i] = group[i];
	s->group[i] = 0;

	for(i = 0; i < 15 && string[i] != 0; i++)
		s->string[i] = string[i];
	s->string[i] = 0;
	s->icon = 0;
	s->mode = mode;
	s->size = 0.25;
	s->scaling = FALSE;
}

void co_init_symbols(void)
{
	static boolean init_symbols = FALSE;

	if(init_symbols)
		return;
	init_symbols = TRUE;
	if(co_load_symb_settings("symbols_settings.bin"))
		return;
	co_symbol_allocated = 64;
	co_symbols = malloc(co_symbol_allocated * sizeof *co_symbols);
	co_set_values(&co_symbols[0], TRUE, 1, 0, 0, TRUE, "asset", "priority",		"critical",		CO_SM_COLOR);
	co_set_values(&co_symbols[1], TRUE, 1, 1, 1, FALSE, "asset", "author",			"",				CO_SM_PRINT);
	co_set_values(&co_symbols[2], TRUE, 0, 0, 1, FALSE, "asset", "approved",		"",				CO_SM_PRINT);

	co_set_values(&co_symbols[3], TRUE, 1, 0, 0, FALSE, "asset", "status",			"incomplete",	CO_SM_COLOR);
	co_set_values(&co_symbols[4], TRUE, 1, 1, 0, FALSE, "asset", "status",			"inprogress",	CO_SM_COLOR);
	co_set_values(&co_symbols[5], TRUE, 0, 1, 0, FALSE, "asset", "status",			"complete",		CO_SM_COLOR);

	co_set_values(&co_symbols[6], FALSE, 1, 0, 1, FALSE, "asset", "approved_model",	"",				CO_SM_COLOR);
	co_set_values(&co_symbols[7], FALSE, 1, 0, 0, FALSE, "asset", "status_model",	"incomplete",	CO_SM_COLOR);
	co_set_values(&co_symbols[8], FALSE, 1, 1, 0, FALSE, "asset", "status_model",	"inprogress",	CO_SM_COLOR);
	co_set_values(&co_symbols[9], FALSE, 0, 1, 0, FALSE, "asset", "status_model",	"complete",		CO_SM_COLOR);

	co_set_values(&co_symbols[10], FALSE, 1, 0, 1, FALSE, "asset", "approved_uv",	"critical",		CO_SM_COLOR);
	co_set_values(&co_symbols[11], FALSE, 1, 0, 0, FALSE, "asset", "status_uv",		"incomplete",	CO_SM_COLOR);
	co_set_values(&co_symbols[12], FALSE, 1, 1, 0, FALSE, "asset", "status_uv",		"inprogress",	CO_SM_COLOR);
	co_set_values(&co_symbols[13], FALSE, 0, 1, 0, FALSE, "asset", "status_uv",		"complete",		CO_SM_COLOR);

	co_set_values(&co_symbols[14], FALSE, 1, 0, 1, FALSE, "asset", "approved_mat",	"critical",		CO_SM_COLOR);
	co_set_values(&co_symbols[15], FALSE, 1, 0, 0, FALSE, "asset", "status_mat",	"incomplete",	CO_SM_COLOR);
	co_set_values(&co_symbols[16], FALSE, 1, 1, 0, FALSE, "asset", "status_mat",	"inprogress",	CO_SM_COLOR);
	co_set_values(&co_symbols[17], FALSE, 0, 1, 0, FALSE, "asset", "status_mat",	"complete",		CO_SM_COLOR);

	co_set_values(&co_symbols[18], FALSE, 1, 0, 1, FALSE, "asset", "approved_tex",	"critical",		CO_SM_COLOR);
	co_set_values(&co_symbols[19], FALSE, 1, 0, 0, FALSE, "asset", "status_tex",	"incomplete",	CO_SM_COLOR);
	co_set_values(&co_symbols[20], FALSE, 1, 1, 0, FALSE, "asset", "status_tex",	"inprogress",	CO_SM_COLOR);
	co_set_values(&co_symbols[21], FALSE, 0, 1, 0, FALSE, "asset", "status_tex",	"complete",		CO_SM_COLOR);

	co_set_values(&co_symbols[22], FALSE, 1, 0, 1, FALSE, "asset", "approved_rig",	"critical",		CO_SM_COLOR);
	co_set_values(&co_symbols[23], FALSE, 1, 0, 0, FALSE, "asset", "status_rig",	"incomplete",	CO_SM_COLOR);
	co_set_values(&co_symbols[24], FALSE, 1, 1, 0, FALSE, "asset", "status_rig",	"inprogress",	CO_SM_COLOR);
	co_set_values(&co_symbols[25], FALSE, 0, 1, 0, FALSE, "asset", "status_rig",	"complete",		CO_SM_COLOR);

	co_set_values(&co_symbols[26], FALSE, 1, 0, 1, FALSE, "asset", "approved_anim",	"critical",		CO_SM_COLOR);
	co_set_values(&co_symbols[27], FALSE, 1, 0, 0, FALSE, "asset", "status_anim",	"incomplete",	CO_SM_COLOR);
	co_set_values(&co_symbols[28], FALSE, 1, 1, 0, FALSE, "asset", "status_anim",	"inprogress",	CO_SM_COLOR);
	co_set_values(&co_symbols[29], FALSE, 0, 1, 0, FALSE, "asset", "status_anim",	"complete",		CO_SM_COLOR);

	co_set_values(&co_symbols[30], TRUE, 1, 0, 0, FALSE, "asset", "section",		"critical", CO_SM_COLOR);
	co_set_values(&co_symbols[31], TRUE, 1, 0, 0, FALSE, "asset", "tool",			"",		CO_SM_PRINT);
	co_set_values(&co_symbols[32], TRUE, 1, 0, 0, FALSE, "asset", "library",		"",		CO_SM_PRINT);
	co_set_values(&co_symbols[33], TRUE, 1, 0, 0, FALSE, "asset", "version",		"",		CO_SM_PRINT);
	co_set_values(&co_symbols[34], TRUE, 1, 0, 0, FALSE, "asset", "comment",		"",		CO_SM_PRINT);
	co_symbol_count = 35;

	co_symbol_icon_count = 1;
	co_symbol_icon_allocated = 64;
	co_symbol_icons = malloc((sizeof *co_symbols) * co_symbol_icon_allocated);
	co_symbol_icons[0].name[0] = 0;
	co_symbol_icons[0].line_allocate = 32;
	co_symbol_icons[0].line_count = 0;
	co_symbol_icons[0].lines = malloc((sizeof *co_symbol_icons[0].lines ) * co_symbol_icons[0].line_allocate);
}


void co_draw_symbols(ENode *node, float *color, float pos_x, float pos_y, float pos_z)
{
	VNTag *tag;
	uint16 group_id, tag_id;
	uint i, j;
	float line = 0;
	char text[64];
	co_init_symbols();

	if(co_symbols == NULL)
		return;

	if(co_symbol_count != 0)
	{
		glPushMatrix();
		glTranslatef(pos_x, pos_y, -1);
		for(i = 0; i < co_symbol_count; i++)
		{
			if(co_symbols[i].active)
			{
				if(co_symbols[i].group[0] != 0)
					e_ns_get_tag_by_name_and_group(node, co_symbols[i].group, co_symbols[i].tag, &group_id, &tag_id);
				else if(co_symbols[i].tag[0] != 0)
					e_ns_get_tag_by_name(node, co_symbols[i].tag, &group_id, &tag_id);
				else
					tag_id = (uint16)-1;
					
				if(tag_id != (uint16)-1)
				{
					tag = e_ns_get_tag(node, group_id, tag_id);
					if(co_symbols[i].string[0] != 0)
					{
						if(VN_TAG_STRING == e_ns_get_tag_type(node, group_id, tag_id))
						{
							for(j = 0; co_symbols[i].string[j] != 0 && co_symbols[i].string[j] == tag->vstring[j]; j++);
							if(co_symbols[i].string[j] != tag->vstring[j])
								tag_id = (uint16)-1;
						}else
							tag_id = (uint16)-1;
					}
					if(tag_id != (uint16)-1)
					{
						switch(co_symbols[i].mode)
						{
							case CO_SM_COLOR :
								color[0] = co_symbols[i].color[0];
								color[1] = co_symbols[i].color[1];
								color[2] = co_symbols[i].color[2];
							break;
							case CO_SM_PRINT :
								if(pos_z > 0)
								{
									if(VN_TAG_STRING == e_ns_get_tag_type(node, group_id, tag_id))
									{
										sprintf(text, "%s : %s", co_symbols[i].tag, tag->vstring);
										sui_draw_text(0, line, SUI_T_SIZE, SUI_T_SPACE, text, co_symbols[i].color[0], co_symbols[i].color[1], co_symbols[i].color[2], 1.0);
										line -= 0.025;
									}
									if(VN_TAG_UINT32 == e_ns_get_tag_type(node, group_id, tag_id))
									{
										sprintf(text, "%s : %f", tag->vstring, tag->vreal64);
										sui_draw_text(0, line, SUI_T_SIZE, SUI_T_SPACE, text, co_symbols[i].color[0], co_symbols[i].color[1], co_symbols[i].color[2], 1.0);
										line -= 0.025;
									}
									if(VN_TAG_REAL64 == e_ns_get_tag_type(node, group_id, tag_id))
									{
										sprintf(text, "%s : %f", tag->vstring, tag->vreal64);
										sui_draw_text(0, line, SUI_T_SIZE, SUI_T_SPACE, text, co_symbols[i].color[0], co_symbols[i].color[1], co_symbols[i].color[2], 1.0);
										line -= 0.025;
									}
								}
							break;
							case CO_SM_ICON :
								if(pos_z > 0)
								{
									if(co_symbols[i].scaling)
										icon_draw(&co_symbol_icons[co_symbols[i].icon], pos_x, pos_y, 50.0 * co_symbols[i].size / pos_z, co_symbols[i].color[0], co_symbols[i].color[1], co_symbols[i].color[2]);
									else
										icon_draw(&co_symbol_icons[co_symbols[i].icon], pos_x, pos_y, co_symbols[i].size, co_symbols[i].color[0], co_symbols[i].color[1], co_symbols[i].color[2]);
								}
							break;
						}		
					}
				}
			}
		}
		glPopMatrix();
	}
}


/*
typedef struct{
	boolean  active;
	float	 color[3];
	boolean  flash;
	char	 tag[16];
	char	 group[16];
	char	 *tag;
	char	 *group;
	COSymbMode mode;
}COSymbSetting;

typedef struct{
	char		name[64];
	SEditorLine *lines;
	uint		line_allocate;
	uint		line_count;
}SDrawing;*/


void co_symbols_get_interface(SUIViewElement *elements)
{
	co_init_symbols();
	elements[0].type = S_VET_SPLIT_MULTI;
	elements[0].text = "Tags";
	elements[0].param.split_multi.add = FALSE;
	elements[0].param.split_multi.del = FALSE;
	elements[0].param.split_multi.count = co_symbol_count;
	elements[0].param.split_multi.current = co_current_edit_cymbol;

	elements[1].type = S_VET_TEXT;
	elements[1].text = "Tag";
	elements[1].param.text.text = co_symbols[co_current_edit_cymbol].tag;
	elements[1].param.text.length = 16;

	elements[2].type = S_VET_TEXT;
	elements[2].text = "Group";
	elements[2].param.text.text = co_symbols[co_current_edit_cymbol].group;
	elements[2].param.text.length = 16;

	elements[3].type = S_VET_TEXT;
	elements[3].text = "value";
	elements[3].param.text.text = co_symbols[co_current_edit_cymbol].string;
	elements[3].param.text.length = 16;

	elements[4].type = S_VET_BOOLEAN;
	elements[4].text = "Active";
	elements[4].param.checkbox = co_symbols[co_current_edit_cymbol].active;

	elements[5].type = S_VET_COLOR;
	elements[5].text = "Color";
	elements[5].param.color[0] = co_symbols[co_current_edit_cymbol].color[0];
	elements[5].param.color[1] = co_symbols[co_current_edit_cymbol].color[1];
	elements[5].param.color[2] = co_symbols[co_current_edit_cymbol].color[2];

	elements[6].type = S_VET_SELECT;
	elements[6].text = "Type";
	elements[6].param.select.select = co_symbols[co_current_edit_cymbol].mode;
	elements[6].param.select.text[0] = "color";
	elements[6].param.select.text[1] = "text";
	elements[6].param.select.text[2] = "icon";
	elements[6].param.select.text[3] = NULL;

	elements[7].type = S_VET_SLIDER;
	elements[7].text = "Size";
	elements[7].param.slider = co_symbols[co_current_edit_cymbol].size;

	elements[8].type = S_VET_BOOLEAN;
	elements[8].text = "Scaling";
	elements[8].param.checkbox = co_symbols[co_current_edit_cymbol].scaling;

	elements[9].type = S_VET_SPLIT_MULTI;
	elements[9].text = "Symbol";
	elements[9].param.split_multi.add = FALSE;
	elements[9].param.split_multi.del = FALSE;
	elements[9].param.split_multi.count = co_symbol_icon_count;
	elements[9].param.split_multi.current = co_symbols[co_current_edit_cymbol].icon;

	elements[10].type = S_VET_CUSTOM;
	elements[10].text = "";
	elements[10].param.custom.func = icon_editor_func;
	elements[10].param.custom.user = NULL;
	elements[10].param.custom.length = 0.4;

	if(co_symbols[co_symbol_count].mode == CO_SM_ICON)
	{

	}
}

void co_symbols_set_interface(SUIViewElement *elements)
{
	uint i, j;
	if(co_current_edit_cymbol != elements[0].param.split_multi.current)
	{
		co_current_edit_cymbol = elements[0].param.split_multi.current;
		return;
	}
	if(elements[0].param.split_multi.add)
	{
		if(co_symbol_count == co_symbol_allocated)
		{	
			co_symbol_allocated += 16;
			co_symbols = realloc(co_symbols, (sizeof *co_symbols) * co_symbol_allocated);
		}
		co_symbols[co_symbol_count] = co_symbols[co_current_edit_cymbol];
		co_current_edit_cymbol = co_symbol_count;
		co_symbol_count++;
	}
	if(elements[0].param.split_multi.del)
	{
		for(i = co_current_edit_cymbol; i < co_symbol_count - 1; i++)
			co_symbols[i] = co_symbols[i + 1];
		co_symbol_count--;
	}

	co_symbols[co_current_edit_cymbol].active = elements[4].param.checkbox;

	co_symbols[co_current_edit_cymbol].color[0] = elements[5].param.color[0];
	co_symbols[co_current_edit_cymbol].color[1] = elements[5].param.color[1];
	co_symbols[co_current_edit_cymbol].color[2] = elements[5].param.color[2];
	co_symbols[co_current_edit_cymbol].mode = elements[6].param.select.select;
	co_symbols[co_current_edit_cymbol].size = elements[7].param.slider;
	co_symbols[co_current_edit_cymbol].scaling = elements[8].param.checkbox;
	co_symbols[co_current_edit_cymbol].icon = elements[9].param.split_multi.current;

	if(elements[9].param.split_multi.add)
	{
		if(co_symbol_icon_count == co_symbol_icon_allocated)
		{	
			co_symbol_icon_allocated += 16;
			co_symbol_icons = realloc(co_symbol_icons, (sizeof *co_symbol_icons) * co_symbol_icon_allocated);
		}
		co_symbol_icons[co_symbol_icon_count] = co_symbol_icons[co_symbols[co_current_edit_cymbol].icon];
		co_symbol_icons[co_symbol_icon_count].lines = malloc((sizeof *co_symbol_icons[co_symbol_icon_count].lines) * co_symbol_icons[co_symbol_icon_count].line_allocate);
		for(i = 0; i < co_symbol_icons[co_symbol_icon_count].line_count; i++)
			co_symbol_icons[co_symbol_icon_count].lines[i]  = co_symbol_icons[co_symbols[co_current_edit_cymbol].icon].lines[i];
		co_symbols[co_current_edit_cymbol].icon = co_symbol_icon_count;
		co_symbol_icon_count++;
	}
	if(elements[9].param.split_multi.del && co_symbol_icon_count > 1)
	{
		free(co_symbol_icons[co_symbols[co_current_edit_cymbol].icon].lines);
		for(i = co_symbols[co_current_edit_cymbol].icon; i < co_symbol_icon_count - 1; i++)
			co_symbol_icons[i] = co_symbol_icons[i + 1];
		j = co_symbols[co_current_edit_cymbol].icon;
		for(i = 0; i < co_symbol_count; i++)
			if(co_symbols[i].icon == j)
				co_symbols[i].icon = 0;
		co_symbol_icon_count--;
	}
}

