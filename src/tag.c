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
	struct tag *t = (struct tag *) malloc(sizeof(struct tag));
	t->notes = init_list();
	t->name = name;
	return t;
}

void
tag_add(struct note *note, struct d_list *tags_list, char *tag_name)
{
	struct d_list *i;
	struct tag *t;

	/* find tag on the list */
	for (i = tags_list; i->next != NULL; i=i->next) {
		t = i->obj;
 		if (strcmp(tag_name, t->name) == 0)
			break;
	}

	/* tag not found, create a new one */
	if (strcmp(tag_name, t->name) != 0) {

		t = new_tag(tag_name);
		d_list_add(t, tags_list);

	}

	d_list_add(note, t->notes);
	t->notes_number++;
}

/* deletes a tag and all its notes */
void
tag_del(struct tag *t, struct d_list *tags_list)
{
	struct d_list *i;
	struct d_list *aux;
	struct tag *t_aux;

	for (i = tags_list; i->next != NULL && i->next->obj != t; i = i->next);

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
	d_list_add(n, n_tag->notes);
}

void
test_tag_note_list_add(void)
{
	struct tag *t = new_tag("test tag add note");
	struct d_list *i = t->notes;

	for (int j=0; j < 10; ++j) {
		d_list_add(new_note("test note"), t->notes);
		i = i->next;
		assert(i != NULL);
		assert(i->obj != NULL);
	}

	delete_list(t->notes);
	free(t);
}

void
test_tag_note_list_del(void)
{
	struct tag *t = new_tag("test tag note del");
	struct d_list *i = t->notes;
	struct d_list *aux;
	int real_size;
	int expected_size = 0;

	for (int j=0; j<10; ++j, i=i->next, ++expected_size) {
		d_list_add(new_note("test"), t->notes);
		assert(i->next != NULL);
		assert(i->next->obj != NULL);
	}


	while (t->notes->next != NULL) {
		real_size = 0;
		del_obj(t->notes->next, t->notes);

		/* count number of objects */
		for (aux=t->notes; aux->next != NULL; aux=aux->next, ++real_size);

		assert(real_size == --expected_size);
	}

	delete_list(t->notes);
	free(t);
}

void
test_tag_note_list_edit()
{
	struct tag *t = new_tag("test tag add note");
	struct tag *n_t = new_tag("test tag edit note");
	struct d_list *i = t->notes;
	struct d_list *aux;
	struct d_list *n_aux;

	int t_size = 0;
	int n_t_size = 0;
	int expected_size = 10;

	for (int j=0; j < expected_size; ++j) {
		d_list_add(new_note("test note"), t->notes);
		i = i->next;
		assert(i != NULL);
		assert(i->obj != NULL);
	}

	i = t->notes;
	while (i->next != NULL) {
		t_size = 0;
		n_t_size = 0;
		tag_edit(i->next->obj, n_t, t);

		/* count number of objects in t*/
		for (aux=i; aux->next != NULL; aux=aux->next, ++t_size);

		/* count number of objects int n_t */
		for (n_aux=n_t->notes; n_aux->next != NULL; n_aux=n_aux->next, ++n_t_size);

		/* check if total number of notes is not altered */
		assert(t_size+n_t_size == expected_size);
	}

	delete_list(t->notes);
	delete_list(n_t->notes);
	free(t);
	free(n_t);
}
