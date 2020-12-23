#ifndef _PROMPT_H
#define _PROMPT_H

/* FUNCTION PROTOTYPES */
char *prompt_user(char *question, char *label, int align_center);
void prompt_add_note(short tag, short priority);
int prompt_delete_tag(void);

/* GLOBAL VARIABLES */
extern WINDOW *prompt_win;
extern PANEL *prompt_panel;

extern char *buffer;

extern int prompt_win_h;
extern int prompt_win_w;

#endif
