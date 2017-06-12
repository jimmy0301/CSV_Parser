#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header_info.h"
#include "csv_sort.h"
#include "csv_data.h"
#include "common.h"
#include "err_code.h"


static int compare(csv_field_t *entry1, csv_field_t *entry2, int *sort_order_list,
						 size_t sort_order_cnt, int sort_order);
static int compare_integer(int a, int b);
static int compare_bool(bool a, bool b);
static int compare_double(double a, double b);
static int compare_char(char *a, char *b);


int
sort_header_parse(char *sort_header, header_t *header, size_t header_cnt,
						int *sort_order_list, size_t *sort_order_list_cnt)
{
	int cnt = 0, i, list_idx = 0;
	char *ptr = NULL;
	char *ptr2 = NULL;

	if (sort_header == NULL || header == NULL)
		return ERR_PARAM_NULL;

	if (sort_header[0] == '\0')
		return ERR_PARAM_INVAL;

	ptr = sort_header;
	while ((ptr2 = strchr(ptr, ',')) != NULL) {
		cnt++;
		ptr = ptr2 + 1;
	}

	ptr = sort_header;
	while ((ptr2 = strchr(ptr, ',')) != NULL) {
		*ptr2 = '\0';
		for (i = 0; i < header_cnt; i++) {
			if (strcmp(ptr, header[i].header_name) == 0) {
				if (list_idx < SORT_ORDER_LIST_SIZE_MAX ) {
					sort_order_list[list_idx] = header[i].header_index;
					list_idx++;
				}
				break;
			}
		}
		ptr = ptr2 + 1;
		if (list_idx >= SORT_ORDER_LIST_SIZE_MAX)
			break;
	}

	if (*ptr != '\0' && list_idx < SORT_ORDER_LIST_SIZE_MAX) {
		for (i = 0; i < header_cnt; i++) {
			if (strcmp(ptr, header[i].header_name) == 0) {
				if (list_idx < SORT_ORDER_LIST_SIZE_MAX) {
					sort_order_list[list_idx] = header[i].header_index;
					list_idx++;
				}
				break;
			}
		}
	}

	*sort_order_list_cnt = list_idx;

	return SUCCESS;
}

int sort_by_field(csv_field_t *csv_data[CSV_ROW_SIZE_MAX], size_t csv_data_size, size_t header_cnt,
						int *sort_order_list, size_t sort_order_cnt, int sort_order)
{
	int i = 0, j;
	int *stack = NULL;
	int top = -1;
	int l = 0;
	int h = csv_data_size - 1;
	int pivot;
	csv_field_t *pivot_data = NULL;
	csv_field_t *tmp_field = NULL;

	if (sort_order_list == NULL) {
		return ERR_PARAM_NULL;
	}

	if (csv_data_size <= 0 || header_cnt <= 0 || sort_order_cnt <= 0)
		return ERR_PARAM_INVAL;

	if ((h - l + 1) <= 0)
		return ERR_DATA_INVAL;

	stack = (int *)malloc(sizeof(int)*(h - l + 1));
	if (stack == NULL)
		return ERR_SYS_MEM;

	stack[++top] = l;
	stack[++top] = h;

	while (top >= 0) {
		h = stack[top--];
		l = stack[top--];

		pivot_data = csv_data[h];
		i = l -1;

		for (j = l; j <= h -1; j++) {
			if (compare(csv_data[j], pivot_data, sort_order_list, sort_order_cnt, sort_order) <= 0) {
				i++;
				tmp_field = csv_data[j];
				csv_data[j] = csv_data[i];
				csv_data[i] = tmp_field;
			}
		}
		tmp_field = csv_data[i + 1];
		csv_data[i + 1] = csv_data[h];
		csv_data[h] = tmp_field;
		pivot = i + 1;

		if (pivot - 1 > l) {
			stack[++top] = l;
			stack[++top] = pivot - 1;
		}

		if (pivot + 1 < h) {
			stack[++top] = pivot + 1;
			stack[++top] = h;
		}
	}

	if (stack != NULL) {
		free(stack);
		stack = NULL;
	}

	return SUCCESS;
}



static int
compare(csv_field_t *entry1, csv_field_t *entry2, int *sort_order_list, size_t sort_order_cnt, int sort_order)
{
	int i = 0, rc = 0;
	while (i < sort_order_cnt && rc == 0) {
		if (entry1[sort_order_list[i]].type == INTEGER) {
			rc = compare_integer(entry1[sort_order_list[i]].integer, entry2[sort_order_list[i]].integer);
		}
		else if (entry1[sort_order_list[i]].type == BOOL) {
			rc = compare_bool(entry1[sort_order_list[i]].boolean, entry2[sort_order_list[i]].boolean);
		}
		else if (entry1[sort_order_list[i]].type == DOUBLE) {
			rc = compare_double(entry1[sort_order_list[i]].double_num, entry2[sort_order_list[i]].double_num);
		}
		else if (entry1[sort_order_list[i]].type == CHAR) {
			rc = compare_char(entry1[sort_order_list[i]].char_str, entry2[sort_order_list[i]].char_str);
		}
		else if (entry1[sort_order_list[i]].type == VARCHAR) {
			rc = compare_char(entry1[sort_order_list[i]].varchar_str, entry2[sort_order_list[i]].varchar_str);
		}
		else if (entry1[sort_order_list[i]].type == DATETIME) {
			rc = compare_integer(entry1[sort_order_list[i]].datetime, entry2[sort_order_list[i]].datetime);
		}
		i++;
	}

	if (sort_order == SORT_ORDER_DESC) {
		rc = -rc;
	}

	return rc;
}

static int
compare_integer(int a, int b)
{
	if (a > b)
		return 1;
	else if (a == b)
		return 0;
	else
		return -1;
}

static int
compare_bool(bool a, bool b)
{
	if (a == b)
		return 0;
	else if (a == true && b == false)
		return 1;
	else
		return -1;
}

static int
compare_double(double a, double b)
{
	if (a == b)
		return 0;
	else if (a > b)
		return 1;
	else
		return -1;
}

static int
compare_char(char *a, char *b)
{
	return strcmp(a, b);
}
