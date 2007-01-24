#ifndef _chpltypes_H_
#define _chpltypes_H_

#define nil 0
#define _nilType void*

#define _chpl_object void*
#define _chpl_value void*

// macros for specifying the correct C constant type
#define INT8( i)   (i)
#define INT16( i)  (i)
#define INT32( i)  (i ## L)
#define INT64( i)  (i ## LL)
#define UINT8( i)  (i ## U)
#define UINT16( i) (i ## U)
#define UINT32( i) (i ## UL)
#define UINT64( i) (i ## ULL)

typedef enum __bool {
  false = 0,
  true = 1
} _bool;

typedef char                _int8;
typedef short int           _int16;
typedef int                 _int32;
typedef long long int       _int64;
typedef unsigned char       _uint8;
typedef unsigned short int  _uint16;
typedef unsigned int        _uint32;
typedef unsigned long long  _uint64;
typedef float               _real32;
typedef double              _real64;
typedef long double         _real128;
typedef float               _imag32;
typedef double              _imag64;
typedef long double         _imag128;
typedef struct __complex64  {_real32 re; _real32 im;} _complex64;
typedef struct __complex128 {_real64 re; _real64 im;} _complex128;
typedef struct __complex256 {_real128 re; _real128 im;} _complex256;
typedef char*               _string;
typedef _int64              _symbol;


// macros for Chapel min/max -> C stdint.h or values.h min/max
#define MIN_INT8            INT8_MIN
#define MIN_INT16           INT16_MIN
#define MIN_INT32           INT32_MIN
#define MIN_INT64           INT64_MIN
#define MAX_INT8            INT8_MAX
#define MAX_INT16           INT16_MAX
#define MAX_INT32           INT32_MAX
#define MAX_INT64           INT64_MAX

#define MIN_UINT8           (0)
#define MIN_UINT16          (0)
#define MIN_UINT32          (0)
#define MIN_UINT64          (0)
#define MAX_UINT8           UINT8_MAX
#define MAX_UINT16          UINT16_MAX
#define MAX_UINT32          UINT32_MAX
#define MAX_UINT64          UINT64_MAX

#define MIN_FLOAT32         (-FLT_MAX)
#define MIN_FLOAT64         (-DBL_MAX)
#define MIN_FLOAT128        (-LDBL_MAX)
#define MAX_FLOAT32         FLT_MAX
#define MAX_FLOAT64         DBL_MAX
#define MAX_FLOAT128        LDBL_MAX

#define ascii(s) ((_int8)(*s))

#define string_copy(rhs) (_glom_strings(1, rhs))

char* _glom_strings(int numstrings, ...);

char* _chpl_tostring_bool(_bool x, char* format);
char* _chpl_tostring_int32(_int32 x, char* format);
char* _chpl_tostring_int64(_int64 x, char* format);
char* _chpl_tostring_uint32(_uint32 x, char* format);
char* _chpl_tostring_uint64(_uint64 x, char* format);
char* _chpl_tostring_real32(_real32 x, char* format);
char* _chpl_tostring_real64(_real64 x, char* format);
char* _chpl_tostring_real128(_real128 x, char* format);

#define _chpl_tostring_imag32 _chpl_tostring_real32
#define _chpl_tostring_imag64 _chpl_tostring_real64
#define _chpl_tostring_imag128 _chpl_tostring_real128

char* _chpl_tostring_complex64(_complex64 x, char* format);
char* _chpl_tostring_complex128(_complex128 x, char* format);
char* _chpl_tostring_complex256(_complex256 x, char* format);

_bool string_contains(_string x, _string y);
_string string_concat(_string x, _string y);
_string string_index(_string x, int i);
_string string_select(_string x, int low, int high);
_string string_strided_select(_string x, int low, int high, int stride);
_bool string_equal(_string x, _string y);
_int64 string_length(_string x);

_int64 real2int( _real64 f);       // return the raw bytes of the float
_int64 object2int( _chpl_object o);  // return the ptr


// Construction and assignment of complex numbers
_complex64  _chpl_complex64( _real32 r, _real32 i);
_complex128 _chpl_complex128( _real64 r, _real64 i);
_complex256 _chpl_complex256( _real128 r, _real128 i);
#define     complex_get_real(c)          &c.re
#define     complex_get_imag(c)          &c.im
#define     complex_set_real(c, r)        c.re=r
#define     complex_set_imag(c, i)        c.im=i

#endif
