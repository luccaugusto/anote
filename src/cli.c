/* HEADERS */
#include <ncurses.h>
#include <math.h>
#include <stdio.h>

#include "list.h"
#include "note.h"
#include "tag.h"
#include "cli.h"

/* FUNCTION PROTOTYPES */
WINDOW *create_new_win(int height, int width, int start_y, int start_x);
void init_cli(void);
void organize_window_space(void);
void load_displayed_tag(char *tag_name);

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
int d_tag_n_number;
char *d_tag_name;

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
	main_win_w = ceil(max_col/10.0) * 7-1; /* 70% for main. round up to fit nicely */
	side_win_w = ceil(max_col/10.0) * 3; /* 30% for side. round up to fit nicely */
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

	do {
		show_win(main_win);
		show_win(side_win);
		show_win(footer);
	} while ((c = getch()) != 'q');

	endwin(); /* end curses */
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

	i = d_tag_notes;
	for (; i->next; i = i->next) {
		n = i->obj;
		display_text = malloc(strlen(note_get_text(n)) + 10);
		sprintf(display_text, "%d. %s [%c]\n", note_get_priority(n), note_get_text(n), (note_get_completed(n)) ? 'V' : '-');
		mvwprintw(window, y_offset++, x_offset, display_text);
	}
}

void
build_tag_panels(WINDOW *window)
{
	Tag t;
	struct d_list *i;
	int x_offset = 1;
	int y_offset = HEADER_HEIGHT;

	for(i = global_tag_list; i->next; i=i->next) {
		t = i->obj;
		if (tag_get_name(t) != d_tag_name)
			mvwprintw(window, y_offset++, x_offset, "%s\n", tag_get_name(t));
	}
}

void
show_cmd(WINDOW *window)
{
	mvwprintw(window, 1, 1, "q: save & quit");
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

