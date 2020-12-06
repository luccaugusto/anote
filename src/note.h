/* TYPES */
struct note {
	int priority; /* 0:max priority */
	char *text;
};

/* FUNCTION PROTOTYPES */

/* TODO */
char *note_get_text(struct note *n);
int note_get_priority(struct note *n);
/* ENDTODO */

struct note *new_note(char *text);
void edit_note_pri(int n_pri, struct note *n);
void edit_note_text(char *n_text, struct note *n);
