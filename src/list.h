/* TYPES */
struct d_list {
	void *obj;
	struct d_list *next;
};

/* FUNCTION PROTOTYPES */
struct d_list *init_list(void);
void delete_list(struct d_list *list);
void d_list_add(void *obj, struct d_list *list, size_t obj_size);
void del_pos(int pos, struct d_list *list);
void del_obj(void *obj, struct d_list *list);
