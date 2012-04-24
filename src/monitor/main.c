/*
 * smbtamonitor
 * real-time monitor for SMB traffic analyzer
 *
 * Copyright (C) Michael Haefner, 2010
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

#include "include/includes.h"

int main(int argc, char *argv[])
{
	/* main configuration structure */
	struct configuration_data conf;

        /* parse command line, and process */
        if ( configuration_parse_cmdline( &conf, argc, argv ) <0 ) exit(1);

	return 0;
}	
