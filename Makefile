#
# Rough and ready Makefile for compiling Connector, Loq Auirou, and UVedit tools.
#
# NOTE: This Makefile has not been tested with the 2004-08-24 release. /Emil
#

VERSE=../verse

CFLAGS=-I$(VERSE) -g
LDFLAGS=-L$(VERSE) -L/usr/X11R6/lib
LDLIBS=-lverse -lGL -lm -lglut -lGLU -lSDL

APPS=connector uvedit loqairou

ALL:		$(APPS)

# -----------------------------------------------

connector:	co_main.o co_game.o co_intro.o co_vn_bitmap.o co_vn_curve.o co_vn_geometry.o co_vn_graphics.o \
		co_vn_handle.o co_vn_head.o co_vn_mat_render.o co_vn_material.o co_vn_object.o co_vn_search.o \
		co_vn_text.o co_widgets.o st_types.o \
		libseduce.a libbetray.a libenough.a
		gcc -o $@ $^ $(LDFLAGS) $(LDLIBS)

uvedit:		uv_draw.o uv_edge_collapse.o uv_geometry.o uv_main.o uv_menu.o uv_overlay.o uv_popup.o uv_tool_corner.o\
		uv_tool_edge.o uv_tool_polygon.o uv_transform.o uv_tool_select.o uv_tool_strip.o uv_unfold.o \
		uv_view.o uv_input_parse.o libseduce.a libbetray.a libenough.a st_types.o
		gcc -o $@ $^ $(LDFLAGS) $(LDLIBS)

loqairou:	la_draw_overlay.o la_flare_fx.o la_geometry_undo.o la_input_parser.o la_intro.o la_key_input.o \
		la_main.o la_particle_fx.o la_pop_up.o la_projection.o la_reference.o la_save.o \
		la_settings.o la_tool_collapse.o la_tool_deploy.o la_tool_draw.o la_tool_edge_connector.o \
		la_tool_manipulator.o la_tool_poly_select.o la_tool_reshape.o la_tool_revolve.o la_tool_select.o \
		la_tool_slice.o la_tool_splitter.o la_tool_subdivide.o \
		st_math.c st_matrix_operations.c st_text.c st_types.c \
		libseduce.a libbetray.a libenough.a
		gcc -o $@ $^ $(LDFLAGS) $(LDLIBS)

# -----------------------------------------------

libseduce.a:	s_background.o s_draw.o s_editor.o s_line_font.o s_main.o s_popup.o s_settings.o s_text.o
		ar -cr $@ $^

libbetray.a:	b_glut.o b_main.o b_sdl.o
		ar -cr $@ $^

libenough.a:	e_storage_bitmap.o e_storage_curve.o e_storage_code.o e_storage_geometry.o e_storage_head.o e_storage_material.o \
		e_storage_node.o e_storage_object.o
		ar -cr $@ $^

# -----------------------------------------------

clean:
	rm -f $(APPS) *.o *.a
