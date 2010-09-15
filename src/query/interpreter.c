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

#include "include/network.h"
#include <talloc.h>

struct interpreter_command {
	int argument_count;
	char *command;
	int command_id;
	char *arguments[100];
	char *SQL_command;
};

struct interpreter_object {
	int object;
	char *name;
	char *sql;
	char *output_term;
	char *from;
	char *to;

};


char *interpreter_prepare_statement(TALLOC_CTX *ctx,
		char *data)
{
	int t = strlen(data);
	char *output=talloc_array(ctx,char,t+2);
	int x = 0;int y = 0;
	int flagfirst=0;
	while (x <= strlen(data)) {
		if (data[x]==',' && flagfirst==0) { output[y]=' '; y++;
						flagfirst=1; } 
		output[y] = data[x];
                if ( data[x]==',' && data[x+1]==' ') x++;
		if ( data[x]==' ' && data[x+1]==' ') x++;
		y++;x++;
	}
	return output;
}




void interpreter_print_table( TALLOC_CTX *ctx,
                int columns,char *data, ...);

void interpreter_print_numbered_table( TALLOC_CTX *ctx,
                int columns,char *data, ... );



void interpreter_print_table( TALLOC_CTX *ctx,
		int columns,char *data, ...)
{
	int col=1;
	int element=0;
	char *res = " ";
	char *arg = NULL;
	va_list ap;
	int count = columns;
	va_start( ap, NULL);
	while (count --) {
		arg = va_arg( ap, char *);
		printf("%-30s\t",arg);
	}
	va_end( ap );
	printf("\n");
	printf(
	"------------------------------------------------------------------------------\n");

	while (res != NULL) {
		res = result_get_element(ctx,element,data);
		if (res != NULL) printf("%-30s\t",res);
		if ( col==columns ) { col = 0; printf("\n"); }
		col++; element++;
	}
}

void bar(unsigned long int total, unsigned long int length) {
	long double percent= total / 100;
	float barpercent= 0.5;
	int x;
	long double erg = (long double) length / percent;
	int barlength = erg *barpercent;
	printf("%10.2Lf%% ", erg);
	for (x=0;x<barlength;x++) { printf("#"); }
	printf("\n");
}

void interpreter_fn_usage( TALLOC_CTX *ctx,
		struct interpreter_command *command_data,
		struct interpreter_object *obj_struct,
		struct configuration_data *config)
{
	char *query;
	char *query2;
	char *qtotal;
	char *qdat;
	int hour;
       	unsigned long int total,bytes;

	if (strcmp(command_data->arguments[0],"r")==0) {
                        query2 = talloc_asprintf(ctx,
				"select sum(length) from read where %s;",
                                obj_struct->sql);
                        qtotal = sql_query(ctx,config,query2);
                        total = atol(result_get_element(ctx,0,qtotal));
	} else if (strcmp(command_data->arguments[0],"w")==0) {
                        query2 = talloc_asprintf(ctx,
                                "select sum(length) from write where %s;",
                                obj_struct->sql);
                        qtotal = sql_query(ctx,config,query2);
                        total = atol(result_get_element(ctx,0,qtotal));
	} else if (strcmp(command_data->arguments[0],"rw")==0) {
			query2 = talloc_asprintf(ctx,
                                "select sum(length) from read where %s;",
                                obj_struct->sql);
                        qtotal = sql_query(ctx,config,query2);
                        total = atol(result_get_element(ctx,0,qtotal));
                        query2 = talloc_asprintf(ctx,
                                "select sum(length) from write where %s;",
                                obj_struct->sql);
                        qtotal = sql_query(ctx,config,query2);
                        total = total + atol(result_get_element(ctx,0,qtotal));
	}	
	for (hour = 0;hour<24;hour++) {
		if (strcmp(command_data->arguments[0],"r")==0) {
			query = talloc_asprintf(ctx,
				"select sum(length) from read where %s"
				" and time(timestamp) > time('%02i:00') and"
				" time(timestamp) < time('%02i:00');",
				obj_struct->sql,hour,hour+1);
			qdat = sql_query(ctx,config,query);
			bytes = atol(result_get_element(ctx,0,qdat));
		} else if (strcmp(command_data->arguments[0],"w")==0) {
                        query = talloc_asprintf(ctx,
                                "select sum(length) from write where %s"
                                " and time(timestamp) > time('%02i:00') and"
                                " time(timestamp) < time('%02i:00');",
                                obj_struct->sql,hour,hour+1);
                        qdat = sql_query(ctx,config,query);
                        bytes = atol(result_get_element(ctx,0,qdat));
		} else if (strcmp(command_data->arguments[0],"rw")==0) {
                        query = talloc_asprintf(ctx,
                                "select sum(length) from read where %s"
                                " and time(timestamp) > time('%02i:00') and"
                                " time(timestamp) < time('%02i:00');",
                                obj_struct->sql,hour,hour+1);
                        qdat = sql_query(ctx,config,query);
                        bytes = atol(result_get_element(ctx,0,qdat));
                        query = talloc_asprintf(ctx,
                                "select sum(length) from write where %s"
                                " and time(timestamp) > time('%02i:00') and"
                                " time(timestamp) < time('%02i:00');",
                                obj_struct->sql,hour,hour+1);
                        qdat = sql_query(ctx,config,query);
                        bytes = bytes + atol(result_get_element(ctx,0,qdat));

		} else {
			printf("ERROR: usage expects r,w, or rw.\n");
			exit(1);
		}
		printf("%02i:00 - %02i:00 : %-10s ",hour,hour+1,common_make_human_readable(ctx,bytes));
		bar(total,bytes);
	}
	printf("total: %s\n", common_make_human_readable(ctx,total));
}


void interpreter_fn_last_activity( TALLOC_CTX *ctx,
                struct interpreter_command *command_data,
                struct interpreter_object *obj_struct,
                struct configuration_data *config)
{
        char *query1;
        char *qdat = NULL;
	char *helper = NULL;
	char **result;
        char *Err;
        int row1,col;

	/* delete any content from the last_activity_data table */
	sqlite3_exec(config->db,"delete from last_activity_data;",NULL,0,NULL);
        if (command_data->argument_count != 1) {
		printf("ERROR: the last_activity function expects 1 arguments.\n");
		exit(1);
	}
	int limit = atoi(command_data->arguments[0]);
	if (limit == 0 ) {
		printf("ERROR: last_activity command syntax error."
			"Limit must be > 0.\n");
		exit(1);
	}

	/* VFS : read */
        query1 = talloc_asprintf(ctx,
		"select username,timestamp,filename,length from read "
		"where %s order by timestamp desc "
		"limit %i;",
		obj_struct->sql,limit);
	qdat = sql_query(ctx,config,query1);
	helper = result_get_element(ctx,0,qdat);
	int row = 0;
	while( helper != NULL ) {
		char *tmp = talloc_asprintf(ctx,
		"INSERT INTO last_activity_data ( timestamp, message) VALUES"
		" ( '%s', '%s: User %s read %s bytes from file %s.');",
		result_get_element(ctx,row+1,qdat),
		result_get_element(ctx,row+1,qdat),
		helper,
		result_get_element(ctx,row+3,qdat),
		result_get_element(ctx,row+2,qdat));
		sqlite3_exec(config->db,tmp,NULL,0,NULL);
		row=row+4;
		helper=result_get_element(ctx,row,qdat);
	}

	/* VFS: write */
        query1 = talloc_asprintf(ctx,
	        "select username,timestamp,filename,length from write "
                "where %s order by timestamp desc "
                "limit %i;",
                obj_struct->sql,limit);
        qdat = sql_query(ctx,config,query1);
        helper = result_get_element(ctx,0,qdat);
        row = 0;
        while( helper != NULL ) {
                char *tmp = talloc_asprintf(ctx,
                "INSERT INTO last_activity_data ( timestamp, message) VALUES"
                " ( '%s', '%s: User %s wrote %s bytes to file %s.');",
                result_get_element(ctx,row+1,qdat),
                result_get_element(ctx,row+1,qdat),
                helper,
                result_get_element(ctx,row+3,qdat),
                result_get_element(ctx,row+2,qdat));
                sqlite3_exec(config->db,tmp,NULL,0,NULL);
                row=row+4;
                helper=result_get_element(ctx,row,qdat);
        }

	/* VFS: open */
        query1 = talloc_asprintf(ctx,
                "select username,timestamp,filename from open "
                "where %s order by timestamp desc "
                "limit %i;",
                obj_struct->sql,limit);
        qdat = sql_query(ctx,config,query1);
        helper = result_get_element(ctx,0,qdat);
        row = 0;
        while( helper != NULL ) {
                char *tmp = talloc_asprintf(ctx,
                "INSERT INTO last_activity_data ( timestamp, message) VALUES"
                " ( '%s', '%s: User %s opened file %s.');",
                result_get_element(ctx,row+1,qdat),
                result_get_element(ctx,row+1,qdat),
                helper,
                result_get_element(ctx,row+2,qdat));
                sqlite3_exec(config->db,tmp,NULL,0,NULL);
                row=row+3;
                helper=result_get_element(ctx,row,qdat);
        }

	/* VFS: close */
        query1 = talloc_asprintf(ctx,
                "select username,timestamp,filename from close "
                "where %s order by timestamp desc "
                "limit %i;",
                obj_struct->sql,limit);
        qdat = sql_query(ctx,config,query1);
        helper = result_get_element(ctx,0,qdat);
        row = 0;
        while( helper != NULL ) {
                char *tmp = talloc_asprintf(ctx,
                "INSERT INTO last_activity_data ( timestamp, message) VALUES"
                " ( '%s', '%s: User %s closed file %s.');",
                result_get_element(ctx,row+1,qdat),
                result_get_element(ctx,row+1,qdat),
                helper,
                result_get_element(ctx,row+2,qdat));
                sqlite3_exec(config->db,tmp,NULL,0,NULL);
                row=row+3;
                helper=result_get_element(ctx,row,qdat);
        }

        /* VFS: rename */
        query1 = talloc_asprintf(ctx,
                "select username,timestamp,source, destination from rename "
                "where %s order by timestamp desc "
                "limit %i;",
                obj_struct->sql,limit);
        qdat = sql_query(ctx,config,query1);
        helper = result_get_element(ctx,0,qdat);
        row = 0;
        while( helper != NULL ) {
                char *tmp = talloc_asprintf(ctx,
                "INSERT INTO last_activity_data ( timestamp, message) VALUES"
                " ( '%s', '%s: User %s renamed file %s to %s.');",
                result_get_element(ctx,row+1,qdat),
                result_get_element(ctx,row+1,qdat),
                helper,
                result_get_element(ctx,row+2,qdat),
                result_get_element(ctx,row+3,qdat));
                sqlite3_exec(config->db,tmp,NULL,0,NULL);
                row=row+4;
                helper=result_get_element(ctx,row,qdat);
        }
	
	/* VFS: mkdir */
        query1 = talloc_asprintf(ctx,
                "select username,timestamp,path from mkdir "
                "where %s order by timestamp desc "
                "limit %i;",
                obj_struct->sql,limit);
        qdat = sql_query(ctx,config,query1);
        helper = result_get_element(ctx,0,qdat);
        row = 0;
        while( helper != NULL ) {
                char *tmp = talloc_asprintf(ctx,
                "INSERT INTO last_activity_data ( timestamp, message) VALUES"
                " ( '%s', '%s: User %s created directory %s.');",
                result_get_element(ctx,row+1,qdat),
                result_get_element(ctx,row+1,qdat),
                helper,
                result_get_element(ctx,row+2,qdat));
                sqlite3_exec(config->db,tmp,NULL,0,NULL);
                row=row+3;
                helper=result_get_element(ctx,row,qdat);
        }

	/* VFS: rmdir */
        query1 = talloc_asprintf(ctx,
                "select username,timestamp,path from rmdir "
                "where %s order by timestamp desc "
                "limit %i;",
                obj_struct->sql,limit);
        qdat = sql_query(ctx,config,query1);
        helper = result_get_element(ctx,0,qdat);
        row = 0;
        while( helper != NULL ) {
                char *tmp = talloc_asprintf(ctx,
                "INSERT INTO last_activity_data ( timestamp, message) VALUES"
                " ( '%s', '%s: User %s removed directory %s.');",
                result_get_element(ctx,row+1,qdat),
                result_get_element(ctx,row+1,qdat),
                helper,
                result_get_element(ctx,row+2,qdat));
                sqlite3_exec(config->db,tmp,NULL,0,NULL);
                row=row+3;
                helper=result_get_element(ctx,row,qdat);
        }

        /* VFS: chdir */
        query1 = talloc_asprintf(ctx,
                "select username,timestamp,path from chdir "
                "where %s order by timestamp desc "
                "limit %i;",
                obj_struct->sql,limit);
        qdat = sql_query(ctx,config,query1);
        helper = result_get_element(ctx,0,qdat);
        row = 0;
        while( helper != NULL ) {
                char *tmp = talloc_asprintf(ctx,
                "INSERT INTO last_activity_data ( timestamp, message) VALUES"
                " ( '%s', '%s: User %s changed to directory %s.');",
                result_get_element(ctx,row+1,qdat),
                result_get_element(ctx,row+1,qdat),
                helper,
                result_get_element(ctx,row+2,qdat));
                sqlite3_exec(config->db,tmp,NULL,0,NULL);
                row=row+3;
                helper=result_get_element(ctx,row,qdat);
        }

	char *tmp = talloc_asprintf(ctx,
		"select message from last_activity_data order by timestamp desc limit %i;",limit);
	sqlite3_get_table(config->db,tmp,&result,&row1,&col,&Err);
	int i=0;
	for (i = row1;i>0;i--) {
		printf("%s\n",result[i]);
	}
	sqlite3_free_table(result);

}

void interpreter_fn_top_list( TALLOC_CTX *ctx,
		struct interpreter_command *command_data,
		struct interpreter_object *obj_struct,
		struct configuration_data *config)
{
	char *query1;
	char *qdat = NULL;
	char *qdat2 = NULL;
	if (command_data->argument_count != 3) {
		printf("ERROR: the top function expects 3 arguments.\n");
		exit(1);
	}
	int limit = atoi(command_data->arguments[0]);
	if (limit == 0 ) {
		printf("ERROR: top command syntax error.\n");
		exit(1);
	}
	unsigned long int length[limit + 1];
	if (strcmp(command_data->arguments[1],"users")==0) {
		if (strcmp(command_data->arguments[2],"r")==0) {
			query1 = talloc_asprintf(ctx,
				"select distinct username from read"
				" where %s order by SUM(length) desc"
				" limit %i;",
				obj_struct->sql,limit);
			qdat = sql_query(ctx,config,query1);
		} else if (strcmp(command_data->arguments[2],"w")==0) {
			query1 = talloc_asprintf(ctx,
				"select distinct username from write "
				"where %s order by sum(length) desc "
				"limit %i;",
				obj_struct->sql,limit);
			qdat = sql_query(ctx,config,query1);
		} else if (strcmp(command_data->arguments[2],"rw")==0) {
			query1 = talloc_asprintf(ctx,
				"select distinct username from "
				"( select * from read UNION select * "
				"from write) where %s order by "
				"SUM(length) desc limit %i;",
				obj_struct->sql,limit);
			qdat = sql_query(ctx,config,query1);
		}
	} else if (strcmp(command_data->arguments[1],"shares")==0) {
		if (strcmp(command_data->arguments[2],"r")==0) {
			query1 = talloc_asprintf(ctx,
				"select distinct share from read where"
				" %s order by sum(length) desc limit %i;",
				obj_struct->sql,limit);
			qdat = sql_query(ctx,config,query1);
		} else if (strcmp(command_data->arguments[2],"w")==0) {
			query1 = talloc_asprintf(ctx,
				"select distinct share from write where"
				" %s order by sum(length) desc limit %i;",
				obj_struct->sql,limit);
			qdat = sql_query(ctx,config,query1);
		} else if (strcmp(command_data->arguments[2],"rw")==0) {
			query1 = talloc_asprintf(ctx,
				"select distinct share from ( select * from"
				" read UNION select * from write) where %s"
				" order by SUM(length) desc limit %i;",
				obj_struct->sql,limit);
			qdat = sql_query(ctx,config,query1);
		}
	} else if (strcmp(command_data->arguments[1],"files")==0) {
		if (strcmp(command_data->arguments[2],"r")==0) {
			query1 = talloc_asprintf(ctx,
				"select distinct filename from read where"
				" %s order by sum(length) desc limit %i;",
				obj_struct->sql,limit);
			qdat = sql_query(ctx,config,query1);
		} else if (strcmp(command_data->arguments[2],"w")==0) {
			query1 = talloc_asprintf(ctx,
				"select distinct filename from write where"
				" %s order by sum(length) desc limit %i;",
				obj_struct->sql,limit);
			qdat = sql_query(ctx,config,query1);
		} else if (strcmp(command_data->arguments[2],"rw")==0) {
			query1 = talloc_asprintf(ctx,
				"select distinct filename from ( select *"
				" from read UNION select * from write) where"
				" %s order by sum(length) desc limit %i;",
				obj_struct->sql,limit);
			qdat = sql_query(ctx,config,query1);
		}
	} else {
		printf("ERROR: top function syntax error.\n");
		exit(1);
	}
	int i = 0;
	char *el = "0";
	while (el != NULL) {
		el = result_get_element(ctx,i,qdat);
		if (el == NULL) break;
		if (strcmp(command_data->arguments[1],"users")==0) {
			if (strcmp(command_data->arguments[2],"r")==0) {
				query1 = talloc_asprintf(ctx,
					"select sum(length) from read where"
					" username=\"%s\" and %s;",
					el,
					obj_struct->sql);
				qdat2 = sql_query(ctx,config,query1);
				length[i]=atol(result_get_element(ctx,0,qdat2));
			} else if (strcmp(command_data->arguments[2],"w")==0) {
				query1 = talloc_asprintf(ctx,
					"select sum(length) from write where "
					"username=\"%s\" and %s;",
					el,
					obj_struct->sql);
				qdat2 = sql_query(ctx,config,query1);
				length[i]=atol(result_get_element(ctx,0,qdat2));
			} else if (strcmp(command_data->arguments[2],"rw")==0) {
				query1 = talloc_asprintf(ctx,
					"select sum(length) from ( select * from "
					"read UNION select * from write) where "
					"username=\"%s\" and %s;",
					el,
					obj_struct->sql);
				qdat2 = sql_query(ctx,config,query1);
				length[i]=atol(result_get_element(ctx,0,qdat2));
			}
		} else if (strcmp(command_data->arguments[1],"shares")==0) {
			if (strcmp(command_data->arguments[2],"r")==0) {
				query1 = talloc_asprintf(ctx,
					"select sum(length) from read"
					" where share=\"%s\" and %s;",
					el,
					obj_struct->sql);
				qdat2 = sql_query(ctx,config,query1);
				length[i]=atol(result_get_element(ctx,0,qdat2));
			} else if (strcmp(command_data->arguments[2],"w")==0) {
				query1 = talloc_asprintf(ctx,
					"select sum(length) from write where"
					" share=\"%s\" and %s;",
					el,
					obj_struct->sql);
				qdat2 = sql_query(ctx,config,query1);
				length[i]=atol(result_get_element(ctx,0,qdat2));
			} else if (strcmp(command_data->arguments[2],"rw")==0) {
				query1 = talloc_asprintf(ctx,
					"select sum(length) from (select * from "
					"read UNION select * from write) where "
					"share=\"%s\" and %s;",
					el,
					obj_struct->sql);
				qdat2 = sql_query(ctx,config,query1);
				length[i]=atol(result_get_element(ctx,0,qdat2));
			}
		} else if (strcmp(command_data->arguments[1],"files")==0) {
			if (strcmp(command_data->arguments[2],"r")==0) {
				query1 = talloc_asprintf(ctx,
					"select sum(length) from read where"
					" filename=\"%s\" and %s;",
					el,
					obj_struct->sql);
				qdat2 = sql_query(ctx,config,query1);
				length[i]=atol(result_get_element(ctx,0,qdat2));
			} else if (strcmp(command_data->arguments[2],"w")==0) {
				query1 = talloc_asprintf(ctx,
					"select sum(length) from write where"
					" filename=\"%s\" and %s;",
					el,
					obj_struct->sql);
				qdat2 = sql_query(ctx,config,query1);
				length[i]=atol(result_get_element(ctx,0,qdat2));
			} else if (strcmp(command_data->arguments[2],"rw")==0) {
				query1 = talloc_asprintf(ctx,
					"select sum(length) from (select * from"
					" read UNION select * from write) where "
					"filename=\"%s\" and %s;",
					el,
					obj_struct->sql);
				qdat2 = sql_query(ctx,config,query1);
				length[i]=atol(result_get_element(ctx,0,qdat2));
			}
		}
		i++;
	}

	i = 0;
	el = "0";
	printf("%-30s%-30s\n","Name","Size");
        printf(
        "------------------------------------------------------------------------------\n");

	while (el != NULL) {
		el = result_get_element(ctx,i,qdat);
		if (el == NULL) break;
		printf("%-30s%-30s\n",el,common_make_human_readable(ctx,length[i]));
		i++;
	}
}
	


void interpreter_fn_list( TALLOC_CTX *ctx,
		struct interpreter_command *command_data,
		struct interpreter_object *obj_struct,
		struct configuration_data *config)
{
	char *query1;
	char *qdat = NULL;

	if (command_data->argument_count != 1) {
		printf("ERROR: function list expects one argument.\n");
		exit(1);
	}
	if (strcmp(command_data->arguments[0],"users") == 0) {
		query1 = talloc_asprintf(ctx,
			"select username,usersid from read where"
			" %s union select username,usersid"
			" from write where %s;",
			obj_struct->sql,obj_struct->sql);
		qdat = sql_query(ctx, config, query1);
		interpreter_print_table( ctx, 2, qdat, "Name","SID");
	} else if (strcmp(command_data->arguments[0],"shares") == 0) {
		query1 = talloc_asprintf(ctx,
			"select share,domain from read where %s "
			"union select share,domain from write where %s;",
			obj_struct->sql,obj_struct->sql);
		qdat = sql_query(ctx, config, query1);
		interpreter_print_table( ctx, 2, qdat, "Name","Domain");
	} else if (strcmp(command_data->arguments[0],"files") == 0) {
		query1 = talloc_asprintf(ctx,
			"select filename from read where %s union"
			" select filename from write where %s;",
			obj_struct->sql,obj_struct->sql);
		qdat = sql_query(ctx,config,query1);
		interpreter_print_table( ctx, 1, qdat,"Name");
	} else {
		printf("ERROR: 	Arguments for the 'list' command"
			" can only be:\n");
		printf("	users, shares, or files.\n");
		exit(1);
	}
}

void interpreter_fn_total( TALLOC_CTX *ctx,
		struct interpreter_command *command_data,
		struct interpreter_object *obj_struct,
		struct configuration_data *config)
{
	char *query1, *query2 = NULL;
	char *qdat;
	unsigned long int sum;
	if (command_data->argument_count != 1) {
		printf("ERROR: function total expects one argument.\n");
		exit(1);
	}

	if (strcmp(command_data->arguments[0],"rw") == 0) {
		query1 = talloc_asprintf(ctx,
			"select SUM(length) from read where %s;",
			obj_struct->sql);
		query2 = talloc_asprintf(ctx,
			"select SUM(length) from write where %s;",
			obj_struct->sql);

		qdat = sql_query(ctx, config,query1);
		sum = atol( result_get_element(ctx,0,qdat));
		qdat = sql_query(ctx, config, query2);
		sum = sum + atol( result_get_element(ctx,0,qdat));
		printf(
"Total number of bytes transfered %s :		%s\n",
			obj_struct->output_term,
			common_make_human_readable(ctx,sum));
		
	} else if (strcmp(command_data->arguments[0],"r") == 0) {
		query1 = talloc_asprintf(ctx,
			"select SUM(length) from read where %s;",
			obj_struct->sql);
		qdat = sql_query(ctx, config,query1);
		
		sum = atol( result_get_element(ctx,0,qdat));
		printf(
"Total number of bytes read %s :		%s\n",
			obj_struct->output_term,
			common_make_human_readable(ctx,sum));
	} else if (strcmp(command_data->arguments[0],"w") == 0) {
		query1 = talloc_asprintf(ctx,
			"select SUM(length) from write where %s;",
			obj_struct->sql);
		qdat = sql_query(ctx, config,query1);
		sum = atol( result_get_element(ctx,0,qdat));
		printf(
"Total number of bytes written %s :		%s\n",
			obj_struct->output_term,
			common_make_human_readable(ctx,sum));
	} else {
		printf("ERROR: parameter to the 'total' command can only be:\n");
		printf("rw, r, or w.\n");
		exit(1);
	}
}

char *interpreter_return_timestamp_now(TALLOC_CTX *ctx)
{
	struct tm *tmp;
	time_t now;
	char *outstr = talloc_array(ctx,char,200);
	now = time(NULL);
	tmp = localtime(&now);
	strftime(outstr,199,"%Y-%m-%d %T",tmp);
	return outstr;
}

char *interpreter_return_timestamp(TALLOC_CTX *ctx,
	char *timestr)
{
	char *ret;
	struct tm *tmp;
	struct tm tmp2;
	time_t now;
	char *outstr = talloc_array(ctx,char,200);
	char *tmpoutstr = talloc_array(ctx,char,200);
	/* "now" */
	if (strcmp(timestr,"now")==0) {
		ret = interpreter_return_timestamp_now(ctx);
		return ret;
	}
	/* "today" */
	/* current date + 00:00:00 */
	if (strcmp(timestr,"today")==0) {
		now = time(NULL);
		tmp = localtime(&now);
		strftime(outstr,199,"%Y-%m-%d 00:00:00",tmp);
		return outstr;
	}
	/* "yesterday" */
	/* current time - 24 hours */
	if (strcmp(timestr,"yesterday")==0) {
		now = time(NULL);
		now = now - (60*60*24);
		tmp = localtime(&now);
		strftime(outstr,199,"%Y-%m-%d 00:00:00",tmp);
		return outstr;
	}
	/* exact time string */
	/* first try the full format */
	if (strptime( timestr, "%Y-%m-%d %T",&tmp2) != NULL) {
		return timestr;
	/* only a date */
	} else if (strptime(timestr,"%Y-%m-%d",&tmp2) != NULL) {
		strftime(outstr,199,"%Y-%m-%d 00:00:00",tmp);
		return outstr;
	/* only a time, assume the todays date */
	} else if (strptime(timestr,"%T",&tmp2) != NULL ||
			strptime(timestr,"%R",&tmp2) != NULL) {
		now = time(NULL);
		tmp = localtime(&now);
		strftime(tmpoutstr,199,"%Y-%m-%d ",tmp);
		outstr= talloc_asprintf(ctx,"%s%s",tmpoutstr,timestr);
		return outstr;
	} 
	printf("ERROR: did not understood the times you have given.\n");
	exit(1);
}	

void interpreter_make_times( TALLOC_CTX *ctx,
	struct interpreter_object *obj_struct,
	struct interpreter_command *command_data)
{
	int arg_flag=0;
	if (command_data->argument_count < 2) {
		obj_struct->from = talloc_asprintf(ctx,"1=1");
		obj_struct->to = talloc_asprintf(ctx,"1=1");
		return;
	}
	switch(obj_struct->object) {
	case INT_OBJ_FILE:
	case INT_OBJ_SHARE:
	case INT_OBJ_USER:
		arg_flag=1;
		break;
	case INT_OBJ_GLOBAL:
		arg_flag=0;
		break;
	}
	


	if (strcmp(command_data->arguments[0+arg_flag],"from")==0) {
		obj_struct->from = talloc_asprintf(ctx, "timestamp > '%s'",
			interpreter_return_timestamp(
			ctx,
			command_data->arguments[1+arg_flag]));
		obj_struct->to = talloc_asprintf(ctx, "timestamp < '%s'",
			interpreter_return_timestamp(
			ctx,
			command_data->arguments[3+arg_flag]));
	} else
	if (strcmp(command_data->arguments[0+arg_flag],"since")==0) {
		obj_struct->from = talloc_asprintf(ctx, "timestamp > '%s'",
			interpreter_return_timestamp(
			ctx,
			command_data->arguments[1+arg_flag]));
		obj_struct->to = talloc_asprintf(ctx,"timestamp < '%s'",
			interpreter_return_timestamp_now(ctx));
	} else {
		obj_struct->from = talloc_asprintf(ctx,"1=1");
		obj_struct->to = talloc_asprintf(ctx,"1=1");
	}
}


void interpreter_run_command( TALLOC_CTX *ctx,
	struct interpreter_command *command_data,
	struct interpreter_object *obj_struct,
	struct configuration_data *config)
{
	if (command_data->command_id == -1) return;
	switch(command_data->command_id)
	{

	case INT_OBJ_FILE:
		obj_struct->object = INT_OBJ_FILE;
		obj_struct->name = talloc_strdup(ctx,command_data->arguments[0]);
		interpreter_make_times(ctx,obj_struct, command_data);
		obj_struct->sql = talloc_asprintf(ctx," filename='%s' and "
			"%s and %s %s",
					command_data->arguments[0],
					obj_struct->from,
					obj_struct->to,
					common_identify(ctx,INT_OBJ_FILE,
						obj_struct->name,config,0));
		obj_struct->output_term = talloc_asprintf(ctx,
			"on file %s", obj_struct->name);
		break;
	case INT_OBJ_SHARE:
		obj_struct->object = INT_OBJ_SHARE;
		obj_struct->name = talloc_strdup(ctx,command_data->arguments[0]);
		interpreter_make_times(ctx,obj_struct, command_data);
		obj_struct->sql = talloc_asprintf(ctx," share='%s' and %s and %s %s",
					command_data->arguments[0],
					obj_struct->from,
					obj_struct->to,
					common_identify(ctx,INT_OBJ_SHARE,
						obj_struct->name,config,0));
		obj_struct->output_term = talloc_asprintf(ctx,
			"on share %s", obj_struct->name);
		break;
	case INT_OBJ_USER:
		obj_struct->object = INT_OBJ_USER;
		obj_struct->name = talloc_strdup(ctx,command_data->arguments[0]);
		interpreter_make_times(ctx,obj_struct, command_data);
		obj_struct->sql = talloc_asprintf(ctx," username='%s' and %s and %s %s",
					command_data->arguments[0],
					obj_struct->from,
					obj_struct->to,
					common_identify(ctx,INT_OBJ_USER,
						obj_struct->name,config,0));
		obj_struct->output_term = talloc_asprintf(ctx,
			"by user %s", obj_struct->name);
		break;
	case INT_OBJ_GLOBAL:
		obj_struct->object = INT_OBJ_GLOBAL;
		obj_struct->name= talloc_strdup(ctx, "global");
		interpreter_make_times(ctx,obj_struct, command_data);
		obj_struct->sql = talloc_asprintf(ctx," 1=1 and %s and %s",
			obj_struct->from,
			obj_struct->to);
		obj_struct->output_term = talloc_asprintf(ctx, "globally");
		interpreter_make_times(ctx,obj_struct, command_data);
		break;
	case INT_OBJ_TOTAL:
		interpreter_fn_total(ctx, command_data, obj_struct,config);
		break;
	case INT_OBJ_LIST:
		interpreter_fn_list(ctx, command_data, obj_struct,config);
		break;
	case INT_OBJ_TOP:
		interpreter_fn_top_list(ctx, command_data, obj_struct,config);
		break;
        case INT_OBJ_LAST:
                interpreter_fn_last_activity(ctx, command_data, obj_struct,config);
                break;
	case INT_OBJ_USAGE:
		interpreter_fn_usage(ctx, command_data, obj_struct,config);
		break;
	}
}

int interpreter_translate_command(const char *cmd)
{
	/* commands */
	if (strcmp(cmd, "total") == 0) return INT_OBJ_TOTAL;
	if (strcmp(cmd, "list") == 0) return INT_OBJ_LIST;
	if (strcmp(cmd, "top") == 0) return INT_OBJ_TOP;
        if (strcmp(cmd, "last_activity")==0) return INT_OBJ_LAST;
	if (strcmp(cmd, "usage") == 0) return INT_OBJ_USAGE;
	/* objects */
	if (strcmp(cmd, "share") == 0) return INT_OBJ_SHARE;
	if (strcmp(cmd, "user") == 0) return INT_OBJ_USER;
	if (strcmp(cmd, "file") == 0) return INT_OBJ_FILE;
	if (strncmp(cmd, "global",6) == 0) return INT_OBJ_GLOBAL;
	return -1;
}


char *interpreter_step( TALLOC_CTX *ctx, char *go_through,
		struct interpreter_command *command_data,
		struct configuration_data *config)
{
	char *go = go_through;
	char *bn, *backup;
	int dif;
	char *en = strstr(go,",");
	if (strlen(go_through) == 0) return NULL;

	backup = en+1;
	dif = 1+(en - go);
	char *res = talloc_strndup( ctx, go, dif);
	command_data->argument_count=0;
	en = strstr(res, " ");
	command_data->command = talloc_strndup( ctx, res, en-res);
	en = en + 1;
	bn = en;
	while (en != NULL) {
		en = strstr(bn, " ");
		if (en == NULL) {
			en = strstr(bn,",");
			if (en == NULL) {
                        	command_data->command_id =
					interpreter_translate_command(command_data->command);
				return backup;
			}
		}
		dif = en-bn;
		command_data->arguments[command_data->argument_count] =
			talloc_strndup( ctx, bn, dif);
		command_data->argument_count =
			command_data->argument_count+1;
		en = en + 1;
		bn = en;
	}
	return backup;
}
		

int interpreter_run( TALLOC_CTX *ctx,
	char *commands,
	struct configuration_data *config )
{

	commands = interpreter_prepare_statement(ctx,commands);
	struct interpreter_command command_data;
	struct interpreter_object command_obj;
	char *go_through =commands; /* don't interpret the first ' */
	int end = strlen(commands);

	/*
	 * set command_obj to global, in case the user doesn't provide
	 * an object.
	 */
	command_obj.object = INT_OBJ_GLOBAL;
        command_obj.name = talloc_strdup(ctx, "global");
	command_obj.sql = talloc_strdup(ctx," 1=1 ");
	command_obj.output_term = talloc_asprintf(ctx, "globally");

	if ( *(commands + end-1) == ';') *(commands+end-1)=',';
		else {
			printf("ERROR: missing semicolon at end of line.\n");
			exit(1);
		}
        if (commands == NULL) {
                printf("ERROR: No commands to interpret.\n");
                exit(1);
        }

	/* only SELECT commands are allowed as raw sql commands */
	if (strncmp(commands,"select",strlen("select"))==0 ||
		strncmp(commands,"SELECT",strlen("select"))== 0) {
		*(commands+end-1)=';';
		char *erg =sql_query(ctx,config,commands);
		interpreter_print_table(ctx,1,erg,"SQL Result");
		exit(0);
	}


	

	while(go_through != NULL) {
		go_through = interpreter_step(ctx,
			go_through,
			&command_data,
			config);
		if (go_through == NULL) break;
		interpreter_run_command(ctx,
			&command_data,
			&command_obj,
			config);
	}
	return 0;
}

void interpreter_command_help()
{
	printf("Interpreter commands\n");
	printf("-----------------------------------------------------\n");
	printf("smbtaquery -q 'OBJECT,COMMAND;'\n");
	printf("-----------------------------------------------------\n");
	printf("OBJECT can be:\n");
	printf("global  			Run a query over the\n"
		"				whole dataset.\n");
	printf("share	SHARE   		Run a query over the\n "
		"				share specified by SHARE\n");
	printf("user 	USER 			Run a query over the \n"
		"				user USER.\n");
	printf("file	FILE			Run a query over the\n"
		"				file FILE.\n");
	printf("-----------------------------------------------------\n");
	printf("COMMAND can be:\n");
	printf("total 	[rw][r][w]		Get the total read(r),\n"
		"				write(w)\n");
	printf("				or read-write values of\n"
		"				the object.\n");
	printf("list 	[shares][users]\n");
	printf("     	[files]			Lists all shares, all\n"
		"				users, all files on an\n");
	printf("				object.\n");
	printf("top 	[num] [shares]\n");
	printf("    	[users] [rw][r][w]	List the top NUM shares,\n"
		"				users or files on the\n");
	printf("				object.\n");
        printf("last_activity [num]\n");
        printf("            			List the last NUM \n"
		"				activities from the \n");
        printf("                                specified object.\n");
	printf("usage	[r][w][rw]		Show usage statistics\n"
		"				on an object\n");

};
