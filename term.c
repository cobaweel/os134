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
 * Terminal driver. Built on top of the VGA driver to provide a more
 * stateful behavior.
 */

#include "vga.h"
#include "term.h"
#include "i8042.h"
#include "kb.h"

#define ROWS 25
#define COLS 80

/*
 * Variable global state connected to the terminal.
 */
vga_color current_color = 20;   /* Current color. */
unsigned pos = 0;               /* The point the curor is at. */

#define ROW (pos / COLS)
#define COL (pos % COLS)

/*
 * Put any intialization here.
 */
term_error term_init(vga_color color)
{
     /* Set the color. */
     current_color = color;

     /* Clear the screen. */
     if (cls() == TERM_TROUBLE) {
       print("Failed to clear screen.\n");
       return TERM_TROUBLE;
     }

     pos = 0;
     set_cursor(pos);

     return TERM_OK;
}

/*
 * Scroll screen one line up. Do not change position. 
 */
void scroll(void)
{
     char c;                    /* Character to move. */
     vga_color l;               /* Color of that character. */
     unsigned int i;            /* Loop variable. */

     /* Move all lines up one line. */
     for (i = ROWS; i < ROWS * COLS; i++) {
          vga_getchr(i, &c);
          vga_getclr(i, &l);
          vga_putchr(c, i - COLS, l);
     }

     /* Clear the last line. */
     for (i = (ROWS - 1) * COLS; i < ROWS * COLS; i ++)
          vga_putchr(' ', i, current_color);
}

/*
 * Print a character.
 *  chr: character to print
 */
term_error printc(char chr)
{
     unsigned int newpos;
     switch (chr) {
     case '\n':
          /* Newline; fill rest of line. */
          if ( COL == 0 ) pos++; /* Edge case. */
          for (; 0 < COL && COL <= COLS ; pos++) {
               switch (vga_putchr(' ', pos, current_color)) {
               case VGA_OK: break;
               case VGA_OUTOFBOUNDS: return VGA_OUTOFBOUNDS; break;
               default: return VGA_PANIC; break;
               }
          }
          break;

     case '\b':
          /* Backspace. Move back a character and delete. */
          switch (vga_putchr(' ', --pos, current_color)) {
          case VGA_OK: break;
          case VGA_OUTOFBOUNDS: return VGA_OUTOFBOUNDS; break;
          default: return VGA_PANIC; break;
          }
          break;

     case '\t':
          /* Tab. Move to the next multiple of 8. */
          newpos = (pos + 8) - ((pos + 8) % 8);
          for (; pos < newpos ; pos++) {
               switch (vga_putchr(' ', pos, current_color)) {
               case VGA_OK: break;
               case VGA_OUTOFBOUNDS: return VGA_OUTOFBOUNDS; break;
               default: return VGA_PANIC; break;
               }
          }
          break;

     default:
          /* Display the character. */
          switch (vga_putchr(chr, pos++, current_color)) {
          case VGA_OK: break;
          case VGA_OUTOFBOUNDS: return VGA_OUTOFBOUNDS; break;
          default: return VGA_PANIC; break;
          }
     }

     if (pos >= ROWS * COLS) {
          scroll();
          pos -= COLS;
     }
     set_cursor(pos);

     return TERM_OK;
}

/*
 * Print a string.
 *  msg: string to be printed
 */
term_error print(char* msg) 
{
     char c;                    /* One character to print. */

     /* Loop through the string. */
     while( (c = *(msg++)) != 0 ) {

          switch (c) {
          case 0:          
               /* Should never get here, since c != 0. */
               return VGA_PANIC;
               break;
	       
          default:
               /* Just print the character. */
               if (printc(c) != TERM_OK) return TERM_TROUBLE;
               break;
          }
     }

     return TERM_OK;
}

/* 
 * Print a number as unsigned hexadecimal.
 *  x: number to be printed
 */
term_error printx(unsigned int x)
{
     int i;

     for (i = 7; i >= 0; i--) {
          /* Print a digit. */
          unsigned char digit = (x >> (i*4)) & 0x0f;
          digit = (digit < 0xa) ? digit + '0' : digit + '7';
          if (printc(digit) != TERM_OK) return TERM_TROUBLE; 
     }
     
     return TERM_OK;
}

/*
 * Print a number as unsigned decimal, or print nothing if number is
 * 0.
 */
term_error printuz(unsigned x)
{
     if ( x > 0 ) {
          if ( printuz(x / 10) != TERM_OK) return TERM_TROUBLE;
          if ( printc(x % 10 + '0') != TERM_OK) return TERM_TROUBLE;
     }

     return TERM_OK;
}

/* 
 * Print a number as unsigned decimal.
 *  d: number to be printed
 */
term_error printu(unsigned x)
{
     if ( x == 0 ) {
          if (printc('0') != TERM_OK) return TERM_TROUBLE;
     } else {
          if (printuz(x) != TERM_OK) return TERM_TROUBLE;
     }

     return TERM_OK;
}

/* 
 * Print a number as signed decimal at the cursor and move the cursor
 * forward.  
 *  d: number to be printed
 */
term_error printd(int x)
{
     /* If x is negative, print a minus sign and reverse sign of  */
     if ( x<0 ) {
          if (printc('-') != TERM_OK) return TERM_TROUBLE;
          x = -x;
     }

     /* Print the rest of x. */
     if (printu(x) != TERM_OK) return TERM_TROUBLE;

     return TERM_OK;
}

/*
 * Clear the terminal.
 */
term_error cls(void)
{
     unsigned int i;            /* Loop counter. */

     /* Print a screen full of spaces. */
     pos = 0;
     set_cursor(pos);

     for (i = 0; i < ROWS * COLS; i++) 
          if (printc(' ') != TERM_OK) return TERM_TROUBLE;

     /* Return the cursor to the beginning. */
     pos = 0;
     set_cursor(pos);

     return TERM_OK;
}

/*
 * Change the current color.
 *  color: color to change it to
 */
term_error color(vga_color color)
{
     current_color = color;

     return TERM_OK;
}

/*
 * Change the current color.
 *  color: color to change it to
 */
term_error getcolor(vga_color* color)
{
     if (color) {
          *color = current_color;
          return TERM_OK;
     } else {
          return TERM_TROUBLE;
     }
}


/*
 * Get key. For keyup events with key value 0, we return the scancode
 * with the MSB set.
 */
key_t getc(void)
{
     static bucky_t bucky_state = 0; /* Current bucky bit state. */
     scancode_t scan = 0;       /* Latest scan code. */
     key_t key;                 /* Latest key code. */

     while (1) {
          /* Get a key from the controller. */
          scan = getkey_blocking();

          /* If the key is a release event, we set the bucky bits, but
           * do not yet return. */
          if ( !ISPRESS(scan) ) {
               /* Get the corresponding release scan code, since that
                * is what the table is indexed by. */
               scan = RELEASE_OF_PRESS(scan);

               /* Maybe clear some bucky bit. */
               bucky_state &= ~(keys[scan].bucky);
          } 

          /* If the key is a press event, we set the bucky bits, and
           * we also return a value. */
          else {
               /* Maybe set some bucky bit. */               
               bucky_state |= keys[scan].bucky;

               /* Return uppercase of lowercase letter. */
               if (bucky_state & KB_SHIFT)  
                    key = keys[scan].shift_key;
               else
                    key = keys[scan].key;

               /* If there is no special keycode defined, we
                * synthesize one from the scan code, with the MSB set
                * to 1. */
               return key ? key : scan | 0x80;
          }
     }
}
