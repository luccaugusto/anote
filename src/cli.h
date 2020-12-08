#define DEFAULT_WIDTH 80
#define HEADER_HEIGHT 3

/* TYPES */
typedef enum {
	MAIN_MENU = 0,
	SIDE_MENU,
} Selection;

extern struct d_list *global_tag_list;

void start_anote_cli(void);
