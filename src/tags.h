struct tag {
	struct d_list *notes;
	int notes_number;
	char *name;
}

void tag_add(struct note *note, struct d_list *tags_list, char *tag);
