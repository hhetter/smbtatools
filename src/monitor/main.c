/*
 * smbtamonitor
 * real-time monitor for samba traffic analyzer
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

#include <curses.h>
#include <stdlib.h>

#define VERSION "0.0.1"

WINDOW *win;

void monitor_shutdown(void)
{
	endwin();
	system("clear");
	curs_set(1);
	fprintf(stdout, "smbtamonitor %s by Michael Haefner\n", VERSION);
	exit(0);
}

int main(void)
{

}	
