/* TYPES */
struct element {
	struct *note;
	struct element *next;
}

struct d_list {
	struct element *elements;
	int size;
};

/* FUNCTION PROTOTYPES */
struct d_list *init_list(void);
void delete_list(struct d_list *list);
int add(struct note *note, struct d_list *list);
int del_pos(struct d_list *list, int pos);
int del_note(struct d_list *list, struct note *note);
