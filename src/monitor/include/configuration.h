/* 
 * smbtaquery 
 * capture transfer data from the vfs_smb_traffic_analyzer module, and store
 * the data via various plugins
 *
 * Copyright (C) Holger Hetterich, 2008
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
#define _SVID_SOURCE || _BSD_SOURCE || _XOPEN_SOURCE >= 500

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <signal.h>
#include <getopt.h>
#include <syslog.h>
#include <sys/select.h>
#include <dlfcn.h>
#include <talloc.h>
#include "../../../iniparser3.0b/src/iniparser.h"

struct configuration_data {
	/* Number of the port to use */
	int port;
	/* hostname of server to connect to */
	char *host;
	/* debug level */
	int debug_level;
	/* configuration file */
	char *config_file;
	/* AES Key */
	unsigned char key[20];
	/* AES Keyfile */
	char *keyfile;
	
	/* runtime data */
	int socket;
};

int configuration_check_configuration( struct configuration_data *c );
int configuration_parse_cmdline( struct configuration_data *c, int argc, char *argv[] );

