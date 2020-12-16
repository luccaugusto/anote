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

		if (i->next) i = i->next;
		else break;
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
	Note needle;

	for (i = tag_get_notes(haystack); i->next; i = i->next) {
		needle = i->obj;
		if (strcmp(needle_text, note_get_text(needle)) == 0)
			break;
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

	for (i = *list; i->next && i->next->obj != t; i = i->next);
	aux = i->next;

	/* tag found */
	if (aux->obj == t) {
		i->next = aux->next;

		for (i = t->notes; i->next; i = i->next)
			note_del(i->obj);

		delete_list(&(t->notes));
		free(aux);
	}
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

/* changes note n from tag cur_tag to tag n_tag */
void
ch_note_tag(Note n, struct tag *n_tag, struct tag *cur_tag)
{
	struct d_list *i;
	struct d_list *aux;

	/* remove n from its tag */
	for (i=cur_tag->notes; i->next != NULL && i->next->obj != n; i = i->next);

	aux = i->next;
	i->next = (aux) ? aux->next : NULL;

	/* add n to n_tag */
	d_list_add(&n, &n_tag->notes, note_get_size());
}

void /* adds a note to the tag ordered by priority */
tag_add_note(Note note, char *tag_name)
{
	struct d_list *ref;
	struct d_list *i;
	struct tag *t;
	int n_pri;

	/* find tag on the list */
	i = global_tag_list;
	while (i->next) {

		t = i->obj;
		if (strcmp(tag_name, tag_get_name(t)) == 0)
			break;

		i = i->next;
	}

	/* tag not found, create a new one */
	if (i->obj == NULL || strcmp(tag_name, tag_get_name(t)) != 0) {

		t = new_tag(tag_name);
		t->notes->obj = note;

		d_list_add(t, &global_tag_list, sizeof(struct tag));

	} else {

		/* keep notes ordered by priotiry */
		n_pri = note_get_priority(note);
		ref = tag_get_notes(t);

		/* ref is the first note with lower priority */
		while (ref->next) {
			if (!(ref->obj && n_pri >= note_get_priority(ref->obj)))
				break;
			ref = ref->next;
		}

		d_list_add_before(note, ref, &t->notes, note_get_size());
		tag_set_n_number(tag_get_n_number(t) + 1, t);
	}
}

void
tag_del_note(Note note, char *tag_name)
{
	struct d_list *i;
	struct tag *t;

	/* find tag on the list */
	i = global_tag_list;
	while (i->next) {

		if (i->obj) {
			t = (i->obj);
 			if (strcmp(tag_name, tag_get_name(t)) == 0)
				break;
		}

		i = i->next;
	}

	/* tag not found, nothing to do */
	if (i->obj == NULL || strcmp(tag_name, tag_get_name(t)) != 0)
		return;

	d_list_del_obj(note, &t->notes);
	tag_set_n_number(tag_get_n_number(t) - 1, t);
}
