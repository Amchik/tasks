#include <stdio.h>
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

void test_simple_query(struct testoasterror* test) {
  char* q = "task hello with label generic and priority 6";
  size_t n = 0;
  struct rcline line = rcparseln(q, &n);
  testoasterror(test, !line.code);
  if (!line.code) {
    testoasterror(test, strcmp(line.statement->primary, "hello") == 0);
    testoasterror(test, strcmp(line.statement->params[0].parameter, "label") == 0);
    testoasterror(test, strcmp(line.statement->params[1].value, "6") == 0);
  }
  _prinrcline(q, line);
}

void test_query_with_quotes(struct testoasterror* test) {
  char* q = "task \"Hello world!\" with label \"Hmm, its looks like \\\"label\\\"...\" and priority 5";
  size_t n = 0;
  struct rcline line = rcparseln(q, &n);
  testoasterror(test, !line.code);
  if (!line.code) {
    testoasterror(test, strcmp(line.statement->primary, "Hello world!") == 0);
    testoasterror(test, strcmp(line.statement->params[0].value, "Hmm, its looks like \"label\"...") == 0);
    testoasterror(test, strcmp(line.statement->params[1].value, "5") == 0);
  }
  _prinrcline(q, line);
}

