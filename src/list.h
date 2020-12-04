/* TYPES */
struct d_list {
	struct note *notes;
	int size;
};

/* FUNCTION PROTOTYPES */
struct d_list *init_list(void);
void delete_list(struct d_list *list);
void add(struct note *note, struct d_list *list);
void del_pos(struct d_list *list, int pos);
int del_note(struct d_list *list, struct note *note);
