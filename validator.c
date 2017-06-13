#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "common.h"
#include "validator.h"


static bool is_valid_integer(char *str);
static bool is_valid_bool(char *str);
static bool is_valid_double(char *str);
static bool is_valid_char(char *str, size_t size);
static bool is_valid_varchar(char *str, size_t size);
static bool is_valid_datetime(char *str);


bool
is_valid_field(char *str, header_type type, size_t size)
{
	if (type == INTEGER)
		return is_valid_integer(str);
	else if (type == BOOL)
		return is_valid_bool(str);
	else if (type == DOUBLE)
		return is_valid_double(str);
	else if (type == CHAR)
		return is_valid_char(str, size);
	else if (type == VARCHAR)
		return is_valid_varchar(str, size);
	else if (type == DATETIME)
		return is_valid_datetime(str);
	else
		return true;
}

static bool
is_valid_integer(char *str)
{
	if (*str == '\0' || str == NULL)
		return false;

	if (*str == '0' && strlen(str) > 1)
		return false;

	if (*str == '-' || *str == '+')
		str = str + 1;

	while (*str != '\0' && isdigit(*str)) {
		str = str + 1;
	}

	if (*str != '\0')
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
is_valid_char(char *str, size_t size)
{
	if (str == NULL)
		return false;

	if (strlen(str) > size ||
		 strlen(str) > VALID_CHAR_SIZE_MAX)
		return false;

	return true;
}

static bool
is_valid_varchar(char *str, size_t size)
{
	if (str == NULL)
		return false;

	if (strlen(str) > size ||
		 strlen(str) > VALID_VARCHAR_SIZE_MAX)
		return false;

	return true;
}

static bool
is_valid_datetime(char *str)
{
	char *ptr = NULL;
	int tmp = 0;

	if (str == NULL || *str == '\0')
		return false;

	while (isdigit(*str))
		str = str + 1;

	if (*str != '/')
		return false;
	else {
		str = str + 1;
		ptr = str;
	}

	while (isdigit(*str))
		str = str + 1;

	if (*str != '/')
		return false;
	else {
		tmp = atoi(ptr);
		if (tmp <= 0 || tmp > 12)
			return false;
		else {
			str = str + 1;
			ptr = str;
		}
	}

	while (isdigit(*str))
		str = str + 1;

	if (*str == ' ') {
		tmp = atoi(ptr);
		if (tmp < 1 || tmp > 31)
			return false;
		str = str+1;
		ptr = str;
	}
	else {
		return false;
	}

	while (isdigit(*str))
		str = str + 1;

	if (*str == ':') {
		tmp = atoi(ptr);
		if (tmp < 0 || tmp > 23)
			return false;
		str = str+1;
		ptr = str;
	}
	else {
		return false;
	}

	while (isdigit(*str))
		str = str + 1;

	if (*str == ':') {
		tmp = atoi(ptr);
		if (tmp < 0 || tmp > 59)
			return false;
		str = str+1;
		ptr = str;
	}
	else {
		return false;
	}

	while (isdigit(*str) && *str != '\0')
		str = str + 1;

	tmp = atoi(ptr);
	if (tmp < 0 || tmp > 59)
		return false;

	if (*str != '\0')
		return false;

	return true;
}
