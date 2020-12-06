/* TYPES */
typedef struct note * Note;

/* FUNCTION PROTOTYPES */
char *note_get_text(Note n);
int note_get_priority(Note n);
int note_get_size(void);

Note new_note(char *text);
void edit_note_pri(int n_pri, Note n);
void edit_note_text(char *n_text, Note n);
