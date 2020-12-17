#ifndef _CONFIG_H
#define _CONFIG_H

#define DEFAULT_PRIORITY 0
#define DEFAULT_DISPLAY_MODE NOTE_ONLY
#define DEFAULT_WIDTH 80
#define MAIN_WIN_REL_WIDTH 70
#define MENU_MARK "->"
#define DEFAULT_TAG "general"
#define NOTES_FILE_NAME ".anotes"
#define MAX_NOTES_PER_PANEL 5

enum COLORS {
	sel_bg = COLOR_BLACK,
	sel_fg = COLOR_YELLOW,
	unsel_bg = COLOR_BLACK,
	unsel_fg = COLOR_RED,
	hl_bg = COLOR_WHITE,
	hl_fg = COLOR_YELLOW,
	nohl_bg = COLOR_BLACK,
	nohl_fg = COLOR_WHITE,
	menu_bg_sel = COLOR_BLACK,
	menu_fg_sel = COLOR_RED,
	menu_bg_usl = COLOR_BLACK,
	menu_fg_usl = COLOR_YELLOW,
};

#endif
