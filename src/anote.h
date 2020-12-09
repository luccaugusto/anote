/* CONTROL FLOW MACROS */
#define EXIT_IF(x,s) if (x){fprintf(strderr, "%s", s); exit(ERR);}
#define RETURN_IF(x,e) if (x) return ERR;

/* TYPES */
typedef enum {
	EFINOOP = 1, /* file could not be opened */
	ENOTSUP,
} ANOTE_ERROR;
