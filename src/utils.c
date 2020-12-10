/* HEADERS */
#include <stdio.h>
#include <string.h>

/* FUNCTION DEFINITION */
/* ************************** *
       STRING MANIPULATION
 * ************************** */

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
