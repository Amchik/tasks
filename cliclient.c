#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "include/table.h"
#include "include/taskrc.h"
#include "include/tasks.h"
#include "include/tutil.h"
#include "include/messages.h"
#include "include/utils.h"
#include "include/cli.h"
#include "include/cliclient.h"

#define UNKNOWN "\e[0;37m<unknown source>\e[0m"

static unsigned int DOWARN = 0;

static char* CURRENT_FILE;
static char* zarg;

void dowarn() {
  if ((DOWARN && 1) == 1)
    echo("Using file \e[1mTasksfile\e[0m. Run tasks with \e[1muse %%\e[0m for select \e[1m~/.taskrc\e[0m");
  DOWARN = 0;
}

void cliclientsetzarg(char* value) {
  zarg = value;
}

void _calcerrors(struct TaskRcContents rc) {
  unsigned int count = 0;
  for (unsigned int i = 0; i < rc.length; i++) {
    if (rc.results[i] && rc.results[i]->error.code >= 100)
      count++;
  }
  if (count) {
    warn("While parsing taskrc found %u errors. Run \e[1m%s [use %s] diagnostic\e[0m"
        " for more information.",
        count, zarg, CURRENT_FILE);
  }
}

void ondiagnostic(cliarg_t* arg) {
  dowarn();
  FILE* fp = fopen(CURRENT_FILE, "r");
  if (!fp) {
    error("Failed to open file %s", CURRENT_FILE);
    exit(1);
    return;
  }
  struct TaskRcContents rc = parseTaskRc(fp);
  fclose(fp);
  int count = 0;
  for (unsigned int i = 0; i < rc.length; i++) {
    struct rcparseresult* res = rc.results[i];
    if (res && res->error.code >= 100) {
      count++;
      char* q = UNKNOWN;
      if (res->query) q = res->query;
      printf("\e[1;31merror\e[0;1m:%u:%u-%u:\e[0m [%u] %s\n %3d | %s\n     | \e[1;31m", 
          i + 1, res->error.cstart, res->error.cend, res->error.code, rcprsgeterrordesc(res->error.code), i + 1,
          q);
      if (res->error.cstart <= res->error.cend) {
        for(int i = 0; i < (int)res->error.cstart; i++) putc(' ', stdout);
        putc('^', stdout);
        for(int i = 0; i < (int)res->error.cend - (int)res->error.cstart - 2; i++) putc('~', stdout);
        printf("\e[0m\n");
      }
      continue;
    }
  }
  if (count)
    echo("Found %d errors.", count);
  else
    echo("No errors found.");
}

void ontable(cliarg_t* arg) {
  dowarn();
  FILE* fp = fopen(CURRENT_FILE, "r");
  if (!fp) {
    error("Failed to open file %s", CURRENT_FILE);
    exit(1);
    return;
  }
  struct TaskRcContents rc = parseTaskRc(fp);
  fclose(fp);
  _calcerrors(rc);
  struct row* mrow = createrow(rgbi(0), "Description", "Label", "Priority", "\b\b\e[0m");
  struct table tbl = {
    .masterrow = *mrow,
    .clrBefore = 1
  };
  tbl.rows = malloc(sizeof(struct row) * (rc.length + 1));
  int rnext = 0;
  for (unsigned int i = 0; i < rc.length; i++) {
    struct Task* tsk = rc.tasks[i];
    if (tsk) {
      tbl.rows[rnext] = *Tasktorow(*tsk);
      rnext++;
    }
  }
  struct row nul = {.color = rgbi(0), .records = 0};
  tbl.rows[rnext] = nul;
  drawtable(&tbl, stdout);
  putc('\n', stdout);
}

void onuse(cliarg_t* args) {
  if (strcmp("%", args->argument) == 0) {
    char* home = getenv("HOME");
    CURRENT_FILE = strapnd(home, "/.taskrc");
  }
  else
    CURRENT_FILE = args->argument;
  DOWARN -= DOWARN && 1;
}

struct cliargs* cliclient() {
  struct cliargs args[] = {
    cliargs("use",        onuse,        1),
    cliargs("table",      ontable,      0),
    cliargs("diagnostic", ondiagnostic, 0),
    {0,0,0}
  };
  struct cliargs* _ = malloc(sizeof(args));
  memcpy(_, args, sizeof(args));
  if (access("Tasksfile", F_OK) != -1) {
    CURRENT_FILE = "Tasksfile";
    DOWARN += 1 << 0;
  } else {
    char* home = getenv("HOME");
    CURRENT_FILE = strapnd(home, "/.taskrc");
  }
  return(_);
}
