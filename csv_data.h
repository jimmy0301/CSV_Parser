#ifndef __CSV_DATA_H
#define __CSV_DATA_H

#include "header_info.h"

#define CSV_FIELD_SIZE_MAX	20
#define CSV_ROW_SIZE_MAX	100000

typedef struct csv_field
{
	header_type type;
	void *data;
}csv_field_t;

extern char* csv_file_read(char *file_name, size_t *file_size);
extern int csv_content_parse(char *csv_content, size_t content_size, header_t *header, csv_field_t* csv_data[CSV_ROW_SIZE_MAX]);

#endif
