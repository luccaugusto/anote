#ifndef _UTILS_H
#define _UTILS_H

/* FUNCTION PROTOTYPES */
int str2int(char *srt);
int is_blank(char *str);
char *substr(char *str, int begin, int end);
char *remove_quotes(char *str);
char *concatenate(char *str, const char *suffix);
char *prepend(const char *prefix, char *str);
int find_split_spot(char *str, int max_length);
int in_str(char needle, char *haystack);
char *read_until_separator(char sep, FILE *file);

#endif
