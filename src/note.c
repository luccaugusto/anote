#include "note.h"

/* TYPES */
struct note {
	int priority; /* 0:max priority */
	int completed;
	char *text;
};

/* FUNCTION DEFINITIONS */
Note
new_note(char *text)
{
	Note n_note = malloc(sizeof(Note));
	n_note->text = text;
	n_note->priority = 0;
	n_note->completed = 0;

	return n_note;
}


void
note_set_priority(int n_pri, Note n)
{
	n->priority = n_pri;
}

void
note_set_completed(int c, Note n)
{
	n->completed = c;
}

void
note_set_text(char *n_text, Note n)
{
	strcpy(n->text, n_text);
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

int
note_get_completed(Note n)
{
	return n->completed;
}
