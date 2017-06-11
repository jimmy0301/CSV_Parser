
#include "csv_data.h"



extern int sort_header_parse(char *sort_header, header_t *header, size_t header_cnt,
									  int *sort_order_list, size_t *sort_order_list_cnt);
extern int sort_by_field(csv_field_t *csv_data[CSV_ROW_SIZE_MAX], size_t csv_data_size, size_t header_cnt,
								 int *sort_order_list, size_t sort_order_cnt, int sort_order);