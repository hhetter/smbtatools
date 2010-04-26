/* 
 * smbtaquery
 * capture transfer data from the vfs_smb_traffic_analyzer module, and store
 * the data via various plugins
 *
 * Copyright (C) Holger Hetterich, 2008-2010
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

#include "include/configuration.h"
#include "include/interpreter.h"
#include "../../include/common.h"
#include <talloc.h>
/* Initialize default values of the configuration.			*/
void configuration_define_defaults( struct configuration_data *c )
{
	c->port = 3390;
	c->host = NULL;
	c->config_file = NULL;
	c->debug_level = 0;
	c->keyfile =NULL;
	c->query = NULL;
}

int configuration_load_key_from_file( struct configuration_data *c)
{
        FILE *keyfile;
        char *key = malloc(sizeof(char) * 17);
        int l;
        keyfile = fopen(c->keyfile, "r");
        if (keyfile == NULL) {
                return -1;
        }
        l = fscanf(keyfile, "%s", key);
        if (strlen(key) != 16) {
                printf("ERROR: Key file in wrong format\n");
                fclose(keyfile);
                exit(1);
        }
        strcpy( (char *) c->key, key);
	syslog(LOG_DEBUG,"KEY LOADEDi\n");
	return 0;
}



int configuration_load_config_file( struct configuration_data *c)
{
	dictionary *Mydict;
	Mydict=iniparser_load( c->config_file);
	char *cc;

	if ( Mydict == NULL ) return -1;

	cc = iniparser_getstring( Mydict, "network:port_number",NULL);
	if (cc != NULL) c->port = atoi(cc);


	cc = iniparser_getstring(Mydict,"general:debug_level",NULL);
	if (cc != NULL) {
		c->debug_level = atoi(cc);
	}
	cc = iniparser_getstring(Mydict,"general:keyfile",NULL);
	if (cc != NULL) {
		configuration_load_key_from_file( c);
	}
	return 0;
}

void configuration_show_help()
{
	printf("smbtaquery (C)2010 by Benjamin Brunner\n");
	printf("\n");
	printf("-i	--inet-port <num>	Set the port-number to	\n");
	printf("				use to <num>.\n");
	printf("-d	--debug-level <num>		Set the debug level to work\n");
	printf("				with to <num>. Default: 0\n");
	printf("-c	--config-file <file>	Load the configuration from\n");
	printf("				a file given as <file>.\n");
	printf("\n");
}

int configuration_parse_cmdline( struct configuration_data *c,
	int argc, char *argv[] )
{
	int i;
	TALLOC_CTX *runtime_mem = NULL;
	configuration_define_defaults( c );


	if ( argc == 1 ) {
		printf("ERROR: not enough arguments.\n\n");
		configuration_show_help();
		return -1;
	}

	while ( 1 ) {
		int option_index = 0;

		static struct option long_options[] = {\
			{ "inet-port", 1, NULL, 'i' },
			{ "debug-level",1, NULL, 'd' },
			{ "config-file",1,NULL,'c'},
			{ "key-file",1,NULL,'k'},
			{ "query",1,NULL,'q'},
			{ "host",1,NULL,'h'},
			{ 0,0,0,0 }
		};

		i = getopt_long( argc, argv,
			"d:i:c:k:q:h:", long_options, &option_index );

		if ( i == -1 ) break;

		switch (i) {
			case 'i':
				c->port = atoi( optarg );
				break;
			case 'h':
				c->host = strdup(optarg);
				break;
			case 'd':
				c->debug_level = atoi( optarg );
				break;
			case 'c':
				c->config_file = strdup( optarg );
				configuration_load_config_file(c);
				break;
			case 'k':
				c->keyfile = strdup( optarg);
				configuration_load_key_from_file(c);
				break;
			case 'q':
				c->query = strdup( optarg );
				break;
			default	:
				printf("ERROR: unkown option.\n\n");
				configuration_show_help();
				return -1;
		}
	}

	/* connect to the server */
	if (c->host == NULL) {
		printf("ERROR: please specify a host running smbtad!\n");
		exit(1);
	}

	c->socket = common_connect_socket( c->host, c->port );

	/* through all options, now run the query command */
	if (c->query != NULL) interpreter_run( runtime_mem, c->query, c);

	close(c->socket);
	TALLOC_FREE(runtime_mem);
	return 0;
}


int configuration_check_configuration( struct configuration_data *c )
{
	if ( c->debug_level <0 || c->debug_level>10 ) {
		printf("ERROR: debug level has to be between 0 and 10.\n");
		return -1;
	}
	if (c->host == NULL) {
		printf("ERROR: please specify a hostname to connect to.\n");
		return -1;
	}
	return 0;
}
