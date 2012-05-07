/*
 * monitor-cli
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
#include <spawn.h>
pthread_t thread;


int configuration_check_configuration( struct configuration_data *c );
void configuration_create_db(struct configuration_data *c);


static void configuration_show_help()
{
        printf("smbtamonitor-cli version %s\n", SMBTAMONITOR_VERSION);
	printf("Command line client for smbtamonitor\n");
	printf("(C)opyright 2011 by Michael Haefner\n");
	printf("(C)opyright 2011 by Benjamin Brunner\n");
        printf("(C)opyright 2011 by Holger Hetterich\n");
	printf("(C)opyright 2011 by Nanuk Krinner\n");
        printf("%s\n", SMBTA_LICENSE);
        printf("\n");
	printf("-u $USER			Monitor user $USER.\n");
	printf("-s $SHARE			Monitor share $SHARE.\n");
	printf("-g				Monitor globally.\n");
	printf("-d $DOMAIN			Monitor domain $DOMAIN.\n");
	printf("-f $FILE			Monitor file $FILE.\n");
	printf("\n");
	printf("-h $HOST			Hostname of the\n");
	printf("				smbtad system.\n");
	printf("-i $PORT			Port number to use to\n");
	printf("				connect to the smbtad\n");
	printf("				system.\n");
	printf("\n");
}

/**
 * Initialize default values of the configuration.
 */
static void configuration_define_defaults( struct configuration_data *c )
{
        c->port = 3390;
        c->host = NULL;
        c->config_file = NULL;
        c->debug_level = 0;
        c->keyfile =NULL;
	c->timer = 10;
	c->unix_socket = 0;
	c->object_name = NULL;
	c->identify = 0;
	c->path = strdup ("/var/tmp/");
}

/* load $HOME/.smbtatools/monitor.config */
static void configuration_default_config(TALLOC_CTX *ctx,struct configuration_data *c)
{
        char *a=getenv("HOME");
        char *f = talloc_asprintf(ctx,"%s/.smbtatools/smbtatools.config",a);
        FILE * fi = fopen(f,"r");
        if (fi != NULL) {
                fclose(fi);
                if (c->config_file != NULL) return;
                c->config_file = f;
        }
}

static int configuration_load_config_file( struct configuration_data *c)
{
        dictionary *Mydict;
        Mydict=iniparser_load( c->config_file);
        char *cc;

        if ( Mydict == NULL ) return -1;

        cc = iniparser_getstring( Mydict, "network:smbta_port_number",NULL);
        if (cc != NULL) c->port = (int) common_myatoi(cc);

        cc = iniparser_getstring( Mydict, "network:smbta_host_name",NULL);
        if (cc != NULL) c->host = strdup(cc);
        cc = iniparser_getstring(Mydict,"general:debug_level",NULL);
        if (cc != NULL) {
                c->debug_level = (int) common_myatoi(cc);
        }
        cc = iniparser_getstring(Mydict,"general:keyfile",NULL);
        if (cc != NULL) {
                common_load_key_from_file( c);
        }
	cc = iniparser_getstring(Mydict,"general:identify",NULL);
	if (cc != NULL) {
		c->identify = (int) common_myatoi(cc);
	}
	cc = iniparser_getstring(Mydict,"network:socket_path",NULL);
	if (cc != NULL) c->path = strdup(cc);
        return 0;
}

static char *configuration_generate_pattern( TALLOC_CTX *ctx, struct configuration_data *c);


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
                        { "host",1,NULL,'h'},
			{ "help",0,NULL,'?'},
			{ "share",1,NULL,'s'},
			{ "user",1,NULL,'u'},
			{ "domain",1,NULL,'d'},
			{ "file",1,NULL,'f'},
			{ "global",0,NULL,'g'},
                        { 0,0,0,0 }
                };

                i = getopt_long( argc, argv,
                        "i:h:?s:u:d:f:g", long_options, &option_index );

                if ( i == -1 ) break;

                switch (i) {
			case 'g':
				c->object_type = 6;
				c->object_name = strdup( "-g");
				break;
			case 'D':
				c->object_type = SMBTA_DOMAIN;
				c->object_name = strdup( optarg );
				break;
				
			case 'I':
				c->identify = (int) common_myatoi( optarg );
				break;
                        case 'i':
                                c->port = (int) common_myatoi( optarg );
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
                                c->debug_level = (int) common_myatoi( optarg );
                                break;
                        case 'c':
                                c->config_file = strdup( optarg );
                                break;
			case 'b':
				c->database = strdup(optarg);
				break;
                        case 'k':
                                c->keyfile = strdup( optarg);
                                common_load_key_from_file(c);
                                break;
			case '?':
				configuration_show_help();
				exit(0);
			case 's':
				c->object_type = SMBTA_SHARE;
				c->object_name = talloc_asprintf(runtime_mem, "-s %s",optarg);
				
				break;
			case 'u':
				c->object_type = SMBTA_USER;
				c->object_name = talloc_asprintf(runtime_mem, "-u %s",optarg);
				break;
			case 'f':
				c->object_type = SMBTA_FILE;
				c->object_name = talloc_asprintf(runtime_mem, "-f %s",optarg);
				break;
			case 't':
				c->timer= (int) common_myatoi(optarg);
				break;
			case 'p':
				c->path = strdup( optarg );
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

	// Run the monitor-gen process
	//

	char *cmdstr = talloc_asprintf(runtime_mem, "exec smbtamonitor-gen -h %s -i %i %s &",
			c->host, c->port, c->object_name);
	printf("%s\n", cmdstr);
	system( cmdstr );
	exit(1);


	while (1 == 1) {
		sleep( 100 );
	}



        close(c->socket);
        TALLOC_FREE(runtime_mem);
        return 0;
}



int configuration_check_configuration( struct configuration_data *c )
{
       struct stat sb;
    
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


	if (c->object_name == NULL) {
		printf("ERROR: please specify at least one object. ( -f -s -g )\n");
		return -1;
	}

	if (c->identify <0 || c->identify >1) {
		printf("\nERROR: please specify either 1 or 0 for identify.\n");
		return -1;
	}

        return 0;
}

void configuration_create_db(struct configuration_data *c)
{
	FILE *db;
	db = fopen(c->database,"r");
	if (db != NULL) { fclose(db); return; }
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
static char *configuration_generate_pattern( TALLOC_CTX *ctx, struct configuration_data *c)
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
