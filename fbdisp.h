/**
 * Part of bootmenu
 * Written by Peter Bosch <peterbosc@gmail.com>
 */

#ifndef FBDISP_H
#define FBDISP_H

#include <stdint.h>
#include "bitmap.h"

#define FB_DEVFILE	"/dev/fb0"

extern int fb_width;
extern int fb_height;

int open_fb();

void close_fb();

void draw_v_line(int x, int y, int h, int rgb);

void draw_h_line(int x, int y, int w, int rgb);

void draw_rect(int x, int y, int w, int h, int rgb);

void fill_rect(int x, int y, int w, int h, int rgb);

void draw_image(int x, int y, bitmap_image_t *image);

void clear_fb(int bg);

#endif
