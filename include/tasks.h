#ifndef _THTASKS
#define _THTASKS

#include <stdbool.h>

#include "table.h"

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

// Creates Label
struct Label createLabel(const char* name, struct color color);

// Creates Task
struct Task createTask(unsigned int id, const char* description, struct Label label, int priority);
// Converts Task to struct row
struct row* Tasktorow(struct Task task);

#endif
