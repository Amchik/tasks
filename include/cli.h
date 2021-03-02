#ifndef _THCLI
#define _THCLI

#include <stdlib.h>
#include <stdbool.h>

// Argument object.
typedef struct __cliarg_e cliarg_t;
struct __cliarg_e {
  char* token;
  char* argument;
};

// CLI argument
struct cliargs {
  // Argument token AKA name.
  char* token;
  // Extra arguments count. Use zero for disable.
  // Example of usage: TOKEN1 ARG1 TOKEN2... -- for arg TOKEN1 extra = 1.
  // Use -1 for unexpected count of extra arguments, changing the pointer will be affect to real pointer.
  char extra;

  void (*handler)(cliarg_t* cliarg);
};

// CLI argument
struct cliargs cliargs(char* token, void (*handler)(cliarg_t* cliarg), char extra);

// Execute all arguments
bool cliexecuteall(struct cliargs* args, int argc, char** argv);

void clinextarg(cliarg_t* self);

#endif
