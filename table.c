#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

#include "include/table.h"

// Create new row by color and records.
// Usage: createrow(color, record1, record2...)
struct row*   __createrow(int n, struct color color, ...) {
  struct row* row = malloc(sizeof(struct row));
  row->color = color;
  
  record_t* records = malloc((n + 1) * sizeof(record_t));
  records[n] = 0;
  va_list lst;
  va_start(lst, color);
  for(int i = 0; i < n + 1; i++) {
    records[i] = va_arg(lst, record_t);
  }
  va_end(lst);

  row->records = records; 

  return(row);
}
// Create new table by master row and rows.
// Usage: createtable(masterrow, row1, row2...)
struct table* __createtable(int n, struct row masterrow, ...) {
  struct table* table = malloc(sizeof(struct table));
  table->masterrow = masterrow;

  struct row* rows = malloc((n + 1) * sizeof(struct row));
  struct row nul = {.color = rgbi(0), .records = 0};
  rows[n] = nul;
  va_list lst;
  va_start(lst, masterrow);
  for(int i = 0; i <= n; i++) {
    rows[i] = va_arg(lst, struct row);
  }
  va_end(lst);

  table->rows = rows;
  return(table);
}
// Create new color structure by red, green and blue
struct color  rgb(unsigned char red, unsigned char green, unsigned char blue) {
  struct color clr = {.red = red, .green = green, .blue = blue};
  return(clr);
}
// Create new color structure by number, ex 0xff12ff
struct color  rgbi(unsigned long number) {
  return(*(struct color*)(void*)&number);
}

// Extends a row, where newsize is elements count
void extendrow(struct row* row, size_t newsize) {
  puts("Do not use that. [extendrow]");
  exit(255);
}

// Return length of records array.
// Array must be ends with 0, read strlen(3)
size_t rowlen(struct row* row) {
  int i = 0;
  while (row->records[i] != 0) { ++i; }
  return(i);
}
// Returns length of rows array
// Array must be ends with 0, read strlen(3)
size_t tablelen(struct table* table) {
  int i = 0;
  struct row nul = {.color = rgbi(0), .records = 0};
  while (table->rows[i].records != nul.records) { ++i; }
  return(i);
}

// --- DRAWING ---

char* _tostr(int n) {
  char* ans = calloc(12, sizeof(char));
  sprintf(ans, "%d", n);
  return(ans);
}

void _printrow
(bool printNum, int n, int numbericlen, int rowsc, const int* rowslen, const struct row* row, FILE* fp) {
  if (!printNum) {
    for(int j = 0; j < (numbericlen + 4); j++) putc(' ', fp);
  } else {
    fprintf(fp, "\e[48;2;%u;%u;%um \e[0m ", row->color.red, row->color.green, row->color.blue);
    fprintf(fp, "#%u", n);
    int nlen = strlen(_tostr(n));
    for(int j = 0; j < (numbericlen - nlen + 1); j++) putc(' ', fp);
  }
  for(int i = 0; i < rowsc; i++) {
    record_t rc = row->records[i];
    bool recordended = false;
    for (int j = 0; j < rowslen[i]; j++) {
      if (recordended) putc(' ', fp);
      else {
        char c = rc[j];
        if (c == 0) {
          putc(' ', fp);
          recordended = true;
          continue;
        }
        putc(c, fp);
      }
    }
    if (i + 1 != rowsc) putc(' ', fp);
  }
}

// Prints table to fp
// Includes colors and underline
void drawtable(struct table* table, FILE* fp) {
  int numbericlen;
  int rowsc;
  int* rowslen;
  int tbllen;
  // Getting lengths
  tbllen = tablelen(table);
  numbericlen = strlen(_tostr(tbllen));
  rowsc = rowlen(&table->masterrow);
  rowslen = malloc(sizeof(int) * rowsc);
  for(int i = 0; i < rowsc; i++) {
    int maxlen = strlen(table->masterrow.records[i]);
    struct row* curr;
    for (int j = 0; (curr = table->rows + j)->records != 0; ++j) {
      int len = strlen(curr->records[i]);
      if (len > maxlen) maxlen = len;
    }
    rowslen[i] = maxlen;
  }

  // printing master row
  fprintf(fp, "\e[1;4m");
  _printrow(false, 0, numbericlen, rowsc, rowslen, &table->masterrow, fp);
  fprintf(fp, "\e[0m\n");

  // printing another rows
  for(int i = 0; i < tbllen; i++) {
    struct row* row = table->rows + i;
    _printrow(true, i + 1, numbericlen, rowsc, rowslen, row, fp);
    putc('\n', fp);
  }

  free(rowslen);
}