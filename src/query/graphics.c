/*
 * smbtaquery
 * tool for querying the samba traffic analyzer
 *
 * Copyright (C) Benjamin Brunner, 2010
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "include/graphics.h"

/**
 * graphics.c, a integrated graphics engine for smbtaquery optimized for
 * SMBTA.
 */

void smbta_gfx_simple_diagram(
		int imgwidth,
		int imgheight,
		unsigned long int *yaxis_r,
		unsigned long int *yaxis_w,
		int type,
		unsigned long int maximum)
{
	int x;
	cairo_surface *surface = cairo_svg_surface_create(
			"test",
			imgwidth,
			imgheight);
	cairo_t *cr = cairo_create(surface);
	/* black background */
	cairo_set_source_rgb(cr,0,0,0);
	cairo_paint(cr);
	cairo_scale(cr, maximum, imgwidth);
	cairo_set_line_width(cr,0.1);
	cairo_move_to(cr,1,0.1);
	for (x = 0;x < imgwidth;x++) {
		if (type == SMBTA_GFX_RW) {
			cairo_move_to(cr, x, 0.1);
			cairo_line_to (cr, x, yaxis_r[x]);
			cairo_move_to(cr, x, yaxis_r[x]);
			cairo_line_to (cr, x, yaxis_w[x]);
		} else if (type == SMBTA_GFX_R) {
			cairo_move_to(cr, x, 0.1);
			cairo_line_to(cr, x, yaxis_r[x]);
		} else if (type == SMBTA_GFX_W) {
			cairo_move_to(cr, x, 0.1);
			cairo_line_to(cr, x, yaxis_w[x]);
		}
	}
	cairo_stroke(cr);
	cairo_surface_flush(cr);
	cairo_surface_destroy(cr);
}

