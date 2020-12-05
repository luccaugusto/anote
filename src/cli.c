/* HEADERS */
#include <ncurses.h>
#include <math.h>
#include <stdio.h>

#include "cli.h"

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

	mvprintw(row/2,(col-strlen("q to quit.\n"))/2,"q to quit.\n");
	refresh();

	main_win = create_new_win(main_win_h, main_win_w, 0, 0);
	side_win = create_new_win(side_win_h, side_win_w, 0, main_win_w);


	while ((c = getch()) != 'q') {
		c = getch();
	}
	printw("Bye\n");
	refresh();
	getch();

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
