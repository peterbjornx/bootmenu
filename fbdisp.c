#include <fcntl.h>        /* for fcntl */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>        /* for mmap */
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "fbdisp.h"

//((fbp_x + fb_vinfo.xoffset) * fb_pix_len) + ((fbp_y + fb_vinfo.yoffset) * fb_finfo.line_length)
#define FB_PIX_AT(fbp_x, fbp_y)  ((uint32_t *) (fb_pixels + ((((fbp_x) + fb_vinfo.xoffset) * fb_pix_len) + (((fbp_y) + fb_vinfo.yoffset) * fb_finfo.line_length))))

int		 fb_fd;
long int	 fb_length = 0;
char		*fb_pixels;
int		 fb_pix_len = 4;
int		 fb_width;
int		 fb_height;

struct fb_var_screeninfo fb_vinfo;
struct fb_fix_screeninfo fb_finfo;

int open_fb()
{
	/* open the file for reading and writing */
	fb_fd = open(FB_DEVFILE,O_RDWR);
	if (!fb_fd) {
		printf("Error: cannot open framebuffer device.\n");
		return 0;
	}
	/* get the fixed screen information */
	if (ioctl (fb_fd, FBIOGET_FSCREENINFO, &fb_finfo)) {
		printf("Error reading fixed information.\n");
		return 0;
	}
	/* get variable screen information */
	if (ioctl (fb_fd, FBIOGET_VSCREENINFO, &fb_vinfo)) {
		printf("Error reading variable information.\n");
		return 0;
	}
	fb_length = fb_vinfo.xres_virtual * fb_vinfo.yres_virtual * (fb_vinfo.bits_per_pixel / 8);
	fb_pixels = (char*)mmap(0, fb_finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
	if ((int)fb_pixels == -1) {
		printf ("Error: failed to map framebuffer device to memory.\n");
		return 0;
	}
	fb_pix_len = (fb_vinfo.bits_per_pixel/8);
	fb_width = fb_vinfo.xres;
	fb_height = fb_vinfo.yres;
	return 1;
}

void close_fb()
{
	munmap(fb_pixels, fb_finfo.smem_len);
	close(fb_fd);
}

void draw_v_line(int x, int y, int h, int rgb)
{
	int _c;
	for (_c = 0; _c < h; _c++)
		*FB_PIX_AT(x,y + _c) = (uint32_t) rgb;
}

void draw_h_line(int x, int y, int w, int rgb)
{
	int _c;
	for (_c = 0; _c < w; _c++)
		*FB_PIX_AT(x + _c,y) = (uint32_t) rgb;
}

void draw_rect(int x, int y, int w, int h, int rgb)
{
	draw_v_line	(x	, y	, h, rgb);
	draw_v_line	(x+w-1	, y	, h, rgb);
	draw_h_line	(x	, y	, w, rgb);
	draw_h_line	(x	, y+h-1	, w, rgb);
}

void fill_rect(int x, int y, int w, int h, int rgb)
{
	int _c;
	for (_c = 0; _c < h; _c++)
		draw_h_line(x, y+_c, w, rgb);
}

void draw_image(int x, int y, bitmap_image_t *image)
{
	int _x, _y;
	for (_x = 0; _x < image->width; _x++)
		for (_y = 0; _y < image->height; _y++) {
			*FB_PIX_AT(x + _x, y + _y) = image->pixels[_x + _y * image->height];
		}
}

void clear_fb(int bg)
{
	fill_rect(0,0,fb_width,fb_height,bg);
}
