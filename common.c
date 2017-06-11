#include <stdio.h>
#include <ctype.h>

#include "common.h"
#include "err_code.h"


int
add_head_space(char *dest, size_t space_size)
{
	int i = 0;

	if (dest == NULL)
		return ERR_PARAM_NULL;

	for (i = 0; i < space_size; i++) {
		*(dest + i) = ' ';
	}

	return SUCCESS;
}

int
remove_tail_space(char *str)
{
	if (str == NULL)
		return ERR_PARAM_NULL;

	while(isspace(*str)) {
		*str = '\0';
		str = str - 1;
	}

	return SUCCESS;
}

int
remove_dquote(char *src, char *dest)
{
	char *ptr = NULL;
	char *ptr2 = NULL;
	
	if (src == NULL || dest == NULL)
		return ERR_PARAM_NULL;

	ptr = src;
	ptr2 = dest;
	while (*ptr != '\0') {
		*ptr2 = *ptr;
		if (*ptr != '"') {
			ptr++;
		}
		else {
			ptr = ptr + 2;
		}
		ptr2++;
	}

	return SUCCESS;
}
