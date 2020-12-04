/* TYPES */
struct d_list {
	struct note *note;
	struct d_list *next;
};

/* FUNCTION PROTOTYPES */
struct d_list *init_list(void);
void delete_list(struct d_list *list);
void d_list_add(struct note *note, struct d_list *list);
void del_pos(int pos, struct d_list *list);
void del_note(struct note *note, struct d_list *list);
