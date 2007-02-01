#include <stdio.h>
#include "astutil.h"
#include "beautify.h"
#include "driver.h"
#include "expr.h"
#include "files.h"
#include "mysystem.h"
#include "stmt.h"
#include "stringutil.h"
#include "symbol.h"
#include "symscope.h"

static int max(int a, int b) {
  return (a >= b) ? a : b;
}

static void setOrder(Map<ClassType*,int>& order, int& maxOrder, ClassType* ct);

static int
getOrder(Map<ClassType*,int>& order, int& maxOrder,
         ClassType* ct, Vec<ClassType*>& visit) {
  if (visit.set_in(ct))
    return 0;
  visit.set_add(ct);
  if (order.get(ct))
    return order.get(ct);
  int i = 0;
  for_fields(field, ct) {
    if (ClassType* fct = dynamic_cast<ClassType*>(field->type)) {
      if (!visit.set_in(fct) && fct->classTag != CLASS_CLASS)
        setOrder(order, maxOrder, fct);
      i = max(i, getOrder(order, maxOrder, fct, visit));
    }
  }
  return i + (ct->classTag != CLASS_CLASS);
}


static void
setOrder(Map<ClassType*,int>& order, int& maxOrder,
         ClassType* ct) {
  if (ct->classTag == CLASS_CLASS || order.get(ct))
    return;
  int i;
  Vec<ClassType*> visit;
  i = getOrder(order, maxOrder, ct, visit);
  order.put(ct, i);
  if (i > maxOrder)
    maxOrder = i;
}


#define STRSUB(x)                               \
  *ch = '\0';                                   \
  sym->cname = stringcat(sym->cname, x, ch+1);  \
  ch = sym->cname

static void legalizeCName(Symbol* sym) {
  for (char* ch = sym->cname; *ch != '\0'; ch++) {
    switch (*ch) {
    case '>': STRSUB("_GREATER_"); break;
    case '<': STRSUB("_LESS_"); break;
    case '=': STRSUB("_EQUAL_"); break;
    case '*': STRSUB("_ASTERISK_"); break;
    case '/': STRSUB("_SLASH_"); break;
    case '%': STRSUB("_PERCENT_"); break;
    case '+': STRSUB("_PLUS_"); break;
    case '-': STRSUB("_HYPHEN_"); break;
    case '^': STRSUB("_CARET_"); break;
    case '&': STRSUB("_AMPERSAND_"); break;
    case '|': STRSUB("_BAR_"); break;
    case '!': STRSUB("_EXCLAMATION_"); break;
    case '#': STRSUB("_POUND_"); break;
    case '?': STRSUB("_QUESTION_"); break;
    case '~': STRSUB("_TILDA_"); break;
    default: break;
    }
  }
}

static void codegen_header(void) {
  ChainHashMap<char*, StringHashFns, int> cnames;
  Vec<TypeSymbol*> typeSymbols;
  Vec<FnSymbol*> fnSymbols;
  Vec<VarSymbol*> varSymbols;

  chpl_main->addPragma("rename _chpl_main");

  cnames.put("abs", 1);
  cnames.put("exit", 1);
  cnames.put("_init", 1);
  cnames.put("stdin", 1);
  cnames.put("close", 1);
  cnames.put("fwrite", 1);
  cnames.put("fread", 1);
  cnames.put("main", 1);
  cnames.put("open", 1);
  cnames.put("printf", 1);
  cnames.put("quad", 1);
  cnames.put("read", 1);
  cnames.put("sleep", 1);
  cnames.put("stderr", 1);
  cnames.put("stdout", 1);
  cnames.put("write", 1);
  cnames.put("y1", 1); // this is ridiculous...
  cnames.put("log2", 1);
  cnames.put("remove", 1);
  cnames.put("fprintf", 1);
  cnames.put("clone", 1);
  cnames.put("new", 1);

  forv_Vec(BaseAST, ast, gAsts) {
    if (DefExpr* def = dynamic_cast<DefExpr*>(ast)) {
      Symbol* sym = def->sym;

      if (sym->name == sym->cname)
        sym->cname = stringcpy(sym->name);

      if (char* pragma = sym->hasPragmaPrefix("rename"))
        sym->cname = stringcpy(pragma+7);

      if (VarSymbol* vs = dynamic_cast<VarSymbol*>(ast))
        if (vs->immediate)
          continue;

      legalizeCName(sym);

      // mangle symbol that is neither field nor formal if the symbol's
      // name has already been encountered
      if (!dynamic_cast<ArgSymbol*>(sym) &&
          !dynamic_cast<UnresolvedSymbol*>(sym) &&
          !dynamic_cast<ClassType*>(sym->parentScope->astParent) &&
          cnames.get(sym->cname))
        sym->cname = stringcat("_", intstring(sym->id), "_", sym->cname);

      cnames.put(sym->cname, 1);
    
      if (TypeSymbol* typeSymbol = dynamic_cast<TypeSymbol*>(sym)) {
        typeSymbols.add(typeSymbol);
      } else if (FnSymbol* fnSymbol = dynamic_cast<FnSymbol*>(sym)) {
        fnSymbols.add(fnSymbol);
      } else if (VarSymbol* varSymbol = dynamic_cast<VarSymbol*>(sym)) {
        if (dynamic_cast<ModuleSymbol*>(varSymbol->parentScope->astParent))
          varSymbols.add(varSymbol);
      }
    }
  }

  fileinfo header;
  openCFile(&header, "_chpl_header", "h");
  FILE* outfile = header.fptr;

  fprintf(outfile, "#include \"stdchpl.h\"\n");
  fprintf(outfile, "\n");

  forv_Vec(TypeSymbol, typeSymbol, typeSymbols) {
    typeSymbol->codegenPrototype(outfile);
  }

  // codegen enumerated types
  forv_Vec(TypeSymbol, typeSymbol, typeSymbols) {
    if (dynamic_cast<EnumType*>(typeSymbol->type))
      typeSymbol->codegenDef(outfile);
  }

  // order records/unions topologically
  //   (int, int) before (int, (int, int))
  Map<ClassType*,int> order;
  int maxOrder = 0;
  forv_Vec(TypeSymbol, ts, typeSymbols) {
    if (ClassType* ct = dynamic_cast<ClassType*>(ts->type))
      setOrder(order, maxOrder, ct);
  }

  // debug
//   for (int i = 0; i < order.n; i++) {
//     if (order.v[i].key && order.v[i].value) {
//       printf("%d: %s\n", order.v[i].value, order.v[i].key->symbol->name);
//     }
//   }
//   printf("%d\n", maxOrder);

  // codegen records/unions in topological order
  Vec<ClassType*> keys;
  order.get_keys(keys);
  for (int i = 1; i <= maxOrder; i++) {
    forv_Vec(ClassType, key, keys) {
      if (order.get(key) == i)
        key->symbol->codegenDef(outfile);
    }
  }

  // codegen remaining types
  forv_Vec(TypeSymbol, typeSymbol, typeSymbols) {
    if (ClassType* ct = dynamic_cast<ClassType*>(typeSymbol->type))
      if (ct->classTag != CLASS_CLASS)
        continue;
    if (dynamic_cast<EnumType*>(typeSymbol->type))
      continue;
    typeSymbol->codegenDef(outfile);
  }

  forv_Vec(FnSymbol, fnSymbol, fnSymbols) {
    fnSymbol->codegenPrototype(outfile);
  }
  forv_Vec(VarSymbol, varSymbol, varSymbols) {
    varSymbol->codegenDef(outfile);
  }

  /** codegen headers for functions generated at codegen time **/
  fprintf(outfile, "void CreateConfigVarTable(void);\n");
  forv_Vec(TypeSymbol, typeSymbol, typeSymbols) {
    if (dynamic_cast<EnumType*>(typeSymbol->type)) {
      fprintf(outfile, "int _convert_string_to_enum");
      typeSymbol->codegen(outfile);
      fprintf(outfile, "(char* inputString, ");
      typeSymbol->codegen(outfile);
      fprintf(outfile, "* val);\n");
      fprintf(outfile, "int setInCommandLine");
      typeSymbol->codegen(outfile);
      fprintf(outfile, "(char* varName, ");
      typeSymbol->codegen(outfile);
      fprintf(outfile, "* value, char* moduleName);\n");
    }
  }

  closeCFile(&header);
}


static void
codegen_config(FILE* outfile) {
  fprintf(outfile, "void CreateConfigVarTable(void) {\n");
  fprintf(outfile, "initConfigVarTable();\n");

  forv_Vec(BaseAST, ast, gAsts) {
    if (DefExpr* def = dynamic_cast<DefExpr*>(ast)) {
      VarSymbol* var = dynamic_cast<VarSymbol*>(def->sym);
      if (var && var->varClass == VAR_CONFIG) {
        fprintf(outfile, "installConfigVar(\"%s\", \"", var->name);
        fprintf(outfile, var->type->symbol->name);
        fprintf(outfile, "\", \"%s\");\n", var->getModule()->name);
      }
    }
  }

  fprintf(outfile, "if (askedToParseArgs()) {\n");
  fprintf(outfile, "  parseConfigArgs();\n");
  fprintf(outfile, "}\n");

  fprintf(outfile, "if (askedToPrintHelpMessage()) {\n");
  fprintf(outfile, "  printHelpTable();\n");
  fprintf(outfile, "  printConfigVarTable();\n");
  fprintf(outfile, "}\n");
  
  fprintf(outfile, "}\n");
}


void codegen(void) {
  if (no_codegen)
    return;

  fileinfo mainfile;
  openCFile(&mainfile, "_main", "c");
  fprintf(mainfile.fptr, "#include \"_chpl_header.h\"\n");

  codegen_makefile(&mainfile);

  codegen_header();

  codegen_config(mainfile.fptr);

  forv_Vec(ModuleSymbol, currentModule, allModules) {
    mysystem(stringcat("# codegen-ing module", currentModule->name),
             "generating comment for --print-commands option");

    fileinfo modulefile;
    openCFile(&modulefile, currentModule->name, "c");
    currentModule->codegenDef(modulefile.fptr);
    closeCFile(&modulefile);
    fprintf(mainfile.fptr, "#include \"%s%s\"\n", currentModule->name, ".c");
    beautify(&modulefile);
  }

  closeCFile(&mainfile);
  beautify(&mainfile);
  makeBinary();
}
