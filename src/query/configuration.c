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
#include "include/convert.h"
#include <talloc.h>

/* creates a local sqlite database that smbtaquery uses while in */
/* operation */
static sqlite3 *create_db()
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
                "timestamp DATE, message varchar, username varchar, file varchar, value varchar, domain varchar, vfs_func varchar )",NULL,0,&zErrormsg);

	return db;
}

int configuration_check_configuration( struct configuration_data *c );


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
	c->query = NULL;
	c->file = NULL;
	c->unix_socket = 0;
	c->query_xmlfile = NULL;
	c->xml_handle = NULL;
	c->query_output = QUERY_ASCII;
	/* with smbtaquery, identification is on by default */
	c->identify = 1;
	/* DBI */
	c->dbhost = NULL;
	c->dbname = NULL;
	c->dbuser = NULL;
	c->dbdriver = NULL;
	c->dbpassword = NULL;
	c->dbpath = NULL;
}

/* load $HOME/.smbtatools/query.config */
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

	cc = iniparser_getstring(Mydict,"general:debug_level",NULL);
	if (cc != NULL) {
		c->debug_level = (int) common_myatoi(cc);
	}
	cc = iniparser_getstring(Mydict,"general:keyfile",NULL);
	if (cc != NULL) {
		common_load_key_from_file( c);
	}

	cc = iniparser_getstring(Mydict,"database:name",NULL);
	if (cc != NULL) c->dbname = strdup(cc);

	cc = iniparser_getstring(Mydict,"database:host",NULL);
	if (cc != NULL) c->dbhost = strdup(cc);

	cc = iniparser_getstring(Mydict,"database:driver",NULL);
	if (cc != NULL) c->dbdriver = strdup(cc);

	cc = iniparser_getstring(Mydict,"database:user",NULL);
	if (cc != NULL) c->dbuser = strdup(cc);

	cc = iniparser_getstring(Mydict,"database:password",NULL);
	if (cc != NULL) c->dbpassword = strdup(cc);

	cc = iniparser_getstring(Mydict,"database:dbpath",NULL);
	if (cc != NULL) c->dbpath = strdup(cc);
	return 0;
}

static void configuration_show_help()
{
	printf("smbtaquery version %s\n", SMBTAQUERY_VERSION);
	printf("(C)opyright 2011 by Benjamin Brunner\n");
	printf("(C)opyright 2011 by Michael Haefner\n");
	printf("(C)opyright 2011 by Holger Hetterich\n");
	printf("%s\n", SMBTA_LICENSE);
	printf("\n");
	printf("-M	--dbdriver <str>	Set the libDBI database driver to use.\n");
	printf("-N	--dbname <str>		Set the name of the database to open.\n");
	printf("-S	--dbuser <str>		Set the name of the user for the database.\n");
	printf("-H	--dbhost <str>		Set the hostname to connect to.\n");
	printf("-P	--dbpassword <str>	Set the password to use.\n");
	printf("-d	--debug-level <num>	Set the debug level to work\n");
	printf("				with to <num>. Default: 0\n");
	printf("-c	--config-file <file>	Load the configuration from\n");
	printf("				a file given as <file>.\n");
	printf("-q      --query			Run an interpreter command,\n");
	printf("				or run a SQL select command.\n");
	printf("-p      --command-help		Interpreter command description.\n");
	printf("-f      --file <file>		Read the commands from a file.\n");
	printf("-x	--xml <file>		Output XML to file <file>.\n");
	printf("-o	--output		Specify the format to output.\n");
	printf("				Default: ascii\n");
	printf("-k	--keyfile <file>	Enable encryption and load the\n");
	printf("				key from <file>.\n");
	printf("-K      --create-key <file>	Create a key for encryption in <file>.\n");
	printf("-I	--identify <num>	0 = don't run identification,\n");
	printf("				1 = run idendification (default)\n");
	printf("-C	--convert		run an interactive conversion/update\n");
	printf("				process to convert an older database\n");
	printf("				to this version of SMBTA.\n");
	printf("-t	--test-db		Only try to connect to the database\n");
	printf("				and return the result.\n");
	printf("\n");
}

static void configuration_set_output( struct configuration_data *c,
	char *fmt)
{
	if (c->query_xmlfile != NULL) {
		printf("ERROR: please either specify -x or the -o option!\n");
		exit(1);
	}
	if (strncmp(optarg,"ascii",5) == 0) {
		c->query_output = QUERY_ASCII;
		return;
	} else if (strncmp(optarg,"html",4) == 0) {
		c->query_output = QUERY_HTML;
		return;
	} else {
		printf("ERROR: unkown output format!\n");
		exit(1);
	}
}

static void configuration_create_key( char *filename )
{
        int f;
	FILE *keyfile;
	char key[20];
        srand( (unsigned)time( NULL ) );
        for ( f = 0; f < 16; f++) {
                *(key+f) = (rand() % 128) +32;
        }
        *(key+16)='\0';
        printf("Key generated.\n");
        keyfile = fopen(filename, "w");
        if (keyfile == NULL) {
                printf("error creating the keyfile!\n");
                exit(1);
        }
        fprintf(keyfile, "%s", key);
        fclose(keyfile);
        printf("File '%s' has been created.\n", filename);

}

/*
 *  * Create a database connection and setup the required tables
 *   * returns 0 if fine, 1 on error
 *    */
static int configuration_database_connect( struct configuration_data *conf )
{
	int rc;
	const char *dberror;
	/**
	 * Initialize the DBI layer
	 * 	 	 
	 */
	if ( conf->dbdriver == NULL) {
		printf("ERROR: drivername == NULL. Exiting.\n");
		return 1;
		}
	rc = dbi_initialize(NULL);
	if ( rc == -1 ) {
		printf("DBI: ERROR dbi_initialize. Exiting.\n");
		printf("- is the given database driver installed?\n");
		return 1;
		}
	conf->DBIconn = dbi_conn_new(conf->dbdriver);
	if (conf->DBIconn == NULL) {
		printf("DBI: ERROR dbi_conn_new, with driver %s.\n",
			conf->dbdriver);
		printf("- sure you have a network connection to the database?\n");
		printf("- sure the given database driver is installed?\n");
		return 1;
		}
	/**
	 * if sqlite is used, we set the dbpath parameter
	 */
	if (strcmp(conf->dbdriver,"sqlite") == 0) {
		dbi_conn_set_option(conf->DBIconn, "sqlite_dbdir",
				conf->dbpath);
		dbi_conn_set_option_numeric(conf->DBIconn, "sqlite_timeout",
				10000);
	} else if (strcmp(conf->dbdriver,"sqlite3") == 0) {
		dbi_conn_set_option(conf->DBIconn, "sqlite3_dbdir",
				conf->dbpath);
		dbi_conn_set_option_numeric(conf->DBIconn, "sqlite3_timeout",
				10000);
	}

	dbi_conn_set_option(conf->DBIconn, "host", conf->dbhost);
	dbi_conn_set_option(conf->DBIconn, "username", conf->dbuser);
	dbi_conn_set_option(conf->DBIconn, "password", conf->dbpassword);
	dbi_conn_set_option(conf->DBIconn, "dbname", conf->dbname);
	dbi_conn_set_option(conf->DBIconn, "encoding", "UTF-8");
	if ( dbi_conn_connect(conf->DBIconn) < 0) {
		printf("DBI: could not connect, please check options.\n");
		dbi_conn_error(conf->DBIconn,&dberror);
		printf("DBI: %s\n",dberror);
		return 1;
		}
	return 0;
}


int configuration_parse_cmdline( struct configuration_data *c,
	int argc, char *argv[] )
{
	/**
	 * SMBTA_CONVERT is used as a flag to run the
	 * conversion function.
	 * SMBTA_DRY_RUN is used as a flag to run the
	 * -t --test-db function
	 */
	int SMBTA_CONVERT = 0;
	int SMBTA_DRY_RUN = 0;
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
			{ "debug-level",1, NULL, 'd' },
			{ "config-file",1,NULL,'c'},
			{ "keyfile",1,NULL,'k'},
			{ "query",1,NULL,'q'},
			{ "host",1,NULL,'h'},
			{ "help",0,NULL,'?'},
			{ "command-help",0,NULL,'p'},
			{ "file",1,NULL,'f'},
			{ "xml",1,NULL,'x' },
			{ "output",1,NULL,'o'},
			{ "create-key",1,NULL,'K'},
			{ "identify",1,NULL,'I'},
			{ "dbdriver",1,NULL,'M'},
			{ "dbname",1,NULL,'N'},
			{ "dbuser",1,NULL,'S'},
			{ "dbhost",1,NULL,'H'},
			{ "dbpassword",1,NULL,'P'},
			{ "convert",0,NULL,'C'},
			{ "version",0,NULL,'v'},
			{ "test-db",0,NULL,'t'},
			{ 0,0,0,0 }
		};

		i = getopt_long( argc, argv,
			"vCM:N:S:H:P:o:d:f:c:k:q:h:x:p?K:I:t", long_options, &option_index );

		if ( i == -1 ) break;

		switch (i) {
			case 'v':
				printf("%s\n",SMBTAQUERY_VERSION);
				exit(0);
			case 'C':
				/**
				 * set SMBTA_CONVERT to 1, run all other cmd line options,
				 * and run the smbta_convert() function later.
				 */
				SMBTA_CONVERT=1;
				break;
			case 'M':
				c->dbdriver = strdup( optarg );
				break;
			case 'N':
				c->dbname = strdup( optarg );
				break;
			case 'S':
				c->dbuser = strdup( optarg );
				break;
			case 'H':
				c->dbhost = strdup( optarg );
				break;
			case 'P':
				c->dbpassword = strdup( optarg );
				break;
			case 'd':
				c->debug_level = (int) common_myatoi( optarg );
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
			case 'x':
				c->query_xmlfile = strdup(optarg);
				break;
			case 'o':
				configuration_set_output(c,optarg);
				break;
			case 'K':
				configuration_create_key( optarg );
				exit(0);
			case 'I': ;
				c->identify = (int) common_myatoi( optarg );
				break;
			case 't': ;
				  /**
				   * dry run: only test if the database
				   * connection can be established and
				   * return the result on the terminal.
				   */
				SMBTA_DRY_RUN = 1;
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

	/**
	 * if convert (-C --convert) was called, run it now
	 */
	if (SMBTA_CONVERT==1) {
		smbta_convert(c);
		exit(0);
	}

	/* create a local sqlite database as a helper */
	c->db=create_db();


	/* Build up the connection to the database */
	if (configuration_database_connect(c) == 1) {
		printf("\nError on database connect.\n");
		talloc_free(runtime_mem);
		exit(1);
	}
	if (SMBTA_DRY_RUN == 1) {
		printf("\nDatabase connection succesful.\n");
		talloc_free(runtime_mem);
		exit(1);
	}

	/* through all options, now run the query command */

	/* open a xml output file for writing if needed */
	/* we finally use ascii XML output by default */
	pid_t pid;
	pid = getpid();
	char *tempff= NULL;
	tempff = talloc_asprintf(runtime_mem,"/tmp/tmp_xml-%d.xml",pid);
	if (c->query_xmlfile == NULL)
		c->query_xmlfile=tempff;
	interpreter_open_xml_file(c);
	if (c->query != NULL) {
		interpreter_run( runtime_mem, c->query, c);
		free(c->query);
	}
	if (c->file != NULL) {
		interpreter_run_from_file( runtime_mem, c->file, c);
		free(c->file);
	}
	interpreter_close_xml_file(c);
	close(c->socket);
	/* use xsltproc to transform the temporary xml file */
	char *mode= NULL;
	char *call = NULL;
	char *data_path = NULL;
	char *path = getenv("SMBTATOOLS_DATA_PATH");
	struct stat sb;
	char *xsltpath = getenv("SMBTATOOLS_XSLTPROC_PATH");
	if (xsltpath == NULL) xsltpath=talloc_asprintf(runtime_mem,"/usr/bin");
	if (path == NULL) 
		data_path = talloc_asprintf(runtime_mem,"/usr/share/smbtatools");
	else
		data_path = talloc_asprintf(runtime_mem,"%s",path);
	/** 
	 * check for xslt data files and /usr/share /usr/local/share,
	 * or fallback to SMBTATOOLS_DATA_PATH
	 */
	char *fcheck = talloc_asprintf(runtime_mem,"%s/xslt-ascii.xml",
			data_path);
	if ( stat(fcheck, &sb) == -1) {
		/**
		 * not found in /usr/share/smbtatools and not in
		 * SMBTATOOLS_DATA_PATH, checking /usr/local/share
		 */
		talloc_free(fcheck);
		fcheck = talloc_asprintf(runtime_mem,"/usr/local/share/smbtatools/xslt-ascii.xml");
		if (stat(fcheck, &sb) == -1) {
			printf("Checking %s\n",fcheck);
			printf("Warning: xslt stylesheets not found. Try setting\n");
			printf("setenv SMBTATOOLS_DATA_PATH \"$PATH/$TO/$STYLESHEETS.\"\n");
		} else {
			talloc_free(data_path);
			data_path = talloc_asprintf(runtime_mem,"/usr/local/share/smbtatools/");
		}
	}

	/* normal operation, just use the temporary xml file */	
	if (strncmp( c->query_xmlfile, tempff,strlen(tempff)) == 0) {
		/* check for xsltproc */
		char *check = NULL;
		check = talloc_asprintf(runtime_mem,"%s/xsltproc", xsltpath);
		if (stat(check, &sb) == -1) {
			printf("ERROR: xsltproc not found at '%s'.\n"
				"Please set SMBTATOOLS_XSLTPROC_PATH env\n"
				"variable to the path, or use the -x option!\n",
				xsltpath);
			TALLOC_FREE(runtime_mem);
			exit(1);
		}
       
		if (c->query_output==QUERY_HTML) {
			mode = talloc_asprintf(runtime_mem,
				"%s/xslt-html.xml",data_path);
		} else if (c->query_output==QUERY_ASCII) {
			mode = talloc_asprintf(runtime_mem,
				"%s/xslt-ascii.xml",data_path);
		}
		call = talloc_asprintf(runtime_mem,
			"%s/xsltproc %s %s", xsltpath, mode,tempff);
		int l = system(call);
		if (l == -1) {
			printf("ERROR: executing xsltproc.\n");
			exit(1);
		}
		l = remove(tempff);
	
	}
				
	TALLOC_FREE(runtime_mem);
	return 0;
}


int configuration_check_configuration( struct configuration_data *c )
{
	if ( c->debug_level <0 || c->debug_level>10 ) {
		printf("ERROR: debug level has to be between 0 and 10.\n");
		return -1;
	}
        if (	c->dbdriver==NULL || 
		( strcmp(c->dbdriver,"pgsql") != 0 &&
                strcmp(c->dbdriver,"mysql")!=0 &&
                strcmp(c->dbdriver,"sqlite3")!=0 )) {
	                printf("ERROR: please specifiy a correct database driver.\n");
	                printf("pgsql   -> postgresql\n");
	                printf("mysql   -> MySQL\n");
	                printf("sqlite3 -> sqlite3\n");
	                return -1;
        }
		
	return 0;
}
