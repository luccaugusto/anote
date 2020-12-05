/* HEADERS */
#include <stdio.h>
#include <ncurses.h>

#include "cli.h"

/* GLOBAL VARIABLES */
extern struct d_list *tags_list;
extern FILE *notes_file;
extern char *notes_file_name;

WINDOW *main_win;
WINDOW *side_win;
int main_h;
int main_w;
int side_h;
int side_w;

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

	main_h = row;
	side_h = row;

	main_w = col/10 * 7; /* 70% for main */
	side_w = col/10 * 3; /* 30% for side */

	mvprintw(row/2,(col-strlen("q to quit.\n"))/2,"q to quit.\n");
	refresh();

	main_win = 

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

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters 
					 * for the vertical and horizontal
					 * lines			*/
	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}

void
delete_win(WINDOW *local_win)
{	
	/* box(local_win, ' ', ' '); : This won't produce the desired
	 * result of erasing the window. It will leave it's four corners 
	 * and so an ugly remnant of window. 
	 */
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	/* The parameters taken are 
	 * 1. win: the window on which to operate
	 * 2. ls: character to be used for the left side of the window 
	 * 3. rs: character to be used for the right side of the window 
	 * 4. ts: character to be used for the top side of the window 
	 * 5. bs: character to be used for the bottom side of the window 
	 * 6. tl: character to be used for the top left corner of the window 
	 * 7. tr: character to be used for the top right corner of the window 
	 * 8. bl: character to be used for the bottom left corner of the window 
	 * 9. br: character to be used for the bottom right corner of the window
	 */
	wrefresh(local_win);
	delwin(local_win);
}
