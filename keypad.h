/**
 * Part of bootmenu
 * Written by Peter Bosch <peterbosc@gmail.com>
 */

#ifndef KEYPAD_H
#define KEYPAD_H

#define KEYPAD_DEV "/dev/input/event0"

#define KEYPAD_VOLDOWN 	(114)
#define KEYPAD_VOLUP 	(115)
#define KEYPAD_MENU	(59)
#define KEYPAD_HOME	(102)
#define KEYPAD_ESC	(158)
#define KEYPAD_POWER	(116)
#define KEYPAD_WAKE	(143)

int open_keypad();

int block_key();

void close_keypad();

#endif
