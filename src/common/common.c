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
double  kb = (unsigned long int ) z / 1024; // kb
strcpy(kbstring,"KB");
if (kb>=1024) { kb = kb/1024; // mb
                strcpy(kbstring,"MB");}
if (kb>=1024) {
                kb = kb/1024; // gb
                strcpy(kbstring,"GB");}

if (kb>=1024) {
                kb = kb/1024; // tb
                strcpy(kbstring,"TB");}
char *output = malloc(sizeof(char)*20);
output = talloc_asprintf( ctx,"%2.2f %s",kb,kbstring);
return output;
}



