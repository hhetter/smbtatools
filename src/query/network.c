

enum network_send_flags { UNSENT = 0, SENT, RECEIVING_HEADER, RECEIVING_HEADER_ONGOING, HEADER_RECEIVED, RECEIVING_DATA, DATA_RECEIVED };


/* blocking select call */
void sql_query( struct configuration_data *config, char *query )
{
        fd_set fd_set_r,fd_set_w,active_fd_set;
        int z=0;
	char *ctx = talloc(NULL, char);
	char *header;
	int header_position=0;
	int data_length=0;
	enum network_send_flags state = UNSENT;
        /* Initialize the set of active input sockets. */
        FD_ZERO (&active_fd_set);
        FD_SET(config->socket,&active_fd_set);
        fd_set_r=active_fd_set;
        fd_set_w=active_fd_set;

	while (state != DATA_RECEIVED) {

	        z=select( sockfd+1,&fd_set_r,&fd_set_w,NULL,NULL);
        	if (FD_ISSET( sockfd,&fd_set_w) && state == UNSENT) { /* ready to write to the socket */
		
	        	/*
         		* The state flags are part of the header
         		* and are descripted in the protocol description
         		* in vfs_smb_traffic_analyzer.h. They begin at byte
        		* 03 of the header.
         		*/
        		char state_flags[9] = "000000\0";
			header = common_create_header(ctx, state_flags, strlen(query));
			common_write_data( header, query, strlen(query), config->sockfd);
			state = SENT;
		if (FD_ISSET( sockfd,&fd_set_r) && state == SENT) { /* ready to read */
			state = RECEIVING_HEADER;
			header = talloc_array(ctx, char, 29);
			common_receive_data(header, sockfd, 26, &header_position);
			if (header_position == 0) {
				network_close_connection(sockfd);
				break;
			}
			if (header_position != 26) {
				state = RECEIVING_HEADER_ONGOING;
				break;
			}
			state = HEADER_RECEIVED;
			data_length = common_get_data_block_length(header);
		}
		if (FD_ISSET( sockfd,&fd_set_r) && state == RECEIVING_HEADER_ONGOING) {
			common_receive_data(header + header_position, sockfd,
				26-header_position, &header_position);

                        if (header_position != 26) break;
                        /* full header */
                        state = HEADER_RECEIVED;
                        data_length= common_get_data_block_length(header);
		}
}




