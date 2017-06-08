#ifndef __COMMON_H
#define __COMMON_H


#define PATH_MAX 4096
#define BUF_SIZE_MAX 8192

#define TRUE	1
#define FALSE	0

typedef unsigned int bool;


extern int remove_header_space(char *str);
extern int remove_tail_space(char *str);

#endif
