#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "driver.h"
#include "../main/version.h"
#include "files.h"
#include "geysa.h"
#include "ast.h"
#include "if1.h"
#include "var.h"
#include "pnode.h"
#include "baseAST.h"

int verbose_level = 0;
int *assert_NULL_var = 0;

void cleanup_for_exit(void) {
  deleteTmpDir();
  stopCatchingSignals();
}

static void gdbShouldBreakHere() {
}

void
clean_exit(int status) {
  if (status != 0) {
    gdbShouldBreakHere();
  }
  cleanup_for_exit();
  exit(status);
}

int
show_error(char *str, IFAAST *a, ...) {
  char nstr[1024];
  va_list ap;
  va_start(ap, a);
  snprintf(nstr, 1023, "%s:%d: %s\n", a->pathname(), a->line(), str);
  vfprintf(stderr, nstr, ap);
  va_end(ap);
  return -1;
}

#ifndef TEST_LIB
int
show_error(char *str, Var *v, ...) {
  char nstr[1024];
  va_list ap;
  va_start(ap, v);
  if (v->sym->ast)
    snprintf(nstr, 1023, "%s:%d: %s\n", v->sym->pathname(), v->sym->line(), str);
  else if (v->def && v->def->code && v->def->code->ast)
    snprintf(nstr, 1023, "%s:%d: %s\n", v->def->code->pathname(), v->def->code->line(), str);
  else
    snprintf(nstr, 1023, "error: %s\n", str);
  vfprintf(stderr, nstr, ap);
  va_end(ap);
  return -1;
}
#else
bool ignore_errors = 0;
bool developer = false;
void get_version(char *) {}
#endif

int
buf_read(char *pathname, char **buf, int *len) {
  struct stat sb;
  int fd;

  *buf = 0;
  *len = 0;
  fd = open(pathname, O_RDONLY);
  if (fd <= 0) 
    return -1;
  memset(&sb, 0, sizeof(sb));
  fstat(fd, &sb);
  *len = sb.st_size;
  *buf = (char*)MALLOC(*len + 2);
  (*buf)[*len] = 0;             /* terminator */
  (*buf)[*len + 1] = 0;         /* sentinal */
  read(fd, *buf, *len);
  close(fd);
  return *len;
}

char *
get_file_line(char *filename, int lineno) {
  static char *last_filename = 0;
  static char *last_buf = 0;
  static Vec<char *> last_lines;

  if (!last_filename || strcmp(filename, last_filename)) {
    int len = 0;
    char *new_buf = 0;
    if (buf_read(filename, &new_buf, &len) < 0)
      return 0;
    last_filename = dupstr(filename);
    last_buf = new_buf;
    char *b = new_buf;
    last_lines.clear();
    last_lines.add(b);
    b = strchr(b, '\n');
    while (b) {
      *b = 0;
      b++;
      last_lines.add(b);
      b = index(b, '\n');
    }
  }
  lineno--; // 0 based
  if (lineno < 0 || lineno > last_lines.n)
    return NULL;
  return last_lines.v[lineno];
}

void
fail(char *str, ...) {
  char nstr[256];
  va_list ap;

  fflush(stdout);
  fflush(stderr);

  va_start(ap, str);
  snprintf(nstr, 255, "fail: %s\n", str);
  vfprintf(stderr, nstr, ap);
  va_end(ap);
  clean_exit(1);
}

char *
dupstr(char *s, char *e) {
  int l = e ? e-s : strlen(s);
  char *ss = (char*)MALLOC(l+1);
  memcpy(ss, s, l);
  ss[l] = 0;
  return ss;
}

void myassert(char *file, int line, char *str) {
  printf("assert %s:%d: %s\n", file, line, str);
  *(int*)0 = 1;
}


// Support for internal errors, adopted from ZPL compiler

static bool isFatal = true;


static char* internalErrorCode(char* filename, int lineno) {
  static char error[8];

  char* filename_start = strrchr(filename, '/');
  if (filename_start) {
    filename_start++;
  }
  else {
    filename_start = filename;
  }
  strncpy(error, filename_start, 3);
  sprintf(error, "%s%04d", error, lineno);
  for (int i = 0; i < 7; i++) {
    if (error[i] >= 'a' && error[i] <= 'z') {
      error[i] += 'A' - 'a';
    }
  }
  return error;
}


int setupDevelError(char *filename, int lineno, bool fatal, bool user, bool cont) {
  if (developer) {
    fprintf(stderr, "[%s:%d] ", filename, lineno);
  }

  if (!user) {
    if (fatal) {
      fprintf(stderr, "Internal error: ");
    }
    else {
      fprintf(stderr, "Internal warning: ");
    }
  }
  else {
    if (fatal) {
      fprintf(stderr, "Error: ");
    }
    else {
      fprintf(stderr, "Warning: ");
    }
  }

  if (!user && !developer) {
    char version[128];
    fprintf(stderr, "%s ", internalErrorCode(filename, lineno));
    get_version(version);
    fprintf(stderr, "chpl Version %s\n", version);
    if (fatal) {
      clean_exit(1);
    }
  }

  isFatal = !cont;
  return 1;
}


static void printUsrLocation(char* filename, int lineno) {
  if (filename || lineno) {
    fprintf(stderr, " (");
    if (filename) {
      fprintf(stderr, "%s", filename);
    }
    if (lineno) {
      if (filename) {
        fprintf(stderr, ":");
      } else {
        fprintf(stderr, "line ");
      }
      fprintf(stderr, "%d", lineno);
    }
    fprintf(stderr, ")");
  }
  fprintf(stderr, "\n");
}


void printProblem(char *fmt, ...) {
  va_list args;

  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);

  printUsrLocation(NULL, 0);

  if (isFatal && !ignore_errors) {
    clean_exit(1);
  }
}


void printProblem(IFAAST* ast, char *fmt, ...) {
  va_list args;
  int usrlineno = 0;
  char *usrfilename = NULL;

  if (ast) {
    usrlineno = ast->line();
    usrfilename = ast->pathname();
  }
  
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);

  printUsrLocation(usrfilename, usrlineno);

  if (isFatal && !ignore_errors) {
    clean_exit(1);
  }
}


void printProblem(BaseAST* ast, char *fmt, ...) {
  va_list args;
  int usrlineno = 0;
  char *usrfilename = NULL;

  if (ast) {
    usrfilename = ast->filename;
    usrlineno = ast->lineno;
  }
  
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);

  printUsrLocation(usrfilename, usrlineno);

  if (developer && ast) {
    if (ast->traversalInfo) {
      fprintf(stderr, "  Constructed in traversal %s\n", ast->traversalInfo);
    }
    if (ast->copyInfo) {
      forv_Vec(char, str, *(ast->copyInfo)) {
        fprintf(stderr, "  Copied in traversal %s\n", str);
      }
    }
  }

  if (isFatal && !ignore_errors) {
    clean_exit(1);
  }
}


static void handleInterrupt(int sig) {
  INT_FATAL("received interrupt");
}

static void handleSegFault(int sig) {
  INT_FATAL("seg fault");
}


void startCatchingSignals(void) {
  if (!developer) {
    signal(SIGINT, handleInterrupt);
    signal(SIGSEGV, handleSegFault);
  }
}


void stopCatchingSignals(void) {
  signal(SIGINT, SIG_DFL);
  signal(SIGSEGV, SIG_DFL);
}
