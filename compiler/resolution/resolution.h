#ifndef _RESOLUTION_H_
#define _RESOLUTION_H_

class CallInfo;

extern SymbolMap paramMap;
extern Vec<CallExpr*> callStack;

bool isDispatchParent(Type* t, Type* pt);

bool canCoerce(Type* actualType, Symbol* actualSym, Type* formalType, FnSymbol* fn, bool* promotes = NULL);
bool canDispatch(Type* actualType, Symbol* actualSym, Type* formalType, FnSymbol* fn = NULL, bool* promotes = NULL, bool paramCoerce = false);

const char* toString(Type* type);
const char* toString(CallInfo* info);
const char* toString(FnSymbol* fn);

void parseExplainFlag(char* flag, int* line, ModuleSymbol** module);

FnSymbol* instantiate(FnSymbol* fn, SymbolMap* subs, CallExpr* call);
void resolveFormals(FnSymbol* fn);
void resolveCall(CallExpr* call, bool errorCheck = true);
void resolveBlock(Expr* body);

FnSymbol* defaultWrap(FnSymbol* fn, Vec<ArgSymbol*>* actualFormals, bool isSquare);
FnSymbol* orderWrap(FnSymbol* fn, Vec<ArgSymbol*>* actualFormals, bool isSquare);
FnSymbol* coercionWrap(FnSymbol* fn, CallInfo* info);
FnSymbol* promotionWrap(FnSymbol* fn, CallInfo* info);

#endif
