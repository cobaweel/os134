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
 * i8259A programmable interrupt controller (PIC). Documented in the
 * 8259A/8259A-2 datasheet, order number 231468-003 from Intel.
 */

#include "std.h"
#include "i80386.h"
#include "interrupt.h"
#include "term.h"
#include "i8259.h"

void irq0_handler(interrupt_error_code n);
void irq1_handler(interrupt_error_code n);
void irq2_handler(interrupt_error_code n);
void irq3_handler(interrupt_error_code n);
void irq4_handler(interrupt_error_code n);
void irq5_handler(interrupt_error_code n);
void irq6_handler(interrupt_error_code n);
void irq7_handler(interrupt_error_code n);
void irq8_handler(interrupt_error_code n);
void irq9_handler(interrupt_error_code n);
void irq10_handler(interrupt_error_code n);
void irq11_handler(interrupt_error_code n);
void irq12_handler(interrupt_error_code n);
void irq13_handler(interrupt_error_code n);
void irq14_handler(interrupt_error_code n);
void irq15_handler(interrupt_error_code n);
void eoi(char);
void default_irq_handler(void);
void initialize_irq_handler_table(void);

make_array(irq_handler, irq_handler_table, 16);

general_error irq_init(void)
{
     /* Send both PICs initialization word ICW1. */
     outb(IRQ_MASTER | IRQ_CMD, IRQ_ICW1 | IRQ_ICW4_COMING);
     outb(IRQ_SLAVE | IRQ_CMD, IRQ_ICW1 | IRQ_ICW4_COMING);

     /* Send both PICs remapping information. */
     outb(IRQ_MASTER | IRQ_DATA, IRQ_REMAP_MASK & 0x20);
     outb(IRQ_SLAVE | IRQ_DATA, IRQ_REMAP_MASK & 0x2F);

     /* Tell both PICs where the slave is mapped.  */
     outb(IRQ_MASTER | IRQ_DATA, IRQ_SLAVE_IS2);
     outb(IRQ_SLAVE | IRQ_DATA, IRQ_SLAVEID_2);

     /* Send both PICs some more configuration data.  */
     outb(IRQ_MASTER | IRQ_DATA, (IRQ_NONBUFFERED |
                                  IRQ_NORMAL_EOI |
                                  IRQ_8086_MODE));
     outb(IRQ_SLAVE | IRQ_DATA, (IRQ_NONBUFFERED |
                                 IRQ_NORMAL_EOI |
                                 IRQ_8086_MODE));
     
     /* Turn on only the keyboard IRQ. */
     outb(IRQ_MASTER | IRQ_DATA, ~IRQ_MASTER_IRQ(IRQ_KEYBOARD));
     outb(IRQ_SLAVE | IRQ_DATA, ~IRQ_SLAVE_IRQ(IRQ_KEYBOARD));

     /* Install handlers for all IRQs. In principle, whenever an IRQ
      * is turned on by a driver, it should also install a handler,
      * but who knows, maybe it won't. This way, we are not stuck with
      * the very generic unhandled-interrupt routine, but something
      * slightly more infomative. */
     interrupt_install_handler(0x20, &irq0_handler);
     interrupt_install_handler(0x21, &irq1_handler);
     interrupt_install_handler(0x22, &irq2_handler);
     interrupt_install_handler(0x23, &irq3_handler);
     interrupt_install_handler(0x24, &irq4_handler);
     interrupt_install_handler(0x25, &irq5_handler);
     interrupt_install_handler(0x26, &irq6_handler);
     interrupt_install_handler(0x27, &irq7_handler);
     interrupt_install_handler(0x28, &irq8_handler);
     interrupt_install_handler(0x29, &irq9_handler);
     interrupt_install_handler(0x2a, &irq10_handler);
     interrupt_install_handler(0x2b, &irq11_handler);
     interrupt_install_handler(0x2c, &irq12_handler);
     interrupt_install_handler(0x2d, &irq13_handler);
     interrupt_install_handler(0x2e, &irq14_handler);
     interrupt_install_handler(0x2f, &irq15_handler);

     /* Now install the individual handlers. */
     unsigned int i;
     for (i = 0; i < 16; i++)
          install_irq_handler(i, &default_irq_handler);

     return GENERAL_OK;
}

void eoi(char whichpic)
{
     outb(whichpic | IRQ_CMD, IRQ_OCW2_NONSPECIFIC_EOI);
}

array_error install_irq_handler (irq_t irq, irq_handler handler)
{
     return aset(irq_handler_table,irq,handler);
}

void default_irq_handler(void)
{
     print("Unhandled IRQ.\n");
     /* Do nothing. */
}

void irq0_handler(interrupt_error_code n __attribute__((unused)))
{
     eoi(IRQ_MASTER);
     irq_handler current_irq_handler;
     aref(irq_handler_table,0,current_irq_handler);
     (*current_irq_handler)();
}

void irq1_handler(interrupt_error_code n __attribute__((unused)))
{
     eoi(IRQ_MASTER);
     irq_handler current_irq_handler;
     aref(irq_handler_table,1,current_irq_handler);
     (*current_irq_handler)();
}

void irq2_handler(interrupt_error_code n __attribute__((unused)))
{
     eoi(IRQ_MASTER);
     irq_handler current_irq_handler;
     aref(irq_handler_table,2,current_irq_handler);
     (*current_irq_handler)();
}

void irq3_handler(interrupt_error_code n __attribute__((unused)))
{
     eoi(IRQ_MASTER);
     irq_handler current_irq_handler;
     aref(irq_handler_table,3,current_irq_handler);
     (*current_irq_handler)();
}

void irq4_handler(interrupt_error_code n __attribute__((unused)))
{
     eoi(IRQ_MASTER);
     irq_handler current_irq_handler;
     aref(irq_handler_table,4,current_irq_handler);
     (*current_irq_handler)();
}

void irq5_handler(interrupt_error_code n __attribute__((unused)))
{
     eoi(IRQ_MASTER);
     irq_handler current_irq_handler;
     aref(irq_handler_table,5,current_irq_handler);
     (*current_irq_handler)();
}

void irq6_handler(interrupt_error_code n __attribute__((unused)))
{
     eoi(IRQ_MASTER);
     irq_handler current_irq_handler;
     aref(irq_handler_table,6,current_irq_handler);
     (*current_irq_handler)();
}

void irq7_handler(interrupt_error_code n __attribute__((unused)))
{
     eoi(IRQ_MASTER);
     irq_handler current_irq_handler;
     aref(irq_handler_table,7,current_irq_handler);
     (*current_irq_handler)();
}

void irq8_handler(interrupt_error_code n __attribute__((unused)))
{
     eoi(IRQ_MASTER);
     eoi(IRQ_SLAVE);
     irq_handler current_irq_handler;
     aref(irq_handler_table,8,current_irq_handler);
     (*current_irq_handler)();
}

void irq9_handler(interrupt_error_code n __attribute__((unused)))
{
     eoi(IRQ_MASTER);
     eoi(IRQ_SLAVE);
     irq_handler current_irq_handler;
     aref(irq_handler_table,9,current_irq_handler);
     (*current_irq_handler)();
}

void irq10_handler(interrupt_error_code n __attribute__((unused)))
{
     eoi(IRQ_MASTER);
     eoi(IRQ_SLAVE);
     irq_handler current_irq_handler;
     aref(irq_handler_table,10,current_irq_handler);
     (*current_irq_handler)();
}

void irq11_handler(interrupt_error_code n __attribute__((unused)))
{
     eoi(IRQ_MASTER);
     eoi(IRQ_SLAVE);
     irq_handler current_irq_handler;
     aref(irq_handler_table,11,current_irq_handler);
     (*current_irq_handler)();
}

void irq12_handler(interrupt_error_code n __attribute__((unused)))
{
     eoi(IRQ_MASTER);
     eoi(IRQ_SLAVE);
     irq_handler current_irq_handler;
     aref(irq_handler_table,12,current_irq_handler);
     (*current_irq_handler)();
}

void irq13_handler(interrupt_error_code n __attribute__((unused)))
{
     eoi(IRQ_MASTER);
     eoi(IRQ_SLAVE);
     irq_handler current_irq_handler;
     aref(irq_handler_table,13,current_irq_handler);
     (*current_irq_handler)();
}

void irq14_handler(interrupt_error_code n __attribute__((unused)))
{
     eoi(IRQ_MASTER);
     eoi(IRQ_SLAVE);
     irq_handler current_irq_handler;
     aref(irq_handler_table,14,current_irq_handler);
     (*current_irq_handler)();
}

void irq15_handler(interrupt_error_code n __attribute__((unused)))
{
     eoi(IRQ_MASTER);
     eoi(IRQ_SLAVE);
     irq_handler current_irq_handler;
     aref(irq_handler_table,15,current_irq_handler);
     (*current_irq_handler)();
}
