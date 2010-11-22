/*
 * rrddriver
 * builds a rrdtool database from smbta data
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

#include <stdio.h>
pthread_t thread;


int configuration_check_configuration( struct configuration_data *c );
void configuration_create_db(struct configuration_data *c);


void configuration_show_help()
{
        printf("rrddriver version %s\n", RRDDRIVER_VERSION);
        printf("(C)opyright 2010 by Michael Haefner\n");
	printf("(C)opyright 2010 by Benjamin Brunner\n");
        printf("(C)opyright 2010 by Holger Hetterich\n");
        printf("%s\n", SMBTA_LICENSE);
        printf("\n");
        printf("-i      --inet-port <num>       Set the port-number to  \n");
        printf("                                use to <num>.\n");
	printf("-h      --host <string>		Set the host name to connect to.\n");
	printf("-n	--unix-domain-socket	Use a unix domain socket to \n");
	printf("				communicate with smbtad.\n");
        printf("-d      --debug-level <num>     Set the debug level to work\n");
        printf("                                with to <num>. Default: 0\n");
        printf("-c      --config-file <file>    Load the configuration from\n");
        printf("                                a file given as <file>.\n");
	printf("-s      --share <string>	Specify a share to monitor.\n");
	printf("-u	--user <string>		Specify a user to monitor.\n");
	printf("-f	--file <string>		Specify a file to monitor.\n");
	printf("-g	--global		Global mode, run over the full\n");
	printf("				data set.\n");
	printf("-t      --timer <num>           Number of seconds defining the intervall\n");
	printf("				to update the rddtool database.\n");
	printf("				Default: 10 seconds\n");
	printf("-b	--database <string>	database filename\n");
	printf("-r	--rrdtool-setup		rrdtool-setup string\n");
	printf("				Default is:\n");
	printf("				DS:readwrite:GAUGE:10:U:U\n");
	printf("				DS:read:GAUGE:10:U:U\n");
	printf("				DS:write:GAUGE:10:U:U\n");
        printf("\n");
}

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
	c->timer = 10;
	c->database = strdup( "database_rrd" );
	c->unix_socket = 0;
	c->rrdtool_setup = strdup( "DS:readwrite:GAUGE:1000:0:U DS:read:GAUGE:1000:0:U DS:write:GAUGE:1000:0:U RRA:AVERAGE:0:10:8640");
	c->object_name = NULL;
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
        return 0;
}



/* load $HOME/.smbtatools/monitor.config */
void configuration_default_config(TALLOC_CTX *ctx,struct configuration_data *c)
{
        char *a=getenv("HOME");
        char *f = talloc_asprintf(ctx,"%s/.smbtatools/rrddriver.config",a);
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
                configuration_load_key_from_file( c);
        }
        return 0;
}

char *configuration_generate_pattern( TALLOC_CTX *ctx, struct configuration_data *c);


int configuration_parse_cmdline( struct configuration_data *c,
        int argc, char *argv[] )
{
        int i;
	char *pattern;
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
                        { "host",1,NULL,'h'},
			{ "help",0,NULL,'?'},
			{ "share",1,NULL,'s'},
			{ "user",1,NULL,'u'},
			{ "file",1,NULL,'f'},
			{ "timer",1,NULL,'t'},
			{ "database",1,NULL,'b'},
			{ "unix-socket",0,NULL,'n'},
			{ "rrdtool-setup",1,NULL,'r'},
                        { 0,0,0,0 }
                };

                i = getopt_long( argc, argv,
                        "ns:u:f:d:i:c:k:h:t:b:r:?", long_options, &option_index );

                if ( i == -1 ) break;

                switch (i) {
                        case 'i':
                                c->port = atoi( optarg );
                                break;
			case 'r':
				free(c->rrdtool_setup);
				c->rrdtool_setup = strdup(optarg);
				break;
			case 'n':
				c->unix_socket = 1;
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
			case 'b':
				c->database = strdup(optarg);
				break;
                        case 'k':
                                c->keyfile = strdup( optarg);
                                configuration_load_key_from_file(c);
                                break;
			case '?':
				configuration_show_help();
				exit(0);
			case 's':
				c->object_type = SMBTA_SHARE;
				c->object_name = strdup( optarg );
				break;
			case 'u':
				c->object_type = SMBTA_USER;
				c->object_name = strdup( optarg );
				break;
			case 'f':
				c->object_type = SMBTA_FILE;
				c->object_name = strdup( optarg );
				break;
			case 't':
				c->timer= atoi(optarg);
				break;
                        default :
                                printf("ERROR: unkown option.\n\n");
                                configuration_show_help();
                                return -1;
                }
        }

        configuration_default_config( runtime_mem, c);
        if (c->config_file != NULL)
                configuration_load_config_file(c);
        if (configuration_check_configuration(c)==-1) exit(1);
	configuration_create_db(c);
	if (c->unix_socket == 0)
		c->socket = common_connect_socket( c->host, c->port );
	else
		c->socket = common_connect_unix_socket(
			"/var/tmp/stadsocket_client");

	monitor_list_init();
        /* through all options, now run the query command */
	pattern = configuration_generate_pattern(runtime_mem, c);
        network_register_monitor(MONITOR_READ,"NULL",pattern,c);
	network_register_monitor(MONITOR_WRITE,"NULL",pattern,c);
	/* run the networking thread */
	pthread_create(&thread,NULL,(void *)&network_handle_data,(void *) c);
	/* main loop 
	 * FIXME: Add Keyboard handling here !!
	 */
	while (1 == 1) {
		sleep( c->timer );
		// push values into rrdtool database
		monitor_list_push_values(c);
	}



        close(c->socket);
        TALLOC_FREE(runtime_mem);
        return 0;
}



int configuration_check_configuration( struct configuration_data *c )
{
	FILE *rrdtoolbin;
        if ( c->debug_level <0 || c->debug_level>10 ) {
                printf("ERROR: debug level has to be between 0 and 10.\n");
                return -1;
        }
        if (c->host == NULL && c->unix_socket == 0) {
                printf("ERROR: please specify a hostname to connect to.\n");
                return -1;
        }
	if (c->timer <= 0 ) {
		printf("ERROR: timer for the update intervall is too short!\n");
		return -1;
	}
	/* check for rrdtool to be available */
	rrdtoolbin = fopen("/usr/bin/rrdtool","r");
	if (rrdtoolbin != NULL)
		fclose(rrdtoolbin);
	else {
		printf("ERROR: rrdtool doesn't exist in /usr/bin/rrdtool !\n");
		return -1;
	}
	if (c->object_name == NULL) {
		printf("ERROR: please specify at least one object. ( -f -s -g )\n");
		return -1;
	}

        return 0;
}

void configuration_create_db(struct configuration_data *c)
{
	char rrdbin[255] = "/usr/bin/rrdtool";
	char fullstr[500];
	int res = 0;
	time_t starttime = time(NULL);
	char timestr[255];
	sprintf(timestr,"%ju",(uintmax_t) starttime);
	sprintf(fullstr,"%s create %s -b %s -s %i %s",
		rrdbin,
		c->database,
		timestr,
		c->timer,
		c->rrdtool_setup);

	res = system( fullstr );
	if (res == -1) {
		printf("ERROR: error creating the database.\n");
	}
}

/* 
 * This fills the filter pattern for the monitor by running identify,
 * and eventually filling more required patterns
 */
char *configuration_generate_pattern( TALLOC_CTX *ctx, struct configuration_data *c)
{
	/*
	 * How the pattern is build:
	 * USER, USERSID, SHARE, FILE, DOMAIN
	 *
	 */
	char *pattern;
	pattern = common_identify( ctx,c->object_type,c->object_name,c,1);
	return pattern;
}	
