
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "st_types.h"

uint st_find_next_word(char *text)
{
	uint i;
	for(i = 1; text[i] > 32; i++);
	return i;
}

boolean st_find_word_compare(char *text_a, char *text_b)
{
	uint i;
	for(i = 0; text_a[i] == text_b[i] && text_a[i] > 32 && text_b[i] > 32; i++);
	return (text_a[i] < 32 && text_b[i] < 32);
}

uint st_text_copy(uint length, char *dest, char *source)
{
	uint i;
	for(i = 0; i < length && source[i] != 0; i++)
		dest[i] = source[i];
	if(i == length)
		return 0;
	dest[i] = source[i];
	return i;
}


char *st_text_copy_allocate(char *source)
{
	uint i;
	char *text;
	for(i = 0; source[i] != 0; i++);
	text = malloc((sizeof *text) * (i + 1));
	for(i = 0; source[i] != 0; i++)
		text[i] = source[i];
	text[i] = 0;
	return text;
}

uint st_word_copy(uint length, char *dest, char *source)
{
	uint i = 0, j;
	for(j = 0; j < length && source[j] < 32; j++);
	while( i < length && source[j] > 32)
		dest[i++] = source[j++];
	if(i == length)
		return 0;
	dest[i] = 0;
	return j;
}


uint st_text_copy_until(uint length, char *dest, char *source, char *until)
{
	uint i = 0, j, k, l;
	for(j = 0; j < length && source[j] < 32; j++);
	while( i < length && source[j] != 0)
	{
		for(k = 0; source[j + k] == until[k] && until[k] != 0 ; k++);
		if(until[k] == 0)
		{
			for(i--; dest[i] < 32; i--);
			dest[i] = 0;
			return j + k;
		}
		else
			dest[i++] = source[j++];
	}
	return 0;
}
