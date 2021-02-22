#ifndef __HAS_TEST_EXECUTABLE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "include/messages.h"
#include "include/table.h"
#include "include/taskrc.h"
#include "include/tasks.h"

int main(int argc, char** argv) {
  char* q = calloc(1, 1);
  for (int i = 1; i < argc; i++) {
    char* tmp = strapnd(q, argv[i]);
    char* tmp2 = strcapnd(tmp, ' ');
    free(q);
    q = tmp2;
  }
  q[strlen(q) - 1] = 0;
  size_t n = 0;
  struct rcparseresult res = rcparseln(q, &n);
  if (res.error.code != 0) {
    printf("Error [%u]:\n | %s\n | \e[1;31m", res.error.code, q);
    if (res.error.cstart > res.error.cend) {
      printf("> cstart=%d;cend=%d \e[0m\n", res.error.cstart, res.error.cend);
    } else {
      for(int i = 0; i < res.error.cstart; i++) putc(' ', stdout);
      putc('^', stdout);
      for(int i = 0; i < (int)res.error.cend - (int)res.error.cstart - 2; i++) putc('~', stdout);
      printf("\e[0m\n");
    }
  } else {
    puts("umm, yeah... parsed... (in gdb main.c:31, res have result of parse)");
    struct rcstatement stm = res.statement;
    printf(
      "%s %s",
      stm.type,
      stm.primary
    );
    if (stm.params) {
      printf("\n\twith ");
      for (int i = 0; stm.params[i].parameter != 0; i++) {
        printf("%s \"%s\", ", stm.params[i].parameter,
            stm.params[i].value);
      }
      printf("\b\b  ");
    }
    if (stm.post) {
      printf("\n\t%s", stm.post);
    }
    printf("\n");
  }
  return(res.error.code);
}

#endif
