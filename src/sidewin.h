#ifndef _SIDEWIN_H
#define _SIDEWIN_H

/* FUNCTION PROTOTYPES */
int anote_panel_height(Tag t);
void anote_show_panel(PANEL *p);
void build_tag_panels(WINDOW *window);
PANEL *anote_new_panel(WINDOW *window, Tag t);
PANEL *anote_search_panel(Tag t);


#endif
