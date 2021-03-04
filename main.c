#ifndef __HAS_TEST_EXECUTABLE

#include "include/cli.h"
#include "include/messages.h"
#include "include/cliclient.h"

int main(int argc, char** argv) {
  if (argc == 1) {
    warn("No arguments passed. Exiting with error code 1");
    return(1);
  }
  struct cliargs* args = cliclient();
  cliclientsetzarg(argv[0]);
  bool res = cliexecuteall(args, argc - 1, argv + 1);
  if (!res)
    warn("Syntax failture: please see readme.md");
  return(!res);
}

#endif
