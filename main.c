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
  struct rcline ln = rcparseln(q, &n);
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
  return(ln.code);
}

