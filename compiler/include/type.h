#ifndef _TYPE_H_
#define _TYPE_H_


#include <stdio.h>
#include "baseAST.h"
#include "analysis.h"
#include "chplenum.h"

/*
  Things which must be changed if instance variablers are added
  to Types:

  1. add variable to class and constructor
  2. copy variable in copyInner

*/

class Symbol;
class EnumSymbol;
class VarSymbol;
class TypeSymbol;
class ArgSymbol;
class FnSymbol;
class Expr;
class DefExpr;
class CallExpr;
class CondStmt;
class ASymbol;
class SymScope;
class VariableType;


class Type : public BaseAST {
 public:
  Vec<Type*> typeParents;     // type hierarchy
  Vec<Type*> dispatchParents; // dispatch hierarchy
  Type* scalarPromotionType;

  TypeSymbol* symbol;
  Symbol* defaultValue;
  FnSymbol *defaultConstructor;
  ASymbol *asymbol;
  Vec<FnSymbol*> methods;
  Type *metaType;
  bool isGeneric;
  Vec<Symbol*> genericSymbols;
  Type *instantiatedFrom;
  ASTMap substitutions;

  Type(astType_t astType, Symbol* init_defaultVal);
  virtual void verify(); 
  COPY_DEF(Type);
  void addSymbol(TypeSymbol* newSymbol);
  Type *instantiate_generic(ASTMap &substitutions);

  virtual void replaceChild(BaseAST* old_ast, BaseAST* new_ast);
  virtual void traverse(Traversal* traversal, bool atTop = true);
  virtual void traverseDef(Traversal* traversal, bool atTop = true);
  virtual void traverseType(Traversal* traversal);
  virtual void traverseDefType(Traversal* traversal);

  virtual int rank(void);

  virtual void print(FILE* outfile);
  virtual void printDef(FILE* outfile);
  virtual void codegen(FILE* outfile);
  virtual void codegenDef(FILE* outfile);
  virtual void codegenPrototype(FILE* outfile);
  virtual void codegenStringToType(FILE* outfile);
  virtual void codegenConfigVarRoutines(FILE* outfile);
  virtual void codegenDefaultFormat(FILE* outfile, bool isRead);

  virtual bool requiresCParamTmp(intentTag intent);
  virtual bool implementedUsingCVals(void);

  virtual bool hasDefaultWriteFunction(void);
  virtual AList<Stmt>* buildDefaultWriteFunctionBody(ArgSymbol* fileArg, ArgSymbol* arg);
  virtual bool hasDefaultReadFunction(void);
  virtual AList<Stmt>* buildDefaultReadFunctionBody(ArgSymbol* fileArg, ArgSymbol* arg);
};

#define forv_Type(_p, _v) forv_Vec(Type, _p, _v)


class FnType : public Type {
 public:
  FnType(void);
  virtual void verify(); 
  virtual void codegen(FILE* outfile);
  virtual void codegenDef(FILE* outfile);
};


class EnumType : public Type {
 public:
  AList<DefExpr>* constants; // EnumSymbols

  EnumType(AList<DefExpr>* init_constants);
  virtual void verify(); 
  COPY_DEF(EnumType);
  virtual void replaceChild(BaseAST* old_ast, BaseAST* new_ast);

  void traverseDefType(Traversal* traversal);

  void printDef(FILE* outfile);
  void codegenDef(FILE* outfile);
  void codegenStringToType(FILE* outfile);
  void codegenConfigVarRoutines(FILE* outfile);
  void codegenDefaultFormat(FILE* outfile, bool isRead);
  bool implementedUsingCVals(void);

  virtual bool hasDefaultWriteFunction(void);
  virtual AList<Stmt>* buildDefaultWriteFunctionBody(ArgSymbol* fileArg, ArgSymbol* arg);
  virtual bool hasDefaultReadFunction(void);
  virtual AList<Stmt>* buildDefaultReadFunctionBody(ArgSymbol* fileArg, ArgSymbol* arg);
};


class LiteralType : public Type {
 public:
  VarSymbol *literal;
  
  LiteralType(VarSymbol *init_literal);
  virtual void verify();
  COPY_DEF(LiteralType);
  virtual void replaceChild(BaseAST* old_ast, BaseAST* new_ast);
  void traverseDefType(Traversal* traversal);

  void printDef(FILE* outfile);
  void codegenDef(FILE* outfile);
  virtual void codegenDefaultFormat(FILE* outfile, bool isRead);
};


class UserType : public Type {
 public:
  Expr* typeExpr;               
  Type* underlyingType;         
  Expr* defaultExpr;            

  UserType(Type* init_underlyingType, Expr* init_defaultExpr = NULL);
  UserType(Expr* init_typeExpr, Expr* init_defaultExpr = NULL);
  UserType(Expr* init_typeExpr, Type* init_underlyingType, Expr* init_defaultExpr = NULL);
  virtual void verify(); 
  COPY_DEF(UserType);
  virtual void replaceChild(BaseAST* old_ast, BaseAST* new_ast);
  void traverseDefType(Traversal* traversal);

  void printDef(FILE* outfile);
  void codegenDef(FILE* outfile);
  void codegenDefaultFormat(FILE* outfile, bool isRead);
};


enum ClassTag {
  CLASS_CLASS,
  CLASS_RECORD
};

class ClassType : public Type {
 public:
  ClassTag classTag;
  SymScope* structScope;
  AList<Stmt>* declarationList;
  bool isPattern;
  EnumType* fieldSelector;

  AList<Expr>* inherits; // used from parsing, sets dispatchParents

  Vec<Symbol*> fields;
  Vec<TypeSymbol*> types;

  ClassType(ClassTag initClassTag);
  virtual void verify(); 
  COPY_DEF(ClassType);
  void addDeclarations(AList<Stmt>* newDeclarations,
                       Stmt* afterStmt = NULL);

  virtual void replaceChild(BaseAST* old_ast, BaseAST* new_ast);
  void traverseDefType(Traversal* traversal);

  bool isNominalType();

  virtual void codegenDef(FILE* outfile);
  virtual void codegenStructName(FILE* outfile);
  virtual void codegenPrototype(FILE* outfile);

  virtual bool implementedUsingCVals(void);

  virtual bool hasDefaultWriteFunction(void);
  virtual AList<Stmt>* buildDefaultWriteFunctionBody(ArgSymbol* fileArg, ArgSymbol* arg);
  virtual bool hasDefaultReadFunction(void);
  virtual AList<Stmt>* buildDefaultReadFunctionBody(ArgSymbol* fileArg, ArgSymbol* arg);

};


class MetaType : public Type {
 public:
  Type* base;

  void traverseDefType(Traversal* traversal);
  MetaType(Type* init_base);
  virtual void verify(); 
};

class SumType : public Type {
 public:
  Vec<Type*> components;

  SumType();
  virtual void verify(); 
  void addType(Type* additionalType);
  virtual void codegenStructName(FILE* outfile);
  void codegenDef(FILE* outfile);
};

class VariableType : public Type {
 public:
  Type* type;

  VariableType(Type *init_type = NULL);
  virtual void verify(); 
  COPY_DEF(VariableType);
  void traverseDefType(Traversal* traversal);
  void codegen(FILE* outfile);
};

class PrimitiveType : public Type {
 public:
  Type *literalType;

  PrimitiveType(Symbol *init_defaultVal = NULL);
  virtual void verify(); 
  void traverseDefType(Traversal* traversal);
};


#ifndef TYPE_EXTERN
#define TYPE_EXTERN extern
#endif

// null-ish types
TYPE_EXTERN PrimitiveType* dtNil;
TYPE_EXTERN PrimitiveType* dtUnknown;
TYPE_EXTERN PrimitiveType* dtUnspecified;
TYPE_EXTERN PrimitiveType* dtVoid;

// primitive types
TYPE_EXTERN PrimitiveType* dtBool;
TYPE_EXTERN PrimitiveType* dtInt;
TYPE_EXTERN PrimitiveType* dtUInt;
TYPE_EXTERN PrimitiveType* dtFloat;
TYPE_EXTERN PrimitiveType* dtComplex;
TYPE_EXTERN PrimitiveType* dtString;
TYPE_EXTERN PrimitiveType* dtSymbol;

// internal types
TYPE_EXTERN Type* dtClosure;
TYPE_EXTERN Type* dtIndex;
TYPE_EXTERN Type* dtMethodToken;
TYPE_EXTERN Type* dtSetterToken;
TYPE_EXTERN Type* dtFile;
TYPE_EXTERN Type* dtUnused; // unused types for interfacing with ifa
void findInternalTypes(void);

// sum-ish types
TYPE_EXTERN Type* dtAny;
TYPE_EXTERN Type* dtScalar;
TYPE_EXTERN Type* dtNumeric;
TYPE_EXTERN ClassType* dtObject;
TYPE_EXTERN ClassType* dtValue;

void initPrimitiveTypes(void);
Type *find_or_make_sum_type(Vec<Type *> *types);
int is_Scalar_Type(Type *t);
int is_Reference_Type(Type *t);
int is_Value_Type(Type *t);
Type *getMetaType(Type *t);
LiteralType *new_LiteralType(VarSymbol *literal_var);

void complete_closure(ClassType *closureType, Vec<Type *> member_types);

#endif
