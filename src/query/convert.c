/*
 * smbtaquery
 * tool for querying the samba traffic analyzer
 *
 * Copyright (C) Benjamin Brunner, 2010
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

struct confdata {
	char dbiuser[50];
	char dbihost[50];
	char dbipassword[50];
	char dbidbname[50];
	char dbidriver[50];
	int entered;
	char sqlite_filename[255];
	dbi_conn DBIconn;
};

static void enter_data( struct confdata *c)
{
	if (c->entered == 1) return;

	printf("\nFollowing information is required to access the target database:\n");
	printf("        - the hostname of the machine running the database\n");
	printf("        - the username to use to connect to the database\n");
	printf("        - the password for this user\n");
	printf("        - the name of the database on the remote system\n");
	printf("        - the type of database you use (mysql, pgsql, sqlite\n");

	printf("\n");
	printf("Enter the hostname of the system running the database:\n");
	scanf("%50s",c->dbihost);
	printf("Enter the user to use to connect to the database:\n");
	scanf("%50s",c->dbiuser);
	printf("Enter the password for user %s:\n",c->dbiuser);
	scanf("%50s",c->dbipassword);
	printf("Enter the name of the remote database:\n");
	scanf("%50s",c->dbidbname);
	printf("Enter the database driver to use (sqlite, mysql, pgsql):\n");
	scanf("%50s",c->dbidriver);

	if (strcmp(c->dbidriver,"mysql") != 0 &&
		       strcmp(c->dbidriver,"pgsql") != 0 &&
		       strcmp(c->dbidriver,"sqlite") != 0) {
		printf("ERROR: enter sqlite, mysql or pgsql as database driver!\n");
		exit(1);
	}
	c->entered = 1;
}

/*
 *  *  * Create a database connection and setup the required tables
 *   *   * returns 0 if fine, 1 on error
 *    *    */
static int convert_database_connect( struct confdata *conf )
{
       int rc;
       const char *dberror;
       printf("Opening connection to the remote database via libDBI...\n");
        /**
	 *          * Initialize the DBI layer
	 *                   *               
	 *                            */
        if ( conf->dbidriver == NULL) {
                printf("ERROR: drivername == NULL. Exiting.\n");
                return 1;
        }
        rc = dbi_initialize(NULL);
        if ( rc == -1 ) {
                printf("DBI: ERROR dbi_initialize. Exiting.\n");
                return 1;
        }
        conf->DBIconn = dbi_conn_new(conf->dbidriver);
        if (conf->DBIconn == NULL) {
                printf("DBI: ERROR dbi_conn_new, with driver %s.\n",
                conf->dbidriver);
                exit(1);
        }
        dbi_conn_set_option(conf->DBIconn, "host", conf->dbihost);
        dbi_conn_set_option(conf->DBIconn, "username", conf->dbiuser);
        dbi_conn_set_option(conf->DBIconn, "password", conf->dbipassword);
        dbi_conn_set_option(conf->DBIconn, "dbname", conf->dbidbname);
        dbi_conn_set_option(conf->DBIconn, "encoding", "UTF-8");
        if ( dbi_conn_connect(conf->DBIconn) < 0) {
                printf("DBI: could not connect, please check options.\n");
                dbi_conn_error(conf->DBIconn,&dberror);
                printf("DBI: %s\n",dberror);
                return 1;
        }
	printf("Ok, DBI connection established.\n");
        return 0;
}


static void _1_2_3_to_1_2_4(struct confdata *c)
{
	printf("\n");
	printf("-> Upgrading database from version 1.0 - 1.2.3 to version 1.2.4.\n");
	printf("\n");
	printf("Requirements:\n");
	printf("You will need access to the sqlite3 database created by the\n");
	printf("former version.\n");
	enter_data(c);
	printf("Enter the filename of the sqlite3 database:\n");
	fgets(c->sqlite_filename,255,stdin);
	printf("\n");
	printf("Opening sqlite database... ");
	sqlite3 *db = NULL;
	int ch;
	ch = sqlite3_open(c->sqlite_filename, &db);
	if (ch != SQLITE_OK) {
		printf("error opening the sqlite database.\n");
		exit(1);
	}
	printf("Ok.\n");
	ch = convert_database_connect(c);
	if (ch == 1) {
		printf("\n ERROR.\n");
		exit(1);
	}

	char *tables[] = { "read", "write", "close", "rename", "open", "chdir", NULL };
	/* go through every table, and copy the contents to the remote database */
	int cc = 0;
	while (tables[cc] != NULL) {
		dbi_result res;
		printf("Processing table '%s'...\n", tables[cc]);
		cc++;
	}
}


	





void smbta_convert()
{
	int chose = 0;
	struct confdata c;
	c.entered = 0;
	printf("\n");
	printf("Select the SMBTA version you are coming from.\n");
	printf("1) 1.0 - 1.2.3\n");
	printf("	Allows to convert a sqlite3 based database created\n");
	printf("	with the SMBTA 1.0 - 1.2.3 releases to a new 1.2.4\n");
	printf("	based database, that has been already setup with\n");
	printf("	'smbta -T'.\n");
	printf("2) Quit program\n");
	printf("\n");
	printf("Enter Number:\n");
	scanf("%d",&chose);
	switch (chose) {
		case 1:
			_1_2_3_to_1_2_4(&c);
			break;
		case 2:
			printf("Exiting.\n");
			exit(0);
			break;
		default:
			printf("Wrong input.\n");
			exit(1);
		}

        return ;
}
