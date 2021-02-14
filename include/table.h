#ifndef _THTABLE
#define _THTABLE

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

// Row record.
typedef const char* record_t;

// Color
struct color {
  unsigned char blue;
  unsigned char green;
  unsigned char red;
  /* please ignore it needs for my shitty code */
  /* BGR, yeah... */
};

// One row of table
struct row {
  // Array of records in single row.
  // Must be ends to 0
  record_t* records;

  // Row color
  struct color color;
};

struct table {
  // Header row
  struct row masterrow;
  // Array of rows.
  // Must be ends to 0
  struct row* rows;

  // Prints color before row clrBefore
  // NOTE: first row is 0, set to -1 to print in start
  int clrBefore;
};

// Create new row by color and records.
// Usage: createrow(color, record1, record2...)
#define createrow(color, ...) __createrow(_NARGS(__VA_ARGS__) - 1, color, __VA_ARGS__)
// Create new table by master row and rows.
// Usage: createtable(masterrow, row1, row2...)
#define createtable(masterrow, ...) __createtable(_NARGS(__VA_ARGS__) - 1, masterrow, __VA_ARGS__)

// Create new row by color and records.
// Usage: createrow(color, record1, record2...)
struct row*   __createrow(int n, struct color color, ...);
// Create new table by master row and rows.
// Usage: createtable(masterrow, row1, row2...)
struct table* __createtable(int n, struct row masterrow, ...);
// Create new color structure by red, green and blue
struct color  rgb(unsigned char red, unsigned char green, unsigned char blue);
// Create new color structure by number, ex 0xff12ff
struct color  rgbi(unsigned long number);

// Extends a row, where newsize is elements count
void extendrow(struct row* row, size_t newsize);

// Return length of records array.
// Array must be ends with 0, read strlen(3)
size_t rowlen(struct row* row);
// Returns length of rows array
// Array must be ends with 0, read strlen(3)
size_t tablelen(struct table* table);

// --- DRAWING ---

// Prints table to fp
// Includes colors and underline
void drawtable(struct table* table, FILE* fp);

#endif
