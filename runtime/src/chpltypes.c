#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chplmem.h"
#include "chplrt.h"
#include "chpltypes.h"
#include "error.h"


/* _glom_strings() expects every argument after the first to be 
   of type char*. */

char* _glom_strings(int numstrings, ...) {
  static int maxNumStrings = 0;
  static char** stringlist = NULL;
  int i;

  if (numstrings > maxNumStrings) {
    maxNumStrings = numstrings;
    stringlist = (char**)_chpl_realloc(stringlist, numstrings, sizeof(char*),
                                       "stringlist buffer in _glom_strings");
  }

  va_list ap;
  va_start(ap, numstrings);
  for (i=0; i<numstrings; i++) {
    stringlist[i] = va_arg(ap, char*);
  }
  va_end(ap);

  unsigned int totlen = 0;
  for (i=0; i<numstrings; i++) {
    totlen += strlen(stringlist[i]);
  }

  char* newstring = (char*)_chpl_malloc((totlen + 1), sizeof(char), 
                                        "_glom_strings result");
  newstring[0] = '\0';
  for (i=0; i<numstrings; i++) {
    strcat(newstring, stringlist[i]);
  }

  if (strlen(newstring) > totlen) {
    char* message = "_glom_strings() buffer overflow";
    printInternalError(message);
  }

  return newstring;
}


/* _string _copy_string(_string* lhs, _string rhs) { */
/*   /\*** REALLOC vs MALLOC */
/*        We have memory leaks when dealing with strings. */
/*        *lhs = (char*)_chpl_realloc(*lhs, (strlen(rhs)+1), sizeof(char), "string copy"); *\/ */
/*   *lhs = (char*)_chpl_malloc(strlen(rhs)+1, sizeof(char), "string copy"); */

/*   strcpy(*lhs, rhs); */
/*   return *lhs; */
/* } */


char* _chpl_tostring_bool(_bool x, char* format) {
  if (x) {
    return _glom_strings(1, "true");
  } else {
    return _glom_strings(1, "false");
  }
}


char* _chpl_tostring_int(_int64 x, char* format) {
  char buffer[256];
  sprintf(buffer, format, x);
  return _glom_strings(1, buffer);
}


char* _chpl_tostring_float(_float64 x, char* format) {
  char buffer[256];
  sprintf(buffer, format, x);
  return _glom_strings(1, buffer);
}


char* _chpl_tostring_complex( _complex64 x, char* format) {
  char buffer[256];
  sprintf(buffer, format, x.re, x.im);
  return _glom_strings(1, buffer);
}


_string
string_concat(_string x, _string y) {
  return _glom_strings(2, x, y);
}


_string
string_strided_select(_string x, int low, int high, int stride) {
  _string result =
    _chpl_malloc((high - low + 2), sizeof(char), "_chpl_string_strided_select temp");
  _string src = x + low - 1;
  _string dst = result;
  while (src - x <= high - 1) {
    *dst++ = *src;
    src += stride;
  }
  *dst = '\0';
  return _glom_strings(1, result);
}

_string
string_select(_string x, int low, int high) {
  return string_strided_select(x, low, high, 1);
}

_string
string_index(_string x, int i) {
  char buffer[2];
  sprintf(buffer, "%c", x[i-1]);
  return _glom_strings(1, buffer);
}


_bool
string_contains(_string x, _string y) {
  if (strstr(x, y))
    return true;
  else
    return false;
}


_bool
string_equal(_string x, _string y) {
  if (!strcmp(x, y)) {
    return true;
  } else {
    return false;
  }
}


_int64
string_length(_string x) {
  return strlen(x);
}


_complex32 
_chpl_complex32( _float32 r, _float32 i) {
  _complex32 ret_c = {r, i};
  return ret_c;
}

_complex64 
_chpl_complex64( _float64 r, _float64 i) {
  _complex64 ret_c = {r, i};
  return ret_c;
}

_complex128 
_chpl_complex128( _float128 r, _float128 i) {
  _complex128 ret_c = {r, i};
  return ret_c;
}
