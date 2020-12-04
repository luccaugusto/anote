/* HEADERS */
#include <stdlib.h>

#include "notes.h"
#include "list.h"

/* FUNCTION DEFINITIONS */
struct d_list *
init_list(void)
{
	struct d_list *list = (struct d_list *) malloc(sizeof(struct d_list));
	list->size = 0;
	return list;
}

void
add(struct note *note, struct d_list *list)
{
	int index = -1;

	if (list != NULL) {

		/* adds a position to the array */
		list->notes = (struct note *) realloc(list->notes,
				(list->size+1) * sizeof(struct note));

		list->notes[list->size++] = *note;

	}
}

void
del_pos(struct d_list *list, int pos)
{
	if (list != NULL && list->size > 0) {

		free(&list->notes[pos]);

		/* shift items */
		for (int i=pos; i < list->size-1; ++i)
			list->notes[i] = list->notes[i+1];

		/* removes a position from the array */
		list->notes = (struct note *) realloc(list->notes,
				(list->size-1) * sizeof(struct note));

		list->size--;

	}
}

int
del_note(struct d_list *list, struct note *note)
{
	int i=-1;

	if (list != NULL && list->size > 0) {

		/* finds the position */
		for (i=0;i < list->size && &list->notes[i] != note; ++i);

		if (&list->notes[i] == note)
			del_pos(list, i);
		/* else note not found */
	}

	return i;
}

void
delete_list(struct d_list *list)
{
	free(list->notes);
	free(list);
}
