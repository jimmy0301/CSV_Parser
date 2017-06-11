#ifndef __CSV_DATA_H
#define __CSV_DATA_H

#include "header_info.h"
#include "common.h"

#define CSV_FIELD_SIZE_MAX	20
#define CSV_ROW_SIZE_MAX	10000

#define CSV_FIELD_CHAR_SIZE_MAX 		128
#define CSV_FIELD_VARCHAR_SIZE_MAX	1024
#define CSV_FIELD_DATETIME_SIZE_MAX	20


typedef struct csv_field
{
	header_type type;
	int integer;
	int datetime;
	bool boolean;
	double double_num;
	char char_str[CSV_FIELD_CHAR_SIZE_MAX];
	char varchar_str[CSV_FIELD_VARCHAR_SIZE_MAX];
	char output_str[CSV_FIELD_VARCHAR_SIZE_MAX + 3];
}csv_field_t;


extern int csv_data_init(csv_field_t *csv_data[CSV_ROW_SIZE_MAX]);
extern char* csv_file_read(char *file_name, size_t *file_size);
extern int csv_content_parse(char *csv_content, size_t content_size, header_t *header, size_t header_cnt,
									  char *err_file_name, csv_field_t *csv_data[CSV_ROW_SIZE_MAX],
									  size_t *csv_data_size);

#endif
