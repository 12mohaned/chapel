/*
 * Copyright 2004-2017 Cray Inc.
 * Other additional copyright holders may be indicated within.
 *
 * The entirety of this work is licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License.
 *
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/************************************* | **************************************
*                                                                             *
* A ResolveScope supports the first phase in mapping a name to a lexically    *
* scoped symbol. A scope is a set of bindings where a binding is a mapping    *
* from a name to a single symbol.  The name must be unique within a given     *
* scope.                                                                      *
*                                                                             *
*                                                                             *
* May 8, 2017:                                                                *
*   Until recently this state was implemented using a std::map and a handful  *
* of file static function.  This first implementation simply wraps that code. *
*                                                                             *
* This representation is able to detect disallowed reuse of a name within a   *
* scope but does not fully support function overloading; a name can only be   *
* mapped to a single AST node.                                                *
*                                                                             *
* Scopes are created for 4 kinds of AST nodes                                 *
*    1) A  BlockStmt  with tag BLOCK_NORMAL                                   *
*    2) An FnSymbol   defines a scope for its formals and query variables     *
*    3) A  TypeSymbol for an enum type                                        *
*    4) A  TypeSymbol for an aggregate type                                   *
*                                                                             *
************************************** | *************************************/

#include "ResolveScope.h"

#include "scopeResolve.h"
#include "stmt.h"
#include "symbol.h"
#include "type.h"

static std::map<BaseAST*, ResolveScope*> sScopeMap;

ResolveScope* ResolveScope::getRootModule() {
  ResolveScope* retval = new ResolveScope(theProgram, NULL);

  retval->addBuiltIns();

  return retval;
}

ResolveScope* ResolveScope::findOrCreateScopeFor(DefExpr* def) {
  BaseAST*      ast    = getScope(def);
  ResolveScope* retval = NULL;

  if (ast == rootModule->block) {
    ast = theProgram->block;
  }

  retval = getScopeFor(ast);

  if (retval == NULL) {
    if (BlockStmt* blockStmt = toBlockStmt(ast)) {
      retval = new ResolveScope(blockStmt, NULL);

    } else if (FnSymbol*  fnSymbol = toFnSymbol(ast)) {
      retval = new ResolveScope(fnSymbol, NULL);

    } else if (TypeSymbol* typeSymbol = toTypeSymbol(ast)) {
      retval = new ResolveScope(typeSymbol, NULL);

    } else {
      INT_ASSERT(false);
    }

    sScopeMap[ast] = retval;
  }

  return retval;
}

ResolveScope* ResolveScope::getScopeFor(BaseAST* ast) {
  std::map<BaseAST*, ResolveScope*>::iterator it;
  ResolveScope*                               retval = NULL;

  it = sScopeMap.find(ast);

  if (it != sScopeMap.end()) {
    retval = it->second;
  }

  return retval;
}

void ResolveScope::destroyAstMap() {
  std::map<BaseAST*, ResolveScope*>::iterator it;

  for (it = sScopeMap.begin(); it != sScopeMap.end(); it++) {
    delete it->second;
  }

  sScopeMap.clear();
}

/************************************* | **************************************
*                                                                             *
*                                                                             *
*                                                                             *
************************************** | *************************************/

ResolveScope::ResolveScope(ModuleSymbol*       modSymbol,
                           const ResolveScope* parent) {
  mAstRef = modSymbol;
  mParent = parent;

  INT_ASSERT(getScopeFor(modSymbol->block) == NULL);

  sScopeMap[modSymbol->block] = this;
}

ResolveScope::ResolveScope(FnSymbol*           fnSymbol,
                           const ResolveScope* parent) {
  mAstRef = fnSymbol;
  mParent = parent;

  INT_ASSERT(getScopeFor(fnSymbol) == NULL);

  sScopeMap[fnSymbol] = this;
}

ResolveScope::ResolveScope(TypeSymbol*         typeSymbol,
                           const ResolveScope* parent) {
  Type* type = typeSymbol->type;

  INT_ASSERT(isEnumType(type) || isAggregateType(type));

  mAstRef = typeSymbol;
  mParent = parent;

  INT_ASSERT(getScopeFor(typeSymbol) == NULL);

  sScopeMap[typeSymbol] = this;
}

ResolveScope::ResolveScope(BlockStmt*          blockStmt,
                           const ResolveScope* parent) {
  mAstRef = blockStmt;
  mParent = parent;

  INT_ASSERT(getScopeFor(blockStmt) == NULL);

  sScopeMap[blockStmt] = this;
}

/************************************* | **************************************
*                                                                             *
* Historically, definitions have been mapped to scopes by                     *
*   1) Walking gDefExprs                                                      *
*   2) Determining the "scope" for a given DefExpr by walking upwards         *
*      through the AST.                                                       *
*   3) Validating that the definition is valid.                               *
*   4) Extending the scope with the definition                                *
*                                                                             *
* As a special case the built-in symbols, which are defined in rootModule,    *
* are mapped as if they were defined in chpl_Program.                         *
*                                                                             *
* 2017/05/23:                                                                 *
*   Begin to modify this process so that scopes and definitions are managed   *
* using a traditional top-down traversal of the AST starting at chpl_Program. *
*                                                                             *
* This process will overlook the compiler defined built-ins.  This function   *
* is responsible for pre-allocating the scope for chpl_Program and then       *
* inserting the built-ins.
*                                                                             *
************************************** | *************************************/

void ResolveScope::addBuiltIns() {
  extend(dtVoid->symbol);
  extend(dtStringC->symbol);

  extend(gFalse);
  extend(gTrue);

  extend(gTryToken);

  extend(dtNil->symbol);
  extend(gNil);

  extend(gNoInit);

  extend(dtUnknown->symbol);
  extend(dtValue->symbol);

  extend(gUnknown);
  extend(gVoid);

  extend(dtBools[BOOL_SIZE_SYS]->symbol);
  extend(dtBools[BOOL_SIZE_1]->symbol);
  extend(dtBools[BOOL_SIZE_8]->symbol);
  extend(dtBools[BOOL_SIZE_16]->symbol);
  extend(dtBools[BOOL_SIZE_32]->symbol);
  extend(dtBools[BOOL_SIZE_64]->symbol);

  extend(dtInt[INT_SIZE_8]->symbol);
  extend(dtInt[INT_SIZE_16]->symbol);
  extend(dtInt[INT_SIZE_32]->symbol);
  extend(dtInt[INT_SIZE_64]->symbol);

  extend(dtUInt[INT_SIZE_8]->symbol);
  extend(dtUInt[INT_SIZE_16]->symbol);
  extend(dtUInt[INT_SIZE_32]->symbol);
  extend(dtUInt[INT_SIZE_64]->symbol);

  extend(dtReal[FLOAT_SIZE_32]->symbol);
  extend(dtReal[FLOAT_SIZE_64]->symbol);

  extend(dtImag[FLOAT_SIZE_32]->symbol);
  extend(dtImag[FLOAT_SIZE_64]->symbol);

  extend(dtComplex[COMPLEX_SIZE_64]->symbol);
  extend(dtComplex[COMPLEX_SIZE_128]->symbol);

  extend(dtStringCopy->symbol);
  extend(gStringCopy);

  extend(dtCVoidPtr->symbol);
  extend(dtCFnPtr->symbol);
  extend(gCVoidPtr);
  extend(dtSymbol->symbol);

  extend(dtFile->symbol);
  extend(gFile);

  extend(dtOpaque->symbol);
  extend(gOpaque);

  extend(dtTaskID->symbol);
  extend(gTaskID);

  extend(dtSyncVarAuxFields->symbol);
  extend(gSyncVarAuxFields);

  extend(dtSingleVarAuxFields->symbol);
  extend(gSingleVarAuxFields);

  extend(dtAny->symbol);
  extend(dtIntegral->symbol);
  extend(dtAnyComplex->symbol);
  extend(dtNumeric->symbol);

  extend(dtIteratorRecord->symbol);
  extend(dtIteratorClass->symbol);

  extend(dtMethodToken->symbol);
  extend(gMethodToken);

  extend(dtTypeDefaultToken->symbol);
  extend(gTypeDefaultToken);

  extend(dtModuleToken->symbol);
  extend(gModuleToken);

  extend(dtAnyEnumerated->symbol);

  extend(gBoundsChecking);
  extend(gCastChecking);
  extend(gDivZeroChecking);
  extend(gPrivatization);
  extend(gLocal);
  extend(gNodeID);
}

/************************************* | **************************************
*                                                                             *
*                                                                             *
*                                                                             *
************************************** | *************************************/

std::string ResolveScope::name() const {
  std::string retval = "";

  if        (ModuleSymbol* modSym  = toModuleSymbol(mAstRef)) {
    retval = modSym->name;

  } else if (FnSymbol*     fnSym   = toFnSymbol(mAstRef))     {
    retval = fnSym->name;

  } else if (TypeSymbol*   typeSym = toTypeSymbol(mAstRef))   {
    retval = typeSym->name;

  } else if (BlockStmt*    block   = toBlockStmt(mAstRef))    {
    char buff[1024];

    sprintf(buff, "BlockStmt %9d", block->id);

    retval = buff;

  } else {
    INT_ASSERT(false);
  }

  return retval;
}

int ResolveScope::depth() const {
  const ResolveScope* ptr    = mParent;
  int                 retval =       0;

  while (ptr != NULL) {
    retval = retval + 1;
    ptr    = ptr->mParent;
  }

  return retval;
}

int ResolveScope::numBindings() const {
  Bindings::const_iterator it;
  int                      retval = 0;

  for (it = mBindings.begin(); it != mBindings.end(); it++) {
    retval = retval + 1;
  }

  return retval;
}

/************************************* | **************************************
*                                                                             *
*                                                                             *
*                                                                             *
************************************** | *************************************/

bool ResolveScope::extend(Symbol* newSym) {
  const char* name   = newSym->name;
  bool        retval = false;

  if (Symbol* oldSym = lookup(name)) {
    FnSymbol* oldFn = toFnSymbol(oldSym);
    FnSymbol* newFn = toFnSymbol(newSym);

    // Do not complain if they are both functions
    if (oldFn != NULL && newFn != NULL) {
      retval = true;

    // e.g. record String and proc String(...)
    } else if (isAggregateTypeAndConstructor(oldSym, newSym) == true ||
               isAggregateTypeAndConstructor(newSym, oldSym) == true) {
      retval = true;

    // Methods currently leak their scope and can conflict with variables
    } else if (isSymbolAndMethod(oldSym, newSym)             == true ||
               isSymbolAndMethod(newSym, oldSym)             == true) {
      retval = true;

    } else {
      USR_FATAL(oldSym,
                "'%s' has multiple definitions, redefined at:\n  %s",
                name,
                newSym->stringLoc());
    }

    // If oldSym is a constructor and newSym is a type, update with the type
    if (newFn == NULL || newFn->_this == NULL) {
      mBindings[name] = newSym;
    }

  } else {
    mBindings[name] = newSym;
    retval          = true;
  }

  return retval;
}

bool ResolveScope::isAggregateTypeAndConstructor(Symbol* sym0, Symbol* sym1) {
  TypeSymbol* typeSym = toTypeSymbol(sym0);
  FnSymbol*   funcSym = toFnSymbol(sym1);
  bool        retval  = false;

  if (typeSym != NULL && funcSym != NULL && funcSym->_this != NULL) {
    AggregateType* type0 = toAggregateType(typeSym->type);
    AggregateType* type1 = toAggregateType(funcSym->_this->type);

    retval = (type0 == type1) ? true : false;
  }

  return retval;
}

bool ResolveScope::isSymbolAndMethod(Symbol* sym0, Symbol* sym1) {
  FnSymbol* fun0   = toFnSymbol(sym0);
  FnSymbol* fun1   = toFnSymbol(sym1);
  bool      retval = false;

  if (fun0 == NULL && fun1 != NULL && fun1->_this != NULL) {
    retval = true;
  }

  return retval;
}

/************************************* | **************************************
*                                                                             *
* Return the module or enum imported by a use call.                           *
* The module could be nested: e.g. "use outermost.middle.innermost;"          *
*                                                                             *
************************************** | *************************************/

Symbol* ResolveScope::getUsedSymbol(Expr* expr) const {
  Symbol* retval = NULL;

  // 1) The common case of                'use <name>;'
  // 2) Also invoked when recursing for   'use <name>.<name>;'
  if (UnresolvedSymExpr* sym = toUnresolvedSymExpr(expr)) {
    //
    // This case handles the (common) case that we're 'use'ing a
    // symbol that we have not yet resolved.
    //
    if (Symbol* symbol = ::lookup(sym->unresolved, expr)) {
      retval = symbol;

    } else {
      USR_FATAL(expr, "Cannot find module or enum '%s'", sym->unresolved);
    }

  // This handles the case of 'use <symbol>.<symbol>'
  } else if (CallExpr* call = toCallExpr(expr)) {
    if (ModuleSymbol* lhs = toModuleSymbol(getUsedSymbol(call->get(1)))) {
      if (SymExpr* rhs = toSymExpr(call->get(2))) {
        const char* rhsName = NULL;

        if (get_string(rhs, &rhsName) == true) {
          if (Symbol* symbol = ::lookup(rhsName, lhs->block)) {
            retval = symbol;

          } else {
            USR_FATAL(expr, "Cannot find module '%s'", rhsName);
          }

        } else {
          INT_FATAL(expr, "Bad qualified name");
        }

      } else {
        INT_FATAL(expr, "Bad qualified name");
      }

    } else {
      USR_FATAL(expr, "Cannot find module");
    }

  } else {
    INT_FATAL(expr, "Bad qualified name");
  }

  return retval;
}

/************************************* | **************************************
*                                                                             *
*                                                                             *
*                                                                             *
************************************** | *************************************/

Symbol* ResolveScope::lookup(const char* name) const {
  std::map<const char*, Symbol*>::const_iterator it     = mBindings.find(name);
  Symbol*                                        retval = NULL;

  if (it != mBindings.end()) {
    retval = it->second;
  }

  return retval;
}

void ResolveScope::describe() const {
  Bindings::const_iterator it;
  const char*              blockParent = "";
  int                      index       = 0;

  if (BlockStmt* block = toBlockStmt(mAstRef)) {
    blockParent = block->parentSymbol->name;
  }

  printf("#<ResolveScope %s %s\n", name().c_str(), blockParent);
  printf("  Depth:       %19d\n", depth());
  printf("  NumBindings: %19d\n", numBindings());

  for (it = mBindings.begin(); it != mBindings.end(); it++, index++) {
    printf("    %3d: %s\n", index, it->first);
  }

  printf(">\n\n");
}
