#ifndef _UTILS_H
#define _UTILS_H

/* FUNCTION PROTOTYPES */
int str2int(char *srt);
int is_blank(char *str);
char *substr(char *str, int begin, int end);
char *remove_quotes(char *str);
char *concatenate(char *inicio, char *fim);
int find_split_spot(char *str, int max_length);
char *read_until_separator(char sep, FILE *file);

#endif
