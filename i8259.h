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
 * i8259A programmable interrupt controller (PIC). Docmunted in the
 * 8259A/8259A-2 datasheet, order number 231468-003 from Intel.
 */

#ifndef _I8259_H
#define _I8259_H

general_error irq_init(void);
typedef void (*irq_handler)(void);
array(irq_handler);
typedef unsigned char irq_t;
array_error install_irq_handler (irq_t irq, irq_handler handler);

/*
 * Apparently, it is "standard" for a PC to have two of these, mapped
 * at the following addresses in I/O space:
 */
#define IRQ_MASTER    0x20
#define IRQ_SLAVE     0xA0

/*
 * An iRQ has two registers we can write to. 
 */
#define IRQ_CMD       0x00
#define IRQ_DATA      0x01

/*
 * ICW 1; send this to CMD at any time.
 */
#define IRQ_8085_STUFF  0     /* Only important for 8080/8085. */
#define IRQ_ICW1        0x10  /* This is the ICW1. */
#define IRQ_LEVEL       0x06  /* Level triggered interrupts. */
#define IRQ_EDGE        0x00  /* Edge triggered interrupts. */
#define IRQ_PACKED      0x02  /* 4 byte address interval. */
#define IRQ_LOOSE       0x00  /* 8 byte address interval. */
#define IRQ_ICW4_COMING 0x01  /* There will be an ICW4 too. */

/*
 * ICW 2; send this to DATA right after an ICW1 
 */
#define IRQ_REMAP_MASK  0xF8  /* Mask over remap address. */

/*
 * ICW 3 for Master; send this to DATA right after an ICW2. 
 */
#define IRQ_SLAVE_IS7   0x80  /* Which IRQ is the slave on? */
#define IRQ_SLAVE_IS6   0x40
#define IRQ_SLAVE_IS5   0x20
#define IRQ_SLAVE_IS4   0x10
#define IRQ_SLAVE_IS3   0x08
#define IRQ_SLAVE_IS2   0x04
#define IRQ_SLAVE_IS1   0x02
#define IRQ_SLAVE_IS0   0x01

/*
 * ICW 3 for Slave; send this to DATA right after an ICW2. 
 */
#define IRQ_SLAVEID_0  0      /* Which IRQ is the slave on? */
#define IRQ_SLAVEID_1  1      /* (in a different format) */
#define IRQ_SLAVEID_2  2
#define IRQ_SLAVEID_3  3
#define IRQ_SLAVEID_4  4
#define IRQ_SLAVEID_5  5
#define IRQ_SLAVEID_6  6
#define IRQ_SLAVEID_7  7

/*
 * ICW 4; send this to DATA right after an ICW1,2,3 where ICW1 had
 * ICW4_COMING set in it.
 */
#define IRQ_SPECIAL_FULLY_NESTED  0x10
#define IRQ_NONBUFFERED           0x00
#define IRQ_MASTER_BUFFERED       0x08
#define IRQ_SLAVE_BUFFERED        0x0C
#define IRQ_AUTO_EOI              0x02
#define IRQ_NORMAL_EOI            0x00
#define IRQ_8086_MODE             0x01
#define IRQ_8085_MODE             0x00

/*
 * OCW 1 send this to DATA at any time. Just send the same information
 * to both PICs, the macro will make sure that the right masks will be
 * generated.
 */
#define IRQ_MASTER_IRQ_NONE 0
#define IRQ_MASTER_IRQ(i) (1<<(i))
#define IRQ_SLAVE_IRQ_NONE 0
#define IRQ_SLAVE_IRQ(i) (1<<((i)>>8))

/*
 * OCW 2; send this to CMD at any time.
 */
#define IRQ_OCW2                 0x00 /* Must be present. */  
#define IRQ_OCW2_IRQ_LEVEL_0     0x00 /*  */
#define IRQ_OCW2_IRQ_LEVEL_1     0x01 /*  */
#define IRQ_OCW2_IRQ_LEVEL_2     0x02 /*  */
#define IRQ_OCW2_IRQ_LEVEL_3     0x03 /*  */
#define IRQ_OCW2_IRQ_LEVEL_4     0x04 /*  */
#define IRQ_OCW2_IRQ_LEVEL_5     0x05 /*  */
#define IRQ_OCW2_IRQ_LEVEL_6     0x06 /*  */
#define IRQ_OCW2_IRQ_LEVEL_7     0x07 /*  */
#define IRQ_OCW2_NONSPECIFIC_EOI 0x20 /*  */
#define IRQ_OCW2_SPECIFIC_EOI    0x60 /*  */
/*
 * Rotation commands are not defined as of yet. We do not use
 * them. See the datasheet. 
 */

/*
 * OCW 3; send this to CMD at any time.
 */
#define IRQ_OCW3                 0x08 /* Must be present. */  
/*
 * Not defined at this time. See the datasheet. We don't use this.
 */

/*
 * IRQs in a "standard" AT-compatible box.
 */
#define IRQ_CLOCK             0
#define IRQ_KEYBOARD          1
#define IRQ_CASCADE           2
#define IRQ_SERIAL2           3
#define IRQ_SERIAL1           4
#define IRQ_XT_HDD            5
#define IRQ_FLOPPY            6
#define IRQ_PRINTER           7
#define IRQ_REALTIMECLOCK     8
#define IRQ_FPU_EXCEPTION     13
#define IRQ_AT_HDD            14
#define IRQ(i)                (i)

#endif /* _IRQ_H */
