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
char _readchfp(FILE* fp, size_t* n) {
  fsetpos(fp, (fpos_t*)n);
  return(fgetc(fp));
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
  while ((q && c != '"') || (!q && (c != ' ' && c != '\t' && !mlfchar(c)))) {
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
      if (q && c == '\\') {
        // um... pls ignore it...
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
  if (sn == 0) {
    goto MLFC;
  }
  // haha, you read this... umm...
  // pls double press Page Down
  if (q) (*n)++;
  // optimize
  char* new = malloc((sn + 1) * sizeof(char));
  memcpy(new, str, sn);
  new[sn] = 0;
  free(str);
  return(new);
}

static unsigned int _LSTPRSTHROW = 0;

#define prslst prslst_old
struct rcstatementlist* prslst_old(void* data, size_t* n,
    char (*readch) (void* obj, size_t* n), const char* type, size_t* cstart) {
  _LSTPRSTHROW = 0;
  int max = 4;
  int ln = 0;
  char c;
  struct rcstatementlist* lst = malloc(max * sizeof(*lst));
  bool hasEnd = false;
  while (true) {
    bool hasDelim = false;
    c = skipwh(data, n, readch, true);
    *cstart = *n;
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
    if (_vlen == (*n - on - 1) && value[_vlen - 1] == ',') {
      value[_vlen - 1] = 0;
      hasDelim = true;
    } else {
      c = skipwh(data, n, readch, false);
      if (c == ',') {
        hasDelim = true;
        (*n)++;
      }
    }
    // encapsulating
    struct rcstatementlist entry = {.parameter = key, .value = value};
    if (ln + 1 >= max) {
      if (max + ST_LST_JUMP >= ST_LST_MAXLEN) {
        free(lst);
        _LSTPRSTHROW = 2;
        return(0);
      }
      max += ST_LST_JUMP;
      lst = realloc(lst, max * sizeof(*lst));
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
      size_t oldn = *n;
      char* and = prsstr(data, n, readch, false);
      if (!and || strcmp(and, "and") != 0) {
        _LSTPRSTHROW = 3;
        *n = oldn;
        break;
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

void _formres(struct rcparseresult* res, struct rcparseerror* err) {
  res->error = *err;
  struct rcstatement stm = {0,0,0,0};
  res->statement = stm;
}

struct rcparseresult _rcparseln(void* data, size_t* n, 
    char (*readch) (void* obj, size_t* n), bool typecheck) {
  struct rcparseresult res = {.error = {0,0,0}, .statement = {0,0,0}, .query = 0};
  struct rcparseerror err = {0,0,0};
  //struct rcstatement* stm = malloc(sizeof(*stm));
  //ans.statement = stm;
  char c;
  // *skip whitespaces*
  c = skipwh(data, n, readch, true);
  // TYPE PRIMARY with KEY VALUE, KEY VALUE and KEY VALUE POST
  // ^~~~
  err.cstart = *n;
  char* type = prsstr(data, n, readch, false);
  if (!type) {
  STRNUL:
    err.code = 100 + _STRPRSTHROW;
    err.cend = *n;
    _formres(&res, &err);
    return(res);
  }
  // hmm, may be it useless?
  if (typecheck && !rchastype(type)) {
    free(type);
    err.code = 120 + 1;
    err.cend = *n;
    _formres(&res, &err);
    return(res);
  }
  // TYPE PRIMARY with KEY VALUE, KEY VALUE and KEY VALUE POST
  //      ^~~~~~~
  c = skipwh(data, n, readch, true);
  err.cstart = *n;
  char* primary = prsstr(data, n, readch, true);
  if (!primary) {
    free(type);
    goto STRNUL; 
  }
  // TYPE PRIMARY with KEY VALUE, KEY VALUE and KEY VALUE POST
  //              ^~~~
  c = skipwh(data, n, readch, true);
  size_t oldn = *n;
  err.cstart = *n;
  char* _with = prsstr(data, n, readch, true);
  char* post = 0;
  struct rcstatementlist* lst = 0;        /*       without "      */
  if (_with && strcmp(_with, "with") == 0 && (4 == (*n - oldn - 1))) {
    // TYPE PRIMARY with KEY VALUE, KEY VALUE and KEY VALUE POST
    //                   ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    char* _ttype = type;
    if (!typecheck) _ttype = 0;
    err.cstart = *n;
    lst = prslst(data, n, readch, _ttype, &err.cstart);
    if (!lst) {
      free(lst);
      free(type);
      free(primary);
      err.code = 160 + _LSTPRSTHROW;
      err.cend = *n;
      _formres(&res, &err);
      return(res);
    }
    free(_with);
  } else if (_with) {
    // TYPE PRIMARY POST
    //              ^~~~
    post = _with;
  }
  if (!post) {
    // TYPE PRIMARY with KEY VALUE, KEY VALUE and KEY VALUE POST
    //                                                      ^~~~
    (*n)++;
    c = skipwh(data, n, readch, false);
    if (!mlfchar(c)) {
      *n -= 2;
      err.cstart = *n;
      post = prsstr(data, n, readch, true);
    }
  }
  struct rcstatement* stm = &res.statement;
  stm->type = type;
  stm->primary = primary;
  stm->params = lst;
  stm->post = post;
  err.code = 0;
  err.cend = 0;
  return(res);
}

struct rcparseresult rcparseln(const char* data, size_t* n) {
  return(_rcparseln((void*)data, n, (char (*) (void* a1, size_t* a2))_readchc, true));
}

#define LNS_JUMP 8
#define LNS_MAX  1024

void _strtrim(char* str, size_t* start) {
  for(*start = 0; str[*start] == ' '; (*start)++);
}

struct rcfile rcparselns(FILE* data, size_t offset) {
  struct rcfile file = {0,0};
  size_t n2 = offset;
  int max = LNS_JUMP;
  int curr = 0;
  file.result = malloc(max * sizeof(struct rcparseresult*));
  char* str;
  while (getline(&str, &n2, data) != -1) {
    struct rcparseresult* ptr = malloc(sizeof(*ptr));

    size_t trimmed;
    _strtrim(str, &trimmed);
    if (mlfchar(str[trimmed]) || str[trimmed] == '#') {
      ptr = 0;
      goto SKIPPRS;
    }

    size_t ln = strlen(str);
    if (str[ln - 1] == '\n') str[ln - 1] = 0;
    size_t n = 0;
    struct rcparseresult res = 
      rcparseln(str, &n);
    memcpy(ptr, &res, sizeof(*ptr));
    ptr->query = malloc(ln + 1);
    memcpy(ptr->query, str, ln);
    ptr->query[ln] = 0;
    //if (res.error.code && res.error.cstart == 0) {
    //  ptr = 0;
    //}
SKIPPRS:
    if (curr + 1 >= max) {
      if (max + LNS_JUMP >= LNS_MAX) {
        // umm...
        puts("!!! EMERGENCY EXIT !!! rcparselns: "
            "file cannot contains great than LNS_MAX lines.");
        exit(255);
        return(file);
      }
      max += LNS_JUMP;
      file.result = realloc(file.result,
          max * sizeof(struct rcparseresult*));
    }
    *(file.result + curr) = ptr;
    curr++;
  }
  file.length = curr;
  return(file);
}

char* rcprsgeterrordesc(unsigned int code) {
  if (code == 0)
    return "No errors.";
  switch (code) {
    case 101:
    case 181:
    case 201:
      return "String parse error: too big string.";
    case 102:
    case 182:
    case 202:
      return "String parse error: unexpected end.";

    case 121:
      return "Type error: unknown type.";

    case 161:
      return "Type error: unknown param in type.";
    case 162:
      return "List parse error: to many params.";
    case 163:
      return "Syntax error: expected 'and'";
  }
  return "Unknown error occured: may memory issue.";
}
