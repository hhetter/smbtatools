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
#include "../../include/version.h"
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


char *interpreter_return_timestamp_now(TALLOC_CTX *ctx);


/*
 * XML output routines
 */

struct xml_last_activity_data{
	/* human readable string */
	char *comment;
	char *timestamp;
	char *user;
	char *file;
	char *domain;
	/* extensions for some VFS functions */
	char *file2;
	char *value;
};

void interpreter_xml_begin_search(
	struct configuration_data *c,
	char *title)
{
	if (c->xml_handle == NULL) return;
	char *ctx = talloc(NULL, char);
	fprintf(c->xml_handle,
		"<search>\n"
		"	<title>%s</title>\n"
		"	<timestamp>%s</timestamp>\n",
		title,
		interpreter_return_timestamp_now(ctx));
	talloc_free(ctx);
}

void interpreter_xml_end_search(
	struct configuration_data *c)
{
	if (c->xml_handle == NULL) return;
	fprintf(c->xml_handle,
		"</search>\n");
}
	
void interpreter_xml_search_result_file(
		struct configuration_data *c,
		char *name,
		char *share,
		char *domain)
{
	if (c->xml_handle == NULL) return;
	fprintf(c->xml_handle,
		"<result>\n"
		"	<file>\n"
		"		<name>%s</name>\n"
		"		<share>%s</share>\n"
		"		<domain>%s</domain>\n"
		"	</file>\n"
		"</result>\n",
		name,
		share,
		domain);
}

void interpreter_xml_search_result_domain(
		struct configuration_data *c,
		char *name)
{
	if (c->xml_handle == NULL) return;
	fprintf(c->xml_handle,
		"<result>\n"
		"	<domain>\n"
		"		<name>%s</name>\n"
		"	</domain>\n"
		"</result>\n",
		name);
}

void interpreter_xml_search_result_sid(
		struct configuration_data *c,
		char *sid,
		char *user,
		char *domain)
{
	if (c->xml_handle == NULL) return;
	fprintf(c->xml_handle,
		"<result>\n"
		"	<sid>\n"
		"		<name>%s</name>\n"
		"		<user>%s</user>\n"
		"		<domain>%s</domain>\n"
		"	</sid>\n"
		"</result>\n",
		sid,user,domain);
}

void interpreter_xml_search_result_user(
		struct configuration_data *c,
		char *name,
		char *domain)
{
	if (c->xml_handle == NULL) return;
	fprintf(c->xml_handle,
		"<result>\n"
		"	<user>\n"
		"		<name>%s</name>\n"
		"		<domain>%s</domain>\n"
		"	</user>\n"
		"</result\n",
		name,
		domain);
}

void interpreter_xml_footer(
	struct configuration_data *c)
{
	char *ctx = talloc(NULL, char);
	if (c->xml_handle == NULL) return;
	fprintf(c->xml_handle,
		"<footer>\n"
		"	<application>SMB Traffic Analyzer</application>\n"
		"	<version>%s</version>\n"
		"	<timestamp>%s</timestamp>\n"
		"	<homepage>http://holger123.wordpress.com/smb-traffic-analyzer/</homepage>\n"
		"</footer>\n"
		"</smbta_output>",
		SMBTAQUERY_VERSION,
		interpreter_return_timestamp_now(ctx));
	TALLOC_FREE(ctx);
}

void interpreter_close_xml_file(
        struct configuration_data *c)
{
        if (c->xml_handle == NULL) return;
        interpreter_xml_footer(c);
        fclose(c->xml_handle);
}

void interpreter_xml_create_header(
	struct configuration_data *c)
{
	fprintf( c->xml_handle,
		"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n<smbta_output>\n\n");
}

void interpreter_open_xml_file(
	struct configuration_data *c)
{
        if (c->query_xmlfile != NULL) {
                c->xml_handle = fopen(c->query_xmlfile,"w");
                if (c->xml_handle == NULL) {
                        printf("ERROR: error opening xml file '%s'.\n",
                                c->query_xmlfile);
                        exit(1);
                }
		interpreter_xml_create_header(c);
        }
}

void interpreter_xml_begin_function(
	struct configuration_data *c,
	char *funcname)
{
	if (c->xml_handle == NULL) return;
	fprintf( c->xml_handle,
		"<%s>\n", funcname);
}

void interpreter_xml_close_function(
	struct configuration_data *c,
	char *funcname)
{
	if (c->xml_handle == NULL) return;
	fprintf( c->xml_handle,
		"</%s>\n", funcname);
}

void interpreter_xml_description(
	struct configuration_data *c,
	char *description_str)
{
	if (c->xml_handle == NULL) return;
	fprintf( c->xml_handle,
		"<description>\n	%s\n</description>\n",
		description_str);
}

void interpreter_xml_value(
	struct configuration_data *c,
	unsigned long int value)
{
	if (c->xml_handle == NULL) return;
	fprintf( c->xml_handle,
		"<value>%lu</value>\n",
		value);
}

void interpreter_xml_totalrow(
	struct configuration_data *c,
	char *value)
{
	if (c->xml_handle == NULL) return;
	fprintf( c->xml_handle,
		"<totalvalue>%s</totalvalue>\n",
		value);
}

void interpreter_xml_usageentry(
	struct configuration_data *c,
	char *timestr,
	unsigned long int value,
	char *conv_str,
	char *percent)
{
	if (c->xml_handle == NULL) return;
	fprintf( c->xml_handle,
		"<usagerow><time>%s</time>\n<value>%lu</value>\n<convval>%s</convval>\n<percent>%s</percent></usagerow>\n\n",
		timestr, value, conv_str, percent);
}

void interpreter_xml_last_activityentry(
	struct configuration_data *c,
	struct xml_last_activity_data *entry,
	char *vfs_function)
{
	if (c->xml_handle == NULL || entry->timestamp == NULL) return;
	fprintf( c->xml_handle,
		"\n<last_activityrow>\n"
			"<timestamp>%s</timestamp>\n"
			"<vfs_func>%s</vfs_func>\n"
			"<user>%s</user>\n"
			"<file>%s</file>\n"
			"<domain>%s</domain>\n"
			"<comment>%s</comment>\n"
			"<value>%s</value>\n"
		"</last_activityrow>\n",
		entry->timestamp,
		vfs_function,
		entry->user,
		entry->file,
		entry->domain,
		entry->comment,
		entry->value);
}

void interpreter_xml_toprow(
	struct configuration_data *c,
	int num,
	char *obj,
	char *val)
{
	if (c->xml_handle == NULL) return;
	fprintf( c->xml_handle,
		"\n<toprow><num>%i</num>\n<object>%s</object>\n<value>%s</value>\n</toprow>\n",
		num,
		obj,
		val);
}
	
void interpreter_xml_objname(
	struct configuration_data *c,
	char *str)
{
	if (c->xml_handle == NULL) return;
	fprintf( c->xml_handle,
		"<objname>%s</objname>\n",
		str);
}

void interpreter_xml_begin_table(
	struct configuration_data *c,
	int columns)
{
	if (c->xml_handle == NULL) return;
	fprintf( c->xml_handle,
		"<table_columns>%i</table_columns>\n",columns);
}

void interpreter_xml_begin_table_row(
	struct configuration_data *c)
{
	if (c->xml_handle == NULL) return;
	fprintf( c->xml_handle,
		"<table_row>");
}

void interpreter_xml_close_table_row(
	struct configuration_data *c)
{
	if (c->xml_handle == NULL) return;
	fprintf( c->xml_handle,
		"</table_row>");
}

void interpreter_xml_begin_table_header(
	struct configuration_data *c)
{
	if (c->xml_handle == NULL) return;
	fprintf( c->xml_handle,
		"<table_header>");
}

void interpreter_xml_table_header_element(
	struct configuration_data *c,
	char *element)
{
	if (c->xml_handle == NULL) return;
	fprintf( c->xml_handle,
		"<header_element>%s</header_element>",
		element);
}

void interpreter_xml_end_table_header(
	struct configuration_data *c)
{
	if (c->xml_handle == NULL) return;
	fprintf( c->xml_handle,
		"</table_header>");
}

void interpreter_xml_begin_table_content(
	struct configuration_data *c)
{
	if (c->xml_handle == NULL) return;
	fprintf( c->xml_handle,
		"<table_content>");
}

void interpreter_xml_table_content(
	struct configuration_data *c,
	char *res)
{
	if (c->xml_handle == NULL) return;
	fprintf( c->xml_handle,
		"<table_value>%s</table_value>",
		res);
}

void interpreter_xml_end_table_content(
	struct configuration_data *c)
{
	if (c->xml_handle == NULL) return;
	fprintf( c->xml_handle,
		"</table_content>");
}

void interpreter_xml_end_table(
	struct configuration_data *c)
{
	if (c->xml_handle == NULL) return;
	fprintf( c->xml_handle,
		"</table>\n");
}
	
void interpreter_xml_strvalue(
	struct configuration_data *c,
	char *str)
{
	if (c->xml_handle == NULL) return;
	fprintf( c->xml_handle,
		"<value>%s</value>\n",
		str);
}


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
		struct configuration_data *c,
                int columns,char *data, ...);




void interpreter_print_table( TALLOC_CTX *ctx,
		struct configuration_data *c,
		int columns,char *data, ...)
{
	int col=1;
	int element=0;
	char *res = " ";
	char *arg = NULL;
	va_list ap;
	int count = columns;
	va_start( ap, NULL);
	interpreter_xml_begin_table(c,columns);
//	interpreter_xml_begin_table_header(c);
	while (count --) {
		arg = va_arg( ap, char *);
		interpreter_xml_table_header_element(c,arg);
	}
	va_end( ap );
//	interpreter_xml_end_table_header(c);


	interpreter_xml_begin_table_row(c);
	while (res != NULL) {
               res = result_get_element(ctx,element,data);
		if (res == NULL) break;
		if ( col == 1 && named_row != NULL) {
		       interpreter_xml_table_named_row(c,named_row,res);
		}
		if (res != NULL && col != 1) interpreter_xml_table_content(c,res);
		if ( col==columns ) { col = 0; 
			interpreter_xml_close_table_row(c);
			interpreter_xml_begin_table_row(c);
		}
		col++; element++;
	}
	interpreter_xml_close_table_row(c);
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

char *percent(TALLOC_CTX *ctx,
	unsigned long int total,
	unsigned long int length)
{
	char *ret;
	long double percent = total / 100;
	long double erg = (long double) length / percent;
	ret = talloc_asprintf(ctx, "%10.2Lf", erg);
	return ret;
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
	char *xmlstr, *xmlstr2  = NULL;
	int hour;
       	unsigned long int total,bytes;

	interpreter_xml_begin_function(config, "usage");
	xmlstr = talloc_asprintf(ctx, "24 hours average usage %s",
		obj_struct->output_term);


	if (strcmp(command_data->arguments[0],"r")==0) {
			xmlstr2 = talloc_asprintf(ctx,"%s by read access.",
				xmlstr);
                        query2 = talloc_asprintf(ctx,
				"select sum(length) from read where %s;",
                                obj_struct->sql);
                        qtotal = sql_query(ctx,config,query2);
                        total = atol(result_get_element(ctx,0,qtotal));
	} else if (strcmp(command_data->arguments[0],"w")==0) {
			xmlstr2 = talloc_asprintf(ctx,"%s by write access.",
				xmlstr);
                        query2 = talloc_asprintf(ctx,
                                "select sum(length) from write where %s;",
                                obj_struct->sql);
                        qtotal = sql_query(ctx,config,query2);
                        total = atol(result_get_element(ctx,0,qtotal));
	} else if (strcmp(command_data->arguments[0],"rw")==0) {
			xmlstr2 = talloc_asprintf(ctx,"%s by read-write access.",
				xmlstr);
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
		interpreter_xml_description(config,xmlstr2);
		xmlstr = talloc_asprintf(ctx, "%02i:00 - %02i:00", hour, hour+1);
		interpreter_xml_usageentry(config, xmlstr, bytes,common_make_human_readable(ctx,bytes),
			 percent(ctx, total ,bytes) );
	}
	interpreter_xml_value(config, total);
	interpreter_xml_close_function(config,"usage");
}


void interpreter_fn_last_activity( TALLOC_CTX *ctx,
                struct interpreter_command *command_data,
                struct interpreter_object *obj_struct,
                struct configuration_data *config)
{
        char *query1;
        char *qdat = NULL;
	char *xmlstr = NULL;
	char *helper = NULL;
	char **result;
        char *Err;
        int row1,col;
	struct xml_last_activity_data xmldata;
	TALLOC_CTX *temp = talloc(ctx, char);
	/* delete any content from the last_activity_data table */
	sqlite3_exec(config->db,"delete from last_activity_data;",NULL,0,NULL);
        if (command_data->argument_count != 1) {
		printf("ERROR: the last_activity function expects 1 argument.\n");
		exit(1);
	}
	int limit = (int) common_myatoi(command_data->arguments[0]);
	if (limit == 0 ) {
		printf("ERROR: last_activity command syntax error."
			"Limit must be > 0.\n");
		exit(1);
	}
	interpreter_xml_begin_function(config, "last_activity");
	xmlstr = talloc_asprintf(ctx,"Last activity %s",obj_struct->output_term);
	interpreter_xml_description(config,xmlstr);
	TALLOC_FREE(xmlstr);

	/* VFS : read */
        query1 = talloc_asprintf(ctx,
		"select username,timestamp,filename,length,domain from read "
		"where %s order by timestamp desc "
		"limit %i;",
		obj_struct->sql,limit);
	qdat = sql_query(ctx,config,query1);
	helper = result_get_element(ctx,0,qdat);
	int row = 0;
	while( helper != NULL ) {
		char *tmp1 = talloc_asprintf(temp,
			"%s: User %s read %s bytes from file %s.",
	                result_get_element(temp,row+1,qdat),
        	        helper,
                	result_get_element(temp,row+3,qdat),
                	result_get_element(temp,row+2,qdat));
		char *tmp = talloc_asprintf(temp,
		"INSERT INTO last_activity_data ( timestamp, message, username, file, value, domain,vfs_func) VALUES"
		" ( '%s', '%s','%s','%s','%s','%s','VFS:READ');",
		result_get_element(temp,row+1,qdat),
		tmp1, helper,result_get_element(temp,row+2,qdat),
		result_get_element(temp,row+3,qdat),result_get_element(temp,row+4,qdat));;
		sqlite3_exec(config->db,tmp,NULL,0,NULL);
		row=row+5;
		helper=result_get_element(temp,row,qdat);
		
	}
	/* VFS: write */
        query1 = talloc_asprintf(ctx,
	        "select username,timestamp,filename,length,domain from write "
                "where %s order by timestamp desc "
                "limit %i;",
                obj_struct->sql,limit);
        qdat = sql_query(ctx,config,query1);
        helper = result_get_element(ctx,0,qdat);
        row = 0;
        while( helper != NULL ) {
                char *tmp1 = talloc_asprintf(temp,
		"%s: User %s wrote %s bytes to file %s.",
		result_get_element(temp,row+1,qdat),
		helper,
		result_get_element(temp,row+3,qdat),
		result_get_element(temp,row+2,qdat));
		char *tmp = talloc_asprintf(temp,
                "INSERT INTO last_activity_data ( timestamp, message,username,file,value,domain,vfs_func) VALUES"
                " ( '%s', '%s','%s','%s','%s','%s','VFS:WRITE');",
                result_get_element(temp,row+1,qdat),
                tmp1,
                helper,
                result_get_element(temp,row+2,qdat),
                result_get_element(temp,row+3,qdat),
		result_get_element(temp,row+4,qdat));
                sqlite3_exec(config->db,tmp,NULL,0,NULL);
                row=row+5;
                helper=result_get_element(temp,row,qdat);
        }

	/* VFS: open */
        query1 = talloc_asprintf(ctx,
                "select username,timestamp,filename,domain from open "
                "where %s order by timestamp desc "
                "limit %i;",
                obj_struct->sql,limit);
        qdat = sql_query(ctx,config,query1);
        helper = result_get_element(ctx,0,qdat);
        row = 0;
        while( helper != NULL ) {
		char *tmp1 = talloc_asprintf(temp,
		"%s: User %s openend file %s.",
		result_get_element(temp,row+1,qdat),
		helper,
		result_get_element(temp,row+2,qdat));

                char *tmp = talloc_asprintf(temp,
                "INSERT INTO last_activity_data ( timestamp, message,username,file,value,domain,vfs_func) VALUES"
                " ( '%s', '%s','%s','%s','%s','%s','VFS:OPEN');",
                result_get_element(temp,row+1,qdat),
                tmp1,
                helper,
                result_get_element(temp,row+2,qdat),
		"0",
		result_get_element(temp,row+3,qdat));
                sqlite3_exec(config->db,tmp,NULL,0,NULL);
                row=row+4;
                helper=result_get_element(temp,row,qdat);
        }

	/* VFS: close */
        query1 = talloc_asprintf(ctx,
                "select username,timestamp,filename,domain from close "
                "where %s order by timestamp desc "
                "limit %i;",
                obj_struct->sql,limit);
        qdat = sql_query(ctx,config,query1);
        helper = result_get_element(ctx,0,qdat);
        row = 0;
        while( helper != NULL ) {
		char *tmp1 = talloc_asprintf(temp,
		"%s: User %s closed file %s.",
		result_get_element(temp,row+1,qdat),
		helper,
		result_get_element(temp,row+2,qdat));

                char *tmp = talloc_asprintf(temp,
                "INSERT INTO last_activity_data ( timestamp, message,username,file,value,domain,vfs_func) VALUES"
                " ( '%s','%s','%s','%s'.'%s','%s','VFS:CLOSE');",
                result_get_element(temp,row+1,qdat),
                tmp1,
                helper,
                result_get_element(temp,row+2,qdat),
		"0",
		result_get_element(temp,row+3,qdat));
                sqlite3_exec(config->db,tmp,NULL,0,NULL);
                row=row+4;
                helper=result_get_element(temp,row,qdat);
        }

        /* VFS: rename */
        query1 = talloc_asprintf(ctx,
                "select username,timestamp,source, destination,domain from rename "
                "where %s order by timestamp desc "
                "limit %i;",
                obj_struct->sql,limit);
        qdat = sql_query(ctx,config,query1);
        helper = result_get_element(ctx,0,qdat);
        row = 0;
        while( helper != NULL ) {
		char *tmp1 = talloc_asprintf(temp,
		"%s: User %s renamed file %s to %s.",
		result_get_element(temp,row+1,qdat),
		helper,
		result_get_element(temp,row+2,qdat),
		result_get_element(temp,row+3,qdat));

                char *tmp = talloc_asprintf(temp,
                "INSERT INTO last_activity_data ( timestamp, message,username,file,value,domain,vfs_func) VALUES"
                " ( '%s','%s','%s','%s','%s','%s','VFS:RENAME');",
                result_get_element(temp,row+1,qdat),
		tmp1,
                helper,
                result_get_element(temp,row+2,qdat),
                result_get_element(temp,row+3,qdat),
		result_get_element(temp,row+4,qdat));
                sqlite3_exec(config->db,tmp,NULL,0,NULL);
                row=row+5;
                helper=result_get_element(temp,row,qdat);
        }
	
	/* VFS: mkdir */
        query1 = talloc_asprintf(ctx,
                "select username,timestamp,path,domain from mkdir "
                "where %s order by timestamp desc "
                "limit %i;",
                obj_struct->sql,limit);
        qdat = sql_query(ctx,config,query1);
        helper = result_get_element(ctx,0,qdat);
        row = 0;
        while( helper != NULL ) {
		char *tmp1 = talloc_asprintf(temp,
		"%s: User %s created directory %s.",
		result_get_element(temp,row+1,qdat),
		helper,
		result_get_element(temp,row+2,qdat));

                char *tmp = talloc_asprintf(temp,
                "INSERT INTO last_activity_data ( timestamp, message,username,file,value,domain,vfs_func) VALUES"
                " ( '%s', '%s','%s','%s','%s','%s','VFS:MKDIR');",
                result_get_element(temp,row+1,qdat),
                tmp1,
                helper,
                result_get_element(temp,row+2,qdat),
		result_get_element(temp,row+3,qdat),
		result_get_element(temp,row+4,qdat));
                sqlite3_exec(config->db,tmp,NULL,0,NULL);
                row=row+4;
                helper=result_get_element(temp,row,qdat);
        }

	/* VFS: rmdir */
        query1 = talloc_asprintf(ctx,
                "select username,timestamp,path,domain from rmdir "
                "where %s order by timestamp desc "
                "limit %i;",
                obj_struct->sql,limit);
        qdat = sql_query(ctx,config,query1);
        helper = result_get_element(ctx,0,qdat);
        row = 0;
        while( helper != NULL ) {
		char *tmp1 = talloc_asprintf(temp,
		"%s: User %s removed directory %s.",
		result_get_element(temp,row+1,qdat),
		helper,
		result_get_element(temp,row+2,qdat));

                char *tmp = talloc_asprintf(temp,
                "INSERT INTO last_activity_data ( timestamp, message,username,file,value,domain,vfs_func) VALUES"
                " ( '%s', '%s','%s','%s','%s','%s','VFS:RMDIR' );",
                result_get_element(temp,row+1,qdat),
                tmp1,
                helper,
                result_get_element(temp,row+2,qdat),
		result_get_element(temp,row+3,qdat),
		result_get_element(temp,row+4,qdat));
                sqlite3_exec(config->db,tmp,NULL,0,NULL);
                row=row+4;
                helper=result_get_element(temp,row,qdat);
        }

        /* VFS: chdir */
        query1 = talloc_asprintf(ctx,
                "select username,timestamp,path,domain from chdir "
                "where %s order by timestamp desc "
                "limit %i;",
                obj_struct->sql,limit);
        qdat = sql_query(ctx,config,query1);
        helper = result_get_element(ctx,0,qdat);
        row = 0;
        while( helper != NULL ) {
		char *tmp1 = talloc_asprintf(temp,
		"%s: User %s changed to directory %s.",
		result_get_element(temp,row+1,qdat),
		helper,
		result_get_element(temp,row+2,qdat));
                char *tmp = talloc_asprintf(temp,
                "INSERT INTO last_activity_data ( timestamp, message, username,file,value,domain,vfs_func) VALUES"
                " ( '%s', '%s','%s','%s','%s','%s','VFS:CHDIR' );",
                result_get_element(temp,row+1,qdat),
                tmp1,
                helper,
                result_get_element(temp,row+2,qdat),
		result_get_element(temp,row+3,qdat),
		result_get_element(temp,row+4,qdat));
                sqlite3_exec(config->db,tmp,NULL,0,NULL);
                row=row+4;
                helper=result_get_element(temp,row,qdat);
        }

	char *tmp = talloc_asprintf(ctx,
		"select timestamp, username, message, file, domain, value, vfs_func from last_activity_data order by timestamp desc limit %i;",limit);
	sqlite3_get_table(config->db,tmp,&result,&row1,&col,&Err);
	

	int i=0;
	for (i = row1*col;i>0;i = i - col) {

                xmldata.timestamp = result[i];
                xmldata.user = result[i+1];
                xmldata.file = result[i+3];
                xmldata.value = result[i+5];
                xmldata.domain = result[i+4];
                xmldata.comment = result[i+2];
		interpreter_xml_last_activityentry(config, &xmldata,result[i+6]);
	}
	sqlite3_free_table(result);
	interpreter_xml_close_function(config,"last_activity");
	TALLOC_FREE(temp);
}

void interpreter_fn_search( TALLOC_CTX *ctx,
		struct interpreter_command *command_data,
		struct interpreter_object *obj_struct,
		struct configuration_data *config)
{
	char *query = NULL;
	char *qdat = NULL;
	char *xmldata = NULL;
	const char *str = NULL;
	const char *str2 = NULL;
	if (command_data->argument_count != 1) {
		printf("ERROR: the search function expects one argument\n");
		exit(1);
	}
	xmldata = talloc_asprintf(ctx,"Fuzzy search for '%s' %s.",
		command_data->arguments[0],
		obj_struct->output_term);

	interpreter_xml_begin_search(config, xmldata);
	
	static const char *tables[] = { "write",  NULL };
	static const char *rows[] = { "filename", "username", "usersid", "domain",  NULL };
	static const char *rules[] = { "distinct filename, share, domain ",
					"distinct username, domain ",
					"distinct usersid, username, domain ",
					"distinct domain ", NULL };
	int i = 0,t = 0, n = 0;
	char *res = NULL;
	str = tables[0];
	str2 = rows[0];
	while (str != NULL) {
		while ( str2 != NULL ) {
			query = talloc_asprintf(ctx,
				"select %s from "
				"write where %s GLOB '%s' and %s;",
				 rules[t],
				 rows[t], command_data->arguments[0], obj_struct->sql);
			qdat = sql_query(ctx,config,query);
			if (strcmp(result_get_element(ctx,0,qdat),"No Results.") != 0) {
				switch (t) {
				case 0: ;
					n = 0;
					res = "\0";
					while (res != NULL) {
						res = result_get_element(ctx, n,qdat);
						if (res != NULL) {
							interpreter_xml_search_result_file(
								config,
								result_get_element(ctx,0+n,qdat), // name
								result_get_element(ctx,1+n,qdat), // share
								result_get_element(ctx,2+n,qdat)); // domain
						}
						n = n + 3;
					}
					break;
				case 1: ;
					n = 0;
					res = "\0";
					while (res != NULL) {
						res = result_get_element(ctx,n,qdat);
						if (res != NULL) {
							interpreter_xml_search_result_user(
								config,
								result_get_element(ctx,n+0,qdat), // name
								result_get_element(ctx,n+1,qdat)); // domain
						}
						n = n +2;
					}
					break;
				case 2: ;
					n = 0;
					res = "\0";
					while (res != NULL) {
						res = result_get_element(ctx,n,qdat);
						if (res != NULL) {
							interpreter_xml_search_result_sid(
								config,
								result_get_element(ctx,n+0,qdat), // sid
								result_get_element(ctx,n+1,qdat), // username
								result_get_element(ctx,n+2,qdat)); // domain
						}
						n = n + 3;
					}
					break;
				case 3: ;
					n = 0;
					res = "\0";
					while (res != NULL) {
						res = result_get_element(ctx,n,qdat);
						if (res != NULL) {
							interpreter_xml_search_result_domain(
								config,
								result_get_element(ctx,n,qdat)); // domain
						}
						n = n + 1;
					}
					break;
				default: break ;
				}
			}
			t++;
			str2 =  rows[t];
		}
	str2 = rows[0];
	t = 0;
	i++;
	str = tables[i];
	}
	interpreter_xml_end_search(config);
}

void interpreter_fn_top_list( TALLOC_CTX *ctx,
		struct interpreter_command *command_data,
		struct interpreter_object *obj_struct,
		struct configuration_data *config)
{
	char *query1;
	char *qdat = NULL;
	char *qdat2 = NULL;
	char *xmldata;
	if (command_data->argument_count != 3) {
		printf("ERROR: the top function expects 3 arguments.\n");
		exit(1);
	}
	int limit = (int) common_myatoi(command_data->arguments[0]);
	if (limit == 0 ) {
		printf("ERROR: top command syntax error.\n");
		exit(1);
	}
	unsigned long int length[limit + 1];
	if (strcmp(command_data->arguments[1],"users")==0) {
		if (strcmp(command_data->arguments[2],"r")==0) {
			xmldata=talloc_asprintf(ctx,"Top %i users %s by read access.",
				limit, obj_struct->output_term);
			query1 = talloc_asprintf(ctx,
				"select distinct username from read"
				" where %s order by SUM(length) desc"
				" limit %i;",
				obj_struct->sql,limit);
			qdat = sql_query(ctx,config,query1);
		} else if (strcmp(command_data->arguments[2],"w")==0) {
			xmldata=talloc_asprintf(ctx,"Top %i users %s by write access.",
				limit, obj_struct->output_term);
			query1 = talloc_asprintf(ctx,
				"select distinct username from write "
				"where %s order by sum(length) desc "
				"limit %i;",
				obj_struct->sql,limit);
			qdat = sql_query(ctx,config,query1);
		} else if (strcmp(command_data->arguments[2],"rw")==0) {
			xmldata=talloc_asprintf(ctx,"Top %i users %s by read-write access.",
				limit,obj_struct->output_term);
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
			xmldata=talloc_asprintf(ctx,"Top %i shares %s by read access.",
				limit, obj_struct->output_term);
			query1 = talloc_asprintf(ctx,
				"select distinct share from read where"
				" %s order by sum(length) desc limit %i;",
				obj_struct->sql,limit);
			qdat = sql_query(ctx,config,query1);
		} else if (strcmp(command_data->arguments[2],"w")==0) {
			xmldata=talloc_asprintf(ctx,"Top %i shares %s by write access.",
				limit,obj_struct->output_term);
			query1 = talloc_asprintf(ctx,
				"select distinct share from write where"
				" %s order by sum(length) desc limit %i;",
				obj_struct->sql,limit);
			qdat = sql_query(ctx,config,query1);
		} else if (strcmp(command_data->arguments[2],"rw")==0) {
			xmldata=talloc_asprintf(ctx,"Top %i shares %s by read-write access.",
				limit,obj_struct->output_term);
			query1 = talloc_asprintf(ctx,
				"select distinct share from ( select * from"
				" read UNION select * from write) where %s"
				" order by SUM(length) desc limit %i;",
				obj_struct->sql,limit);
			qdat = sql_query(ctx,config,query1);
		}
	} else if (strcmp(command_data->arguments[1],"files")==0) {
		if (strcmp(command_data->arguments[2],"r")==0) {
			xmldata=talloc_asprintf(ctx,"Top %i files %s by read access.",
				limit,obj_struct->output_term);
			query1 = talloc_asprintf(ctx,
				"select distinct filename from read where"
				" %s order by sum(length) desc limit %i;",
				obj_struct->sql,limit);
			qdat = sql_query(ctx,config,query1);
		} else if (strcmp(command_data->arguments[2],"w")==0) {
			xmldata=talloc_asprintf(ctx,"Top %i files %s by write access.",
				limit,obj_struct->output_term);
			query1 = talloc_asprintf(ctx,
				"select distinct filename from write where"
				" %s order by sum(length) desc limit %i;",
				obj_struct->sql,limit);
			qdat = sql_query(ctx,config,query1);
		} else if (strcmp(command_data->arguments[2],"rw")==0) {
			xmldata=talloc_asprintf(ctx,"Top %i files %s by read-write access.",
				limit,obj_struct->output_term);
			query1 = talloc_asprintf(ctx,
				"select distinct filename from ( select *"
				" from read UNION select * from write) where"
				" %s order by sum(length) desc limit %i;",
				obj_struct->sql,limit);
			qdat = sql_query(ctx,config,query1);
		}
	} else if (strcmp(command_data->arguments[1],"domains")==0) {
		if (strcmp(command_data->arguments[2],"r")==0) {
			xmldata=talloc_asprintf(ctx,"Top %i domains %s by read access.",
				limit,obj_struct->output_term);
			query1 = talloc_asprintf(ctx,
				"select distinct domain from read where"
				" %s order by sum(length) desc limit %i;",
				obj_struct->sql,limit);
			qdat = sql_query(ctx,config,query1);
		} else if (strcmp(command_data->arguments[2],"w")==0) {
			xmldata = talloc_asprintf(ctx,"Top %i domains %s by write access.",
				limit,obj_struct->output_term);
			query1= talloc_asprintf(ctx,
				"select distinct domain from write where"
				" %s order by sum(length) desc limit %i;",
				obj_struct->sql,limit);
			qdat = sql_query(ctx,config,query1);
		} else if (strcmp(command_data->arguments[2],"rw")==0) {
			xmldata = talloc_asprintf(ctx,"Top %i domains %s by read-write access.",
				limit,obj_struct->output_term);
			query1=talloc_asprintf(ctx,
				"select distinct domain from ( select *"
				"from read UNION selec t* from write ) where"
				" %s order by sum(length) desc limit %i;",
				obj_struct->sql,limit);
			qdat=sql_query(ctx,config,query1);
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
		} else if (strcmp(command_data->arguments[1],"domains")==0) {
			if (strcmp(command_data->arguments[2],"r")==0) {
				query1 = talloc_asprintf(ctx,
					"select sum(length) from read where"
					" domain=\"%s\" and %s;",
					el,
					obj_struct->sql);
				qdat2 = sql_query(ctx,config,query1);
				length[i]=atol(result_get_element(ctx,0,qdat2));
		} else if (strcmp(command_data->arguments[2],"w")==0) {
				query1 = talloc_asprintf(ctx,
					"select sum(length) from write where"
					" domain=\"%s\" and %s;",
					el,
					obj_struct->sql);
				qdat2 = sql_query(ctx,config,query1);
				length[i]=atol(result_get_element(ctx,0,qdat2));
		} else if (strcmp(command_data->arguments[2],"rw")==0) {
				query1 = talloc_asprintf(ctx,
					"select sum(length) from ( select * from "
					"read UNION select * from write) where "
					"domain=\"%s\" and %s;",
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

	interpreter_xml_begin_function(config, "top");
	interpreter_xml_description(config, xmldata);
	int num= 0;
	while (el != NULL) {
		num++;
		el = result_get_element(ctx,i,qdat);
		if (el == NULL) break;
		interpreter_xml_toprow(config,num, el,common_make_human_readable(ctx,length[i]));
		i++;
	}

	interpreter_xml_close_function(config, "top");
}
	


void interpreter_fn_list( TALLOC_CTX *ctx,
		struct interpreter_command *command_data,
		struct interpreter_object *obj_struct,
		struct configuration_data *config)
{
	char *query1;
	char *qdat = NULL;
	char *xmldata = NULL;

	if (command_data->argument_count != 1) {
		printf("ERROR: function list expects one argument.\n");
		exit(1);
	}
	if (strcmp(command_data->arguments[0],"users") == 0) {
		xmldata= talloc_asprintf(ctx,"List of users %s",
			obj_struct->output_term);
		query1 = talloc_asprintf(ctx,
			"select username,usersid from read where"
			" %s union select username,usersid"
			" from write where %s;",
			obj_struct->sql,obj_struct->sql);
		qdat = sql_query(ctx, config, query1);
		interpreter_xml_begin_function(config,"list");
		interpreter_xml_description(config,xmldata);
		interpreter_print_table( ctx, config, 2, qdat, "Name","SID");
		interpreter_xml_close_function(config,"list");
	} else if (strcmp(command_data->arguments[0],"domains") == 0) {
		xmldata = talloc_asprintf(ctx,"List of domains %s",
			obj_struct->output_term);
		query1 = talloc_asprintf(ctx,
			"select domain from read where"
			"%s union select domain"
			" from write where %s;",
			obj_struct->sql, obj_struct->sql);
		qdat=sql_query(ctx,config,query1);
		interpreter_xml_begin_function(config,"list");
		interpreter_xml_description(config,xmldata);
		interpreter_print_table( ctx, config, "domain",1,qdat,"Domain");
		interpreter_xml_close_function(config,"list");
	} else if (strcmp(command_data->arguments[0],"shares") == 0) {
		xmldata=talloc_asprintf(ctx,"List of shares %s",
			obj_struct->output_term);
		query1 = talloc_asprintf(ctx,
			"select share,domain from read where %s "
			"union select share,domain from write where %s;",
			obj_struct->sql,obj_struct->sql);
		qdat = sql_query(ctx, config, query1);
		interpreter_xml_begin_function(config,"list");
		interpreter_xml_description(config,xmldata);
		interpreter_print_table( ctx, config, 2, qdat, "Name","Domain");
		interpreter_xml_close_function(config,"list");
	} else if (strcmp(command_data->arguments[0],"files") == 0) {
		xmldata=talloc_asprintf(ctx,"List of files %s",
			obj_struct->output_term);
		query1 = talloc_asprintf(ctx,
			"select filename,share from read where %s union"
			" select filename,share from write where %s;",
			obj_struct->sql,obj_struct->sql);

		qdat = sql_query(ctx,config,query1);
		interpreter_xml_begin_function(config,"list");
		interpreter_xml_description(config,xmldata);
		interpreter_print_table( ctx, config, 2, qdat,"Name","Share");
		interpreter_xml_close_function(config,"list");
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
	char *query1, *query2, *xmldata = NULL;
	char *qdat;
	unsigned long int sum;
	if (command_data->argument_count != 1) {
		printf("ERROR: function total expects one argument.\n");
		exit(1);
	}

	interpreter_xml_begin_function(config,"total");

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
		xmldata = talloc_asprintf(ctx,
			"Total number of bytes transfered %s.",
			obj_struct->output_term);
		interpreter_xml_description(config,xmldata);
		interpreter_xml_totalrow(config,
			common_make_human_readable(ctx,sum));
		
	} else if (strcmp(command_data->arguments[0],"r") == 0) {
		query1 = talloc_asprintf(ctx,
			"select SUM(length) from read where %s;",
			obj_struct->sql);
		qdat = sql_query(ctx, config,query1);
		
		sum = atol( result_get_element(ctx,0,qdat));
		xmldata = talloc_asprintf(ctx,
			"Total number of bytes read %s.",
			obj_struct->output_term);
		interpreter_xml_description(config,xmldata);
		interpreter_xml_totalrow(config,
			common_make_human_readable(ctx,sum));

	} else if (strcmp(command_data->arguments[0],"w") == 0) {
		query1 = talloc_asprintf(ctx,
			"select SUM(length) from write where %s;",
			obj_struct->sql);
		qdat = sql_query(ctx, config,query1);
		sum = atol( result_get_element(ctx,0,qdat));
		xmldata = talloc_asprintf(ctx,
			"Total number of bytes written %s.",
			obj_struct->output_term);
		interpreter_xml_description(config,xmldata);
		interpreter_xml_totalrow(config,
			common_make_human_readable(ctx,sum));

	} else {
		printf("ERROR: parameter to the 'total' command can only be:\n");
		printf("rw, r, or w.\n");
		exit(1);
	}

	interpreter_xml_close_function(config,"total");
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
	struct tm *tmp = NULL;
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
		obj_struct->sql = talloc_asprintf(ctx,"%s and filename='%s' and "
			"%s and %s %s",
					obj_struct->sql,
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
		obj_struct->sql = talloc_asprintf(ctx,"%s and share='%s' and %s and %s %s",
					obj_struct->sql,
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
		obj_struct->sql = talloc_asprintf(ctx,"%s and username='%s' and %s and %s %s",
					obj_struct->sql,
					command_data->arguments[0],
					obj_struct->from,
					obj_struct->to,
					common_identify(ctx,INT_OBJ_USER,
						obj_struct->name,config,0));
		obj_struct->output_term = talloc_asprintf(ctx,
			"by user %s", obj_struct->name);
		break;
	case INT_OBJ_DOMAIN:
		obj_struct->object = INT_OBJ_DOMAIN;
		obj_struct->name = talloc_strdup(ctx,command_data->arguments[0]);
		interpreter_make_times(ctx,obj_struct,command_data);
		obj_struct->sql = talloc_asprintf(ctx,"%s and domain='%s' and %s and %s %s",
					obj_struct->sql,
					command_data->arguments[0],
					obj_struct->from,
					obj_struct->to,
					common_identify(ctx,INT_OBJ_DOMAIN,
						obj_struct->name,config,0));
		obj_struct->output_term = talloc_asprintf(ctx,
			"by domain %s",obj_struct->name);
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
	case INT_OBJ_SEARCH:
		interpreter_fn_search(ctx, command_data, obj_struct,config);
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
	if (strcmp(cmd, "search") == 0) return INT_OBJ_SEARCH;
	/* objects */
	if (strcmp(cmd, "share") == 0) return INT_OBJ_SHARE;
	if (strcmp(cmd, "user") == 0) return INT_OBJ_USER;
	if (strcmp(cmd, "file") == 0) return INT_OBJ_FILE;
	if (strcmp(cmd, "domain") == 0) return INT_OBJ_DOMAIN;
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
	if (en == NULL) {
		printf("ERROR: any function requires a parameter.\n");
		TALLOC_FREE(ctx);
		exit(1);
	}
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
		interpreter_print_table(ctx,config, 1,erg,"SQL Result");
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

int interpreter_run_from_file( TALLOC_CTX *ctx,
        char *filename,
        struct configuration_data *config)
{
        FILE *file;
        char cmd[5000];

        file = fopen( filename, "r");
        if (file == NULL) {
                printf("ERROR: error reading file '%s'.\n",filename);
		TALLOC_FREE(ctx);
                exit(1);
        }

        while (!feof(file)) {
                fgets(cmd,4999,file);
		cmd[strlen(cmd)-1] = '\0';
                if ( cmd[0] != '#' && cmd[0] != '\n' && cmd[0] != '\0') {
                        // anything not a comment is a command
                        interpreter_run(ctx, cmd,config);
                }
        }
	fclose(file);
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
	printf("domain	DOMAIN			Run a query over the\n"
		"				domain DOMAIN\n");
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
	printf("search 	[string]		Does a search for STRING\n");
	printf("				over the whole database.\n");

};
