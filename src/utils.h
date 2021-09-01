#ifndef _UTILS_H
#define _UTILS_H

/* FUNCTION PROTOTYPES */
int str2int(char *srt);
int is_blank(char *str);
void substr(char *str, char *substr, int begin, int end);
void remove_quotes(char *str, char *clearstr);
char *concatenate(char *str, const char *suffix);
char *prepend(const char *prefix, char *str);
int find_split_spot(char *str, int max_length);
int in_str(char needle, char *haystack);
void read_until_separator(char sep, char *buffer, FILE *file);

#endif
