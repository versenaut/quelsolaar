#
# Rough and ready Makefile for compiling Connector, Loq Auirou, and UVedit tools.
#

VERSE=../verse

.PHONY:		clean dist dist-enough dist-connector dist-loqairou

CFLAGS=-I$(VERSE) -g -Wstrict-prototypes
LDFLAGS=-L$(VERSE) -L/usr/X11R6/lib
LDLIBS=-lverse -lGL -lm -lglut  -lGLU -lSDL

# Comment out the below to disable persuade. You cannot build QuelSolaar without
# Persuade, but Loq Airou and Connector support compiling without it.
#PERSUADE=libpersuade.a
#CFLAGS+=-DPERSUADE_ENABLED

DATE=`date --iso-8601 | tr -d -`
SYS=`uname -s | tr -d ' ' | tr [A-Z] [a-z]`-`uname -m | tr -d ' '`
DIST=$(DATE)-$(SYS)


APPS=connector uvedit loqairou quelsolaar

ALL:		$(APPS)

# -----------------------------------------------

connector:	co_3d_view.o co_main.o co_game.o co_intro.o co_popup.o co_projection.o co_summary.o co_vn_audio.o co_vn_bitmap.o \
		co_vn_curve.o co_vn_geometry.o co_vn_graphics.o co_vn_handle.o co_vn_head.o co_vn_mat_render.o co_vn_material.o \
		co_vn_object.o co_vn_search.o co_vn_text.o co_widgets.o  \
		st_matrix_operations.o \
		libseduce.a libbetray.a libenough.a $(PERSUADE) libdeceive.a
		gcc -o $@ $^ $(LDFLAGS) $(LDLIBS)

uvedit:		uv_draw.o uv_edge_collapse.o uv_geometry.o uv_main.o uv_menu.o uv_overlay.o uv_popup.o uv_texture.o uv_tool_corner.o\
		uv_tool_edge.o uv_tool_polygon.o uv_transform.o uv_tool_select.o uv_tool_strip.o uv_unfold.o \
		uv_view.o uv_input_parse.o libbetray.a libenough.a $(PERSUADE) libdeceive.a libseduce.a
		gcc -o $@ $^ $(LDFLAGS) $(LDLIBS)

loqairou:	la_cache.o la_draw_overlay.o la_flare_fx.o la_geometry_undo.o la_hole.o la_input_parser.o la_intro.o la_key_input.o \
		la_main.o la_neighbor.o la_particle_fx.o la_pop_up.o la_projection.o la_reference.o la_save.o \
		la_settings.o la_tool_collapse.o la_tool_center.o la_tool_deploy.o la_tool_draw.o la_tool_edge_connector.o \
		la_tool_manipulator.o la_tool_poly_select.o la_tool_reshape.o la_tool_revolve.o la_tool_select.o \
		la_tool_slice.o la_tool_splitter.o la_tool_subdivide.o \
		st_math.o st_matrix_operations.o st_text.o \
		libseduce.a libbetray.a libenough.a libdeceive.a $(PERSUADE)
		gcc -o $@ $^ $(LDFLAGS) $(LDLIBS)

lpaint:		lp_geometry.o lp_handle.o lp_layer_group.o lp_main.o lp_menu.o lp_paint.o lp_popup.o \
		lp_projection.o \
		st_matrix_operations.o \
		libbetray.a libdeceive.a libenough.a libseduce.a
		gcc -o $@ $^ $(LDFLAGS) $(LDLIBS)

quelsolaar:	qs_camera.o qs_intro.o qs_main.o \
		st_math.o st_matrix_operations.o st_text.c st_types.o \
		libpersuade.a libseduce.a libbetray.a libenough.a
		gcc -o $@ $^ $(LDFLAGS) $(LDLIBS)

pngsave:	pngsave.c libenough.a
		gcc -o $@ $(CFLAGS) $^ $(LDFLAGS) $(LDLIBS) -lpng

# -----------------------------------------------

libseduce.a:	s_background.o s_draw.o s_editor.o s_line_font.o s_main.o s_popup.o s_settings.o s_text.o
		ar -cr $@ $^

libbetray.a:	b_glut.o b_main.o b_sdl.o
		ar -cr $@ $^

libenough.a:	e_storage_audio.o e_storage_bitmap.o e_storage_curve.o e_storage_geometry.o e_storage_head.o e_storage_material.o \
		e_storage_node.o e_storage_object.o e_storage_text.o st_types.o
		ar -cr $@ $^

libpersuade.a:	p_extension.o p_geometry.o p_object_environment.o p_object_handle.o p_object_light.o p_object_param.o \
		p_object_render.o p_sds_array.o p_sds_geo_clean.o p_sds_geo_divide.o p_sds_obj.o \
		p_sds_obj_anim.o p_sds_obj_displace.o p_sds_obj_edge_normal.o p_sds_obj_param.o p_sds_obj_sort.o p_sds_obj_tess.o \
		p_sds_table.o p_sds_table_debug.o p_sds_table_edge_sort.o p_sds_table_normals.o p_sds_table_split.o p_shader_bind.o \
		p_shader_extension.o p_shader_gl_one_fall_back.o p_shader_writer.o p_status_print.o \
		p_task.o p_texture.o
		ar -cr $@ $^

libdeceive.a:	d_main.o
		ar -cr $@ $^

# -----------------------------------------------

dist:	$(APPS)
	$(MAKE) dist-enough dist-connector dist-loqairou

dist-enough:	libenough.a enough.h README.enough
		tar czf enough-$(DIST).tar.gz $^

dist-connector:	connector README.connector
		tar czf connector-$(DIST).tar.gz $^

dist-loqairou:	loqairou README.loqairou
		mkdir -p loqairou-$(DIST) && cp --parents -r $^ docs/loq-airou loqairou-$(DIST)
		tar czf loqairou-$(DIST).tar.gz loqairou-$(DIST)

# -----------------------------------------------

clean:
	rm -f $(APPS) *.o *.a *.tar.gz
