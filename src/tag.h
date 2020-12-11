/* TYPES */
typedef struct tag *Tag;

/* FUNCTION PROTOTYPES */
Tag new_tag(char *name);
Tag tag_get(char *name);

struct d_list *tag_get_notes(Tag t);
char *tag_get_name(Tag t);
int tag_get_n_number(Tag t);
size_t tag_get_size();
Note tag_search_note(char *needle_text, struct tag *haystack);

void tag_del(Tag t, struct d_list **list);
void ch_note_tag(Note n, Tag n_tag, Tag cur_tag);
void tag_add_note(Note note, char *tag);

/* GLOBAL VARIABLES */
extern struct d_list *global_tag_list;
