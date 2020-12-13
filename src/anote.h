/* CONTROL FLOW MACROS */
#define EXIT_IF(x,s) if (x){fprintf(strderr, "%s", s); exit(errno);}
#define RETURN_IF(x,e) if (x) return -(errno = e);

#define MAX(x,y) (x > y ? x : y)

/* TYPES */
typedef enum {
	EFINOOP = 1, /* file could not be opened */
} ANOTE_ERROR;
