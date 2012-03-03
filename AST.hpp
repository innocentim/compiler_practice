#ifndef __AST_H__
#define __AST_H__

#include "list.hpp"
#include <string>
#include <llvm/Value.h>

struct VarDef;
struct Stmt;
struct Definition;
struct Expr;
struct CGContext;

typedef std::list<VarDef*> VarList;
typedef std::list<Stmt*> StmtList;
typedef std::list<Definition*> DefList;
typedef std::list<Expr*> ExprList;
typedef std::string Identifier;

struct Stmt {
	virtual llvm::Value * codeGen(CGContext*) = 0;
	virtual void emitSource() const = 0;
};

struct Expr{
	Identifier type;
	virtual llvm::Value * codeGen(CGContext*) = 0;
	virtual void emitSource() const = 0;
};

struct Stmts {
	std::list<Stmt*> stmts;
	Stmts() {};
	virtual llvm::Value * codeGen(CGContext*);
	virtual void emitSource() const ;
};

struct FactorVar : public Expr {
	Identifier str;
	FactorVar(Identifier & str) : str(str) {};
	virtual llvm::Value * codeGen(CGContext*);
	virtual void emitSource() const ;
};

struct FactorNum : public Expr {
	long long value;
	FactorNum(long long value) : value(value) {};
	virtual llvm::Value * codeGen(CGContext*);
	virtual void emitSource() const ;
};

struct FactorStr : public Expr {
	std::string & str;
	FactorStr(std::string & str) : str(str) {};
	virtual llvm::Value * codeGen(CGContext*);
	virtual void emitSource() const ;
};

struct FactorCall : public Expr {
	const Identifier & name;
	ExprList & args;
	FactorCall(const Identifier & name, ExprList & args) : name(name), args(args) {};
	virtual llvm::Value * codeGen(CGContext*);
	virtual void emitSource() const ;
};

struct BinaryOp : public Expr {
	int op;
	Expr & left;
	Expr & right;
	BinaryOp(Expr & left, int op, Expr & right) : op(op), left(left), right(right){};
	virtual llvm::Value * codeGen(CGContext*);
	virtual void emitSource() const ;
};

struct Assignment : public Expr {
	const Identifier & lvalue;
	Expr & rvalue;
	Assignment(const Identifier & lvalue, Expr & rvalue) : lvalue(lvalue), rvalue(rvalue) {};
	virtual llvm::Value * codeGen(CGContext*);
	virtual void emitSource() const ;
};

struct Definition : public Stmt {
	const Identifier & type;
	const Identifier & name;
	Definition(const Identifier & type, const Identifier & name) : type(type), name(name) {};
	virtual llvm::Value * codeGen(CGContext*) = 0;
	virtual void emitSource() const = 0;
};

struct VarDef : public Definition {
	Expr * assign;
	VarDef(const Identifier & type, const Identifier & name) : Definition(type, name), assign(NULL) {};
	VarDef(const Identifier & type, const Identifier & name, Expr * assign) : Definition(type, name), assign(assign) {};
	virtual llvm::Value * codeGen(CGContext*);
	virtual void emitSource() const ;
};

struct FuncDef : public Definition {
	VarList & args;
	Stmts & body;
	FuncDef(const Identifier & type, const Identifier &name, VarList & args, Stmts & body) : Definition(type, name), args(args), body(body) {};
	virtual llvm::Value * codeGen(CGContext*);
	virtual void emitSource() const ;
};

struct ExprStmt : public Stmt {
	Expr & expr;
	ExprStmt(Expr & expr) : expr(expr) {};
	virtual llvm::Value * codeGen(CGContext*);
	virtual void emitSource() const ;
};

struct Return : public Stmt {
	Expr * ret;
	Return(Expr * ret) : ret(ret) {};
	virtual llvm::Value * codeGen(CGContext*);
	virtual void emitSource() const ;
};

#endif
