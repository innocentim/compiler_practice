#ifndef __AST_H__
#define __AST_H__

#include "list.hpp"
#include <string>
#include <llvm/Value.h>

class VarDef;
class Stmt;
class Definition;
class Expr;
class CGContext;

typedef list<VarDef*> VarList;
typedef list<Stmt*> StmtList;
typedef list<Definition*> DefList;
typedef list<Expr*> ExprList;
typedef std::string Identifier;

class Stmt {
public:
    virtual llvm::Value * codeGen(CGContext*) = 0;
    virtual void emitSource() const = 0;
};

class Expr{
public:
    Identifier type;
    virtual llvm::Value * codeGen(CGContext*) = 0;
    virtual void emitSource() const = 0;
};

class Stmts {
public:
    list<Stmt*> stmts;
    Stmts() {};
    virtual llvm::Value * codeGen(CGContext*);
    virtual void emitSource() const ;
};

class FactorVar : public Expr {
public:
    Identifier str;
    FactorVar(Identifier & str) : str(str) {};
    virtual llvm::Value * codeGen(CGContext*);
    virtual void emitSource() const ;
};

class FactorNum : public Expr {
public:
    long long value;
    FactorNum(long long value) : value(value) {};
    virtual llvm::Value * codeGen(CGContext*);
    virtual void emitSource() const ;
};

class FactorStr : public Expr {
public:
    std::string & str;
    FactorStr(std::string & str) : str(str) {};
    virtual llvm::Value * codeGen(CGContext*);
    virtual void emitSource() const ;
};

class FactorCall : public Expr {
public:
    const Identifier & name;
    ExprList & args;
    FactorCall(const Identifier & name, ExprList & args) : name(name), args(args) {};
    virtual llvm::Value * codeGen(CGContext*);
    virtual void emitSource() const ;
};

class BinaryOp : public Expr {
public:
    int op;
    Expr & left;
    Expr & right;
    BinaryOp(Expr & left, int op, Expr & right) : op(op), left(left), right(right){};
    virtual llvm::Value * codeGen(CGContext*);
    virtual void emitSource() const ;
};

class Assignment : public Expr {
public:
    const Identifier & lvalue;
    Expr & rvalue;
    Assignment(const Identifier & lvalue, Expr & rvalue) : lvalue(lvalue), rvalue(rvalue) {};
    virtual llvm::Value * codeGen(CGContext*);
    virtual void emitSource() const ;
};

class Definition : public Stmt {
public:
    const Identifier & type;
    const Identifier & name;
    Definition(const Identifier & type, const Identifier & name) : type(type), name(name) {};
    virtual llvm::Value * codeGen(CGContext*) = 0;
    virtual void emitSource() const = 0;
};

class VarDef : public Definition {
public:
    Expr * assign;
    VarDef(const Identifier & type, const Identifier & name) : Definition(type, name), assign(NULL) {};
    VarDef(const Identifier & type, const Identifier & name, Expr * assign) : Definition(type, name), assign(assign) {};
    virtual llvm::Value * codeGen(CGContext*);
    virtual void emitSource() const ;
};

class FuncDef : public Definition {
public:
    VarList & args;
    Stmts & body;
    FuncDef(const Identifier & type, const Identifier &name, VarList & args, Stmts & body) : Definition(type, name), args(args), body(body) {};
    virtual llvm::Value * codeGen(CGContext*);
    virtual void emitSource() const ;
};

class ExprStmt : public Stmt {
public:
    Expr & expr;
    ExprStmt(Expr & expr) : expr(expr) {};
    virtual llvm::Value * codeGen(CGContext*);
    virtual void emitSource() const ;
};

class Return : public Stmt {
public:
    Expr * ret;
    Return(Expr * ret) : ret(ret) {};
    virtual llvm::Value * codeGen(CGContext*);
    virtual void emitSource() const ;
};

#endif
