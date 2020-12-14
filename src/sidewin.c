/* Manipulate the panels of the side window */
/* HEADERS */
#include <ncurses.h>
#include <panel.h>
#include <string.h>

#include "anote.h"
#include "config.h"
#include "list.h"
#include "note.h"
#include "tag.h"
#include "cli.h"
#include "utils.h"
#include "sidewin.h"

/* FUNCTION DEFINITIONS */

int /* calculate panel height */
anote_panel_height(Tag t)
{
	int p_height;
	int t_n_number = tag_get_n_number(t);

	if (0 <= t_n_number && t_n_number < MAX_NOTES_PER_PANEL)
		p_height = t_n_number;
	else
		p_height = MAX_NOTES_PER_PANEL;

 	/* height + header + borders */
	p_height += HEADER_HEIGHT + 3;

	return p_height;
}

void
build_tag_panels(WINDOW *window)
{
	Tag t;
	PANEL *p;
	struct d_list *i;
	int x_offset = 1;
	int y_offset = HEADER_HEIGHT;

	for(i = global_tag_list; i->next; i=i->next) {
		t = i->obj;
		if (tag_get_name(t) != d_tag_name) {
			p = anote_new_panel(window, t, y_offset, x_offset);
			anote_show_panel(p, y_offset, x_offset);

			y_offset += anote_panel_height(t);
		}
	}

	update_panels();
}

void
anote_show_panel(PANEL *p, int y_offset, int x_offset)
{
	struct d_list *j;
	WINDOW *p_window;
	int k;
	int p_height;
	char *text;

	p_height = anote_panel_height((Tag) panel_userptr(p));
	p_window = panel_window(p);

	box(p_window, 0, 0);
	draw_headers(p_window, p_height, side_win_w, tag_get_name((Tag) panel_userptr(p)));

	/* limit of MAX_NOTES_PER_PANEL */
	j = tag_get_notes((Tag) panel_userptr(p));
	k = 0;
	while (j->obj && k < MAX_NOTES_PER_PANEL) {
		text = note_get_text(j->obj);

		/* truncate the string if its longer than side_win_w-borders characters */
		if (strlen(text) > side_win_w - 2) {
			/* -5 = 2 borders and ... */
			text = substr(text, 0, side_win_w - 5);
			text = concatenate(text, "...");
		}

		mvwprintw(p_window, y_offset, x_offset, text);

		++y_offset;
		++k;

		if (j->next) j = j->next;
		else break;
	}

	if (k < tag_get_n_number((Tag) panel_userptr(p)))
		mvwprintw(p_window, y_offset, x_offset, "+++");
}

PANEL * /* create panel and insert it on list */
anote_new_panel(WINDOW *window, Tag t, int y_offset, int x_offset)
{
	PANEL *p;
	WINDOW *p_window;

	p_window = derwin(window, anote_panel_height(t), side_win_w - 2, y_offset, x_offset);
	p = new_panel(p_window);
	set_panel_userptr(p, t); /* panel_userptr point to its tag */
	d_list_add_circ(p, &panel_list, sizeof(*p));

	return p;
}
