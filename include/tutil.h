/*
 * Tutil - tasks util
 * Additional functions for taskrc and
 * tasks
 */
#ifndef _THTUTIL
#define _THTUTIL

#include <stdio.h>

#include "taskrc.h"
#include "tasks.h"

// Appends label to the file and update file struct if exists
void rcfileappndlbl(FILE* fp, struct Label label);
// Appends task to the file and update file struct if exists
void rcfileappndtsk(FILE* fp, struct Task task);

// Edit file by the file struct (*file) in one line
void rcfileeditln(struct TaskRcContents* taskrc, FILE* fp,
    unsigned int line);

// Returns error code, 0 - no error.
// Use tutil_getcodedesc() for get code description.
unsigned int tutil_geterrorcode();
// Returns code description.
char* tutil_getcodedesc(unsigned int code);

#endif
