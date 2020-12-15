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

/* GLOBAL VARIABLES */
int side_y_offset = HEADER_HEIGHT;
int side_x_offset = 1;
struct d_list *top_pan_index;
WINDOW *side_win;

/* FUNCTION DEFINITIONS */

int /* calculate panel height */
anote_panel_height(Tag t)
{
	if (t == NULL)
		return 0;

	int p_height;
	int t_n_number = tag_get_n_number(t);

	if (0 <= t_n_number && t_n_number < MAX_NOTES_PER_PANEL)
		p_height = t_n_number;
	else
		p_height = MAX_NOTES_PER_PANEL;

 	/* height + header + borders */
	p_height+=HEADER_HEIGHT + 3;

	return p_height;
}

void
anote_show_panel(PANEL *p)
{
	struct d_list *j;
	WINDOW *p_window;
	int k;
	int p_height;
	int y_offset = HEADER_HEIGHT;
	int x_offset = 1;
	char *text;

	if (!p)
		return;

	p_height = anote_panel_height((Tag) panel_userptr(p));
	p_window = panel_window(p);

	box(p_window, 0, 0);
	draw_headers(p_window, p_height, side_win_w - 2, tag_get_name((Tag) panel_userptr(p)));

	/* limit of MAX_NOTES_PER_PANEL */
	j = tag_get_notes((Tag) panel_userptr(p));
	k = 0;
	while (j->obj && k < MAX_NOTES_PER_PANEL) {
		text = note_get_text(j->obj);

		/* truncate the string if its longer than side_win_w-borders characters */
		if (strlen(text) > side_win_w - 2) {
			/* -5 = 2 borders and ... */
			text = substr(text, 0, side_win_w - 7);
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

void
build_tag_panels(void)
{
	Tag t;
	PANEL *p;
	struct d_list *i;

	i = global_tag_list;
	while (i->obj) {
		t = i->obj;
		if (tag_get_name(t) != d_tag_name) {
			p = anote_new_panel(t);
			anote_show_panel(p);
		}

		if (i->next) i = i->next;
		else break;
	}

	update_panels();
}

PANEL * /* create panel and insert it on list */
anote_new_panel(Tag t)
{
	PANEL *p = NULL;
	WINDOW *p_window;
	int p_height = anote_panel_height(t);

	p_window = derwin(side_win, p_height, side_win_w - 2, side_y_offset, side_x_offset);
	if (p_window) {
		p = new_panel(p_window);
		set_panel_userptr(p, t); /* panel_userptr point to its tag */
		d_list_add_circ(p, &panel_list, sizeof(*p));
		side_y_offset+=p_height;
	}

	return p;
}

PANEL * /* returns the panel containing t */
anote_search_panel(Tag t)
{
	PANEL *p = NULL;
	struct d_list *i;

	i = top_pan_index;
	while (panel_userptr(i->obj) != t) {
		if (i->next) i = i->next;
		else break;
	}

	/* found it */
	if (panel_userptr(i->obj) == t) {
		p = i->obj;
	}

	return p;
}
/* repositions every panel setting the top_pan_index to
 * the first position available and working from there.
 */
void
scroll_panels(void)
{
	int new_y = HEADER_HEIGHT + 10;
	int new_x = 1;
	struct d_list *i;

	i = top_pan_index;
	do {

		move_panel(i->obj, new_y, new_x);
		anote_show_panel(i->obj);
		new_y += anote_panel_height((Tag) panel_userptr(i->obj));

		i = i->next;
	} while (i->obj && i != top_pan_index);

	doupdate();
}

void
reload_side_win(void)
{
	struct d_list *i;

	CLEAR_WINDOW(side_win);

	i = top_pan_index;
	do {
		werase(panel_window(i->obj));
		anote_show_panel(i->obj);

		i = i->next;
	} while (i->obj && i != top_pan_index);

	update_panels();
}

void /* tag manipulations */
side_win_actions(int c)
{
	switch(c)
	{
		case KEY_ENTER:
			/* DISPLAY SELECTED TAG */
			break;
		case 'j':      /* FALLTHROUGH */
		case KEY_DOWN:
			CLEAR_WINDOW(side_win);
			//top_pan_index = top_pan_index->next;
			scroll_panels();
			reload_side_win();
			break;
		case 'k':      /* FALLTHROUGH */
		case KEY_UP:
			break;

		case KEY_NPAGE:
			break;

		case KEY_PPAGE:
			break;
		case A_TAB:
			cur_win = main_win;
			break;
		default:
			break;
	}

}
