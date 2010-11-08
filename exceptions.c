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
 * Handle the i386's exceptions.
 */                  

#include "std.h"
#include "exceptions.h"
#include "interrupt.h"
#include "term.h"

void divide_by_zero(interrupt_error_code errno __attribute__((unused)))
{
     print("Divide_by_zero.\n");
}

void debug (interrupt_error_code errno __attribute__((unused)))
{
     print("Debug.\n");
}

void non_maskable_interrupt(interrupt_error_code errno __attribute__((unused)))
{
     print("Non_maskable_interrupt.\n");
}

void breakpoint(interrupt_error_code errno __attribute__((unused)))
{
     print("Breakpoint.\n");
}

void into_detected_overflow(interrupt_error_code errno __attribute__((unused)))
{
     print("Into_detected_overflow.\n");
}

void out_of_bounds(interrupt_error_code errno __attribute__((unused)))
{
     print("Out_of_bounds.\n");
}

void invalid_opcode(interrupt_error_code errno __attribute__((unused)))
{
     print("Invalid_opcode.\n");
}

void no_coprocessor(interrupt_error_code errno __attribute__((unused)))
{
     print("No_coprocessor.\n");
}

void double_fault(interrupt_error_code errno __attribute__((unused)))
{
     print("Double_fault.\n");
}

void coprocessor_segment_overrun(interrupt_error_code errno __attribute__((unused)))
{
     print("Coprocessor_segment_overrun.\n");
}

void bad_tss(interrupt_error_code errno __attribute__((unused)))
{
     print("Bad_tss.\n");
}

void segment_not_present(interrupt_error_code errno __attribute__((unused)))
{
     print("Segment_not_present.\n");
}

void stack_fault(interrupt_error_code errno __attribute__((unused)))
{
     print("Stack_fault.\n");
}

void general_protection_fault(interrupt_error_code errno __attribute__((unused)))
{
     print("General_protection_fault.\n");
}

void page_fault(interrupt_error_code errno __attribute__((unused)))
{
     print("Page_fault.\n");
}

void unknown_interrupt(interrupt_error_code errno __attribute__((unused)))
{
     print("Unknown_interrupt.\n");
}

void coprocessor_fault(interrupt_error_code errno __attribute__((unused)))
{
     print("Coprocessor_fault.\n");
}

void alignment_check(interrupt_error_code errno __attribute__((unused)))
{
     print("Alignment_check.\n");
}

void machine_check(interrupt_error_code errno __attribute__((unused)))
{
     print("Machine_check.\n");
}

void reserved(interrupt_error_code errno __attribute__((unused)))
{
     print("Reserved.\n");
}

general_error exceptions_init(void)
{
     interrupt_install_handler(0, &divide_by_zero);     
     interrupt_install_handler(1, &debug);
     interrupt_install_handler(2, &non_maskable_interrupt); 
     interrupt_install_handler(3, &breakpoint);
     interrupt_install_handler(4, &into_detected_overflow);
     interrupt_install_handler(5, &out_of_bounds);
     interrupt_install_handler(6, &invalid_opcode);
     interrupt_install_handler(7, &no_coprocessor);
     interrupt_install_handler(8, &double_fault);
     interrupt_install_handler(9, &coprocessor_segment_overrun);
     interrupt_install_handler(10, &bad_tss);
     interrupt_install_handler(11, &segment_not_present);
     interrupt_install_handler(12, &stack_fault);
     interrupt_install_handler(13, &general_protection_fault);
     interrupt_install_handler(14, &page_fault);
     interrupt_install_handler(15, &unknown_interrupt);
     interrupt_install_handler(16, &coprocessor_fault);
     interrupt_install_handler(17, &alignment_check);
     interrupt_install_handler(18, &machine_check);
     interrupt_install_handler(19, &reserved);
     interrupt_install_handler(20, &reserved);
     interrupt_install_handler(21, &reserved);
     interrupt_install_handler(22, &reserved);
     interrupt_install_handler(23, &reserved);
     interrupt_install_handler(24, &reserved);
     interrupt_install_handler(25, &reserved);
     interrupt_install_handler(26, &reserved);
     interrupt_install_handler(27, &reserved);
     interrupt_install_handler(28, &reserved);
     interrupt_install_handler(29, &reserved);
     interrupt_install_handler(30, &reserved);
     interrupt_install_handler(31, &reserved);

     return GENERAL_OK;
}

