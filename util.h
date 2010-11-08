/* OS134, Copyright (C) 2005, Benjamin Stein, Jaap Weel, Ting Liao --
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at
your option) any later version.  This program is distributed in the
hope that it will be useful, but WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU General Public License for more details.  You
should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA */

/*
 * Utility functions, to be replaced later by a standard library.
 */

#ifndef _UTIL_H
#define _UTIL_H


/*
 * Convert number to hexadecimal string representation.
 *  ***TODO*** Replace by safe function.
 */
void phex(unsigned x, char* c);

/*
 * Copies one bounded array to the other. Fails if it doesn't fit.
 *  s: source
 *  d: destination
 */
general_error cpy(array_char s, array_char d);

#endif /* _UTIL_H */
