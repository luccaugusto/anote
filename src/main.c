/* CLI client for the anote application.
 * Provides improved functionalitty over the shell script
 */

/* HEADERS */
#include <stdio.h>
#include <unistd.h>

#include "anote.h"
#include "cli.h"
#include "list.h"
#include "note.h"
#include "tag.h"

/* FUNCTION PROTOTYPES */
void load_notes_from_file(void);
int write_notes_to_file(void);
void help(void);

/* GLOBAL VARIABLES */
struct d_list *global_tag_list;
char *errmsg;
FILE *notes_file;
char *notes_file_name;
ANOTE_ERROR ERR;

void
load_notes_from_file(void)
{
	char *notes_path;
	char *cur_note;
	char *cur_tag;
	int cur_pri;
	Note n;

	strcpy(notes_path, getenv("NOTES_PATH"));

	if (!notes_path) {
		notes_path = notes_path == NULL ? getenv("XDG_CONFIG_HOME") : notes_path;
		notes_path = realloc(notes_path, strlen(notes_path) + 6);
		sprintf(notes_path, "%sanote/", notes_path);
	}
	notes_file_name = malloc(strlen(notes_path) + 9);
	sprintf(notes_file_name, "%snotes.txt", notes_path);
	notes_file = fopen(notes_file_name, "r");

	/* no notes yet */
	if (!notes_file)
		return;

	/* else reads file and creates notes */
	while (fscanf(notes_file, "%s %d %s", cur_tag, cur_pri, cur_note) > 0) {
		n = new_note(cur_note);
		note_set_priority(cur_pri, n);
		tag_add_note(n, cur_tag);
	}

	fclose(notes_file);
}

int /* allways overwrites file so deletes and edits are saved correctly*/
write_notes_to_file(void)
{
	Tag t;
	Note n;
	struct d_list *i;
	struct d_list *j;
	int notes_written = 0;

	errmsg = malloc(sizeof(notes_file_name) + strlen("File .. could not be opened\n"));
	sprintf(errmsg, "File %s could not be opened\n", notes_file_name);
	notes_file = fopen(notes_file_name, "w");

	RETURN_IF(!notes_file, EFINOOP);

	for (i = global_tag_list; i->next; i = i->next) {
		t = i->obj;
		for (j = tag_get_notes(t); j->next; j = j->next) {
			n = j->obj;
			fprintf(notes_file, "%s %d %s\n", tag_get_name(t), note_get_priority(n), note_get_text(n));
			notes_written++;
		}
	}

	fclose(notes_file);
	return notes_written;
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
	char c;
	char command;
	char *note;
	char *tag_name = "general"; /* default tag */
	int interactive = 1;
	int priority = 0; /* default priority */

	global_tag_list = new_list_node();

	while ((c = getopt(argc,argv,"a:i:hp:t:")) != -1) {
		switch (c) {
			case 'a': /* add note */
				command = 'a';
				note = optarg;
				interactive = 0;
				break;
			case 'i': /* import file */
				command = 'i';
				notes_file_name = optarg;
				interactive = 0;
				break;
			case 'p':
				priority = optarg;
				break;
			case 't': /* specify tag */
				tag_name = optarg;
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

	switch (command) {
		case 'a':
			load_notes_from_file();

			Note n = new_note(note);
			note_set_priority(priority, n);
			tag_add_note(n, tag_name);

			write_notes_to_file();
			break;
		case 'i':/* import file */
			printf("Operation not yet supported");
			exit(-ENOTSUP);
	}

	if (interactive) {
		load_notes_from_file();
		start_anote_cli();
	}

	return 0;
}
