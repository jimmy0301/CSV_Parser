#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "common.h"
#include "validator.h"
#include "header_info.h"


static bool is_valid_integer(char *str);
static bool is_valid_bool(char *str);
static bool is_valid_double(char *str);
static bool is_valid_string(char *str, size_t size);
static bool is_valid_datetime(char *str);


bool
is_valid_field(header_type type, char *str, size_t size)
{
	if (type == INTEGER)
		return is_valid_integer(str);
	else if (type == BOOL)
		return is_valid_bool(str);
	else if (type == DOUBLE)
		return is_valid_double(str);
	else if (type == CHAR || type == VARCHAR)
		return is_valid_string(str, size);
	else if (type == DATETIME)
		return is_valid_datetime(str);
	else
		return true;
}

static bool
is_valid_integer(char *str)
{
	int cnt = 0;

	if (*str == '\0' || str == NULL)
		return false;

	if (*str == '0')
		return false;

	if (*str == '-' || *str == '+')
		str = str + 1;

	while (*str != '\0' && isdigit(*str)) {
		cnt++;
		str = str + 1;
	}

	if (cnt != strlen(str))
		return false;

	return true;
}

static bool
is_valid_bool(char *str)
{
	if (str == NULL || *str == '\0')
		return false;

	if (strcmp(str, VALID_BOOL_TRUE_STR) == 0 ||
		 strcmp(str, VALID_BOOL_FALSE_STR) == 0)
		return true;
	else
		return false;
}

static bool
is_valid_double(char *str)
{
	char *remain_str;

	if (*str == '\0' || str == NULL)
		return false;

	strtod(str, &remain_str);
	if (*remain_str != '\0')
		return false;
	return true;
}

static bool
is_valid_string(char *str, size_t size)
{
	if (str == NULL)
		return false;

	if (strlen(str) > size)
		return false;

	return true;
}

static bool
is_valid_datetime(char *str)
{
	return true;
}
