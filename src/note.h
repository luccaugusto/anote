/* TYPES */
typedef struct note *Note;

/* FUNCTION PROTOTYPES */
Note new_note(char *text);
void note_del(Note n);
void note_set_priority(int n_pri, Note n);
void note_set_completed(int c, Note n);
void note_set_text(char *n_text, Note n);

char *note_get_text(Note n);
int note_get_priority(Note n);
int note_get_size(void);
int note_get_completed(Note n);
