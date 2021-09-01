/* HEADERS */
#include <errno.h>
#include <ncurses.h>
#include <menu.h>
#include <panel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "anote.h"
#include "config.h"
#include "list.h"
#include "note.h"
#include "prompt.h"
#include "tag.h"
#include "utils.h"
#include "cli.h"

/* FUNCTION PROTOTYPES */
void init_cli(void);
void organize_window_space(void);
void housekeeping(void);

void delete_win(WINDOW *local_win);
void hide_win(WINDOW *window);
void populate_main_win(void);
void reload_main_win(void);
void show_cmd(WINDOW *window, char *comm[]);
void update_watch(void);

void main_win_actions(int c);
void execution_loop(void);

/* GLOBAL VARIABLES */
Tag displayed_tag;             /* displayed tag             */
Note n_aux;                    /* note buffer object        */
struct d_list *sel_note_i;     /* selected note index       */
struct d_list *d_tag_notes;    /* displayed tag notes       */
int d_tag_n_number;            /* number of displayed notes */
char *d_tag_name;              /* displayed tag name        */

WINDOW *main_win;              /* main window               */
WINDOW *cur_win;               /* currently selected window */
WINDOW *footer;                /* footer window             */

int MAIN_WIN_COLORS;           /* dimensions and color      */
int main_win_h;
int main_win_w;
int footer_h;
int footer_w;
int max_row;
int max_col;

/* should have an even number of strings + a NULL temrination */
char *commands[] = {
	"q: save & quit",
	"c: mark complete",
	"a: quick add nt",
	"A: add nt to tag",
	"i: add nt set priority",
	"I: add nt set pri and tag",
	"d: del selected note",
	"D: del selected tag",
	"Enter: show note details",
	"Tab: Change note",
	NULL,
};

/* FUNCTION DEFINITIONS */
void
init_cli(void)
{
	if (initscr() == NULL)/* start ncurses                   */
		exit(errno);

	start_color();        /* start colors support            */
	raw();                /* Line buffering disabled	     */
	keypad(stdscr, TRUE); /* Enables function and arrow keys */
	noecho();             /* Don't echo() while we do getch  */
	curs_set(0);          /* hide cursor                     */

	/* setup color pairs */
	init_pair(SELECTED_COLORS, sel_fg, sel_bg);
	init_pair(UNSELECTED_COLORS, unsel_fg, unsel_bg);
	init_pair(HIGHLIGHT_COLORS, hl_fg, hl_bg);
	init_pair(MENU_COLORS_FG, menu_fg_sel, menu_bg_sel);
	init_pair(MENU_COLORS_BG, menu_fg_usl, menu_bg_usl);
	MAIN_WIN_COLORS = SELECTED_COLORS;
}

/* creates main and footer windows
 * and calculate their sizes
 */
void
organize_window_space(void)
{
	getmaxyx(stdscr,max_row,max_col);

	footer_h = HEADER_HEIGHT + 1;
	main_win_h = max_row - footer_h;
	prompt_win_h = HEADER_HEIGHT + HEADER_HEIGHT;

	footer_w = max_col;
	main_win_w = max_col;
	prompt_win_w = max_col/2;
}

void
load_displayed_tag(char *tag_name)
{
	displayed_tag = tag_get(tag_name);
	d_tag_n_number = tag_get_n_number(displayed_tag);
	d_tag_name = tag_get_name(displayed_tag);
	d_tag_notes = tag_get_notes(displayed_tag);
}

void
start_anote_cli(void)
{
	char *label = "Notes";

	init_cli();
	organize_window_space();

	/* initialize input buffer */
	buffer = calloc(BUFFER_SIZE, sizeof(char));

	/* show informed tag notes on main window as default */
	load_displayed_tag(def_tag);
	sel_note_i = d_tag_notes;

	main_win = create_new_win(main_win_h, main_win_w, 0, 0);
	footer = create_new_win(footer_h, footer_w, main_win_h, 0);
	cur_win = main_win;

	prompt_win = create_new_win(prompt_win_h, prompt_win_w, max_row/4, max_col/4);
	prompt_panel = new_panel(prompt_win);

	label = malloc(sizeof(char) * (strlen(label) + strlen(def_tag)));
	sprintf(label, "%s Notes", def_tag);
	draw_headers(main_win, main_win_h, main_win_w, label, COLOR_PAIR(MAIN_WIN_COLORS));

	populate_main_win();

	show_cmd(footer, commands);

	update_panels();
	doupdate();

	execution_loop();

	housekeeping();
	endwin(); /* end curses */
}

void
housekeeping(void)
{
	struct d_list *i;
	del_panel(prompt_panel);

	i = panel_list;
	do {

		del_panel(i->obj);
		delwin(panel_window(i->obj));

		i = i->next;
	} while (i != panel_list);

	delete_list_circ(&panel_list);

	/* free input buffer */
	free(buffer);

	/* put cursor back on screen */
	curs_set(1);
}

WINDOW *
create_new_win(int height, int width, int start_y, int start_x)
{
	WINDOW *local_win;

	local_win = newwin(height, width, start_y, start_x);

	/* use default caracters for borders */
	box(local_win, 0 , 0);
	wrefresh(local_win);

	return local_win;
}

void
delete_win(WINDOW *local_win)
{
	hide_win(local_win);
	delwin(local_win);
}

void
show_win(WINDOW *window, chtype color)
{
	wattrset(window, color);
	box(window, 0, 0);
	wrefresh(window);
}

void
hide_win(WINDOW *window)
{
	/* draws a blank border to erase the old one */
	wborder(window, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(window);
}

void
reload_main_win(void)
{
	werase(main_win);
	color_main_win();

	load_displayed_tag(d_tag_name);
	populate_main_win();
}

void
color_main_win(void)
{
	char *label;

	label = malloc(sizeof(char) * (7 + strlen(d_tag_name)));
	sprintf(label, "%s Notes", d_tag_name);
	draw_headers(main_win, main_win_h, main_win_w, label, COLOR_PAIR(MAIN_WIN_COLORS));
}
void
draw_headers(WINDOW *window, int height, int width, char *label, chtype color)
{
	wattrset(window, color);
	mvwaddch(window, 2, 0, ACS_LTEE);
	mvwhline(window, 2, 1, ACS_HLINE, width - 2);
	mvwaddch(window, 2, width - 1, ACS_RTEE);
	print_align_center(window, 1, 0, width, label, color);
	attroff(color);
}

void
print_align_center(WINDOW *win, int start_y, int start_x, int width, char *string, chtype color)
{
	int length, x, y;

	win = (win == NULL) ? stdscr : win;

	getyx(win, y, x);

	x = (start_x == 0) ? x : start_x;
	y = (start_y == 0) ? y : start_y;

	width = (width == 0) ? DEFAULT_WIDTH : width;

	length = strlen(string);
	x = start_x + (width - length) / 2;
	wattrset(win, color);
	mvwprintw(win, y, x, "%s", string);
	refresh();
}

void
populate_main_win(void)
{
	struct d_list *i;   /* indexes                       */
	char *text;         /* note text                     */
	char *remainder;    /* text split remainder          */
	char *substring;    /* split substring               */
	int mw_content_w;   /* main_win usable portion width */
	int split_pos;
	int beg_pos;
	int y_offset = HEADER_HEIGHT;
	int x_offset = strlen(MENU_MARK) + 1;

	mw_content_w = (main_win_w - 2 - strlen(MENU_MARK));

	CLEAR_WINDOW(main_win);

	/* no notes */
	if (d_tag_n_number == 0) {
		mvwprintw(main_win, HEADER_HEIGHT, 1, "No notes in this tag");
		return;
	}

	/* else has notes */
	i = d_tag_notes;
	while (i->obj) {

		if (SELECTED_NOTE(i->obj)) {
			wattrset(main_win, COLOR_PAIR(SELECTED_COLORS));
			mvwprintw(main_win, y_offset, 1, MENU_MARK);
		} else {
			wattrset(main_win, COLOR_PAIR(UNSELECTED_COLORS));
			mvwprintw(main_win, y_offset, 1, "  ");
		}

		text = note_get_text(i->obj);
		/* note too long, break it */
		if (strlen(text) > mw_content_w) {

			beg_pos = 0;
			while (beg_pos < strlen(text)) {
				substr(text, remainder, beg_pos, strlen(text));

				/* indented note */
				split_pos = find_split_spot(remainder, mw_content_w - strlen(MENU_MARK) - 2);

				substr(text, substring, beg_pos, beg_pos + split_pos);
				/* indent only after line break */
				if (beg_pos == 0)
					mvwprintw(main_win, y_offset++, x_offset, substring);
				else
					mvwprintw(main_win, y_offset++, x_offset, "  %s", substring);

				free(substring);
				free(remainder);

				beg_pos += split_pos;
			}

		} else {
			mvwprintw(main_win, y_offset++, x_offset, text);
		}

		CONTINUE_IF(i, i->next);
	}
}

void
show_cmd(WINDOW *window, char *comm[])
{
	/* commands array should always have a pair number
	 * of commands + a NULL termination,
	 * insert a empty string if one is missing */
	int col_offset = 1;

	int i = 0;
	while (comm[i]) {
		/* highlight the command */
		wattrset(footer, COLOR_PAIR(HIGHLIGHT_COLORS));
		mvwprintw(window, 1, col_offset, comm[i]);
		mvwprintw(window, 2, col_offset, comm[i+1]);
		wattroff(footer, COLOR_PAIR(HIGHLIGHT_COLORS));

		/* description in normal color */
		mvwprintw(window, 1, col_offset+1, (comm[i])+1);
		mvwprintw(window, 2, col_offset+1, (comm[i+1])+1);

		col_offset += MAX(strlen(comm[i]), strlen(comm[i+1])) + 1;

		i+=2;
	}
}

void /* notes manipulation */
main_win_actions(int c)
{
	char *answer;

	switch(c)
	{
		/* MOVE KEYS */
		case 'j':      /* FALLTHROUGH */
		case KEY_DOWN:
			if (sel_note_i->next)
				sel_note_i = sel_note_i->next;
			populate_main_win();
			break;

		case 'k':      /* FALLTHROUGH */
		case KEY_UP:
			sel_note_i = d_list_prev(sel_note_i->obj, &d_tag_notes);
			populate_main_win();
			break;

		case 'c': /* MARK COMPLETE */
			note_set_completed(1, n_aux);
			populate_main_win();
			break;

		/* MANIPULATION KEYS */
		case 'd': /* delete */
			if (sel_note_i != NULL) {
				answer = prompt_user("Delete selected note? [y/N]: ", "Deleting Note", ALIGN_LEFT);
				if (answer[0] == 'y' || answer[0] == 'Y') {

					n_aux = sel_note_i->obj;
					sel_note_i = d_list_prev(sel_note_i->obj, &d_tag_notes);

					tag_del_note(n_aux, d_tag_name);
					note_del(n_aux);

					load_displayed_tag(d_tag_name);

					/* erase list from the menu */
					populate_main_win();
				}
			} else {
				prompt_user("Nothing to delete here", "Deleting Note", ALIGN_CENTER);
			}
			break;

		case A_TAB:
			/* TODO change displayed tag */
			break;

		case A_CR: /* FALLTHROUGH */
		case A_NEWLINE:
			n_aux = sel_note_i->obj;
			prompt_show_details(n_aux);
			break;

		default:
			break;
	}
}

void
execution_loop(void)
{
	int c = -1;
	do {
		switch (c) {
			case 'a': /* QUICK ADD, default priority */
				prompt_add_note(0, 0);
				break;
			case 'i': /* ADD A NOTE set priority */
				prompt_add_note(0, 1);
				break;
			case 'A': /* ADD A NOTE set tag */
				prompt_add_note(1, 0);
				break;
			case 'I': /* ADD A NOTE set priority and tag */
				prompt_add_note(1, 1);
				break;
			case 'D': /* Delete a tag */
				if (prompt_delete_tag()) {
					prompt_user("Tag Deleted", "Deleting Tag", ALIGN_CENTER);
				} else {
					prompt_user("Tag was not deleted", "Deleting Tag", ALIGN_CENTER);
				}
				break;
			case 'Z': /* QUIT PROGRAM */
				c = wgetch(cur_win);
				if (c == 'Z' || c == 'Q')
					goto quit_anote;
				break;
			default:
				main_win_actions(c);
				break;
		}
		show_win(main_win, COLOR_PAIR(MAIN_WIN_COLORS));
		show_win(footer, COLOR_PAIR(UNSELECTED_COLORS));
	} while ((c = wgetch(cur_win)) != 'q');

quit_anote:
	return;
}
