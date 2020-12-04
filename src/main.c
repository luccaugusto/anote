/* CLI client for the anote application.
 * Provides improved functionalitty over the shell script
 */

/* HEADERS */
#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>

#include "list.h"
#include "notes.h"
#include "tag.h"

#define ENOTSUP 1

struct d_list *tags_list;
FILE *notes_file;
char *notes_file_name;

void
start_anote(void)
{
	int c;
	int row,col;

	initscr();            /* start ncurses                   */
	raw();                /* Line buffering disabled	     */
	keypad(stdscr, TRUE); /* Enables function and arrow keys */
	noecho();             /* Don't echo() while we do getch  */

	getmaxyx(stdscr,row,col);

	mvprintw(row/2,(col-strlen("Welcome to aNote.\n"))/2,"Welcome to aNote.\n");
	printw("press q to quit.\n");
	refresh();
	while ((c = getch()) != 'q') {
		c = getch();
	}
	printw("Bye\n");
	refresh();
	getch();

	endwin(); /* end curses */
}

void
help(void)
{
	printf("anote [-i <notes file>]\n");
}

int
main(int argc, char *argv[])
{
	char c;
	char command;
	char *note;
	char *tag = "general"; /* default tag */
	int interactive = 1;

	tags_list = init_list();

	while ((c = getopt(argc,argv,"f:a:t:")) != -1) {
		switch (c) {
			case 'f': /* import file */
				notes_file_name = optarg;
				notes_file= fopen(notes_file_name, "r");
				interactive = 0;
				break;
			case 'a': /* add note */
				command = 'a';
				note = optarg;
				interactive = 0;
				break;
			case 't': /* specify tag */
				tag = optarg;
				break;
			case '?':
				if (optopt == 'f')
					printf("No file specified");
				/* TODO
				 * other error messages
				 */
				else
					help();
			default:
				help();
				return 1;
		}
	}

	switch (command) {
		case 'a':
			tag_add(note, tags_list, tag);
			break;
		case 'f':/* import file */
			return -ENOTSUP;
	}


	if (interactive)
		start_anote();

	test_d_list_add();
	test_d_list_del();

	return 0;
}
