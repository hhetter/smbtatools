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

#include <stdlib.h>
#include "include/configuration.h"
#include "../../include/version.h"
int configuration_check_configuration( struct configuration_data *c );


void configuration_show_help()
{
        printf("smbtamonitor version %s\n", SMBTAMONITOR_VERSION);
        printf("(C)opyright 2010 by Michael Haefner\n");
	printf("(C)opyright 2010 by Benjamin Brunner\n");
        printf("(C)opyright 2010 by Holger Hetterich\n");
        printf("%s\n", SMBTA_LICENSE);
        printf("\n");
        printf("-i      --inet-port <num>       Set the port-number to  \n");
        printf("                                use to <num>.\n");
        printf("-d      --debug-level <num>     Set the debug level to work\n");
        printf("                                with to <num>. Default: 0\n");
        printf("-c      --config-file <file>    Load the configuration from\n");
        printf("                                a file given as <file>.\n");
	printf("-s      --share <string>	Specify a share to monitor.\n");
	printf("-u	--user <string>		Specify a user to monitor.\n");
	printf("-f	--file <string>		Specify a file to monitor.\n");
	printf("-g	--global		Global mode, run over the full\n");
	printf("				data set.\n");
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
        char *f = talloc_asprintf(ctx,"%s/.smbtatools/monitor.config",a);
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
                        { "host",1,NULL,'h'},
			{ "help",0,NULL,'?'},
			{ "share",1,NULL,'s'},
			{ "user",1,NULL,'u'},
			{ "file",1,NULL,'f'},
                        { 0,0,0,0 }
                };

                i = getopt_long( argc, argv,
                        "s:u:f:d:i:c:k:h:?", long_options, &option_index );

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
        c->socket = common_connect_socket( c->host, c->port );

        /* through all options, now run the query command */

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

/* 
 * This fills the filter pattern for the monitor by running identify,
 * and eventually filling more required patterns
 */
void configuration_generate_pattern( struct configuration_data *c)
{
	/*
	 * How the pattern is build:
	 * USER, USERSID, SHARE, FILE, DOMAIN
	 *
	 */
	int t;
	for (t=0; t<5; t++) {
		if (c->object_type != t) c->pattern[t]=strdup("*");
	}

	c->pattern[c->object_type] = strdup(c->object_name);
}	
