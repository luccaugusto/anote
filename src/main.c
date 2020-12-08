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

struct d_list *global_tag_list;
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
	char *tag_name = "general"; /* default tag */
	int interactive = 1;

	global_tag_list = new_list_node();

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
				tag_name = optarg;
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
			tag_add_note(note, tag_name);
			break;
		case 'i':/* import file */
			return -ENOTSUP;
	}


	Tag t = new_tag("general");
	Tag t1 = new_tag("music");
	Tag t2 = new_tag("movies");

	d_list_add(t, &global_tag_list, tag_get_size());
	d_list_add(t1, &global_tag_list, tag_get_size());
	d_list_add(t2, &global_tag_list, tag_get_size());

	Note n;
	for (int i=0; i < 10; ++i) {
		n = new_note("teste");
		note_set_priority(i, n);
		tag_add_note(n, "general");
		tag_add_note(new_note("teste music"), "music");
		tag_add_note(new_note("teste movies"), "movies");
	}

	if (interactive)
		start_anote_cli();

	return 0;
}
