#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "include/tasks.h"
#include "include/table.h"
#include "include/utils.h"
#include "include/taskrc.h"

#define COMPLETED "✓"

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
  row->records = malloc(sizeof(record_t) * 5);
  row->records[4] = 0;
  row->records[0] = task.description;
  row->records[1] = task.label.name;
  row->records[2] = itoa(task.priority);
  row->records[3] = "";
  if (task.completed) row->records[3] = COMPLETED;
  return(row);
}

struct TaskRcContents parseTaskRc(FILE* stream) {
  struct rcfile file = rcparselns(stream, 0);
  // 2 loops: parsing labels then tasks
  struct Label** labels = calloc(file.length, sizeof(void*));
  struct Task**  tasks  = calloc(file.length, sizeof(void*));
  // 1:
  for(int i = 0; i < file.length; i++) {
    struct rcparseresult* res = file.result[i];
    if (!res || res->error.code 
        || strcmp(res->statement.type, "label") != 0) continue;
    struct Label lbl = {
      .name = res->statement.primary,
      .color = {0,0,0}
    };
    if (res->statement.params) {
      const char* _clr = res->statement.params->value;
      if (_clr && _clr[0] == '#') {
        _clr++;
        errno = 0;
        lbl.color = rgbi(
            strtol(_clr, 0, 16)
            );
        if (errno != 0) {
          errno = 0;
          res->error.code = 3;
        }
      }
    }
    labels[i] = malloc(sizeof(lbl));
    memcpy(labels[i], &lbl, sizeof(lbl));
  }
  // 2:
  for (int i = 0; i < file.length; i++) {
    struct rcparseresult* res = file.result[i];
    if (!res || res->error.code 
        || strcmp(res->statement.type, "task") != 0) continue;
    struct Task task = {
      .id = 0,
      .description = res->statement.primary,
      .completed = (res->statement.post 
          && strcmp(res->statement.post, "completed") == 0),
      .label = NOLABEL,
      .priority = 10
    };
    if (res->statement.params) {
      int i = 0;
      struct rcstatementlist* params = res->statement.params;
      while ((params + i)->parameter != 0) {
        if (strcmp((params + i)->parameter, "label") == 0) {
          // find label
          for (int j = 0; j < file.length; j++) {
            if (labels[j] == 0) continue;
            if (strcmp(labels[j]->name, (params + i)->value) == 0) {
              task.label = *labels[j];
              break;
            }
          }
          if (task.label.name == NOLABEL.name) {
            res->error.code = 1;
          }
        } else if (strcmp((params + i)->parameter, "priority") == 0) {
          errno = 0;
          task.priority = strtol((params + i)->value, 0, 10);
          if (errno != 0) {
            errno = 0;
            task.priority = 10;
            res->error.code = 2;
          }
        }
        i++;
      }
    }
    tasks[i] = malloc(sizeof(task));
    memcpy(tasks[i], &task, sizeof(task));
  }
  struct TaskRcContents ans = {
    .tasks = tasks,
    .labels = labels,
    .results = file.result,
    .length = file.length,
  };
  return(ans);
}
