#include <stdio.h>

#include "err_code.h"
#include "header_info.h"
#include "conf.h"
#include "csv_data.h"


static void
usage(const char *program)
{
	printf("Usage: %s $config_file_path\n", program);
	return;
}

int main(int argc, char *argv[])
{
	int rc = SUCCESS, i, j;
	size_t header_cnt = 0;
	size_t file_size = 0;
	char *file_content = NULL;
	conf_t conf;
	header_t header[HEADER_INFO_CNT_MAX];
	csv_field_t **csv_data;

	if (argc < 2) {
		usage(argv[0]);
		return SUCCESS;
	}

	if ((rc = conf_init(&conf)) != SUCCESS) {
		printf("Failed to initialize the conf structure. (%d)\n", rc);
		return ERR_DATA_INVAL;
	}

	if ((rc = conf_parse(argv[argc-1], &conf)) != SUCCESS) {
		printf("Failed to parse config file. (%d)\n", rc);
		return ERR_DATA_INVAL;
	}
	conf_dump(&conf);

	printf("=======\n");

	if ((rc = header_init(header, HEADER_INFO_CNT_MAX)) != SUCCESS) {
		printf("Failed to initialize header info. (%d)\n", rc);
		conf_free(&conf);
		return ERR_DATA_INVAL;
	}

	if ((rc = header_parse(conf.headers, header, &header_cnt)) != SUCCESS) {
		printf("Failed to parse header info. (%d)\n", rc);
		conf_free(&conf);
		return ERR_DATA_INVAL;
	}

	header_dump(header, header_cnt);

	printf("=======\n");

	if ((file_content = csv_file_read(conf.input_file, &file_size)) == NULL) {
		conf_free(&conf);
		printf("Failed to get csv file content\n");
		return ERR_DATA_INVAL;
	}

	printf("file_content = %s\n", file_content);

	csv_data = (csv_field_t**)malloc(sizeof(csv_field_t*)*CSV_ROW_SIZE_MAX);
	if (csv_data == NULL) {
		conf_free(&conf);
		free(file_content);
		file_content = NULL;
		return ERR_SYS_MEM;
	}

	for (i = 0; i < CSV_ROW_SIZE_MAX; i++) {
		csv_data[i] = (csv_field_t*)malloc(sizeof(csv_field_t)*CSV_FIELD_SIZE_MAX);
		if (csv_data[i] == NULL) {
			break;
		}
	}

	if (i < CSV_ROW_SIZE_MAX) {
		for (j = 0; j < i; j++) {
			if (csv_data[i] != NULL) {
				free(csv_data[i]);
				csv_data[i] = NULL;
			}
		}
		free(csv_data);
		csv_data = NULL;
		printf("Failed to malloc csv_data\n");
		return ERR_SYS_MEM;
	}

	if (csv_content_parse(file_content, file_size, header, header_cnt, csv_data)) {
		conf_free(&conf);
		free(file_content);
		file_content = NULL;
		return ERR_DATA_INVAL;
	}

	if (file_content != NULL) {
		free(file_content);
		file_content = NULL;
	}

	for (i = 0; i < CSV_ROW_SIZE_MAX; i++) {
		if (csv_data[i] != NULL) {
			free(csv_data[i]);
			csv_data[i] = NULL;
		}
	}

	if (csv_data != NULL) {
		free(csv_data);
		csv_data = NULL;
	}
	conf_free(&conf);
	return SUCCESS;
}
