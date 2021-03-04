#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef _THUTILS_INCLUDE_USELESS_FUNCTIONS

#define _GET_PNAME_PBEGIN "/proc/"
#define _GET_PNAME_PEND "/comm"

#define _GETPPIDOF_C_PBEGIN "/proc/"
#define _GETPPIDOF_C_PEND "/stat"

char* read_file(const char* filename) {
  FILE* f = fopen(filename, "rb");
  fseek(f, 0, SEEK_END);
  size_t fsize = ftell(f);
  fseek(f, 0, SEEK_SET);  //same as rewind(f);

  char* string = (char*)malloc(fsize + 1);
  fread(string, fsize, 1, f);
  fclose(f);

  string[fsize] = 0;
  return(string);
}

char* get_pname(unsigned int pid) {
  char* _spid = calloc(12, sizeof(char));
  sprintf(_spid, "%u", pid);
  char* path = malloc(strlen(_GET_PNAME_PBEGIN _GET_PNAME_PEND) + 
      strlen(_spid));
  sprintf(path, _GET_PNAME_PBEGIN "%u" _GET_PNAME_PEND, pid);
  free(_spid);

  FILE* fp = fopen(path, "r");
  if (!fp) return("/sbin/init");
  char* ans;
  size_t n = 0;
  getline(&ans, &n, fp);
  fclose(fp);
  free(path);

  return(ans);
}

// https://gist.github.com/Amchik/64e1c40bdd531a9fb09df6a2931cfda9
unsigned int getppidof(unsigned int pid) {
  char* spid = calloc(12, sizeof(char));
  sprintf(spid, "%u", pid);
  char* path = malloc(strlen(_GETPPIDOF_C_PBEGIN _GETPPIDOF_C_PEND) + 
      strlen(spid) + 1);
  sprintf(path, _GETPPIDOF_C_PBEGIN "%s" _GETPPIDOF_C_PEND, spid);
  free(spid);

  FILE* fp = fopen(path, "r");
  if (!fp) return(0); // process $pid doesnt exists
  char* sppid;
  size_t n = 0;
  getline(&sppid, &n, fp);
  sppid = strtok(sppid, ")"); // see proc(5)
  sppid = strtok(0, ")");     // for more info...
  sppid = strtok(sppid, " ");
  sppid = strtok(0, " ");     // getting ppid
  fclose(fp);

  unsigned int ppid = atoi(sppid);
  free(path);
  return(ppid);
}

bool strends(const char* str, const char* suffix) {
  size_t sn = strlen(suffix);
  size_t strn = strlen(str);
  if (sn > strn || strn == 0 || sn == 0) return(false);
  for (int i = sn - 1; i >= strn - 1; i--) {
    if (str[i] != suffix[i]) return(false);
  }
  return(true);
}

int strdlen(const char* string) {
  int dlen = 0;
  int rlen = strlen(string);
  bool escape = false;
  for (int i = 0; i < rlen; i++) {
    if (string[i] == '\e') {
      escape = true;
    } else if (escape && string[i] == 'm') {
      escape = false;
    } else if (!escape) {
      dlen++;
    }
  }
  return(dlen);
}

#endif

char* strcapnd(char* original, char new) {
  size_t size;
  if (original) size = strlen(original) + 2;
  else size = 2;
  char* tmp = malloc(size);
  tmp[0] = '\0';
  if (!original) original = "";
  sprintf(tmp, "%s%c", original, new);
  free(original);
  return(tmp);
}

char* strapnd(const char* s1, const char* s2) {
  // todo: move into another file
  size_t resc = strlen(s1) + strlen(s2) + 1;
  char* result = calloc(resc, sizeof(char));
  strcat(result, s1);
  strcat(result, s2);
  return(result);
}

char* itoa(int n) {
  char* ans = calloc(12, sizeof(char));
  sprintf(ans, "%d", n);
  return(ans);
}

