/* vim: tabstop=4 shiftwidth=4 noexpandtab
 *
 * Kernel Argument Parser
 *
 * Parses arguments passed by, ie, a Multiboot bootloader.
 *
 * Part of the ToAruOS Kernel.
 * (C) 2011 Kevin Lange
 */
#include <system.h>

extern void bochs_install_wallpaper(char *);

/**
 * Parse the given arguments to the kernel.
 *
 * @param arg A string containing all arguments, separated by spaces.
 */
void
parse_args(
		char * arg /* Arguments */
		) {
	/* Sanity check... */
	if (!arg) { return; }
	char * pch;         /* Tokenizer pointer */
	char * save;        /* We use the reentrant form of strtok */
	char * argv[1024];  /* Command tokens (space-separated elements) */
	int    tokenid = 0; /* argc, basically */

	/* Tokenize the arguments, splitting at spaces */
	pch = strtok_r(arg," ",&save);
	if (!pch) { return; }
	while (pch != NULL) {
		argv[tokenid] = (char *)pch;
		++tokenid;
		pch = strtok_r(NULL," ",&save);
	}
	argv[tokenid] = NULL;
	/* Tokens are now stored in argv. */


	for (int i = 0; i < tokenid; ++i) {
		/* Parse each provided argument */
		char * pch_i;
		char * save_i;
		char * argp[1024];
		int    argc = 0;
		pch_i = strtok_r(argv[i],"=",&save_i);
		if (!pch_i) { continue; }
		while (pch_i != NULL) {
			argp[argc] = (char *)pch_i;
			++argc;
			pch_i = strtok_r(NULL,"=",&save_i);
		}
		argp[argc] = NULL;

		if (!strcmp(argp[0],"vid")) {
			if (argc < 2) { kprintf("vid=?\n"); continue; }
			uint16_t x, y;
			if (argc < 4) {
				x = 1024;
				y = 768;
			} else {
				x = atoi(argp[2]);
				y = atoi(argp[3]);
				kprintf("[video] Requested display resolution is %dx%d\n", x, y);
			}
			if (!strcmp(argp[1],"qemu")) {
				/* Bochs / Qemu Video Device */
				graphics_install_bochs(x,y);
			} else if (!strcmp(argp[1],"vesa")) {
				graphics_install_vesa(x,y);
			} else {
				kprintf("Unrecognized video adapter: %s\n", argp[1]);
			}
		} else if (!strcmp(argp[0],"hdd")) {
			extern void ext2_disk_mount(void);
			ext2_disk_mount();
		} else if (!strcmp(argp[0],"single")) {
			boot_arg = "--single";
		} else if (!strcmp(argp[0],"vgaterm")) {
			boot_arg = "--vga";
		} else if (!strcmp(argp[0],"start")) {
			if (argc < 2) { kprintf("start=?\n"); continue; }
			boot_arg_extra = argp[1];
		} else if (!strcmp(argp[0],"logtoserial")) {
			kprint_to_serial = 1;
		}
	}
}

