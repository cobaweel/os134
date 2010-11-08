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

#ifndef _I8042_H
#define _I8042_H

#include "std.h"

typedef signed char scancode_t;
typedef enum i8042_error { 
     I8042_OK, I8042_HW_FAIL, I8042_UNDERFLOW
} i8042_error;

general_error i8042_init(void);
i8042_error getkey(scancode_t* s);
scancode_t getkey_blocking(void);

#endif /* _I8042_H */
