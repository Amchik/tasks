#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "include/cli.h"

#define clifull(cliarg) ((struct __cliarg*)(void*)cliarg)
struct __cliarg {
  struct __cliarg_e base;
  
  int __argsleft;
  char** __argv;
};

// CLI argument
struct cliargs cliargs(char* token, void (*handler)(cliarg_t* cliarg), char extra) {
  struct cliargs object = {
    .token = token,
    .handler = handler,
    .extra = extra,
  };
  return(object);
}

static clierror_t CLIWHEREERROR = {
  .position = 0,
  .token = 0
};

// Get error
clierror_t cligeterror() {
  return(CLIWHEREERROR);
}

struct cliargs* _findarg(struct cliargs* args, const char* token) {
  struct cliargs* curr = args;
  while (true) {
    if (curr->token == 0) break; 
    if (strcmp(curr->token, token) == 0)
      return(curr);
    curr++;
  }
  return(0);
}

// Execute all arguments
bool cliexecuteall(struct cliargs* args, int argc, char** argv) {
  int n = 0;
  for (;n < argc; ++n) {
    char* token = argv[n];
    char* carg = 0;
    CLIWHEREERROR.token = token;
    CLIWHEREERROR.position = n; 
    CLIWHEREERROR.addit = -1;
    struct cliargs* arg = _findarg(args, token);
    if (!arg)
      return(false);
    if (arg->extra > 0 && argc - 1 - n < arg->extra) {
      CLIWHEREERROR.addit = argc - 1 - n;
      return(false);
    }
    if (arg->extra != 0) 
      carg = argv[++n];
    struct __cliarg targ = {
      .base = {
        .token = token,
        .argument = carg,
      },
      .__argsleft = argc - 1 - n,
      .__argv = argv + n + 1,
    };
    arg->handler(&(targ.base));
    if (arg->extra == -1) n = argc - 1 - targ.__argsleft;
    else if (arg->extra != 0) n += arg->extra - 1;
  }
  return(true);
}

void clinextarg(cliarg_t* self) {
  struct __cliarg* that = clifull(self);
  if ((that->__argsleft)-- == 0) {
    self->argument = 0;
    return;
  }
  self->argument = *(that->__argv++);
}
