#ifndef _THTASKS
#define _THTASKS

#include <stdbool.h>

#include "table.h"
#include "taskrc.h"

// Task label
struct Label {
  const char* name;
  struct color color;
};
// TO-DO Task
struct Task {
  // Id
  unsigned int id;
  // Task fully description
  const char* description;
  // Label
  struct Label label;
  // Priority
  int priority;

  // Is completed?
  bool completed;
};

// United Task and Label array
struct TaskRcContents {
  // Array of pointer to tasks.
  // Pointer can be equals zero if in line no task query. See results[line]->error for check for error.
  struct Task** tasks;
  // Array of pointer to labels.
  // Pointer can be equals zero if in line no label query. See results[line]->error for check for error.
  struct Label** labels;
  // Array of pointer to query parse results.
  // Pointer can be equals zero if line commented.
  struct rcparseresult** results;

  // Length of parse data and all arrays
  unsigned int length;
};

// Default label, use if no label provided.
static const struct Label NOLABEL = { .name = "", .color = {0,0,0} };

// Creates Label
struct Label createLabel(const char* name, struct color color);

// Creates Task
struct Task createTask(unsigned int id, const char* description, struct Label label, int priority);
// Converts Task to struct row
struct row* Tasktorow(struct Task task);

// Parsing file via rcparselns and converts it to task and labels.
struct TaskRcContents parseTaskRc(FILE* stream);

#endif
