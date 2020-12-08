/* HEADERS */
#include <ncurses.h>
#include <math.h>
#include <menu.h>
#include <panel.h>
#include <stdio.h>

#include "list.h"
#include "keys.h"
#include "note.h"
#include "tag.h"
#include "cli.h"

/* FUNCTION PROTOTYPES */
WINDOW *create_new_win(int height, int width, int start_y, int start_x);
void init_cli(void);
void organize_window_space(void);
void load_displayed_tag(char *tag_name);
void housekeeping(void);

void delete_win(WINDOW *local_win);
void show_win(WINDOW *window);
void print_tag_notes(WINDOW *window, Tag t);
void build_tag_panels(WINDOW *window);
void show_cmd(WINDOW *window);
void print_align_center(WINDOW *win, int start_y, int start_x, int width, char *string/*, chtype color*/);

/* GLOBAL VARIABLES */
extern struct d_list *global_tag_list;
extern FILE *notes_file;
extern char *notes_file_name;

Tag displayed_tag;
struct d_list *d_tag_notes;
struct d_list *panel_list;
int d_tag_n_number;
int display_mode = 4;
Selection selected_menu = MAIN_MENU;
char *d_tag_name;

ITEM **main_items;
MENU *main_menu;
PANEL *t_panel;
WINDOW *main_win;
WINDOW *side_win;
WINDOW *footer;
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
	/*start_color();*/    /* colors not yet supported        */
	raw();                /* Line buffering disabled	     */
	keypad(stdscr, TRUE); /* Enables function and arrow keys */
	noecho();             /* Don't echo() while we do getch  */
}

/* creates main, side and footer windows
 * and calculate their sizes
 */
void
organize_window_space(void)
{
	getmaxyx(stdscr,max_row,max_col);

	footer_h = 4;
	main_win_h = max_row - footer_h;
	side_win_h = max_row - footer_h;

	footer_w = max_col;
	main_win_w = ceil(max_col/10.0) * 7; /* 70% for main. round up to fit nicely */
	side_win_w = max_col - main_win_w;
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
	int c;
	char *label;
	panel_list = new_list_node_circ();

	init_cli();
	organize_window_space();

	/* show general notes on main window as default */
	load_displayed_tag("general");

	main_win = create_new_win(main_win_h, main_win_w, 0, 0);
	side_win = create_new_win(side_win_h, side_win_w, 0, main_win_w);
	footer = create_new_win(footer_h, footer_w, main_win_h, 0);

	sprintf(label,"%s Notes", tag_get_name(displayed_tag));
	draw_headers(main_win, main_win_h, main_win_w, label);
	draw_headers(side_win, side_win_h, side_win_w, "Other Notes");

	display_tag_notes(main_win);
	build_tag_panels(side_win);
	show_cmd(footer);

	doupdate();
	t_panel = panel_list->obj;

	do {
		show_win(main_win);
		show_win(side_win);
		show_win(footer);
		/*
		switch (c) {
			case TAB:
				t_panel = (PANEL *)panel_userptr(t_panel);
				top_panel(t_panel);
				break;
			default:
				break;
		}
		*/
		update_panels();
		doupdate();
		switch(c)
		{
			case KEY_DOWN:
			menu_driver(main_menu, REQ_DOWN_ITEM);
			break;
			case KEY_UP:
			menu_driver(main_menu, REQ_UP_ITEM);
			break;
			case KEY_NPAGE:
			menu_driver(main_menu, REQ_SCR_DPAGE);
			break;
			case KEY_PPAGE:
			menu_driver(main_menu, REQ_SCR_UPAGE);
			break;
		}
		wrefresh(main_win);

		if (selected_menu == MAIN_MENU)
			c = wgetch(main_win);
		else
			c = wgetch(side_win);

	} while (c != 'q');

	housekeeping();
	endwin(); /* end curses */
}

void
housekeeping(void)
{
	unpost_menu(main_menu);
	free_menu(main_menu);
	for(int i = 0; i < d_list_length(d_tag_notes); ++i)
		free_item(main_items[i]);
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
	/* draws a blank border to erase the old one */
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(local_win);
	delwin(local_win);
}

void /* colors not yet supported */
show_win(WINDOW *window/*, chtype color */)
{
	box(window, 0, 0);
	wrefresh(window);
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
display_tag_notes(WINDOW *window)
{
	struct d_list *i;
	Note n;
	char *display_text;
	int x_offset = 1;
	int y_offset = HEADER_HEIGHT;

	/* Create items */
	i = d_tag_notes;
	main_items = (ITEM **)calloc(d_list_length(d_tag_notes), sizeof(ITEM *));
	for(int j=0; i->next; ++j, i = i->next) {
		n = i->obj;
		/*
		display_text = malloc(strlen(note_get_text(n)) + 10);
		switch (display_mode) {
			case 1:
				sprintf(display_text, "%s\n", note_get_text(n));
				break;
			case 2:
				sprintf(display_text, "%s [%c]\n", note_get_text(n), (note_get_completed(n)) ? 'V' : '-');
				break;
			case 3:
				sprintf(display_text, "%d. %s\n", note_get_priority(n), note_get_text(n));
				break;
			case 4:
				sprintf(display_text, "%d. %s [%c]\n", note_get_priority(n), note_get_text(n), (note_get_completed(n)) ? 'V' : '-');
				break;
		}
		*/
		main_items[j] = new_item("teste", "display_text");
	}

	main_menu = new_menu((ITEM **)main_items);
	keypad(main_win, TRUE);

	/* Set main window and sub window */
	set_menu_win(main_menu, main_win);
	set_menu_sub(main_menu, derwin(main_win, main_win_h, main_win_w, HEADER_HEIGHT, 1));
	set_menu_format(main_menu, 5, 1);

	/* * to mark */
	set_menu_mark(main_menu, " * ");

	post_menu(main_menu);
	wrefresh(main_win);
}

void
build_tag_panels(WINDOW *window)
{
	Tag t;
	struct d_list *i;
	int x_offset = 1;
	int y_offset = HEADER_HEIGHT;
	PANEL *p;



	for(i = global_tag_list; i->next; i=i->next) {
		t = i->obj;
		if (tag_get_name(t) != d_tag_name) {

			p = new_panel(window);
			d_list_add_circ(p, &panel_list, sizeof(*p));
			mvwprintw(window, y_offset++, x_offset, "%s\n", tag_get_name(t));

		}
	}

	/* point each panel to the next one */
	for(i = panel_list; i->next != panel_list; i = i->next) {
		set_panel_userptr(i->obj, i->next->obj);
	}
	set_panel_userptr(i->obj, i->next->obj);
	update_panels();
}

void
show_cmd(WINDOW *window)
{
	mvwprintw(window, 1, 1, "q: save & quit");
	mvwprintw(window, 2, 1, "t: toogle show mode");
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

