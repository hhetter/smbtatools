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

#include <netdb.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <signal.h>
#include <pthread.h>
#include <getopt.h>
#include <syslog.h>
#include <sys/select.h>
#include <dlfcn.h>
#include <string.h>
#include <stdio.h>
#include <talloc.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include "../src/common/config-struct.h"

/* define TALLOC_FREE when older talloc versions are used */
#ifndef TALLOC_FREE
        #define TALLOC_FREE(ctx) do { talloc_free(ctx); ctx=NULL; } while(0)
#endif


enum network_send_flags {
        UNSENT = 0,
        SENT,
        RECEIVING_HEADER,
        RECEIVING_HEADER_ONGOING,
        HEADER_RECEIVED,
        RECEIVING_DATA,
        RECEIVING_DATA_ONGOING,
        DATA_RECEIVED };

enum IntCommands {
        INT_OBJ_SHARE = 0,
        INT_OBJ_USER,
        INT_OBJ_FILE,
        INT_OBJ_TOTAL,
        INT_OBJ_LIST,
        INT_OBJ_TOP,
        INT_OBJ_GLOBAL,
        INT_OBJ_LAST,
        INT_OBJ_USAGE};


/*
 * return a string representation of z
 * e.g. z=1024 output will be 1KB
 */
long long int common_myatoi( char *num);
int common_load_key_from_file( struct configuration_data *c);
char *common_make_human_readable( TALLOC_CTX *ctx, unsigned long int z );
int common_connect_socket( const char *hostname,int iport );
char *common_create_header( TALLOC_CTX *ctx, const char *state_flags, size_t data_len);
void common_write_data( char *header, char *data, int dlength, int _socket);
void common_receive_data( char *buf, int sock, int length, int *rlen);
int common_get_data_block_length( char *header );
char *sql_query( TALLOC_CTX *ctx, struct configuration_data *config, char *query );
void network_close_connection( int sockfd );
int common_connect_unix_socket( char *name );
char *common_identify( TALLOC_CTX *ctx,
        enum IntCommands Type,
        char *data,
        struct configuration_data *config, int qtype);
char *result_get_element( TALLOC_CTX *ctx, int number, const char *data );
