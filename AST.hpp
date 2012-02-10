#ifndef __AST_H__
#define __AST_H__

#include <string>
#include <vector>
#include <llvm/Value.h>

struct VarDef;
struct Stmt;
struct Expr;

typedef std::vector<VarDef*> VarList;
typedef std::vector<Stmt*> StmtList;
typedef std::vector<Expr*> ExprList;

struct Stmt {
	virtual llvm::Value * codeGen() = 0;
	virtual void emitSource() = 0;
};

struct Expr{
	virtual llvm::Value * codeGen() = 0;
	virtual void emitSource() = 0;
};

struct Identifier : public Expr {
	std::string str;
	Identifier(std::string & str) : str(str) {};
	virtual llvm::Value * codeGen();
	virtual void emitSource();
};

struct FactorNum : public Expr {
	long long value;
	FactorNum(long long value) : value(value) {};
	virtual llvm::Value * codeGen();
	virtual void emitSource();
};

struct FactorStr : public Expr {
	std::string & str;
	FactorStr(std::string & str) : str(str) {};
	virtual llvm::Value * codeGen();
	virtual void emitSource();
};

struct FactorCall : public Expr {
	const Identifier & name;
	ExprList & args;
	FactorCall(const Identifier & name, ExprList & args) : name(name), args(args) {};
	virtual llvm::Value * codeGen();
	virtual void emitSource();
};

struct BinaryOp : public Expr {
	int op;
	Expr & left;
	Expr & right;
	BinaryOp(Expr & left, int op, Expr & right) : op(op), left(left), right(right){};
	virtual llvm::Value * codeGen();
	virtual void emitSource();
};

struct Assignment : public Expr {
	Identifier & lvalue;
	Expr & rvalue;
	Assignment(Identifier & lvalue, Expr & rvalue) : lvalue(lvalue), rvalue(rvalue) {};
	virtual llvm::Value * codeGen();
	virtual void emitSource();
};

struct VarDef : public Stmt {
	const Identifier & type;
	const Identifier & name;
	Expr * assign;
	VarDef(const Identifier & type, const Identifier & name) : type(type), name(name) {};
	VarDef(const Identifier & type, const Identifier & name, Expr * assign) : type(type), name(name), assign(assign) {};
	virtual llvm::Value * codeGen();
	virtual void emitSource();
};

struct FuncDef : public Stmt {
	const Identifier & type;
	const Identifier & name;
	VarList & args;
	StmtList & body;
	FuncDef(const Identifier & type, const Identifier &name, VarList & args, StmtList & body) : type(type), name(name), args(args), body(body) {};
	virtual llvm::Value * codeGen();
	virtual void emitSource();
};

struct ExprStmt : public Stmt {
	Expr & expr;
	ExprStmt(Expr & expr) : expr(expr) {};
	virtual llvm::Value * codeGen();
	virtual void emitSource();
};

#endif
