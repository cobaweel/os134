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
 * Deal with interrupt tables.
 */ 
#include "std.h"
#include "vga.h"
#include "i80386.h"
#include "handlers.h"
#include "segments.h"
#include "interrupt.h"
#include "term.h"

void initialize_idt(void);
void default_interrupt_handler(interrupt_error_code n);
void generalized_interrupt_handler(interrupt_handler_nr i);

/*
 * And here is the actual IDT. All 255 entries of it.
 */
i386_interrupt_descriptor idt[256];

/*
 * The wacky intel pointer to the IDT.
 */
i386_bounded_pointer idt_pointer = {
     sizeof(idt),               /* Size of the IDT. */
     (void*) &idt[0]            /* Location of the IDT. */
};

/*
 * Once the assembly routine is called by the i386, it clears
 * interrupts and sets up the stack, only to call a C function from
 * yet another jump table! 
 */
interrupt_handler interrupt_handler_table[256];

/*
 * Initialize the interrupt system.
 */
general_error interrupt_init(void) 
{
     int i;                     /* Loop counter. */

     /* Initialize the IDT. */
     initialize_idt();

     /* Load the IDT. */
     load_idt(&idt_pointer);

     /* Load the jump table. */
     for (i=0; i<=255; i++)
          interrupt_handler_table[i] = &default_interrupt_handler;

     /* If this goes wrong, there's little we can do about it. In
      * fact, we will have probably already crashed by now. */
     return GENERAL_OK;
}

/*
 * Calls the correct handler from the table.
 */
void generalized_interrupt_handler(interrupt_handler_nr i)
{
     (interrupt_handler_table[i])(0);
}

/*
 * Before handlers have been installed, this is what happens.
 */
void default_interrupt_handler(interrupt_error_code n __attribute__((unused)))
{
     /* Display uninformative message. */
     print("SOMEBODY SET UP US THE BOMB (unhandled interrupt)\n");

     return;
}

general_error interrupt_install_handler(interrupt_handler_nr n, 
                                        interrupt_handler h)
{
     if (h != NULL) {
          interrupt_handler_table[n] = h;
          return GENERAL_OK;
     } else {
          return GENERAL_TROUBLE;
     }
}

void initialize_idt(void)
{
     int i = 0;

#define ENTRY(x)\
     idt[i].handler_0_15  = I386_INTHANDLER_0_15((unsigned int) &x);\
     idt[i].selector      = I386_CODE_SEGMENT_DESCRIPTOR_OFFSET;\
     idt[i].info1         = I386_INTINFO1;\
     idt[i].info2         = ( I386_INTINFO2 | \
                              I386_INTINFO2_PRESENT | \
                              I386_INTINFO2_RING0 );\
     idt[i].handler_16_31 = I386_INTHANDLER_16_31((unsigned int) &x);\
     i++;

     ENTRY(int0);               /* Division by zero. */
     ENTRY(int1);               /* Debug. */
     ENTRY(int2);               /* Non-maskable interrupt. */
     ENTRY(int3);               /* Breakpoint. */
     ENTRY(int4);               /* INTO detected overflow. */
     ENTRY(int5);               /* Out of bounds. */
     ENTRY(int6);               /* Invalid opcode. */
     ENTRY(int7);               /* No coprocessor. */
     ENTRY(int8);               /* Double fault. */
     ENTRY(int9);               /* Coprocessor segment overrun. */
     ENTRY(int10);              /* Bad TSS. */
     ENTRY(int11);              /* Segment not present. */
     ENTRY(int12);              /* Stack fault. */
     ENTRY(int13);              /* General protection fault. */
     ENTRY(int14);              /* Page fault. */
     ENTRY(int15);              /* Unknown interrupt. */
     ENTRY(int16);              /* Coprocessor fault. */
     ENTRY(int17);              /* Alignment check. */
     ENTRY(int18);              /* Machine check. */
     ENTRY(int19);              /* "Reserved". */
     ENTRY(int20);              /* "Reserved". */
     ENTRY(int21);              /* "Reserved". */
     ENTRY(int22);              /* "Reserved". */
     ENTRY(int23);              /* "Reserved". */
     ENTRY(int24);              /* "Reserved". */
     ENTRY(int25);              /* "Reserved". */
     ENTRY(int26);              /* "Reserved". */
     ENTRY(int27);              /* "Reserved". */
     ENTRY(int28);              /* "Reserved". */
     ENTRY(int29);              /* "Reserved". */
     ENTRY(int30);              /* "Reserved". */
     ENTRY(int31);              /* "Reserved". */
     ENTRY(int32);              /* IRQ0. */
     ENTRY(int33);              /* IRQ1. */
     ENTRY(int34);              /* IRQ2. */
     ENTRY(int35);              /* IRQ3. */
     ENTRY(int36);              /* IRQ4. */
     ENTRY(int37);              /* IRQ5. */
     ENTRY(int38);              /* IRQ6. */
     ENTRY(int39);              /* IRQ7. */
     ENTRY(int40);              /* IRQ8. */
     ENTRY(int41);              /* IRQ9. */
     ENTRY(int42);              /* IRQ10. */
     ENTRY(int43);              /* IRQ11. */
     ENTRY(int44);              /* IRQ12. */
     ENTRY(int45);              /* IRQ13. */
     ENTRY(int46);              /* IRQ14. */
     ENTRY(int47);              /* IRQ15. */
     ENTRY(int48);              /* Programmable. */
     ENTRY(int49);              /* Programmable. */
     ENTRY(int50);              /* Programmable. */
     ENTRY(int51);              /* Programmable. */
     ENTRY(int52);              /* Programmable. */
     ENTRY(int53);              /* Programmable. */
     ENTRY(int54);              /* Programmable. */
     ENTRY(int55);              /* Programmable. */
     ENTRY(int56);              /* Programmable. */
     ENTRY(int57);              /* Programmable. */
     ENTRY(int58);              /* Programmable. */
     ENTRY(int59);              /* Programmable. */
     ENTRY(int60);              /* Programmable. */
     ENTRY(int61);              /* Programmable. */
     ENTRY(int62);              /* Programmable. */
     ENTRY(int63);              /* Programmable. */
     ENTRY(int64);              /* Programmable. */
     ENTRY(int65);              /* Programmable. */
     ENTRY(int66);              /* Programmable. */
     ENTRY(int67);              /* Programmable. */
     ENTRY(int68);              /* Programmable. */
     ENTRY(int69);              /* Programmable. */
     ENTRY(int70);              /* Programmable. */
     ENTRY(int71);              /* Programmable. */
     ENTRY(int72);              /* Programmable. */
     ENTRY(int73);              /* Programmable. */
     ENTRY(int74);              /* Programmable. */
     ENTRY(int75);              /* Programmable. */
     ENTRY(int76);              /* Programmable. */
     ENTRY(int77);              /* Programmable. */
     ENTRY(int78);              /* Programmable. */
     ENTRY(int79);              /* Programmable. */
     ENTRY(int80);              /* Programmable. */
     ENTRY(int81);              /* Programmable. */
     ENTRY(int82);              /* Programmable. */
     ENTRY(int83);              /* Programmable. */
     ENTRY(int84);              /* Programmable. */
     ENTRY(int85);              /* Programmable. */
     ENTRY(int86);              /* Programmable. */
     ENTRY(int87);              /* Programmable. */
     ENTRY(int88);              /* Programmable. */
     ENTRY(int89);              /* Programmable. */
     ENTRY(int90);              /* Programmable. */
     ENTRY(int91);              /* Programmable. */
     ENTRY(int92);              /* Programmable. */
     ENTRY(int93);              /* Programmable. */
     ENTRY(int94);              /* Programmable. */
     ENTRY(int95);              /* Programmable. */
     ENTRY(int96);              /* Programmable. */
     ENTRY(int97);              /* Programmable. */
     ENTRY(int98);              /* Programmable. */
     ENTRY(int99);              /* Programmable. */
     ENTRY(int100);             /* Programmable. */
     ENTRY(int101);             /* Programmable. */
     ENTRY(int102);             /* Programmable. */
     ENTRY(int103);             /* Programmable. */
     ENTRY(int104);             /* Programmable. */
     ENTRY(int105);             /* Programmable. */
     ENTRY(int106);             /* Programmable. */
     ENTRY(int107);             /* Programmable. */
     ENTRY(int108);             /* Programmable. */
     ENTRY(int109);             /* Programmable. */
     ENTRY(int110);             /* Programmable. */
     ENTRY(int111);             /* Programmable. */
     ENTRY(int112);             /* Programmable. */
     ENTRY(int113);             /* Programmable. */
     ENTRY(int114);             /* Programmable. */
     ENTRY(int115);             /* Programmable. */
     ENTRY(int116);             /* Programmable. */
     ENTRY(int117);             /* Programmable. */
     ENTRY(int118);             /* Programmable. */
     ENTRY(int119);             /* Programmable. */
     ENTRY(int120);             /* Programmable. */
     ENTRY(int121);             /* Programmable. */
     ENTRY(int122);             /* Programmable. */
     ENTRY(int123);             /* Programmable. */
     ENTRY(int124);             /* Programmable. */
     ENTRY(int125);             /* Programmable. */
     ENTRY(int126);             /* Programmable. */
     ENTRY(int127);             /* Programmable. */
     ENTRY(int128);             /* Programmable. */
     ENTRY(int129);             /* Programmable. */
     ENTRY(int130);             /* Programmable. */
     ENTRY(int131);             /* Programmable. */
     ENTRY(int132);             /* Programmable. */
     ENTRY(int133);             /* Programmable. */
     ENTRY(int134);             /* Programmable. */
     ENTRY(int135);             /* Programmable. */
     ENTRY(int136);             /* Programmable. */
     ENTRY(int137);             /* Programmable. */
     ENTRY(int138);             /* Programmable. */
     ENTRY(int139);             /* Programmable. */
     ENTRY(int140);             /* Programmable. */
     ENTRY(int141);             /* Programmable. */
     ENTRY(int142);             /* Programmable. */
     ENTRY(int143);             /* Programmable. */
     ENTRY(int144);             /* Programmable. */
     ENTRY(int145);             /* Programmable. */
     ENTRY(int146);             /* Programmable. */
     ENTRY(int147);             /* Programmable. */
     ENTRY(int148);             /* Programmable. */
     ENTRY(int149);             /* Programmable. */
     ENTRY(int150);             /* Programmable. */
     ENTRY(int151);             /* Programmable. */
     ENTRY(int152);             /* Programmable. */
     ENTRY(int153);             /* Programmable. */
     ENTRY(int154);             /* Programmable. */
     ENTRY(int155);             /* Programmable. */
     ENTRY(int156);             /* Programmable. */
     ENTRY(int157);             /* Programmable. */
     ENTRY(int158);             /* Programmable. */
     ENTRY(int159);             /* Programmable. */
     ENTRY(int160);             /* Programmable. */
     ENTRY(int161);             /* Programmable. */
     ENTRY(int162);             /* Programmable. */
     ENTRY(int163);             /* Programmable. */
     ENTRY(int164);             /* Programmable. */
     ENTRY(int165);             /* Programmable. */
     ENTRY(int166);             /* Programmable. */
     ENTRY(int167);             /* Programmable. */
     ENTRY(int168);             /* Programmable. */
     ENTRY(int169);             /* Programmable. */
     ENTRY(int170);             /* Programmable. */
     ENTRY(int171);             /* Programmable. */
     ENTRY(int172);             /* Programmable. */
     ENTRY(int173);             /* Programmable. */
     ENTRY(int174);             /* Programmable. */
     ENTRY(int175);             /* Programmable. */
     ENTRY(int176);             /* Programmable. */
     ENTRY(int177);             /* Programmable. */
     ENTRY(int178);             /* Programmable. */
     ENTRY(int179);             /* Programmable. */
     ENTRY(int180);             /* Programmable. */
     ENTRY(int181);             /* Programmable. */
     ENTRY(int182);             /* Programmable. */
     ENTRY(int183);             /* Programmable. */
     ENTRY(int184);             /* Programmable. */
     ENTRY(int185);             /* Programmable. */
     ENTRY(int186);             /* Programmable. */
     ENTRY(int187);             /* Programmable. */
     ENTRY(int188);             /* Programmable. */
     ENTRY(int189);             /* Programmable. */
     ENTRY(int190);             /* Programmable. */
     ENTRY(int191);             /* Programmable. */
     ENTRY(int192);             /* Programmable. */
     ENTRY(int193);             /* Programmable. */
     ENTRY(int194);             /* Programmable. */
     ENTRY(int195);             /* Programmable. */
     ENTRY(int196);             /* Programmable. */
     ENTRY(int197);             /* Programmable. */
     ENTRY(int198);             /* Programmable. */
     ENTRY(int199);             /* Programmable. */
     ENTRY(int200);             /* Programmable. */
     ENTRY(int201);             /* Programmable. */
     ENTRY(int202);             /* Programmable. */
     ENTRY(int203);             /* Programmable. */
     ENTRY(int204);             /* Programmable. */
     ENTRY(int205);             /* Programmable. */
     ENTRY(int206);             /* Programmable. */
     ENTRY(int207);             /* Programmable. */
     ENTRY(int208);             /* Programmable. */
     ENTRY(int209);             /* Programmable. */
     ENTRY(int210);             /* Programmable. */
     ENTRY(int211);             /* Programmable. */
     ENTRY(int212);             /* Programmable. */
     ENTRY(int213);             /* Programmable. */
     ENTRY(int214);             /* Programmable. */
     ENTRY(int215);             /* Programmable. */
     ENTRY(int216);             /* Programmable. */
     ENTRY(int217);             /* Programmable. */
     ENTRY(int218);             /* Programmable. */
     ENTRY(int219);             /* Programmable. */
     ENTRY(int220);             /* Programmable. */
     ENTRY(int221);             /* Programmable. */
     ENTRY(int222);             /* Programmable. */
     ENTRY(int223);             /* Programmable. */
     ENTRY(int224);             /* Programmable. */
     ENTRY(int225);             /* Programmable. */
     ENTRY(int226);             /* Programmable. */
     ENTRY(int227);             /* Programmable. */
     ENTRY(int228);             /* Programmable. */
     ENTRY(int229);             /* Programmable. */
     ENTRY(int230);             /* Programmable. */
     ENTRY(int231);             /* Programmable. */
     ENTRY(int232);             /* Programmable. */
     ENTRY(int233);             /* Programmable. */
     ENTRY(int234);             /* Programmable. */
     ENTRY(int235);             /* Programmable. */
     ENTRY(int236);             /* Programmable. */
     ENTRY(int237);             /* Programmable. */
     ENTRY(int238);             /* Programmable. */
     ENTRY(int239);             /* Programmable. */
     ENTRY(int240);             /* Programmable. */
     ENTRY(int241);             /* Programmable. */
     ENTRY(int242);             /* Programmable. */
     ENTRY(int243);             /* Programmable. */
     ENTRY(int244);             /* Programmable. */
     ENTRY(int245);             /* Programmable. */
     ENTRY(int246);             /* Programmable. */
     ENTRY(int247);             /* Programmable. */
     ENTRY(int248);             /* Programmable. */
     ENTRY(int249);             /* Programmable. */
     ENTRY(int250);             /* Programmable. */
     ENTRY(int251);             /* Programmable. */
     ENTRY(int252);             /* Programmable. */
     ENTRY(int253);             /* Programmable. */
     ENTRY(int254);             /* Programmable. */
     ENTRY(int255);             /* Programmable. */

     return;
}
