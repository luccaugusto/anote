/* CLI client for the anote application.
 * Provides improved functionalitty over the shell script
 */

/* HEADERS */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "anote.h"
#include "cli.h"
#include "list.h"
#include "note.h"
#include "tag.h"
#include "utils.h"

/* FUNCTION PROTOTYPES */
void load_notes_from_file(void);
int write_notes_to_file(char *mode);
void build_file_name(void);
void list_notes(void);
void help(void);

/* GLOBAL VARIABLES */
struct d_list *global_tag_list;
char *errmsg;
FILE *notes_file;
char *notes_file_name;
char *arg_tag_name = "general"; /* general is default tag */
ANOTE_ERROR aerr;

void
load_notes_from_file(void)
{
	char *cur_tag = "";
	char *cur_note;
	int cur_pri;
	int c;
	Note n;
	Tag t;

	build_file_name();

	notes_file = fopen(notes_file_name, "r");

	/* reads file and creates notes */
	if (notes_file) {
		/* each line of the file is of the format
		 * tag priority note_text \n */

		while (!feof(notes_file)) {
			cur_tag = read_until_separator(' ', notes_file);
			cur_pri = str2int(read_until_separator(' ', notes_file));
			cur_note = read_until_separator('\n', notes_file);

			/* if read something */
			if (strcmp(cur_tag, "") != 0) {
				n = new_note(cur_note);
				note_set_priority(cur_pri, n);
				tag_add_note(n, cur_tag);
			}
		}

		fclose(notes_file);
	}

}

int
write_notes_to_file(char *mode)
{
	Tag t;
	Note n;
	struct d_list *i;
	struct d_list *j;
	int notes_written = 0;
	int n_pri;

	notes_file = fopen(notes_file_name, mode);

	RETURN_IF(!notes_file, errno);

	for (i = global_tag_list; i->obj; i = i->next) {
		t = i->obj;
		for (j = tag_get_notes(t); j->obj; j = j->next) {
			n = j->obj;
			fprintf(notes_file, "%s %d %s\n", tag_get_name(t), note_get_priority(n), note_get_text(n));
			notes_written++;
		}
	}

	fclose(notes_file);
	return notes_written;
}

void
build_file_name(void)
{
	char *notes_path = getenv("NOTES_PATH");

	/* defaults to XDG_CONFIG_HOME/anote */
	if (!notes_path) {
		notes_path = notes_path == NULL ? getenv("XDG_CONFIG_HOME") : notes_path;
		notes_path = realloc(notes_path, strlen(notes_path) + 6);
		sprintf(notes_path, "%sanote", notes_path);
	}

	notes_file_name = malloc(strlen(notes_path) + 9);
	sprintf(notes_file_name, "%s/notes.txt", notes_path);
}

void
list_notes(void)
{
	struct d_list *i;
	struct d_list *j;
	Note n;
	Tag t;

	for (i = global_tag_list; i->obj; i = i->next) {
		t = i->obj;
		printf("Notes Tagged %s\n", tag_get_name(t));
		for (j = tag_get_notes(t); j->obj; j = j->next) {
			n = j->obj;
			printf("\t- %d %s\n", note_get_priority(n), note_get_text(n));
		}
	}
}

void
help(void)
{
	/* TODO: write the full help thingy */
	printf("anote [-t] [-c] [SOME TAG] [SOME NOTE]: Adds a note to a specific Tag. If no tag is informed adds to general notes\n");
	printf("       -c: adds note to calcurse todo list\n");
	printf("       -l [SOME TAG]:Prompts with a dmenu to select notes file to list notes from\n");
	printf("       -d default tag, same as '-t general'\n");
	printf("       -s [SOMETHING]: Search for notes with SOMETHING. Date and numbers are valid\n");
	printf("       -r [TAG]: prompts with a dmenu to select note to delete. if no tag is informed, prompt user for tags via dmenu\n");
	printf("       -d: short for -t general\n");
	printf("       -i <notes file>\n");
	printf(" NOTE: if dmenu is not installed, the options relying on dmenu will only print the options shown\n");
	printf("       in dmenu to stdout. These options must be used with the -t option to take effect\n");
}

int
main(int argc, char *argv[])
{
	Note n;
	char c;
	char command;
	char *note;
	int interactive = 1;
	int priority = 0; /* default priority */
	Tag default_tag;

	global_tag_list = new_list_node();

	while ((c = getopt(argc,argv,"a:i:hlp:t:")) != -1) {
		switch (c) {
			case 'a': /* add note */
				interactive = 0;
				command = 'a';
				note = optarg;
				break;
			case 'i': /* import file */
				interactive = 0;
				command = 'i';
				notes_file_name = optarg;
				break;
			case 'h':
				interactive = 0;
				help();
				break;
			case 'l':
				interactive = 0;
				command = 'l';
				break;
			case 'p':
				priority = optarg;
				break;
			case 't': /* specify tag */
				arg_tag_name = optarg;
				break;
			case '?':
				interactive = 0;
				if (optopt == 'i')
					printf("No file specified\n");
				else if (optopt == 't')
					printf("No tag specified\n");
				else if (optopt == 'a')
					printf("No note was passed as argument\n");
				else
					help();
				break;
			default:
				interactive = 0;
				help();
				return 1;
		}
	}

	/* load informed tag */
	default_tag = new_tag(arg_tag_name);
	d_list_add(default_tag, &global_tag_list, tag_get_size());

	switch (command) {
		case 'a':
			build_file_name();
			n = new_note(note);
			note_set_priority(priority, n);
			tag_add_note(n, arg_tag_name);
			if(write_notes_to_file("a") <= 0){
				fprintf(stderr, "Error opening file at %s: %s\n", notes_file_name, strerror( errno ));
			}
			break;

		case 'i':/* import file */
			printf("Operation not yet supported");
			exit(-ENOTSUP);
			break;

		case 'l':
			load_notes_from_file();
			list_notes();
			break;
	}

	if (interactive) {
		load_notes_from_file();
		start_anote_cli();
		if(write_notes_to_file("w") <= 0){
			fprintf(stderr, "Error opening file at %s: %s\n", notes_file_name, strerror( errno ));
		}
	}

	return 0;
}
