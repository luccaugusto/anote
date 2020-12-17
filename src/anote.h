#ifndef _ANOTE_H
#define _ANOTE_H

/* CONTROL FLOW MACROS */
#define EXIT_IF(x,s) if (x){fprintf(strderr, "%s", s); exit(errno);}
#define RETURN_IF(x,e) if (x) return -(errno = e);
#define CONTINUE_IF(x,y) if (y) x = y; else break;

#define MAX(x,y) (x > y ? x : y)

/* TYPES */
typedef enum {
	EFINOOP = 1, /* file could not be opened */
} ANOTE_ERROR;

/* GLOBAL VARIABLES */
extern struct d_list *global_tag_list;
extern FILE *notes_file;
extern char *notes_file_name;
extern char *arg_tag_name;

#endif
