/* CLI client for the anote application.
 * Provides improved functionalitty over the shell script
 */

/* HEADERS */
#include <stdio.h>

#include "list.h"
#include "notes.h"

struct d_list *notes_list;

int
main()
{

	notes_list = init_list();

	struct note *n = new_note("teste 1");
	add(n, notes_list);

	for (int i=0; i < notes_list->size; ++i) {
		printf("%s\n", notes_list->notes[i].text);
	}

	return 0;
}
