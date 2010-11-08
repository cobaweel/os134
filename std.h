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
 * Standard definitions to be included in all kernel files.
 */

#ifndef _STD_H
#define _STD_H

/*
 * If we have included std.h, that means we are in C code, not
 * assembly:
 */
#define C_CODE
#define NULL 0

/* 
 * Booleans. I do not stick to the usual uppercase convention here,
 * since booleans are usually treated as primitive types.
 */
enum bool { false, true };
typedef enum bool bool;

/* 
 * A single machine word. We shall assume, for want of a better
 * option, that a word is an unsigned.
 */
typedef unsigned word;

/*
 * Error to be used in "all other cases".
 */
typedef enum general_error { 
     GENERAL_OK, GENERAL_TROUBLE  
} general_error;


/***********************************************************************
 * Bounded arrays.
 ***********************************************************************/

/*
 * The type for a bounded array; the entire struct is supposed to be
 * passed around by value as if it were a pointer.
 */
#define array(type) typedef struct { type* ptr; unsigned len; } array_ ## type

array(char);
array(int);
array(unsigned);
array(void);

/*
 * This macro creates a bounded array unsafely from a pointer. 
 */
#define unsafe_array(type,ptr_,len_)\
 ((array_ ## type) { .ptr = (type*) (ptr_), .len = (len_) })

/*
 * This macro creates a static array of elements.
 */
#define make_array(type, name,len_)\
type name##UNSAFE[(len_)];\
array_##type name = { name##UNSAFE, (len_) }

/*
 * This macro creates a bounded array safely, but is has bound 0. 
 */
#define empty_array(type)\
 ((array_ ## type) { .ptr = (type*) 0, .len = 0 })

/*
 * This macro creates a bounded array sort of safely from a string ptr.
 */
#define str_array(type,ptr_)\
 ((array_ ## type) { .ptr = (ptr_),\
                     .len = ({int __i =  0; while(*ptr_++) __i++; __i; }) } )
                              

/*
 * Error messages resulting from array operations. 
 */
typedef enum array_error { 
     ARRAY_OK, ARRAY_OUTOFBOUNDS  
} array_error;

/*
 * Assign the i-th element of a to x. 
 */
#define aref(a,i,x)  ( ((i)<(a).len) ? ({ (x)=(a).ptr[(i)]; ARRAY_OK; }) \
                                     : ARRAY_OUTOFBOUNDS  )

/*
 * Assign x to the i-th element of a. 
 */
#define aset(a,i,x)  ( ((i)<(a).len) ? ({ (a).ptr[(i)]=(x); ARRAY_OK; }) \
                                     : ARRAY_OUTOFBOUNDS  )

/*
 * Iterate over the elements of an array.
 */
#define foreach(a,i) for ((i) = 0; (i)<(a).len; (i)++)

#endif /* STD_H */
