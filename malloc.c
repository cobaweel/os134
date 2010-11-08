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
 * Allocate memory in a very simple manner.
 */

#include "std.h"
#include "malloc.h"

/*
 * The big old free pointer.
 */
array_char malloc_fp = empty_array(char);

/* 
 * Initialize the memory allocator.
 *  fp: initial free pointer
 */
malloc_error malloc_init(array_char fp)
{
     /* Initialize the allocator. */
     malloc_fp = fp;
     
     /* Report succes. */
     return MALLOC_OK;
}

/*
 * Allocate an array of characters.
 *  len: length of the array
 */
array_char malloc(unsigned len)
{
     /* Align. */
     malloc_fp.ptr = (char *) ( (unsigned) malloc_fp.ptr & 0xfffffff8);

     /* Create array. */
     array_char new = unsafe_array(char, malloc_fp.ptr, len);

     /* Allocate. */
     malloc_fp.ptr += len;
     malloc_fp.len -= len;

     /* Return newly created array. */
     return new;
}
