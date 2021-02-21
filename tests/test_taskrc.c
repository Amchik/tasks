#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "testoasterror/src/testoasterror.h"

#include "../include/taskrc.h"

void _prinrcline(char* q, struct rcline ln) {
  printf("Result of parse query:\n\tQuery:\t\t\"%s\"\n\tStatus code:\t%u\n\tChar:\t\t%d (near \"%s\")\n\tStatement:\t0x%x\n",
      q, ln.code, ln.linen, q + ln.linen, (size_t)ln.statement);
  if (ln.statement != 0) {
    struct rcstatement stm = *ln.statement;
    printf("Statement:\n\tType:\t\t%s\n\tPrimary:\t\"%s\"\n"
        "\tParams:\t\t0x%x\n\tPost:\t\t0x%x (\"%s\")\n",
        stm.type, stm.primary, (size_t)stm.params, (size_t)stm.post, stm.post);
    if (stm.params != 0) {
      puts("Params:");
      struct rcstatementlist* prm = stm.params;
      do {
        printf("\t[@0x%x] \"%s\"\t:: [@0x%x] \"%s\"\n",
            (size_t)prm->parameter, prm->parameter, (size_t)prm->value, prm->value);
        prm += 1;
      } while (prm->parameter != 0);
    }
  }
}

struct rcline test_query(struct testoasterror *test, char* q, int vn, ...) {
  size_t n = 0;
  struct rcline line = rcparseln(q, &n);
  testoasterror(test, !line.code);
  if (!line.code) {
    va_list lst;
    va_start(lst, vn);
    for (int i = 0; i < vn; i++) {
      char* val = va_arg(lst, char*);
      testoasterror(test, strcmp(
            line.statement->params[i].value,
            val
            ) == 0);
    }
    va_end(lst);
  }
  _prinrcline(q, line);
  return(line);
}

void test_query_2_props(struct testoasterror *test) {
  test_query(test, "task hello with label \"xxx\", label yyy, label zzz and priority 5 completed",
      3, "xxx", "yyy", "zzz");
}

void test_query_with_quotes(struct testoasterror* test) {
  char* q = "task \"Hello world!\" with label \"str \\\"label\\\"...\" and priority 5";
  test_query(test, q, 2, "str \"label\"...", "5");
}
