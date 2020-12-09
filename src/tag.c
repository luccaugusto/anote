/* HEADERS */
#include <assert.h>
#include <string.h>

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
	struct tag *t;
	/* find tag on the list */
	i = global_tag_list;
	while (i->next) {

		if (i->obj) {
			t = (i->obj);
 			if (strcmp(name, t->name) == 0)
				break;
		}

		i = i->next;
	}

	/* reached and of list and did not match any tags */
	if (!i)
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
	if (t->name)
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

/* deletes a tag and all its notes */
void
tag_del(struct tag *t, struct d_list **list)
{
	struct d_list *i;
	struct d_list *aux;
	struct tag *t_aux;

	for (i = *list; i->next && i->next->obj != t; i = i->next);
	aux = i->next;

	/* note found */
	if (aux->obj == t) {
		i->next = aux->next;

		t_aux = aux->obj;
		delete_list(t_aux->notes);
		free(aux);
	}

	t->notes_number--;
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

		if (i->obj) {
			t = (i->obj);
 			if (strcmp(tag_name, t->name) == 0)
				break;
		}

		i = i->next;
	}

	/* tag not found, create a new one */
	if (i->obj == NULL || strcmp(tag_name, t->name) != 0) {
		t = new_tag(tag_name);
		d_list_add(t, &global_tag_list, sizeof(struct tag));
	}

	/* keep notes ordered by priotiry */
	n_pri = note_get_priority(note);
	ref = t->notes;

	/* ref is the first note with lower priority */
	while (ref->next) {
		if (!(ref->obj && n_pri >= note_get_priority(ref->obj)))
				break;
		ref = ref->next;
	}

	d_list_add_before(note, ref, &t->notes, note_get_size());
	t->notes_number++;
}
