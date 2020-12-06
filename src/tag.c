/* HEADERS */
#include <assert.h>
#include <string.h>

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

void
tag_add_note(struct note *note, struct d_list **list, char *tag_name)
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

	d_list_add(note, &t->notes, sizeof(struct note));
	//d_list_add(&note, &t.notes);
	t->notes_number++;
}

/* deletes a tag and all its notes */
void
tag_del(struct tag *t, struct d_list *list)
{
	struct d_list *i;
	struct d_list *aux;
	struct tag *t_aux;

	for (i = list; i->next != NULL && i->next->obj != t; i = i->next);

	aux = i->next;
	i->next = aux->next;

	t_aux = aux->obj;
	delete_list(t_aux->notes);
	free(aux);
}

/* changes note n from tag cur_tag to tag n_tag */
void
tag_edit(struct note *n, struct tag *n_tag, struct tag *cur_tag)
{
	struct d_list *i;
	struct d_list *aux;

	/* remove n from its tag */
	for (i=cur_tag->notes; i->next != NULL && i->next->obj != n; i = i->next);
	aux = i->next;
	i->next = aux->next;

	/* add n to n_tag */
	d_list_add(&n, &n_tag->notes, sizeof(*n));
	//d_list_add(&n, &n_tag->notes);
}
