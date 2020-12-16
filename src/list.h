#ifndef _LIST_H
#define _LIST_H

/* TYPES */
struct d_list {
	void *obj;
	struct d_list *next;
};

/* FUNCTION PROTOTYPES */
int d_list_length(struct d_list **list);
struct d_list *new_list_node(void);
struct d_list *new_list_node_circ(void);
void delete_list(struct d_list **list);
void delete_list_circ(struct d_list **list);
void d_list_append(void *obj, struct d_list **list, size_t obj_size);
void d_list_add_before(void *obj, struct d_list *ref, struct d_list **list, size_t obj_size);
void d_list_add_circ(void *obj, struct d_list **list, size_t obj_size);
void d_list_del_obj(void *obj, struct d_list **list);
void d_list_del_obj_circ(void *obj, struct d_list **list);
struct d_list *d_list_find(void *obj, struct d_list **list);
struct d_list *d_list_prev(void *obj, struct d_list **list);

#endif
