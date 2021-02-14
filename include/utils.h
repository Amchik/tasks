#ifndef _ZF_H_UTILS
#define _ZF_H_UTILS

#include <stdbool.h>
#include <sys/types.h>

#define __NARGS_SEQ(_1,_2,_3,_4,_5,_6,_7,_8,N,...) N
#define _NARGS(...) __NARGS_SEQ(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1)

char* read_file(const char* filename);

char* get_pname(pid_t pid);
pid_t getppidof(pid_t pid);

bool strends(const char* str, const char* suffix);
int strdlen(const char* string);
char* strapnd(const char* s1, const char* s2);
char* strcapnd(char* original, char new);

char* itoa(int n);

#endif
