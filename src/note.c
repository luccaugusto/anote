/* HEADERS */
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
note_del(Note n)
{
	free(n->text);
	free(n);
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
	n->text = realloc(n->text, sizeof(n_text));
	strcpy(n->text, n_text);
}

char *
note_get_text(Note n)
{
	char *r = "";
	if (n && n->text)
		r = n->text;
	return r;
}

int
note_get_priority(Note n)
{
	int p = 0;
	if (n)
		p = n->priority;
	return p;
}

int
note_get_size(void)
{
	return sizeof(struct note);
}

int
note_get_completed(Note n)
{
	int c = 0;
	if (n)
		c = n->completed;
	return c;
}
