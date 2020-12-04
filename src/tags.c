/* HEADERS */
#include <string.h>

#include "d_list.h"
#include "notes.h"
#include "tags.h"

/* adds a note to a tag, creates new tag if tag is not found */
void
tag_add(struct note *note, struct d_list *tags_list, char *tag)
{
	struct d_list *i;
	struct tag *t;

	/* find tag on the list */
	for (i = tags_list; i->next != NULL &&
			(strcmp(tag, i->obj->name) != 0) ; i=i->next);

	t = i;
	/* tag not found, create a new one */
	if (strcmp(tag, i->obj->name) != 0) {

		t = new_tag();
		t->name = tag;
		d_list_add(t, tags_list);

	}

	d_list_add(note, t->notes);
	t->notes_number++;
}
