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
 * Deal with segment tables.
 */ 
#include "std.h"
#include "i80386.h"
#include "segments.h"

/* 
 * Here is the GDT, in all it's gory glory.
 */
i386_segment_descriptor gdt[] = {
     { 0,0,0,0,0,0 },           /* Null descriptor. Don't ask. */
     { I386_SEGLIMIT_0_15  (0xfffff), /* Code segment. */
       I386_SEGBASE_0_15   (0),
       I386_SEGBASE_16_23  (0),
       I386_SEGINFO1_XR |       /* Exec and read allowed.*/
       I386_SEGINFO1_PRESENT |  /* Segment present. */
       I386_SEGINFO1_DATA_CODE, /* "Data/code" segment (not "system". */
       I386_SEGINFO2_OP32 |     /* 32 bit operand. */
       I386_SEGINFO2_PAGE |     /* Granularity is one page. */       
       I386_SEGINFO2_LIMIT(0xfffff),
       I386_SEGBASE_24_31(0) },
     { I386_SEGLIMIT_0_15  (0xfffff), /* Data segment. */
       I386_SEGBASE_0_15   (0),
       I386_SEGBASE_16_23  (0),
       I386_SEGINFO1_RW |       /* Read and write allowed.*/
       I386_SEGINFO1_PRESENT |  /* Segment present. */
       I386_SEGINFO1_DATA_CODE, /* "Data/code" segment (not "system". */
       I386_SEGINFO2_OP32 |     /* 32 bit operand. */
       I386_SEGINFO2_PAGE |     /* Granularity is one page. */       
       I386_SEGINFO2_LIMIT(0xfffff),
       I386_SEGBASE_24_31(0) }
};

/*
 * And here's the wacky intel pointer to it.
 */
i386_bounded_pointer gdt_pointer = {
     sizeof(gdt),               /* Size of the GDT. */
     (void*) &gdt[0]            /* Location of the GDT. */
};

general_error segments_install_gdt() 
{
     /* Load the GDT. */
     load_gdt(&gdt_pointer);

     /* If this goes wrong, there's little we can do about it. In
      * fact, we will have probably already crashed by now. */
     return GENERAL_OK;
}
