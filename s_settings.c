#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "seduce.h"

typedef struct{
	char	name[32];
	char	*text;
	double	real;
	uint	integer;
}Setting;

struct{
	Setting			*array;
	uint			count;
	uint			version;
	
}SUIGlobalSettings;

static void init_setting(void)
{
	static boolean init = FALSE;
	if(init)
		return;
	init = TRUE;
	SUIGlobalSettings.array = malloc((sizeof *SUIGlobalSettings.array) * 32);
	SUIGlobalSettings.count = 0;
	SUIGlobalSettings.version = 345;
}
static Setting * get_setting(const char *name)
{
	uint i;
	init_setting();
	for(i = 0; i < SUIGlobalSettings.count; i++)
		if(strcmp(SUIGlobalSettings.array[i].name, name) == 0)
			return &SUIGlobalSettings.array[i];
	return NULL;
}

static Setting * add_setting(const char *name)
{
	uint i;
	init_setting();
	for(i = 0; i < 32 && name[i] != 0; i++)
		SUIGlobalSettings.array[SUIGlobalSettings.count].name[i] = name[i];
	SUIGlobalSettings.array[SUIGlobalSettings.count].text = NULL;
	SUIGlobalSettings.array[SUIGlobalSettings.count].real = 0;
	SUIGlobalSettings.array[SUIGlobalSettings.count].integer = 0;
	return &SUIGlobalSettings.array[SUIGlobalSettings.count++];
}

double	sui_get_setting_double(char *setting, double default_value)
{
	Setting	 *s;
	init_setting();
	s = get_setting(setting);
	if(s == NULL)
	{
		s = add_setting(setting);
		s->real = default_value;
	}
	return s->real;
}

void sui_set_setting_double(char *setting, double value)
{
	Setting	 *s;
	init_setting();
	s = get_setting(setting);
	if(s == NULL)
		s = add_setting(setting);
	s->real = value;
	s->integer = (uint)value;
	SUIGlobalSettings.version++;
}

uint sui_get_setting_int(char *setting, uint default_value)
{
	Setting	 *s;
	init_setting();
	s = get_setting(setting);
	if(s == NULL)
	{
		s = add_setting(setting);
		s->integer = default_value;
	}
	return s->integer;
}

void sui_set_setting_int(char *setting, uint value)
{
	Setting	 *s;
	init_setting();
	s = get_setting(setting);
	if(s == NULL)
		s = add_setting(setting);
	s->integer = value;
	SUIGlobalSettings.version++;
}

char * sui_get_setting_text(const char *setting, const char *default_text)
{
	Setting	 *s;
	init_setting();
	s = get_setting(setting);
	if(s == NULL)
	{
		size_t	len;

		s = add_setting(setting);
		len = strlen(default_text);
		s->text = malloc(len + 1);
		strcpy(s->text, default_text);
	}
	return s->text;
}

void sui_set_setting_text(const char *setting, const char *text)
{
	Setting	 *s;
	uint i;
	init_setting();
	s = get_setting(setting);
	if(s == NULL)
		s = add_setting(setting);
	else
		free(s->text);
	SUIGlobalSettings.version++;
	for(i = 0; text[i] != 0; i++)
		;
	s->text = malloc((sizeof *s->text) * ++i);
	for(i = 0; text[i] != 0; i++)
		s->text[i] = text[i]; 
/*	s->text = text;*/
}

boolean sui_test_setting_version(uint *version)
{
	init_setting();
	if(SUIGlobalSettings.version == *version)
		return FALSE;
	*version = SUIGlobalSettings.version;
	return TRUE;
}

void sui_save_settings(const char *file_name)
{
	uint i;
	FILE *settings;
	init_setting();
	settings = fopen(file_name, "w");
	for(i = 0; i < SUIGlobalSettings.count; i++)
	{
		if(SUIGlobalSettings.array[i].text != NULL)
			fprintf(settings, "%s %s\n", SUIGlobalSettings.array[i].name, SUIGlobalSettings.array[i].text);
		else if(SUIGlobalSettings.array[i].integer != 0)
			fprintf(settings, "%s %u\n", SUIGlobalSettings.array[i].name, SUIGlobalSettings.array[i].integer);
		else
			fprintf(settings, "%s %f\n", SUIGlobalSettings.array[i].name, SUIGlobalSettings.array[i].real);

	}
	fclose(settings);
}

void sui_load_settings(const char *file_name)
{
	char	line[512], key[64], value[32];
	float	r;
	FILE	*settings;
	init_setting();
	if((settings = fopen(file_name, "r")) != NULL)
	{
/*		uint i;
		for(i = 0; i < 512; i++)
			line[i] = 0;
		while((fgets(line, sizeof line, settings)) != NULL)
		{

			if(sscanf(line, "%s%s", key, value) == 2)
			{
				printf("key %s\n", key);
				if(sscanf(value, "%f", &r) == 1)
				{
					sui_set_setting_double(key, r);
					if(sscanf(value, "%u", &i) == 1)
						sui_set_setting_int(key, i);
				}
				else
				{
					sui_set_setting_text(key, value);
				}
			}
		}*/
		fclose(settings);
	}
}

