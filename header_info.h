#ifndef __HEADER_INFO_H
#define __HEADER_INFO_H


#define HEADER_TYPE_INTEGER_STR	"INTEGER"
#define HEADER_TYPE_BOOL_STR		"BOOL"
#define HEADER_TYPE_DOUBLE_STR	"DOUBLE"
#define HEADER_TYPE_CHAR_STR		"CHAR"
#define HEADER_TYPE_VARCHAR_STR	"VARCHAR"
#define HEADER_TYPE_DATETIME_STR	"DATETIME"

#define HEADER_NAME_SIZE_MAX	32
#define HEADER_INFO_CNT_MAX	20

typedef enum
{
	INTEGER,
	BOOL,
	DOUBLE,
	CHAR,
	VARCHAR,
	DATETIME,
	NO_TYPE
}header_type;

typedef struct header_info
{
	size_t header_index;
	char header_name[HEADER_NAME_SIZE_MAX];
	header_type type;
	size_t size;
}header_t;

extern int header_init(header_t *header, size_t header_cnt);
extern int header_parse(char *header_str, header_t *header, size_t *header_cnt);
extern void header_dump(header_t *header, size_t header_cnt);

#endif
