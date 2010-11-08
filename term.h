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
 * Terminal driver. Built on top of the VGA driver to provide a more
 * stateful behavior.
 */

#ifndef _TERM_H
#define _TERM_H

#include "vga.h"
#include "kb.h"

enum term_error { TERM_OK, TERM_TROUBLE };
typedef enum term_error term_error;

/*
 * Put any intialization here.
 */
term_error term_init(vga_color color);

/*
 * Print a string.
 *  msg: string to be printed
 */
term_error print(char* msg);

/* 
 * Print a number as unsigned hexadecimal.
 *  x: number to be printed
 */
term_error printx(unsigned x);

term_error printc(char chr);

/* 
 * Print a number as unsigned decimal.
 *  d: number to be printed
 */
term_error printu(unsigned x);

/* 
 * Print a number as signed decimal.
 *  d: number to be printed
 */
term_error printd(int x);

/*
 * Clear the terminal.
 */
term_error cls(void);

/*
 * Change the current color.
 *  color: color to change it to
 */
term_error color(vga_color color);

/*
 * Change the current color.
 *  color: color to change it to
 */
term_error getcolor(vga_color* color);



/*
 * Get key.
 */
key_t getc(void);

#endif /* _TERM_H */
