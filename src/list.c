/* HEADERS */
#include <assert.h>
#include <stdlib.h>

#include "notes.h"
#include "list.h"

/* FUNCTION DEFINITIONS */
struct d_list *
init_list(void)
{
	struct d_list *list = (struct d_list *) malloc(sizeof(struct d_list));
	list->next = NULL;
	return list;
}

void
d_list_add(void *obj, struct d_list *list)
{
	struct d_list *i;
	if (list != NULL) {

		for (i=list; i->next != NULL; i=i->next);

		i->next = (struct d_list *) malloc(sizeof(struct d_list));
		i->next->obj = obj;
		i->next->next = NULL;
	}
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
		for (i=list; i->next != obj; i=i->next);

		aux = i->next;
		i->next = aux->next;
		free(aux);
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

void
test_d_list_add(void)
{
	struct d_list *n_list = init_list();
	struct d_list *i = n_list;

	for (int j=0; j < 10; ++j) {
		d_list_add(new_note("test"), n_list);
		i = i->next;
		assert(i != NULL);
		assert(i->obj != NULL);
	}

	delete_list(n_list);
}

void
test_d_list_del(void)
{
	struct d_list *n_list = init_list();
	struct d_list *i = n_list;
	struct d_list *aux;
	int real_size;
	int expected_size = 0;

	for (int j=0; j<10; ++j, i=i->next, ++expected_size) {
		d_list_add(new_note("test"), n_list);
		assert(i->next != NULL);
	}


	while (n_list->next != NULL) {
		real_size = 0;
		del_obj(n_list->next, n_list);

		/* count number of objects */
		for (aux=n_list; aux->next != NULL; aux=aux->next, ++real_size);

		assert(real_size == --expected_size);
	}

	delete_list(n_list);
}
