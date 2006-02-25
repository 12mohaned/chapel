#include "driver.h"
#include "files.h"
#include "filesToAST.h"
#include "parser.h"
#include "stringutil.h"
#include "symbol.h"
#include "symtab.h"
#include "countTokens.h"
#include "yy.h"
#include "../traversals/fixup.h"
#include "runtime.h"

void parse(void) {
  // parse prelude
  Symboltable::parsePrelude();
  char* chplroot = sysdirToChplRoot(system_dir);
  prelude = ParseFile(stringcat(chplroot, "/modules/standard/prelude.chpl"),
                      MOD_INTERNAL);

  // parse user files
  Symboltable::doneParsingPreludes();

  yydebug = debugParserLevel;
  compilerModule = ParseFile(stringcat(chplroot, "/modules/standard/_chpl_compiler.chpl"),
                             MOD_STANDARD);

  baseModule = ParseFile(stringcat(chplroot, "/modules/standard/_chpl_base.chpl"), MOD_STANDARD);
  closureModule = ParseFile(stringcat(chplroot, "/modules/standard/_chpl_closure.chpl"), MOD_STANDARD);

  if (!fnostdincs && !fnostdincs_but_file) {
    ParseFile(stringcat(chplroot, "/modules/standard/_chpl_complex.chpl"),
              MOD_STANDARD);
  }
  if (!fnostdincs) {
    fileModule = ParseFile(stringcat(chplroot, 
                                     "/modules/standard/_chpl_file.chpl"),
                           MOD_STANDARD);
  }
  if (!fnostdincs && !fnostdincs_but_file) {
    tupleModule = ParseFile(stringcat(chplroot,
                                      "/modules/standard/_chpl_htuple.chpl"),
                            MOD_STANDARD);
    ParseFile(stringcat(chplroot, "/modules/standard/_chpl_adomain.chpl"),
              MOD_STANDARD);
    ParseFile(stringcat(chplroot, "/modules/standard/_chpl_data.chpl"),
              MOD_STANDARD);
    seqModule = ParseFile(stringcat(chplroot, "/modules/standard/_chpl_seq.chpl"),
                          MOD_STANDARD);
    standardModule = ParseFile(stringcat(chplroot, "/modules/standard/_chpl_standard.chpl"),
                               MOD_STANDARD);
  }

  int filenum = 0;
  char* inputFilename = NULL;

  while ((inputFilename = nthFilename(filenum++))) {
    ParseFile(inputFilename, MOD_USER);
  }
  finishCountingTokens();

  if (userModules.n == 0)
    ParseFile(stringcat(chplroot, "/modules/standard/i.chpl"), MOD_USER);

  Symboltable::doneParsingUserFiles();

  Pass* fixup = new Fixup();
  fixup->run(Symboltable::getModules(MODULES_ALL));

  if (!fnostdincs && !fnostdincs_but_file) {
    chpl_htuple = dynamic_cast<TypeSymbol*>(Symboltable::lookupInScope("_htuple", tupleModule->modScope));
    chpl_seq = dynamic_cast<TypeSymbol*>(Symboltable::lookupInScope("seq", seqModule->modScope));
  }

  if (!fnostdincs) {
    chpl_stdin = dynamic_cast<VarSymbol*>(Symboltable::lookupInScope("stdin", fileModule->modScope));
    chpl_stdout = dynamic_cast<VarSymbol*>(Symboltable::lookupInScope("stdout", fileModule->modScope));
    chpl_stderr = dynamic_cast<VarSymbol*>(Symboltable::lookupInScope("stderr", fileModule->modScope));
  }

  chpl_input_filename = dynamic_cast<VarSymbol*>(Symboltable::lookupInScope("chpl_input_filename", prelude->modScope));
  chpl_input_lineno = dynamic_cast<VarSymbol*>(Symboltable::lookupInScope("chpl_input_lineno", prelude->modScope));

  setterToken = dynamic_cast<VarSymbol*>(Symboltable::lookupInScope("_setterToken", baseModule->modScope));
  methodToken = dynamic_cast<VarSymbol*>(Symboltable::lookupInScope("_methodToken", baseModule->modScope));

}
