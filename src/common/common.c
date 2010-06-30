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


