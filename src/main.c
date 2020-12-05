/* CLI client for the anote application.
 * Provides improved functionalitty over the shell script
 */

/* HEADERS */
#include <stdio.h>
#include <unistd.h>

#include "cli.h"
#include "list.h"
#include "note.h"
#include "tag.h"

#define ENOTSUP 1

struct d_list *tags_list;
FILE *notes_file;
char *notes_file_name;

void
help(void)
{
	/* TODO: write the full help thingy */
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

	while ((c = getopt(argc,argv,"i:a:t:")) != -1) {
		switch (c) {
			case 'i': /* import file */
				command = 'i';
				notes_file_name = optarg;
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
				if (optopt == 'i')
					printf("No file specified\n");
				else if (optopt == 't')
					printf("No tag specified\n");
				else if (optopt == 'a')
					printf("No note was passed as argument\n");
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
		case 'i':/* import file */
			return -ENOTSUP;
	}


	if (interactive)
		start_anote_cli();

	test_d_list_add();
	test_d_list_del();
	test_tag_note_list_add();
	test_tag_note_list_del();
	test_tag_note_list_edit();

	return 0;
}
