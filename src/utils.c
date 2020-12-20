/* HEADERS */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

/* FUNCTION DEFINITION */
/* ************************** *
       STRING MANIPULATION
 * ************************** */

int
str2int(char *str)
{
	int l = strlen(str);
	int val = 1; /* value of the position (units, decimals, hundreds) */
	int ret = 0;

	/* starts from the end of the string (less significative position)
	 * and multiply by the value of its position
	 */
	for (int i=l-1; i>=0; --i) {
		ret += (str[i]-48) * val;
		val*=10;
	}
	return ret;
}

int
is_blank(char *str)
{
	int blank = 1;
	int i = 0;
	while (i < strlen(str)) {
		if (!(str[i] == ' ' || str[i] == '\n' || str[i] == '\t')) {
			blank = 0;
			break;
		}
		++i;
	}
	return blank;
}

char *
substr(char *str, int begin, int end)
{
	char *ret;

	if (end > strlen(str))
		end = strlen(str);

	/* empty string*/
	if (begin >= end)
		return "\0";

	ret = malloc(sizeof(char) * (end-begin));

	memcpy(ret, &str[begin], (end-begin));

	return ret;
}

/* remove quotation marks and spaces outside of them */
char *
remove_quotes(char *str)
{
	char *ret;
	int strsize = strlen(str);
	int inside = 0;
	int c = 0; /* return counter */
	int type = '"';
	ret = (char *) malloc(strsize-2);

	int i=0;
	/* finds which quotation mark is used */
	for (; i<strsize; ++i) {
		if (str[i] == '"') {
			type = '"';
			break;
		}
		if (str[i] == '\'') {
			type = '\'';
			break;
		}
	}

	for (; i<strsize; ++i) {
		if (str[i] == type) {
			inside = !inside;
			continue;
		}
		if (inside)
			ret[c++] = str[i];
	}

	return ret;
}

char *
concatenate(char *beginning, char *ending)
{
	char *ret;
	int beg_size = strlen(beginning);
	int end_size = strlen(ending);

	/* adds \0 position */
	ret = (char *) malloc(sizeof(char) * (beg_size + end_size + 1));

	for (int i=0; i < beg_size; ++i)
		ret[i] = beginning[i];

	for (int i=0; i < end_size; ++i) {
		ret[beg_size+i] = ending[i];
	}

	ret[beg_size + end_size] = '\0';

	return ret;
}

/* finds a position to split the string
 * that won't split a word in half
 * and will be the closest to max_length
 */
int
find_split_spot(char *str, int max_length)
{
	max_length = (max_length > strlen(str)) ?
		strlen(str) : max_length;

	/* finds the space immediatelly before max_length */
	while (str[max_length] != ' ')
		max_length--;

	return max_length;
}

char *
read_until_separator(char sep, FILE *file)
{
	char *ret = "";
	int c;
	while ((c = fgetc(file)) != EOF && c != sep) {
		ret = concatenate(ret, (char *) &c);
	}

	return ret;
}
