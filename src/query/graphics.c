/*
 * smbtaquery
 * tool for querying the samba traffic analyzer
 *
 * Copyright (C) Benjamin Brunner, 2010,2011,2012
 * Copyright (C) Holger Hetterich, 2012
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
#include <netdb.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <signal.h>
#include <pthread.h>
#include <getopt.h>
#include <syslog.h>
#include <sys/select.h>
#include <dlfcn.h>
#include <string.h>
#include <stdio.h>
#include <talloc.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <dbi.h>
/**
 * graphics.c, a integrated graphics engine for smbtaquery optimized for
 * SMBTA.
 */

enum SMBTA_GFX {
	SMBTA_GFX_R,
	SMBTA_GFX_W,
	SMBTA_GFX_RW};
/*
static double smbta_gfx_downscale_maximum( unsigned long int kb,
		char *type)
{
	double rr;
	char kbstring[20];
	char *output;
	long long int result = kb;
	long long int rest = 0;
	lldiv_t diff;
	strcpy(kbstring,"Bytes");
	if (kb >= (long long )1024*1024*1024*1024) {
		diff = lldiv(kb,(long long ) 1024*1024*1024*1024); // tb
		strcpy(kbstring,"TB");
		result = diff.quot;
		rest = diff.rem;
	} else
		if (kb >= (long long )1024*1024*1024) {
		diff = lldiv(kb,(long long) 1024*1024*1024); // gb
		strcpy(kbstring,"GB");
		result = diff.quot;
		rest = diff.rem;
		} else
			if (kb >= (long long) 1024*1024) {
				diff = lldiv(kb,(long long) 1024*1024); // mb
				strcpy(kbstring,"MB");
				result = diff.quot;
				rest = diff.rem;
		     	} else
			if (kb >= 1024) {
				diff =  lldiv(kb, (long long) 1024); // kb
				strcpy(kbstring,"KB");
				result = diff.quot;
				rest = diff.rem;
			}
	output = talloc_asprintf( NULL,"%lli.%lli",result,rest);
	sscanf(output,"%lf",&rr);
	talloc_free(output);
	strcpy(type,kbstring);
	return rr;
}
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
	double per;
	cairo_surface_t *surface = cairo_svg_surface_create(
			"test",
			imgwidth,
			imgheight);
	cairo_t *cr = cairo_create(surface);
	/* black background */
	per = (double) maximum / 100;
	cairo_set_source_rgb(cr,0,0,0);
	cairo_paint(cr);
	cairo_scale(cr, imgwidth,imgheight); // imgwidth
	cairo_set_line_width(cr,0.01);
	cairo_set_source_rgb(cr,0,1,0);
	for (x = 0;x < imgwidth;x++) {
		double xpos = ((double) x / ( (double) imgwidth / 100)) / 100;
		cairo_move_to(cr, xpos, 1.0);
		if (type == SMBTA_GFX_RW) {
			cairo_set_source_rgb(cr,0,1,0);
			cairo_line_to (cr, xpos, 1- (double) (( double) (yaxis_r[x]/ per)/ 100));
			cairo_move_to(cr, xpos, 1- (double) (( double) (yaxis_r[x]/ per)/ 100));
			cairo_set_source_rgb(cr,0,0.5,0);
			cairo_line_to (cr, xpos, 1- (double) (( double) (yaxis_w[x]/ per)/ 100));
		} else if (type == SMBTA_GFX_R) {
			cairo_line_to(cr, xpos, 1 - (double) ( ( (double) yaxis_r[x]/per) / 100));
		} else if (type == SMBTA_GFX_W) {
			cairo_line_to(cr, xpos, 1 - (double) ( ( (double) yaxis_w[x]/per) / 100));
		}
	}
	cairo_stroke(cr);
	cairo_surface_flush(surface);
	cairo_surface_destroy(surface);
	cairo_destroy(cr);
}

