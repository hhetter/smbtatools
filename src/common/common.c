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
char *common_make_human_readable( TALLOC_CTX *ctx, unsigned long int z )
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
	output = talloc_asprintf( ctx,"%4.02LF %s",kb,kbstring);
	return output;
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
        retval = atoi( header+11 );
        return retval;
}

/*
 * Run a complete SQL query
 */
char *sql_query( TALLOC_CTX *ctx,
        struct configuration_data *config,
        char *query )
{
        if (config->debug_level>0) printf("\nSQL-QUERY: %s\n",query);
        fd_set fd_set_r,fd_set_w,active_fd_set;
        int z=0;
        char *header=NULL;
        int header_position=0;
        char *body=NULL;
        int body_position=0;
        int data_length=0;
        int sockfd = config->socket;
        enum network_send_flags state = UNSENT;
        /* Initialize the set of active input sockets. */
        FD_ZERO (&active_fd_set);
        FD_SET(config->socket,&active_fd_set);
        fd_set_r=active_fd_set;
        fd_set_w=active_fd_set;
        while (state != DATA_RECEIVED) {
                /* Initialize the set of active input sockets. */
                FD_ZERO (&active_fd_set);
                FD_SET(config->socket,&active_fd_set);
                fd_set_r=active_fd_set;
                fd_set_w=active_fd_set;

                z=select( sockfd+1,&fd_set_r,&fd_set_w,NULL,NULL);
                if (FD_ISSET( sockfd,&fd_set_w) && state == UNSENT) {
                        /* ready to write to the socket */

                        /*
                        * The state flags are part of the header
                        * and are descripted in the protocol description
                        * in vfs_smb_traffic_analyzer.h
                        * in the Samba sources. They begin at byte
                        * 03 of the header.
                        */
                        char state_flags[9] = "000000\0";
                        header = common_create_header(ctx,
                                state_flags, strlen(query));
                        common_write_data( header,
                                query, strlen(query), sockfd);
                        state = SENT;
                        continue;
                }
                if (FD_ISSET( sockfd,&fd_set_r) && state == SENT) {
                        /* ready to read */
                        state = RECEIVING_HEADER;
                        header = talloc_array(ctx, char, 29);
                        common_receive_data(header, sockfd, 26,
                                &header_position);
                        if (header_position == 0) {
                                network_close_connection(sockfd);
                                continue;
                        }
                        if (header_position != 26) {
                                state = RECEIVING_HEADER_ONGOING;
                                continue;
                        }
                        state = HEADER_RECEIVED;
                        data_length = common_get_data_block_length(header);
                        continue;
                } else
                if (FD_ISSET( sockfd,&fd_set_r) &&
                        state == RECEIVING_HEADER_ONGOING) {
                        common_receive_data(header + header_position, sockfd,
                                26-header_position, &header_position);

                        if (header_position != 26) continue;
                        /* full header */
                        state = HEADER_RECEIVED;
                        data_length= common_get_data_block_length(header);
                        continue;
                } else
                if (FD_ISSET( sockfd,&fd_set_r) && state == HEADER_RECEIVED) {
                        state = RECEIVING_DATA;
                        body = talloc_array(ctx, char, data_length +2);
                        body_position = 0;
                        common_receive_data(body,
                                        sockfd,
                                        data_length,
                                        &body_position);
                        if (body_position==0) {
                                network_close_connection(sockfd);
                                continue;
                        }
                        if (body_position != data_length) {
                                state = RECEIVING_DATA_ONGOING;
                                continue;
                        }
                        /* full data set received */
                        state = DATA_RECEIVED;
                        break;
                } else
                if (FD_ISSET( sockfd,&fd_set_r) &&
                        state == RECEIVING_DATA_ONGOING) {
                        common_receive_data(body + body_position,sockfd,
                                data_length - body_position, &body_position);
                        if (body_position != data_length) continue;
                        state = DATA_RECEIVED;
                        break;
                }
        }
        return body;
}

char *result_get_element( TALLOC_CTX *ctx, int number, const char *data );

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
char *result_get_element( TALLOC_CTX *ctx, int number, const char *data )
{
        char bytecount[10];
        int datcount = 0;
        int t;
        int c = 0;
        int blocksize = 0;
        char *result = NULL;

        for (c = 0; c <= number; c++) {
                for (t = datcount; t<datcount+4 ; t++) {
                        bytecount[t-datcount]=data[t];
                }
                bytecount[4]='\0';
                blocksize = atoi(bytecount);
                if (blocksize == 0) return NULL;
                if ( c == number) {
                        result = talloc_array(ctx,char, blocksize +1);
                        datcount = datcount + 4;
                        for (t = datcount; t<datcount + blocksize; t++) {
                                result[t-datcount] = data[t];
                        }
                        result[blocksize]='\0';
                        datcount = datcount + blocksize +1;
                        break;
                } else datcount = datcount + 4 + blocksize+1;
        }
        return result;
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
	printf("ROWS:%i\n",interpreter_get_result_rows(qdat,cols));
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
			/*
			case INT_OBJ_DOMAIN:
				retstr = talloc_asprintf(ctx,
				"0001*0001*0001*0001*%04i%s",
				(int) strlen(data),data);
				break;
			*/
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

