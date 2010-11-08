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
 * OS134 kernel. This is the center of the world.
 */

#include "std.h"
#include "vga.h"
#include "util.h"
#include "malloc.h"
#include "multiboot.h"
#include "segments.h"
#include "interrupt.h"
#include "i80386.h"
#include "i8259.h"
#include "i8042.h"
#include "exceptions.h"
#include "term.h"
#include "kb.h"


/*
 * The main entry point into the kernel, which gets called from asm.S
 */
void _main(void)
{
     /* It is not completely clear that the following magic is robust
      * against all possible compiler optimizations, but it seems to
      * not to be a big deal in practice. Multiboot leaves some
      * information sitting in registers eax and ebx. */
     register unsigned _magic asm (MULTIBOOT_MAGIC_LOCATION);
     register unsigned _info  asm (MULTIBOOT_INFO_LOCATION);
     unsigned magic = _magic;
     struct multiboot_info* info = (struct multiboot_info*) _info;

     /* Initialize display driver. Video memory is, unless one chooses
      * to mess with it, mapped at 0xb8000, and 4000 bytes long. */
     vga_init(unsafe_array(char, 0xb8000, 4000));

     /* Initialize the memory allocator. ***TODO*** figure out where
      * available memory starts. */
     malloc_init(unsafe_array(char, 0x200000, 0x200000));

     /* Initialize the terminal driver. */
     term_init(VGA_COLOR(VGA_LIGHT_GREY,VGA_BLACK));

     /* Initialize the keyboard to qwerty. */
     kb_init(us);

     /* Check whether we were loaded by a valid multiboot loader. */
     if (magic == MULTIBOOT_K_MAGIC) {
          color (VGA_COLOR(VGA_WHITE, VGA_BLUE));
          print("Boot was very succesful.\n");
          color (VGA_COLOR(VGA_LIGHT_GREY, VGA_BLACK));
     } else {
          print("No valid multiboot loader detected.\n");
     }

     /* Some more multiboot information. */
     print("Multiboot magic= "); printx(magic); print("... OK\n");
     print("Multiboot lower= "); printx(info->mem_lower); print("... OK\n");
     print("Multiboot upper= "); printx(info->mem_upper); print("... OK\n");

     /* Test malloc. */
     char* msg = "Hello malloc.";
     array_char old = str_array(char,msg);
     array_char new = malloc(20);
     if ( cpy(old,new) == GENERAL_TROUBLE ) print("malloc failed.\n");
     else print(msg);
     print("new.ptr "); printx((unsigned)new.ptr); print("\n");
     print("new.len "); printx((unsigned)new.len); print("\n");

     /* Initialize segments. */
     print("Initialize GDT... "); 
     printu(segments_install_gdt()); print("\n");

     /* Initialize interrupts. */
     print("Initialize IDT... "); 
     printu(interrupt_init()); print("\n");

     /* Initialize exceptions. */
     print("Initialize exceptions... ");
     printu(exceptions_init()); print("\n");

     /* Initialize IRQs. */
     print("Initialize IRQs... ");
     printu(irq_init()); print("\n");

     /* Initialize the keyboard driver. */
     i8042_init();

     /* Turn interrupts on. */
     print("Turn interrupts on... ");
     sti(); print("done.\n");

     /* Hang. */
     while(true) {
          char c = getc();
          int i;
          if (c & 0x80) {
               vga_color clr;
               /* printx(c & 0x7f); */
               switch (c & ~(0xffffff80)) {
               case 0x45:       /* Pause: Dvorak. */
                    kb_init(dv); break;
               case 0x46:       /* ScrLk: Qwerty.  */
                    kb_init(us); break;
               case 0x47:       /* Home: Clear screen. */
                    cls();       break;
               case 0x4f:       /* Surprise... */
                    getcolor(&clr);
                    color(clr+1); 
                    cls();
                    print("\n\n\n\n\n\n\n\n");
                    for(i = 0; i<5; i++){
                         clr += 0x10;
                         color((((clr >> 4) + 1) << 4) | (clr++ & 0x0f)); 
                         print("\t\t\t\tSURPRISE!!!!!!\n");
                    }
                    break;
               }
          } else {
               printc(c);
          }
     }
}
