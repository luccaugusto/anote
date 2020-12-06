/* HEADERS */
#include <stdlib.h>

#include "list.h"

/* FUNCTION DEFINITIONS */
struct d_list *
new_list_node(void)
{
	struct d_list *list = (struct d_list *) malloc(sizeof(struct d_list));
	list->obj = NULL;
	list->next = NULL;
}

void
d_list_add(void *obj, struct d_list **list, size_t obj_size)
{
	struct d_list *i;
	for (i=*list; i->obj; i = i->next);

	i->next = new_list_node();
	i->obj = malloc(obj_size);

	memcpy(i->obj, obj, obj_size);
}


void
del_pos(int pos, struct d_list *list)
{
	int i;
	struct d_list *j;
	struct d_list *aux;

	/* never delete the head */
	if (list != NULL && list->next != NULL && pos > 0) {

		/* finds the position */
		for (i=0, j=list; i<pos-1 && j->next->next != NULL; ++i, j=j->next);

		aux = j->next;
		j->next = aux->next;
		free(aux);
	}
}

void
del_obj(void *obj, struct d_list *list)
{
	struct d_list *i;
	struct d_list *aux;

	/* never delete the head */
	if (list != NULL && list->next != NULL && obj != list->obj) {

		/* finds the position */
		for (i=list; i->next != NULL && i->next != obj; i=i->next);

		if (i->next == obj) {
			aux = i->next;
			i->next = aux->next;
			free(aux);
		}
	}
}

void
delete_list(struct d_list *list)
{
	/* delete all objects */
	while (list->next != NULL)
		del_obj(list->next,list);

	free(list->next);
	free(list);
}
