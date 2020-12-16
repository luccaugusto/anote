/* HEADERS */
#include <ncurses.h>
#include <panel.h>
#include <string.h>
#include <stdlib.h>

#include "anote.h"
#include "config.h"
#include "list.h"
#include "note.h"
#include "tag.h"
#include "cli.h"
#include "utils.h"
#include "sidewin.h"
#include "prompt.h"

/* GLOBAL VARIABLES */
WINDOW *prompt_win;
PANEL *prompt_panel;

int prompt_win_h;
int prompt_win_w;

char *
prompt_user(char *question, char *label,int align_center)
{
	char *answer = malloc(sizeof(char));
	WINDOW *p_win = panel_window(prompt_panel);;

	box(p_win, 0, 0);
	draw_headers(p_win, prompt_win_h, prompt_win_w, label);

	if (align_center)
		print_align_center(p_win, HEADER_HEIGHT, 1, prompt_win_w, question);
	else
		mvwprintw(p_win, HEADER_HEIGHT, 1, question);

	show_panel(prompt_panel);

	echo();
	wgetstr(p_win, answer);
	noecho();

	werase(p_win);
	hide_panel(prompt_panel);

	return answer;
}

void
prompt_add_note(short tag, short priority)
{
	int intput;
	int create_panel = 0;
	int n_pri = DEFAULT_PRIORITY;
	char *input;
	char *label;
	char *label2;
	char *label3;
	char *n_tag = DEFAULT_TAG;
	Note n_aux;
	Tag t_aux;
	PANEL *p;

	if (tag && priority) {
		label = "Adding [note], priority and tag";
		label2 = "Adding note, [priority] and tag";
		label3 = "Adding note, priority and [tag]";
	} else if (tag) {
		label = "Adding [note] and tag";
		label3 = "Adding note and [tag]";
	} else if (priority) {
		label = "Adding [note] and priority";
		label2 = "Adding note and [priority]";
	} else {
		label = "Adding note";
	}

	input = prompt_user("Note text [blank to cancel]: ", label, ALIGN_LEFT);

	if (is_blank(input))
		return;

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
			if (tag_get(n_tag) == NULL)
				create_panel = 1;
		}
	}

	note_set_priority(n_pri, n_aux);
	tag_add_note(n_aux, n_tag);

	if (create_panel) {
		t_aux = tag_get(n_tag);
		p = anote_new_panel(t_aux);
		d_list_add_circ(p, &panel_list, sizeof(*p));
		d_list_add_circ(t_aux, &circ_tag_list, tag_get_size());
	}

	/* reload main window or side window */
	if (strcmp(n_tag, d_tag_name) == 0)
		reload_main_win();
	else
		reload_side_win();
}

int
prompt_delete_tag(void)
{
	Tag t;
	PANEL *p;
	int r = 0;
	char *answer = malloc(sizeof(char));

	answer = prompt_user("Delete which tag? [blank to cancel] ", "Deleting tag", ALIGN_LEFT);
	if (!is_blank(answer)) {

		t = tag_get(answer);
		if (!t) {
			prompt_user("Tag does not exist", "Deleting tag", ALIGN_CENTER);
		} else {

			/* deletes the panel containing the tag */
			p = anote_search_panel(t);
			if (p == NULL)
				return 0;

			d_list_del_obj(p, &panel_list);
			del_panel(p);
			tag_del(t, &global_tag_list);
			tag_del_circ(t, &circ_tag_list);
			r = 1;
		}
	}
	return r;
}

void /* colors not yet supported */
print_align_center(WINDOW *win, int start_y, int start_x, int width, char *string/*, chtype color*/)
{
	int length, x, y;

	win = (win == NULL) ? stdscr : win;

	getyx(win, y, x);

	x = (start_x == 0) ? x : start_x;
	y = (start_y == 0) ? y : start_y;

	width = (width == 0) ? DEFAULT_WIDTH : width;

	length = strlen(string);
	x = start_x + (width - length) / 2;
	/*wattron(win, color);*/ /* colors not yet supported */
	mvwprintw(win, y, x, "%s", string);
	/*wattroff(win, color);*/ /* colors not yet supported */
	refresh();
}
