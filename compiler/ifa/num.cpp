#include <math.h>
#include <string.h>
#include <stdio.h>
#include "num.h"
#include "prim_data.h"

static int 
sprint_float_val(char* str, float128 val) {
  int numchars = sprintf(str, "%Lg", val);
  if (strchr(str, '.') == NULL && strchr(str, 'e') == NULL) {
    strcat(str, ".0");
    return numchars + 2;
  } else {
    return numchars;
  }
}

static int 
sprint_complex_val(char* str, float128 real, float128 imm) {
  int numchars = 0;
  numchars += sprintf(str+numchars, "(");
  numchars += sprint_float_val(str+numchars, real);
  numchars += sprintf(str+numchars, ",");
  numchars += sprint_float_val(str+numchars, imm);
  numchars += sprintf(str+numchars, "i)");
  return numchars;
}

int 
sprint_imm(char *str, char *control_string, Immediate &imm) {
  int res = -1;
  switch (imm.const_kind) {
    case NUM_KIND_NONE:
      break;
    case NUM_KIND_UINT: {
      switch (imm.num_index) {
        case INT_SIZE_1: 
          res = sprintf(str, control_string, imm.v_bool); break;
        case INT_SIZE_8: 
          res = sprintf(str, control_string, imm.v_uint8); break;
        case INT_SIZE_16:
          res = sprintf(str, control_string, imm.v_uint16); break;
        case INT_SIZE_32:
          res = sprintf(str, control_string, imm.v_uint32); break;
        case INT_SIZE_64:
          res = sprintf(str, control_string, imm.v_uint64); break;
        default: assert(!"case");
      }
      break;
    }
    case NUM_KIND_INT: {
      switch (imm.num_index) {
        case INT_SIZE_1: 
          res = sprintf(str, control_string, imm.v_bool); break;
        case INT_SIZE_8: 
          res = sprintf(str, control_string, imm.v_int8); break;
        case INT_SIZE_16:
          res = sprintf(str, control_string, imm.v_int16); break;
        case INT_SIZE_32:
          res = sprintf(str, control_string, imm.v_int32); break;
        case INT_SIZE_64:
          res = sprintf(str, control_string, imm.v_int64); break;
        default: assert(!"case");
      }
      break;
    }
    case NUM_KIND_FLOAT: case NUM_KIND_IMAG:
      switch (imm.num_index) {
        case FLOAT_SIZE_32:
          res = sprintf(str, control_string, imm.v_float32); break;
        case FLOAT_SIZE_64:
          res = sprintf(str, control_string, imm.v_float64); break;
        case FLOAT_SIZE_128:
          res = sprintf(str, control_string, imm.v_float128); break;
        default: assert(!"case");
      }
      break;
    case NUM_KIND_COMPLEX:
      switch (imm.num_index) {
        case COMPLEX_SIZE_64:
          res = sprintf(str, control_string, 
                        imm.v_complex64.r, imm.v_complex64.i);
          break;
        case COMPLEX_SIZE_128:
          res = sprintf(str, control_string, 
                        imm.v_complex128.r, imm.v_complex128.i); 
          break;
        case COMPLEX_SIZE_256: 
          res = sprintf(str, control_string, 
                        imm.v_complex256.r, imm.v_complex256.i); 
          break;
        default: assert(!"case");
      }
      break;
    case CONST_KIND_STRING:
      res = sprintf(str, control_string, imm.v_string); break;
      break;
  }
  return res;
}

int 
sprint_imm(char *str, Immediate &imm) {
  int res = -1;
  switch (imm.const_kind) {
    case NUM_KIND_NONE:
      break;
    case NUM_KIND_UINT: {
      switch (imm.num_index) {
        case INT_SIZE_1: 
          res = sprintf(str, "%u", imm.v_bool); break;
        case INT_SIZE_8: 
          res = sprintf(str, "%u", imm.v_uint8); break;
        case INT_SIZE_16:
          res = sprintf(str, "%u", imm.v_uint16); break;
        case INT_SIZE_32:
          res = sprintf(str, "%u", imm.v_uint32); break;
        case INT_SIZE_64:
          res = sprintf(str, "%llu", imm.v_uint64); break;
        default: assert(!"case");
      }
      break;
    }
    case NUM_KIND_INT: {
      switch (imm.num_index) {
        case INT_SIZE_1: 
          res = sprintf(str, "%d", imm.v_bool); break;
        case INT_SIZE_8: 
          res = sprintf(str, "%d", imm.v_int8); break;
        case INT_SIZE_16:
          res = sprintf(str, "%d", imm.v_int16); break;
        case INT_SIZE_32:
          res = sprintf(str, "%d", imm.v_int32); break;
        case INT_SIZE_64:
          res = sprintf(str, "%lld", imm.v_int64); break;
        default: assert(!"case");
      }
      break;
    }
    case NUM_KIND_FLOAT: case NUM_KIND_IMAG:
      switch (imm.num_index) {
        case FLOAT_SIZE_32:
          res = sprint_float_val(str, imm.v_float32); break;
        case FLOAT_SIZE_64:
          res = sprint_float_val(str, imm.v_float64); break;
        case FLOAT_SIZE_128:
          res = sprint_float_val(str, imm.v_float128); break;
        default: assert(!"case");
      }
      break;
    case NUM_KIND_COMPLEX:
      switch (imm.num_index) {
        case COMPLEX_SIZE_64:
          res = sprint_complex_val(str,imm.v_complex64.r,imm.v_complex64.i); 
          break;
        case COMPLEX_SIZE_128:
          res = sprint_complex_val(str,imm.v_complex128.r,imm.v_complex128.i); 
          break;
        case COMPLEX_SIZE_256: 
          res = sprint_complex_val(str,imm.v_complex256.r,imm.v_complex256.i); 
          break;
        default: assert(!"case");
      }
      break;
    case CONST_KIND_STRING:
      res = sprintf(str, "\"%s\"", imm.v_string); break;
      break;
  }
  return res;
}

int 
fprint_imm(FILE *fp, Immediate &imm) {
  int res = -1;
  switch (imm.const_kind) {
    case NUM_KIND_NONE:
      break;
    case NUM_KIND_UINT: {
      switch (imm.num_index) {
        case INT_SIZE_1: 
          res = fprintf(fp, "%u", imm.v_bool); break;
        case INT_SIZE_8: 
          res = fprintf(fp, "%u", imm.v_uint8); break;
        case INT_SIZE_16:
          res = fprintf(fp, "%u", imm.v_uint16); break;
        case INT_SIZE_32:
          res = fprintf(fp, "%u", imm.v_uint32); break;
        case INT_SIZE_64:
          res = fprintf(fp, "%llu", imm.v_uint64); break;
        default: assert(!"case");
      }
      break;
    }
    case NUM_KIND_INT: {
      switch (imm.num_index) {
        case INT_SIZE_1: 
          res = fprintf(fp, "%d", imm.v_bool); break;
        case INT_SIZE_8: 
          res = fprintf(fp, "%d", imm.v_int8); break;
        case INT_SIZE_16:
          res = fprintf(fp, "%d", imm.v_int16); break;
        case INT_SIZE_32:
          res = fprintf(fp, "%d", imm.v_int32); break;
        case INT_SIZE_64:
          res = fprintf(fp, "%lld", imm.v_int64); break;
        default: assert(!"case");
      }
      break;
    }
    case NUM_KIND_FLOAT: case NUM_KIND_IMAG:
      char str[80];
      switch (imm.num_index) {
        case FLOAT_SIZE_32:  
          res = sprint_float_val(str, imm.v_float32); 
          break;
        case FLOAT_SIZE_64: {
          res = sprint_float_val(str, imm.v_float64); 
          break;
        }
        case FLOAT_SIZE_128: {
          res = sprint_float_val(str, imm.v_float128); 
          break;
        }
        default: assert(!"case");
      }
      fputs(str, fp);
      break;
    case NUM_KIND_COMPLEX:
      switch (imm.num_index) {
        case COMPLEX_SIZE_64: {
          char str[80];
          res = sprint_complex_val(str, imm.v_complex64.r, imm.v_complex64.i); 
          fputs(str, fp);
          break;
        }
        case COMPLEX_SIZE_128: {
          char str[160];
          res = sprint_complex_val(str, 
                                   imm.v_complex128.r, imm.v_complex128.i); 
          fputs(str, fp);
          break;
        }
        case COMPLEX_SIZE_256: {
          char str[320];
          res = sprint_complex_val(str, 
                                   imm.v_complex256.r, imm.v_complex256.i); 
          fputs(str, fp);
          break;
        }
        default: assert(!"case");
      }
      break;
    case CONST_KIND_STRING:
      res = fprintf(fp, "\"%s\"", imm.v_string); break;
      break;
  }
  return res;
}

void
coerce_immediate(Immediate *from, Immediate *to) {
#include "cast_code.cc"
}

#define DO_FOLD(_op) \
      switch (imm->const_kind) { \
        case NUM_KIND_NONE: \
          break; \
        case NUM_KIND_UINT: { \
          switch (imm->num_index) { \
            case INT_SIZE_1:  \
              imm->v_bool = im1.v_bool _op im2.v_bool; break; \
            case INT_SIZE_8:  \
              imm->v_uint8 = im1.v_uint8 _op im2.v_uint8; break; \
            case INT_SIZE_16: \
              imm->v_uint16 = im1.v_uint16 _op im2.v_uint16; break; \
            case INT_SIZE_32: \
              imm->v_uint32 = im1.v_uint32 _op im2.v_uint32; break; \
            case INT_SIZE_64: \
              imm->v_uint64 = im1.uint_value() _op im2.uint_value(); break; \
            default: assert(!"case"); \
          } \
          break; \
        } \
        case NUM_KIND_INT: { \
          switch (imm->num_index) { \
            case INT_SIZE_1:  \
              imm->v_bool = im1.v_bool _op im2.v_bool; break; \
            case INT_SIZE_8:  \
              imm->v_int8 = im1.v_int8 _op im2.v_int8; break; \
            case INT_SIZE_16: \
              imm->v_int16 = im1.v_int16 _op im2.v_int16; break; \
            case INT_SIZE_32: \
              imm->v_int32 = im1.v_int32 _op im2.v_int32; break; \
            case INT_SIZE_64: \
              imm->v_int64 = im1.int_value() _op im2.int_value(); break; \
            default: assert(!"case"); \
          } \
          break; \
        } \
        case NUM_KIND_FLOAT: case NUM_KIND_IMAG: \
          switch (imm->num_index) { \
            case FLOAT_SIZE_32: \
              imm->v_float32 = im1.v_float32 _op im2.v_float32; break; \
            case FLOAT_SIZE_64: \
              imm->v_float64 = im1.v_float64 _op im2.v_float64; break; \
            case FLOAT_SIZE_128: \
              imm->v_float128 = im1.v_float128 _op im2.v_float128; break; \
            default: assert(!"case"); \
          } \
          break; \
      }

#define DO_FOLDF(_op) \
      switch (imm->const_kind) { \
        case NUM_KIND_NONE: \
          break; \
        case NUM_KIND_UINT: { \
          switch (imm->num_index) { \
            case INT_SIZE_1:  \
              imm->v_bool = (bool)_op(im1.v_bool, im2.v_bool); break; \
            case INT_SIZE_8:  \
              imm->v_uint8 = (uint8)_op(im1.v_uint8, im2.v_uint8); break; \
            case INT_SIZE_16: \
              imm->v_uint16 = (uint16)_op(im1.v_uint16, im2.v_uint16); break; \
            case INT_SIZE_32: \
              imm->v_uint32 = (uint32)_op(im1.v_uint32, im2.v_uint32); break; \
            case INT_SIZE_64: \
              imm->v_uint64 = (uint64)_op(im1.uint_value(), im2.uint_value()); break; \
            default: assert(!"case"); \
          } \
          break; \
        } \
        case NUM_KIND_INT: { \
          switch (imm->num_index) { \
            case INT_SIZE_1:  \
              imm->v_bool = (bool)_op(im1.v_bool, im2.v_bool); break; \
            case INT_SIZE_8:  \
              imm->v_int8 = (int8)_op(im1.v_int8, im2.v_int8); break; \
            case INT_SIZE_16: \
              imm->v_int16 = (int16)_op(im1.v_int16, im2.v_int16); break; \
            case INT_SIZE_32: \
              imm->v_int32 = (int32)_op(im1.v_int32, im2.v_int32); break; \
            case INT_SIZE_64: \
              imm->v_int64 = (int64)_op(im1.int_value(), im2.int_value()); break; \
            default: assert(!"case"); \
          } \
          break; \
        } \
        case NUM_KIND_FLOAT: case NUM_KIND_IMAG: \
          switch (imm->num_index) { \
            case FLOAT_SIZE_32: \
              imm->v_float32 = (float32)_op(im1.v_float32, im2.v_float32); break; \
            case FLOAT_SIZE_64: \
              imm->v_float64 = (float64)_op(im1.v_float64, im2.v_float64); break; \
            case FLOAT_SIZE_128: \
              imm->v_float128 = (float128)_op(im1.v_float128, im2.v_float128); break; \
            default: assert(!"case"); \
          } \
          break; \
      }

#define DO_FOLDB(_op) \
      switch (im1.const_kind) { \
        case NUM_KIND_NONE: \
          break; \
        case NUM_KIND_UINT: { \
          switch (im1.num_index) { \
            case INT_SIZE_1:  \
              imm->v_bool = im1.v_bool _op im2.v_bool; break; \
            case INT_SIZE_8:  \
              imm->v_bool = im1.v_uint8 _op im2.v_uint8; break; \
            case INT_SIZE_16: \
              imm->v_bool = im1.v_uint16 _op im2.v_uint16; break; \
            case INT_SIZE_32: \
              imm->v_bool = im1.v_uint32 _op im2.v_uint32; break; \
            case INT_SIZE_64: \
              imm->v_bool = im1.uint_value() _op im2.uint_value(); break; \
            default: assert(!"case"); \
          } \
          break; \
        } \
        case NUM_KIND_INT: { \
          switch (im1.num_index) { \
            case INT_SIZE_1:  \
              imm->v_bool = im1.v_bool _op im2.v_bool; break; \
            case INT_SIZE_8:  \
              imm->v_bool = im1.v_int8 _op im2.v_int8; break; \
            case INT_SIZE_16: \
              imm->v_bool = im1.v_int16 _op im2.v_int16; break; \
            case INT_SIZE_32: \
              imm->v_bool = im1.v_int32 _op im2.v_int32; break; \
            case INT_SIZE_64: \
              imm->v_bool = im1.int_value() _op im2.int_value(); break; \
            default: assert(!"case"); \
          } \
          break; \
        } \
        case NUM_KIND_FLOAT: case NUM_KIND_IMAG: \
          switch (im1.num_index) { \
            case FLOAT_SIZE_32: \
              imm->v_bool = im1.v_float32 _op im2.v_float32; break; \
            case FLOAT_SIZE_64: \
              imm->v_bool = im1.v_float64 _op im2.v_float64; break; \
            case FLOAT_SIZE_128: \
              imm->v_bool = im1.v_float128 _op im2.v_float128; break; \
            default: assert(!"case"); \
          } \
          break; \
      }

#define DO_FOLDI(_op) \
      switch (imm->const_kind) { \
        case NUM_KIND_NONE: \
          break; \
        case NUM_KIND_UINT: { \
          switch (imm->num_index) { \
            case INT_SIZE_1:  \
              imm->v_bool = im1.v_bool _op im2.v_bool; break; \
            case INT_SIZE_8:  \
              imm->v_uint8 = im1.v_uint8 _op im2.v_uint8; break; \
            case INT_SIZE_16: \
              imm->v_uint16 = im1.v_uint16 _op im2.v_uint16; break; \
            case INT_SIZE_32: \
              imm->v_uint32 = im1.v_uint32 _op im2.v_uint32; break; \
            case INT_SIZE_64: \
              imm->v_uint64 = im1.uint_value() _op im2.uint_value(); break; \
            default: assert(!"case"); \
          } \
          break; \
        } \
        case NUM_KIND_INT: { \
          switch (imm->num_index) { \
            case INT_SIZE_1:  \
              imm->v_bool = im1.v_bool _op im2.v_bool; break; \
            case INT_SIZE_8:  \
              imm->v_int8 = im1.v_int8 _op im2.v_int8; break; \
            case INT_SIZE_16: \
              imm->v_int16 = im1.v_int16 _op im2.v_int16; break; \
            case INT_SIZE_32: \
              imm->v_int32 = im1.v_int32 _op im2.v_int32; break; \
            case INT_SIZE_64: \
              imm->v_int64 = im1.int_value() _op im2.int_value(); break; \
            default: assert(!"case"); \
          } \
          break; \
        } \
        case NUM_KIND_FLOAT: case NUM_KIND_IMAG: \
          switch (imm->num_index) { \
            default: assert(!"case"); \
          } \
          break; \
      }

#define DO_FOLD1(_op) \
      switch (imm->const_kind) { \
        case NUM_KIND_NONE: \
          break; \
        case NUM_KIND_UINT: { \
          switch (imm->num_index) { \
            case INT_SIZE_1:  \
              imm->v_bool = _op im1.v_bool; break; \
            case INT_SIZE_8:  \
              imm->v_uint8 = _op im1.v_uint8; break; \
            case INT_SIZE_16: \
              imm->v_uint16 = _op im1.v_uint16; break; \
            case INT_SIZE_32: \
              imm->v_uint32 = _op im1.v_uint32; break; \
            case INT_SIZE_64: \
              imm->v_uint64 = _op im1.v_uint64; break; \
            default: assert(!"case"); \
          } \
          break; \
        } \
        case NUM_KIND_INT: { \
          switch (imm->num_index) { \
            case INT_SIZE_1:  \
              imm->v_bool = _op im1.v_bool; break; \
            case INT_SIZE_8:  \
              imm->v_int8 = _op im1.v_int8; break; \
            case INT_SIZE_16: \
              imm->v_int16 = _op im1.v_int16; break; \
            case INT_SIZE_32: \
              imm->v_int32 = _op im1.v_int32; break; \
            case INT_SIZE_64: \
              imm->v_int64 = _op im1.int_value(); break;        \
            default: assert(!"case"); \
          } \
          break; \
        } \
        case NUM_KIND_FLOAT: case NUM_KIND_IMAG: \
          switch (imm->num_index) { \
            case FLOAT_SIZE_32: \
              imm->v_float32 = _op im1.v_float32; break; \
            case FLOAT_SIZE_64: \
              imm->v_float64 =  _op im1.v_float64; break; \
            case FLOAT_SIZE_128: \
              imm->v_float128 =  _op im1.v_float128; break; \
            default: assert(!"case"); \
          } \
          break; \
      }

#define DO_FOLD1I(_op) \
      switch (imm->const_kind) { \
        case NUM_KIND_NONE: \
          break; \
        case NUM_KIND_UINT: { \
          switch (imm->num_index) { \
            case INT_SIZE_1:  \
              imm->v_bool = _op im1.v_bool; break; \
            case INT_SIZE_8:  \
              imm->v_uint8 = _op im1.v_uint8; break; \
            case INT_SIZE_16: \
              imm->v_uint16 = _op im1.v_uint16; break; \
            case INT_SIZE_32: \
              imm->v_uint32 = _op im1.v_uint32; break; \
            case INT_SIZE_64: \
              imm->v_uint64 = _op im1.v_uint64; break; \
            default: assert(!"case"); \
          } \
          break; \
        } \
        case NUM_KIND_INT: { \
          switch (imm->num_index) { \
            case INT_SIZE_1:  \
              imm->v_bool = _op im1.v_bool; break; \
            case INT_SIZE_8:  \
              imm->v_int8 = _op im1.v_int8; break; \
            case INT_SIZE_16: \
              imm->v_int16 = _op im1.v_int16; break; \
            case INT_SIZE_32: \
              imm->v_int32 = _op im1.v_int32; break; \
            case INT_SIZE_64: \
              imm->v_int64 = _op im1.int_value(); break;        \
            default: assert(!"case"); \
          } \
          break; \
        } \
        case NUM_KIND_FLOAT: case NUM_KIND_IMAG: \
          switch (imm->num_index) { \
            default: assert(!"case"); \
          } \
          break; \
      }

void
fold_result(Immediate *im1, Immediate *im2, Immediate *imm) {
  if (im1->const_kind == im2->const_kind) {
    if (im1->num_index > im2->num_index) {
      imm->const_kind = im1->const_kind;
      imm->num_index = im1->num_index;
      return;
    } else {
      imm->const_kind = im2->const_kind;
      imm->num_index = im2->num_index;
      return;
    }
  }

  // if non-complex and complex -> complex
  if ((im1->const_kind == NUM_KIND_COMPLEX) ||
      (im2->const_kind == NUM_KIND_COMPLEX)) {
    if (im2->const_kind == NUM_KIND_COMPLEX) {   // swap im1 to the complex
      Immediate *t = im1;
      im1 = im2;
      im2 = t;
    }
    // WAW: the following needs some fixing (e.g., 128-bit int/uint?)
    if ((im2->const_kind == NUM_KIND_UINT) ||
        (im2->const_kind == NUM_KIND_INT)) {
      if (float_type_precision[im1->num_index] >= int_type_precision[im2->num_index]) {
        imm->num_index = im1->num_index;
      } else { // else, int/uint has greater width?
        imm->num_index =  (int_type_precision[im2->num_index] <= 32) ? FLOAT_SIZE_32 : FLOAT_SIZE_64;
      }
    } else {  // else, im2 must be float?
      imm->num_index = (im1->num_index >= im2->num_index) ? im1->num_index : im2->num_index;
      }
    im1->const_kind = NUM_KIND_COMPLEX;
    return;
  }

  if (im2->const_kind == NUM_KIND_FLOAT) {
    Immediate *t = im2; im2 = im1; im1 = t;
  }
  if (im1->const_kind == NUM_KIND_FLOAT) {
    if (int_type_precision[im2->const_kind] <= float_type_precision[im1->const_kind]) {
      imm->const_kind = im1->const_kind;
      imm->num_index = im1->num_index;
      return;
    }
    if (int_type_precision[im2->const_kind] <= 32) {
      imm->const_kind = NUM_KIND_FLOAT;
      imm->num_index = FLOAT_SIZE_32;
      return;
    }
    imm->const_kind = NUM_KIND_FLOAT;
    imm->num_index = FLOAT_SIZE_64;
    return;
  }
  // mixed signed and unsigned
  if (im1->num_index >= INT_SIZE_64 || im2->num_index >= INT_SIZE_64) {
    imm->const_kind = NUM_KIND_INT;
    imm->num_index = INT_SIZE_64;
    return;
  } else if (im1->num_index >= INT_SIZE_32 || im2->num_index >= INT_SIZE_32) {
    imm->const_kind = NUM_KIND_INT;
    imm->num_index = INT_SIZE_32;
    return;
  } else if (im1->num_index >= INT_SIZE_16 || im2->num_index >= INT_SIZE_16) {
    imm->const_kind = NUM_KIND_INT;
    imm->num_index = INT_SIZE_16;
    return;
  } else if (im1->num_index >= INT_SIZE_8 || im2->num_index >= INT_SIZE_8) {
    imm->const_kind = NUM_KIND_INT;
    imm->num_index = INT_SIZE_8;
    return;
  }
  imm->const_kind = NUM_KIND_UINT;
  imm->num_index = INT_SIZE_1;
  return;
}

void
fold_constant(int op, Immediate *aim1, Immediate *aim2, Immediate *imm) {
  Immediate im1(*aim1), im2, coerce;
  if (aim2)
    im2 = *aim2;
  switch (op) {
    default: fail("fold constant op not supported"); break;
    case P_prim_mult:
    case P_prim_div:
    case P_prim_mod:
    case P_prim_add:
    case P_prim_subtract:
    case P_prim_and:
    case P_prim_or:
    case P_prim_xor:
    case P_prim_pow:
      fold_result(&im1, &im2, &coerce);
      imm->const_kind = coerce.const_kind;
      imm->num_index = coerce.num_index;
      break;
    case P_prim_lsh:
    case P_prim_rsh:
      imm->const_kind = im1.const_kind;
      imm->num_index = im1.num_index;
      break;
    case P_prim_less:
    case P_prim_lessorequal:
    case P_prim_greater:
    case P_prim_greaterorequal:
    case P_prim_equal:
    case P_prim_notequal:
      fold_result(&im1, &im2, &coerce);
      imm->const_kind = NUM_KIND_UINT;
      imm->num_index = INT_SIZE_1;
      break;
    case P_prim_land:
    case P_prim_lor:
    case P_prim_lnot:
      coerce.const_kind = imm->const_kind = NUM_KIND_UINT;
      coerce.num_index = imm->num_index = INT_SIZE_1;
      break;
    case P_prim_plus:
    case P_prim_minus:
    case P_prim_not:
      imm->const_kind = im1.const_kind;
      imm->num_index = im1.num_index;
      break;
  }
  if (coerce.const_kind) {
    coerce_immediate(&im1, &coerce);
    im1 = coerce;
    if (aim2) {
      coerce_immediate(&im2, &coerce);
      im2 = coerce;
    }
  }
  switch (op) {
    default: fail("fold constant op not supported"); break;
    case P_prim_mult: DO_FOLD(*); break;
    case P_prim_div: DO_FOLD(/); break;
    case P_prim_mod: DO_FOLDI(%); break;
    case P_prim_add: DO_FOLD(+); break;
    case P_prim_subtract: DO_FOLD(-); break;
    case P_prim_lsh: DO_FOLDI(<<); break;
    case P_prim_rsh: DO_FOLDI(>>); break;
    case P_prim_less: DO_FOLDB(<); break;
    case P_prim_lessorequal: DO_FOLDB(<=); break;
    case P_prim_greater: DO_FOLDB(>); break;
    case P_prim_greaterorequal: DO_FOLDB(>=); break;
    case P_prim_equal: DO_FOLDB(==); break;
    case P_prim_notequal: DO_FOLDB(!=); break;
    case P_prim_and: DO_FOLDI(&); break;
    case P_prim_xor: DO_FOLDI(^); break;
    case P_prim_or: DO_FOLDI(|); break;
    case P_prim_land: DO_FOLD(&&); break;
    case P_prim_lor: DO_FOLD(||); break;
    case P_prim_plus: DO_FOLD1(+); break;
    case P_prim_minus: DO_FOLD1(-); break;
    case P_prim_not: DO_FOLD1I(~); break;
    case P_prim_lnot: DO_FOLD1(!); break;
    case P_prim_pow: DO_FOLDF(pow); break;
  }
}

void
convert_string_to_immediate(char *str, Immediate *imm) {
  switch (imm->const_kind) {
    case NUM_KIND_NONE:
      break;
    case NUM_KIND_UINT: {
      switch (imm->num_index) {
        case INT_SIZE_8: 
          if (str[0] != '\'')
            imm->v_uint8 = strtoul(str, 0, 10);
          else {
            if (str[1] != '\\')
              imm->v_uint8 = str[1];
            else
              imm->v_uint8 = str[2];
          }
          break;
        case INT_SIZE_16:
          imm->v_uint16 = strtoul(str, 0, 10); break;
        case INT_SIZE_32:
          imm->v_uint32 = strtoul(str, 0, 10); break;
        case INT_SIZE_64:
          imm->v_uint64 = strtoull(str, 0, 10); break;
        default: assert(!"case");
      }
      break;
    }
    case NUM_KIND_INT: {
      switch (imm->num_index) {
        case INT_SIZE_8: 
          if (str[0] != '\'')
            imm->v_int8 = strtoul(str, 0, 10);
          else {
            if (str[1] != '\\')
              imm->v_int8 = str[1];
            else
              imm->v_int8 = str[2];
          }
          break;
        case INT_SIZE_16:
          imm->v_int16 = strtol(str, 0, 10); break;
        case INT_SIZE_32:
          imm->v_int32 = strtol(str, 0, 10); break;
        case INT_SIZE_64:
          imm->v_int64 = strtoll(str, 0, 10); break;
        default: assert(!"case");
      }
      break;
    }
    case NUM_KIND_FLOAT: case NUM_KIND_IMAG:
      switch (imm->num_index) {
        case FLOAT_SIZE_32:
          imm->v_float32 = atof( str); break;
        case FLOAT_SIZE_64:
          imm->v_float64 = atof( str); break;
        case FLOAT_SIZE_128:
          imm->v_float128 = atof( str); break;
        default: assert(!"case");
      }
      break;
  }
}
