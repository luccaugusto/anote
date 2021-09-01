/* HEADERS */
#include <ncurses.h>
#include <panel.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"
#include "anote.h"
#include "config.h"
#include "list.h"
#include "note.h"
#include "tag.h"
#include "cli.h"
#include "utils.h"
#include "prompt.h"

/* GLOBAL VARIABLES */
WINDOW *prompt_win;
PANEL *prompt_panel;

char *buffer;

int prompt_win_h;
int prompt_win_w;

char *
prompt_user(char *question, char *label,int align_center)
{
	char *answer;
	WINDOW *p_win = panel_window(prompt_panel);;

	wattrset(p_win, COLOR_PAIR(HIGHLIGHT_COLORS));
	box(p_win, 0, 0);
	draw_headers(p_win, prompt_win_h, prompt_win_w, label, COLOR_PAIR(HIGHLIGHT_COLORS));

	if (align_center)
		print_align_center(p_win, HEADER_HEIGHT, 1, prompt_win_w, question, COLOR_PAIR(HIGHLIGHT_COLORS));
	else
		mvwprintw(p_win, HEADER_HEIGHT, 1, question);

	show_panel(prompt_panel);

	echo();
	wgetstr(p_win, buffer);
	noecho();

	werase(p_win);
	hide_panel(prompt_panel);

	answer = malloc(sizeof(char) * strlen(buffer));
	strcpy(answer, buffer);

	return answer;
}

void
prompt_add_note(short tag, short priority)
{
	int intput;
	int n_pri = DEFAULT_PRIORITY;
	char *input;
	char *label;
	char *label2;
	char *label3;
	char *n_tag = DEFAULT_TAG;
	Note n_aux;

	if (tag && priority) {
		label = "Adding [note], priority and tag";
		label2 = "Adding note, [priority] and tag";
		label3 = "Adding note, priority and [tag]";
	} else if (tag) {
		label = "Adding [note] and tag";
		label2 = "";
		label3 = "Adding note and [tag]";
	} else if (priority) {
		label = "Adding [note] and priority";
		label2 = "Adding note and [priority]";
		label3 = "";
	} else {
		label = "Adding note";
		label2 = "";
		label3 = "";
	}

	/* show cursor */
	curs_set(1);

	input = prompt_user("Note text [blank to cancel]: ", label, ALIGN_LEFT);

	if (is_blank(input))
		return;

	while (in_str('|', input) || strlen(input) > main_win_w - 2)
		input = prompt_user("Note too long or '|' detected, try again:\n|", label, ALIGN_LEFT);

	n_aux = new_note(input);

	if (priority) {
		intput = str2int(prompt_user("Note priority [0-9]: ", label2, ALIGN_LEFT));

		while (intput < 0 || 9 < intput)
			intput = str2int(prompt_user("Type a valid number please [0-9]: ", label2, ALIGN_LEFT));

		n_pri = intput;
	}

	if (tag) {
		input = prompt_user("On which tag? [blank for default]: ", label3, ALIGN_LEFT);
		if (!is_blank(input)) {
			n_tag = input;
		}
	}

	note_set_priority(n_pri, n_aux);
	tag_add_note(n_aux, n_tag);

	/* hide cursor again */
	curs_set(0);

	reload_main_win();
}

void
prompt_show_details(Note n)
{
	char *label;
	char *mark;

	mark = (note_get_completed(n) ? COMPLETE_MARK : INCOMPLETE_MARK);
	label = calloc(40 + strlen(mark), sizeof(char));

	sprintf(label, "Completed: %s    |    Priority: %d", mark, note_get_priority(n));
	prompt_user(label, "Selected Note Details", ALIGN_CENTER);
}

int
prompt_delete_tag(void)
{
	Tag t;
	int r = 0;
	char *answer = malloc(sizeof(char));

	curs_set(1);
	answer = prompt_user("Delete which tag? [blank to cancel] ", "Deleting tag", ALIGN_LEFT);
	if (!is_blank(answer)) {

		t = tag_get(answer);
		if (!t) {
			prompt_user("Tag does not exist", "Deleting tag", ALIGN_CENTER);
		} else {

			if (SELECTED_TAG(answer)) {
				top_tag_index = top_tag_index->next;
				sel_tag_index = top_tag_index;
			}

			tag_del(t, &global_tag_list);
			tag_del_circ(t, &circ_tag_list);
			r = 1;

			update_panels();
		}
	}
	curs_set(0);
	return r;
}
