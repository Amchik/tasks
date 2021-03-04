#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "include/messages.h"
#include "include/utils.h"

void _do_print(const char* format, va_list args) {
  vprintf(format, args);
  printf("\n");
}

void echo(const char* format, ...) {
  char* y = strapnd(" \e[0;34m[i]\e[0m ", format);
  va_list args;
  va_start(args, format);
  _do_print(y, args);
  va_end(args);
  free(y);
}
void warn(const char* format, ...) {
  char* y = strapnd(" \e[0;33m[w]\e[0m ", format);
  va_list args;
  va_start(args, format);
  _do_print(y, args);
  va_end(args);
  free(y);
}
void error(const char* format, ...) {
  char* y = strapnd(" \e[0;31m[e]\e[0m ", format);
  va_list args;
  va_start(args, format);
  _do_print(y, args);
  va_end(args);
  free(y);
}

