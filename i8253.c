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
 * Handle the i8253 programmable interrupt timer.
 */                  

#include "std.h"
#include "i8253.h"
#include "i8259.h"
#include "term.h"

/*
 * The 8253 is the PIT in a PC. In an AT, it is called the 8254. Later
 * HCMOS versions were called 82C54. These days, of course, they're
 * all emulated and embedded 5 layers down in who knows what
 * do-everything "legacy I/O" chip.
 */



void pit_handler(void)
{
     
}

general_error i8253_init(void)
{


}
