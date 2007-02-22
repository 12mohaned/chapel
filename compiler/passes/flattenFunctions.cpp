#include "alist.h"
#include "astutil.h"
#include "expr.h"
#include "passes.h"
#include "stmt.h"
#include "symscope.h"


//
// returns true if the symbol is defined in an outer function to fn
// third argument not used at call site
//
static bool
isOuterVar(Symbol* sym, FnSymbol* fn, SymScope* scope = NULL) {
  if (!scope)
    scope = fn->parentScope;
  if (scope->astParent == scope->astParent->getModule()->block)
    return false;
  else if (sym->parentScope == scope)
    return true;
  else
    return isOuterVar(sym, fn, scope->parent);
}


//
// finds outer vars directly used in a function
//
static void
findOuterVars(FnSymbol* fn, Vec<Symbol*>* uses) {
  Vec<BaseAST*> asts;
  collect_asts(&asts, fn);
  forv_Vec(BaseAST, ast, asts) {
    if (SymExpr* symExpr = dynamic_cast<SymExpr*>(ast)) {
      Symbol* sym = symExpr->var;
      if (dynamic_cast<VarSymbol*>(sym) || dynamic_cast<ArgSymbol*>(sym))
        if (isOuterVar(sym, fn))
          uses->add_exclusive(sym);
    }
  }
}


static void
addVarsToFormals(FnSymbol* fn, Vec<Symbol*>* vars) {
  ASTMap update_map;
  forv_Vec(Symbol, sym, *vars) {
    if (sym) {
      ArgSymbol* arg = new ArgSymbol(INTENT_REF, sym->name, sym->type);
      fn->insertFormalAtTail(new DefExpr(arg));
      update_map.put(sym, arg);
    }
  }
  if (update_map.n)
    update_symbols(fn->body, &update_map);
}


static void
addVarsToActuals(CallExpr* call, Vec<Symbol*>* vars) {
  forv_Vec(Symbol, sym, *vars) {
    if (sym)
      call->insertAtTail(sym);
  }
}


void flattenFunctions(void) {
  compute_call_sites();

  Vec<FnSymbol*> all_functions;
  collect_functions(&all_functions);

  Vec<FnSymbol*> all_nested_functions;
  Map<FnSymbol*,Vec<Symbol*>*> args_map;
  forv_Vec(FnSymbol, fn, all_functions) {
    if (dynamic_cast<FnSymbol*>(fn->defPoint->parentSymbol)) {
      all_nested_functions.add_exclusive(fn);
      Vec<Symbol*>* uses = new Vec<Symbol*>();
      findOuterVars(fn, uses);
      args_map.put(fn, uses);
    }
  }

  // iterate to get outer vars in a function based on outer vars in
  // functions it calls
  bool change;
  do {
    change = false;
    forv_Vec(FnSymbol, fn, all_nested_functions) {
      Vec<BaseAST*> asts;
      collect_top_asts(&asts, fn);
      Vec<Symbol*>* uses = args_map.get(fn);
      forv_Vec(BaseAST, ast, asts) {
        if (CallExpr* call = dynamic_cast<CallExpr*>(ast)) {
          if (call->isResolved()) {
            if (FnSymbol* fcall = call->findFnSymbol()) {
              Vec<Symbol*>* call_uses = args_map.get(fcall);
              if (call_uses) {
                forv_Vec(Symbol, sym, *call_uses) {
                  if (isOuterVar(sym, fn))
                    if (uses->add_exclusive(sym))
                      change = true;
                }
              }
            }
          }
        }
      }
    }
  } while (change);

  // update all call sites of nested functions this must be done
  // before updating the function so that the outer var actuals can be
  // updated with the outer var functions when the formals are updated
  // (in nested functions that call one another)
  forv_Vec(FnSymbol, fn, all_nested_functions) {
    Vec<Symbol*>* uses = args_map.get(fn);
    forv_Vec(CallExpr, call, *fn->calledBy) {
      addVarsToActuals(call, uses);
    }
  }

  // move nested functions to module level
  forv_Vec(FnSymbol, fn, all_nested_functions) {
    ModuleSymbol* mod = fn->getModule();
    Expr* def = fn->defPoint;
    def->remove();
    mod->block->insertAtTail(def);
  }

  // add extra formals to nested functions
  forv_Vec(FnSymbol, fn, all_nested_functions) {
    Vec<Symbol*>* uses = args_map.get(fn);
    addVarsToFormals(fn, uses);
  }
}

