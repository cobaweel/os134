/* -*- c -*-  */
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
 * This is the linker map for the kernel. It specifies the layout of
 * the binary object file. It does not specify where the kernel is to
 * be loaded; the multiboot header does that.
 */

ENTRY (_loader)

SECTIONS
{
    /* The kernel gets loaded at the 1MB boundary. */
    . = 0x00100000;
    .text :  { *(.text) }
    .rodata ALIGN (0x1000) : { *(.rodata) }
    .data   ALIGN (0x1000) : { *(.data) }

    /* I do not actually know what this does. */
    .bss :
    {
        _sbss = .;
        *(COMMON)
        *(.bss)
        _ebss = .;
    }
}
