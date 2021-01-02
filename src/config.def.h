#ifndef _CONFIG_H
#define _CONFIG_H

/* DEFAULT VALUES */
#define DEFAULT_WIDTH                      80 /* default stuff width      */
#define DEFAULT_PRIORITY                    0 /* default note priority    */
#define DEFAULT_TAG                 "general" /* default tag  name        */
#define DEFAULT_EXPANDED                    0 /* 1 expanded - 0 collapsed */
#define DEFAULT_LAYOUT              (SW_LEFT) /* side window position     */
#define DEFAULT_LAY_SIZE             (BIG_SW) /* windows sizes            */

/* CLI PROPORTIONS AND LIMITS */
#define MAX_NOTES_PER_PANEL                 5 /* collapsed panel capacity */
#define MAIN_WIN_REL_WIDTH                 70 /* main win relative width  */

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
