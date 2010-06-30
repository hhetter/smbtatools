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




