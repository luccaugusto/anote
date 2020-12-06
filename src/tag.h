/* TYPES */
struct tag {
	struct d_list *notes;
	int notes_number;
	char *name;
};

extern struct d_list *global_tag_list;

/* FUNCTION PROTOTYPES */
struct tag *new_tag(char *name);
void tag_del(struct tag *t, struct d_list *tags_list);
void tag_add_note(struct note *note, struct d_list **tags_list, char *tag);
void tag_edit(struct note *n, struct tag *n_tag, struct tag *cur_tag);
