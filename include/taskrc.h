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

// TaskRC line
struct rcline {
  unsigned int linen;
  unsigned int code;
  struct rcstatement* statement;
};

bool rchastype(const char* type);
bool rchasparam(const char* param, const char* type);

struct rcline rcparseln(const char* data, size_t* n);
struct rcline* rcparselns(const char* data, size_t offset);

#endif
