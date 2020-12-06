/* TYPES */
struct tag {
	struct d_list *notes;
	int notes_number;
	char *name;
};

extern struct d_list *global_tag_list;

/* FUNCTION PROTOTYPES */
struct tag *new_tag(char *name);
struct tag *tag_get(char *name);

/* TODO */
char *tag_get_name(struct tag *t);
int tag_get_n_number(struct tag *t);
struct d_list *tag_get_notes(struct tag *t);
/* ENDTODO */

void tag_del(struct tag *t, struct d_list **list);
void ch_note_tag(struct note *n, struct tag *n_tag, struct tag *cur_tag);
void tag_add_note(struct note *note, struct d_list **tags_list, char *tag);
