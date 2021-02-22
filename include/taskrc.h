#ifndef _THTASKSRC
#define _THTASKSRC

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
  // Error code (see todo.com for more info)
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
};

// Result of parsing file
struct rcfile {
  // Statements, pointer can be zero
  // if line is empty
  struct rcparseresult** result;
};

// TaskRC line [OBSOLETE]
struct rcline {
  unsigned int linen;
  unsigned int code;
  struct rcstatement* statement;
};

bool rchastype(const char* type);
bool rchasparam(const char* param, const char* type);

struct rcparseresult rcparseln(const char* data, size_t* n);
struct rcfile rcparselns(const char* data, size_t offset);

#endif
