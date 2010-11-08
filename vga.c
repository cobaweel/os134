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
 * VGA driver. Driver for a standard VGA graphics board in text mode.
 */

#include "std.h"
#include "vga.h"
#include "i80386.h"

/*
 * Forward declarations for LOCAL procedures.
 */
/* None so far. */

/* 
 * Static global state connected to the VGA board.
 */
array_char vga = empty_array(char); /* The video memory. */

/*
 * Initialize video memory driver global state.
 *  vga_ An array referring to the VGA memory.
 */
vga_error vga_init(array_char vga_)
{
     unsigned i;                /* Loop counter. */

     /* Initialize video memory. */
     vga = vga_;

     /* Blank out the screen. */
     for ( i = 0; i < vga.len; i++) aset(vga, i, 0);

     /* Report succes. */
     return VGA_OK;
}

/*
 * Sets the cursor position.
 */
vga_error set_cursor(unsigned dest)
{
     outb(CRT_ADDRESS, CRT_CURSOR_LOC_LO);
     outb(CRT_DATA, dest);
     outb(CRT_ADDRESS, CRT_CURSOR_LOC_HI);
     outb(CRT_DATA, (dest) >> 8);

     return VGA_OK;
}

/*
 * Print a character to the screen.
 *  chr The character to be printed.
 *  dest The (raw) position on the screen where it should go.
 */
vga_error vga_putchr(char chr, unsigned dest, vga_color color)
{
     /* Every other byte in VGA memory is a character. */
     unsigned offset = dest * 2; 
     
     /* Display the character. */
     if (aset(vga, offset, chr) == ARRAY_OUTOFBOUNDS)
          return VGA_OUTOFBOUNDS;
     if (aset(vga, offset+1, color) == ARRAY_OUTOFBOUNDS)
          return VGA_OUTOFBOUNDS;

     /* Report succes. */
     return VGA_OK;
}

/*
 * Get a character from the screen.
 *  src The (raw) position on the screen where it should come from.
 */
vga_error vga_getchr(unsigned src, char* chr)
{
     /* Every other byte in VGA memory is a character. */
     unsigned offset = src * 2; 
     
     if (aref(vga, offset, *chr) == ARRAY_OUTOFBOUNDS)
          return VGA_OUTOFBOUNDS;

     /* Report succes. */
     return VGA_OK;
}

/*
 * Get a color from the screen.
 *  src The (raw) position on the screen where it should come from.
 */
vga_error vga_getclr(unsigned src, vga_color* clr)
{
     /* Every other byte in VGA memory is a color. */
     unsigned offset = src * 2 + 1; 
     
     if (aref(vga, offset, *clr) == ARRAY_OUTOFBOUNDS)
          return VGA_OUTOFBOUNDS;

     /* Report succes. */
     return VGA_OK;
}

