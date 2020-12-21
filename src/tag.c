/* HEADERS */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "anote.h"
#include "list.h"
#include "note.h"
#include "tag.h"

/* TYPES */
struct tag {
	struct d_list *notes;
	int notes_number;
	char *name;
};

/* adds a note to a tag, creates new tag if tag is not found */
struct tag *
new_tag(char *name)
{
	struct tag *t = malloc(sizeof(struct tag));
	t->notes = new_list_node();
	t->name = name;
	t->notes_number = 0;
	return t;
}

/* returns NULL if no tags match name */
struct tag *
tag_get(char *name)
{
	struct d_list *i;
	struct tag *t = NULL;
	/* find tag on the list */
	i = global_tag_list;
	while (i->obj) {

		if (i->obj) {
			t = (i->obj);
 			if (strcmp(name, tag_get_name(t)) == 0)
				break;
		}

		CONTINUE_IF(i, i->next);
	}

	/* tag not found */
	if (strcmp(name, tag_get_name(i->obj)) != 0)
		t = NULL;

	return t;
}

/* sets list = the list of notes of struct tag *t and returns the number of notes in list */
struct d_list *
tag_get_notes(struct tag *t)
{
	struct d_list *i = NULL;
	if (t)
		i = t->notes;
	return i;
}

char *
tag_get_name(struct tag *t)
{
	char *r = "";
	if (t)
		r = (t->name);
	return r;
}

int
tag_get_n_number(struct tag *t)
{
	int r = 0;
	if (t->notes_number)
		r = (t->notes_number);
	return r;
}


size_t
tag_get_size(void)
{
	return sizeof(struct tag);
}

void
tag_set_note_list(struct d_list **list, struct tag *t)
{
	t->notes = *list;
	t->notes_number = d_list_length(list);
}

void
tag_set_n_number(int n_number, struct tag *t)
{
	t->notes_number = n_number;
}

void
tag_set_name(char *name, struct tag *t)
{
	t->name = name;
}

Note
tag_search_note(const char *needle_text, struct tag *haystack)
{
	struct d_list *i;
	Note needle = NULL;

	i = tag_get_notes(haystack);
	while (i->obj) {
		needle = i->obj;
		if (strcmp(needle_text, note_get_text(needle)) == 0)
			break;

		CONTINUE_IF(i, i->next);
	}

	if (strcmp(needle_text, note_get_text(needle)) != 0)
		needle = NULL;

	return needle;
}

/* deletes a tag and all its notes */
void
tag_del(struct tag *t, struct d_list **list)
{
	struct d_list *i;
	struct d_list *aux;

	i = *list;
	aux = NULL;

	/* has to move the head */
	if (i->obj == t) {

		aux = i;
		*list = (*list)->next;

	} else {

		/* finds tag in list */
		while (i->obj) {
			if (i->next->obj == t) {
				aux = i->next;
				i->next = aux->next;
				break;
			}
			CONTINUE_IF(i, i->next);
		}

	}

	/* tag found */
	if (aux->obj == t) {

		/* delete all notes */
		for (i = t->notes; i->next; i = i->next)
			note_del(i->obj);

		delete_list(&(t->notes));
		free(aux);
	}
	/* else nothing to do */
}

/* deletes a tag and all its notes circular list*/
void
tag_del_circ(struct tag *t, struct d_list **list)
{
	struct d_list *i;
	struct d_list *aux;
	char *name = tag_get_name(t);

	/* find the tag on list */
	i = *list;
	do {
		if (strcmp(name, tag_get_name(i->next->obj)) == 0)
			break;

		i = i->next;
	} while (i->next != *list);

	aux = i->next;

	/* tag found */
	if (strcmp(name, tag_get_name(i->next->obj)) == 0) {
		i->next = aux->next;
		free(aux);
	}
}

void /* adds a note to the tag ordered by priority */
tag_add_note(Note note, char *tag_name)
{
	struct d_list *ref;
	struct d_list *i;
	struct tag *t;
	int append = 1;
	int found = 0;
	int n_pri;

	/* find tag on the list */
	i = global_tag_list;
	while (i->obj) {

		t = i->obj;
		if (strcmp(tag_name, tag_get_name(t)) == 0) {
			found = 1;
			break;
		}

		CONTINUE_IF(i, i->next);
	}

	if (i->obj != NULL && found) {

		/* keep notes ordered by priotiry */
		n_pri = note_get_priority(note);
		ref = tag_get_notes(t);

		/* if there is a note with a lower priority, add current note before it
		 * otherwise append it to the list.
		 * 0 = max priority */
		while (ref->obj) {
			if (n_pri < note_get_priority(ref->obj)) {
				append = 0;
				break;
			}

			CONTINUE_IF(ref, ref->next);
		}

		if (append)
			d_list_append(note, &(t->notes), note_get_size());
		else
			d_list_add_before(note, ref, &(t->notes), note_get_size());

		t->notes_number++;

	/* tag not found, create a new one */
	} else {

		t = new_tag(tag_name);
		t->notes->obj = note;
		t->notes_number++;

		d_list_append(t, &global_tag_list, sizeof(struct tag));
	}
}

void
tag_del_note(Note note, char *tag_name)
{
	struct d_list *i;
	struct tag *t;

	/* find tag on the list */
	i = global_tag_list;
	while (i->obj) {

		if (i->obj) {
			t = (i->obj);
 			if (strcmp(tag_name, tag_get_name(t)) == 0)
				break;
		}

		CONTINUE_IF(i, i->next);
	}

	/* tag not found, nothing to do */
	if (i->obj == NULL || strcmp(tag_name, tag_get_name(t)) != 0)
		return;

	d_list_del_obj(note, &t->notes);
	tag_set_n_number(t->notes_number - 1, t);
}
