#ifndef _PASSLIST_H_
#define _PASSLIST_H_

/* This is the default list of passes that will be run by the
   compiler.  The passes shown will be run in the order shown.

   This file may be saved and fed to the compiler using the
   --passlist argument to specify a different set of passes
   dynamically.
*/

PassInfo passlist[] = {
  FIRST,
  RUN(parse),                   // parse files and create AST
  RUN(check_parsed),            // checks semantics of parsed AST
  RUN(cleanup),                 // post parsing transformations
  RUN(scopeResolve),            // resolve symbols by scope
  RUN(normalize),               // normalization transformations
  RUN(check_normalized),        // check semantics of normalized AST
  RUN(build_default_functions), // build default functions

  RUN(functionResolution), // --no-infer function resolution

  RUN(pre_instantiate),    // pre-instantiation for analysis

  RUN(runInterpreter), // INTERPRETER
                       //   Enabled by -i or --interpreter.
                       //   Terminates after running if enabled.

  RUN(runAnalysis),      // analysis
  RUN(resolve_analyzed), // resolve functions/types with analysis information

  RUN(resolveSymbols), // move to resolve_analyzed

  RUN(removeNamedParameters),     // move to resolve_analyzed/functionResolution
  RUN(removeTypeVariableActuals), // move to resolve_analyzed/functionResolution
  RUN(removeTypeVariableFormals), // move to resolve_analyzed/functionResolution

  RUN(check_resolved), // checks semantics of resolved AST

  RUN(createNestedFuncIterators),
  RUN(removeNestedFunctions),

  RUN(inlineFunctions), // function inlining
  RUN(copy_propagation), // not implemented yet
  RUN(codegen),
  LAST
};

#endif
