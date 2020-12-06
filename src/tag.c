/* HEADERS */
#include <assert.h>
#include <string.h>

#include "anote.h"
#include "list.h"
#include "note.h"
#include "tag.h"

/* adds a note to a tag, creates new tag if tag is not found */

struct tag *
new_tag(char *name)
{
	struct tag *t = malloc(sizeof(struct tag));
	t->notes = new_list_node();
	t->name = name;
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

void
tag_add_note(Note note, struct d_list **list, char *tag_name)
{
	struct d_list *i;
	struct tag *t;

	/* find tag on the list */
	i = *list;
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
		d_list_add(t, list, sizeof(struct tag));
		//d_list_add(&t, &list);
	}

	d_list_add(note, &t->notes, note_get_size());
	//d_list_add(&note, &t.notes);
	t->notes_number++;
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
