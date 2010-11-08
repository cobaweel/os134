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
 * Interface to the extremely primitive VGA driver.
 */

#ifndef _VGA_H
#define _VGA_H

#include "std.h"

/* 
 * Errors in the VGA driver.
 */
enum vga_error { VGA_OK, VGA_UNINITIALIZED, VGA_OUTOFBOUNDS, VGA_PANIC };
typedef enum vga_error vga_error;     

/* 
 * Colors on the VGA screen.
 */
enum vga_color_component {
     VGA_BLACK, VGA_BLUE, VGA_GREEN, VGA_CYAN, VGA_RED, VGA_MAGENTA,
     VGA_BROWN, VGA_LIGHT_GREY, 
     VGA_DARK_GREY, VGA_LIGHT_BLUE, VGA_LIGHT_GREEN, VGA_LIGHT_CYAN, 
     VGA_LIGHT_RED, VGA_LIGHT_MAGENTA, VGA_LIGHT_BROWN, VGA_WHITE
};
#define VGA_COLOR(fore,back) ((back << 4) | (fore & 0x0F))
typedef char vga_color;

/* Some alternative spellings. */
#define VGA_LIGHT_GRAY VGA_LIGHT_GREY
#define VGA_DARK_GRAY VGA_DARK_GREY
#define VGA_COLOUR VGA_COLOR
#define vga_colour vga_color


/* 
 * Entry points into the VGA driver.
 */
vga_error vga_init(array_char vga);
vga_error vga_putchr(char  chr, unsigned dest, vga_color color);
vga_error vga_getchr(unsigned src, char* chr);
vga_error vga_getclr(unsigned src, vga_color* clr);
vga_error set_cursor(unsigned dest);

/*
 * The "CRT Controller" registers.
 */
#define CRT_ADDRESS    0x3d4
#define CRT_DATA       0x3d5
#define CRT_HORIZ_TOTAL  0x00       /* Horizontal Total Register. */
#define CRT_END_HORIZ_DISP  0x01 /* End Horizontal Display Register. */
#define CRT_START_HORIZ_BLANK  0x02 /* Start Horizontal Blanking Register. */
#define CRT_END_HORIZ_BLANK  0x03 /* End Horizontal Blanking Register. */
#define CRT_START_HORIZ_RETRACE  0x04 /* Start Horizontal Retrace Register. */
#define CRT_END_HORIZ_RETRACE  0x05 /* End Horizontal Retrace Register. */
#define CRT_VERT_TOTAL  0x06        /* Vertical Total Register. */
#define CRT_OVERFLOW  0x07          /* Overflow Register. */
#define CRT_PRESET_ROWSCAN  0x08    /* Preset Row Scan Register. */
#define CRT_MAX_SCANLINE  0x09      /* Maximum Scan Line Register. */
#define CRT_CURSOR_START  0x0A      /* Cursor Start Register. */
#define CRT_CURSOR_END  0x0B        /* Cursor End Register. */
#define CRT_START_HI  0x0C          /* Start Address High Register. */
#define CRT_START_LO  0x0D          /* Start Address Low Register. */
#define CRT_CURSOR_LOC_HI  0x0E   /* Cursor Location High Register. */
#define CRT_CURSOR_LOC_LO  0x0F    /* Cursor Location Low Register. */
#define CRT_VERT_RETRACE_START  0x10 /* Vertical Retrace Start Register. */
#define CRT_VERT_RETRACE_END  0x11 /* Vertical Retrace End Register. */
#define CRT_VERT_DISP_END  0x12   /* Vertical Display End Register. */
#define CRT_OFFSET  0x13            /* Offset Register. */
#define CRT_UNDERLINE_LOC  0x14     /* Underline Location Register. */
#define CRT_START_VERT_BLANK  0x15 /* Start Vertical Blanking Register. */
#define CRT_END_VERT_BLANK  0x16    /* End Vertical Blanking. */
#define CRT_CRTC_MODE_CTL  0x17     /* CRTC Mode Control Register. */
#define CRT_LINE_COMPARE  0x18      /* Line Compare Register. */


/*
 * VGA Low-level documentation at:
 *
 *  http://www.osdever.net/FreeVGA/home.htm
 *
 * These registers are not used here, but I'll leave the definitions
 * in.
 */

/* 
 * VGA General Ports
 */
#define VGA_ADDR 0x3CE          /* Address port for general. */
#define VGA_DATA 0x3CF          /* Data port for general. */

/*
 * VGA register numbers.
 */
#define VGA_SETRESET          0x00 /* Set/Reset Register */
#define VGA_ENABLE_SETRESET   0x01 /* Enable Set/Reset Register */
#define VGA_COLOR_COMPARE     0x02 /* Color Compare Register */
#define VGA_DATA_ROTATE       0x03 /* Data Rotate Register */
#define VGA_READ_MAP_SELECT   0x04 /* Read Map Select Register */
#define VGA_GRAPHICS_MODE     0x05 /* Graphics Mode Register */
#define VGA_MISC_GRAPHICS     0x06 /* Miscellaneous Graphics Register */
#define VGA_COLOR_DONT_CARE   0x07 /* Color Don't Care Register */
#define VGA_BIT_MASK          0x08 /* Bit Mask Register */

/*
 * VGA "Attribute" ports.
 */
#define VGA_ATTR_ADDR  0x3C0    /* Address port for attribute. */
#define VGA_ATTR_DATA  0x3C1    /* Data port for attribute. */

/*
 * VGA "Sequencer" ports.
 */
#define VGA_SEQ_ADDR      0x3C4 /* Address port for sequencer. */
#define VGA_SEQ_DATA      0x3C5 /* Data port for sequencer. */
#define VGA_RESET         0x00  /* Reset Register. */
#define VGA_CLOCKING_MODE 0x01  /* Clocking Mode Register. */
#define VGA_MAP_MASK      0x02  /* Map Mask Register. */
#define VGA_CHARMAP       0x03  /* Character Map Select Register. */
#define VGA_SEQUENCER_MEM 0x04  /* Sequencer Memory Mode Register. */

/*
 * VGA "Color" registers.
 */
#define VGA_DAC_ADDR_WM 0x3C8   /* DAC Address Write Mode Register. */
#define VGA_DAC_ADDR_RM 0x3C7   /* DAC Address Read Mode Register. */
#define VGA_DAC_DATA    0x3C9   /* DAC Data Register. */
#define VGA_DAC_STATE 0x3C7     /* DAC State Register. */

/*
 * VGA "External" registers.
 */




 

#endif /* _VGA_H */
