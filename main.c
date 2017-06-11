#include <stdio.h>

#include "err_code.h"
#include "header_info.h"
#include "conf.h"
#include "csv_data.h"
#include "csv_sort.h"


static void
usage(const char *program)
{
	printf("Usage: %s $config_file_path\n", program);
	return;
}

int main(int argc, char *argv[])
{
	int rc = SUCCESS, i;
	size_t header_cnt = 0;
	size_t file_size = 0;
	size_t csv_data_size = 0;
	size_t sort_order_cnt = 0;
	char *file_content = NULL;
	int sort_order_list[CSV_FIELD_SIZE_MAX];
	conf_t conf;
	header_t header[HEADER_INFO_CNT_MAX];
	csv_field_t **csv_data;

	if (argc < 2) {
		usage(argv[0]);
		return SUCCESS;
	}

	/* Parse config file */
	if ((rc = conf_init(&conf)) != SUCCESS) {
		printf("Failed to initialize the conf structure. (%d)\n", rc);
		return rc;
	}

	if ((rc = conf_parse(argv[argc-1], &conf)) != SUCCESS) {
		printf("Failed to parse config file. (%d)\n", rc);
		return rc;
	}
	//conf_dump(&conf);

	//printf("=======\n");

	/* Parse header info */
	if ((rc = header_init(header, HEADER_INFO_CNT_MAX)) != SUCCESS) {
		printf("Failed to initialize header info. (%d)\n", rc);
		conf_free(&conf);
		return rc;
	}

	if ((rc = header_parse(conf.headers, header, &header_cnt)) != SUCCESS) {
		printf("Failed to parse header info. (%d)\n", rc);
		conf_free(&conf);
		return rc;
	}

	//header_dump(header, header_cnt);

	//printf("=======\n");

	if ((file_content = csv_file_read(conf.input_file, &file_size)) == NULL) {
		printf("Failed to get csv file content\n");
		rc = ERR_DATA_INVAL;
		conf_free(&conf);
		return rc;
	}

	if (file_size == 0) {
		printf("The csv file is empty.\n");
		return SUCCESS;
	}
	//printf("file_content = %s\n", file_content);

	csv_data = (csv_field_t **)malloc(sizeof(csv_field_t*)*CSV_ROW_SIZE_MAX);
	if (csv_data == NULL) {
		rc = ERR_SYS_MEM;
		printf("Failed to malloc csv data (%d).\n", rc);
		free(file_content);
		file_content = NULL;
		conf_free(&conf);
		return rc;
	}

	for (i = 0; i < CSV_ROW_SIZE_MAX; i++) {
		csv_data[i] = (csv_field_t *)malloc(sizeof(csv_field_t)*CSV_FIELD_SIZE_MAX);
		if (csv_data[i] == NULL) {
			break;
		}
	}

	if (i < CSV_ROW_SIZE_MAX) {
		rc = ERR_SYS_MEM;
		printf("Failed to malloc csv field data (%d).\n", rc);
		goto end;
	}

	/*if ((rc = csv_data_init(csv_data)) != SUCCESS) {
		printf("Failed to init csv_data (%d).\n", rc);
		goto end;
	}*/

	/* parse csv file */
	if ((rc = csv_content_parse(file_content, file_size, header,
								 header_cnt, conf.error_file, csv_data, &csv_data_size)) != SUCCESS) {
		printf("Failed to parse csv file (%d).\n", rc);
		rc = ERR_DATA_INVAL;
		goto end;
	}

	if ((rc = sort_header_parse(conf.sort_headers, header, header_cnt,
										sort_order_list, &sort_order_cnt))) {
		printf("Failed to parse sort header (%d).\n", rc);
		rc = ERR_DATA_INVAL;
		goto end;
	}

	for (i = 0; i < sort_order_cnt; i++) 
		printf("sort_order = %d\n", sort_order_list[i]);

	if (sort_by_field(csv_data, csv_data_size, header_cnt, sort_order_list,
							sort_order_cnt, conf.sort_order) != SUCCESS) {
		printf("Failed to sort csv_data\n");
		goto end;
	}

	csv_data_print(csv_data, csv_data_size, header_cnt);

end:
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

	return rc;
}
