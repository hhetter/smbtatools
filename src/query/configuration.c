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
#include "../../include/version.h"
#include "include/configuration.h"
#include "include/interpreter.h"
#include <talloc.h>
#include <sqlite3.h>
sqlite3 *create_db()
{
	char *a=getenv("HOME");
	char path[255];
	char full_path[255];
	strncpy(full_path,a,250);
	strncpy(path,a,250);
	strncat(path,"/.smbtatools",250);	
	mkdir(path, S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO);
	strncat(full_path,"/.smbtatools/smbtaquery-db.sql",250);
	sqlite3 *db;
        int rc;
        char *zErrormsg;
        rc=sqlite3_open( full_path,&db);
        if ( rc ) {
                syslog(LOG_DAEMON,
                        "plugin-sqlite3 : ERROR: Can't open Database :"
                        " %s, exiting.\n",
                        sqlite3_errmsg(db));
                return NULL;
        }

	rc = sqlite3_exec( db, \
                "CREATE TABLE last_activity_data ("
                "timestamp DATE, message varchar )",NULL,0,&zErrormsg);


	return db;
}

int configuration_check_configuration( struct configuration_data *c );


/**
 * Initialize default values of the configuration.
 */
void configuration_define_defaults( struct configuration_data *c )
{
	c->port = 3390;
	c->host = NULL;
	c->config_file = NULL;
	c->debug_level = 0;
	c->keyfile =NULL;
	c->query = NULL;
	c->file = NULL;
	c->unix_socket = 0;
}

/* load $HOME/.smbtatools/query.config */
void configuration_default_config(TALLOC_CTX *ctx,struct configuration_data *c)
{
        char *a=getenv("HOME");
        char *f = talloc_asprintf(ctx,"%s/.smbtatools/query.config",a);
	FILE * fi = fopen(f,"r");
        if (fi != NULL) {
                fclose(fi);
                if (c->config_file != NULL) return;
		c->config_file = f;
        }
}


int configuration_load_config_file( struct configuration_data *c)
{
	dictionary *Mydict;
	Mydict=iniparser_load( c->config_file);
	char *cc;

	if ( Mydict == NULL ) return -1;

	cc = iniparser_getstring( Mydict, "network:port_number",NULL);
	if (cc != NULL) c->port = atoi(cc);

	cc = iniparser_getstring( Mydict, "network:host_name",NULL);
	if (cc != NULL) c->host = strdup(cc);
	cc = iniparser_getstring(Mydict,"general:debug_level",NULL);
	if (cc != NULL) {
		c->debug_level = atoi(cc);
	}
	cc = iniparser_getstring(Mydict,"general:keyfile",NULL);
	if (cc != NULL) {
		common_load_key_from_file( c);
	}
	cc = iniparser_getstring( Mydict, "network:unix_domain_socket",NULL);
	if (cc != NULL) c->unix_socket = 1;

	return 0;
}

void configuration_show_help()
{
	printf("smbtaquery version %s\n", SMBTAQUERY_VERSION);
	printf("(C)opyright 2010 by Benjamin Brunner\n");
	printf("(C)opyright 2010 by Michael Haefner\n");
	printf("(C)opyright 2010 by Holger Hetterich\n");
	printf("%s\n", SMBTA_LICENSE);
	printf("\n");
	printf("-i	--inet-port <num>	Set the port-number to	\n");
	printf("				use to <num>.\n");
	printf("-h	--host <string>		Define the host name to \n");
	printf("				connect to.\n");
	printf("-d	--debug-level <num>	Set the debug level to work\n");
	printf("				with to <num>. Default: 0\n");
	printf("-c	--config-file <file>	Load the configuration from\n");
	printf("				a file given as <file>.\n");
	printf("-q      --query			Run an interpreter command,\n");
	printf("				or run a SQL select command.\n");
	printf("-p      --command-help		Interpreter command description.\n");
	printf("-f      --file <file>		Read the commands from a file.\n");
	printf("-u	--unix-domain-socket	Use a unix domain socket to \n");
	printf("				connect to smbtad.\n");
	printf("\n");
}

int configuration_parse_cmdline( struct configuration_data *c,
	int argc, char *argv[] )
{
	int i;
	TALLOC_CTX *runtime_mem = NULL;
	c->db = create_db();
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
			{ "help",0,NULL,'?'},
			{ "command-help",0,NULL,'p'},
			{ "file",1,NULL,'f'},
			{ "unix-domain-socket",0,NULL,'u'},
			{ 0,0,0,0 }
		};

		i = getopt_long( argc, argv,
			"d:f:i:c:k:q:h:p?u", long_options, &option_index );

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
				break;
			case 'k':
				c->keyfile = strdup( optarg);
				common_load_key_from_file(c);
				break;
			case 'q':
				c->query = strdup( optarg );
				break;
			case 'p':
				interpreter_command_help();
				exit(0);
				break;
			case '?':
				configuration_show_help();
				exit(0);
			case 'f':
				c->file = strdup( optarg );
				break;
			case 'u':
				c->unix_socket = 1;
				break;
			default	:
				printf("ERROR: unkown option.\n\n");
				configuration_show_help();
				return -1;
		}
	}

	configuration_default_config( runtime_mem, c);
	if (c->config_file != NULL)
		configuration_load_config_file(c);
	if (configuration_check_configuration(c)==-1) exit(1);
	if (c->unix_socket != 1)
		c->socket = common_connect_socket( c->host,
			c->port );
	else
		c->socket = common_connect_unix_socket(
			"/var/tmp/stadsocket_client");

	/* through all options, now run the query command */
	if (c->query != NULL) {
		interpreter_run( runtime_mem, c->query, c);
		free(c->query);
	}
	if (c->file != NULL) {
		interpreter_run_from_file( runtime_mem, c->file, c);
		free(c->file);
	}

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
	if (c->host == NULL && c->unix_socket != 1) {
		printf("ERROR: please specify a hostname to connect to.\n");
		return -1;
	}
	return 0;
}
