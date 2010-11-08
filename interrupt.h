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
 * Interrupts. 
 */

#ifndef _INTERRUPT_H
#define _INTERRUPT_H


/*
 * The error code that *some* (not all!) i386 exceptions generate, but
 * that all OS134 interrupts generate (it just defaults to 0).
 */
typedef unsigned char interrupt_error_code;

/*
 * An interrupt handler function. You can register these.
 */
typedef void (*interrupt_handler)(interrupt_error_code);

/*
 * The number of an interrupt handler.
 */
typedef unsigned char interrupt_handler_nr;

/*
 * Initialize the interrupt system.
 */
general_error interrupt_init(void);

/*
 * Install an interrupt handler.
 */
general_error interrupt_install_handler(interrupt_handler_nr n, 
                                        interrupt_handler h);


#endif /* _INTERRUPT_H */
