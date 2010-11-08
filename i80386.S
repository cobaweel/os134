#include "i80386.h"
#include "segments.h"

.macro  begin 
        pushl   %ebp
        movl    %esp, %ebp
.endm

.macro end
        movl    %ebp, %esp
        popl    %ebp
        ret
.endm                

#define ARG(i) (4*(i+2))(%ebp)
        
.globl sti
sti:    /* Set interrupts. */
        begin
        sti
        end

.globl cli
cli:    /* Clear interrupts. */
        begin
        cli
        end

.globl outb
outb:   
        begin
        movw    ARG(0), %dx
        movb    ARG(1), %al        
        outb    %al, %dx
        outb    %al, $0x80
        end

.globl outw
outw:   
        begin
        movw    ARG(0), %dx
        movw    ARG(1), %ax
        outw    %ax, %dx
        end

.globl outl
outl:   
        begin
        movw    ARG(0), %dx
        movl    ARG(1), %eax
        outl    %eax, %dx
        end

.globl inb
inb:   
        begin
        movw    ARG(0), %dx
        inb    %dx, %al
        end

.globl inw
inw:   
        begin
        movw    ARG(0), %dx
        inw    %dx, %ax
        end

.globl inl
inl:   
        begin
        movw    ARG(0), %dx
        inl    %dx, %eax
        end

        
.globl sys
sys:
        begin
        int $0x80
        end

.globl sys2
sys2:
        begin
        int $0x3
        end        
                                                        
.globl load_gdt
load_gdt: /* Load GDT. */
        begin
        movl    ARG(0), %eax    /* Pointer to GDT. */
        lgdt    (%eax)          /* Load GDT. */
        pushw   $I386_DATA_SEGMENT_DESCRIPTOR_OFFSET
        popw    %ds
        pushw   $I386_DATA_SEGMENT_DESCRIPTOR_OFFSET
        popw    %es
        pushw   $I386_DATA_SEGMENT_DESCRIPTOR_OFFSET
        popw    %fs
        pushw   $I386_DATA_SEGMENT_DESCRIPTOR_OFFSET
        popw    %gs
        ljmp    $I386_CODE_SEGMENT_DESCRIPTOR_OFFSET, $code_segment_jump
code_segment_jump:              
        end

.globl load_idt
load_idt: /* Load IDT. */
        begin
        movl    ARG(0), %eax    /* Pointer to IDT. */
        lidt    (%eax)          /* Load IDT. */
        end

/* EMPTY LINE. */
