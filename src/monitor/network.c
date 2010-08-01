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
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include "include/includes.h"
/*
 * transmit the monitor function and the pattern
 * to smbtad and receive the id
 */
int network_register_monitor( enum monitor_fn func, char *pattern, struct configuration_data *c)
{
        char *tosend, *data;
        int i = asprintf(&tosend, "~~0001%i%s",func,pattern);
        char *body = sql_query(NULL,c,tosend);
        printf("%s\n",body);
	data = result_get_element(NULL,0,body);
	monitor_list_add( atoi(data ));	
        return 1;
}

