#ifndef _RUNTIME_H_
#define _RUNTIME_H_

#include "expr.h"
#include "stmt.h"
#include "symbol.h"
#include "type.h"

extern ModuleSymbol* prelude;
extern ModuleSymbol* baseModule;
extern ModuleSymbol* fileModule;
extern ModuleSymbol* tupleModule;
extern ModuleSymbol* seqModule;
extern ModuleSymbol* standardModule;
extern ModuleSymbol* compilerModule;

extern TypeSymbol* chpl_htuple;
extern TypeSymbol* chpl_seq;

extern VarSymbol* chpl_stdin;
extern VarSymbol* chpl_stdout;
extern VarSymbol* chpl_stderr;

extern VarSymbol* chpl_input_filename;
extern VarSymbol* chpl_input_lineno;

extern VarSymbol* setterToken;
extern VarSymbol* methodToken;

extern VarSymbol* chpl_true;
extern VarSymbol* chpl_false;

#endif
