#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "csv_data.h"
#include "err_code.h"
#include "common.h"


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

	return file_content;
}

int
csv_content_parse(char *csv_content, size_t content_size, header_t *header, csv_field_t *csv_data[CSV_ROW_SIZE_MAX])
{
	char *start_ptr = NULL, *end_ptr = NULL, *field_start = NULL, *field_end = NULL, *ptr = NULL;
	bool has_field_dquote = FALSE;
	bool has_content_dquote = FALSE;
	bool is_valid = TRUE;

	if (csv_content == NULL || header == NULL || csv_data == NULL)
		return ERR_PARAM_NULL;
	if (csv_content[0] == '\0')
		return ERR_PARAM_INVAL;

	start_ptr = csv_content;
	end_ptr = csv_content + content_size;

	while (isspace(*start_ptr))
		start_ptr++;

	field_start = start_ptr;
	ptr = field_start
	while ((field_end = strchr(field_start, ',')) != NULL) {
		if (*ptr == '"' && has_field_dquote == FALSE) {
			has_field_dquote = TRUE;
		}

		ptr = ptr + 1;
	}

	return SUCCESS;
}
