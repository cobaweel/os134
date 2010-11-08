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
 * Handle the i8042 keyboard (and kitchen sink) controller.
 */                  

#include "std.h"
#include "i80386.h"
#include "i8042.h"
#include "i8259.h"
#include "term.h"

void keyboard_handler(void);

/* 
 * This buffer should not be too big, since type-ahead buffers that
 * are too big are actually annoying.
 */
#define BUFFER_SIZE 256
scancode_t  buffer[BUFFER_SIZE];
unsigned char buffer_latest = 0;
unsigned char buffer_oldest = 0;

general_error i8042_init(void)
{
     /* Install the IRQ handler for the controller. */
     install_irq_handler(1, &keyboard_handler);

     /* Read an input character and drop it. Somehow, the 8042 comes
      * with a spurious character on startup. */
     inb(0x60);

     return GENERAL_OK;
}

i8042_error getkey(scancode_t* s)
{
     if ( buffer_latest == buffer_oldest )
          return I8042_UNDERFLOW;
     else
          *s = buffer[buffer_oldest++];

     buffer_oldest %= BUFFER_SIZE;

     return I8042_OK;
}

scancode_t getkey_blocking(void)
{
     scancode_t s;

     while ( getkey(&s) != I8042_OK );

     return s;
}

void keyboard_handler(void)
{
     scancode_t scancode = inb(0x60);

     if ( buffer_latest + 1 == buffer_oldest ) 
          /* No space for you! Silently drop the scancode. */
          ;
     else
          /* Add scancode to buffer. */
          buffer[++buffer_latest] = scancode;
}
