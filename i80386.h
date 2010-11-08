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
 * Contains information about the i386 chip. This should not contain
 * any particular "design choice" values, just a generic description
 * of the i386. This header can be included from C and assembly files.
 */

#ifndef _I386_H
#define _I386_H

#ifdef C_CODE
#include "std.h"

/*
 * Set interrupts.
 */ 
void sti(void);

/*
 * Clear interrupts.
 */
void cli(void);

/*
 * Output to I/O port.
 */
void outb(unsigned short int port, unsigned char byte);
void outw(unsigned short int port, unsigned short byte);
void outl(unsigned short int port, unsigned int byte);

unsigned char inb(unsigned short int port);
unsigned short inw(unsigned short int port);
unsigned int inl(unsigned short int port);

void sys(void);
void sys2(void);

/*
 * The format of an i386 segment descriptor. It has the following,
 * supremely bizarre layout. More information is on p. 11-12 of the
 * Pentium manual, Vol. 3. From bkerndev:
 *
 * info1:
 *
 * | 7  |6   5  |4  |3   2   1   0   | P: Segment present?
 * |---------------------------------- DPL: Ring
 * | P  |DPL    |S  | type           | S: System segmt (0); code/data segmt (1)
 *                                     type: 4 bits described later
 * info2:
 * 
 *  |7  |6  |5  |4  |3   2   2   0|  G: 1 byte granularity (0) or 4 kB (1)
 *  |-----------------------------|  D: Operand size: 16bit(0) or 32-bit(1)
 *  |G  |D  |S  |A  |limit_16_19  |  0: MUST BE 0 (DEFINED BY INTEL)
 *                                   A: "Available for System" (set to 0)
 *
 * The least significant bit of type is the "accessed" bit which is
 * set whenever the segment is accessed; it is unimportant to us.  The
 * others are defined (Intel p.11-14):
 * 000=readonly, 001=read/write, 010=ro&expanddown, 011=rw&expanddown
 * 100=execonly, 101=exec/read,  110=xo&conforming, 111=xr&conforming
 * The conforming and expanddown properties are irrelevant to us.
 */
struct i386_segment_descriptor {
     unsigned short limit_0_15;
     unsigned short base_0_15;
     unsigned char  base_16_23;
     unsigned char  seginfo1;
     unsigned char  seginfo2;
     unsigned char  base_24_31;
} __attribute__((packed));
typedef struct i386_segment_descriptor i386_segment_descriptor;

/*
 * The type i386_bounded_pointer is used only by the 386's descriptor
 * table load instructions. It contains both a 32-bit pointer and the
 * size of the table it is pointing to.
 */
struct i386_bounded_pointer {
     unsigned short limit;
     void *base;
} __attribute__((packed));
typedef struct i386_bounded_pointer i386_bounded_pointer;

/* 
 * This is how the fields are laid out.
 */
#define I386_SEGINFO1_PRESENT            0x80 /* Better be on. */
#define I386_SEGINFO1_RING0              0x00 /* Most privileged. */
#define I386_SEGINFO1_RING1              0x20 /* Less privileged. */
#define I386_SEGINFO1_RING2              0x40 /* Even less privileged. */
#define I386_SEGINFO1_RING3              0x60 /* Least privileged. */
#define I386_SEGINFO1_SYSTEM             0x00 /* "System" segment. */
#define I386_SEGINFO1_DATA_CODE          0x10 /* "Data/code" segment. */
#define I386_SEGINFO1_RO                 0x00 /* Read only. */
#define I386_SEGINFO1_RW                 0x02 /* Read/write. */
#define I386_SEGINFO1_RO_EXP             0x04 /* Read/only, expand. */
#define I386_SEGINFO1_RW_EXP             0x06 /* Read/write, expand. */
#define I386_SEGINFO1_XO                 0x08 /* Execute only. */
#define I386_SEGINFO1_XR                 0x0A /* Execute/read. */
#define I386_SEGINFO1_XO_CON             0x0C /* Execute only, conforming. */
#define I386_SEGINFO1_XR_CON             0x0E /* Execute/read, conforming. */
#define I386_SEGINFO2_OP16               0x00 /* 16 bit operands. */
#define I386_SEGINFO2_OP32               0x40 /* 32 bit operands. */
#define I386_SEGINFO2_BYTE               0x00 /* Specify size in bytes. */
#define I386_SEGINFO2_PAGE               0x80 /* Specify size in 4K pages. */
#define I386_SEGINFO2_LIMIT(x)           ((x)&0x0F)
#define I386_SEGBASE_0_15(x)             (((x)&0x0000ffff) >> 0)
#define I386_SEGBASE_16_23(x)            (((x)&0x00ff0000) >> 16)
#define I386_SEGBASE_24_31(x)            (((x)&0xff000000) >> 24)
#define I386_SEGLIMIT_0_15(x)            (((x)&0x0000ffff) >> 0)
#define I386_SEGLIMIT_16_19(x)           (((x)&0x000f0000) >> 16)

/*
 * Load the GDT pointer and the segment registers. There is
 * necessarily some hardcodedness in this; check segments.h. I
 * apologize for the awful unmodularity.
 */         
void load_gdt(i386_bounded_pointer* gdtp);

struct i386_interrupt_descriptor {
     unsigned short handler_0_15;
     unsigned short selector;
     unsigned char  info1;
     unsigned char  info2;
     unsigned short handler_16_31;
};

typedef struct i386_interrupt_descriptor i386_interrupt_descriptor;
/*
 * This is how the fields are laid out.
 */
#define I386_INTHANDLER_0_15(x)   (((x) & 0x0000ffff) >> 0)
#define I386_INTHANDLER_16_31(x)  (((x) & 0xffff0000) >> 16)
#define I386_INTINFO1             0x00 /* Magic. */
#define I386_INTINFO2             0x0E /* Magic. */
#define I386_INTINFO2_PRESENT     0x80 /* Better be on. */
#define I386_INTINFO2_RING0       0x00 /* Most privileged. */
#define I386_INTINFO2_RING1       0x20
#define I386_INTINFO2_RING2       0x40
#define I386_INTINFO2_RING3       0x60 /* Least privileged. */

/*
 * Load the IDT pointer.
 */         
void load_idt(i386_bounded_pointer* idtp);



#endif /* C_CODE */

#endif /* _I386_H */
