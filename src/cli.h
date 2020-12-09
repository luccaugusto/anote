#define DEFAULT_WIDTH 80
#define HEADER_HEIGHT 3

/* types */
typedef enum {
	NOTE_ONLY = 0,
	NOTE_COMP,
	NOTE_PRIO,
	NOTE_COMP_PRIO,
} DisplayModes;

/* GLOBAL VARIABLES */
extern struct d_list *global_tag_list;

/* FUNCTION PROTOTYPES */
void start_anote_cli(void);
