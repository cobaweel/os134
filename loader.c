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
 * Loader. loads an a.out into the memory.
 */

/* The starting address for the text (code) section - 64K of code. */
#define I_ADDRESS 0x600000
/* The starting address for the data section - 64K of data. */
#define D_ADDRESS 0x800000


typedef unsigned short uint16_t;
typedef unsigned long uint32_t;

typedef struct {
    uint16_t magic __PACKED__;	/* Magic number. */
    uint16_t version __PACKED__; /* Version number. */
    uint32_t code_size __PACKED__; /* Text segment size. */
    uint32_t data_size __PACKED__; /* Initialized data size. */
    uint32_t bss_size __PACKED__; /* Uninitialized data size. */
    uint32_t syms_size __PACKED__;
    uint32_t entry __PACKED__;
    uint32_t code_offset __PACKED__;
    uint32_t data_offset __PACKED__;
} aout_head_t;			/* 28 bytes */



/*
 * Since the OS does not have File I/O code yet, we need an image
 * pointer that points to the file I have not figured out a way to do
 * that yet, one reason is that we have no file system in the OS yet.
 */

/* 
 * Entry is the entry point of the program. 
 */
int load_aout(char *filename, unsigned char *image, unsigned *entry)
{

/*
 * Load the a.out format file filename.
 *
 * Read the text segment from the file to I_ADDRESS.
 *
 * Read the data segment from the file to D_ADDRESS. Zero out the BSS segment.
 *
 * Create and map in a stack segment (usually separate from the data
 * segment, since the data heap and stack grow separately.) Place
 * arguments from the command line or calling program on the stack.
 *
 * Set registers appropriately and jump to the starting address.
 */
    aout_head_t *aout;

    /* Validate headers. */
    aout = (aout_head_t *) image;
    image += sizeof(aout_head_t);	/* Move to the code section. */


    /* Get entry point. */
    (*entry) = aout->entry;

    /* Load text to I_ADDRESS.
     *
     * Copy aout->code_size bytes of code starting at image +
     * code_offset to I_ADDRESS. */
    image += aout->code_size;

    /* Load DATA to D_ADDRESS.
     *
     * Copy aout->data_size bytes of code starting at image +
     * data_offset to D_ADDRESS. */
    image += aout->data_size;

    /* Set uninitialized data to 0. */
    /* Copy bss_size bytes of 0 starting at D_ADDRESS + aout->data_size. */

}
