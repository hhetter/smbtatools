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

struct interpreter_command {
	int argument_count;
	char *command;
	char **arguments;
	char *SQL_command;
}

void interpreter_run_command( TALLOC_CTX *ctx,
	int command,
	struct interpreter_command *command_data);
{
	switch(command)
	{

	case INT_OBJ_FILE:
		obj_struct->object = INT_OBJ_FILE;
		obj_struct->name = talloc_strdup(ctx,command_data->command);
		obj_struct->sql = talloc_asprintf(ctx," where file='%s'",
					command_data->command);
		break;
	case INT_OBJ_SHARE:
		obj_struct->object = INT_OBJ_SHARE;
		obj_struct->name = talloc_strdup(ctx,command_data->command);
		obj_struct->sql = talloc_asprintf(ctx," where share='%s'",
					command_data->command);
		break;
	case INT_OBJ_USER:
		obj_struct->object = INT_OBJ_USER;
		obj_struct->name = talloc_strdup(ctx,command_data->command);
		obj_struct->sql = talloc_asprintf(ctx," where user='%s'",
					command_data->command);
		break;
	case INT_OBJ_TOTAL:
		if (command_data->argument_count != 1) {
			printf("ERROR: Command 'total' requires one argument.\n");
			exit(1);
		}
		if (strcmp(command_data->arguments[0],"rw") == NULL) {
			
			
		

int interpreter_run( const char *commands )
{
	struct interpreter_command command_data;
	char *go_through =commands+1; /* don't interpret the first ' */
	char *cmd = NULL;
	char **params = NULL;
	int parsestate==0;

        if (commands == NULL) {
                printf("ERROR: No commands to interpret.\n");
                exit(1);
        }

	while(parsestate != -1) {
		parsestate = interpreter_step(&go_through, &command_data);
		interpreter_run_command(parsestate, &command_data);
	}
	return 0;
}


