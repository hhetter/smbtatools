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
	INT_OBJ_GLOBAL,
        INT_OBJ_LAST};


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


int interpreter_get_result_rows( char *data, int columns)
{

        int col=1;
	int row = 0;
        int element=0;
        char *res = " ";
	char *ctx = talloc(NULL, char);
        while (res != NULL) {
                res = result_get_element(ctx,element,data);
                // if (res != NULL) printf("OO:%-30s\t",res);
                if ( col==columns ) { col = 0; row ++;  }
                col++; element++;
        }
	TALLOC_FREE(ctx);
	return row;
}
	
/*	char *res = talloc( NULL, char);
	int element=0,row=1,col =1;
        while (res != NULL) {
                res = result_get_element(res,element,qdat);
		printf("%s\n",res);
                if ( col==columns ) { row++;col = 0;}
                col++; element++;
		TALLOC_FREE(res);
        }
	return row;
*/

/*
        while (res != NULL) {
                res = result_get_element(ctx,element,data);
                if (res != NULL) printf("%-30s\t",res);
                if ( col==columns ) { col = 0; printf("\n"); }
                col++; element++;
        }
*/


void interpreter_print_table( TALLOC_CTX *ctx,
                int columns,char *data, ...);


char *interpreter_identify( TALLOC_CTX *ctx,
	enum IntCommands Type,
	char *data,
	struct configuration_data *config)
{
	char *query;
	char *qdat;
	int cols = 0;
	printf("Identifying %s ... ",data);
	if (Type==INT_OBJ_USER) {
		/* identify users by SID */
		query = talloc_asprintf(ctx,
			"select distinct(usersid), username, "
			"domain from read where username = '%s' "
			"UNION select distinct(usersid), username,"
			"domain from write where username = '%s';",
			data,data);
		qdat = sql_query(
			ctx,
			config,
			query);
		cols = 3;
	} else if (Type==INT_OBJ_SHARE) {
		/* identify shares by domain */
		query = talloc_asprintf(ctx,
			"select distinct(domain), share from read "
			"where share = '%s' UNION select "
			"distinct(domain), share from write where "
			"share = '%s';", data,data);
		qdat = sql_query(
			ctx,
			config,
			query);
		cols = 2;
	} else if (Type==INT_OBJ_FILE) {
		/* identify files by share */
		query = talloc_asprintf(ctx,
			"select distinct(share), filename "
			"from read where filename = '%s' "
			"UNION select distinct(share), filename "
			"from write where filename = '%s';",data,data);
		qdat = sql_query(
			ctx,
			config,
			query);
		cols = 2;
	} else {
		printf("ERROR: Identification of an unkown object type!\n");
		exit(1);
	}
	/* if only one row has been returned, the query is unique
	 * or failed
	 */
	if (interpreter_get_result_rows(qdat,cols) == 1) {
		/* check if the query does query for an existing */
		/* object at all.				 */
		char *cmp = NULL;
		cmp = result_get_element(ctx,0,qdat);
		if (strcmp(cmp,"No Results.") == 0) {
			switch(Type) {
			case INT_OBJ_USER:
				printf("User %s doesn't exist ", data);
				break;
			case INT_OBJ_SHARE:
				printf("Share %s doesn't exist ",data);
				break;
			case INT_OBJ_FILE:
				printf("File %s doesn't exist ",data);
				break;
			default:
				printf("ERROR: Unsupported type of object!\n");
				exit(1);
			}
			printf("in the database.\n");
			exit(1);
		}
				
		printf("Identified ");
		switch(Type) {
		case INT_OBJ_USER:
			printf("user %s ",data);
			break;
		case INT_OBJ_SHARE:
			printf("share %s ",data);
			break;
		case INT_OBJ_FILE:
			printf("file %s ",data);
			break;
		default:
			printf("ERROR: Unsupported type of object!\n");
			exit(1);
		}
		printf("as a unique item in the database.\n");
		return NULL;
	}
printf("UNABLE TO IDENTIFY !!!! RESULT: %i",interpreter_get_result_rows(qdat,cols));
exit(1);
return NULL;
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
void interpreter_fn_last_activity( TALLOC_CTX *ctx,
                struct interpreter_command *command_data,
                struct interpreter_object *obj_struct,
                struct configuration_data *config)
{
        char *query1;
        char *qdat = NULL;
        char *qdat2 = NULL;

        if (command_data->argument_count != 3) {
		printf("ERROR: the last_activity function expects 3 arguments.\n");
		exit(1);
	}
	int limit = atoi(command_data->arguments[0]);
	if (limit == 0 ) {
		printf("ERROR: last_activity command syntax error.\n");
		exit(1);
	}
        unsigned long int length[limit + 1];
        if (strcmp(command_data->arguments[1],"users")==0) {
            if (strcmp(command_data->arguments[2],"r")==0) {
                query1 = talloc_asprintf(ctx,
                            "select distinct username from read "
                            "where %s "
                            "limit %i;",
                        obj_struct->sql,limit);
                qdat = sql_query(ctx,config,query1);

            } else if (strcmp(command_data->arguments[2],"w")==0) {
                query1 = talloc_asprintf(ctx,
                            "select distinct username from write "
                            "where %s "
                            "limit %i",
                            obj_struct->sql,limit);
                qdat = sql_query(ctx,config,query1);
            } else if (strcmp(command_data->arguments[2],"rw")==0) {
                query1 = talloc_asprintf(ctx,
                            "select distinct username from"
                            "( select * from read UNION select * "
                            "from write) where %s "
                            "limit %i;",
                            obj_struct->sql,limit);
			qdat = sql_query(ctx,config,query1);
            }
        } else {
		printf("ERROR: top function syntax error.\n");
		exit(1);
	}
	int i = 0;
        char *el = "0";
        char *el2 = "0";
	while (el != NULL) {
		el = result_get_element(ctx,i,qdat);
		if (el == NULL) break;
		if (strcmp(command_data->arguments[1],"users")==0) {
			if (strcmp(command_data->arguments[2],"r")==0) {
				query1 = talloc_asprintf(ctx,
					"select timestamp from read where "
					"username=\"%s\" and %s "
                                        "order by timestamp desc "
                                        "limit %i;",
					el,
					obj_struct->sql, limit);
				qdat2 = sql_query(ctx,config,query1);
				length[i]=atol(result_get_element(ctx,0,qdat2));
			} else if (strcmp(command_data->arguments[2],"w")==0) {
				query1 = talloc_asprintf(ctx,
					"select timestamp from write where "
					"username=\"%s\" and %s "
                                        "order by timestamp desc "
                                        "limit %i;",
					el,
					obj_struct->sql, limit);
				qdat2 = sql_query(ctx,config,query1);
				length[i]=atol(result_get_element(ctx,0,qdat2));
			} else if (strcmp(command_data->arguments[2],"rw")==0) {
				query1 = talloc_asprintf(ctx,
					"select timestamp from ( select * from "
					"read UNION select * from write) where "
					"username=\"%s\" and %s "
                                        "order by timestamp desc "
                                        "limit %i;",
					el,
					obj_struct->sql, limit);
				qdat2 = sql_query(ctx,config,query1);
				length[i]=atol(result_get_element(ctx,0,qdat2));
			}
		}
        i = 0;
	el = "0";
        el2 = "0";
	printf("%-30s%-30s\n","Name","Date");
        printf(
        "------------------------------------------------------------------------------\n");
	while (el2 != NULL) {
		el = result_get_element(ctx,0,qdat);
                el2 = result_get_element(ctx,i,qdat2);
		if (el2 == NULL) break;
		printf("%-30s%-30s\n",el, el2);
		i++;
	}
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
		interpreter_identify(ctx, INT_OBJ_FILE, obj_struct->name,config);
		break;
	case INT_OBJ_SHARE:
		obj_struct->object = INT_OBJ_SHARE;
		obj_struct->name = talloc_strdup(ctx,command_data->arguments[0]);
		obj_struct->sql = talloc_asprintf(ctx," share='%s'",
					command_data->arguments[0]);
		obj_struct->output_term = talloc_asprintf(ctx,
			"on share %s", obj_struct->name);
		interpreter_identify(ctx, INT_OBJ_SHARE,obj_struct->name,config);
		break;
	case INT_OBJ_USER:
		obj_struct->object = INT_OBJ_USER;
		obj_struct->name = talloc_strdup(ctx,command_data->arguments[0]);
		obj_struct->sql = talloc_asprintf(ctx," username='%s'",
					command_data->arguments[0]);
		obj_struct->output_term = talloc_asprintf(ctx,
			"by user %s", obj_struct->name);
		interpreter_identify(ctx, INT_OBJ_USER,obj_struct->name,config);
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
        case INT_OBJ_LAST:
                interpreter_fn_last_activity(ctx, command_data, obj_struct,config);
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
	printf("OBJECT can be:\n");
	printf("global  			Run a query over the whole dataset.\n");
	printf("share	SHARE   		Run a query over the share specified by\n");
	printf("				SHARE.\n");
	printf("user 	USER 			Run a query over the user USER.\n");
	printf("file FILE			Run a query over the file FILE.\n");
	printf("COMMAND can be:\n");
	printf("total 	[rw][r][w]		Get the total read(r), write(w)\n");
	printf("				or read-write values of the\n");
	printf("				object.\n");
	printf("list 	[shares][users]\n");
	printf("     	[files]			Lists all shares, all users, all\n");
	printf("				files on an object.\n");
	printf("top 	[num] [shares]\n");
	printf("    	[users] [rw][r][w]	List the top NUM shares, users or\n");
	printf("				files on the object.\n");
        printf("last_activity [num]\n");
        printf("    [users] [rw][r][w]          List the last NUM activities\n");
        printf("                                from the user.\n");
};
