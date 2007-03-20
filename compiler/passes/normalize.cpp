/*** normalize
 ***
 *** This pass and function normalizes parsed and scope-resolved AST.
 ***/

#include "astutil.h"
#include "build.h"
#include "expr.h"
#include "passes.h"
#include "runtime.h"
#include "stmt.h"
#include "stringutil.h"
#include "symbol.h"
#include "symscope.h"

bool normalized = false;

static void change_method_into_constructor(FnSymbol* fn);
static void build_lvalue_function(FnSymbol* fn);
static void normalize_returns(FnSymbol* fn);
static void call_constructor_for_class(CallExpr* call);
static void decompose_special_calls(CallExpr* call);
static void hack_resolve_types(Expr* expr);
static void apply_getters_setters(FnSymbol* fn);
static void insert_call_temps(CallExpr* call);
static void fix_user_assign(CallExpr* call);
static void fix_def_expr(VarSymbol* var);
static void tag_global(FnSymbol* fn);
static void fixup_array_formals(FnSymbol* fn);
static void clone_parameterized_primitive_methods(FnSymbol* fn);
static void fixup_query_formals(FnSymbol* fn);


void normalize(void) {
  forv_Vec(ModuleSymbol, mod, allModules) {
    normalize(mod);
  }
  normalized = true;
  forv_Vec(ModuleSymbol, mod, allModules) {
    for_alist(Expr, expr, mod->initFn->body->body) {
      if (DefExpr* def = dynamic_cast<DefExpr*>(expr))
        if ((dynamic_cast<VarSymbol*>(def->sym) && !def->sym->isCompilerTemp) ||
            dynamic_cast<TypeSymbol*>(def->sym) ||
            dynamic_cast<FnSymbol*>(def->sym))
          mod->block->insertAtTail(def->remove());
    }
  }
}

void normalize(BaseAST* base) {
  Vec<BaseAST*> asts;

  asts.clear();
  collect_asts( &asts, base);
  forv_Vec(BaseAST, ast, asts) {
    if (FnSymbol* fn = dynamic_cast<FnSymbol*>(ast)) {
      currentLineno = fn->lineno;
      currentFilename = fn->filename;
      fixup_array_formals(fn);
      clone_parameterized_primitive_methods(fn);
      fixup_query_formals(fn);
      change_method_into_constructor(fn);
    }
  }

  asts.clear();
  collect_asts_postorder(&asts, base);
  forv_Vec(BaseAST, ast, asts) {
    if (FnSymbol* fn = dynamic_cast<FnSymbol*>(ast)) {
      currentLineno = fn->lineno;
      currentFilename = fn->filename;
      fixup_array_formals(fn);
      fixup_query_formals(fn);
      if (fn->buildSetter)
        build_lvalue_function(fn);
    }
  }

  asts.clear();
  collect_asts(&asts, base);
  forv_Vec(BaseAST, ast, asts) {
    if (FnSymbol* fn = dynamic_cast<FnSymbol*>(ast)) {
      normalize_returns(fn);
    }
  }

  asts.clear();
  collect_asts_postorder(&asts, base);
  forv_Vec(BaseAST, ast, asts) {
    currentLineno = ast->lineno;
    currentFilename = ast->filename;
    if (CallExpr* a = dynamic_cast<CallExpr*>(ast)) {
      call_constructor_for_class(a);
      decompose_special_calls(a);
    }
  }

  asts.clear();
  collect_asts_postorder(&asts, base);
  forv_Vec(BaseAST, ast, asts) {
    currentLineno = ast->lineno;
    currentFilename = ast->filename;
    if (FnSymbol* a = dynamic_cast<FnSymbol*>(ast))
      if (!a->defSetGet)
        apply_getters_setters(a);
  }

  asts.clear();
  collect_asts_postorder(&asts, base);
  forv_Vec(BaseAST, ast, asts) {
    currentLineno = ast->lineno;
    currentFilename = ast->filename;
    if (DefExpr* a = dynamic_cast<DefExpr*>(ast)) {
      if (VarSymbol* var = dynamic_cast<VarSymbol*>(a->sym))
        if (dynamic_cast<FnSymbol*>(a->parentSymbol))
          fix_def_expr(var);
    }
  }

  asts.clear();
  collect_asts_postorder(&asts, base);
  forv_Vec(BaseAST, ast, asts) {
    currentLineno = ast->lineno;
    currentFilename = ast->filename;
    if (CallExpr* a = dynamic_cast<CallExpr*>(ast)) {
      insert_call_temps(a);
      fix_user_assign(a);
    }
  }

  asts.clear();
  collect_asts_postorder(&asts, base);
  forv_Vec(BaseAST, ast, asts) {
    if (FnSymbol *fn = dynamic_cast<FnSymbol*>(ast)) {
      tag_global(fn);
    }
  }

  asts.clear();
  collect_asts_postorder(&asts, base);
  forv_Vec(BaseAST, ast, asts) {
    currentLineno = ast->lineno;
    currentFilename = ast->filename;
    if (Expr* a = dynamic_cast<Expr*>(ast)) {
      hack_resolve_types(a);
    }
  }
}


static void build_lvalue_function(FnSymbol* fn) {
  FnSymbol* new_fn = fn->copy();
  fn->defPoint->insertAfter(new DefExpr(new_fn));
  if (fn->_this)
    fn->_this->type->methods.add(new_fn);
  fn->buildSetter = false;
  new_fn->retType = dtVoid;
  new_fn->cname = stringcat("_setter_", fn->cname);
  ArgSymbol* setterToken = new ArgSymbol(INTENT_BLANK, "_st",
                                         dtSetterToken);
  ArgSymbol* lvalue = new ArgSymbol(INTENT_BLANK, "_lvalue", dtAny);
  Expr* exprType = NULL;
  if (new_fn->retExprType) {
    lvalue->type = dtUnknown;
    exprType = new_fn->retExprType;
    exprType->remove();
  }
  new_fn->insertFormalAtTail(new DefExpr(setterToken));
  new_fn->insertFormalAtTail(new DefExpr(lvalue, NULL, exprType));
  Vec<BaseAST*> asts;
  collect_asts_postorder(&asts, new_fn->body);
  forv_Vec(BaseAST, ast, asts) {
    if (CallExpr* returnStmt = dynamic_cast<CallExpr*>(ast)) {
      if (returnStmt->isPrimitive(PRIMITIVE_RETURN) &&
          returnStmt->parentSymbol == new_fn) {
        Expr* expr = returnStmt->get(1);
        expr->replace(new SymExpr(gVoid));
        returnStmt->insertBefore(new CallExpr("=", expr, lvalue));
      }
    }
  }
}


static void normalize_returns(FnSymbol* fn) {
  Vec<BaseAST*> asts;
  Vec<CallExpr*> rets;
  collect_asts(&asts, fn);
  forv_Vec(BaseAST, ast, asts) {
    if (CallExpr* returnStmt = dynamic_cast<CallExpr*>(ast)) {
      if (returnStmt->isPrimitive(PRIMITIVE_RETURN) ||
          returnStmt->isPrimitive(PRIMITIVE_YIELD))
        if (returnStmt->parentSymbol == fn) // not in a nested function
          rets.add(returnStmt);
    }
  }
  if (rets.n == 0) {
    if (fn->fnClass == FN_ITERATOR)
      USR_FATAL(fn, "iterator does not yield or return a value");
    fn->insertAtTail(new CallExpr(PRIMITIVE_RETURN, gVoid));
    return;
  }
  if (rets.n == 1) {
    CallExpr* ret = rets.v[0];
    if (ret == fn->body->body->last() && dynamic_cast<SymExpr*>(ret->get(1)))
      return;
  }
  SymExpr* retSym = dynamic_cast<SymExpr*>(rets.v[0]->get(1));
  bool returns_void = retSym && retSym->var == gVoid;
  LabelSymbol* label = new LabelSymbol(stringcat("_end_", fn->name));
  fn->insertAtTail(new DefExpr(label));
  VarSymbol* retval = NULL;
  if (returns_void) {
    fn->insertAtTail(new CallExpr(PRIMITIVE_RETURN, gVoid));
  } else {
    retval = new VarSymbol(stringcat("_ret_", fn->name), fn->retType);
    retval->isCompilerTemp = true;
    retval->canReference = true;
    if (fn->isParam)
      retval->consClass = VAR_PARAM;
    fn->insertAtHead(new DefExpr(retval));
    fn->insertAtTail(new CallExpr(PRIMITIVE_RETURN, retval));
  }
  bool label_is_used = false;
  forv_Vec(CallExpr, ret, rets) {
    if (retval) {
      Expr* ret_expr = ret->get(1);
      ret_expr->remove();
      if (fn->retExprType)
        ret_expr = new CallExpr("_cast", fn->retExprType->copy(), ret_expr);
      ret->insertBefore(new CallExpr(PRIMITIVE_MOVE, retval, ret_expr));
    }
    if (fn->fnClass == FN_ITERATOR) {
      if (!retval)
        INT_FATAL(ret, "unexpected case");
      if (ret->isPrimitive(PRIMITIVE_RETURN)) {
        ret->insertAfter(new GotoStmt(goto_normal, label));
        label_is_used = true;
      }
      ret->replace(new CallExpr(PRIMITIVE_YIELD, retval));
    } else if (ret->next != label->defPoint) {
      ret->replace(new GotoStmt(goto_normal, label));
      label_is_used = true;
    } else {
      ret->remove();
    }
  }
  if (!label_is_used)
    label->defPoint->remove();
}


static void call_constructor_for_class(CallExpr* call) {
  if (SymExpr* baseVar = dynamic_cast<SymExpr*>(call->baseExpr)) {
    if (TypeSymbol* ts = dynamic_cast<TypeSymbol*>(baseVar->var)) {
      if (ClassType* ct = dynamic_cast<ClassType*>(ts->type)) {
        if (ct->defaultConstructor)
          call->baseExpr->replace(new SymExpr(ct->defaultConstructor->name));
        else
          INT_FATAL(call, "class type has no default constructor");
      }
    }
  }
}


static void decompose_special_calls(CallExpr* call) {
  if (call->isResolved())
    return;
  if (!call->argList->isEmpty() > 0) {
    Expr* firstArg = dynamic_cast<Expr*>(call->argList->get(1));
    SymExpr* symArg = dynamic_cast<SymExpr*>(firstArg);
    // don't decompose method calls
    if (symArg && symArg->var == gMethodToken)
      return;
  }
  if (call->isNamed(".")) {
    if (SymExpr* sym = dynamic_cast<SymExpr*>(call->get(2))) {
      if (VarSymbol* var = dynamic_cast<VarSymbol*>(sym->var)) {
        if (var->immediate &&
            var->immediate->const_kind == CONST_KIND_STRING &&
            !strcmp(var->immediate->v_string, "read")) {
          if (CallExpr* parent = dynamic_cast<CallExpr*>(call->parentExpr)) {
            while (parent->argList->length() > 1) {
              Expr* arg = parent->get(1)->remove();
              parent->getStmtExpr()->insertBefore(new CallExpr(call->copy(), arg));
            }
          }
        }
      }
    }
  } else if (call->isNamed("read")) {
    for_actuals(actual, call) {
      actual->remove();
      call->getStmtExpr()->insertBefore(
        new CallExpr(
          new CallExpr(".", chpl_stdin, new_StringSymbol("read")), actual));
    }
    call->getStmtExpr()->remove();
  }
}


static void apply_getters_setters(FnSymbol* fn) {
  // Most generally:
  //   x.f(a) = y --> f(_mt, x)(a, _st, y)
  // which is the same as
  //   call(= call(call(. x "f") a) y) --> call(call(f _mt x) a _st y)
  // Also:
  //   x.f = y --> f(_mt, x, _st, y)
  //   f(a) = y --> f(a, _st, y)
  //   x.f --> f(_mt, x)
  //   x.f(a) --> f(_mt, x)(a)
  // Note:
  //   call(call or )( indicates partial
  Vec<BaseAST*> asts;
  collect_asts_postorder(&asts, fn);
  forv_Vec(BaseAST, ast, asts) {
    if (CallExpr* call = dynamic_cast<CallExpr*>(ast)) {
      currentLineno = call->lineno;
      currentFilename = call->filename;
      if (call->getFunction() != fn) // in a nested function, handle
                                     // later, because it may be a
                                     // getter or a setter
        continue;
      if (call->isNamed(".")) { // handle getter
        if (CallExpr* parent = dynamic_cast<CallExpr*>(call->parentExpr))
          if (parent->isNamed("="))
            if (parent->get(1) == call)
              continue; // handle setter below
        char* method = NULL;
        if (SymExpr* symExpr = dynamic_cast<SymExpr*>(call->get(2)))
          if (VarSymbol* var = dynamic_cast<VarSymbol*>(symExpr->var))
            if (var->immediate->const_kind == CONST_KIND_STRING)
              method = var->immediate->v_string;
        if (!method)
          INT_FATAL(call, "No method name for getter or setter");
        Expr* _this = call->get(1);
        _this->remove();
        CallExpr* getter = new CallExpr(method, gMethodToken, _this);
        getter->methodTag = true;
        call->replace(getter);
        if (CallExpr* parent = dynamic_cast<CallExpr*>(getter->parentExpr))
          if (parent->baseExpr == getter)
            getter->partialTag = true;
      } else if (call->isNamed("=")) {
        if (CallExpr* lhs = dynamic_cast<CallExpr*>(call->get(1))) {
          if (lhs->isNamed(".")) {
            char* method = NULL;
            if (SymExpr* symExpr = dynamic_cast<SymExpr*>(lhs->get(2)))
              if (VarSymbol* var = dynamic_cast<VarSymbol*>(symExpr->var))
                if (var->immediate->const_kind == CONST_KIND_STRING)
                  method = var->immediate->v_string;
            if (!method)
              INT_FATAL(call, "No method name for getter or setter");
            Expr* _this = lhs->get(1);
            _this->remove();
            Expr* rhs = call->get(2);
            rhs->remove();
            CallExpr* setter =
              new CallExpr(method, gMethodToken, _this, gSetterToken, rhs);
            call->replace(setter);
          } else {
            Expr* rhs = call->get(2);
            rhs->remove();
            lhs->remove();
            call->replace(lhs);
            lhs->insertAtTail(gSetterToken);
            lhs->insertAtTail(rhs);
          }
        }
      }
    }
  }
}


static void insert_call_temps(CallExpr* call) {
  if (!call->parentExpr || !call->getStmtExpr())
    return;

  if (call == call->getStmtExpr())
    return;
  
  if (dynamic_cast<DefExpr*>(call->parentExpr))
    return;

  if (call->partialTag)
    return;

  if (call->primitive)
    return;

  if (CallExpr* parentCall = dynamic_cast<CallExpr*>(call->parentExpr)) {
    if (parentCall->isPrimitive(PRIMITIVE_MOVE) ||
        parentCall->isPrimitive(PRIMITIVE_REF))
      return;
    //    if (parentCall->isNamed("_init"))
    //      call = parentCall;
  }

  Expr* stmt = call->getStmtExpr();
  VarSymbol* tmp = new VarSymbol("_tmp", dtUnknown, VAR_NORMAL, VAR_VAR);
  tmp->isCompilerTemp = true;
  tmp->canReference = true;
  tmp->canParam = true;
  tmp->canType = true;
  call->replace(new SymExpr(tmp));
  stmt->insertBefore(new DefExpr(tmp));
  stmt->insertBefore(new CallExpr(PRIMITIVE_MOVE, tmp, call));
}


static void fix_user_assign(CallExpr* call) {
  if (!call->parentExpr ||
      call->getStmtExpr() == call->parentExpr ||
      !call->isNamed("="))
    return;
  CallExpr* move = new CallExpr(PRIMITIVE_MOVE, call->get(1)->copy());
  call->replace(move);
  move->insertAtTail(call);
}

//
// fix_def_expr removes DefExpr::exprType and DefExpr::init from a
//   variable's def expression, normalizing the AST with primitive
//   moves, calls to _copy, _init, and _cast, and assignments.
//
static void
fix_def_expr(VarSymbol* var) {
  Expr* type = var->defPoint->exprType;
  Expr* init = var->defPoint->init;
  Expr* stmt = var->defPoint; // insertion point
  VarSymbol* constTemp = var; // temp for constants

  if (!type && !init)
    return; // already fixed

  //
  // insert temporary for constants to assist constant checking
  //
  if (var->consClass == VAR_CONST) {
    constTemp = new VarSymbol("_constTmp");
    constTemp->isCompilerTemp = true;
    constTemp->canReference = true;
    stmt->insertBefore(new DefExpr(constTemp));
    stmt->insertAfter(new CallExpr(PRIMITIVE_MOVE, var, constTemp));
  }

  //
  // insert code to initialize config variable from the command line
  //
  if (var->varClass == VAR_CONFIG && var->consClass != VAR_PARAM) {
    Expr* noop = new CallExpr(PRIMITIVE_NOOP);
    ModuleSymbol* module = var->getModule();
    stmt->insertAfter(
      new CondStmt(
        new CallExpr("!",
          new CallExpr(primitives_map.get("_config_has_value"),
                       new_StringSymbol(var->name),
                       new_StringSymbol(module->name))),
        noop,
        new CallExpr(PRIMITIVE_MOVE, constTemp,
          new CallExpr("_cast",
            new CallExpr(PRIMITIVE_TYPEOF, constTemp),
            new CallExpr(primitives_map.get("_config_get_value"),
                         new_StringSymbol(var->name),
                         new_StringSymbol(module->name))))));
    stmt = noop; // insert regular definition code in then block
  }
  if (var->varClass == VAR_CONFIG && var->consClass == VAR_PARAM) {
    if (char* value = configParamMap.get(canonicalize_string(var->name))) {
      if (SymExpr* symExpr = dynamic_cast<SymExpr*>(init)) {
        if (VarSymbol* varSymbol = dynamic_cast<VarSymbol*>(symExpr->var)) {
          if (varSymbol->immediate) {
            Immediate* imm;
            if (varSymbol->immediate->const_kind == CONST_KIND_STRING) {
              imm = new Immediate(value);
            } else {
              imm = new Immediate(*varSymbol->immediate);
              convert_string_to_immediate(value, imm);
            }
            init->replace(new SymExpr(new_ImmediateSymbol(imm)));
            init = var->defPoint->init;
          }
        } else if (EnumSymbol* sym = dynamic_cast<EnumSymbol*>(symExpr->var)) {
          if (EnumType* et = dynamic_cast<EnumType*>(sym->type)) {
            for_alist(DefExpr, constant, et->constants) {
              if (!strcmp(constant->sym->name, value)) {
                init->replace(new SymExpr(constant->sym));
                init = var->defPoint->init;
                break;
              }
            }
          }
        }
      }
    }
  }

  if (type) {

    //
    // use cast for parameters to avoid multiple parameter assignments
    //
    if (init && var->consClass == VAR_PARAM) {
      stmt->insertAfter(
        new CallExpr(PRIMITIVE_MOVE, var,
          new CallExpr("_cast", type->remove(), init->remove())));
      return;
    }

    //
    // initialize variable based on specified type and then assign it
    // the initialization expression if it exists
    //
    VarSymbol* typeTemp = new VarSymbol("_typeTmp");
    typeTemp->isCompilerTemp = true;
    stmt->insertBefore(new DefExpr(typeTemp));
    stmt->insertBefore(
      new CallExpr(PRIMITIVE_MOVE, typeTemp,
        new CallExpr("_init", type->remove())));
    if (init) {
      VarSymbol* initTemp = new VarSymbol("_tmp");
      initTemp->isCompilerTemp = true;
      initTemp->canReference = true;
      initTemp->canParam = true;
      stmt->insertBefore(new DefExpr(initTemp));
      stmt->insertBefore(new CallExpr(PRIMITIVE_MOVE, initTemp, init->remove()));
      stmt->insertAfter(new CallExpr(PRIMITIVE_MOVE, constTemp, typeTemp));
      stmt->insertAfter(
        new CallExpr(PRIMITIVE_MOVE, typeTemp,
          new CallExpr("=", typeTemp, initTemp)));
    } else
      stmt->insertAfter(new CallExpr(PRIMITIVE_MOVE, constTemp, typeTemp));

  } else {

    //
    // initialize untyped variable with initialization expression
    //
    stmt->insertAfter(
      new CallExpr(PRIMITIVE_MOVE, constTemp,
        new CallExpr("_copy", init->remove())));

  }
}


static void hack_resolve_types(Expr* expr) {
  if (DefExpr* def = dynamic_cast<DefExpr*>(expr)) {
    if (ArgSymbol* arg = dynamic_cast<ArgSymbol*>(def->sym)) {
      if (arg->type == dtUnknown && def->exprType) {
        Type* type = def->exprType->typeInfo();
        if (type != dtUnknown && type != dtAny) {
          arg->type = type;
          def->exprType->remove();
        }
      }
    }
  }
}


static void tag_global(FnSymbol* fn) {
  if (fn->global)
    return;
  for_formals(formal, fn) {
    if (ClassType* ct = dynamic_cast<ClassType*>(formal->type))
      if (ct->classTag == CLASS_CLASS &&
          !ct->symbol->hasPragma("domain") &&
          !ct->symbol->hasPragma("array"))
        fn->global = true;
    if (SymExpr* sym = dynamic_cast<SymExpr*>(formal->defPoint->exprType))
      if (ClassType* ct = dynamic_cast<ClassType*>(sym->var->type))
        if (ct->classTag == CLASS_CLASS &&
            !ct->symbol->hasPragma("domain") &&
            !ct->symbol->hasPragma("array"))
          fn->global = true;
  }
  if (fn->global) {
    fn->parentScope->removeVisibleFunction(fn);
    rootScope->addVisibleFunction(fn);
    if (dynamic_cast<FnSymbol*>(fn->defPoint->parentSymbol)) {
      ModuleSymbol* mod = fn->getModule();
      Expr* def = fn->defPoint;
      CallExpr* noop = new CallExpr(PRIMITIVE_NOOP);
      def->insertBefore(noop);
      fn->visiblePoint = noop;
      def->remove();
      mod->block->insertAtTail(def);
    }
  }
}


static void fixup_array_formals(FnSymbol* fn) {
  if (fn->normalizedOnce)
    return;
  fn->normalizedOnce = true;
  Vec<BaseAST*> asts;
  collect_top_asts(&asts, fn);
  Vec<BaseAST*> all_asts;
  collect_asts(&all_asts, fn);
  forv_Vec(BaseAST, ast, asts) {
    if (CallExpr* call = dynamic_cast<CallExpr*>(ast)) {
      if (call->isNamed("_build_array_type")) {
        SymExpr* sym = dynamic_cast<SymExpr*>(call->get(1));
        DefExpr* def = dynamic_cast<DefExpr*>(call->get(1));
        DefExpr* parent = dynamic_cast<DefExpr*>(call->parentExpr);
        if (call->argList->length() == 1)
          if (!parent || !dynamic_cast<ArgSymbol*>(parent->sym) ||
              parent->exprType != call)
            USR_FATAL(call, "array without element type can only "
                      "be used as a formal argument type");
        if (def || (sym && sym->var == gNil) || call->argList->length() == 1) {
          if (!parent || !dynamic_cast<ArgSymbol*>(parent->sym)
              || parent->exprType != call)
            USR_FATAL(call, "array with empty or queried domain can "
                      "only be used as a formal argument type");
          parent->exprType->replace(new SymExpr(chpl_array));
          if (!fn->where) {
            fn->where = new BlockStmt(new SymExpr(gTrue));
            insert_help(fn->where, NULL, fn, fn->argScope);
          }
          Expr* expr = fn->where->body->only();
          if (call->argList->length() == 2)
            expr->replace(new CallExpr("&", expr->copy(),
                            new CallExpr("==", call->get(2)->remove(),
                              new CallExpr(".", parent->sym, new_StringSymbol("eltType")))));
          if (def) {
            forv_Vec(BaseAST, ast, all_asts) {
              if (SymExpr* sym = dynamic_cast<SymExpr*>(ast)) {
                if (sym->var == def->sym)
                  sym->replace(new CallExpr(".", parent->sym, new_StringSymbol("_dom")));
              }
            }
          } else if (!sym || sym->var != gNil) {
            VarSymbol* tmp = new VarSymbol(stringcat("_view_", parent->sym->name));
            forv_Vec(BaseAST, ast, all_asts) {
              if (SymExpr* sym = dynamic_cast<SymExpr*>(ast)) {
                if (sym->var == parent->sym)
                  sym->var = tmp;
              }
            }
            fn->insertAtHead(new CondStmt(
              new CallExpr(".", parent->sym, new_StringSymbol("_value")),
              new CallExpr(PRIMITIVE_MOVE, tmp,
                new CallExpr(new CallExpr(".", parent->sym,
                                          new_StringSymbol("view")),
                             call->get(1)->copy()))));
            fn->insertAtHead(new CallExpr(PRIMITIVE_MOVE, tmp, parent->sym));
            fn->insertAtHead(new DefExpr(tmp));
          }
        } else {  //// DUPLICATED CODE ABOVE AND BELOW
          DefExpr* parent = dynamic_cast<DefExpr*>(call->parentExpr);
          if (parent && dynamic_cast<ArgSymbol*>(parent->sym) && parent->exprType == call) {
            call->baseExpr->replace(new SymExpr("_build_array"));
            VarSymbol* tmp = new VarSymbol(stringcat("_view_", parent->sym->name));
            forv_Vec(BaseAST, ast, all_asts) {
              if (SymExpr* sym = dynamic_cast<SymExpr*>(ast)) {
                if (sym->var == parent->sym)
                  sym->var = tmp;
              }
            }
            fn->insertAtHead(new CondStmt(
              new CallExpr(".", parent->sym, new_StringSymbol("_value")),
              new CallExpr(PRIMITIVE_MOVE, tmp,
                new CallExpr(new CallExpr(".", parent->sym,
                                          new_StringSymbol("view")),
                             call->get(1)->copy()))));
            fn->insertAtHead(new CallExpr(PRIMITIVE_MOVE, tmp, parent->sym));
            fn->insertAtHead(new DefExpr(tmp));
          }
        }
      }
    }
  }
}


static void clone_parameterized_primitive_methods(FnSymbol* fn) {
  if (dynamic_cast<ArgSymbol*>(fn->_this)) {
    if (fn->_this->type == dtInt[INT_SIZE_32]) {
      for (int i=INT_SIZE_1; i<INT_SIZE_NUM; i++) {
        if (dtInt[i] && i != INT_SIZE_32) {
          FnSymbol* nfn = fn->copy();
          nfn->_this->type = dtInt[i];
          fn->defPoint->insertBefore(new DefExpr(nfn));
        }
      }
    }
    if (fn->_this->type == dtUInt[INT_SIZE_32]) {
      for (int i=INT_SIZE_1; i<INT_SIZE_NUM; i++) {
        if (dtUInt[i] && i != INT_SIZE_32) {
          FnSymbol* nfn = fn->copy();
          nfn->_this->type = dtUInt[i];
          fn->defPoint->insertBefore(new DefExpr(nfn));
        }
      }
    }
    if (fn->_this->type == dtReal[FLOAT_SIZE_64]) {
      for (int i=FLOAT_SIZE_16; i<FLOAT_SIZE_NUM; i++) {
        if (dtReal[i] && i != FLOAT_SIZE_64) {
          FnSymbol* nfn = fn->copy();
          nfn->_this->type = dtReal[i];
          fn->defPoint->insertBefore(new DefExpr(nfn));
        }
      }
    }
    if (fn->_this->type == dtImag[FLOAT_SIZE_64]) {
      for (int i=FLOAT_SIZE_16; i<FLOAT_SIZE_NUM; i++) {
        if (dtImag[i] && i != FLOAT_SIZE_64) {
          FnSymbol* nfn = fn->copy();
          nfn->_this->type = dtImag[i];
          fn->defPoint->insertBefore(new DefExpr(nfn));
        }
      }
    }
    if (fn->_this->type == dtComplex[COMPLEX_SIZE_128]) {
      for (int i=COMPLEX_SIZE_32; i<COMPLEX_SIZE_NUM; i++) {
        if (dtComplex[i] && i != COMPLEX_SIZE_128) {
          FnSymbol* nfn = fn->copy();
          nfn->_this->type = dtComplex[i];
          fn->defPoint->insertBefore(new DefExpr(nfn));
        }
      }
    }
  }
}


static void
clone_for_parameterized_primitive_formals(FnSymbol* fn,
                                          DefExpr* def,
                                          int width) {
  ASTMap map;
  FnSymbol* newfn = fn->copy(&map);
  DefExpr* newdef = dynamic_cast<DefExpr*>(map.get(def));
  Symbol* newsym = newdef->sym;
  newdef->replace(new SymExpr(new_IntSymbol(width)));
  Vec<BaseAST*> asts;
  map.get_values(asts);
  forv_Vec(BaseAST, ast, asts) {
    if (SymExpr* se = dynamic_cast<SymExpr*>(ast))
      if (se->var == newsym)
        se->var = new_IntSymbol(width);
  }
  fn->defPoint->insertAfter(new DefExpr(newfn));
  fixup_query_formals(newfn);
}

static void
replace_query_uses(ArgSymbol* formal, DefExpr* def, ArgSymbol* arg,
                   Vec<BaseAST*>& asts) {
  if (!arg->isTypeVariable && arg->intent != INTENT_PARAM)
    USR_FATAL(def, "query variable is not type or parameter");
  forv_Vec(BaseAST, ast, asts) {
    if (SymExpr* se = dynamic_cast<SymExpr*>(ast)) {
      if (se->var == def->sym) {
        se->replace(new CallExpr(".", formal, new_StringSymbol(arg->name)));
      }
    }
  }
}

static void
add_to_where_clause(ArgSymbol* formal, Expr* expr, ArgSymbol* arg) {
  if (!arg->isTypeVariable && arg->intent != INTENT_PARAM)
    USR_FATAL(expr, "type actual is not type or parameter");
  FnSymbol* fn = formal->defPoint->getFunction();
  if (!fn->where) {
    fn->where = new BlockStmt(new SymExpr(gTrue));
    insert_help(fn->where, NULL, fn, fn->argScope);
  }
  Expr* where = fn->where->body->only();
  where->replace(new CallExpr("&", where->copy(),
                   new CallExpr("==", expr->copy(),
                     new CallExpr(".", formal, new_StringSymbol(arg->name)))));
}

static void
fixup_query_formals(FnSymbol* fn) {
  for_formals(formal, fn) {
    if (CallExpr* call = dynamic_cast<CallExpr*>(formal->defPoint->exprType)) {
      // clone query primitive types
      if (DefExpr* def = dynamic_cast<DefExpr*>(call->get(1))) {
        if (call->isNamed("int") || call->isNamed("uint")) {
          for( int i=INT_SIZE_1; i<INT_SIZE_NUM; i++)
            if (dtInt[i])
              clone_for_parameterized_primitive_formals(fn, def,
                                                        get_width(dtInt[i]));
          fn->defPoint->remove();
          return;
        } else if (call->isNamed("real") || call->isNamed("imag")) {
          for( int i=FLOAT_SIZE_16; i<FLOAT_SIZE_NUM; i++)
            if (dtReal[i])
              clone_for_parameterized_primitive_formals(fn, def,
                                                        get_width(dtReal[i]));
          fn->defPoint->remove();
          return;
        } else if (call->isNamed("complex")) {
          for( int i=COMPLEX_SIZE_32; i<COMPLEX_SIZE_NUM; i++)
            if (dtComplex[i])
              clone_for_parameterized_primitive_formals(fn, def,
                                                        get_width(dtComplex[i]));
          fn->defPoint->remove();
          return;
        }
      }
      bool queried = false;
      for_actuals(actual, call) {
        if (dynamic_cast<DefExpr*>(actual))
          queried = true;
        if (NamedExpr* named = dynamic_cast<NamedExpr*>(actual))
          if (dynamic_cast<DefExpr*>(named->actual))
            queried = true;
      }
      if (queried) {
        Vec<BaseAST*> asts;
        collect_asts(&asts, fn);
        SymExpr* base = dynamic_cast<SymExpr*>(call->baseExpr);
        if (!base)
          USR_FATAL(base, "illegal queried type expression");
        TypeSymbol* ts = dynamic_cast<TypeSymbol*>(base->var);
        if (!ts)
          USR_FATAL(base, "illegal queried type expression");
        Vec<ArgSymbol*> args;
        for_formals(arg, ts->type->defaultConstructor) {
          args.add(arg);
        }
        for_actuals(actual, call) {
          if (NamedExpr* named = dynamic_cast<NamedExpr*>(actual)) {
            for (int i = 0; i < args.n; i++) {
              if (!strcmp(named->name, args.v[i]->name)) {
                if (DefExpr* def = dynamic_cast<DefExpr*>(named->actual)) {
                  replace_query_uses(formal, def, args.v[i], asts);
                } else {
                  add_to_where_clause(formal, named->actual, args.v[i]);
                }
                args.v[i] = NULL;
                break;
              }
            }
          }
        }
        for_actuals(actual, call) {
          if (!dynamic_cast<NamedExpr*>(actual)) {
            for (int i = 0; i < args.n; i++) {
              if (args.v[i]) {
                if (DefExpr* def = dynamic_cast<DefExpr*>(actual)) {
                  replace_query_uses(formal, def, args.v[i], asts);
                } else {
                  add_to_where_clause(formal, actual, args.v[i]);
                }
                args.v[i] = NULL;
                break;
              }
            }
          }
        }
        formal->defPoint->exprType->remove();
        formal->type = ts->type;
      }
    }
  }
}


static void change_method_into_constructor(FnSymbol* fn) {
  if (fn->formals->length() <= 1)
    return;
  if (fn->getFormal(1)->type != dtMethodToken)
    return;
  if (fn->getFormal(2)->type == dtUnknown)
    INT_FATAL(fn, "this argument has unknown type");
  if (strcmp(fn->getFormal(2)->type->symbol->name, fn->name))
    return;
  ClassType* ct = dynamic_cast<ClassType*>(fn->getFormal(2)->type);
  if (!ct)
    INT_FATAL(fn, "constructor on non-class type");
  fn->name = canonicalize_string(stringcat("_construct_", fn->name));
  fn->_this = new VarSymbol("this", ct);
  fn->insertAtHead(new CallExpr(PRIMITIVE_MOVE, fn->_this, new CallExpr(ct->symbol)));
  fn->insertAtHead(new DefExpr(fn->_this));
  fn->insertAtTail(new CallExpr(PRIMITIVE_RETURN, new SymExpr(fn->_this)));
  ASTMap map;
  map.put(fn->getFormal(2), fn->_this);
  fn->formals->get(2)->remove();
  fn->formals->get(1)->remove();
  update_symbols(fn, &map);
  ct->symbol->defPoint->insertBefore(fn->defPoint->remove());
}
