/* HEADERS */
#include <stdlib.h>
#include <string.h>

#include "list.h"

/* FUNCTION DEFINITIONS */
int
d_list_length(struct d_list **list)
{
	int l = 0;
	struct d_list *i;
	if ((*list)) {
		i=(*list);
		while (i->obj) {
			++l;

			if (i->next) i = i->next;
			else break;
		}
	}
	return l;
}

struct d_list *
new_list_node(void)
{
	struct d_list *list = (struct d_list *) malloc(sizeof(struct d_list));
	list->obj = NULL;
	list->next = NULL;
	return list;
}

struct d_list *
new_list_node_circ(void)
{
	struct d_list *list = (struct d_list *) malloc(sizeof(struct d_list));
	list->obj = NULL;
	list->next = list;
	return list;
}

void /* append to list */
d_list_append(void *obj, struct d_list **list, size_t obj_size)
{
	struct d_list *i;

	i=*list;
	/* insert on head */
	if (i->obj == NULL) {

		i->obj = malloc(obj_size);
		memcpy(i->obj, obj, obj_size);

	} else {

		for (; i->next; i = i->next);

		i->next = new_list_node();
		i->next->obj = malloc(obj_size);
		memcpy(i->next->obj, obj, obj_size);
	}

}

void /* adds obj before ref, if ref not found, adds to end of the list*/
d_list_add_before(void *obj, struct d_list *ref, struct d_list **list, size_t obj_size)
{
	struct d_list *i;
	struct d_list *aux;

	/* has to add new head */
	if (ref == *list) {

		(*list) = new_list_node();
		(*list)->next = ref;
		(*list)->obj = malloc(obj_size);
		memcpy((*list)->obj, obj, obj_size);

	} else {

		/* finds the reference */
		i = *list;
		while (i->next && i->next != ref)
			i = i->next;

		aux = i->next;
		i->next = new_list_node();
		i->next->next = aux;
		i->next->obj = malloc(obj_size);
		memcpy(i->next->obj, obj, obj_size);
	}
}

void /* adds last object and points it to the head to make a circular list */
d_list_add_circ(void *obj, struct d_list **list, size_t obj_size)
{
	struct d_list *i;

	i = *list;
	/* adds to the head */
	if (i->obj == NULL) {

		i->obj = malloc(obj_size);
		memcpy(i->next->obj, obj, obj_size);

	} else {
		for (; i->next != *list; i = i->next);
		i->next = new_list_node();
		i->next->obj = malloc(obj_size);
		i->next->next = *list;

		memcpy(i->next->obj, obj, obj_size);
	}

}

void /* removes obj node from list, does not free obj */
d_list_del_obj(void *obj, struct d_list **list)
{
	struct d_list *i;
	struct d_list *aux;

	i=*list;
	if (i->obj == obj) {

		aux = *list;
		*list = (*list)->next;
		free(aux);

	} else {

		/* finds the position */
		for (; i->next && i->next->obj != obj; i = i->next);

		if (i->next->obj == obj) {
			aux = i->next;
			i->next = i->next->next;
			free(aux);
		}
	}
}

void /* removes obj node from circular list, does not free obj */
d_list_del_obj_circ(void *obj, struct d_list **list)
{
	struct d_list *i;
	struct d_list *aux;

	i=*list;
	/* reposition the head */
	if (i->obj == obj) {

		aux = *list;
		*list = (*list)->next;
		free(aux);

	} else {

		/* finds the position */
		do {
			if (i->next->obj == obj)
				break;

			i = i->next;
		} while (i != *list);

		if (i->next->obj == obj) {
			aux = i->next;
			i->next = i->next->next;
			free(aux);
		}
	}
}

void /* delete all objects */
delete_list(struct d_list **list)
{
	free(*list);
}

void /* delete all objects in circular list*/
delete_list_circ(struct d_list **list)
{
	struct d_list *aux;

	while ((*list)->next != *list) {
		aux = (*list)->next;
		(*list)->next = aux->next;
		free(aux);
	}

	free(*list);
}

struct d_list *
d_list_find(void *obj, struct d_list **list)
{
	struct d_list *i;

	i = *list;
	for (; i->next && i->obj != obj; i = i->next);

	if (i->obj != obj)
		i = NULL;

	return i;
}

struct d_list *
d_list_prev(void *obj, struct d_list **list)
{
	struct d_list *i;

	i = *list;
	for (; i->next && i->next->obj != obj; i = i->next);

	if (i->next->obj != obj)
		i = NULL;

	return i;
}
