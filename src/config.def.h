#ifndef _CONFIG_H
#define _CONFIG_H

/* DEFAULT VALUES */
#define DEFAULT_WIDTH                      80 /* default stuff width      */
#define DEFAULT_PRIORITY                    0 /* default note priority    */
#define DEFAULT_TAG                 "general" /* default tag  name        */
#define DEFAULT_EXPANDED                    0 /* 1 expanded - 0 collapsed */

/* CLI CHARACTERS */
#define MENU_MARK                        "->" /* menu selection pointer   */
#define COMPLETE_MARK                     "Y" /* Complete symbol          */
#define INCOMPLETE_MARK                   "N" /* Incomplete Symbol        */

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
