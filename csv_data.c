#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "csv_data.h"
#include "validator.h"
#include "err_code.h"
#include "common.h"


#define FIELD_VAL_SIZE_MAX	1024


static int csv_field_set(csv_field_t *csv_field, header_t *header, char *field_val,
								 bool has_dquote, bool is_empty_str);
static int csv_field_integer_set(csv_field_t **csv_field, char *field_val, bool has_dquote);
static int csv_field_bool_set(csv_field_t **csv_field, char *field_val, bool has_dquote);
static int csv_field_double_set(csv_field_t **csv_field, char *field_val, bool has_dquote);
static int csv_field_char_set(csv_field_t **csv_field, char *field_val,size_t char_size, bool has_dquote, bool is_empty_str);
static int csv_field_varchar_set(csv_field_t **csv_field, char *field_val, size_t varchar_size, bool has_dquote, bool is_empty_str);
static int csv_field_datetime_set(csv_field_t **csv_field, char *field_val, bool has_dquote);


int
csv_data_init(csv_field_t *csv_data[CSV_ROW_SIZE_MAX])
{
	int i;

	if (csv_data == NULL)
		return ERR_PARAM_NULL;

	for (i = 0; i < CSV_ROW_SIZE_MAX; i++) {
		memset(csv_data[i], 0, sizeof(csv_field_t)*CSV_FIELD_SIZE_MAX);
	}

	return SUCCESS;
}

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

/* doesn't remove tail space */
int
csv_content_parse(char *csv_content, size_t content_size, header_t *header,
						size_t header_cnt, char *err_file_name, csv_field_t *csv_data[CSV_ROW_SIZE_MAX],
						size_t *csv_data_size)
{
	char *start_ptr = NULL, *end_ptr = NULL, *field_start = NULL, *ptr = NULL, *orig_field_start = NULL;
	char *row_start = NULL;
	size_t field_cnt = 0;
	size_t row_data = 0;
	size_t dquote_cnt = 0;
	size_t field_val_len = 0;
	char field_val[FIELD_VAL_SIZE_MAX];
	bool is_valid_row_data = true;
	bool is_valid_field_data = true;
	bool is_end_row = false;
	bool need_check = true;
	bool has_dquote = false;
	bool is_empty_str = false;
	FILE *fp_err = NULL;

	if (csv_content == NULL || header == NULL ||
		 err_file_name == NULL || csv_data == NULL)
		return ERR_PARAM_NULL;
	if (csv_content[0] == '\0' || err_file_name[0] == '\0')
		return ERR_PARAM_INVAL;

	if ((fp_err = fopen(err_file_name, "w")) == NULL)
		return ERR_IO_OPEN;

	field_start = csv_content;
	start_ptr = csv_content;
	end_ptr = csv_content + (content_size - 1);

	row_start = field_start;
	while (field_start < end_ptr) {
		memset(field_val, 0, FIELD_VAL_SIZE_MAX);

		/* remove space */
		while (isspace(*field_start))
			field_start++;

		if (*field_start == '"') {
			ptr = field_start + 1;
			orig_field_start = field_start + 1;
			has_dquote = true;
			dquote_cnt++;
			while (dquote_cnt != 0) {
				if (*ptr == '"') {
					dquote_cnt--;
					if ((ptr + 1) <= end_ptr) {
						// """
						if ((*(ptr + 1)) == '"') {
							dquote_cnt++;
							if ((ptr + 2) <= end_ptr)
								ptr = ptr + 2;
							else {
								ptr = end_ptr;
								field_val_len = ptr - field_start - 1;
								field_start = ptr;
								need_check = false;
								is_valid_row_data = false;
								is_valid_field_data = false;
								dquote_cnt = 0;
								break;
							}
						}
						// "123", or "123"\n, or "",
						else if ((*(ptr + 1)) == ',' || (*(ptr + 1)) == '\n') {
							/* field_value = (field_start+1) len = ptr-field_start-2 */
							/* is_valid_field */
							dquote_cnt = 0;
							field_val_len = ptr - field_start;
							if (field_val_len > 1) {
								field_start = field_start + 1;
								field_val_len = ptr - field_start;
							}
							else {
								is_empty_str = true;
								orig_field_start = field_start;
								field_val_len = ptr - field_start + 1;
							}

							if ((ptr + 2) < end_ptr) {
								field_start = ptr + 2;
								if (*(ptr + 1) == '\n') {
									is_end_row = true;
									//row_start = field_start;
								}
							}
							// ptr + 2 >= end_ptr
							else {
								//"123",\0 or "123",\n\0
								if (*(ptr + 1) == ',') {
									need_check = false;
									is_valid_field_data = false;
									is_valid_row_data = false;
								}

								if (*(ptr+1) == '\n') {
									is_end_row = true;
								}

								field_start = end_ptr;
								break;
							}
						}
						// "123"\rXXX
						else if ((*(ptr + 1)) == '\r') {
							if ((ptr + 2) <= end_ptr) {
								//"123"\r\n
								if (*(ptr + 2) == '\n') {
									field_val_len = ptr - orig_field_start;
									if ((ptr + 3) <= end_ptr) {
										field_start = ptr + 3;
									}
									else {
										field_start = end_ptr;
									}
									//row_start = field_start;
								}
								else {
									need_check = false;
									is_valid_field_data = false;
									is_valid_row_data = false;
									break;
								}
							}
							else {
								//*ptr == '"'&& ptr + 2 > end_ptr "123"\r\0
								ptr = end_ptr;
								field_val_len = ptr - orig_field_start + 1;
								field_start = ptr;
								need_check = false;
								is_valid_row_data = false;
								is_valid_field_data = false;
								is_end_row = true;
								break;
							}
						}
						else {
							//invalid "123"123 go \n and output
							is_valid_row_data = false;
							is_valid_field_data = false;
							need_check = false;
							is_end_row = true;
							break;
						}
					}// end if (((ptr + 1)) <= end_ptr)
					else {
						need_check = false;
						is_valid_row_data = false;
						is_valid_field_data = false;
						is_end_row = true;
						break;
					}
				}//end if (*ptr == '"')
				// *ptr != '""'
				else {
					if ((ptr + 1) <= end_ptr) {
						ptr = ptr + 1;
					}
					else {
						field_start = end_ptr;
						need_check = false;
						is_valid_row_data = false;
						is_valid_field_data = false;
						break;
					}
				}
			}//end while(dquote_cnt != 0)
		}//end field_start == '"'
		//,123,456\n
		else if (*field_start == ',') {
			ptr = field_start;
			if (field_start == start_ptr) {
				row_start = field_start;
				is_empty_str = true;
				field_start = ptr + 1;
			}
			else {
				if (*(ptr - 1) == ',' || *(ptr - 1) == '\n') {
					is_empty_str = true;
				}
				if ((ptr + 1) <= end_ptr) {
					if (*(ptr + 1) == ',') {
						if ((ptr + 2) <= end_ptr) {
							field_start = ptr + 2;
						}
						else {
							field_start = end_ptr;
						}
						is_empty_str = true;
					}
					else {
						field_start = ptr + 1;
					}
				}
				else {
					need_check = false;
					is_valid_field_data = false;
					is_valid_row_data = false;
					field_start = end_ptr;
				}
			}
		}
		/* field_start != '"' && field_start != ',' */
		else if (*field_start == '\n') {
			if ((field_start + 1) <= end_ptr) {
				field_start = field_start + 1;
			}
		}
		else {
			ptr = field_start;
			orig_field_start = field_start;
			while (*ptr != ',' && *ptr != '\n') {
				if ((ptr + 1) <= end_ptr)
					ptr++;
				else {
					break;
				}
			}
			if (*ptr == ',') {
				if ((ptr + 1) < end_ptr) {
					if (*(ptr + 1) == '\n') {
						need_check = false;
						is_valid_field_data = false;
						is_valid_row_data = false;
						field_start = ptr + 2;
					}
					else {
						field_start = ptr + 1;
					}
				}
				else if ((ptr + 1) == end_ptr){
					need_check = false;
					is_valid_field_data = false;
					is_valid_row_data = false;
					field_start = end_ptr;
				}
				else {
					field_start = ptr;
				}
				field_val_len = ptr - orig_field_start;
			}
			else if (*ptr == '\n') {
				is_end_row = true;
				if (*(ptr - 1) == '\r') {
					field_val_len = (ptr - 1) - orig_field_start;
				}
				else {
					field_val_len = ptr - orig_field_start;
				}
				if (ptr == end_ptr)
					field_start = ptr;
				else
					field_start = ptr + 1;
				//row_start = field_start;
			}
			//ptr == end_ptr didn't find ',' or  '\n'
			else {
				is_end_row = true;
				field_val_len = ptr - orig_field_start;
				field_start = ptr;
			}
		}

		printf("field_start =%s\n", field_start);
		printf("field_cnt = %zd\n", field_cnt);
		/*printf("need_check = %d\n", need_check);
		printf("is_valid_row_data = %d\n", is_valid_row_data);
		printf("is_valid_field_data = %d\n", is_valid_field_data);
		printf("header_cnt = %zd\n", header_cnt);*/
		if (need_check) {
			printf("=======has_check=====\n");
			if (field_cnt == header_cnt) {
				is_valid_row_data = false;
				is_valid_field_data = false;
			}
			else {
				printf("======check_val=======\n");
				printf("is_empty_str = %d\n", is_empty_str);
				if (is_empty_str) {
					field_val[0] = '\0';
				}
				else {
					if (field_val_len < FIELD_VAL_SIZE_MAX) {
						strncpy(field_val, orig_field_start, field_val_len);
					}
					else {
						is_valid_row_data = false;
						is_valid_field_data = false;
					}
				}
				if (!is_valid_field(field_val, header[field_cnt].type, header[field_cnt].size)) {
					is_valid_field_data = false;
				}
				else {
					is_valid_field_data = true;
				}
			}
		}

		if (is_valid_field_data) {
			printf("valid_field\n");
			csv_field_set(&csv_data[row_data][field_cnt], &header[field_cnt], field_val, has_dquote, is_empty_str);
			printf("field_val = %s\n", field_val);
			field_cnt++;
		}

		printf("is_end_row = %d\n", is_end_row);
		if (!is_valid_row_data || !is_valid_field_data) {
			printf("in error\n");
			while (((ptr + 1) <= end_ptr) && *ptr != '\n') {
				ptr++;
			}
			printf("ptr = %s\n", ptr);
			printf("row_start = %s\n", row_start);
			fwrite(row_start, ptr - row_start + 1, sizeof(char), fp_err);
			if (*ptr == '\n') {
				if ((ptr + 1) <= end_ptr) {
					field_start = ptr + 1;
					row_start = field_start;
				}
				else {
					field_start = ptr;
				}
			}
			else {
				field_start = ptr;
			}
			field_cnt = 0;
			dquote_cnt = 0;
			has_dquote = false;
			is_empty_str = false;
			is_valid_row_data = true;
			is_valid_field_data = true;
			need_check = true;
			is_end_row = false;
			continue;
		}

		if (is_end_row) {
			field_cnt = 0;
			is_valid_row_data = true;
			is_end_row = false;
			row_start = field_start;
			row_data++;
		}

		printf("row_data = %zd\n", row_data);

		printf("==========\n");
		dquote_cnt = 0;
		has_dquote = false;
		is_empty_str = false;
		need_check = true;
		is_valid_field_data = true;
	}

	*csv_data_size = row_data;
	if (fp_err != NULL) {
		fclose(fp_err);
		fp_err = NULL;
	}

	return SUCCESS;
}

static int
csv_field_set(csv_field_t *csv_field, header_t *header, char *field_val, bool has_dquote, bool is_empty_str)
{
	if (csv_field == NULL || header == NULL || field_val == NULL)
		return ERR_PARAM_NULL;

	if ((has_dquote != true &&  has_dquote != false) ||
		 (is_empty_str != true && is_empty_str != false))
		return ERR_PARAM_INVAL;

	csv_field->type = header->type;
	switch (header->type) {
		case INTEGER:
			return csv_field_integer_set(&csv_field, field_val, has_dquote);
			break;
		case BOOL:
			return csv_field_bool_set(&csv_field, field_val, has_dquote);
			break;
		case DOUBLE:
			return csv_field_double_set(&csv_field, field_val, has_dquote);
			break;
		case CHAR:
			return csv_field_char_set(&csv_field, field_val, header->size, has_dquote, is_empty_str);
			break;
		case VARCHAR:
			return csv_field_varchar_set(&csv_field, field_val, header->size, has_dquote, is_empty_str);
			break;
		case DATETIME:
			return csv_field_datetime_set(&csv_field, field_val, has_dquote);
			break;
		default:
			break;
	}

	return SUCCESS;
}

static int
csv_field_integer_set(csv_field_t **csv_field, char *field_val, bool has_dquote)
{
	if (*csv_field == NULL || field_val == NULL)
		return ERR_PARAM_NULL;

	if (field_val[0] == '\0')
		return ERR_PARAM_INVAL;

	(**csv_field).integer = atoi(field_val);
	if (has_dquote) {
		snprintf((**csv_field).output_str, CSV_FIELD_VARCHAR_SIZE_MAX + 3, "\"%s\"", field_val);
	}
	else {
		snprintf((**csv_field).output_str, CSV_FIELD_VARCHAR_SIZE_MAX + 3, "%s", field_val);
	}

	return SUCCESS;
}

static int
csv_field_bool_set(csv_field_t **csv_field, char *field_val, bool has_dquote)
{
	if (*csv_field == NULL || field_val == NULL)
		return ERR_PARAM_NULL;

	if (field_val[0] == '\0')
		return ERR_PARAM_INVAL;

	if (strcmp(field_val, VALID_BOOL_TRUE_STR) == 0) {
		(**csv_field).boolean = true;
	}
	else {
		(**csv_field).boolean = false;
	}

	if (has_dquote) {
		snprintf((**csv_field).output_str, CSV_FIELD_VARCHAR_SIZE_MAX + 3, "\"%s\"", field_val);
	}
	else {
		snprintf((**csv_field).output_str, CSV_FIELD_VARCHAR_SIZE_MAX + 3, "%s", field_val);
	}

	return SUCCESS;
}

static int
csv_field_double_set(csv_field_t **csv_field, char *field_val, bool has_dquote)
{
	if (*csv_field == NULL || field_val == NULL)
		return ERR_PARAM_NULL;

	if (field_val[0] == '\0')
		return ERR_PARAM_INVAL;

	(**csv_field).double_num = strtod(field_val, NULL);
	if (has_dquote) {
		snprintf((**csv_field).output_str, CSV_FIELD_VARCHAR_SIZE_MAX + 3, "\"%s\"", field_val);
	}
	else {
		snprintf((**csv_field).output_str, CSV_FIELD_VARCHAR_SIZE_MAX + 3, "%s", field_val);
	}

	return SUCCESS;
}

static int
csv_field_char_set(csv_field_t **csv_field, char *field_val, size_t char_size, bool has_dquote, bool is_empty_str)
{
	size_t space_size = 0;

	if (*csv_field == NULL || field_val == NULL)
		return ERR_PARAM_NULL;

	if (strlen(field_val) == (char_size - 1)) {
		remove_dquote(field_val, (**csv_field).char_str);
		if (has_dquote) {
			snprintf((**csv_field).output_str, CSV_FIELD_VARCHAR_SIZE_MAX + 3 , "\"%s\"", field_val);
		}
		else {
			snprintf((**csv_field).output_str, CSV_FIELD_VARCHAR_SIZE_MAX + 3, "%s", field_val);
		}
	}
	else if (strlen(field_val) < (char_size - 1)) {
		if (has_dquote) {
			if (is_empty_str) {
				snprintf((**csv_field).output_str, CSV_FIELD_VARCHAR_SIZE_MAX + 3, "\"\"");
				(**csv_field).char_str[0] = '\0';
			}
			else {
				space_size = char_size - strlen(field_val);
				remove_dquote(field_val, ((**csv_field).char_str) + space_size);
				add_head_space((**csv_field).char_str, space_size);
				(**csv_field).output_str[0] = '"';
				add_head_space((**csv_field).output_str + 1, space_size);
				snprintf((**csv_field).output_str + space_size + 1,
						CSV_FIELD_VARCHAR_SIZE_MAX - space_size + 2, "%s\"", field_val);
			}
		}
		else {
			if (is_empty_str) {
				(**csv_field).output_str[0] = '\0';
				(**csv_field).char_str[0] = '\0';
			}
			else {
				space_size = char_size - strlen(field_val);
				remove_dquote(field_val, ((**csv_field).char_str) + space_size);
				add_head_space((**csv_field).char_str, space_size);
				add_head_space((**csv_field).output_str, space_size);
				snprintf((**csv_field).output_str + space_size,
						CSV_FIELD_VARCHAR_SIZE_MAX + 3 - space_size, "%s", field_val);
			}
		}
	}

	return SUCCESS;
}

static int
csv_field_varchar_set(csv_field_t **csv_field, char *field_val, size_t varchar_size,
							 bool has_dquote, bool is_empty_str)
{
	size_t space_size = 0;

	if (*csv_field == NULL || field_val == NULL)
		return ERR_PARAM_NULL;

	if (strlen(field_val) == (varchar_size - 1)) {
		remove_dquote(field_val, (**csv_field).varchar_str);
		if (has_dquote) {
			snprintf((**csv_field).output_str, CSV_FIELD_VARCHAR_SIZE_MAX + 3 , "\"%s\"", field_val);
		}
		else {
			snprintf((**csv_field).output_str, CSV_FIELD_VARCHAR_SIZE_MAX + 3, "%s", field_val);
		}
	}
	else if (strlen(field_val) < (varchar_size - 1)) {
		if (has_dquote) {
			if (is_empty_str) {
				snprintf((**csv_field).output_str, CSV_FIELD_VARCHAR_SIZE_MAX + 3, "\"\"");
				(**csv_field).varchar_str[0] = '\0';
			}
			else {
				space_size = varchar_size - strlen(field_val);
				remove_dquote(field_val, ((**csv_field).varchar_str) + space_size);
				add_head_space((**csv_field).varchar_str, space_size);
				(**csv_field).output_str[0] = '"';
				add_head_space((**csv_field).output_str + 1, space_size);
				snprintf((**csv_field).output_str + space_size + 1,
						CSV_FIELD_VARCHAR_SIZE_MAX - space_size + 2, "%s\"", field_val);
			}
		}
		else {
			if (is_empty_str) {
				(**csv_field).output_str[0] = '\0';
				(**csv_field).varchar_str[0] = '\0';
			}
			else {
				space_size = varchar_size - strlen(field_val);
				remove_dquote(field_val, ((**csv_field).varchar_str) + space_size);
				add_head_space((**csv_field).varchar_str, space_size);
				add_head_space((**csv_field).output_str, space_size);
				snprintf((**csv_field).output_str + space_size,
						CSV_FIELD_VARCHAR_SIZE_MAX + 3 - space_size, "%s", field_val);
			}
		}
	}

	return SUCCESS;
}

static int
csv_field_datetime_set(csv_field_t **csv_field, char *field_val, bool has_dquote)
{
	int year, month, day, hour, minute, second;
	struct tm when;

	if (*csv_field == NULL || field_val == NULL)
		return ERR_PARAM_NULL;

	if (field_val[0] == '\0')
		return ERR_PARAM_INVAL;

	sscanf(field_val, "%d/%d/%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);

	when.tm_year = year;
	when.tm_mon = month;
	when.tm_mday = day;
	when.tm_hour = hour;
	when.tm_min = minute;
	when.tm_sec = second;

	(**csv_field).datetime = (int) mktime(&when);

	if (has_dquote) {
		snprintf((**csv_field).output_str, CSV_FIELD_VARCHAR_SIZE_MAX + 3, "\"%s\"", field_val);
	}
	else {
		snprintf((**csv_field).output_str, CSV_FIELD_VARCHAR_SIZE_MAX + 3, "%s", field_val);
	}

	return SUCCESS;
}

int csv_data_print(csv_field_t *csv_data[CSV_ROW_SIZE_MAX], size_t row_size, size_t column_size)
{
	int i, j;
	for (i = 0; i < row_size; i++) {
		for (j = 0; j < column_size; j++) {
			if (j < (column_size - 1))
				printf("%s,", csv_data[i][j].output_str);
			else if (j == (column_size - 1)){
				printf("%s", csv_data[i][j].output_str);
			}
			else {
				printf("%s\n", csv_data[i][j].output_str);
			}
		}
	}

	return SUCCESS;
}

int
csv_data_write_file(char *file_name, csv_field_t *csv_data[CSV_ROW_SIZE_MAX], size_t row_size, size_t column_size)
{
	FILE *fp = NULL;
	int i, j;

	if (file_name == NULL || csv_data == NULL)
		return ERR_PARAM_NULL;

	if (row_size <= 0 || column_size <= 0)
		return ERR_PARAM_INVAL;

	fp = fopen(file_name, "w");
	if (fp == NULL)
		return ERR_IO_OPEN;

	for (i = 0; i < row_size; i++) {
		for (j = 0; j < column_size; j++) {
			//printf("output_field = %s\n", csv_data[i][j].output_str);
			if (j != (column_size - 1)) {
				if (csv_data[i][j].output_str[0] == '\0') {
					fprintf(fp, ",");
				}
				else {
					fprintf(fp, "%s,", csv_data[i][j].output_str);
				}
			}
			else {
				fprintf(fp, "%s\n", csv_data[i][j].output_str);
			}
		}
	}

	if (fp != NULL) {
		fclose(fp);
		fp = NULL;
	}

	return SUCCESS;
}
