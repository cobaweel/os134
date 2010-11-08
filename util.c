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

#include "std.h"
#include "util.h"


/*
 * Convert number to hexadecimal string representation.
 *  ***TODO*** Replace by safe function.
 */
void phex(unsigned x, char* c)
{
     int i;
     c[8] = 0;
     for (i = 7; i >= 0; i--) {
          switch (x & 0xf) {
          case 0x0: c[i] = '0'; break;
          case 0x1: c[i] = '1'; break;
          case 0x2: c[i] = '2'; break;
          case 0x3: c[i] = '3'; break;
          case 0x4: c[i] = '4'; break;
          case 0x5: c[i] = '5'; break;
          case 0x6: c[i] = '6'; break;
          case 0x7: c[i] = '7'; break;
          case 0x8: c[i] = '8'; break;
          case 0x9: c[i] = '9'; break;
          case 0xa: c[i] = 'a'; break;
          case 0xb: c[i] = 'b'; break;
          case 0xc: c[i] = 'c'; break;
          case 0xd: c[i] = 'd'; break;
          case 0xe: c[i] = 'e'; break;
          case 0xf: c[i] = 'f'; break;
          }
          x = x >> 4;
     }
}

/*
 * Copies one bounded array to the other. Fails if it doesn't fit.
 *  s: source
 *  d: destination
 */
general_error cpy(array_char s, array_char d) 
{
     unsigned i;
     
     foreach(s,i) {
          char c;
          if ( aref(s,i,c) != ARRAY_OK ) return GENERAL_TROUBLE;
          if ( aset(d,i,c) != ARRAY_OK ) return GENERAL_TROUBLE;
     }
     
     return GENERAL_OK;
}

