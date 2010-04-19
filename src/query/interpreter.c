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

#include "include/interpreter.h"
#include <talloc.h>

struct interpreter_command {
	int argument_count;
	char *command;
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
	INT_OBJ_TOTAL };


char *sql_query(TALLOC_CTX *ctx, const char *query, int *rows)
{
	printf("SQL-QUERY: %s\n",query);
	char *erg = strdup("test01");
	return erg;
}

void interpreter_fn_total( TALLOC_CTX *ctx,
		struct interpreter_command *command_data,
		struct interpreter_object *obj_struct)
{
	char *query1, *query2 = NULL;
	char *qdat;
	int rows;
	unsigned int sum;
	if (command_data->argument_count != 1) {
		printf("ERROR: function total expects one argument.\n");
		exit(1);
	}

	if (strcmp(command_data->arguments[0],"rw") == 0) {
		query1 = talloc_asprintf(ctx,
			"select SUM(bytes) from read %s",
			obj_struct->sql);
		query2 = talloc_asprintf(ctx,
			"select SUM(bytes) from write %s",
			obj_struct->sql);

		qdat = sql_query(ctx, query1, &rows);
		if (rows != 1) {
			printf("ERROR: SQL query failure!\n");
			exit(1);
		}
		sum = atol(qdat);
		qdat = sql_query(ctx, query2, &rows);
		if (rows != 1) {
			printf("ERROR: SQL query failure!\n");
			exit(1);
		}
		sum = sum + atol(qdat);
		printf("Total number of bytes transfered %s : %u\n",
			obj_struct->output_term, sum);
		
	} else if (strcmp(command_data->arguments[0],"r") == 0) {
		query1 = talloc_asprintf(ctx,
			"select SUM(bytes) from read %s",
			obj_struct->sql);
		qdat = sql_query(ctx, query1, &rows);
		sum = atol(qdat);
		printf("Total number of bytes read %s : %u\n",
			obj_struct->sql, sum);
	} else if (strcmp(command_data->arguments[0],"w") == 0) {
		query1 = talloc_asprintf(ctx,
			"select SUM(bytes) from write %s",
			obj_struct->sql);
		qdat = sql_query(ctx, query1, &rows);
		sum = atol(qdat);
		printf("Total number of bytes written %s : %u\n",
			obj_struct->output_term, sum);
	} else {
		printf("ERROR: parameter to the 'total' command can only be:\n");
		printf("	rw, r, or w.\n");
		exit(1);
	}
}


void interpreter_run_command( TALLOC_CTX *ctx,
	int command,
	struct interpreter_command *command_data,
	struct interpreter_object *obj_struct)
{
	if (command == -1) return;

	switch(command)
	{

	case INT_OBJ_FILE:
		obj_struct->object = INT_OBJ_FILE;
		obj_struct->name = talloc_strdup(ctx,command_data->arguments[0]);
		obj_struct->sql = talloc_asprintf(ctx," where file='%s'",
					command_data->command);
		obj_struct->output_term = talloc_asprintf(ctx,
			"on file %s", obj_struct->name);
		break;
	case INT_OBJ_SHARE:
		obj_struct->object = INT_OBJ_SHARE;
		obj_struct->name = talloc_strdup(ctx,command_data->arguments[0]);
		obj_struct->sql = talloc_asprintf(ctx," where share='%s'",
					command_data->command);
		obj_struct->output_term = talloc_asprintf(ctx,
			"on share %s", obj_struct->name);
		break;
	case INT_OBJ_USER:
		obj_struct->object = INT_OBJ_USER;
		obj_struct->name = talloc_strdup(ctx,command_data->arguments[0]);
		obj_struct->sql = talloc_asprintf(ctx," where user='%s'",
					command_data->command);
		obj_struct->output_term = talloc_asprintf(ctx,
			"by user %s", obj_struct->name);
		break;
	case INT_OBJ_TOTAL:
		interpreter_fn_total(ctx, command_data, obj_struct);
		break;
	}
}

int interpreter_translate_command(const char *cmd)
{
	if (strcmp(cmd, "total") == 0) return INT_OBJ_TOTAL;
	if (strcmp(cmd, "share") == 0) return INT_OBJ_SHARE;
	if (strcmp(cmd, "user") == 0) return INT_OBJ_USER;
	if (strcmp(cmd, "file") == 0) return INT_OBJ_FILE;
	return -1;
}


int interpreter_step( TALLOC_CTX *ctx, char *go_through,
		struct interpreter_command *command_data)
{
	char *go = go_through;
	char *bn;
	int dif;
	char *en = strstr(go,",");
	if (en == NULL) {
		en = strstr(go,";");
		if (en == NULL) {
			printf("ERROR: Missing semikolon at end of statement.\n");
			exit(1);
		}
	}
	dif = 1+(en - go);
	char *res = talloc_strndup( ctx, go, dif);
	printf("RES: %s\n",res);
	command_data->argument_count=0;
	en = strstr(res, " ");
	if (en == NULL) {
		command_data->command = talloc_strndup( ctx, res, dif);
		printf("CMD1: >%s<\n",command_data->command);
		return interpreter_translate_command( command_data->command);
	}
	command_data->command = talloc_strndup( ctx, res, en-res);
	printf("CMD: >%s<\n",command_data->command);
	en = en + 1;
	bn = en;
	while (en != NULL) {
		en = strstr(bn, " ");
		if (en == NULL) {
			en = strstr(bn,",");
			if (en == NULL) return -1;
		}
		dif = en-bn;
		printf("DIF: %i\n",dif);
		command_data->arguments[command_data->argument_count] =
			talloc_strndup( ctx, bn, dif);
		command_data->argument_count = command_data->argument_count+1;
		printf("ARG: >%s<\n",command_data->arguments[command_data->argument_count-1]);
		en = en + 1;
		bn = en;
	}
	return interpreter_translate_command( command_data->command);
}
		

int interpreter_run( TALLOC_CTX *ctx,char *commands )
{
	struct interpreter_command command_data;
	struct interpreter_object command_obj;
	char *go_through =commands; /* don't interpret the first ' */
	int parsestate=0;

        if (commands == NULL) {
                printf("ERROR: No commands to interpret.\n");
                exit(1);
        }

	while(parsestate != -1) {
		parsestate = interpreter_step(ctx, go_through, &command_data);
		printf("parsestate %i\n",parsestate);
		interpreter_run_command(ctx, parsestate, &command_data, &command_obj);
	}
	return 0;
}


