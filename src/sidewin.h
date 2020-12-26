#ifndef _SIDEWIN_H
#define _SIDEWIN_H

#define SELECTED_TAG(x) (strcmp(tag_get_name(sel_tag_index->obj), x) == 0)

/* GLOBAL VARIABLES */
extern WINDOW *side_win;
extern struct d_list *top_tag_index;
extern struct d_list *sel_tag_index;
extern struct d_list *circ_tag_list;
extern char *side_w_header;
extern int SIDE_WIN_COLORS;
extern int expanded;

/* FUNCTION PROTOTYPES */
int anote_panel_height(Tag t, int full);
void anote_show_panel(PANEL *p);
void build_tag_panels(void);
PANEL *anote_new_panel(Tag t);
PANEL *anote_search_panel(Tag t);
void delete_panels(void);
void reload_side_win(void);
void color_side_win(void);
void scroll_panels(void);
void side_win_actions(int c);


#endif
