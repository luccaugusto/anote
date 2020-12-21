#ifndef _CONFIG_H
#define _CONFIG_H

/* DEFAULT VALUES */
#define DEFAULT_WIDTH 80
#define DEFAULT_PRIORITY 0
#define DEFAULT_TAG "general"
#define DEFAULT_EXPANDED 1

/* CLI PROPORTIONS AND LIMITS */
#define MAX_NOTES_PER_PANEL 5
#define MAIN_WIN_REL_WIDTH 70

/* CLI CHARACTERS */
#define MENU_MARK "->"
#define COMPLETE_MARK "C"
#define INCOMPLETE_MARK "-"
#define DEFAULT_DISPLAY_MODE NOTE_ONLY

/* NAME OF THE FILE TO SAVE THE NOTES */
#define NOTES_FILE_NAME ".anotes"

enum COLORS {
	sel_bg = COLOR_BLACK,
	sel_fg = COLOR_RED,
	unsel_bg = COLOR_BLACK,
	unsel_fg = COLOR_YELLOW,
	hl_bg = COLOR_BLACK,
	hl_fg = COLOR_MAGENTA,
	menu_bg_sel = COLOR_BLACK,
	menu_fg_sel = COLOR_RED,
	menu_bg_usl = COLOR_BLACK,
	menu_fg_usl = COLOR_YELLOW,
};

#endif
