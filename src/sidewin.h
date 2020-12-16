#ifndef _SIDEWIN_H
#define _SIDEWIN_H

/* GLOBAL VARIABLES */
extern WINDOW *side_win;
extern struct d_list *top_tag_index;
extern struct d_list *circ_tag_list;

/* FUNCTION PROTOTYPES */
int anote_panel_height(Tag t);
void anote_show_panel(PANEL *p);
void build_tag_panels(void);
PANEL *anote_new_panel(Tag t);
PANEL *anote_search_panel(Tag t);
void delete_panels(void);
void reload_side_win(void);
void scroll_panels(void);
void side_win_actions(int c);


#endif
