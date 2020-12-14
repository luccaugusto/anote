#ifndef _PROMPT_H
#define _PROMPT_H

/* FUNCTION PROTOTYPES */
char *prompt_user(char *question, int align_center);
void prompt_add_note(short tag, short priority);
int prompt_delete_tag(void);
void print_align_center(WINDOW *win, int start_y, int start_x, int width, char *string/*, chtype color*/);

/* GLOBAL VARIABLES */
extern WINDOW *prompt_win;
extern PANEL *prompt_panel;

extern int prompt_win_h;
extern int prompt_win_w;

#endif
