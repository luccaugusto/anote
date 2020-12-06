/* HEADERS */
#include <ncurses.h>
#include <math.h>
#include <stdio.h>

#include "cli.h"
#include "list.h"
#include "note.h"
#include "tag.h"

/* FUNCTION PROTOTYPES */
WINDOW *create_new_win(int height, int width, int start_y, int start_x);
void delete_win(WINDOW *local_win);

/* GLOBAL VARIABLES */
extern struct d_list *tags_list;
extern FILE *notes_file;
extern char *notes_file_name;

WINDOW *main_win;
WINDOW *side_win;
int main_win_h;
int main_win_w;
int side_win_h;
int side_win_w;

void
start_anote_cli(void)
{
	int c;
	int row,col;

	initscr();            /* start ncurses                   */
	raw();                /* Line buffering disabled	     */
	keypad(stdscr, TRUE); /* Enables function and arrow keys */
	noecho();             /* Don't echo() while we do getch  */

	getmaxyx(stdscr,row,col);

	main_win_h = row;
	side_win_h = row;

	main_win_w = ceil(col/10.0) * 7-1; /* 70% for main. round up to fit nicely */
	side_win_w = ceil(col/10.0) * 3; /* 30% for side. round up to fit nicely */

	main_win = create_new_win(main_win_h, main_win_w, 0, 0);
	side_win = create_new_win(side_win_h, side_win_w, 0, main_win_w);

	mvwprintw(main_win, row-1, 2,"q to quit.\n");
	mvwprintw(side_win, row-1, 2,"q to quit.\n");

	print_all_notes(main_win);
	while ((c = getch()) != 'q') {
		c = getch();
		wrefresh(main_win);
		wrefresh(side_win);
	}

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

void
show_win(WINDOW *window)
{
	box(window, 0, 0);
	wrefresh(window);
}

void
print_all_notes(WINDOW *window)
{
	struct d_list *i;
	struct d_list *j;
	struct note *n;
	struct tag *t;

	for (i = global_tag_list; i->next != NULL; i = i->next) {
		t = i->obj;
		for (j = t->notes; j->next != NULL; j = j->next) {
			n = j->obj;
			wprintw(window, "%s\n", n->text);
			wrefresh(window);
		}
	}
}
