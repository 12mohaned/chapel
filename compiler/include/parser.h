#ifndef _PARSER_H_
#define _PARSER_H_

#include "symbol.h"

ModuleSymbol* ParseFile(char* filename, modType moduletype);
AList* parse_string(char* string);

#endif
