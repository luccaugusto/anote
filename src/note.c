#include "note.h"

/* TYPES */
struct note {
	int priority; /* 0:max priority */
	char *text;
};

/* FUNCTION DEFINITIONS */
Note
new_note(char *text)
{
	Note n_note = malloc(sizeof(Note));
	n_note->text = text;
	n_note->priority = 0;

	return n_note;
}

void
edit_note_text(char *n_text, Note n)
{
	strcpy(n->text, n_text);
}

void
edit_note_pri(int n_pri, Note n)
{
	n->priority = n_pri;
}

char *
note_get_text(Note n)
{
	return n->text;
}

int
note_get_priority(Note n)
{
	return n->priority;
}

int
note_get_size(void)
{
	return sizeof(struct note);
}
