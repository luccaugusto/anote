/* TYPES */
struct d_list {
	void *obj;
	struct d_list *next;
};

/* FUNCTION PROTOTYPES */
int d_list_length(struct d_list *list);
struct d_list *new_list_node(void);
void delete_list(struct d_list *list);
void d_list_add(void *obj, struct d_list **list, size_t obj_size);
void d_list_add_before(void *obj, struct d_list *ref, struct d_list **list, size_t obj_size);
void d_list_add_circular(void *obj, struct d_list **list, size_t obj_size);
void d_list_del_pos(int pos, struct d_list *list);
void d_list_del_obj(void *obj, struct d_list *list);
