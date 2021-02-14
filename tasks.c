#include <stdlib.h>
#include <stdbool.h>

#include "include/tasks.h"
#include "include/table.h"
#include "include/utils.h"

// Creates Label
struct Label createLabel(const char* name, struct color color) {
  struct Label label = {.name = name, .color = color};
  return(label);
}

// Creates Task
struct Task createTask(unsigned int id, const char* description, struct Label label, int priority) {
  struct Task task = {
    .id = id,
    .description = description,
    .label = label,
    .priority = priority,
    .completed = false
  };
  return(task);
}
// Converts Task to struct row
struct row* Tasktorow(struct Task task) {
  struct row* row = malloc(sizeof(struct row));
  row->color = task.label.color;
  row->records = malloc(sizeof(record_t) * 4);
  row->records[3] = 0;
  row->records[0] = task.description;
  row->records[1] = task.label.name;
  row->records[2] = itoa(task.priority);
  return(row);
}
