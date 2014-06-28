/**
 * Part of bootmenu
 * Written by Peter Bosch <peterbosc@gmail.com>
 */

#include "keypad.h"
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int keypad_fd;

int open_keypad()
{
	keypad_fd = open(KEYPAD_DEV, O_RDONLY);
	if (keypad_fd == -1)
		return 0;
	return 1;
}

int block_key()
{
	struct input_event ev;
	while (1) {
		read(keypad_fd, &ev, sizeof(struct input_event));
		if ((ev.type == 1) && (ev.value == 0))
			return ev.code;
	}
	return -1;
}

void close_keypad()
{
	close(keypad_fd);
}
