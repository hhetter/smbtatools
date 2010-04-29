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
#include "../../include/common.h"
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
};

enum IntCommands { 
	INT_OBJ_SHARE = 0,
	INT_OBJ_USER,
	INT_OBJ_FILE,
	INT_OBJ_TOTAL,
	INT_OBJ_LIST,
	INT_OBJ_TOP,
	INT_OBJ_GLOBAL };


char *interpreter_prepare_statement(TALLOC_CTX *ctx,
		char *data)
{
	int t = strlen(data);
	char *output=talloc_array(ctx,char,t+2);
	int x = 0;int y = 0;
	while (x <= strlen(data)) {
		output[y] = data[x];
                if ( data[x]==',' && data[x+1]==' ') x++;
		y++;x++;
	}
	return output;
}


void interpreter_print_table( TALLOC_CTX *ctx,
		int columns,char *data, ...)
{
	int col=1;
	int element=0;
	char *res = " ";
	char *arg = NULL;
	va_list ap;
	int count = columns;
	va_start( ap, count);
	while (count --) {
		arg = va_arg( ap,char * );
		printf("%s\t",arg);
	}
	va_end( ap );
	printf("\n");
	printf(
	"------------------------------------------------------------------------------\n");

	while (res != NULL) {
		res = result_get_element(ctx,element,data);
		if (res != NULL) printf("%s\t",res);
		if ( col==columns ) { col = 0; printf("\n"); }
		col++; element++;
	}
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
				"select distinct username from read where %s order by SUM(length) desc limit %i;",
				obj_struct->sql,limit);
			qdat = sql_query(ctx,config,query1);
		} else if (strcmp(command_data->arguments[2],"w")==0) {
			query1 = talloc_asprintf(ctx,
				"select distinct username from write where %s order by sum(length) desc limit %i;",
				obj_struct->sql,limit);
			qdat = sql_query(ctx,config,query1);
		}
	} else if (strcmp(command_data->arguments[1],"shares")==0) {
		if (strcmp(command_data->arguments[2],"r")==0) {
			query1 = talloc_asprintf(ctx,
				"select distinct share from read where %s order by sum(length) desc limit %i;",
				obj_struct->sql,limit);
			qdat = sql_query(ctx,config,query1);
		} else if (strcmp(command_data->arguments[2],"w")==0) {
			query1 = talloc_asprintf(ctx,
				"select distinct share from write where %s order by sum(length) desc limit %i;",
				obj_struct->sql,limit);
			qdat = sql_query(ctx,config,query1);
		}
	}
	int i = 0;
	char *el = "0";
	while (el != NULL) {
		el = result_get_element(ctx,i,qdat);
		if (el == NULL) break;
		if (strcmp(command_data->arguments[1],"users")==0) {
			if (strcmp(command_data->arguments[2],"r")==0) {
				query1 = talloc_asprintf(ctx,
					"select sum(length) from read where username=\"%s\" and %s;",el,obj_struct->sql);
				qdat2 = sql_query(ctx,config,query1);
				length[i]=atol(result_get_element(ctx,0,qdat2));
			} else if (strcmp(command_data->arguments[2],"w")==0) {
				query1 = talloc_asprintf(ctx,
					"select sum(length) from write where username=\"%s\" and %s;",el,obj_struct->sql);
				qdat2 = sql_query(ctx,config,query1);
				length[i]=atol(result_get_element(ctx,0,qdat2));
			}
		} else if (strcmp(command_data->arguments[1],"shares")==0) {
			if (strcmp(command_data->arguments[2],"r")==0) {
				query1 = talloc_asprintf(ctx,
					"select sum(length) from read where share=\"%s\" and %s;",el,obj_struct->sql);
				qdat2 = sql_query(ctx,config,query1);
				length[i]=atol(result_get_element(ctx,0,qdat2));
			} else if (strcmp(command_data->arguments[2],"w")==0) {
				query1 = talloc_asprintf(ctx,
					"select sum(length) from write where share=\"%s\" and %s;",el,obj_struct->sql);
				qdat2 = sql_query(ctx,config,query1);
				length[i]=atol(result_get_element(ctx,0,qdat2));
			}
		}
		i++;
	}

	i = 0;
	el = "0";
	while (el != NULL) {
		el = result_get_element(ctx,i,qdat);
		if (el == NULL) break;
		printf("%s\t%s\n",el,common_make_human_readable(ctx,length[i]));
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
			"select username,usersid from read union select username,usersid from write where %s;",
			obj_struct->sql);
		qdat = sql_query(ctx, config, query1);
		interpreter_print_table( ctx, 2, qdat, "Name","SID");
	} else if (strcmp(command_data->arguments[0],"shares") == 0) {
		query1 = talloc_asprintf(ctx,
			"select share from read union select share from write where %s;",
			obj_struct->sql);
		qdat = sql_query(ctx, config, query1);
		interpreter_print_table( ctx, 1, qdat, "Name");
	} else if (strcmp(command_data->arguments[0],"files") == 0) {
		query1 = talloc_asprintf(ctx,
			"select filename from read union select filename from write where %s;",
			obj_struct->sql);
		qdat = sql_query(ctx,config,query1);
		interpreter_print_table( ctx, 1, qdat,"Name");
	} else {
		printf("ERROR: 	Arguments for the 'list' command can only be:\n");
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
		/* if (rows != 1) {
			printf("ERROR: SQL query failure!\n");
			exit(1);
		}
		*/
		qdat = sql_query(ctx, config, query2);
		/*if (rows != 1) {
			printf("ERROR: SQL query failure!\n");
			exit(1);
		}*/
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
		obj_struct->sql = talloc_asprintf(ctx," filename='%s'",
					command_data->arguments[0]);
		obj_struct->output_term = talloc_asprintf(ctx,
			"on file %s", obj_struct->name);
		break;
	case INT_OBJ_SHARE:
		obj_struct->object = INT_OBJ_SHARE;
		obj_struct->name = talloc_strdup(ctx,command_data->arguments[0]);
		obj_struct->sql = talloc_asprintf(ctx," share='%s'",
					command_data->arguments[0]);
		obj_struct->output_term = talloc_asprintf(ctx,
			"on share %s", obj_struct->name);
		break;
	case INT_OBJ_USER:
		obj_struct->object = INT_OBJ_USER;
		obj_struct->name = talloc_strdup(ctx,command_data->arguments[0]);
		obj_struct->sql = talloc_asprintf(ctx," username='%s'",
					command_data->arguments[0]);
		obj_struct->output_term = talloc_asprintf(ctx,
			"by user %s", obj_struct->name);
		break;
	case INT_OBJ_GLOBAL:
		obj_struct->object = INT_OBJ_GLOBAL;
		obj_struct->name= talloc_strdup(ctx, "global");
		obj_struct->sql = talloc_strdup(ctx," 1=1 ");
		obj_struct->output_term = talloc_asprintf(ctx, "globally");
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

	}
}

int interpreter_translate_command(const char *cmd)
{
	/* commands */
	if (strcmp(cmd, "total") == 0) return INT_OBJ_TOTAL;
	if (strcmp(cmd, "list") == 0) return INT_OBJ_LIST;
	if (strcmp(cmd, "top") == 0) return INT_OBJ_TOP;
	/* objects */
	if (strcmp(cmd, "share") == 0) return INT_OBJ_SHARE;
	if (strcmp(cmd, "user") == 0) return INT_OBJ_USER;
	if (strcmp(cmd, "file") == 0) return INT_OBJ_FILE;
	if (strncmp(cmd, "global",5) == 0) return INT_OBJ_GLOBAL;
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
		printf("%s\n",erg);
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


