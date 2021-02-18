#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "include/taskrc.h"

// --- ABSTRACT ---

#define ST_STR_MAXLEN 512
#define ST_STR_JUMP 50

#define ST_LST_MAXLEN 64
#define ST_LST_JUMP 8

static char* rctypes[] = {
  "task", "priority", "label", 0,
  "label", "color", 0,
  0
};

// --- CODE ---

char _readchc(const char* data, size_t* n) {
  return(data[(*n)++]);
}

// --- IMPLEMENTATION ---

bool rchastype(const char* type) {
  char** ptr = rctypes;
  while (*ptr != 0) {
    if (strcmp(*ptr, type) == 0)
      return(true);
    for(;*ptr != 0;ptr++);
    ptr++;
  }
  return(false);
}
bool rchasparam(const char* param, const char* type) {
  char** ptr = rctypes;
  while (*ptr != 0) {
    if (strcmp(*ptr, type) == 0) {
      ptr++;
      while (*ptr != 0) {
        if (strcmp(*ptr, param) == 0)
          return(true);
        ptr++;
      }
      // param not found
      return(false);
    }
    for(;*ptr != 0;ptr++);
    ptr++;
  }
  // type not found
  return(false);
}

bool mlfchar(char c) {
  return(c == EOF || c == 0 || c == '\n');
}
char skipwh(void* data, size_t* n,
    char (*readch) (void* obj, size_t* n), bool prev) {
  char c;
  (*n)--;
  do {
    c = readch(data, n);
    if (mlfchar(c)) {
      return(c);
    }
  } while (c == ' ' || c == '\t');
  if (prev) {
    *n -= 2;
    return(readch(data, n));
  }
  return(c);
}

static unsigned int _STRPRSTHROW = 0;

char* prsstr(void* data, size_t* n,
    char (*readch) (void* obj, size_t* n), bool frmtq) {
  _STRPRSTHROW = 0;
  char c = readch(data, n);
  int sn = 0;
  int max = 255;
  char* str = malloc(max * sizeof(char));
  bool q = false;
  if (c == '"' && frmtq) {
    q = true;
    c = readch(data, n);
  }
  while ((q && c != '"') || (!q && (c != ' ' && !mlfchar(c)))) {
    if ((q && !mlfchar(c)) || !q) {
      if (sn + 2 > max) {
        if (max + ST_STR_JUMP >= ST_STR_MAXLEN) {
          _STRPRSTHROW = 1;
          free(str);
          return(0);
        }
        max += ST_STR_JUMP;
        str = realloc(str, max);
      }
      if (c == '\\') {
        c = readch(data, n);
        if (mlfchar(c)) goto MLFC;
      }
      str[sn] = c;
      sn++;
    } else {
MLFC:
      _STRPRSTHROW = 2;
      free(str);
      return(0);
    }
    c = readch(data, n);
  }
  // optimize
  char* new = malloc((sn + 1) * sizeof(char));
  memcpy(new, str, sn);
  new[sn] = 0;
  free(str);
  return(new);
}

static unsigned int _LSTPRSTHROW = 0;

struct rcstatementlist* prslst(void* data, size_t* n,
    char (*readch) (void* obj, size_t* n), const char* type) {
  _LSTPRSTHROW = 0;
  int max = 4;
  int ln = 0;
  char c;
  struct rcstatementlist* lst = malloc(max * sizeof(*lst));
  bool hasEnd = false;
  while (true) {
    bool hasDelim = false;
    c = skipwh(data, n, readch, true);
    char* key = prsstr(data, n, readch, false);
    if (!key) {
      free(lst);
      _LSTPRSTHROW = 20 + _STRPRSTHROW;
      return(0);
    }
    if (type && !rchasparam(key, type)) {
      free(lst);
      _LSTPRSTHROW = 1;
      return(0);
    }
    c = skipwh(data, n, readch, true);
    size_t on = *n;
    char* value = prsstr(data, n, readch, true);
    if (!value) {
      free(lst);
      _LSTPRSTHROW = 40 + _STRPRSTHROW;
      return(0);
    }
    size_t _vlen = strlen(value); // may be fix it...
    // i have no idea how it work...
    // upd: it not work, um..., todo: create issue about it, when
    //  it throw segfault... um...
    if (_vlen - 1 == (*n - on - 1) && value[_vlen - 1] == ',') {
      value[_vlen - 1] = 0;
      hasDelim = true;
    }
    // encapsulating
    struct rcstatementlist entry = {.parameter = key, .value = value};
    if (ln + 2 >= max) {
      if (max + ST_LST_JUMP >= ST_LST_MAXLEN) {
        free(lst);
        _LSTPRSTHROW = 2;
        return(0);
      }
      max += ST_LST_JUMP;
      lst = realloc(lst, max);
    }
    lst[ln] = entry;
    ln++;
    // check for end
    if (!hasEnd) {
      if (hasDelim) continue;
      (*n)--;
      c = readch(data, n);
      if (c == ',') continue;
      c = skipwh(data, n, readch, false);
      if (c == ',') continue;
      (*n)--;
      char* and = prsstr(data, n, readch, false);
      if (!and || strcmp(and, "and") != 0) {
        free(lst);
        _LSTPRSTHROW = 3;
        return(0);
      }
      hasEnd = true;
    } else break;
  }
  struct rcstatementlist* new = malloc((ln + 1) * sizeof(*new));
  memcpy(new, lst, ln * sizeof(*new));
  struct rcstatementlist nul = {0, 0};
  new[ln] = nul;
  free(lst);
  return(new);
}

struct rcline _rcparseln(void* data, size_t* n, 
    char (*readch) (void* obj, size_t* n), bool typecheck) {
  struct rcline ans = {0,0,0};
  //struct rcstatement* stm = malloc(sizeof(*stm));
  //ans.statement = stm;
  char c = ' ';
  // *skip whitespaces*
  c = skipwh(data, n, readch, true);
  // TYPE PRIMARY with KEY VALUE, KEY VALUE and KEY VALUE POST
  // ^~~~
  char* type = prsstr(data, n, readch, false);
  if (!type) {
STRNUL:
    ans.code = 100 + _STRPRSTHROW;
    ans.linen = *n;
    return(ans);
  }
  // hmm, may be it useless?
  if (typecheck && !rchastype(type)) {
    free(type);
    ans.code = 120 + 1;
    ans.linen = *n;
    return(ans);
  }
  // TYPE PRIMARY with KEY VALUE, KEY VALUE and KEY VALUE POST
  //      ^~~~~~~
  c = skipwh(data, n, readch, true);
  char* primary = prsstr(data, n, readch, true);
  if (!primary) {
    free(type);
    goto STRNUL; 
  }
  // TYPE PRIMARY with KEY VALUE, KEY VALUE and KEY VALUE POST
  //              ^~~~
  c = skipwh(data, n, readch, true);
  char* _with = prsstr(data, n, readch, false);
  if (!_with) {
    free(type);
    free(primary);
    goto STRNUL;
  }
  if (strcmp(_with, "with") != 0) {
    free(_with);
    free(type);
    free(primary);
    ans.code = 140 + 1;
    ans.linen = *n;
    return(ans);
  }
  free(_with);
  // TYPE PRIMARY with KEY VALUE, KEY VALUE and KEY VALUE POST
  //                   ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  char* _ttype = type;
  if (!typecheck) _ttype = 0;
  struct rcstatementlist* lst = prslst(data, n, readch, _ttype);
  if (!lst) {
    free(lst);
    free(type);
    free(primary);
    ans.code = 160 + _LSTPRSTHROW;
    ans.linen = *n;
    return(ans);
  }
  struct rcstatement* stm = malloc(sizeof(*stm));
  stm->type = type;
  stm->primary = primary;
  stm->params = lst;
  stm->post = "TODO:";
  ans.linen = 0;
  ans.code = 0;
  ans.statement = stm; 
  return(ans);
}

struct rcline rcparseln(const char* data, size_t* n) {
  return(_rcparseln((void*)data, n, (char (*) (void* a1, size_t* a2))_readchc, true));
}
struct rcline* rcparselns(const char* data, size_t offset);

