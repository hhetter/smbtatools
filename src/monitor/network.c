/*
 * smbtamonitor
 * real-time monitor for samba traffic analyzer
 *
 * Copyright (C) Michael Haefner, 2010
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
#include "include/includes.h"



/**
 *  * create a unix domain socket
 *   *
 *    */
int network_create_unix_socket(char *name)
{
	/* Create a streaming UNIX Domain Socket */
        int s,len;
        struct sockaddr_un local;
        if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
                syslog(LOG_DAEMON, "ERROR: unix socket creation failed.");
                return 1;
        }

        local.sun_family= AF_UNIX;
        strcpy(local.sun_path,name);
        unlink(local.sun_path);
        len=strlen(local.sun_path) + sizeof(local.sun_family);
        bind(s,(struct sockaddr *) &local, len);
        listen(s,50);
										        return s;
}




/*
 * transmit the monitor function and the pattern
 * to smbtad and receive the id
 */
int network_register_monitor( enum monitor_fn func,
	char *param,
	char *pattern, struct configuration_data *c)
{
        char *tosend, *data;
        tosend = talloc_asprintf(NULL,"~~0001%i%04i%s%s",
			                func,(int) strlen(param),param,pattern);
        char *body = connect_monitor(NULL,c,tosend);
        data = result_get_monitor_element(NULL,0,body);
        int retval = (int) common_myatoi(data);



	monitor_list_add( (int) retval, func);	
	talloc_free(tosend);
	talloc_free(data);
	talloc_free(body);
        return 1;
}


/*
 * to be run as a detached thread, this function
 * receives data once the monitors are running
 * and updates the monitor list accordingly with
 * the results
 */
void network_handle_data( struct configuration_data *c)
{
        pthread_detach(pthread_self());
        fd_set fd_set_r,fd_set_w,active_fd_set;
        int z=0;
        char *header=NULL;
        int header_position=0;
        char *body=NULL;
        int body_position=0;
        int data_length=0;
        int sockfd = c->socket;
        enum network_send_flags state = UNSENT;

	state = UNSENT;
	while (1 == 1) {
	        /* Initialize the set of active input sockets. */
		header = NULL;
		header_position = 0;
		body = NULL;
		data_length = 0;
		sockfd = c->socket;
       		FD_ZERO (&active_fd_set);
	        FD_SET(c->socket,&active_fd_set);
	        fd_set_r=active_fd_set;
 	  	fd_set_w=active_fd_set;
	        while (state != DATA_RECEIVED) {
        	        /* Initialize the set of active input sockets. */
                	FD_ZERO (&active_fd_set);
                	FD_SET(c->socket,&active_fd_set);
                	fd_set_r=active_fd_set;
                	fd_set_w=active_fd_set;

                	z=select( sockfd+1,&fd_set_r,NULL,NULL,NULL);
                	if (FD_ISSET( sockfd,&fd_set_r) && state == UNSENT) {
                        	/* ready to read */
                        	state = RECEIVING_HEADER;
                        	header = talloc_array(NULL, char, 29);
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
                        	data_length = common_get_data_block_length(
					header);
                        	continue;
                	} else
                	if (FD_ISSET( sockfd,&fd_set_r) &&
                        	state == RECEIVING_HEADER_ONGOING) {
                        	common_receive_data(header + header_position,
					sockfd,
                                	26-header_position, &header_position);

                        	if (header_position != 26) continue;
                        	/* full header */
                        	state = HEADER_RECEIVED;
                        	data_length= common_get_data_block_length(
					header);
                        	continue;
                	} else
                	if (FD_ISSET( sockfd,&fd_set_r) &&
				state == HEADER_RECEIVED) {
                        	state = RECEIVING_DATA;
                        	body = talloc_array(NULL, char, data_length +2);
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
                        	state = UNSENT;
                        	break;
                	} else
                	if (FD_ISSET( sockfd,&fd_set_r) &&
                        	state == RECEIVING_DATA_ONGOING) {
                        	common_receive_data(body + body_position,
					sockfd,
                                	data_length - body_position,
					&body_position);
                        	if (body_position != data_length) continue;
                        	state = UNSENT;
                        	break;
                	}
        	}
                if (c->keyfile != NULL) { // body is encrypted
                        char *crypted = common_decrypt(NULL,body,data_length,
                                (const unsigned char *) c->key);
                        monitor_list_change_results(crypted,c);
                        talloc_free(crypted);
                        } else monitor_list_change_results(body,c);
		talloc_free(header);
		talloc_free(body);
	}
}



