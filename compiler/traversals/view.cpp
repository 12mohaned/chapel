#include <typeinfo>
#include "view.h"
#include "stmt.h"
#include "expr.h"
#include "symbol.h"
#include "type.h"
#include "symtab.h"
#include "stringutil.h"
#include "chpl_log.h"

View::View(bool initNumberSymbols, int initMarkAST) {
  indent = 0;
  numberSymbols = initNumberSymbols;
  markAST = initMarkAST;
}

void View::run(Vec<ModuleSymbol*>* modules) {
  forv_Vec(ModuleSymbol, mod, *modules) {
    printf("\nMODULE: %s\n", mod->name);
    mod->startTraversal(this);
  }
  printf("\n\n");
  fflush(stdout);
}

void View::preProcessStmt(Stmt* stmt) {
  printf("\n");
  for (int i = 0; i < indent; i++) {
    printf(" ");
  }
  printf("(");
  if (numberSymbols) {
    printf("%ld ", stmt->id);
  }
  if (stmt->id == markAST) {
    printf("***");
  }
  printf("%s", astTypeName[stmt->astType]);
  indent += 2;
}

void View::postProcessStmt(Stmt* stmt) {
  printf(")");
  indent -= 2;
}

void View::preProcessExpr(Expr* expr) {
  printf("\n");
  for (int i = 0; i < indent; i++) {
    printf(" ");
  }
  printf("(");
  if (expr->id == markAST) {
    printf("***");
  }
  if (numberSymbols) {
    printf("%ld ", expr->id);
  }
  printf("%s", astTypeName[expr->astType]);

  if (CallExpr* call = dynamic_cast<CallExpr*>(expr)) {
    if (call->isPrimitive())
      printf(" %s", call->primitive->name);
  }

  if (NamedExpr* named = dynamic_cast<NamedExpr*>(expr)) {
    printf(" \"%s\"", named->name);
  }

  long i;
  char *str;
  if (get_int(expr, &i)) {
    printf(" %ld", i);
  } else if (get_string(expr, &str)) {
    printf(" \"%s\"", str);
  }
  indent += 2;
}

void View::postProcessExpr(Expr* expr) {
  printf(")");
  indent -= 2;
}

void View::preProcessSymbol(Symbol* sym) {
  printf(" '");
  if (sym->id == markAST) {
    printf("*** ");
  }
  if (dynamic_cast<FnSymbol*>(sym)) {
    printf("fn ");
  } else if (dynamic_cast<ArgSymbol*>(sym)) {
    printf("arg ");
  } else if (dynamic_cast<TypeSymbol*>(sym)) {
    printf("type ");
  }
  printf("%s", sym->name);
  if (numberSymbols) {
    printf("[%ld]", sym->id);
  }
  if (sym->type && sym->type->symbol) {
    printf(":%s", sym->type->symbol->name);
    if (numberSymbols) {
      printf("[%ld]", sym->type->symbol->id);
    }
  }
  printf("'");
}

void View::postProcessSymbol(Symbol* sym) {

}

void View::preProcessType(Type* type) {

}

void View::postProcessType(Type* type) {

}

void print_view(BaseAST* ast) {
  TRAVERSE(ast, new View(), true);
  printf("\n\n");
  fflush(stdout);
}

void print_view_noline(BaseAST* ast) {
  TRAVERSE(ast, new View(), true);
  fflush(stdout);
}

void nprint_view(BaseAST* ast) {
  TRAVERSE(ast, new View(true), true);
  printf("\n\n");
  fflush(stdout);
}

void nprint_view_noline(BaseAST* ast) {
  TRAVERSE(ast, new View(true), true);
  fflush(stdout);
}

void mark_view(BaseAST* ast, long id) {
  TRAVERSE(ast, new View(false, id), true);
  printf("\n\n");
  fflush(stdout);
}

void view(void) {
  Pass* pass = new View();
  pass->run(Symboltable::getModules(pass->whichModules));
}

void print_view_prototypes(BaseAST* base, char* only_named) {
  Vec<BaseAST*> asts;
  collect_asts(&asts, base);
  forv_Vec(BaseAST, ast, asts) {
    if (FnSymbol* fn = dynamic_cast<FnSymbol*>(ast)) {
      if (only_named && !strcmp(only_named, fn->name)) {
        printf("%s %ld:\n", fn->name, fn->id);
        print_view(fn->formals);
      }
    }
  }
}

void print_view_functions(BaseAST* base, char* only_named) {
  Vec<BaseAST*> asts;
  collect_asts(&asts, base);
  forv_Vec(BaseAST, ast, asts) {
    if (FnSymbol* fn = dynamic_cast<FnSymbol*>(ast)) {
      if (only_named && !strcmp(only_named, fn->name)) {
        print_view(fn);
      }
    }
  }
}
