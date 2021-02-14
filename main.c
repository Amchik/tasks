#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "include/messages.h"
#include "include/table.h"
#include "include/tasks.h"

int main(int argc, char** argv) {
  struct Label generic = createLabel("generic", rgbi(0xe74c3c));
  struct Task t1 = createTask(1, "Hello world!", generic, 7);
  struct Task t2 = createTask(2, "Buy a shop (?)", generic, 9);

  struct row* mrow = createrow(rgbi(0xffffff), "Description", "Label", "Priority");
  struct row* r1 = Tasktorow(t1);
  struct row* r2 = Tasktorow(t2);
  struct table* table = createtable(*mrow, *r1, *r2);
  table->clrBefore = 1;
  drawtable(table, stdout);
  free(table);
  free(r1);
  free(r2);
  free(mrow);
  return(0);
}

