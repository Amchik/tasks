#include "testoasterror/src/testoasterror.h"

#include "tests_funcs.h"

#define RESC 255
#define FUNC 3

int main(void) {
  bool res[RESC];
  void (*funcs[FUNC])(struct testoasterror*) = {
    test_simple_query,
    test_query_with_quotes,
    test_query_2_props
  };

  struct testoasterror test;
  testoasterror_init(&test, res, RESC, funcs, FUNC);
  bool x = testoasterror_run(&test);

  return(!x);
}
