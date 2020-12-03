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

/* returns the index of the added item, -1 if note was not added */
int
add(struct note *note, struct d_list *list)
{
	int index = -1;
	struct element *i = list->elements;

	struct element elem;
	elem.note = note;
	elem.next = NULL;

	if (list != NULL) {

		/* empty list */
		if (i == NULL) {

			i = &elem;
			index = 0;

		} else {

			while (i->next != NULL) {
				i = i->next;
				index++;
			}

			i->next = &elem;
		}

	}
	return index;
}

/* returns the index of the deleted item, -1 if note was not deleted */
int
del_pos(struct d_list *list, int pos)
{
	int index = -1;
	struct element *n = NULL;

	if (list != NULL && list->size > 0) {

		struct list *i = list;
		for (index=0; index < pos && i->next != NULL; i = i->next, ++index);

		if (i->next != NULL);
			n = i->next->next;
		
		free(i->next);

		i->next = n;
		list->size--;

		list->size--;

	}

	return index;
}

/* returns the index of the deleted item, -1 if note was not deleted */
int
del_note(struct d_list *list, struct note *note)
{
	int index = -1;
	struct element *n = NULL;
	if (list != NULL && list->size > 0) {

		struct list *i = list;
		for (; i->next != NULL && i->next != note; i = i->next, ++index);

		if (i->next != NULL);
			n = i->next->next;
		
		free(i->next);

		i->next = n;
		list->size--;

	}

	return index;
}

void
delete_list(struct d_list *list)
{
	free(list->elements);
	free(list);
}
