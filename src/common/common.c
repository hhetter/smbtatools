/*
 * common functions used by both clients
 *
 * Copyright (C) Michael Haefner, 2010
 * Copyright (C) Holger Hetterich, 2010
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

#include "../../include/common.h"


void network_close_connection( int sockfd ) {
// fixme , do something.
}



/*
 * Returns a string representation of
 * unsigned long int z    bytes.
 *
 */
char *common_make_human_readable( TALLOC_CTX *ctx, unsigned long long k )
{
	char kbstring[20];
	char *output;
	long long kb =  (long long ) k;
	long long int rest = 0;
	lldiv_t diff;
	strcpy(kbstring,"Bytes");
	if (kb >= 1024) { diff =  lldiv(kb, 1024); // kb
			strcpy(kbstring,"KB");
			kb = diff.quot;
			rest = diff.rem;
			}
	if (kb >= 1024) { diff = lldiv(kb+rest, 1024); // mb
			strcpy(kbstring,"MB");
			kb = diff.quot;
			rest = diff.rem;
		}
	if (kb >= 1024) {
	                diff = lldiv(kb+rest,1024); // gb
			kb = diff.quot;
			rest = diff.rem;
	                strcpy(kbstring,"GB");}
	if (kb >= 1024) {
	                diff = lldiv(kb+rest,1024); // tb
			kb = diff.quot;
			rest = diff.rem;
	                strcpy(kbstring,"TB");}
	output = talloc_asprintf( ctx,"%lli.%lli %s",kb,rest,kbstring);
	return output;
}

/*
 * Returns a string representation of
 * unsigned long int z    bytes.
 *
 */
char *common_make_human_readableFORM( TALLOC_CTX *ctx, unsigned long int z )
{
        char kbstring[20];
        char *output;
        strcpy(kbstring,"Bytes");
        long double kb = (long double) z;
        if (kb>=1024) { kb = kb/1024; // kb
                        strcpy(kbstring,"KB");}
        if (kb>=1024) { kb = kb/1024; // mb
                        strcpy(kbstring,"MB");}
        if (kb>=1024) {
                        kb = kb/1024; // gb
                        strcpy(kbstring,"GB");}
        if (kb>=1024) {
                        kb = kb/1024; // tb
                        strcpy(kbstring,"TB");}
        output = talloc_asprintf( ctx,"%04.02LF %s",kb,kbstring);
        return output;
}

/*
 * connect to the unix socket at char *name
 * in the filesystem
 */
int common_connect_unix_socket( char *name )
{
	int len, sock;
	struct sockaddr_un remote;
        if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
                printf("ERROR: Couldn't create unix socket.\n");
                exit(1);
        }

        remote.sun_family = AF_UNIX;
        strncpy(remote.sun_path, name,
		sizeof(remote.sun_path));
        len=strlen(remote.sun_path) + sizeof(remote.sun_family);
        if (connect(sock, (struct sockaddr *)&remote, len) == -1 ) {
		printf("ERROR: Couldn't connect to unix socket.\n");
                close(sock);
                exit(1);
        }
        return sock;
}

/*
 * Take a hostname as string, get its IP,
 * and connect a server socket.
 * returns the socket handle
 */
int common_connect_socket( const char *hostname, int iport )
{
	if ( hostname == NULL ) {
		printf("ERROR: no hostname given.\n");
		exit(1);
	}

        struct addrinfo *ai;
        struct addrinfo hints;
        memset(&hints,'\0',sizeof(hints));

        hints.ai_flags=AI_ADDRCONFIG;
        hints.ai_socktype=SOCK_STREAM;
        char port[255];
        sprintf(port,"%i",iport);
        int e=getaddrinfo(hostname,port,&hints,&ai);
        if (e!=0) {
                printf("ERROR: error getaddrinfo\n");
                exit(1);
        }
        int sockfd=socket(ai->ai_family,ai->ai_socktype,ai->ai_protocol);
        if (sockfd==-1) {
		printf("ERROR: error in socket operation!\n");
		exit(1);
	};
        int result=connect(sockfd,ai->ai_addr,ai->ai_addrlen);
        if (result==-1) {
		printf("ERROR: error in connect operation!\n");
		exit(1);
	};

        return sockfd;

}

/**
 * Create a SMBTAv2 header.
 * TALLLOC_CTX *ctx             Talloc context to work on
 * const char *state_flags      State flag string
 * int len                      length of the data block
 */
char *common_create_header( TALLOC_CTX *ctx,
        const char *state_flags, size_t data_len)
{
        char *header = talloc_asprintf( ctx, "V2.%s%017u",
                                        state_flags, (unsigned int) data_len);
        return header;
}


/**
 * Actually send header and data over the network
 * char *header         Header data
 * char *data           Data Block
 * int dlength          Length of data block
 * int socket
 */
void common_write_data( char *header, char *data,
                        int dlength, int _socket)
{
                int len = strlen(header);
		send(_socket,header,len,0);
		int h=send(_socket,data,dlength,0);
		if (h == -1) {
			printf("Error sending data to socket!\n");
			exit(1);
		}
}

/**
 * AES decrypt a data block.
 * returns the encrypted data block
 */
char *common_decrypt( TALLOC_CTX *ctx, char *body, int len, const unsigned char *thekey)
{
        AES_KEY key;
        int i;
        if (thekey == NULL) return NULL;
        AES_set_decrypt_key(thekey, 128, &key);
        char *data = talloc_array( ctx, char, len+4); // malloc(sizeof( char ) * (len +4));
        int s1 = ( len / 16 );
        for ( i = 0; i < s1; i++) {
                AES_decrypt((unsigned char *) body + (i*16), (unsigned char *) data+(i*16), &key);
        }
        data[len+1]= '\0';
        return data;
}

/**
 * Encryption of a data block with AES
 * TALLOC_CTX *ctx      Talloc context to work on
 * const char *akey     128bit key for the encryption
 * const char *str      Data buffer to encrypt, \0 terminated
 * int *len             Will be set to the length of the
 *                      resulting data block
 * The caller has to take care for the memory
 * allocated on the context.
 */
char *common_encrypt( TALLOC_CTX *ctx,
        const char *akey, const char *str, size_t *len)
{
        int s1,s2,h,d;
        AES_KEY key;
        unsigned char filler[17]= "................";
        char *output;
        unsigned char crypted[18];
        if (akey == NULL) return NULL;
        AES_set_encrypt_key((unsigned char *) akey, 128, &key);
        s1 = strlen(str) / 16;
        s2 = strlen(str) % 16;
        for (h = 0; h < s2; h++) *(filler+h)=*(str+(s1*16)+h);
        output = talloc_array(ctx, char, (s1*16)+17 );
        d=0;
        for (h = 0; h < s1; h++) {
                AES_encrypt((unsigned char *) str+(16*h), crypted, &key);
                for (d = 0; d<16; d++) output[d+(16*h)]=crypted[d];
        }
        AES_encrypt( (unsigned char *) str+(16*h), filler, &key );
        for (d = 0;d < 16; d++) output[d+(16*h)]=*(filler+d);
        *len = (s1*16)+16;
        return output;
}



/**
 * receive a data block. return 0 if the connection
 * was closed, or return the received buffer
 * int sock             The handle
 * int length           The number of bytes to
 *                      receive
 * char *buf		The databuffer in which to
 *			write the received data.
 * int *rlen		length of the received
 * 			data block, modified by
 * 			the function
 */
void common_receive_data( char *buf, int sock, int length, int *rlen)
{
        size_t t;
        t = recv( sock, buf, length, 0);

        if (t == 0) {
                /* connection closed */
                *rlen = 0;
                return;
        }
        *(buf + t) = '\0';
        *rlen = *rlen + t;
}

/**
 * Return the length of the data block to come given a header
 * as input.
 */
int common_get_data_block_length( char *header )
{
        int retval;
        retval = (int) common_myatoi( header+11 );
        return retval;
}

int common_load_key_from_file( struct configuration_data *c)
{
        FILE *keyfile;
        char *key = malloc(sizeof(char) * 21);
        int l;
        keyfile = fopen(c->keyfile, "r");
        if (keyfile == NULL) {
                return -1;
        }
        l = fscanf(keyfile, "%20s", key);
        if (strlen(key) != 16) {
                printf("ERROR: Key file in wrong format\n");
                fclose(keyfile);
                exit(1);
        }
        strncpy( (char *) c->key, key, 20);
        return 0;
}

long long int common_myatoi( char *num)
{
	errno = 0;
	long long val;
	val = strtoll(num, NULL, 10);
	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
		|| (errno != 0 && val == 0)) {
		perror("strtol");
		exit(1);
	}
	return val;
}


/*
 * Run a complete SQL query
 */
dbi_result sql_query( TALLOC_CTX *ctx,
        struct configuration_data *config,
        char *query )
{
	dbi_result res;
        if (config->debug_level>0) printf("\nSQL-QUERY: %s\n",query);
	res = dbi_conn_query( config->DBIconn, query);
	if ( res == NULL) return NULL;
	return res;
}


void interpreter_print_numbered_table( TALLOC_CTX *ctx,
                int columns,char *data, ... )
{
        int col=1;
        int row=1;
        int element=0;
        char *res = " ";
        char *arg = NULL;
        va_list ap;
        int count = columns;
        va_start( ap, NULL);
        printf("     ");
        while (count --) {
                arg = va_arg( ap, char *);
                printf("%-30s\t",arg);
        }
        va_end( ap );
        printf("\n");
        printf(
        "------------------------------------------------------------------------------\n");
        printf("%04i|",row);
        while (res != NULL) {
                res = result_get_element(ctx,element,data);
                if (res != NULL) printf("%-30s\t",res);
                if ( col==columns ) {
                        row++;
                        col = 0;
                        if (result_get_element(ctx,element+1,data) != NULL)
                                printf("\n%04i|",row);
                }
                col++; element++;
        }
}


/*
 * Get a single column of the result data from a query-result
 *
 * TALLOC_CTX *ctx              the talloc context to work on
 * int number                   the number of the column to get
 * const char *data             the result data block
 */
char *result_get_element( TALLOC_CTX *ctx, int number, dbi_result data )
{
	int fields; long long rr;
	int dv, dv2;
	const char *result;
	char *bufres;
	int rows;
	number++;
	dbi_result_first_row(data);
	fields = dbi_result_get_numfields(data);
	if (fields == DBI_FIELD_ERROR) return NULL;
	rows = dbi_result_get_numrows(data);
	if (rows == 0) {
		bufres = talloc_asprintf(ctx,"No Results.");
		return bufres;
	}
	if (number <= fields) {
		if (dbi_result_get_field_type_idx(
				data, (unsigned int) number) ==
				DBI_TYPE_STRING) {
					result = dbi_result_get_string_idx(
						data,
						(unsigned int) number);
					bufres = talloc_strdup( ctx,(const char *) result);
					return bufres;
					} else {
						rr = dbi_result_get_longlong_idx(
								data,
								(unsigned int) number);
						unsigned int attribs;
						attribs = dbi_result_get_field_attribs_idx(data, 1);

						bufres = talloc_asprintf(ctx,"%lli",rr);
						return bufres;
					}
	} else {
		dv = (number / fields)+1;
		dv2 = (number+fields) - ( dv * fields);
		if ( dbi_result_seek_row(data,dv) == 0) {
			return NULL;
		}
		if (dbi_result_get_field_type_idx(
					data, (unsigned int) dv2) == DBI_TYPE_STRING) {
			result = dbi_result_get_string_idx(
				data,
				(unsigned int) dv2);
			bufres = talloc_strdup(ctx,(const char *) result);
			return bufres;
		} else {
			int rr = dbi_result_get_int_idx(
					data,
					(unsigned int) number);
			bufres = talloc_asprintf(ctx,"%i",rr);
			return bufres;
		}
	}
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
		if (res == NULL) break;
                // if (res != NULL) printf("OO:%-30s\t",res);
                if ( col==columns ) { col = 0; row ++;  }
                col++; element++;
        }
        TALLOC_FREE(ctx);
        return row ;
}

/**
 * if config->identify is 0, we don't run sql queries to
 * identify an object, we just generate a pattern. This
 * is required if smbtad 's database is handling is shut down
 */
char *non_db_simple_identify( TALLOC_CTX *ctx,
	enum IntCommands Type,
	char *data,
	struct configuration_data *config,
	int qtype)
{
	char *retstr = NULL;
	if (qtype == 0) {
		retstr = talloc_asprintf(ctx, "and 1=1 ");
		return retstr;
	}
	switch(Type) {
	case INT_OBJ_USER:
		retstr = talloc_asprintf(ctx,
			"%04i%s0001*0001*0001*0001*",
			(int) strlen(data),data);
		break;
	case INT_OBJ_SHARE:
		retstr = talloc_asprintf(ctx,
			"0001*0001*%04i%s0001*0001*",
			(int) strlen(data),data);
		break;
	case INT_OBJ_FILE:
		retstr = talloc_asprintf(ctx,
			"0001*0001*0001*%04i%s0001*",
			(int) strlen(data),data);
		break;
	case INT_OBJ_DOMAIN:
		retstr = talloc_asprintf(ctx,
			"0001*0001*0001*0001*%04i%s",
			(int) strlen(data),data);
		break;
	default:
		printf("ERROR: unsupported type of object\n");
		exit(1);
	}
	return retstr;
}

char *common_identify( TALLOC_CTX *ctx,
        enum IntCommands Type,
        char *data,
        struct configuration_data *config,
	int qtype)
{
        char *query;
        char *qdat;
        char *retstr = NULL;
        int cols = 0;

	if (config->identify == 0) {
		retstr = non_db_simple_identify(ctx,Type,data,config,qtype);
		return retstr;
	}

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
	} else if (Type==INT_OBJ_DOMAIN) {
		/* identify domains */
		query = talloc_asprintf(ctx,
			"select distinct(domain) from read "
			"where domain = '%s' "
			"UNION select distinct(domain) from write "
			"where domain = '%s';",data,data);
		qdat = sql_query(
			ctx,
			config,
			query);
		cols = 1;
        } else {
                printf("ERROR: Identification of an unkown object type!\n");
                exit(1);
        }
        /* if only one row has been returned, the query is unique
         * or failed
         */
        if (interpreter_get_result_rows(qdat,cols) == 1) {
                /* check if the query does query for an existing */
                /* object at all.                                */
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
			case INT_OBJ_DOMAIN:
				printf("Domain %s doesn't exist ",data);
				break;
                        default:
                                printf("ERROR: Unsupported type of object!\n");
                                exit(1);
                        }
                        printf("in the database.\n");
                        exit(1);
                } else {
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
			case INT_OBJ_DOMAIN:
				printf("domain %s ",data);
				break;
                	default:
                        	printf("ERROR: Unsupported type of object!\n");
                        	exit(1);
                	}
                	if (qtype == 0 ) retstr = talloc_asprintf(ctx,"and 1=1 ");
			if (qtype == 1 ) {
				switch(Type) {
				case INT_OBJ_USER:
					retstr = talloc_asprintf(ctx,
					"%04i%s0001*0001*0001*0001*",
					(int) strlen(data),data);
					break;
				case INT_OBJ_SHARE:
					retstr = talloc_asprintf(ctx,
					"0001*0001*%04i%s0001*0001*",
					(int) strlen(data),data);
					break;
				case INT_OBJ_FILE:
					retstr = talloc_asprintf(ctx,
					"0001*0001*0001*%04i%s0001*",
					(int) strlen(data),data);
					break;
				case INT_OBJ_DOMAIN:
					retstr = talloc_asprintf(ctx,
					"0001*0001*0001*0001*%04i%s",
					(int) strlen(data),data);
					break;
				default:
					printf("ERROR: unsupported type of object\n");
					exit(1);
				}
			}
                	printf("as a unique item in the database.\n");
                	return retstr;
		}
        }

        /*
         * The result is not unique. Present the user with a list of
         * items to choose.
         */
        switch(Type) {
        case INT_OBJ_USER:
                printf("User '%s' is ambiguous. Please choose:\n",data);
                interpreter_print_numbered_table(ctx,cols,qdat,"SID",
			"Name",
			"Domain");
                break;
        case INT_OBJ_SHARE:
                printf("Share '%s' is ambiguous. Please choose:\n",data);
                interpreter_print_numbered_table(ctx,cols,qdat,
			"Domain",
			"Share");
                break;
        case INT_OBJ_FILE:
                printf("File '%s' is ambiguous. Please choose:\n",data);
                interpreter_print_numbered_table(ctx,cols,qdat,
			"Share","File");
                break;
	case INT_OBJ_DOMAIN:
		printf("Domain '%s' is ambiguous. Please choose:\n",data);
		interpreter_print_numbered_table(ctx,cols,qdat,"Domain");
		break;
        default:
                printf("ERROR: Unsupported type of object!\n");
                exit(1);

        }
        int number;
        printf("\nEnter number: ");
        scanf( "%i", &number);
        number--;
        if (number<0) {
                printf("ERROR: invalid input.\n");
                exit(1);
        }
        switch(Type) {
        case INT_OBJ_USER: ;
		if (qtype == 0) {
	                retstr = talloc_asprintf(ctx,"and usersid='%s' and domain='%s' ",
                        result_get_element(ctx,number*cols,qdat),
                        result_get_element(ctx,(number*cols)+2,qdat));}
		else { 
			retstr = talloc_asprintf(ctx,"%04i%s%04i%s0001*0001*%04i%s",
				(int) strlen(data),
				data,
				(int) strlen(result_get_element(ctx,number*cols,qdat)),
				result_get_element(ctx,number*cols,qdat),
				(int) strlen(result_get_element(ctx,(number*cols)+2,qdat)),
				result_get_element(ctx,(number*cols)+2,qdat));
		}
                break;
        case INT_OBJ_SHARE: ;
		if (qtype == 0) {
			retstr = talloc_asprintf(ctx,"and domain='%s' ",
                        	result_get_element(ctx,number*cols,qdat));}
			else {
			retstr = talloc_asprintf(ctx,"0001*0001*%04i%s0001*%04i%s",
				(int) strlen(data),data,
				(int) strlen(result_get_element(ctx,number*cols,qdat)),
				result_get_element(ctx,number*cols,qdat));
			}
                break;
        case INT_OBJ_FILE: ;
		if (qtype == 0) {
                	retstr = talloc_asprintf(ctx,"and share='%s' ",
                        	result_get_element(ctx,number*cols,qdat));
		} else {
			retstr = talloc_asprintf(ctx,"0001*0001*%04i%s%04i%s0001*",
				(int) strlen(result_get_element(ctx,number*cols,qdat)),
				result_get_element(ctx,number*cols,qdat),
				(int) strlen(data),data);
		}
                break;
        default:
                printf("ERROR: Unsupported type of object!\n");
                exit(1);
        }

        return retstr;
}

