#include "fbdisp.h"
#include "keypad.h"
#include "bitmap.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int item_count	= 0;
int item_space	= 32;
int item_width	= 128;
int item_height	= 128;
int fallback_selection = 0;

bitmap_image_t   **item_image;
uint32_t	*item_colours;
int selected = 0;

void exit_failsafe()
{
	printf("ERROR\n");
	exit(fallback_selection);
	//TODO: Handle normal boot here to prevent bricks
}

void do_item()
{
	close_keypad();
	close_fb();
	exit(selected);
}

void update_disp()
{
	int disp_w = item_width * item_count + item_space * (item_count - 1);
	int disp_h = item_height;
	int s_x = (fb_width - disp_w) / 2;
	int s_y = (fb_height - disp_h) / 2;
	int c;
	for (c = 0; c < item_count; c++){
		if (item_image[c])
			draw_image(s_x, s_y, item_image[c]);
		else
			fill_rect(s_x, s_y, item_width, item_height, item_colours[c]);
		if (c == selected)
			draw_rect(s_x, s_y, item_width, item_height, 0x00FFFFFF);
		s_x += item_width + item_space;
	}
}

int main_loop()
{
	int key;
	key = block_key();
	switch (key) {
		case KEYPAD_VOLDOWN:
			if (selected > 0)
				selected--;
			update_disp();
			break;
		case KEYPAD_VOLUP:
			if (selected < (item_count - 1))
				selected++;
			update_disp();
			break;
		case KEYPAD_MENU:
			do_item();
			break;
	}
	return 1;
}

void usage()
{
	printf("Usage: bootmenu -h | -n <N> [-w <N>] [-b <N>] [-s <N>] [-d <i>]\n");
	printf("	{-p <path> | -c <colour>}\n");
	printf("\n	-h	Print this help\n");
	printf("	-n	Specify the number of items\n");
	printf("	-w	Specify the item width\n");
	printf("	-b	Specify the item height\n");
	printf("	-s	Specify the horizontal space between items\n");
	printf("	-d	Specify the fallback selection\n");
	printf("	-p	Add an item represented as an image at <path>\n");
	printf("	-c	Add an item represented as a coloured square\n\n");
	printf("Images must be in BMP24 format, colours are in B8G8R8 format\n");
	
}

void parse_opts(int argc, char **argv)
{
	int idx;
	char c;
	while ((c = getopt (argc, argv, "hn:w:b:s:p:c:d:")) != 255)
	switch (c)
	{
		case 'h':
			usage();
			exit(fallback_selection);
			break;
		case 'n':
			item_count = atoi(optarg);
			item_image   = malloc(sizeof(bitmap_image_t *) * item_count);
			item_colours = malloc(sizeof(uint32_t) * item_count);
			break;
		case 'w':
			item_width = atoi(optarg);
			break;
		case 'b':
			item_height = atoi(optarg);
			break;
		case 's':
			item_space = atoi(optarg);
			break;
		case 'd':
			fallback_selection = atoi(optarg);
			break;
		case 'p':
			if (!item_count) {
				fprintf (stderr, "Count not specified\n");
				exit(fallback_selection);
			}
			if (idx == item_count) {
				fprintf (stderr, "Too many items\n");
				exit(fallback_selection);
			}
			item_image[idx] = load_bmp(optarg);
			if (!item_image[idx++]) {
				fprintf (stderr, "Error loading image %s\n", optopt);
				exit(fallback_selection);
			}
			break;
		case 'c':
			if (!item_count) {
				fprintf (stderr, "Count not specified\n");
				exit(fallback_selection);
			}
			if (idx == item_count) {
				fprintf (stderr, "Too many items\n");
				exit(fallback_selection);
			}
			item_colours[idx++] = strtol(optarg, NULL, 0);
			break;
		case '?':
			if (optopt == 'c')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint (optopt))
				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
			exit(fallback_selection);
			break;
		default:
			printf("%i",c);
			exit(fallback_selection);
	}
}

int main(int argc, char **argv)
{
	printf("bootmenu 0.01 by Peter Bosch <peterbosc@gmail.com>\n");
	parse_opts(argc, argv);
	if (!item_count) {
		usage();
		exit(fallback_selection);
	}
	if (!open_keypad())
		exit_failsafe("couldn't open keypad");
	if (!open_fb())
		exit_failsafe("couldn't open framebuffer");
	printf("starting");
	clear_fb(0);
	update_disp();
	while(main_loop());
	close_keypad();
}
