#ifndef __CONF_H
#define __CONF_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"
#include "err_code.h"


#define CSV_INPUT_FILE_STR 	"CSV_INPUT"
#define CSV_OUTPUT_FILE_STR	"CSV_OUTPUT"
#define CSV_ERR_FILE_STR		"CSV_ERROR"
#define CSV_RES_FILE_STR		"CSV_RESULT"
#define CSV_FORMULA_STR			"CSV_FORMULA"
#define CSV_HEADERS_STR			"HEADERS"
#define CSV_SORT_HEADERS_STR	"SORT_HEADERS"
#define CSV_SORT_ORDER_STR		"SORT_ORDER"
#define CSV_SORT_ASC_STR		"ASC"
#define CSV_SORT_DESC_STR		"DESC"

#define FORMULA_CNT_MAX	3

#define SORT_ORDER_ASC 	0
#define SORT_ORDER_DESC	1


typedef struct conf_content
{
	char input_file[PATH_MAX];
	char output_file[PATH_MAX];
	char error_file[PATH_MAX];
	char res_file[PATH_MAX];
	char *formula[FORMULA_CNT_MAX];
	char *headers;
	char *sort_headers;
	int sort_order;
}conf_t;

extern int conf_init(conf_t *conf);
extern int conf_free(conf_t *conf);
extern int conf_parse(char *file_name, conf_t *conf);
extern void conf_dump(conf_t *conf);

#endif
