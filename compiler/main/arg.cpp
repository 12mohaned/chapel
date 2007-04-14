/*****************************************************************************

This code is based on arg.cc developed by John Plevyak and released as
part of his iterative flow analysis package available at SourceForge
(http://sourceforge.net/projects/ifa/).  The original code is:

Copyright (c) 1992-2006 John Plevyak.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************************/


#include <stdio.h>
#include "arg.h"
#include "chpl.h"

static char *SPACES = "                                                                               ";
static char *arg_types_keys = (char *)"IPSDfF+TLN";
static char *arg_types_desc[] = {
  (char *)"int     ",
  (char *)"path    ",
  (char *)"string  ",
  (char *)"double  ",
  (char *)"->false ",
  (char *)"->true  ",
  (char *)"incr    ",
  (char *)"toggle  ",
  (char *)"int64   ",
  (char *)"->true  ",
  (char *)"        "
};


static void 
bad_flag(char* flag) {
  fprintf(stderr, "Unrecognized flag: '%s' (use '-h' for help)\n", flag);
  exit(1);
}


static void 
missing_arg(char* currentFlag) {
  fprintf(stderr, "Missing argument for flag: '%s' (use '-h' for help)\n", 
          currentFlag);
  exit(1);
}


void 
process_arg(ArgumentState *arg_state, int i, char ***argv, char* currentFlag) {
  char * arg = NULL;
  ArgumentDescription *desc = arg_state->desc;
  if (desc[i].type) {
    char type = desc[i].type[0];
    if (type=='F'||type=='f'||type=='N')
      *(bool *)desc[i].location = type!='f' ? true : false;
    else if (type=='T')
      *(int *)desc[i].location = !*(int *)desc[i].location;
    else if (type == '+') 
      (*(int *)desc[i].location)++;
    else {
      arg = *++(**argv) ? **argv : *++(*argv);
      if (!arg) missing_arg(currentFlag);
      switch (type) {
        case 'I':
          *(int *)desc[i].location = atoi(arg);
          break;
        case 'D':
          *(double *)desc[i].location = atof(arg);
          break;
        case 'L':
          *(int64 *)desc[i].location = atoll(arg);
          break;
        case 'P': strncpy((char *)desc[i].location,arg, FILENAME_MAX);
          break;
        case 'S': strncpy((char *)desc[i].location,arg, atoi(desc[i].type+1));
          break;
        default:
          fprintf(stderr, "%s:bad argument description\n", 
                 arg_state->program_name);
          exit(1);
          break;
      }
      **argv += strlen(**argv)-1;
    }
  }
  if (desc[i].pfn)
    desc[i].pfn(arg_state, arg);
}


void
process_args(ArgumentState *arg_state, int argc, char **aargv) {
  int i, len;
  char *end = 0;
  char **argv = (char**)malloc((argc+1)*sizeof(char*));
  for (i = 0; i < argc; i++)
    argv[i] = _dupstr(aargv[i]);
  argv[i] = NULL;
  ArgumentDescription *desc = arg_state->desc;
  /* Grab Environment Variables */
  for (i = 0;; i++) {
    if (!desc[i].name)
      break; 
    if (desc[i].env) {
      char type = desc[i].type[0];
      char * env = getenv(desc[i].env);
      if (!env) continue;
      switch (type) {
        case '+': (*(int *)desc[i].location)++; break;
        case 'f': 
        case 'F': 
        case 'N':
          *(bool *)desc[i].location = type!='f'?1:0; break;
        case 'T': *(int *)desc[i].location = !*(int *)desc[i].location; break;
        case 'I': *(int *)desc[i].location = strtol(env, NULL, 0); break;
        case 'D': *(double *)desc[i].location = strtod(env, NULL); break;
        case 'L': *(int64 *)desc[i].location = strtoll(env, NULL, 0); break;
        case 'P': strncpy((char *)desc[i].location, env, FILENAME_MAX); break;
        case 'S': strncpy((char *)desc[i].location, env, strtol(desc[i].type+1, NULL, 0)); break;
      }
      if (desc[i].pfn)
        desc[i].pfn(arg_state, env);
    }
  }

  /*
    Grab Command Line Arguments
  */
  while (*++argv) {
    if (**argv == '-') {
      if ((*argv)[1] == '-') {
        for (i = 0;; i++) {
          if (!desc[i].name)
            bad_flag(*argv);
          if ((end = strchr((*argv)+2, '=')))
            len = end - ((*argv) + 2);
          else
            len = strlen((*argv) + 2);
          int flaglen = (int)strlen(desc[i].name);
          if (len == flaglen &&
              !strncmp(desc[i].name,(*argv)+2, len))
          {
            char* currentFlag = _dupstr(*argv);
            if (!end)
              *argv += strlen(*argv) - 1;
            else
              *argv = end;
            process_arg(arg_state, i, &argv, currentFlag);
            break;
          } else if (desc[i].type && desc[i].type[0] == 'N' &&
                     len == flaglen+3 &&
                     !strncmp("no-", (*argv)+2, 3) &&
                     !strncmp(desc[i].name,(*argv)+5,len-3)) {
            char* currentFlag = _dupstr(*argv);
            if (!end)
              *argv += strlen(*argv) - 1;
            else
              *argv = end;
            desc[i].type = "f";
            process_arg(arg_state, i, &argv, currentFlag);
            desc[i].type = "N";
          }
        }
      } else {
        while (*++(*argv))
          for (i = 0;; i++) {
            if (!desc[i].name)
              bad_flag((*argv)-1);
            if (desc[i].key == **argv) {
              process_arg(arg_state, i, &argv, (*argv)-1);
              break;
            }
          }
      }
    } else {
      arg_state->file_argument = (char **)realloc(
        arg_state->file_argument, 
        sizeof(char**) * (arg_state->nfile_arguments + 2));
      arg_state->file_argument[arg_state->nfile_arguments++] = *argv;
      arg_state->file_argument[arg_state->nfile_arguments] = NULL;
    }
  }
}

void 
usage(ArgumentState *arg_state, char *arg_unused) {
  ArgumentDescription *desc = arg_state->desc;
  int i;

  (void)arg_unused;
  fprintf(stderr,"Usage: %s [flags|source files]\n",arg_state->program_name);
  for (i = 0;; i++) {
    if (!desc[i].name)
      break;
    if (desc[i].name[0] == '\0') {
      if (strcmp(desc[i].description, "Developer Flags") == 0) {
        if (developer) {
          fprintf(stderr, "\n\n%s\n", desc[i].description);
          fprintf(stderr, "===============\n");
          continue;
        } else {
          break;
        }
      } else {
        int len = strlen(desc[i].description);
        int j;
        static bool firstDesc = true;
        fprintf(stderr, "\n%s:", desc[i].description);
        if (firstDesc) {
          fprintf(stderr, "       default   type     description");
        }
        fprintf(stderr, "\n");
        for (j=0; j<= len; j++) {
          fprintf(stderr, "-");
        }
        if (firstDesc) {
          fprintf(stderr, "       --------  -------  --------------------------------");
          firstDesc = false;
        }
        fprintf(stderr, "\n");
        //        fprintf(stderr, "-------------------------\n");
        continue;
      }
    }
    int isNoFlag = (desc[i].type && desc[i].type[0] == 'N') ? 5 : 0;
    fprintf(stderr,"  %c%c%c %s%s%s%s ", 
            desc[i].key != ' ' ? '-' : ' ', desc[i].key, 
            (desc[i].key != ' ' && desc[i].name && desc[i].name[0]) ? ',' : ' ', 
            (desc[i].name && desc[i].name[0] != '\0') ? "--" : "  ",
            (isNoFlag) ? "[no-]" : "",
            desc[i].name,
            (strlen(desc[i].name) + isNoFlag + 62 < 79) ?
            &SPACES[strlen(desc[i].name) + isNoFlag + 62] : "");
    switch(desc[i].type?desc[i].type[0]:0) {
      case 0: fprintf(stderr, "         "); break;
      case 'L':
        fprintf(stderr,
#ifdef __alpha
                "%-9ld",
#elifdef FreeBSD
                "%-9qd",
#else
                "%-9lld",
#endif
                *(int64*)desc[i].location);
        break;
      case 'P':
      case 'S':
        if (*(char*)desc[i].location) {
          if (strlen((char*)desc[i].location) < 10)
            fprintf(stderr, "%-9s", (char*)desc[i].location);
          else {
            ((char*)desc[i].location)[7] = 0;
            fprintf(stderr, "%-7s..", (char*)desc[i].location);
          }
        } else
          fprintf(stderr, "         ");
        break;
      case 'D':
        fprintf(stderr, "%-9.3e", *(double*)desc[i].location);
        break;
      case '+': 
      case 'I':
        fprintf(stderr, "%-9d", *(int *)desc[i].location);
        break;
    case 'T': case 'f': case 'F': case 'N':
        fprintf(stderr, "%-9s", *(bool *)desc[i].location?"true ":"false");
        break;
    }
    fprintf(stderr, " %s", 
            arg_types_desc[desc[i].type?strchr(arg_types_keys,desc[i].type[0])-
                           arg_types_keys : strlen(arg_types_keys)]);

    fprintf(stderr," %s\n",desc[i].description);
  }
  exit(1);
}

void
free_args(ArgumentState *arg_state) {
  if (arg_state->file_argument)
    free(arg_state->file_argument);
}
