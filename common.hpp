#ifndef __COMMON_H__
#define __COMMON_H__

#include <llvm/Module.h>
#include <llvm/Operator.h>
#include <vector>
#include <string>

struct Context;

enum Token{
	tok_eof,
	tok_identifier,
	tok_kw_if,
	tok_kw_while,
	tok_kw_else,
	tok_const_num,
	tok_const_str,
	tok_punc_plus,
	tok_punc_minus,
	tok_punc_star,
	tok_punc_slash,
	tok_punc_equ,
	tok_punc_equequ,
	tok_punc_mod,
	tok_punc_tilde,
	tok_punc_amp,
	tok_punc_ampamp,
	tok_punc_pipe,
	tok_punc_pipepipe,
	tok_punc_caret,
	tok_punc_exclaim,
	tok_punc_less,
	tok_punc_lessequ,
	tok_punc_greater,
	tok_punc_greaterequ,
	tok_punc_exclaimequ,
	tok_punc_lbrace,
	tok_punc_rbrace,
	tok_punc_lparen,
	tok_punc_rparen,
	tok_punc_lsquare,
	tok_punc_rsquare,
	tok_punc_dot,
	tok_punc_comma,
	tok_punc_slashslash,
	tok_punc_slashstar,
	tok_invalid,
};

enum Type{
	type_void,
	type_int,
	type_str,
	type_int_ref,
	type_str_ref,
	type_invalid,
};

struct Statement{
	virtual void emit_source() = 0;
	virtual llvm::Value * emit_target(llvm::BasicBlock*) = 0;
};

struct Expr : public Statement{
	virtual void emit_source() = 0;
	virtual llvm::Value * emit_target(llvm::BasicBlock*) = 0;
	virtual Type get_type() = 0;
};

struct Func_def;
struct Var_def;
struct Top{
	std::vector<Var_def*> vars;
	std::vector<Func_def*> funcs;
	
	Top();
	virtual void emit_source();
	virtual llvm::Module * emit_target();
};

struct Statements{
	std::vector<Var_def*> vars;
	std::vector<Func_def*> funcs;
	std::vector<Statement*> stmts;

	Statements();
	virtual void emit_source();
	virtual void emit_target(llvm::Function*);
};

struct Var_def{
	std::string type_str;
	Type type;
	std::string name;
	llvm::User * llvm_bind;

	Var_def();
	virtual void emit_source();
	virtual void emit_target(llvm::BasicBlock*);
	virtual std::string& get_name(){
		return name;
	};
	virtual Type get_type();
};

struct Func_def{
	Var_def * ret_var;
	std::vector<Var_def*> args;
	Statements * stmts;
	llvm::Function * llvm_bind;

	Func_def();
	virtual void emit_source();
	virtual void emit_target();
	virtual std::string& get_name(){
		return ret_var->name;
	};
	virtual Type get_type();
};

struct Factor_const_num : public Expr{
	unsigned int value;

	Factor_const_num();
	virtual void emit_source();
	virtual llvm::Value* emit_target(llvm::BasicBlock*);
	virtual Type get_type();
};

struct Factor_const_str : public Expr{
	std::string value;

	Factor_const_str();
	virtual void emit_source();
	virtual llvm::Value* emit_target(llvm::BasicBlock*);
	virtual Type get_type();
};

struct Factor_var : public Expr{
	std::string name;
	Var_def * bind;

	Factor_var();
	virtual void emit_source();
	virtual llvm::Value* emit_target(llvm::BasicBlock*);
	virtual Type get_type();
};

struct Factor_call : public Expr{
	std::string name;
	std::vector<std::string> args;
	Func_def * bind;

	Factor_call();
	virtual void emit_source();
	virtual llvm::Value* emit_target(llvm::BasicBlock*);
	virtual Type get_type();
};

struct Binary_op : public Expr{
	Token op;
	Expr * left;
	Expr * right;

	Binary_op() : op(tok_invalid), left(NULL), right(NULL){};
	virtual void emit_source();
	virtual llvm::Value* emit_target(llvm::BasicBlock*);
	virtual Type get_type();
};

struct If_block : public Statement{
	Expr * condition;
	Statements * stmts_true, * stmts_false;

	If_block();
	virtual void emit_source();
	virtual llvm::Value * emit_target(llvm::BasicBlock*);
};

struct While_block : public Statement{
	Expr * condition;
	Statements * stmts;

	While_block();
	virtual void emit_source();
	virtual llvm::Value * emit_target(llvm::BasicBlock*);
};

#endif
