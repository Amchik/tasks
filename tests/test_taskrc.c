#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "testoasterror/src/testoasterror.h"

#include "../include/taskrc.h"

void _prinrcline(char* q, struct rcparseresult res) {
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
    struct rcstatement stm = res.statement;
    printf(
      "%s %s",
      stm.type,
      stm.primary
    );
    if (stm.params) {
      printf("\n  with ");
      for (int i = 0; stm.params[i].parameter != 0; i++) {
        printf("%s \"%s\", ", stm.params[i].parameter,
            stm.params[i].value);
      }
      printf("\b\b  ");
    }
    if (stm.post) {
      printf("\n  post \"%s\"", stm.post);
    }
    printf("\n");
  }

}

struct rcparseresult test_query(struct testoasterror *test, char* q, int vn, ...) {
  size_t n = 0;
  struct rcparseresult res = rcparseln(q, &n);
  testoasterror(test, !res.error.code);
  if (!res.error.code) {
    va_list lst;
    va_start(lst, vn);
    for (int i = 0; i < vn; i++) {
      char* val = va_arg(lst, char*);
      testoasterror(test, strcmp(
            res.statement.params[i].value,
            val
            ) == 0);
    }
    char* post = va_arg(lst, char*);
    if (post) {
      testoasterror(test, strcmp(res.statement.post, post) == 0);
    } else {
      testoasterror(test, !res.statement.post);
    }
    va_end(lst);
  }
  _prinrcline(q, res);
  return(res);
}

void test_query_2_props(struct testoasterror *test) {
  test_query(test, "task hello with label \"xxx\", label yyy, label zzz and priority 5 completed",
      3, "xxx", "yyy", "zzz", "completed");
}

void test_query_with_quotes(struct testoasterror* test) {
  char* q = "task \"Hello world!\" with label \"str \\\"label\\\"...\" and priority 5";
  test_query(test, q, 2, "str \"label\"...", "5", 0);
}

void test_this_query_because(struct testoasterror* test) {
  test_query(test, "label food with color #e74c3c", 1, "#e74c3c", 0);
}

