#ifndef _THTASKSRC
#define _THTASKSRC

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// --- ABSTRACT ---

// List of parameters in taskrc statement
struct rcstatementlist {
  const char* parameter;
  const char* value;
};

// TaskRC statement
struct rcstatement {
  const char* type;
  const char* primary;

  struct rcstatementlist* params;

  const char* post;
};

// Describes parsing error
struct rcparseerror {
  // Error code
  // Use rcprsgeterrordesc(code) for get description.
  unsigned int code;

  // # of start char
  unsigned int cstart;
  // # of error char
  unsigned int cend;
};

// Result of parsing
struct rcparseresult {
  // Result of parsing
  struct rcstatement statement;
  // Error
  struct rcparseerror error;

  // Line
  char* query;
};

// Result of parsing file
struct rcfile {
  // Statements, pointer can be zero
  // if line is empty
  struct rcparseresult** result;
  // Count of lines
  unsigned int length;
};

// TaskRC line [OBSOLETE]
struct rcline {
  unsigned int linen;
  unsigned int code;
  struct rcstatement* statement;
};

// Has type exists?
bool rchastype(const char* type);
// Has param exists in type and has type exists?
bool rchasparam(const char* param, const char* type);

// Parse one line
struct rcparseresult rcparseln(const char* data, size_t* n);
// Parse file
struct rcfile rcparselns(FILE* data, size_t offset);

// Get description for error code.
char* rcprsgeterrordesc(unsigned int code);

#endif
