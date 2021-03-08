#ifndef _THCLI
#define _THCLI

#include <stdlib.h>
#include <stdbool.h>

// Argument object.
typedef struct __cliarg_e cliarg_t;
// Argument object.
struct __cliarg_e {
  // Argument token AKA name.
  char* token;
  // Current argument, change it by calling clinextarg()
  // If arguments ended this property will be set to NULL.
  char* argument;
};

// CLI argument
struct cliargs {
  // Argument token AKA name.
  char* token;
  // Extra arguments count. Use zero for disable.
  // Example of usage: TOKEN1 ARG1 TOKEN2... -- for arg TOKEN1 extra = 1.
  // Use -1 for unexpected count of extra arguments.
  // For go to next argument call clinextarg() by cliarg_t
  char extra;

  void (*handler)(cliarg_t* cliarg);
};

typedef struct {
  // Argument token AKA name.
  char* token;
  // Position
  unsigned int position;
} clierror_t;

// CLI argument
struct cliargs cliargs(char* token, void (*handler)(cliarg_t* cliarg), char extra);

// Get error
clierror_t cligeterror();

// Execute all arguments
bool cliexecuteall(struct cliargs* args, int argc, char** argv);

// Go to next argument
void clinextarg(cliarg_t* self);

#endif
