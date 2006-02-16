#ifndef _STMT_H_
#define _STMT_H_

#include <stdio.h>
#include "alist.h"
#include "analysis.h"
#include "baseAST.h"
#include "symbol.h"


extern bool printCppLineno;
extern bool printChplLineno;
extern bool inUserModule;
extern bool justStartedGeneratingFunction;

class Expr;
class DefExpr;
class AAST;

class Stmt : public BaseAST {
 public:
  Stmt* parentStmt;
  AAST *ainfo;

  Stmt(astType_t astType = STMT);
  virtual void verify(void);
  COPY_DEF(Stmt);
  void codegen(FILE* outfile);
  virtual void codegenStmt(FILE* outfile);
  virtual void callReplaceChild(BaseAST* new_ast);
  virtual void traverse(Traversal* traversal, bool atTop = true);
  virtual void traverseDef(Traversal* traversal, bool atTop = true);
  virtual void traverseStmt(Traversal* traversal);
  virtual ASTContext getContext(void);
};
#define forv_Stmt(_p, _v) forv_Vec(Stmt, _p, _v)


class ExprStmt : public Stmt {
 public:
  Expr* expr;

  ExprStmt(Expr* initExpr);
  virtual void verify(void);
  COPY_DEF(ExprStmt);
  virtual void replaceChild(BaseAST* old_ast, BaseAST* new_ast);
  void traverseStmt(Traversal* traversal);

  virtual void print(FILE* outfile);
  virtual bool noCodegen();
  virtual void codegenStmt(FILE* outfile);
};


class ReturnStmt : public ExprStmt {
 public:
  bool yield;
  ReturnStmt(Expr* initExpr = NULL, bool init_yield = false);
  ReturnStmt(Symbol* initExpr, bool init_yield = false);
  ReturnStmt(char* initExpr, bool init_yield = false);
  virtual void verify(void);
  COPY_DEF(ReturnStmt);
  void print(FILE* outfile);
  void codegenStmt(FILE* outfile);
  bool returnsVoid();
};


enum blockStmtType {
  BLOCK_NORMAL = 0,
  BLOCK_WHILE_DO,
  BLOCK_DO_WHILE,
  BLOCK_FOR,
  BLOCK_FORALL,
  BLOCK_ORDERED_FORALL,
  BLOCK_ATOMIC,
  BLOCK_COBEGIN
};


class BlockStmt : public Stmt {
 public:
  blockStmtType blockType;
  AList<Stmt>* body;
  SymScope* blkScope;
  LabelSymbol* pre_loop;
  LabelSymbol* post_loop;

  BlockStmt::BlockStmt(AList<Stmt>* init_body = new AList<Stmt>(), 
                       blockStmtType init_blockType = BLOCK_NORMAL);
  BlockStmt::BlockStmt(Stmt* init_body,
                       blockStmtType init_blockType = BLOCK_NORMAL);
  virtual void verify(void);
  COPY_DEF(BlockStmt);
  virtual void replaceChild(BaseAST* old_ast, BaseAST* new_ast);
  void traverseStmt(Traversal* traversal);
  void print(FILE* outfile);
  void codegenStmt(FILE* outfile);

  void insertAtHead(BaseAST* ast);
  void insertAtTail(BaseAST* ast);

  bool isLoop(void);
};


enum ForLoopStmtTag {
  FORLOOPSTMT_FOR,           // for ... do
  FORLOOPSTMT_ORDEREDFORALL, // ordered forall ... do
  FORLOOPSTMT_FORALL         // forall .. do
};


class ForLoopStmt : public Stmt {
 public:
  ForLoopStmtTag forLoopStmtTag;
  AList<DefExpr>* indices;
  AList<Expr>* iterators;
  BlockStmt* innerStmt;
  SymScope* indexScope;

  ForLoopStmt(ForLoopStmtTag initForLoopStmtTag,
              AList<DefExpr>* initIndices,
              AList<Expr>* initIterators,
              BaseAST* initInnerStmt);
  virtual void verify(void);
  COPY_DEF(ForLoopStmt);
  virtual void replaceChild(BaseAST* old_ast, BaseAST* new_ast);
  void traverseStmt(Traversal* traversal);
  void print(FILE* outfile);
  void codegenStmt(FILE* outfile);
};


class CondStmt : public Stmt {
 public:
  Expr* condExpr;
  BlockStmt* thenStmt;
  BlockStmt* elseStmt;

  CondStmt(Expr* iCondExpr, BaseAST* iThenStmt, BaseAST* iElseStmt = NULL);
  virtual void verify(void);
  COPY_DEF(CondStmt);
  virtual void replaceChild(BaseAST* old_ast, BaseAST* new_ast);
  void traverseStmt(Traversal* traversal);

  void print(FILE* outfile);
  void codegenStmt(FILE* outfile);
};


class WhenStmt : public Stmt {
 public:
  AList<Expr>* caseExprs;
  BlockStmt* doStmt;

  WhenStmt(AList<Expr>* init_caseExprs = NULL, BlockStmt* init_doStmt = NULL);
  WhenStmt(AList<Expr>* init_caseExprs, Stmt* init_doStmt);
  WhenStmt(AList<Expr>* init_caseExprs, AList<Stmt>* init_doStmt);
  virtual void verify(void);
  COPY_DEF(WhenStmt);
  virtual void replaceChild(BaseAST* old_ast, BaseAST* new_ast);
  void traverseStmt(Traversal* traversal);
  void print(FILE* outfile);
  void codegenStmt(FILE* outfile);
};


class SelectStmt : public Stmt {
 public:
  Expr* caseExpr;
  AList<WhenStmt>* whenStmts;

  SelectStmt(Expr* init_caseExpr = NULL, AList<WhenStmt>* init_whenStmts = NULL);
  virtual void verify(void);
  COPY_DEF(SelectStmt);
  virtual void replaceChild(BaseAST* old_ast, BaseAST* new_ast);
  void traverseStmt(Traversal* traversal);
  void print(FILE* outfile);
  void codegenStmt(FILE* outfile);
};


class LabelStmt : public Stmt {
 public:
  DefExpr* defLabel;
  Stmt* stmt;
  
  LabelStmt(DefExpr* iDefLabel);
  LabelStmt(Symbol* iDefLabel);
  LabelStmt(char* iDefLabel);
  virtual void verify(void);
  COPY_DEF(LabelStmt);
  virtual void replaceChild(BaseAST* old_ast, BaseAST* new_ast);
  void traverseStmt(Traversal* traversal);

  void print(FILE* outfile);
  void codegenStmt(FILE* outfile);
  char* labelName(void);
};


enum gotoType {
  goto_normal = 0,
  goto_break,
  goto_continue
};


class GotoStmt : public Stmt {
 public:
  Symbol* label;
  gotoType goto_type;

  GotoStmt(gotoType init_goto_type);
  GotoStmt(gotoType init_goto_type, char* init_label);
  GotoStmt(gotoType init_goto_type, Symbol* init_label);
  virtual void verify(void);
  COPY_DEF(GotoStmt);
  void traverseStmt(Traversal* traversal);
  void print(FILE* outfile);
  void codegenStmt(FILE* outfile);
};


#endif
