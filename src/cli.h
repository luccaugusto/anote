#ifndef _CLI_H
#define _CLI_H

#define HEADER_HEIGHT 3
#define SELECTED_COLORS 1
#define UNSELECTED_COLORS 2
#define HIGHLIGHT_COLORS 3
#define DEFAULT_COLORS 4

/* types */
typedef enum {
	NOTE_ONLY = 0,
	NOTE_COMP,
	NOTE_PRIO,
	NOTE_COMP_PRIO,
} DisplayModes;

/* GLOBAL VARIABLES */
extern struct d_list *global_tag_list;
extern char *arg_tag_name;

/* FUNCTION PROTOTYPES */
void start_anote_cli(void);

#endif
