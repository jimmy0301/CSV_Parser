#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "header_info.h"
#include "err_code.h"


int
header_init(header_t *header, size_t header_cnt)
{
	if (header == NULL)
		return ERR_PARAM_NULL;

	if (header_cnt == 0)
		return ERR_PARAM_INVAL;

	memset(header, 0, sizeof(header_t)*header_cnt);
	return SUCCESS;
}

static header_type
get_header_type(char *type_str)
{
	if (strncmp(type_str, HEADER_TYPE_INTEGER_STR, strlen(HEADER_TYPE_INTEGER_STR)) == 0) {
		return INTEGER;
	}
	else if (strncmp(type_str, HEADER_TYPE_BOOL_STR, strlen(HEADER_TYPE_BOOL_STR)) == 0) {
		return BOOL;
	}
	else if (strncmp(type_str, HEADER_TYPE_DOUBLE_STR, strlen(HEADER_TYPE_DOUBLE_STR)) == 0) {
		return DOUBLE;
	}
	else if (strncmp(type_str, HEADER_TYPE_CHAR_STR, strlen(HEADER_TYPE_CHAR_STR)) == 0) {
		return CHAR;
	}
	else if (strncmp(type_str, HEADER_TYPE_VARCHAR_STR, strlen(HEADER_TYPE_VARCHAR_STR)) == 0) {
		return VARCHAR;
	}
	else if (strncmp(type_str, HEADER_TYPE_DATETIME_STR, strlen(HEADER_TYPE_DATETIME_STR)) == 0) {
		return DATETIME;
	}

	return NO_TYPE;
}

int
header_parse(char *header_str, header_t *header, size_t *header_cnt)
{
	char *ptr = NULL, *ptr2 = NULL, *ptr3 = NULL;

	if (header_str == NULL || header == NULL)
		return ERR_PARAM_NULL;

	ptr = header_str;
	while (isspace(*ptr))
		ptr = ptr + 1;

	while ((ptr2 = strchr(ptr, ',')) != NULL) {
		*ptr2 = '\0';
		ptr2 = ptr2 + 1;
		if ((ptr3 = strchr(ptr, ' ')) != NULL) {
			*ptr3 = '\0';
			header[(*header_cnt)].header_index = (*header_cnt) + 1;
			snprintf(header[(*header_cnt)].header_name, HEADER_NAME_SIZE_MAX, "%s", ptr);
			ptr3 = ptr3 + 1;
			header[(*header_cnt)].type = get_header_type(ptr3);
			if (header[(*header_cnt)].type == CHAR) {
				ptr3 = ptr3 + strlen(HEADER_TYPE_CHAR_STR) + 1;
				header[(*header_cnt)].size = atoi(ptr3);
			}
			else if (header[(*header_cnt)].type == VARCHAR) {
				ptr3 = ptr3 + strlen(HEADER_TYPE_VARCHAR_STR) + 1;
				header[(*header_cnt)].size = atoi(ptr3);
			}

			while (isspace(*ptr2))
				ptr2 = ptr2 + 1;

			ptr = ptr2;
			(*header_cnt)++;

			if ((*header_cnt) > (HEADER_INFO_CNT_MAX - 1))
				return SUCCESS;
		}
	}

	if ((ptr3 = strchr(ptr, ' ')) != NULL) {
		*ptr3 = '\0';
		header[(*header_cnt)].header_index = (*header_cnt) + 1;
		snprintf(header[(*header_cnt)].header_name, HEADER_NAME_SIZE_MAX, "%s", ptr);
		ptr3 = ptr3 + 1;
		header[(*header_cnt)].type = get_header_type(ptr3);
		if (header[(*header_cnt)].type == CHAR) {
			ptr3 = ptr3 + strlen(HEADER_TYPE_CHAR_STR) + 1;
			header[(*header_cnt)].size = atoi(ptr3);
		}
		else if (header[(*header_cnt)].type == VARCHAR) {
			ptr3 = ptr3 + strlen(HEADER_TYPE_VARCHAR_STR) + 1;
			header[(*header_cnt)].size = atoi(ptr3);
		}
		(*header_cnt)++;
	}

	return SUCCESS;
}

void
header_dump(header_t *header, size_t header_cnt)
{
	int i;
	for (i = 0; i < header_cnt; i++) {
		printf("index = %zd\n", header[i].header_index);
		printf("header_name = %s\n", header[i].header_name);
		printf("header_type = %d\n", header[i].type);
		printf("size = %zd\n", header[i].size);
	}

	return;
}
