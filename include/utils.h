#ifndef _ZF_H_UTILS
#define _ZF_H_UTILS

// By the way, this file from zfetch

#include <stdbool.h>

#define __NARGS_SEQ(_1,_2,_3,_4,_5,_6,_7,_8,N,...) N
#define _NARGS(...) __NARGS_SEQ(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1)

#ifdef _THUTILS_INCLUDE_USELESS_FUNCTIONS
// Reads all file contents.
char* read_file(const char* filename);

// Getting name of pid
char* get_pname(unsigned int pid);
// Getting parent pid of another pid
unsigned int getppidof(unsigned int pid);

// Has str end with suffix?
bool strends(const char* str, const char* suffix);
// Display string length
int strdlen(const char* string);
#endif
// Returns new string, that contains s1 and s2.
char* strapnd(const char* s1, const char* s2);
// Returns new string with new char at end. original will be free.
char* strcapnd(const char* original, char end);

// Converts integer to string
char* itoa(int n);

#endif
