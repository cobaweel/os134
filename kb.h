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

#ifndef _KB_H
#define _KB_H

#include "i8042.h"

/*
 * Is this scancode a press or release event?
 */
#define ISRELEASE(c) ((c) & 0x80)
#define ISPRESS(c)   (!((c) & 0x80))
#define RELEASE_OF_PRESS(c) (((c) & 0xff) - 0x80)

/*
 * Bucky bits.
 */
typedef char bucky_t;
#define KB_CTL_L     0x01            /* Left Ctrl. */
#define KB_CTL_R     0x02            /* Right Ctrl. */
#define KB_ALT_L     0x04            /* Left Alt. */
#define KB_ALT_R     0x08            /* Right Alt. */
#define KB_META_L    0x10            /* Left "Flying window." */
#define KB_META_R    0x20            /* Right "Flying window." */
#define KB_SUPER     0x40            /* "Menu." */
#define KB_SHIFT     0x80            /* Shift. */

/*
 * It so happens that if a scancode has an obvious 7-bit ascii
 * representation, that that is automatically its key code. Other
 * scancodes have keycode 0 associated with them, leaving it up to the
 * terminal driver to find something to do with them.
 */
typedef char key_t;
#define NOKEY       0

/*
 * A keymap contains, for every scancode, the corresponding keycode,
 * as well as what buckybits should be set on all other keys that are
 * pressed while this key is being pressed.
 *
 * Note that there is a concept of depress and release scancodes,
 * which does not apply to keycodes. Scancodes only map to a keycode
 * if they are depress scancodes.
 */
struct keymap_t {
     key_t key;
     key_t shift_key;
     bucky_t bucky;
};
typedef struct keymap_t keymap_t;

keymap_t *keys;
extern keymap_t us[256];
extern keymap_t dv[256];

void kb_init(keymap_t* keymap);
keymap_t fromscan(scancode_t scan);

#endif /* _KB_H */
