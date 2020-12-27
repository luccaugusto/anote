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
#include "sidewin.h"
#include "utils.h"
#include "cli.h"

/* FUNCTION PROTOTYPES */
void init_cli(void);
void organize_window_space(void);
void housekeeping(void);

void delete_win(WINDOW *local_win);
void hide_win(WINDOW *window);
void populate_main_menu(void);
void reload_main_win(void);
void bind_menu(WINDOW *window, MENU *menu, int height, int width);
void show_cmd(WINDOW *window, char *comm[]);
void update_watch(void);

void main_win_actions(int c);
void side_win_actions(int c);
void execution_loop(void);

/* GLOBAL VARIABLES */
Tag displayed_tag;
Note n_aux;
struct d_list *d_tag_notes;
int d_tag_n_number;
int main_items_size;
char *d_tag_name;

WINDOW *main_win;
WINDOW *cur_win;
WINDOW *footer;
PANEL *layouts_panel;
ITEM **main_items;
MENU *main_menu;

int MAIN_WIN_COLORS;
int main_win_h;
int main_win_w;
int side_win_h;
int side_win_w;
int footer_h;
int footer_w;
int max_row;
int max_col;

/* should have an even number of strings + a NULL temrination */
char *commands[] = {
	"q: save & quit",
	"e: expand selected tag",
	"c: mark complete",
	"a: quick add nt",
	"A: add nt to tag",
	"i: add nt set priority",
	"I: add nt set pri and tag",
	"d: del selected note",
	"D: del selected tag",
	"Enter: Sel tag to main window",
	"Enter: show note details",
	"Tab: Change window",
	NULL,
};

/* should have an even number of strings + a NULL temrination */
char *layout_commands[] = {
	"l: Put side window to the left",
	"r: Put side window to the right",
	"b: Make side window big",
	"d: restore side window default size",
	NULL,
};

/* FUNCTION DEFINITIONS */
void
init_cli(void)
{
	initscr();            /* start ncurses                   */
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
	SIDE_WIN_COLORS = UNSELECTED_COLORS;
}

/* creates main, side and footer windows
 * and calculate their sizes
 */
void
organize_window_space(void)
{
	getmaxyx(stdscr,max_row,max_col);

	footer_h = HEADER_HEIGHT + 1;
	main_win_h = max_row - footer_h;
	side_win_h = max_row - footer_h;
	prompt_win_h = HEADER_HEIGHT + HEADER_HEIGHT;

	footer_w = max_col;
	main_win_w = max_col/100.0 * MAIN_WIN_REL_WIDTH; /* 70% for main */
	side_win_w = max_col - main_win_w;
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

	main_items = NULL;
	main_items_size = 0;

	/* show informed tag notes on main window as default */
	load_displayed_tag(def_tag);

	switch (DEFAULT_LAYOUT) {
		case SW_RIGHT:
			main_win = create_new_win(main_win_h, main_win_w, 0, 0);
			side_win = create_new_win(side_win_h, side_win_w, 0, main_win_w);
			break;
		case BIG_SW:   /* FALLTHROUGH */
			side_win_w = main_win_w;
			main_win_w = max_col - side_win_w;
			wresize(side_win, side_win_h, side_win_w);
			wresize(main_win, main_win_h, main_win_w);
		default:       /* FALLTHROUGH */
		case SW_LEFT:
			side_win = create_new_win(side_win_h, side_win_w, 0, 0);
			main_win = create_new_win(main_win_h, main_win_w, 0, side_win_w);
			break;
	}

	footer = create_new_win(footer_h, footer_w, main_win_h, 0);

	layouts_panel = new_panel(create_new_win(footer_h, footer_w, main_win_h, 0));
	prompt_win = create_new_win(prompt_win_h, prompt_win_w, max_row/4, max_col/4);
	prompt_panel = new_panel(prompt_win);

	cur_win = main_win;

	label = malloc(sizeof(char) * (strlen(label) + strlen(def_tag)));
	sprintf(label, "%s Notes", def_tag);
	draw_headers(main_win, main_win_h, main_win_w, label, COLOR_PAIR(MAIN_WIN_COLORS));

	/* number of tags on header */
	side_w_header = malloc(sizeof(char) * 26); /* "Other Tags [xxx]" */
	sprintf(side_w_header, "Other Tags [%d]", d_list_length(&global_tag_list) - 1);
	draw_headers(side_win, side_win_h, side_win_w, side_w_header, COLOR_PAIR(SIDE_WIN_COLORS));

	populate_main_menu();
	bind_menu(main_win, main_menu, main_win_h, main_win_w);

	build_tag_panels();
	show_cmd(footer, commands);
	show_cmd(panel_window(layouts_panel), layout_commands);

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
	int j;
	del_panel(prompt_panel);

	unpost_menu(main_menu);
	free_menu(main_menu);

	i = panel_list;
	do {

		del_panel(i->obj);
		delwin(panel_window(i->obj));

		i = i->next;
	} while (i != panel_list);

	delete_list_circ(&panel_list);

	/* free the menu items */
	j = 0;
	if (main_items) {
		while (main_items[j] != NULL) {
			free_item(main_items[j++]);
		}
		free(main_items);
	}

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
change_layout(AnoteLayout l)
{
	switch (l) {
		case SW_RIGHT:
			mvwin(main_win, 0, 0);
			mvwin(side_win, 0, main_win_w);
			break;
		case SW_LEFT:
			mvwin(side_win, 0, 0);
			mvwin(main_win, 0, side_win_w);
			break;
		case BIG_SW:
			side_win_w = main_win_w;
			main_win_w = max_col - side_win_w;
			wresize(side_win, side_win_h, side_win_w);
			wresize(main_win, main_win_h, main_win_w);
			if (getbegx(main_win) == 0)
				mvwin(side_win, 0, main_win_w);
			else
				mvwin(main_win, 0, side_win_w);

			reload_main_win();
			reload_side_win();
			break;
		default:
			break;
	}
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
	werase(menu_sub(main_menu));
	REFRESH_MAIN_MENU;
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
populate_main_menu(void)
{
	struct d_list *i;   /* indexes */
	int j = 0;
	char *text;         /* note text                     */
	char *full_text;    /* full_text for item split      */
	char *remainder;    /* item split remainder          */
	int split_pos;      /* item split position           */
	int mw_content_w;   /* main_win usable portion width */

	mw_content_w = (main_win_w - 2 - strlen(MENU_MARK));

	/* free old menu
	 * free old texts and items */
	unpost_menu(main_menu);
	free_menu(main_menu);

	if (main_items) {
		while (main_items[j] != NULL) {
			if (free_item(main_items[j++]) < 0)
				return;
		}
		free(main_items);
	}

	/* Create items */

	/* no notes */
	if (d_tag_n_number == 0) {

		main_items = calloc(1, sizeof(ITEM *));
		main_items[0] = (ITEM *) NULL;
		main_menu = new_menu((ITEM **) main_items);
		mvwprintw(main_win, HEADER_HEIGHT, 1, "No notes in this tag");

		return;

	}

	/* else has notes */
	j = 0;
	main_items_size = d_tag_n_number + 1;
	main_items = (ITEM **) calloc(main_items_size, sizeof(ITEM *));

	i = d_tag_notes;
	while (i->obj) {

		text = note_get_text(i->obj);

		/* note wider than main_window_w - borders - MENU_MARK
		 * split text in more items.
		 * keep description as full text as it is used
		 * to delete items */
		/* TODO: ADD SUPPORT TO LONG NOTES */
		if (0 && strlen(text) > mw_content_w) {
			full_text = text;

			split_pos = find_split_spot(full_text, mw_content_w);
			remainder = substr(full_text, split_pos, strlen(full_text));

			main_items[j] = new_item(substr(full_text, 0, split_pos), full_text);
			++j;

			do {

 				/* indent text */
				if (strlen(remainder) > 0)
					remainder = prepend("    ", remainder);

				split_pos = find_split_spot(remainder, mw_content_w);
				text = substr(remainder, 0, split_pos);

				/* adds a new position to the main items array */
				main_items = realloc(main_items,
						++main_items_size * sizeof (ITEM *));

				/* adds the part of the note to the items */
				main_items[j] = new_item(text, full_text);

				remainder = substr(remainder, split_pos, strlen(remainder));

				++j;
			} while (strlen(remainder) > mw_content_w);

		} else {

			main_items[j] = new_item(text, text);
			++j;
		}

		CONTINUE_IF(i, i->next);
	}

	main_items[d_tag_n_number] = (ITEM *) NULL;
	main_menu = new_menu(main_items);

	if (!main_menu)
		exit(errno);
}

void
bind_menu(WINDOW *window, MENU *menu, int height, int width)
{
	keypad(window, TRUE);

	/* Set main window and sub window */
	set_menu_win(menu, window);
	set_menu_sub(menu, derwin(window, height-HEADER_HEIGHT, width-2, HEADER_HEIGHT, 1));

	set_menu_fore(menu, COLOR_PAIR(MENU_COLORS_FG));
	set_menu_back(menu, COLOR_PAIR(MENU_COLORS_BG));

	set_menu_mark(menu, MENU_MARK);

	menu_opts_off(menu,O_NONCYCLIC);
	menu_opts_off(menu,O_SHOWDESC);
	post_menu(menu);
	wrefresh(window);
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
			menu_driver(main_menu, REQ_DOWN_ITEM);
			break;

		case 'k':      /* FALLTHROUGH */
		case KEY_UP:
			menu_driver(main_menu, REQ_UP_ITEM);
			break;

		case KEY_NPAGE:
			menu_driver(main_menu, REQ_SCR_DPAGE);
			break;

		case KEY_PPAGE:
			menu_driver(main_menu, REQ_SCR_UPAGE);
			break;

		case 'c': /* MARK COMPLETE */
			n_aux = tag_search_note(item_description(current_item(main_menu)), displayed_tag);
			note_set_completed(1, n_aux);
			REFRESH_MAIN_MENU;
			break;

		/* MANIPULATION KEYS */
		case 'd': /* delete */
			if (current_item(main_menu)) {
				answer = prompt_user("Delete selected note? [y/N]: ", "Deleting Note", ALIGN_LEFT);
				if (answer[0] == 'y' || answer[0] == 'Y') {
					n_aux = tag_search_note(item_description(current_item(main_menu)), displayed_tag);

					tag_del_note(n_aux, d_tag_name);
					note_del(n_aux);

					load_displayed_tag(d_tag_name);

					/* erase list from the menu */
					werase(menu_sub(main_menu));
					REFRESH_MAIN_MENU;
				}
			} else {
				prompt_user("Nothing to delete here", "Deleting Note", ALIGN_CENTER);
			}
			break;

		case A_TAB:
			cur_win = side_win;
			MAIN_WIN_COLORS = UNSELECTED_COLORS;
			SIDE_WIN_COLORS = SELECTED_COLORS;
			color_main_win();
			color_side_win();
			break;

		case A_CR: /* FALLTHROUGH */
		case A_NEWLINE:
			n_aux = tag_search_note(item_description(current_item(main_menu)), displayed_tag);
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
				sprintf(side_w_header, "Other Tags [%d]", d_list_length(&global_tag_list) -1);
				draw_headers(side_win, side_win_h, side_win_w, side_w_header, COLOR_PAIR(SIDE_WIN_COLORS));
				break;
			case 'I': /* ADD A NOTE set priority and tag */
				prompt_add_note(1, 1);
				sprintf(side_w_header, "Other Tags [%d]", d_list_length(&global_tag_list) -1);
				draw_headers(side_win, side_win_h, side_win_w, side_w_header, COLOR_PAIR(SIDE_WIN_COLORS));
				break;
			case 'D': /* Delete a tag */
				if (prompt_delete_tag()) {
					prompt_user("Tag Deleted", "Deleting Tag", ALIGN_CENTER);
					sprintf(side_w_header, "Other Tags [%d]", d_list_length(&global_tag_list) -1);
					reload_side_win();
				} else {
					prompt_user("Tag was not deleted", "Deleting Tag", ALIGN_CENTER);
				}
				break;
			case 'e':
				/* toogles expanded */
				expanded = !expanded;

				CLEAR_WINDOW(side_win);
				delete_panels();
				scroll_panels();
				reload_side_win();
				break;
			case 't': /* TOOGLE LAYOUTS */
				show_panel(layouts_panel);
				update_panels();
				doupdate();

				switch (wgetch(cur_win)) {
					case 'l':
						change_layout(SW_LEFT);
						break;
					case 'r':
						change_layout(SW_RIGHT);
						break;
					case 'b':
						change_layout(BIG_SW);
						break;
					default:
						break;
				}

				hide_panel(layouts_panel);
				update_panels();
				doupdate();
				break;

			case 'Z': /* QUIT PROGRAM */
				c = wgetch(cur_win);
				if (c == 'Z') goto quit_anote;
				if (c == 'Q') goto quit_anote;
				break;
			default:
				if (cur_win == main_win) main_win_actions(c);
				else                     side_win_actions(c);
				break;
		}
		show_win(main_win, COLOR_PAIR(MAIN_WIN_COLORS));
		show_win(side_win, COLOR_PAIR(SIDE_WIN_COLORS));
		show_win(footer, COLOR_PAIR(UNSELECTED_COLORS));
	} while ((c = wgetch(cur_win)) != 'q');
quit_anote:
	return;
}
