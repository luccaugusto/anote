/* CLI client for the anote application.
 * Provides improved functionalitty over the shell script
 */

/* HEADERS */
#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>

#include "list.h"
#include "notes.h"

struct d_list *notes_list;
FILE *notes_file;
char *notes_file_name;

void
start_anote(void)
{
	int c;

	initscr();            /* start ncurses                  */
	raw();                /* Line buffering disabled	    */
	keypad(stdscr, TRUE); /* We get F1, F2 etc..		    */
	noecho();             /* Don't echo() while we do getch */

	printw("Welcome to aNote.\n");
	printw("press F1 to quit.\n");
	refresh();
	while ((c = getch()) != KEY_F(1)) {
		c = getch();
	}
	printw("Bye\n");
	refresh();

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
	char *note;
	int interactive = 1;

	notes_list = init_list();

	while((c = getopt(argc,argv,"i:a:")) != -1) {
		switch(c) {
			case 'f':
				notes_file_name = optarg;
				notes_file= fopen(notes_file_name, "r");
				interactive = 0;
				break;
			case 'a':
				note = optarg;
				d_list_add(note, notes_list);
				interactive = 0;
				break;
			case '?':
				if (optopt == 'f')
					printf("No file specified");
				else
					help();
			default:
				help();
				return 1;
		}
	}


	if (interactive)
		start_anote();

	test_d_list_add();
	test_d_list_del_note();

	return 0;
}
