#include "include/cli.h"
#include "include/messages.h"
#include "include/cliclient.h"

int main(int argc, char** argv) {
  if (argc == 1) {
    argc = 2;
    char* _argv[] = {
      argv[0],
      "table",
    };
    argv = _argv;
  }
  struct cliargs* args = cliclient(argv[0]);
  bool res = cliexecuteall(args, argc - 1, argv + 1);
  if (!res)
    error("Unknown argument \e[1m%s\e[0m #%u. Try \e[1m%s help\e[0m", 
        cligeterror().token, cligeterror().position, argv[0]);
  return(!res);
}
