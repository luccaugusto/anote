struct tag {
	struct d_list *notes;
	int notes_number;
	char *name;
};

void tag_del(struct tag *t, struct d_list *tags_list);
void tag_add_note(struct note *note, struct d_list *tags_list, char *tag);
void tag_edit(struct note *n, struct tag *n_tag, struct tag *cur_tag);
