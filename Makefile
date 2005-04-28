#
# Rough and ready Makefile for compiling Connector, Loq Auirou, and UVedit tools.
#

VERSE=../verse

CFLAGS=-I$(VERSE) -g -Wstrict-prototypes
LDFLAGS=-L$(VERSE) -L/usr/X11R6/lib
LDLIBS=-lverse -lGL -lm -lglut  -lGLU -lSDL

APPS=connector uvedit loqairou #quelsolaar

ALL:		$(APPS)

# -----------------------------------------------

connector:	co_main.o co_game.o co_intro.o co_vn_audio.o co_vn_bitmap.o co_vn_curve.o co_vn_geometry.o co_vn_graphics.o \
		co_vn_handle.o co_vn_head.o co_vn_mat_render.o co_vn_material.o co_vn_object.o co_vn_search.o \
		co_vn_text.o co_widgets.o  \
		libseduce.a libbetray.a libenough.a
		gcc -o $@ $^ $(LDFLAGS) $(LDLIBS)

uvedit:		uv_draw.o uv_edge_collapse.o uv_geometry.o uv_main.o uv_menu.o uv_overlay.o uv_popup.o uv_tool_corner.o\
		uv_tool_edge.o uv_tool_polygon.o uv_transform.o uv_tool_select.o uv_tool_strip.o uv_unfold.o \
		uv_view.o uv_input_parse.o libseduce.a libbetray.a libenough.a
		gcc -o $@ $^ $(LDFLAGS) $(LDLIBS)

loqairou:	la_draw_overlay.o la_flare_fx.o la_geometry_undo.o la_input_parser.o la_intro.o la_key_input.o \
		la_main.o la_particle_fx.o la_pop_up.o la_projection.o la_reference.o la_save.o \
		la_settings.o la_tool_collapse.o la_tool_deploy.o la_tool_draw.o la_tool_edge_connector.o \
		la_tool_manipulator.o la_tool_poly_select.o la_tool_reshape.o la_tool_revolve.o la_tool_select.o \
		la_tool_slice.o la_tool_splitter.o la_tool_subdivide.o \
		st_math.c st_matrix_operations.c st_text.c \
		libseduce.a libbetray.a libenough.a
		gcc -o $@ $^ $(LDFLAGS) $(LDLIBS)

quelsolaar:	qs_camera.o qs_intro.o qs_main.o \
		st_math.c st_matrix_operations.c st_text.c st_types.c \
		libpersuade.a libseduce.a libbetray.a libenough.a
		gcc -o $@ $^ $(LDFLAGS) $(LDLIBS)

vml-saver:	vml-saver.c libenough.a

# -----------------------------------------------

libseduce.a:	s_background.o s_draw.o s_editor.o s_line_font.o s_main.o s_popup.o s_settings.o s_text.o
		ar -cr $@ $^

libbetray.a:	b_glut.o b_main.o b_sdl.o
		ar -cr $@ $^

libenough.a:	e_storage_audio.o e_storage_bitmap.o e_storage_curve.o e_storage_geometry.o e_storage_head.o e_storage_material.o \
		e_storage_node.o e_storage_object.o e_storage_text.o st_types.o
		ar -cr $@ $^

libpersuade.a:	p_geo_back_ref.o p_geo_renderable_mesh.o p_geo_subdivide.o p_geo_subdivide_basic.o p_geo_subdivide_depend.o \
		p_geo_subdivide_edge.o p_geo_subdivide_face.o p_geo_subdivide_param.o p_geo_subdivide_select.o p_geo_subdivide_vertex.o \
		p_geo_tesselation_table.o p_geometry.o p_object_environment.o p_object_handle.o p_object_param.o p_object_render.o \
		p_shader_bind.o p_shader_extension.o p_shader_writer.o p_task.o p_temp_draw.o p_texture.o

# -----------------------------------------------

clean:
	rm -f $(APPS) *.o *.a
