#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "csv_data.h"
#include "err_code.h"
#include "common.h"


#define FIELD_VAL_SIZE_MAX	1024

char*
csv_file_read(char *file_name, size_t *file_size)
{
	FILE *fp = NULL;
	char *file_content = NULL;

	printf("file_name = %s\n", file_name);
	if (file_name == NULL || file_size == NULL)
		return NULL;

	if ((fp = fopen(file_name, "r")) == NULL) {
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	*file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	printf("file size =%zd\n", *file_size);
	if (*file_size == 0)
		return NULL;

	file_content = (char *)malloc(sizeof(char)*(*file_size + 1));
	if (file_content == NULL) {
		fclose(fp);
		fp = NULL;
		return NULL;
	}

	if (fread(file_content, sizeof(char), *file_size, fp) != *file_size) {
		free(file_content);
		file_content = NULL;
		fclose(fp);
		fp = NULL;
		return NULL;
	}

	if (fp != NULL) {
		fclose(fp);
		fp = NULL;
	}

	return file_content;
}

int
csv_content_parse(char *csv_content, size_t content_size, header_t *header,
						size_t header_cnt, csv_field_t *csv_data[CSV_ROW_SIZE_MAX])
{
	char *end_ptr = NULL, *field_start = NULL, *ptr = NULL;
	size_t field_cnt = 0;
	size_t row_data = 0;
	size_t dquote_cnt = 0;
	char field_val[FIELD_VAL_SIZE_MAX];
	bool has_content_dquote = FALSE;
	bool is_valid_row_data = TRUE;

	if (csv_content == NULL || header == NULL || csv_data == NULL)
		return ERR_PARAM_NULL;
	if (csv_content[0] == '\0')
		return ERR_PARAM_INVAL;

	field_start = csv_content;
	end_ptr = csv_content + (content_size - 1);

	while (field_start < end_ptr) {
		memset(field_val, 0, FIELD_VAL_SIZE_MAX);
		while (isspace(*field_start))
			field_start++;
		ptr = field_start + 1;
		if (*field_start == '"') {
			dquote_cnt++;
			while (dquote_cnt != 0) {
				if (*ptr == '"') {
					dquote_cnt--;
					if ((ptr + 1) <= end_ptr) {
						// "123""
						if ((*(ptr + 1)) == '"') {
							dquote_cnt++;
							if ((ptr + 2) <= end_ptr)
								ptr = ptr + 2;
							else {
								ptr = end_ptr;
								if ((ptr - field_start - 1) < FIELD_VAL_SIZE_MAX) {
									strncpy(field_val, field_start, ptr - field_start - 1);
									printf("%s\n", field_val);
								}
								field_start = ptr;
								field_cnt++;
								break;
							}
						}
						else if ((*(ptr + 1)) == ',' || (*(ptr + 1)) == '\n') {
							/* field_value = (field_start+1) len = ptr-field_start-2 */
							/* is_valid_field */
							dquote_cnt = 0;
							if ((ptr - field_start) == 1) {
								
							}
							else {
								field_start = field_start + 1;
								if ((ptr - field_start) < FIELD_VAL_SIZE_MAX) {
									strncpy(field_val, field_start, ptr - field_start);
									printf("%s\n", field_val);
								}
								else {
									strncpy(field_val, field_start, FIELD_VAL_SIZE_MAX - 1);
								}
							}
							if ((ptr + 2) <= end_ptr) {
								ptr = ptr + 2;
							}
							else {
								ptr = end_ptr;
							}
							field_start = ptr;
							field_cnt++;
							break;
						}
						// "123"\r
						else if ((*(ptr + 1)) == '\r') {
							if ((ptr + 2) <= end_ptr) {
								//"123"\r\n
								if (*(ptr + 2) == '\n') {
									field_start = field_start + 1;
									if ((ptr - field_start - 1) < FIELD_VAL_SIZE_MAX) {
										strncpy(field_val, field_start, ptr - field_start - 1);
										printf("%s\n", field_val);
									}
									else {
										strncpy(field_val, field_start, FIELD_VAL_SIZE_MAX - 1);
									}
								}
								else {
									ptr = ptr + 2;
								}
							}
							else {
								ptr = end_ptr;
								field_start = field_start + 1;
								if ((ptr - field_start + 1) < FIELD_VAL_SIZE_MAX) {
									strncpy(field_val, field_start, ptr - field_start + 1);
									printf("%s\n", field_val);
								}
								else {
									strncpy(field_val, field_start, FIELD_VAL_SIZE_MAX - 1);
								}
								field_start = ptr;
								is_valid_row_data = FALSE;
								field_cnt++;
								break;
							}
						}
						else {
							//"123"123
							printf("===yes===\n");
							if ((ptr + 1) <= end_ptr) {
								is_valid_row_data = FALSE;
								ptr = ptr + 1;
								dquote_cnt = 1;
							}
							else {
								ptr = end_ptr;
								field_start = end_ptr;
								field_cnt++;
								break;
							}
						}
					}
				}
				else {
					if ((ptr + 1) <= end_ptr) {
						ptr = ptr + 1;
					}
					else {
						ptr = end_ptr;
						field_cnt++;
						break;
					}
				}
			}
		}
		is_valid_row_data = TRUE;
		field_cnt = 0;
		ptr = ptr + 1;
	}

	return SUCCESS;
}
