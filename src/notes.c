#include "notes.h"

/* FUNCTION DEFINITIONS */
struct note *
new_note(char *text)
{
	struct note *n_note = (struct note *) malloc(sizeof(struct note));
	n_note->text = text;
	n_note->priority = 0;
	n_note->tag = "general";

	return n_note;
}

void
edit_note_text(char *n_text, struct note *n)
{
	n->text = n_text;
}

void
edit_note_pri(int n_pri, struct note *n)
{
	n->priority = n_pri;
}
