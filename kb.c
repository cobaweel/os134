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

#include "kb.h"

/*
 * Whenever the keycode is set to 0, the driver will automatically
 * make the key code equal to scan code with the MSB set to 1.
 */
keymap_t us[256] = {
     {0,            0,          0},          /* ? */    
     {27,           27,         0},          /* Escape. */
     {'1',          '!',        0},
     {'2',          '@',        0},
     {'3',          '#',        0},
     {'4',          '$',        0},
     {'5',          '%',        0},
     {'6',          '^',        0},
     {'7',          '&',        0},
     {'8',          '*',        0},
     {'9',          '(',        0},
     {'0',          ')',        0},
     {'-',          '_',        0},
     {'=',          '+',        0},
     {'\b',         '\b',       0},          /* Backspace. */
     {'\t',         '\t',       0},          /* Tab. */
     {'q',          'Q',        0},
     {'w',          'W',        0},
     {'e',          'E',        0},
     {'r',          'R',        0},
     {'t',          'T',        0},
     {'y',          'Y',        0},
     {'u',          'U',        0},
     {'i',          'I',        0},
     {'o',          'O',        0},
     {'p',          'P',        0},
     {'[',          '[',        0},
     {']',          ']',        0},
     {'\n',         '\n',       0},          /* Enter. */
     {0,            0,          KB_CTL_L},   /* Control. */
     {'a',          'A',        0},
     {'s',          'S',        0},
     {'d',          'D',        0},
     {'f',          'F',        0},
     {'g',          'G',        0},
     {'h',          'H',        0},
     {'j',          'J',        0},
     {'k',          'K',        0},
     {'l',          'L',        0},
     {';',          ';',        0},
     {'\'',         '\'',       0},
     {'`',          '`',        0},
     {0,            0,          KB_SHIFT},   /* Left shift. */
     {'\\',         '\\',       0},
     {'z',          'Z',        0},
     {'x',          'X',        0},
     {'c',          'C',        0},
     {'v',          'V',        0},
     {'b',          'B',        0},
     {'n',          'N',        0},
     {'m',          'M',        0},
     {',',          ',',        0},
     {'.',          '.',        0},
     {'/',          '/',        0},
     {0,            0,          KB_SHIFT},   /* Right shift. */
     {'*',          '*',        0},
     {0,            0,          KB_ALT_L},   /* Left alt. */
     {' ',          ' ',        0},
     {0,            0,          0},
     {0,            0,          KB_CTL_L},   /* Caps lock. */
     {0,            0,          0},          /* F1 */
     {0,            0,          0},          /* F2 */
     {0,            0,          0},          /* F3 */
     {0,            0,          0},          /* F4 */
     {0,            0,          0},          /* F5 */
     {0,            0,          0},          /* F6 */
     {0,            0,          0},          /* F7 */
     {0,            0,          0},          /* F8 */
     {0,            0,          0},          /* F9 */
     {0,            0,          0},          /* F10 */
     {0,            0,          0},          /* Num lock. */
     {0,            0,          0},          /* Scroll Lock. */
     {0,            0,          0},          /* Home key. */
     {0,            0,          0},          /* Up Arrow. */
     {0,            0,          0},          /* Page Up. */
     {'-',          '-',        0},          
     {0,            0,          0},          /* Left Arrow */
     {0,            0,          0},
     {'+',          '+',        0},          
     {0,            0,          0},          /* Right Arrow */
     {0,            0,          0},          /* End key*/
     {0,            0,          0},          /* Down Arrow */
     {0,            0,          0},          /* Page Down */
     {0,            0,          0},          /* Insert Key */
     {0,            0,          0},          /* Delete Key */
     {0,            0,          0},          /* Another enter?? */
     {0,            0,          0},          /* ? */
     {0,            0,          0},          /* ? */
     {0,            0,          0},          /* F11 Key */
     {0,            0,          0},          /* F12 Key */
     {0,            0,          0}           /* Other keys undefined. */
};

keymap_t dv[256] = {
     {0,            0,          0},          /* ? */    
     {27,           27,         0},          /* Escape. */
     {'1',          '!',        0},
     {'2',          '@',        0},
     {'3',          '#',        0},
     {'4',          '$',        0},
     {'5',          '%',        0},
     {'6',          '^',        0},
     {'7',          '&',        0},
     {'8',          '*',        0},
     {'9',          '(',        0},
     {'0',          ')',        0},
     {'[',          '_',        0},
     {']',          '+',        0},
     {'\b',         '\b',       0},          /* Backspace. */
     {'\t',         '\t',       0},          /* Tab. */
     {'\'',          'Q',        0},
     {',',          'W',        0},
     {'.',          'E',        0},
     {'p',          'P',        0},
     {'y',          'Y',        0},
     {'f',          'F',        0},
     {'g',          'G',        0},
     {'c',          'C',        0},
     {'r',          'R',        0},
     {'l',          'L',        0},
     {'/',          '[',        0},
     {'=',          ']',        0},
     {'\n',         '\n',       0},          /* Enter. */
     {0,            0,          KB_CTL_L},   /* Control. */
     {'a',          'A',        0},
     {'o',          'O',        0},
     {'e',          'E',        0},
     {'u',          'U',        0},
     {'i',          'I',        0},
     {'d',          'D',        0},
     {'h',          'H',        0},
     {'t',          'T',        0},
     {'n',          'N',        0},
     {'s',          'S',        0},
     {'-',         '\'',       0},
     {'`',          '`',        0},
     {0,            0,          KB_SHIFT},   /* Left shift. */
     {'\\',         '\\',       0},
     {';',          ':',        0},
     {'q',          'Q',        0},
     {'j',          'J',        0},
     {'k',          'K',        0},
     {'x',          'X',        0},
     {'b',          'B',        0},
     {'m',          'M',        0},
     {'w',          'W',        0},
     {'v',          'V',        0},
     {'z',          'Z',        0},
     {0,            0,          KB_SHIFT},   /* Right shift. */
     {'*',          '*',        0},
     {0,            0,          KB_ALT_L},   /* Left alt. */
     {' ',          ' ',        0},
     {0,            0,          0},
     {0,            0,          KB_CTL_L},   /* Caps lock. */
     {0,            0,          0},          /* F1 */
     {0,            0,          0},          /* F2 */
     {0,            0,          0},          /* F3 */
     {0,            0,          0},          /* F4 */
     {0,            0,          0},          /* F5 */
     {0,            0,          0},          /* F6 */
     {0,            0,          0},          /* F7 */
     {0,            0,          0},          /* F8 */
     {0,            0,          0},          /* F9 */
     {0,            0,          0},          /* F10 */
     {0,            0,          0},          /* Num lock. */
     {0,            0,          0},          /* Scroll Lock. */
     {0,            0,          0},          /* Home key. */
     {0,            0,          0},          /* Up Arrow. */
     {0,            0,          0},          /* Page Up. */
     {'-',          '-',        0},          
     {0,            0,          0},          /* Left Arrow */
     {0,            0,          0},
     {'+',          '+',        0},          
     {0,            0,          0},          /* Right Arrow */
     {0,            0,          0},          /* End key*/
     {0,            0,          0},          /* Down Arrow */
     {0,            0,          0},          /* Page Down */
     {0,            0,          0},          /* Insert Key */
     {0,            0,          0},          /* Delete Key */
     {0,            0,          0},          /* Another enter?? */
     {0,            0,          0},          /* ? */
     {0,            0,          0},          /* ? */
     {0,            0,          0},          /* F11 Key */
     {0,            0,          0},          /* F12 Key */
     {0,            0,          0}           /* Other keys undefined. */
};


/*
 * Initialize the keymap.
 */
void kb_init(keymap_t* map)
{
     keys = map;
}

/*
 * Keycode from scancode.
 */
keymap_t fromscan(scancode_t scan)
{
     if (ISPRESS(scan))
          return keys[(int) scan];
     else
          return keys[(int) -scan];
}

