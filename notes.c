#include "list.h"
#include "notes.h"

/* GLOBAL VARIABLES */
int g_id = 0; /* global id */
struct list note_list;

struct note
new_note(char *text)
{
	struct note n_note;
	n_note.note = text;
	n_note.priority = 0;
}
