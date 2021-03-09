#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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

void _calcerrors(struct TaskRcContents rc) {
  unsigned int count = 0;
  for (unsigned int i = 0; i < rc.length; i++) {
    if (rc.results[i] && rc.results[i]->error.code >= 100)
      count++;
  }
  if (count) {
    warn("While parsing taskrc found %u errors. Run \e[1m%s \e[3m[use %s]\e[0;1m diagnostic\e[0m"
        " for more information.",
        count, zarg, CURRENT_FILE);
  }
}

// _findtype(content-
int _fndtp(const struct TaskRcContents* cntnt, char* type, char* primary) {
  for(int i = 0; i < (int)cntnt->length; i++) {
    if (cntnt->results[i] != 0 && 
        cntnt->results[i]->error.code == 0 &&
        strcmp(type, cntnt->results[i]->statement.type) == 0 &&
        strcmp(primary, cntnt->results[i]->statement.primary) == 0)
      return i;
  }
  return(-1);
}

int _clcwhtedt(char* type, char* toedit) {
  bool task = !strcmp(type, "task");
  bool labl = !strcmp(type, "label");
  if (strcmp(toedit, "primary") == 0) {
    return(1 + labl);
  } else if (strcmp(toedit, "post") == 0) {
    return(3 + labl);
  } else if (task && strcmp(toedit, "label") == 0) {
    return(11);
  } else if (task && strcmp(toedit, "priority") == 0) {
    return(12);
  } else if (task && strcmp(toedit, "status") == 0) {
    return(13);
  } else if (labl && strcmp(toedit, "color") == 0) {
    return(21);
  }
  return(-1);
}

void oneditln(cliarg_t* arg) {
  char* type = arg->argument;
  clinextarg(arg);
  char* prim = arg->argument;
  FILE* fp = fopen(CURRENT_FILE, "r+");
  if (!fp) {
    error("todo move it into function aaa. fp == 0");
    exit(255);
  }
  struct TaskRcContents cntnt = parseTaskRc(fp);
  fseek(fp, 0, SEEK_SET);
  int where = _fndtp(&cntnt, type, prim);
  if (where == -1) {
    error("Record \"%s\" of type %s not found.", prim, type);
    exit(4);
  }
  clinextarg(arg);
  char* toedit = arg->argument;
  clinextarg(arg);
  char* withedit = arg->argument;
  int whatedit = _clcwhtedt(type, toedit);
  if (whatedit == -1) {
    error("Unknown param to edit. Avalibe: 1, 2, 11, 12, 13, 21");
  }
  switch(whatedit) {
    case 1:
      cntnt.tasks[where]->description = withedit;
      break;
    case 2:
      cntnt.labels[where]->name = withedit;
      break;
    case 3:
      warn("Unsupported action 'post' for type 'task'. Please use complete in future.");
      goto TASKPOST;
      break;
    case 4:
      warn("Unsupported action 'post' for type 'task'. Skipping...");
      break;
    case 11:
      {
        int wl = _fndtp(&cntnt, "label", withedit);
        if (wl == 0) 
          warn("No label %%%s found. Skipping...", withedit);
        else
          cntnt.tasks[where]->label = *cntnt.labels[wl];
      }
      break;
    case 12:
      cntnt.tasks[where]->priority = atoi(withedit);
      break;
    case 13:
TASKPOST:
      cntnt.tasks[where]->completed = strcmp(withedit, "yes") == 0;
      break;
    case 21:
      withedit++;
      cntnt.labels[where]->color = rgbi(strtol(withedit, 0, 16));
      break;
  }
  rcfileeditln(&cntnt, fp, where + 1);
  fclose(fp);
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

struct cliargs* cliclient(char* arg0) {
  zarg = arg0;
  struct cliargs args[] = {
    cliargs("use",        onuse,        1),
    cliargs("table",      ontable,      0),
    cliargs("diagnostic", ondiagnostic, 0),
    cliargs("edit",       oneditln,     4),
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
