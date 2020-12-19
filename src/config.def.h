#ifndef _CONFIG_H
#define _CONFIG_H

#define DEFAULT_PRIORITY 0
#define DEFAULT_DISPLAY_MODE NOTE_COMP_PRIO
#define DEFAULT_WIDTH 80
#define MAIN_WIN_REL_WIDTH 70
#define MENU_MARK "->"
#define DEFAULT_TAG "general"
#define NOTES_FILE_NAME ".anotes"
#define MAX_NOTES_PER_PANEL 5

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
