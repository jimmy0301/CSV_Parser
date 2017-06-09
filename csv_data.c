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

/* doesn't remove tail space */
int
csv_content_parse(char *csv_content, size_t content_size, header_t *header,
						size_t header_cnt, char *err_file_name, csv_field_t *csv_data[CSV_ROW_SIZE_MAX])
{
	char *end_ptr = NULL, *field_start = NULL, *ptr = NULL, *orig_field_start = NULL;
	char *row_start = NULL;
	size_t field_cnt = 0;
	size_t row_data = 0;
	size_t dquote_cnt = 0;
	size_t field_val_len = 0;
	char field_val[FIELD_VAL_SIZE_MAX];
	bool is_valid_row_data = true;
	FILE *fp_err = NULL;

	if (csv_content == NULL || header == NULL ||
		 err_file_name == NULL || csv_data == NULL)
		return ERR_PARAM_NULL;
	if (csv_content[0] == '\0' || err_file_name[0] == '\0')
		return ERR_PARAM_INVAL;

	if ((fp_err = fopen(err_file_name, "w")) == NULL)
		return ERR_IO_OPEN;

	field_start = csv_content;
	end_ptr = csv_content + (content_size - 1);

	row_start = field_start;
	while (field_start < end_ptr) {
		memset(field_val, 0, FIELD_VAL_SIZE_MAX);
		while (isspace(*field_start))
			field_start++;
		if (*field_start == '"') {
			ptr = field_start + 1;
			orig_field_start = field_start + 1;
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
								field_val_len = ptr - field_start - 1;
								field_start = ptr;
								field_cnt++;
								break;
							}
						}
						// "123", or "123"\n
						else if ((*(ptr + 1)) == ',' || (*(ptr + 1)) == '\n') {
							/* field_value = (field_start+1) len = ptr-field_start-2 */
							/* is_valid_field */
							dquote_cnt = 0;
							if (field_val_len  > 1) {
								field_start = field_start + 1;
							}
							field_val_len = ptr - field_start;
							if ((ptr + 2) <= end_ptr) {
								if (*(ptr + 1) == '\n')
									row_start = ptr + 2;
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
									field_val_len = ptr - orig_field_start;
									if ((ptr + 3) <= end_ptr) {
										ptr = ptr + 3;
									}
									else {
										ptr = end_ptr;
									}
									field_start = ptr;
									row_start = ptr;
								}
								else {
									ptr = ptr + 2;
								}
							}
							else {
								ptr = end_ptr;
								field_val_len = ptr - orig_field_start + 1;
								field_start = ptr;
								is_valid_row_data = false;
								field_cnt++;
								break;
							}
						}
						else {
							//TODO: invalid "123"123 go \n and output
							while (((ptr + 1) <= end_ptr) && *ptr != '\n') {
								ptr++;
							}

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
							break;
						}
					}// end else if (((ptr + 1)) <= end_ptr)
				}//end if (*ptr == '"')
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
			}//end while(dquote_cnt != 0)
			if (field_val_len < FIELD_VAL_SIZE_MAX) {
				strncpy(field_val, orig_field_start, field_val_len);
			}
			else {
				strncpy(field_val, orig_field_start, FIELD_VAL_SIZE_MAX - 1);
			}
			printf("field_val =%s\n", field_val);
		}//end field_start == '"'
		//,123,456\n
		else if (*field_start == ',') {
			row_start = field_start;
			if ((field_start + 1) <= end_ptr) {
				field_start = field_start + 1;
			}
			//add 空字串 ""
			field_cnt++;
		}
		/* field_start != '"' && field_start != ',' */
		else {
			printf("====start====\n");
			printf("field_start =%s\n", field_start);
			printf("====end======\n");
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
				if (ptr == end_ptr) {
					field_val_len = ptr - orig_field_start + 1;
					field_start = ptr;
				}
				else {
					field_val_len = ptr - orig_field_start;
					field_start = ptr + 1;
				}
				field_cnt++;
			}
			else if (*ptr == '\n') {
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

				field_cnt++;
			}
			else {
				field_val_len = ptr - orig_field_start;
				field_start = ptr;
			}
			if (field_val_len < FIELD_VAL_SIZE_MAX) {
				strncpy(field_val, orig_field_start, field_val_len);
			}
			else {
				strncpy(field_val, orig_field_start, FIELD_VAL_SIZE_MAX - 1);
			}
			printf("field_val =%s\n", field_val);
		}
		is_valid_row_data = true;
		field_cnt = 0;
		ptr = ptr + 1;
	}

	if (fp_err != NULL) {
		fclose(fp_err);
		fp_err = NULL;
	}

	return SUCCESS;
}
