#include <stdio.h>
#include <ctype.h>

#include "common.h"
#include "err_code.h"


int remove_tail_space(char *str)
{
	if (str == NULL)
		return ERR_PARAM_NULL;

	while(isspace(*str)) {
		*str = '\0';
		str = str - 1;
	}

	return SUCCESS;
}
