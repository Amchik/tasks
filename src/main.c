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
  if (!res) {
    if (cligeterror().addit == -1)
      error("Unknown argument \e[1m%s\e[0m #%u. Try \e[1m%s help\e[0m", 
          cligeterror().token, cligeterror().position + 1, argv[0]);
    else
      error("Unknown argument \e[1m%s\e[0m+%d #%u. Try \e[1m%s help\e[0m", 
          cligeterror().token, cligeterror().addit, cligeterror().position + 1, argv[0]);
  }
  return(!res);
}
