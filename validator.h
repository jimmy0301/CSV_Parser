#ifndef __VALIDATOR_H
#define __VALIDATOR_H


#include "header_info.h"


#define VALID_BOOL_TRUE_STR		"TRUE"
#define VALID_BOOL_FALSE_STR		"FALSE"

#define VALID_CHAR_SIZE_MAX		128
#define VALID_VARCHAR_SIZE_MAX	1024


extern bool is_valid_field(char *str, header_type type, size_t size);


#endif
