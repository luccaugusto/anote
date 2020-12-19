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
int SIDE_WIN_COLORS;
int side_x_offset = 1;
int side_y_offset = HEADER_HEIGHT;
struct d_list *top_tag_index;
struct d_list *sel_tag_index;
struct d_list *circ_tag_list;
struct d_list *panel_list;
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
	p_height+=HEADER_HEIGHT + 2;

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
	char *name;
	chtype color;

	if (!p)
		return;


	p_height = anote_panel_height((Tag) panel_userptr(p));
	p_window = panel_window(p);
	name = tag_get_name((Tag) panel_userptr(p));

	color = COLOR_PAIR(UNSELECTED_COLORS);
	if (strcmp(tag_get_name(sel_tag_index->obj), name) == 0)
		color = COLOR_PAIR(SELECTED_COLORS);

	wattrset(p_window, color);
	box(p_window, 0, 0);
	draw_headers(p_window, p_height, side_win_w - 2, name, color);

	j = tag_get_notes(tag_get(name));
	k = 0;
	/* limit of MAX_NOTES_PER_PANEL */
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

		CONTINUE_IF(j, j->next);
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

	side_y_offset = HEADER_HEIGHT;

	circ_tag_list = new_list_node_circ();

	/* first tag is the first of the circular list */
	top_tag_index = circ_tag_list;
	sel_tag_index = top_tag_index;

	i = global_tag_list;
	while (i->obj) {
		t = i->obj;
		if (tag_get_name(t) != d_tag_name) {
			d_list_add_circ(t, &circ_tag_list, tag_get_size());
			p = anote_new_panel(t);
			anote_show_panel(p);
		}

		CONTINUE_IF(i, i->next);
	}

	update_panels();
	doupdate();
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
	Tag t_aux;
	PANEL *p = NULL;
	struct d_list *i; /* traverse tag_list   */
	char *name = tag_get_name(t);;

	i = panel_list;
	do {
		t_aux = (Tag) panel_userptr(i->obj);

		if (strcmp(name, tag_get_name(t_aux)) == 0)
			break;

		i = i->next;
	} while (i != panel_list);

	/* found it */
	if (strcmp(name, tag_get_name(t_aux)) == 0)
		p = i->obj;

	return p;
}

/* repositions every panel setting the top_tag_index to
 * the first position available and working from there.
 */
void
scroll_panels(void)
{
	side_y_offset = HEADER_HEIGHT;
	struct d_list *i;
	PANEL *p;
	Tag tag;

	i = top_tag_index;
	do {

		tag = i->obj;
		p = anote_search_panel(tag);
		if (p) {
			move_panel(p, side_y_offset, side_x_offset);
			side_y_offset += anote_panel_height(tag);
		} else {
			anote_new_panel(tag);
		}

		i = i->next;
	} while (side_y_offset < side_win_h && i != top_tag_index);

	update_panels();
	doupdate();
}

void
delete_panels(void)
{
	struct d_list *i;

	i = panel_list;
	do {

		del_panel(i->obj);

	} while (i != panel_list);

	delete_list(&panel_list);

	panel_list = new_list_node_circ();
}

void
reload_side_win(void)
{
	struct d_list *i;
	PANEL *p;

	werase(side_win);
	draw_headers(side_win, side_win_h, side_win_w, "Other Notes", COLOR_PAIR(SIDE_WIN_COLORS));

	i = top_tag_index;
	do {
		p = anote_search_panel(i->obj);

		werase(panel_window(p));
		anote_show_panel(p);

		i = i->next;
	} while (i != top_tag_index);
}

void /* tag manipulations */
side_win_actions(int c)
{
	switch(c)
	{
		case A_CR:      /* FALLTHROUGH */
		case A_NEWLINE:
			CLEAR_WINDOW(side_win);
			load_displayed_tag(tag_get_name(sel_tag_index->obj));
			delete_panels();
			delete_list_circ(&circ_tag_list);
			build_tag_panels();
			reload_side_win();
			reload_main_win();
			cur_win = main_win;
			break;

		case 'j':      /* FALLTHROUGH */
		case KEY_DOWN:
			CLEAR_WINDOW(side_win);
			top_tag_index = top_tag_index->next;
			sel_tag_index = top_tag_index;
			delete_panels();
			scroll_panels();
			reload_side_win();
			break;

		case 'k':      /* FALLTHROUGH */
		case KEY_UP:
			CLEAR_WINDOW(side_win);
			top_tag_index = d_list_prev(top_tag_index->obj, &circ_tag_list);
			sel_tag_index = top_tag_index;
			delete_panels();
			scroll_panels();
			reload_side_win();
			break;

		case A_TAB:
			cur_win = main_win;
			MAIN_WIN_COLORS = SELECTED_COLORS;
			SIDE_WIN_COLORS = UNSELECTED_COLORS;
			reload_main_win();
			reload_side_win();
			break;

		default:
			break;
	}

}
