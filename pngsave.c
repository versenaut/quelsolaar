/*
 * PNG image saver. Saves bitmap nodes out as PNG files, using Enough and the libpng library.
 *
 * A lot of inspiration came from <http://zarb.org/~gc/html/libpng.html>, thanks to G. Cottenceau
 * for that.
 * 
 * Written by Emil Brink.
 * 
 * Copyright (c) 2005, PDC, KTH. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 * 
 *  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the distribution.
 *  * Neither the name of the <ORGANIZATION> nor the names of its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT
 * NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <png.h>

#include "verse.h"
#include "enough.h"

/* Save out given bitmap node as a PNG image. */
static void save_bitmap(ENode *node)
{
	char		fname[128];
	uint		size[3], tiles[2], i, chan, x, y;
	uint8		row[4 * 1024], *data[4] = { NULL, NULL, NULL, NULL };
	EBitLayer	*layer, *rgba[4] =  { NULL, NULL, NULL, NULL };
	png_structp	png_ptr;
	png_infop	info_ptr;
	FILE		*f;

	e_nsb_get_size((ENode *) node, &size[0], &size[1], &size[2]);

	if(size[2] != 1)
	{
		fprintf(stderr, "** Can't save 3D image %s as PNG\n", e_ns_get_node_name(node));
		return;
	}

	for(layer = e_nsb_get_layer_next(node, 0); layer != NULL; layer = e_nsb_get_layer_next(node, e_nsb_get_layer_id(layer) + 1))
	{
		const char	*ln = e_nsb_get_layer_name(layer);

		if(strcmp(ln, "col_r") == 0)
			rgba[0] = layer;
		else if(strcmp(ln, "col_g") == 0)
			rgba[1] = layer;
		else if(strcmp(ln, "col_b") == 0)
			rgba[2] = layer;
		else if(strcmp(ln, "alpha") == 0)
			rgba[3] = layer;
	}
	chan = rgba[3] == NULL ? 3 : 4;

	for(i = 0; i < chan; i++)
	{
		if(e_nsb_get_layer_type(rgba[i]) != VN_B_LAYER_UINT8)
		{
			fprintf(stderr, "** Can't save non 8-bit layer \"%s\" in \"%s\"\n", e_nsb_get_layer_name(rgba[i]), e_ns_get_node_name(node));
			return;
		}
		data[i] = e_nsb_get_layer_data(node, rgba[i]);
	}

	sprintf(fname, "%s.png", e_ns_get_node_name(node));
	if((f = fopen(fname, "wb")) == NULL)
	{
		fprintf(stderr, "** Couldn't open \"%s\" for writing\n", fname);
		return;
	}

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(png_ptr == NULL)
	{
		fprintf(stderr, "** Couldn't create PNG write struct for \"%s\"\n", fname);
		fclose(f);
		return;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if(info_ptr == NULL)
	{
		fprintf(stderr, "** Couldn't create PNG info struct for \"%s\"\n", fname);
		fclose(f);
		return;
	}
	if(setjmp(png_jmpbuf(png_ptr)) != 0)
	{
		fprintf(stderr, "** Couldn't initialize PNG setjmp() for \"%s\"\n", fname);
		fclose(f);
		return;
	}

	png_init_io(png_ptr, f);
	
	png_set_IHDR(png_ptr, info_ptr, size[0], size[1], 8,
		     chan == 3 ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_RGB_ALPHA,
		     PNG_INTERLACE_NONE,
		     PNG_COMPRESSION_TYPE_BASE,
		     PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr, info_ptr);

	for(y = 0; y < size[1]; y++)
	{
		for(x = 0; x < size[0]; x++)
		{
			for(i = 0; i < chan; i++)
				row[chan * x + i] = data[i] ? *(data[i] + y * size[0] + x) : 0;
		}
		png_write_row(png_ptr, row);
	}
	png_write_end(png_ptr, NULL);
	
	fclose(f);
}

static void save_data(void)
{
	ENode *node;
	uint i;

	for(i = 0; i < V_NT_NUM_TYPES; i++)
	{
		for(node = e_ns_get_node_next(0, 0, i); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, i))
		{
			if(e_ns_get_node_type(node) == V_NT_BITMAP)
				save_bitmap(node);
		}
	}
}

/* Just iterate over all bitmaps, and ask Enough for their layer data. Causes data to be downloaded. */
static void download_data(void)
{
	void *layer, *buffer;
	ENode *node;

	for(node = e_ns_get_node_next(0, 0, V_NT_BITMAP); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, V_NT_BITMAP))
	{
		for(layer = e_nsb_get_layer_next(node, 0); layer != NULL; layer = e_nsb_get_layer_next(node, e_nsb_get_layer_id(layer) + 1))
			e_nsb_get_layer_data(node, layer);
	}
}

static const char * find_param(int argc, char **argv, const char *option, const char *default_text)
{
	int i;

	for(i = 1; i < argc - 1; i++)
		if(strcmp(argv[i], option) == 0)
			return argv[i + 1];
	return default_text;
}

int main(int argc, char **argv)
{
	uint32		i, seconds, s, interval, repeat;
	const char	*name, *pass, *address, *tmp;

	enough_init();
	name    = find_param(argc, argv, "-n", "pngsave");
	pass    = find_param(argc, argv, "-p", "pass");
	address = find_param(argc, argv, "-a", "localhost");
	repeat  = find_param(argc, argv, "-r", NULL) != NULL;
	tmp     = find_param(argc, argv, "-i", "10");
	if(tmp != NULL)
	{
		interval = strtoul(tmp, NULL, 10);
		printf("Waiting %u seconds until save\n", interval);
	}

	for(i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "help") == 0)
		{
			printf("usage: verse_saver <pamams>\n");
			printf("params:\n");
			printf("-h This text.\n");
			printf("-n <name>\n");
			printf("-p <pass>\n");
			printf("-a <address>\n");
			printf("-i <save interval in seconds>\n");
			printf("-1 Save only once, then exit\n");
			return EXIT_SUCCESS;
		}
	}
	if(e_vc_connect(address, name, pass, NULL) == -1)
	{
		printf("Error: Invalid server address '%s'\n", address);
		return TRUE;
	}
	printf("Connecting to %s\n", address);
	while(interval != 0)
	{
		verse_session_get_time(&seconds, NULL);
		s = seconds;
		while(seconds < s + interval)
		{
			verse_callback_update(10000);
			download_data();
			verse_session_get_time(&seconds, NULL);
		}
		printf("Done waiting, beginning save\n");
		save_data();
		printf("Save complete\n");
		if(!repeat)
			break;
	}
	return EXIT_SUCCESS;
}
