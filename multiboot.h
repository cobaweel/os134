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
 * Contains information about GNU Multiboot that cannot be included in
 * assembly language, and thus has to be in a separate file. This
 * header can be included form C and assembly files.
 */

#ifndef _MULTIBOOT_H
#define _MULTIBOOT_H

/*
 * Some constants from the GNU Multiboot specification.  
 * http://www.gnu.org/software/grub/manual/multiboot/multiboot.html
 */

/* Magic number in header. */
#define MULTIBOOT_H_MAGIC (0x1BADB002)

/* Magic number passed to kernel. */
#define MULTIBOOT_K_MAGIC (0x2BADB002)

/* Align loaded modules on page boundaries. */
#define MULTIBOOT_ALIGN (1<<0)

/* Provide memory map to kernel. */
#define MULTIBOOT_MEMINFO (1<<1)

/* Provide video info. */
#define MULTIBOOT_VIDINFO (1<<2)

/* Specify load addresses explicitly in Multiboot header. */
#define MULTIBOOT_EXPLICIT (1<<16)

/* Graphics modes. */
#define MULTIBOOT_GR_LINEAR (0)
#define MULTIBOOT_GR_EGA (1)

/* Where the magic number goes. */
#define MULTIBOOT_MAGIC_LOCATION "eax"

/* Where the info struct goes. */
#define MULTIBOOT_INFO_LOCATION "ebx"


#ifdef C_CODE

/* The info struct we get passed by the Multiboot loader. */
struct multiboot_info {
     unsigned flags;              /* 0 */
     unsigned mem_lower;          /* 4 */
     unsigned mem_upper;          /* 8 */
     unsigned boot_device;        /* 12 */
     unsigned cmdline;            /* 16 */
     unsigned mods_count;         /* 20 */
     unsigned mods_addr;          /* 24 */
     unsigned syms[3];            /* 28, 32, 36, 40 */
     unsigned mmap_length;        /* 44 */
     unsigned mmap_addr;          /* 48 */
     unsigned drives_length;      /* 52 */
     unsigned drives_addr;        /* 56 */
     unsigned config_table;       /* 60 */
     unsigned boot_loader_name;   /* 64 */
     unsigned apm_table;          /* 68 */
     unsigned vbe_control_info;   /* 72 */
     unsigned vbe_mode_info;      /* 76 */
     short vbe_mode;              /* 80 */
     short vbe_seg;               /* 82 */
     unsigned vbe_interface_off;  /* 84 */
     unsigned vbe_interface_len;  /* 86 */
} __attribute__((packed));

#endif /* C_CODE */

#endif /* _MULTIBOOT_H */
