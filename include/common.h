/*
 * common functions used by both clients
 *
 * Copyright (C) Michael Haefner, 2010
 * Copyright (C) Holger Hetterich, 2010
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

#define _XOPEN_SOURCE
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


/*
 * return a string representation of z
 * e.g. z=1024 output will be 1KB
 */
char *common_make_human_readable( TALLOC_CTX *ctx, unsigned long int z );
int common_connect_socket( const char *hostname,int iport );
