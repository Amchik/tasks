#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "include/taskrc.h"
#include "include/tasks.h"
#include "include/tutil.h"

void rcfileappndq(FILE* fp, char* query) {
  fprintf(fp, "\n%s", query);
}
char* getsafestr(const char* str) {
  char* lname = calloc(512, 1);
  size_t slen = strlen(str);
  for(int i = 0, j = 0; i < 512 && j < slen; i++) {
    switch(str[j]) {
      case '"':
        lname[i] = '\\';
        i++;
        break;
      case '\\':
        lname[i] = '\\';
        i++;
        break;
    }
    lname[i] = str[j];
    j++;
  }
  return(lname);
}

char* formT(struct Task task) {
  char* q = calloc(1024, 1);
  char* comp = "";
  if (task.completed) comp = " completed";
  char* prim = getsafestr(task.description);
  char* prio = itoa(task.priority);
  char* labl = getsafestr(task.label.name);
  sprintf(q, "task \"%s\" with label \"%s\" and priority %s%s",
      prim, labl, prio, comp);
  free(prim);
  free(prio);
  free(labl);
  return(q);
}

char* formL(struct Label label) {
  char* q = calloc(1024, 1);
  char* name = getsafestr(label.name);
  sprintf(q, "label \"%s\" with color #%.6lX", name, rgbl(label.color));
  free(name);
  return(q);
}

// Appends label to the file and update file struct if exists
void rcfileappndlbl(FILE* fp, struct Label label) {
  char* q = formL(label);
  rcfileappndq(fp, q);
  free(q);
}
// Appends task to the file and update file struct if exists
void rcfileappndtsk(FILE* fp, struct Task task) {
  char* q = formT(task);
  rcfileappndq(fp, q);
  free(q);
}

// Edit file by the file struct (*file) in one line
void rcfileeditln(struct TaskRcContents* taskrc, FILE* fp,
    unsigned int line) {
  char* q = 0;
  if (taskrc->labels[line] == 0) {
    if (taskrc->tasks[line] == 0)
      q = calloc(1, 1);
    else
      q = formT(*taskrc->tasks[line]);
  } else
    q = formL(*taskrc->labels[line]);
  // TODO: comment
  fseek(fp, 0, SEEK_END);
  unsigned long fsize = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  unsigned int ln = 1;
  size_t offset = 0, offset2 = 0;
  while (ln < line) {
    char c;
    while ((c = getc(fp)) != '\n') 
      offset++;
    offset++;
    ln++;
  }
  offset2 = offset;
  while (ln <= line) {
    char c;
    while ((c = getc(fp)) != '\n') 
      offset2++;
    offset2++;
    ln++;
  }
  size_t datalen = fsize - offset2;
  char* dataend = malloc(datalen + 1);
  fseek(fp, offset2, SEEK_SET);
  fread(dataend, datalen, 1, fp);
  dataend[datalen] = 0;
  fseek(fp, offset, SEEK_SET);
  fputs(q, fp);
  fputc('\n', fp);
  fputs(dataend, fp);
  size_t wlen = strlen(q) + 1 + offset + datalen;
  if (wlen < fsize) {
    for(size_t i = wlen; i < fsize; i++) fputc(0, fp);
    // note: it needs to be fixed normaly, for example
    //  after this overwrite file contents before \0
    // and yeah, FIXME
  }
  // --
  free(q);
  free(dataend);
}

// Returns error code, 0 - no error.
// Use tutil_getcodedesc() for get code description.
unsigned int tutil_geterrorcode() {
  return(0);
}
// Returns code description.
char* tutil_getcodedesc(unsigned int code) {
  if (code == 0)
    return("No errors.");
  return("Unknown error occured: may memory issue.");
}
