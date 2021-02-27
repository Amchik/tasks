#include "include/table.h"
#ifndef __HAS_TEST_EXECUTABLE

#include <stdio.h>

#include "include/tasks.h"
#include "include/cli.h"
#include "include/taskrc.h"
#include "include/messages.h"

#define UNKNOWN "\e[0;38m<unknown source>\e[0m"

static char* zarg = "tasks";

void _calcerrors(struct TaskRcContents rc, char* f) {
  if (!f) f = "\e[3mfile";
  int count = 0;
  for (int i = 0; i < rc.length; i++) {
    if (rc.results[i] && rc.results[i]->error.code >= 100)
      count++;
  }
  if (count) {
    warn("While parsing taskrc found %d errors. Run \e[1m%s diagnostic %s\e[0m"
        " for more information.",
        count, zarg, f);
  }
}

void ondiagnostic(cliarg_t* arg) {
  FILE* fp = fopen(arg->argument, "r");
  if (!fp) {
    error("Failed to open file %s", arg->argument);
    return;
  }
  struct TaskRcContents rc = parseTaskRc(fp);
  fclose(fp);
  int count = 0;
  for (int i = 0; i < rc.length; i++) {
    struct rcparseresult* res = rc.results[i];
    if (res && res->error.code >= 100) {
      count++;
      char* q = UNKNOWN;
      if (res->query) q = res->query;
      printf("\e[1;31merror\e[0;1m:%u:%u-%u:\e[0m [%u] %s\n %3d | %s\n     | \e[1;31m", 
          i + 1, res->error.cstart, res->error.cend, res->error.code, rcprsgeterrordesc(res->error.code), i + 1,
          q);
      if (res->error.cstart <= res->error.cend) {
        for(int i = 0; i < res->error.cstart; i++) putc(' ', stdout);
        putc('^', stdout);
        for(int i = 0; i < (int)res->error.cend - (int)res->error.cstart - 2; i++) putc('~', stdout);
        printf("\e[0m\n");
      }
      continue;
    }
  }
  if (count)
    echo("Found %d errors.", count);
}

void onparsetaskrc(cliarg_t* arg) {
  FILE* fp = fopen(arg->argument, "r");
  if (!fp) {
    puts("Failed to open file.");
    return;
  }
  struct TaskRcContents rc = parseTaskRc(fp);
  fclose(fp);
  for (unsigned int i = 0; i < rc.length; i++) {
    printf("Line [%u] contains...", i + 1);
    struct rcparseresult* res = rc.results[i];
    if (!res) { puts("nothing"); continue; }
    if (res->error.code >= 100) {
      char* q = UNKNOWN;
      if (res->query) q = res->query;
      printf("error %u @%u:%u\n | %s\n | \e[1;31m", 
          res->error.code, res->error.cstart, res->error.cend,
          q);
      if (res->error.cstart <= res->error.cend) {
        for(int i = 0; i < res->error.cstart; i++) putc(' ', stdout);
        putc('^', stdout);
        for(int i = 0; i < (int)res->error.cend - (int)res->error.cstart - 2; i++) putc('~', stdout);
        printf("\e[0m\n");
      }
      printf(" | \e[1;31m> cstart=%d;cend=%d \e[0m\n", res->error.cstart, res->error.cend);
      continue;
    }
    switch (*res->statement.type) {
      case 'l':
        {
          struct Label* lbl = rc.labels[i];
          printf("Label %%%s with color rgb(%u, %u, %u)\n",
              lbl->name, lbl->color.red, lbl->color.green, lbl->color.blue);
        }
        break;

      case 't':
        {
          struct Task tsk = *rc.tasks[i];
          printf("task %s with priority %d and label %%%s ",
              tsk.description, tsk.priority, tsk.label.name);
          if (!tsk.completed) printf("not ");
          puts("completed");
        }
        break;

      default:
        puts("what?");
        break;
    }
  }
}

void ontable(cliarg_t* arg) {
  FILE* fp = fopen(arg->argument, "r");
  if (!fp) {
    error("Failed to open file %s", arg->argument);
    return;
  }
  struct TaskRcContents rc = parseTaskRc(fp);
  fclose(fp);
  _calcerrors(rc, arg->argument);
  struct row* mrow = createrow(rgbi(0), "Description", "Label", "Priority", "\b\b\e[0m");
  struct table tbl = {
    .masterrow = *mrow,
    .clrBefore = 1
  };
  tbl.rows = malloc(sizeof(struct row) * (rc.length + 1));
  int rnext = 0;
  for (int i = 0; i < rc.length; i++) {
    struct Task* tsk = rc.tasks[i];
    if (tsk) {
      tbl.rows[rnext] = *Tasktorow(*tsk);
      rnext++;
    }
  }
  struct row nul = {.color = rgbi(0), .records = 0};
  tbl.rows[rnext] = nul;
  drawtable(&tbl, stdout);
}

int main(int argc, char** argv) {
  zarg = argv[0];
  if (argc == 1) {
    warn("No arguments passed. Exiting with error code 1");
    return(1);
  }
  struct cliargs args[] = {
    cliargs("taskrc", onparsetaskrc, 1),
    cliargs("table", ontable, 1),
    cliargs("diagnostic", ondiagnostic, 1),
    {0,0,0}
  };
  bool res = cliexecuteall(args, argc - 1, argv + 1);
  if (!res)
    warn("Syntax failture: please see readme.md");
  return(!res);
}

#endif
