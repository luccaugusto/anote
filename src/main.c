/* CLI client for the anote application.
 * Provides improved functionalitty over the shell script
 */

/* HEADERS */
#include <errno.h>
#include <getopt.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "main.h"
#include "anote.h"
#include "config.h"
#include "list.h"
#include "note.h"
#include "tag.h"
#include "cli.h"
#include "utils.h"

/* FUNCTION PROTOTYPES */
void load_notes_from_file(char *n_file);
int write_notes_to_file(char *mode);
void build_file_name(void);
void list_all(void);
void list_tag(Tag t);
void list_notes(char *tag);
void help(void);

/* GLOBAL VARIABLES */
struct d_list *global_tag_list;
struct d_list *top_tag_index;
struct d_list *sel_tag_index;
struct d_list *circ_tag_list;
struct d_list *panel_list;
char *errmsg;
FILE *notes_file;
char *notes_path;
char *notes_file_name;
char *arg_tag_name = NULL;
char *def_tag = DEFAULT_TAG;
ANOTE_ERROR aerr;


void
load_notes_from_file(char *n_file)
{
	char *cur_tag = "";
	char *cur_note;
	char *aux;
	int cur_pri;
	int cur_complete;
	Note n;

	notes_file = fopen(n_file, "r");

	/* reads file and creates notes */
	if (notes_file) {
		/* each line of the file is of the format
		 * tag priority note_text \n */

		while (!feof(notes_file)) {
			read_until_separator('|', cur_tag, notes_file);
			cur_pri = str2int(aux);

			read_until_separator('|', aux, notes_file);
			cur_complete = str2int(aux);

			read_until_separator('\n', cur_note, notes_file);

			/* if read something */
			if (!is_blank(cur_tag) && !is_blank(cur_note)) {
				n = new_note(cur_note);
				note_set_priority(cur_pri, n);
				note_set_completed(cur_complete, n);
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
	FILE *notes_real_file;
	struct d_list *i;
	struct d_list *j;
	int ch;
	int total_n_number = 0;
	int notes_written = 0;
	char *text;
	char *swap = NULL;

	/* if on write mode, write to swap file first */
	if (strcmp(mode, "w") == 0) {
		swap = malloc(sizeof(char) * (strlen(notes_file_name) + 4));
		sprintf(swap, "%s.swp", notes_file_name);

		notes_file = fopen(swap, "w+");

	} else {

		notes_file = fopen(notes_file_name, mode);

	}

	RETURN_IF(!notes_file, -errno);

	i = global_tag_list;
	while (i->obj) {
		t = i->obj;
		j = tag_get_notes(t);
		total_n_number += tag_get_n_number(t);

		while (j->obj) {

			n = j->obj;
			text = note_get_text(n);
			if (!is_blank(text)) {
				fprintf(notes_file, "%s|%d|%d|%s\n", tag_get_name(t), note_get_priority(n), note_get_completed(n), text);
				notes_written++;
			}

			CONTINUE_IF(j, j->next);
		}

		CONTINUE_IF(i, i->next);
	}

	/* ERROR */
	if (total_n_number != notes_written) {
		fprintf(stderr, "Error: some notes could not be written. Swap file: %s\n", swap);
		exit(-ENOTENW);
	}

	/* copy contents to real notes file */
	if (swap) {
		/* go to beggining of file */
		fseek(notes_file, 0, SEEK_SET);

		if ((notes_real_file = fopen(notes_file_name, mode)) == NULL)
			return -errno;

		while ((ch = fgetc(notes_file)) != EOF) {
			fputc(ch, notes_real_file);
		}

		fclose(notes_real_file);
		remove(swap);
		return notes_written;
	}

	fclose(notes_file);
	return notes_written;
}

void
build_file_name(void)
{
	char *aux = getenv("NOTES_PATH");

	/* defaults to XDG_CONFIG_HOME/anote */
	if (!aux) {
		aux = getenv("XDG_CONFIG_HOME");
		notes_path = calloc(strlen(aux), sizeof(char));
		strcpy(notes_path, aux);
		notes_path = concatenate(notes_path, "/.anote");
	} else {
		notes_path = calloc(strlen(aux), sizeof(char));
		strcpy(notes_path, aux);
	}

	notes_file_name = concatenate(notes_path, "/"NOTES_FILE_NAME);
}

void
list_all(void)
{
	int has_notes = 0;
	struct d_list *i;
	struct d_list *j;
	Note n;
	Tag t;

	i = global_tag_list;
	while (i->obj) {
		t = i->obj;
		j = tag_get_notes(t);

		if (d_list_length(&j) > 0)
			printf("Notes Tagged %s\n", tag_get_name(t));

		while (j->obj) {

			n = j->obj;
			printf("\t- %d %s\n", note_get_priority(n), note_get_text(n));
			has_notes++;

			CONTINUE_IF(j, j->next);
		}

		CONTINUE_IF(i, i->next);
	}

	if (!has_notes)
		printf("No notes\n");
}

void
list_tag(Tag t)
{
	int has_notes = 0;
	struct d_list *j;
	Note n;

	j = tag_get_notes(t);

	if (d_list_length(&j) > 0)
		printf("Notes Tagged %s\n", tag_get_name(t));

	while (j->obj) {

		n = j->obj;
		printf("\t- %d %s\n", note_get_priority(n), note_get_text(n));
		has_notes++;

		CONTINUE_IF(j, j->next);
	}

	if (!has_notes)
		printf("No notes tagged %s\n", tag_get_name(t));
}

void
list_notes(char *tag)
{
	/* list everything */
	if (tag) {
		list_tag(tag_get(tag));
	} else {
		list_all();
	}
}

void
help(void)
{
	/* TO DO: write the full help thingy */
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
	char command = ' ';
	char *note = "";
	int interactive = 1;
	int priority = 0; /* default priority */
	Tag default_tag;

	global_tag_list = new_list_node();

	while ((c = getopt(argc,argv,"a:di:hlp:rt:")) != -1) {
		switch (c) {
			case 'a':                           /* add note */
				interactive = 0;
				command = 'a';
				note = optarg;
				break;
			case 'd':                           /* set default tag */
				arg_tag_name = DEFAULT_TAG;
				break;
			case 'i':                           /* import file */
				command = 'i';
				notes_file_name = optarg;
				break;
			case 'h':                           /* help */
				interactive = 0;
				help();
				break;
			case 'l':                           /* list */
				interactive = 0;
				command = 'l';
				break;
			case 'p':                           /* specify priority */
				priority = str2int(optarg);
				break;
			case 'r':                           /* remove */
				interactive = 0;
				command = 'r';
				break;
			case 't':                           /* specify tag */
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
	default_tag = new_tag(def_tag);
	d_list_append(default_tag, &global_tag_list, tag_get_size());

	switch (command) {
		case 'a':
			build_file_name();
			n = new_note(note);
			note_set_priority(priority, n);
			tag_add_note(n, def_tag);
			if(write_notes_to_file("a") < 0){
				fprintf(stderr, "Error opening file at %s: %s\n", notes_file_name, strerror( errno ));
			}
			break;

		case 'r':
			errno = ENOTSUP;
			fprintf(stderr, "ERROR: %s\n", strerror( errno ));
			exit(-errno);
			break;
		case 'i':/* import file */
			load_notes_from_file(notes_file_name);
			break;
		case 'l':
			build_file_name();
			load_notes_from_file(notes_file_name);
			list_notes(arg_tag_name);
			break;
		default:
			break;
	}

	if (interactive) {
		build_file_name();
		load_notes_from_file(notes_file_name);
		/* load tag passed on arguments or default tag */
		def_tag = (arg_tag_name) ? arg_tag_name : def_tag;
		start_anote_cli();
		if(write_notes_to_file("w") < 0){
			fprintf(stderr, "Error opening file at %s: %s\n", notes_file_name, strerror(errno));
		}
	}

	return 0;
}
