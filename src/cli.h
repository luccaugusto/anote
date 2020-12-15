#ifndef _CLI_H
#define _CLI_H

#define HEADER_HEIGHT 3
#define SELECTED_COLORS 1
#define UNSELECTED_COLORS 2
#define HIGHLIGHT_COLORS 3
#define DEFAULT_COLORS 4

#define CLEAR_WINDOW(x)     \
	wmove(x, HEADER_HEIGHT + 2, 1); \
	wclrtobot(x);

/* ANOTE KEYS */
typedef enum {
	A_TAB = 9,
} A_Keys;

/* types */
typedef enum {
	ALIGN_LEFT = 0,
	ALIGN_CENTER,
	NOTE_ONLY,
	NOTE_COMP,
	NOTE_PRIO,
	NOTE_COMP_PRIO,
} DisplayModes;

/* FUNCTION PROTOTYPES */
void start_anote_cli(void);
void draw_headers(WINDOW *window, int height, int width, char *label/*, chtype color */);
void reload_main_win(void);
void reload_side_win(void);

/* GLOBAL VARIABLES */
extern WINDOW *cur_win;
extern WINDOW *main_win;
extern Tag displayed_tag;
extern int d_tag_n_number;
extern char *d_tag_name;
extern struct d_list *d_tag_notes;

extern struct d_list *panel_list;
extern int prompt_win_h;
extern int prompt_win_w;
extern int main_win_h;
extern int main_win_w;
extern int side_win_h;
extern int side_win_w;
extern int footer_h;
extern int footer_w;
extern int max_row;
extern int max_col;

#endif
