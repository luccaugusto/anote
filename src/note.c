/* HEADERS */
#include <stdlib.h>
#include <string.h>

#include "note.h"

/* TYPES */
struct note {
	int priority; /* 0:max priority */
	int completed;
	char text[MAX_NOTE_LENGTH];
};

/* FUNCTION DEFINITIONS */
Note
new_note(char *text)
{
	if (strlen(text) > MAX_NOTE_LENGTH)
		return NULL;

	Note n_note = malloc(sizeof(struct note));
	strncpy(n_note->text, text, strlen(text));
	n_note->priority = 0;
	n_note->completed = 0;

	return n_note;
}

void
note_del(Note n)
{
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
note_set_text(char *text, Note n)
{
	if (strlen(text) > MAX_NOTE_LENGTH)
		return;

	strncpy(n->text, text, strlen(text));
}

char *
note_get_text(Note n)
{
	char *r = malloc(sizeof(char) * MAX_NOTE_LENGTH);
	if (n && n->text)
		strncpy(r, n->text, strlen(n->text));

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
