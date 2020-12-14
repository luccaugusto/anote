/* HEADERS */
#include <errno.h>
#include <ncurses.h>
#include <menu.h>
#include <panel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "anote.h"
#include "config.h"
#include "list.h"
#include "note.h"
#include "tag.h"
#include "sidewin.h"
#include "utils.h"
#include "cli.h"

/* FUNCTION PROTOTYPES */
WINDOW *create_new_win(int height, int width, int start_y, int start_x);
void init_cli(void);
void organize_window_space(void);
void load_displayed_tag(char *tag_name);
void housekeeping(void);

void delete_win(WINDOW *local_win);
void show_win(WINDOW *window, chtype color);
void hide_win(WINDOW *window);
void reload_main_win(void);
void reload_side_win(void);
void populate_main_menu(void);
void bind_menu(WINDOW *window, MENU *menu, int height, int width);
void show_cmd(WINDOW *window);
void print_align_center(WINDOW *win, int start_y, int start_x, int width, char *string/*, chtype color*/);

void main_win_actions(int c);
void side_win_actions(int c);
char *prompt_user(char *question, int align_center);

void prompt_add_note(short tag, short priority);

/* GLOBAL VARIABLES */
Tag displayed_tag;
Note n_aux;
struct d_list *d_tag_notes;
struct d_list *panel_list;
int display_mode = DEFAULT_DISPLAY_MODE;
int d_tag_n_number;
int main_items_size;
char *d_tag_name;
char **display_text_list;

WINDOW *prompt_win;
WINDOW *main_win;
WINDOW *side_win;
WINDOW *cur_win;
WINDOW *footer;
MENU *main_menu;
MENU *side_menu;
ITEM **main_items;
ITEM *cur_item;
PANEL *prompt_panel;
PANEL *t_panel;

int prompt_win_h;
int prompt_win_w;
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
	init_pair(SELECTED_COLORS, COLOR_YELLOW, COLOR_YELLOW);
	init_pair(UNSELECTED_COLORS, COLOR_YELLOW, COLOR_YELLOW);
	init_pair(HIGHLIGHT_COLORS, COLOR_YELLOW, COLOR_YELLOW);
	init_pair(DEFAULT_COLORS, COLOR_YELLOW, COLOR_YELLOW);
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
	prompt_win_h = HEADER_HEIGHT + 1;

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
	int c = -1;
	char *label = "Notes";
	panel_list = new_list_node_circ();

	init_cli();
	organize_window_space();

	/* show informed tag notes on main window as default */
	load_displayed_tag(arg_tag_name);

	main_win = create_new_win(main_win_h, main_win_w, 0, 0);
	side_win = create_new_win(side_win_h, side_win_w, 0, main_win_w);
	footer = create_new_win(footer_h, footer_w, main_win_h, 0);
	prompt_win = create_new_win(prompt_win_h, prompt_win_w, max_row/4, max_col/4);
	prompt_panel = new_panel(prompt_win);

	cur_win = main_win;

	label = malloc(strlen(label) + strlen(arg_tag_name));
	sprintf(label, "%s Notes", arg_tag_name);
	draw_headers(main_win, main_win_h, main_win_w, label);
	draw_headers(side_win, side_win_h, side_win_w, "Other Notes");

	populate_main_menu();
	bind_menu(main_win, main_menu, main_win_h, main_win_w);

	build_tag_panels(side_win);
	show_cmd(footer);

	doupdate();
	t_panel = panel_list->obj;

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
			default:
				if (cur_win == main_win) main_win_actions(c);
				else                     side_win_actions(c);
				break;
		}
		show_win(main_win, SELECTED_COLORS);
		show_win(side_win, UNSELECTED_COLORS);
		show_win(footer, UNSELECTED_COLORS);
	} while ((c = wgetch(cur_win)) != 'q');


	housekeeping();
	endwin(); /* end curses */
}

void
housekeeping(void)
{
	free(prompt_panel);
	free(t_panel);

	unpost_menu(main_menu);
	free_menu(main_menu);

	/* free the menu items */
	if (main_items != NULL) {
		for (int j=0; j < d_tag_n_number; ++j) {
			if (main_items[j] != NULL)
				free(main_items[j]);
		}
		free(main_items);
	}
}

WINDOW
*create_new_win(int height, int width, int start_y, int start_x)
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
	attron(color);
	box(window, 0, 0);
	wrefresh(window);
	attroff(color);
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
	load_displayed_tag(d_tag_name);

	werase(menu_sub(main_menu));
	populate_main_menu();
	bind_menu(main_win, main_menu, main_win_h, main_win_w);
	wrefresh(menu_win(main_menu));
}

void
reload_side_win(void)
{
	struct d_list *i;

	i = panel_list;
	while (i->obj) {
		if (tag_get_name((Tag) panel_userptr(i->obj)) != d_tag_name) {
			werase(panel_window(i->obj));
			anote_show_panel(i->obj);
		}

		if (i->next) i = i->next;
		else break;
	}

	update_panels();
}

void /* colors not yet supported */
draw_headers(WINDOW *window, int height, int width, char *label/*, chtype color */)
{
	mvwaddch(window, 2, 0, ACS_LTEE);
	mvwhline(window, 2, 1, ACS_HLINE, width - 2);
	mvwaddch(window, 2, width - 1, ACS_RTEE);
	print_align_center(window, 1, 0, width, label/*, COLOR_PAIR(label_color)*/);
}

void
populate_main_menu(void)
{
	struct d_list *i;
	/*Note n = NULL;*/
	char *text;
	int j=0;

	if (display_text_list)
		free(display_text_list);

	/* free the old items */
	if (main_items) {
		for (int j=0; j < main_items_size; ++j) {
			if (main_items[j] != NULL)
				free(main_items[j]);
		}
		free(main_items);
	}


	/* Create items */
	if (d_tag_n_number > 0) {

		main_items = (ITEM **) calloc(d_tag_n_number + 1, sizeof(ITEM *));
		main_items_size = d_tag_n_number + 1;
		display_text_list = (char **) malloc(sizeof(char *) * (d_tag_n_number + 1));

		for(i = d_tag_notes; i->next; i = i->next) {
			text = note_get_text(i->obj);
			/* TODO support different display modes
			   switch (display_mode) {
			   case NOTE_ONLY:
			   display_text_list[j] = malloc(strlen(text));
			   sprintf(display_text_list[j], "%s", text);
			   break;
			   case NOTE_COMP:
			   display_text_list[j] = malloc(strlen(text) + 5);
			   sprintf(display_text_list[j], "%s [%c]", text, (note_get_completed(n)) ? 'V' : '-');
			   break;
			   case NOTE_PRIO:
			   display_text_list[j] = malloc(strlen(text) + 7);
			   sprintf(display_text_list[j], "%d. %s", note_get_priority(n), text);
			   break;
			   case NOTE_COMP_PRIO:
			   display_text_list[j] = malloc(strlen(text) + 12);
			   sprintf(display_text_list[j], "%d. %s [%c]", note_get_priority(n), text, (note_get_completed(n)) ? 'V' : '-');
			   break;
			   }
			   main_items[j] = new_item(display_text_list[j], display_text_list[j]);
			   */
			main_items[j++] = new_item(text, text);
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

}

void /* colors not yet supported */
bind_menu(WINDOW *window, MENU *menu, int height, int width/* chtype colors */)
{
	keypad(window, TRUE);
	unpost_menu(menu);

	/* Set main window and sub window */
	set_menu_win(menu, window);
	set_menu_sub(menu, derwin(window, height-HEADER_HEIGHT, width-2, HEADER_HEIGHT, 1));

	/* * to mark */
	set_menu_mark(menu, MENU_MARK);

	menu_opts_off(menu,O_NONCYCLIC);
	menu_opts_off(menu,O_SHOWDESC);
	post_menu(menu);
	wrefresh(window);
}

void
show_cmd(WINDOW *window)
{
	int col_offset = 1;
	char *commands[] = {
		"q: save & quit",
		"t: toogle show mode",
		"a: quick add note",
		"A: add note to chosen tag",
		"i: add note set priority",
		"I: add note set priority and tag",
		NULL,
	};

	int i = 0;
	while (commands[i]) {
		mvwprintw(window, 1, col_offset, commands[i]);
		mvwprintw(window, 2, col_offset, commands[i+1]);

		col_offset += MAX(strlen(commands[i]), strlen(commands[i+1])) + 1;

		i+=2;
	}
}

void /* colors not yet supported */
print_align_center(WINDOW *win, int start_y, int start_x, int width, char *string/*, chtype color*/)
{
	int length, x, y;

	win = (win == NULL) ? stdscr : win;

	getyx(win, y, x);

	x = (start_x == 0) ? x : start_x;
	y = (start_y == 0) ? y : start_y;

	width = (width == 0) ? DEFAULT_WIDTH : width;

	length = strlen(string);
	x = start_x + (width - length) / 2;
	/*wattron(win, color);*/ /* colors not yet supported */
	mvwprintw(win, y, x, "%s", string);
	/*wattroff(win, color);*/ /* colors not yet supported */
	refresh();
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

		/* MANIPULATION KEYS */
		case 'd': /* delete */
			if (current_item(main_menu)) {
				answer = prompt_user("Delete selected note? [y/N]: ", ALIGN_LEFT);
				if (answer[0] == 'y' || answer[0] == 'Y') {
					n_aux = tag_search_note(item_description(current_item(main_menu)), displayed_tag);

					tag_del_note(n_aux, d_tag_name);
					note_del(n_aux);

					load_displayed_tag(d_tag_name);

					/* erase list from the menu */
					werase(menu_sub(main_menu));
					populate_main_menu();
					bind_menu(main_win, main_menu, main_win_h, main_win_w);
					wrefresh(menu_win(main_menu));
				}
			} else {
				prompt_user("Nothing to delete here", ALIGN_CENTER);
			}
			break;
		case KEY_CTAB:
			cur_win = side_win;
			break;
		default:
			break;
	}
}

void /* tag manipulations */
side_win_actions(int c)
{
	switch(c)
	{
		/* TODO DO NOT MOVE ON THE MENU, MOVE ON PANElS */
		case KEY_ENTER:
			/* DISPLAY SELECTED TAG */
			break;
		case 'j':      /* FALLTHROUGH */
		case KEY_DOWN:
			menu_driver(side_menu, REQ_DOWN_ITEM);
			break;
		case 'k':      /* FALLTHROUGH */
		case KEY_UP:
			menu_driver(side_menu, REQ_UP_ITEM);
			break;

		case KEY_NPAGE:
			menu_driver(side_menu, REQ_SCR_DPAGE);
			break;

		case KEY_PPAGE:
			menu_driver(side_menu, REQ_SCR_UPAGE);
			break;
		case KEY_CTAB:
			cur_win = main_win;
			break;
		default:
			break;
	}

}

char *
prompt_user(char *question, int align_center)
{
	char *answer = malloc(sizeof(char));
	WINDOW *p_win = panel_window(prompt_panel);;

	box(p_win, 0, 0);

	if (align_center)
		print_align_center(p_win, 1, 1, prompt_win_w, question);
	else
		mvwprintw(p_win, 1, 1, question);

	show_panel(prompt_panel);

	echo();
	wgetstr(p_win, answer);
	noecho();

	werase(p_win);
	hide_panel(prompt_panel);

	return answer;
}

void
prompt_add_note(short tag, short priority)
{
	int intput;
	int create_panel = 0;
	int n_pri = DEFAULT_PRIORITY;
	char *input;
	char *n_tag = DEFAULT_TAG;

	input = prompt_user("Note text [blank to cancel]: ", ALIGN_LEFT);

	if (is_blank(input))
		return;

	n_aux = new_note(input);

	if (priority) {
		intput = str2int(prompt_user("Note priority [0-9]: ", 0));

		while (intput < 0 || 9 < intput)
			intput = str2int(prompt_user("Type a valid number please [0-9]: ", 0));

		n_pri = intput;
	}

	if (tag) {
		input = prompt_user("On which tag? [blank for default]: ", 0);
		if (!is_blank(input))
			n_tag = input;
	}

	note_set_priority(n_pri, n_aux);
	tag_add_note(n_aux, n_tag);

	/* reload main window or side window */
	if (strcmp(n_tag, d_tag_name) == 0)
		reload_main_win();
	else
		reload_side_win();
}
