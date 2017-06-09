#ifndef __COMMON_H
#define __COMMON_H


#define PATH_MAX 4096
#define BUF_SIZE_MAX 8192

typedef unsigned char u_char;

#ifndef bool
   #define bool   u_char
   #define true   (1 == 1)
   #define false  (!true)
#endif


extern int remove_header_space(char *str);
extern int remove_tail_space(char *str);

#endif
