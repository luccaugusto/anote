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
void reload_main_win(void);
void populate_main_menu(void);
void bind_menu(WINDOW *window, MENU *menu, int height, int width);
void show_cmd(WINDOW *window);
char *build_note_display_text(Note n);
void update_watch(void);

void main_win_actions(int c);
void side_win_actions(int c);
void execution_loop(void);

/* GLOBAL VARIABLES */
Tag displayed_tag;
Note n_aux;
struct d_list *d_tag_notes;
DisplayModes note_dismode = DEFAULT_DISPLAY_MODE;
int d_tag_n_number;
int main_items_size;
char *d_tag_name;
char **display_text_list;

WINDOW *main_win;
WINDOW *cur_win;
WINDOW *footer;
MENU *main_menu;
ITEM **main_items;

int MAIN_WIN_COLORS;
int main_win_h;
int main_win_w;
int side_win_h;
int side_win_w;
int footer_h;
int footer_w;
int max_row;
int max_col;

/* FUNCTION DEFINITIONS */
void
init_cli(void)
{
	initscr();            /* start ncurses                   */
	start_color();        /* start colors support            */
	raw();                /* Line buffering disabled	     */
	keypad(stdscr, TRUE); /* Enables function and arrow keys */
	noecho();             /* Don't echo() while we do getch  */

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
	panel_list = new_list_node_circ();

	init_cli();
	organize_window_space();

	/* initialize input buffer */
	buffer = calloc(BUFFER_SIZE, sizeof(char));

	/* show informed tag notes on main window as default */
	load_displayed_tag(def_tag);

	main_items = (ITEM **) calloc((d_tag_n_number + 1), sizeof(ITEM *));
	display_text_list = (char **) calloc((d_tag_n_number + 1), sizeof(char *));

	main_win = create_new_win(main_win_h, main_win_w, 0, 0);
	side_win = create_new_win(side_win_h, side_win_w, 0, main_win_w);
	footer = create_new_win(footer_h, footer_w, main_win_h, 0);
	prompt_win = create_new_win(prompt_win_h, prompt_win_w, max_row/4, max_col/4);
	prompt_panel = new_panel(prompt_win);

	cur_win = main_win;

	label = malloc(strlen(label) + strlen(def_tag));
	sprintf(label, "%s Notes", def_tag);
	draw_headers(main_win, main_win_h, main_win_w, label, COLOR_PAIR(MAIN_WIN_COLORS));
	draw_headers(side_win, side_win_h, side_win_w, "Other Notes", COLOR_PAIR(SIDE_WIN_COLORS));

	populate_main_menu();
	bind_menu(main_win, main_menu, main_win_h, main_win_w);

	build_tag_panels();
	show_cmd(footer);

	doupdate();

	execution_loop();

	housekeeping();
	endwin(); /* end curses */
}

void
housekeeping(void)
{
	//struct d_list *i;
	free(prompt_panel);

	unpost_menu(main_menu);
	free_menu(main_menu);

	/*
	i = panel_list;
	while (i->obj) {
		del_panel(i->obj);
		d_list_del_obj(i->obj, &panel_list);

		CONTINUE_IF(i, i->next);
	}
	*/

	free(panel_list);

	/* free the menu items */
	if (main_items != NULL) {
		int j = 0;
		while(main_items[j])
			free(main_items[j++]);
		free(main_items);
	}

	/* free input buffer */
	free(buffer);
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
	struct d_list *i;
	char *text;         /* note text                     */
	char *full_text;    /* full_text for item split      */
	char *remainder;    /* item split remainder          */
	int split_pos;      /* item split position           */
	int j = 0;
	int offset = 0;     /* long note split offset        */
	int mw_content_w;   /* main_win usable portion width */

	mw_content_w = (main_win_w - 2 - strlen(MENU_MARK));

	/* free old menu */
	if (main_menu) {
		free_menu(main_menu);
		unpost_menu(main_menu);
	}

	/* free old texts and items
	for (j=0; j < main_items_size; ++j) {
		free(display_text_list[j]);
		free(main_items[j]);
	} */


	/* Create items */
	if (d_tag_n_number > 0) {

		j = 0;
		main_items_size = d_tag_n_number + 1;
		main_items = (ITEM **) realloc(main_items, sizeof(ITEM *) * (main_items_size));
		display_text_list = (char **) realloc(display_text_list, sizeof(char *) * (main_items_size));

		i = d_tag_notes;
		while (i->obj && j < d_tag_n_number) {

			text = note_get_text(i->obj);

			/* note wider than main_window - borders and MENU_MARK
			 * split text in more items.
			 * keep description as full text as it is used
			 * to delete items */
			if (strlen(text) > mw_content_w) {
				full_text = build_note_display_text(i->obj);

				split_pos = find_split_spot(full_text, mw_content_w);
				remainder = substr(full_text, split_pos, strlen(full_text));
				remainder = concatenate("    ", remainder); /* indent text */

				main_items[j] = new_item(substr(full_text, 0, split_pos), text);
				++j;
				do {
					offset += mw_content_w;

					/* adds a new position to the main items array */
					main_items = realloc(main_items,
							++main_items_size * sizeof (ITEM *));

					/* adds the part of the note to the items */
					main_items[j] = new_item(remainder, text);

					remainder = substr(full_text, offset, offset + mw_content_w);
					remainder = concatenate("\t", remainder); /* indent text */

					++j;
				} while (strlen(remainder) > mw_content_w);

			} else {

				display_text_list[j] = build_note_display_text(i->obj);
				main_items[j] = new_item(display_text_list[j], text);
				++j;
			}

			CONTINUE_IF(i, i->next);
		}

		display_text_list[d_tag_n_number] = (char *) NULL;
		main_items[d_tag_n_number] = (ITEM *) NULL;
		main_menu = new_menu((ITEM **) main_items);

		if (!main_menu)
			exit(errno);

	} else {
		main_items = calloc(1, sizeof(ITEM *));
		main_items[0] = (ITEM *) NULL;
		main_menu = new_menu((ITEM **) main_items);
		mvwprintw(main_win, HEADER_HEIGHT, 1, "No notes in this tag");
	}

	set_menu_fore(main_menu, COLOR_PAIR(MENU_COLORS_FG));
	set_menu_back(main_menu, COLOR_PAIR(MENU_COLORS_BG));
}

void
bind_menu(WINDOW *window, MENU *menu, int height, int width)
{
	keypad(window, TRUE);

	/* Set main window and sub window */
	set_menu_win(menu, window);
	set_menu_sub(menu, derwin(window, height-HEADER_HEIGHT, width-2, HEADER_HEIGHT, 1));

	set_menu_mark(menu, MENU_MARK);

	menu_opts_off(menu,O_NONCYCLIC);
	menu_opts_off(menu,O_SHOWDESC);
	post_menu(menu);
	wrefresh(window);
}

void
show_cmd(WINDOW *window)
{
	/* commands array should always have a pair number
	 * of commands + a NULL termination,
	 * insert a empty string if one is missing */
	int col_offset = 1;
	char *commands[] = {
		"q: save & quit",
		"t: toogle show mode",
		"e: expand selected tag",
		"c: mark complete",
		"a: quick add nt",
		"A: add nt to tag",
		"i: add nt set priority",
		"I: add nt set pri and tag",
		"d: del selected note",
		"D: del selected tag",
		"Enter: Sel tag to main window",
		"Tab: Change window",
		NULL,
	};

	int i = 0;
	while (commands[i]) {
		/* highlight the command */
		wattrset(footer, COLOR_PAIR(HIGHLIGHT_COLORS));
		mvwprintw(window, 1, col_offset, commands[i]);
		mvwprintw(window, 2, col_offset, commands[i+1]);
		wattroff(footer, COLOR_PAIR(HIGHLIGHT_COLORS));

		/* description in normal color */
		mvwprintw(window, 1, col_offset+1, (commands[i])+1);
		mvwprintw(window, 2, col_offset+1, (commands[i+1])+1);

		col_offset += MAX(strlen(commands[i]), strlen(commands[i+1])) + 1;

		i+=2;
	}
}

 /* builds the string to be displayed according
  * with the active display mode */
char *
build_note_display_text(Note n)
{
	char *str;
	char *bd_buffer = malloc(sizeof(char) * BUFFER_SIZE);

	switch (note_dismode) {
		case NOTE_COMP:
			sprintf(bd_buffer, "%s [%s]", note_get_text(n), (note_get_completed(n)) ? COMPLETE_MARK : INCOMPLETE_MARK);
			break;
		case NOTE_PRIO:
			sprintf(bd_buffer, "%d. %s", note_get_priority(n), note_get_text(n));
			break;
		case NOTE_COMP_PRIO:
			sprintf(bd_buffer, "%d. %s [%s]", note_get_priority(n), note_get_text(n), (note_get_completed(n)) ? COMPLETE_MARK : INCOMPLETE_MARK);
			break;
		case NOTE_ONLY: /* FALLTHROUGH */
		default:
			sprintf(bd_buffer, "%s", note_get_text(n));
			break;
	}

	str = malloc(sizeof(char) * strlen(bd_buffer));
	strcpy(str, bd_buffer);
	free(bd_buffer);

	return str;
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

			case 't': /* Toggle display mode */
				note_dismode = (note_dismode + 1) % (NOTE_COMP_PRIO + 1);
				reload_main_win();
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
