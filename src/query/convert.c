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
#include "../webmon/rrddriver/include/vfs_smb_traffic_analyzer.h"
#include <dbi.h>

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

struct db_entry {
	char *data;
	int length;


	char *username;
	char *domain;
	char *share;
	char *timestamp;
	char *usersid;
	char *filename;
	char *mode;
	char *path;
	char *result;
	char *destination;
	char *mondata;
	unsigned long int len;
	char *vfs_id;
	char *source;
	int vfs_op;
};

static void enter_data( struct confdata *c)
{
	if (c->entered == 1) return;

	printf("\nFollowing information is required to access the target database:\n");
	printf("        - the hostname of the machine running the database\n");
	printf("        - the username to use to connect to the database\n");
	printf("        - the password for this user\n");
	printf("        - the name of the database on the remote system\n");
	printf("        - the type of database you use (mysql, pgsql, sqlite)\n");

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

static char *create_database_string(TALLOC_CTX *ctx,struct db_entry *entry)
{
	/*
	 * create a database string from the given metadata in a cache entry
	 */
	char *retstr=NULL;
	switch( entry->vfs_op ) {
        case vfs_id_rename: ;
                retstr = talloc_asprintf(ctx, "INSERT INTO %s ("
                        "username, usersid, share, domain, timestamp,"
                        "source, destination, result) VALUES ("
                        "'%s','%s','%s','%s','%s',"
                        "'%s','%s',%s);",
                        entry->vfs_id,entry->username,entry->usersid,entry->share,entry->domain,entry->timestamp,
                        entry->source,entry->destination,entry->result);
		break;
        case vfs_id_close: ;
                retstr = talloc_asprintf(ctx, "INSERT INTO %s ("
                        "username, usersid, share, domain, timestamp,"
                        "filename, result) VALUES ("
                        "'%s','%s','%s','%s','%s',"
                        "'%s',%s);",
                        entry->vfs_id,entry->username,entry->usersid,entry->share,entry->domain,entry->timestamp,
                        entry->filename,entry->result);
                break;
        case vfs_id_open: ;
                retstr = talloc_asprintf(ctx, "INSERT INTO %s ("
                        "username, usersid, share, domain, timestamp,"
                        "filename, mode, result) VALUES ("
                        "'%s','%s','%s','%s','%s',"
                        "'%s',%s,%s);",
                        entry->vfs_id,entry->username,entry->usersid,entry->share,entry->domain,entry->timestamp,
                        entry->filename,entry->mode,entry->result);
                break;
        case vfs_id_chdir: ;
                retstr = talloc_asprintf( ctx, "INSERT INTO %s ("
                        "username, usersid, share, domain, timestamp,"
                        "path, result) VALUES ("
                        "'%s','%s','%s','%s','%s',"
                        "'%s',%s);",
                        entry->vfs_id,entry->username,entry->usersid,entry->share,entry->domain,entry->timestamp,
                        entry->path,entry->result);
                break;
        case vfs_id_mkdir: ;
                retstr = talloc_asprintf(ctx, "INSERT INTO %s ("
                        "username, usersid, share, domain, timestamp,"
                        "path, mode, result) VALUES ("
                        "'%s','%s','%s','%s','%s',"
                        "'%s',%s,%s);",
                        entry->vfs_id,entry->username,entry->usersid,entry->share,entry->domain,entry->timestamp,
                        entry->path, entry->mode, entry->result);
                break;
        case vfs_id_write:
        case vfs_id_pwrite: ;
                if (entry->len == 0) {
                        retstr=NULL;
                        break;
                }
                retstr = talloc_asprintf(ctx, "INSERT INTO %s ("
                        "username, usersid, share, domain, timestamp,"
                        "filename, length) VALUES ("
                        "'%s','%s','%s','%s','%s',"
                        "'%s',%lu);",
                        entry->vfs_id,entry->username,entry->usersid,entry->share,entry->domain,entry->timestamp,
                        entry->filename,entry->len);
                break;	
        case vfs_id_read:
        case vfs_id_pread: ;
                if (entry->len == 0) {
                        retstr=NULL;
                        break;
                }
                retstr = talloc_asprintf(ctx, "INSERT INTO %s ("
                        "username, usersid, share, domain, timestamp,"
                        "filename, length) VALUES ("
                        "'%s','%s','%s','%s','%s',"
                        "'%s',%lu);",
                        entry->vfs_id,entry->username,entry->usersid,entry->share,entry->domain,entry->timestamp,
                        entry->filename,entry->len);
                break;
	default: ;
	}	
	return retstr;
}

static void convert_check_dbi_res( dbi_result rs)
{
	if (rs == NULL) {
		printf("DBI ERROR.\n");
		exit(1);
	}
}

static void _1_2_4_to_1_2_5(struct confdata *c)
{
	int ch=0;
	printf("\n");
	printf("-> Upgrading database from version 1.2.4 to 1.2.5.\n");
	printf("-------------------------------------------------------------\n");
	printf("\n");
	enter_data(c);
	ch = convert_database_connect(c);
	printf("Updating tables...\n");
	dbi_result rs = dbi_conn_query(c->DBIconn,
		"ALTER TABLE mkdir ALTER COLUMN result TYPE bigint;" );
	convert_check_dbi_res(rs);

	rs = dbi_conn_query(c->DBIconn,
		"ALTER TABLE chdir ALTER COLUMN result TYPE bigint;");
	convert_check_dbi_res(rs);

	rs = dbi_conn_query(c->DBIconn,
		"ALTER TABLE rmdir ALTER COLUMN result TYPE bigint;");
	convert_check_dbi_res(rs);

	rs = dbi_conn_query(c->DBIconn,
		"ALTER TABLE rename ALTER COLUMN result TYPE bigint;");
	convert_check_dbi_res(rs);

	printf("Database update to SMBTA version 1.2.5.\n");
	exit(0);
}

static void _1_2_3_to_1_2_4(struct confdata *c)
{
	TALLOC_CTX *ctx = NULL;
	struct db_entry *entry = (struct db_entry *) malloc(sizeof(struct db_entry));
	printf("\n");
	printf("-> Upgrading database from version 1.0 - 1.2.3 to version 1.2.4.\n");
	printf("----------------------------------------------------------------\n");
	printf("\n");
	printf("Requirements:\n");
	printf("You will need access to the sqlite3 database created by the\n");
	printf("former version.\n");
	enter_data(c);
	printf("Enter the filename of the sqlite3 database:\n");
	scanf("%200s",c->sqlite_filename);
	printf("\n");
	printf("Opening sqlite database... ");
	sqlite3 *db = NULL;
	int ch;
	ch = sqlite3_open_v2(c->sqlite_filename, &db, SQLITE_OPEN_READONLY,NULL);
	if (ch != SQLITE_OK ) {
		printf("error opening the sqlite database.\n");
		exit(1);
	}
	printf("Ok.\n");
	ch = convert_database_connect(c);
	if (ch == 1) {
		printf("\n ERROR.\n");
		exit(1);
	}

	char *tables[] = { "read", "write", "mkdir",  "close", "rename", "open", "chdir", NULL };

	/* go through every table, and copy the contents to the remote database */
	int cc = 0;
	while (tables[cc] != NULL) {
		printf("Processing table '%s'...\n", tables[cc]);
		#define CREATE_COMMONS "vfs_id integer,username varchar,usersid varchar,share varchar,domain varchar,timestamp timestamp,"
		/**
		 * Get all the common values out of sqlite3
		 */
		sqlite3_stmt **ppStmt = NULL;
		printf("Receiving all contents of table '%s'...\n",tables[cc]);
		char *query = talloc_asprintf(NULL,"select * from %s;",tables[cc]);
		sqlite3_prepare(  db, query, -1,ppStmt, NULL);
		talloc_free(query);
		printf("Processing content...\n");
		int result;
		do {
			
			result=sqlite3_step(*ppStmt);
			if (result==SQLITE_ROW){
				/**
				 * process common data
				 */
				entry->username = talloc_asprintf(ctx,"%s",sqlite3_column_text(*ppStmt, 1));
				entry->usersid = talloc_asprintf(ctx,"%s",sqlite3_column_text(*ppStmt,2));
				entry->share = talloc_asprintf(ctx,"%s",sqlite3_column_text(*ppStmt,3));
				entry->domain = talloc_asprintf(ctx,"%s",sqlite3_column_text(*ppStmt,4));
				entry->timestamp = talloc_asprintf(ctx,"%s",sqlite3_column_text(*ppStmt,5));
				/**
				 * process table specifics
				 */
				switch(cc) {
	
				case 0: ;
					entry->vfs_op = vfs_id_read;
					entry->filename = talloc_asprintf(ctx,"%s",sqlite3_column_text(*ppStmt,6));
					entry->len = sqlite3_column_int(*ppStmt, 7);
					break;
				case 1: ;
					entry->vfs_op = vfs_id_write;
					entry->filename = talloc_asprintf(ctx,"%s",sqlite3_column_text(*ppStmt,6));
					entry->len = sqlite3_column_int(*ppStmt,7);
					break;
				case 2: ;
					entry->vfs_op = vfs_id_mkdir;
					entry->path = talloc_asprintf(ctx,"%s",sqlite3_column_text(*ppStmt,6));
					entry->mode = talloc_asprintf(ctx,"%s",sqlite3_column_text(*ppStmt,7));
					entry->result = talloc_asprintf(ctx,"%s",sqlite3_column_text(*ppStmt,8));
					break;
				case 3: ;
					entry->vfs_op = vfs_id_close;
					entry->filename = talloc_asprintf(ctx,"%s",sqlite3_column_text(*ppStmt,6));
					entry->result = talloc_asprintf(ctx,"%s",sqlite3_column_text(*ppStmt,7));
					break;
				case 4: ;
					entry->vfs_op = vfs_id_rename;
					entry->source = talloc_asprintf(ctx,"%s",sqlite3_column_text(*ppStmt,6));
					entry->destination = talloc_asprintf(ctx,"%s",sqlite3_column_text(*ppStmt,7));
					entry->result = talloc_asprintf(ctx,"%s",sqlite3_column_text(*ppStmt,8));
					break;
				case 5: ;
					entry->vfs_op = vfs_id_chdir;
					entry->path = talloc_asprintf(ctx,"%s",sqlite3_column_text(*ppStmt,6));
					entry->result = talloc_asprintf(ctx,"%s",sqlite3_column_text(*ppStmt,7));
					break;
				case 6: ;
					entry->vfs_op = vfs_id_open;
					entry->filename = talloc_asprintf(ctx,"%s",sqlite3_column_text(*ppStmt,6));
					entry->mode = talloc_asprintf(ctx,"%s",sqlite3_column_text(*ppStmt,7));
					entry->result = talloc_asprintf(ctx,"%s",sqlite3_column_text(*ppStmt,8));
					break;
				case 7: ;
					entry->vfs_op = vfs_id_chdir;
					entry->path = talloc_asprintf(ctx,"%s",sqlite3_column_text(*ppStmt,6));
					entry->result = talloc_asprintf(ctx,"%s",sqlite3_column_text(*ppStmt,7));
					break;
				default:
					;
					printf("Error: Unkown VFS identifier.\n");
					exit(1);
				}
				/**
				 * create an insertion string and push through DBI
				 */
				char *insert = create_database_string(ctx, entry);
				if (insert == NULL) {
					printf("ERROR creating database string.\n");
					talloc_free(ctx);
					exit(1);
				}
				dbi_result rs = dbi_conn_query(c->DBIconn, insert);
				if (rs == NULL) {
					printf("DBI ERROR.\n");
					talloc_free(ctx);
					exit(1);
				}
				TALLOC_FREE(ctx);
				dbi_result_free(rs);
			}
		} while (result == SQLITE_ROW);
		sqlite3_finalize(*ppStmt);
		/* next table */
		cc++;
	}

	free(entry);
}


	





void smbta_convert()
{
	int chose = 0;
	struct confdata c;
	c.entered = 0;
	printf("\n");
	printf("Select the SMBTA version you are coming from.\n");
	printf("1) 1.0, 1.1, 1.2.0, 1.2.1, 1.2.2, 1.2.3:\n");
	printf("	Allows to convert a sqlite3 based database created\n");
	printf("	with the SMBTA 1.0 - 1.2.3 releases to current \n");
	printf("	database, that has been already setup with\n");
	printf("	'smbta -T'.\n");
	printf("2) 1.2.4:\n");
	printf("	Converts a database to the current format when you\n");
	printf("	are coming from a 1.2.4 version.\n");
	printf("3) Quit program\n");
	printf("\n");
	printf("Enter Number:\n");
	scanf("%d",&chose);
	switch (chose) {
		case 1:
			_1_2_3_to_1_2_4(&c);
			_1_2_4_to_1_2_5(&c);
			break;
		case 2:
			_1_2_4_to_1_2_5(&c);
			break;
		case 3:
			printf("Exiting.\n");
			exit(0);
			break;
		default:
			printf("Wrong input.\n");
			exit(1);
		}

        return ;
}
