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

#include "../../include/common.h"
#include "include/configuration.h"
enum network_send_flags { 
	UNSENT = 0,
	SENT,
	RECEIVING_HEADER,
	RECEIVING_HEADER_ONGOING,
	HEADER_RECEIVED,
	RECEIVING_DATA,
	RECEIVING_DATA_ONGOING,
	DATA_RECEIVED };


void network_close_connection( int sockfd ) {
// fixme , do something.
}

/*
 * Get a single column of the result data from a query-result
 *
 * TALLOC_CTX *ctx		the talloc context to work on
 * int number			the number of the column to get
 * const char *data		the result data block
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

char *sql_query( TALLOC_CTX *ctx,
	struct configuration_data *config,
	char *query )
{
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



