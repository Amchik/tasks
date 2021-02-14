#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "include/messages.h"
#include "include/table.h"

int main(int argc, char** argv) {
  struct row* mrow = createrow(rgbi(0xffffff), "First word", "Other");
  struct row* row1 = createrow(rgbi(0x1f009c), "Hello", "world!");
  struct row* row2 = createrow(rgbi(0x2e4c3c), "Good", "to see you!");
  struct table* table = createtable(*mrow, *row1, *row2);
  echo("Row #1 length: %d", rowlen(row1));
  echo("Table length: %d", tablelen(table));
  echo("Oh no, im printing a table...");
  drawtable(table, stdout);
  free(table);
  free(row1);
  free(row2);
  free(mrow);
  return(0);
}

