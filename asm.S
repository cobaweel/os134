/* OS134, Copyright (C) 2005, Benjamin Stein, Jaap Weel, Ting Liao --
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
nnthe Free Software Foundation; either version 2 of the License, or (at
your option) any later version.  This program is distributed in the
hope that it will be useful, but WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU General Public License for more details.  You
should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA */

/*
 * Assembly language routines for the extremely primitive "kernel".
 */

/*
 * This file is .S not .s so it will run through the C preprocessor.
 */                 
        
.global _loader

/*        
 * We follow the Multiboot specification, which involves the
 * following magic header. Because we are generating an ELF kernel,
 * the information about where to load the kernel is in the ELF header,
 * not the Multiboot header. It is specified in the linker script.
 */         

#include "multiboot.h"
multiboot_header:       
.align 4
.set FLAGS, MULTIBOOT_ALIGN | MULTIBOOT_MEMINFO
.long MULTIBOOT_H_MAGIC         # Multiboot magic number.
.long FLAGS                     # Multiboot flags.
.long -(MULTIBOOT_H_MAGIC + FLAGS) # Multiboot checksum.

/*
 * Reserve initial kernel stack space
 */
_loader:
        mov     8(%ebx), %esp   # Stack at very end of memory
        and     $0xfffffff0, %esp # Align stack
        call    _main           # Call kernel proper.
        hlt                     # Halt machine.
