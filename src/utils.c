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
concatenate(char *str, const char *suffix)
{
	int l_str = strlen(str);
	int l_suffix = strlen(suffix);

	str = realloc(str, sizeof(char) * (l_str + l_suffix + 1));

	memcpy(&str[l_str], suffix, l_suffix);
	str[l_str + l_suffix] = '\0';

	return str;
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

int
in_str(char needle, char *haystack)
{
	int i = 0;
	int len = strlen(haystack);
	while (i < len) {
		if (haystack[i++] == needle)
			return 1;
	}
	return 0;
}

char *
read_until_separator(char sep, FILE *file)
{
	char *ret = calloc(1, sizeof(char));
	int c;
	while ((c = fgetc(file)) != EOF && c != sep) {
		ret = concatenate(ret, (char *) &c);
	}

	return ret;
}

