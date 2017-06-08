#include "conf.h"


int
conf_init(conf_t *conf)
{
	int i = 0;

	if (conf == NULL)
		return ERR_PARAM_NULL;

	memset(conf->input_file, 0, PATH_MAX);
	memset(conf->output_file, 0, PATH_MAX);
	memset(conf->error_file, 0, PATH_MAX);
	memset(conf->res_file, 0, PATH_MAX);

	for (i = 0; i < FORMULA_CNT_MAX; i++)
		conf->formula[i] = NULL;

	conf->headers = NULL;
	conf->sort_headers = NULL;
	conf->sort_order = SORT_ORDER_ASC;

	return SUCCESS;
}

int
conf_free(conf_t *conf)
{
	int i;

	if (conf == NULL)
		return ERR_PARAM_NULL;

	for (i = 0; i < FORMULA_CNT_MAX; i++) {
		if (conf->formula[i] != NULL) {
			free(conf->formula[i]);
			conf->formula[i] = NULL;
		}
	}

	if (conf->headers != NULL) {
		free(conf->headers);
		conf->headers = NULL;
	}

	if (conf->sort_headers != NULL) {
		free(conf->headers);
		conf->headers = NULL;
	}

	return SUCCESS;
}

int
conf_parse(char *file_name, conf_t *conf)
{
	FILE *fp = NULL;
	int formula_cnt = 0;
	char buf[BUF_SIZE_MAX];
	char *ptr, *ptr2, *ptr3;

	if (file_name == NULL || conf == NULL)
		return ERR_PARAM_NULL;

	if (file_name[0] == '\0')
		return ERR_PARAM_INVAL;

	fp = fopen(file_name, "r");
	if (fp == NULL) {
		return ERR_IO_OPEN;
	}

	memset(buf, 0, BUF_SIZE_MAX);
	while (fgets(buf, BUF_SIZE_MAX, fp) != NULL) {
		ptr = &buf[strlen(buf) - 1];
		while (*ptr == '\n' || *ptr == '\r') {
			*ptr = '\0';
			ptr = ptr - 1;
		}

		remove_tail_space(ptr);

		ptr = buf;
		while (isspace(*ptr))
			ptr = ptr + 1;

		if ((ptr2 = strchr(ptr, '=')) != NULL) {
			*ptr2 = '\0';
			ptr3 = ptr2;
			ptr3 = ptr3 - 1;
			remove_tail_space(ptr3);
			ptr2 = ptr2 + 1;
			if (strcmp(ptr, CSV_INPUT_FILE_STR) == 0)
				snprintf(conf->input_file, PATH_MAX, "%s", ptr2);
			else if (strcmp(ptr, CSV_OUTPUT_FILE_STR) == 0)
				snprintf(conf->output_file, PATH_MAX, "%s", ptr2);
			else if (strcmp(ptr, CSV_ERR_FILE_STR) == 0)
				snprintf(conf->error_file, PATH_MAX, "%s", ptr2);
			else if (strcmp(ptr, CSV_RES_FILE_STR) == 0)
				snprintf(conf->res_file, PATH_MAX, "%s", ptr2);
			else if (strcmp(ptr, CSV_FORMULA_STR) == 0) {
				if (formula_cnt < FORMULA_CNT_MAX) {
					conf->formula[formula_cnt] = strdup(ptr2);
					formula_cnt++;
				}
			}
			else if (strcmp(ptr, CSV_HEADERS_STR) == 0)
				conf->headers = strdup(ptr2);
			else if (strcmp(ptr, CSV_SORT_HEADERS_STR) == 0)
				conf->sort_headers = strdup(ptr2);
			else if (strcmp(ptr, CSV_SORT_ORDER_STR) == 0) {
				if (strcmp(ptr2, CSV_SORT_ASC_STR) == 0) {
					conf->sort_order = SORT_ORDER_ASC;
				}
				else {
					conf->sort_order = SORT_ORDER_DESC;
				}
			}
			else {
				continue;
			}
		}
		else {
			continue;
		}
	}

	if (fp != NULL) {
		fclose(fp);
		fp = NULL;
	}
	return SUCCESS;
}

void
conf_dump(conf_t *conf)
{
	int i;
	printf("input_file = %s\n", conf->input_file);
	printf("output_file = %s\n", conf->output_file);
	printf("error_file = %s\n", conf->error_file);
	printf("res_file = %s\n", conf->res_file);

	for (i = 0; i < FORMULA_CNT_MAX; i++) {
		if (conf->formula[i] != NULL) {
			printf("formula %d = %s\n", i + 1, conf->formula[i]);
		}
	}

	printf("headers = %s\n", conf->headers);
	printf("sort_headers = %s\n", conf->sort_headers);
	printf("sort_order = %d\n", conf->sort_order);

	return;
}
