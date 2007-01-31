#ifndef _PRIMITIVE_H_
#define _PRIMITIVE_H_

#include "chpl.h"

enum PrimitiveTag {
  PRIMITIVE_UNKNOWN = 0,    // use for any primitives not in this list
  PRIMITIVE_NOOP,
  PRIMITIVE_MOVE,
  PRIMITIVE_REF,
  PRIMITIVE_UNARY_MINUS,
  PRIMITIVE_UNARY_PLUS,
  PRIMITIVE_UNARY_NOT,
  PRIMITIVE_UNARY_LNOT,
  PRIMITIVE_ADD,
  PRIMITIVE_SUBTRACT,
  PRIMITIVE_MULT,
  PRIMITIVE_DIV,
  PRIMITIVE_MOD,
  PRIMITIVE_LSH,
  PRIMITIVE_RSH,
  PRIMITIVE_EQUAL,
  PRIMITIVE_NOTEQUAL,
  PRIMITIVE_LESSOREQUAL,
  PRIMITIVE_GREATEROREQUAL,
  PRIMITIVE_LESS,
  PRIMITIVE_GREATER,
  PRIMITIVE_AND,
  PRIMITIVE_OR,
  PRIMITIVE_XOR,
  PRIMITIVE_POW,
  PRIMITIVE_MIN,
  PRIMITIVE_MAX,
  PRIMITIVE_PROD_ID,                   // product identity operand
  PRIMITIVE_BAND_ID,                   // bit-wise AND identity operand
  PRIMITIVE_BOR_ID,                    // bit-wise OR identity operand
  PRIMITIVE_BXOR_ID,                   // bit-wise XOR identity operand
  PRIMITIVE_LAND_ID,                   // logical AND identity operand
  PRIMITIVE_LOR_ID,                    // logical OR identity operand
  PRIMITIVE_LXOR_ID,                   // logical XOR identity operand
  PRIMITIVE_GETCID,
  PRIMITIVE_SETCID,
  PRIMITIVE_UNION_GETID,
  PRIMITIVE_UNION_SETID,
  PRIMITIVE_GET_MEMBER,
  PRIMITIVE_GET_MEMBER_VALUE,
  PRIMITIVE_SET_MEMBER,
  PRIMITIVE_GET_MEMBER_REF_TO,         // get the value of the reference
  PRIMITIVE_SET_MEMBER_REF_TO,         // set the reference to a var
  PRIMITIVE_SET_HEAPVAR,               // used to alloc on_heap vars
  PRIMITIVE_REFC_INIT,                 // initialize ref-counted heap var
  PRIMITIVE_REFC_TOUCH,                // touch reference-counted var
  PRIMITIVE_REFC_RELEASE,              // dec ref-counter and maybe free var

  PRIMITIVE_THREAD_INIT,
  PRIMITIVE_THREAD_ID,
  PRIMITIVE_GET_SERIAL,                // get serial state
  PRIMITIVE_SET_SERIAL,                // set serial state to true or false

  PRIMITIVE_SYNC_LOCK,                
  PRIMITIVE_SYNC_UNLOCK,
  PRIMITIVE_SYNC_SIGNAL_FULL,
  PRIMITIVE_SYNC_BROADCAST_FULL,
  PRIMITIVE_SYNC_WAIT_FULL,
  PRIMITIVE_SYNC_SIGNAL_EMPTY,
  PRIMITIVE_SYNC_BROADCAST_EMPTY,
  PRIMITIVE_SYNC_WAIT_EMPTY,
  PRIMITIVE_MUTEX_NEW,
  PRIMITIVE_MUTEX_DESTROY,
  PRIMITIVE_CONDVAR_NEW,
  PRIMITIVE_CONDVAR_DESTROY,

  PRIMITIVE_CHPL_ALLOC,
  PRIMITIVE_CHPL_FREE,                 // only for on_heap vars?
  PRIMITIVE_PTR_EQUAL,
  PRIMITIVE_PTR_NOTEQUAL,
  PRIMITIVE_CAST,
  PRIMITIVE_DYNAMIC_CAST,
  PRIMITIVE_ISSUBTYPE,
  PRIMITIVE_TYPEOF,
  PRIMITIVE_USE,
  PRIMITIVE_TUPLE_EXPAND,
  PRIMITIVE_ARRAY_FREE,
  PRIMITIVE_ARRAY_FREE_ELTS,
  PRIMITIVE_ARRAY_INIT,
  PRIMITIVE_ARRAY_GET,
  PRIMITIVE_ARRAY_GET_VALUE,
  PRIMITIVE_ARRAY_SET,
  PRIMITIVE_ARRAY_SET_FIRST,
  PRIMITIVE_ERROR,
  PRIMITIVE_WHEN,
  PRIMITIVE_LOOP_PARAM,

  PRIMITIVE_CLASS_NULL, // check reference/class is NULL

  PRIMITIVE_GC_INIT,  // set reference count to zero
  PRIMITIVE_GC_TOUCH, // increase reference count
  PRIMITIVE_GC_FREE,  // decrease reference count
  PRIMITIVE_GC_ISPOS, // check reference count is greater than zero
  PRIMITIVE_GC_ISNEG, // check reference count is less than zero

  PRIMITIVE_GET_LINENO,
  PRIMITIVE_GET_FILENAME,

  PRIMITIVE_INT_ERROR,

  NUM_KNOWN_PRIMS
};

class PrimitiveOp { public:
  PrimitiveTag tag;
  char *name;
  Type *(*returnInfo)(CallExpr*);
  bool isReference;

  PrimitiveOp(PrimitiveTag atag, char *aname, Type *(*areturnInfo)(CallExpr*), bool aIsReference);
};

extern HashMap<char *, StringHashFns, PrimitiveOp *> primitives_map;

extern PrimitiveOp* primitives[NUM_KNOWN_PRIMS];

void initPrimitive();

#endif
