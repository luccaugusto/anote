#ifndef _SIDEWIN_H
#define _SIDEWIN_H

/* GLOBAL VARIABLES */
extern PANEL *t_panel;
extern WINDOW *side_win;

/* FUNCTION PROTOTYPES */
int anote_panel_height(Tag t);
void anote_show_panel(PANEL *p);
void build_tag_panels(WINDOW *window);
PANEL *anote_new_panel(WINDOW *window, Tag t);
PANEL *anote_search_panel(Tag t);
void reload_side_win(void);
void side_win_actions(int c);


#endif
