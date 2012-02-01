#ifndef __COMMON_H__
#define __COMMON_H__

#include <string>
#include <vector>
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

enum prim_type{
	type_void,
	type_int,
	type_str,
	type_invalid,
};


struct Statement{
	virtual void emit_source() = 0;
	virtual void emit_target() = 0;
	virtual void context_register(Context*) = 0;
	virtual void check(Context*) = 0;
};

struct Definition : Statement{
	virtual void emit_source() = 0;
	virtual void emit_target() = 0;
	virtual void check(Context*) = 0;
	virtual void context_register(Context*) = 0;
	virtual std::string& get_name() = 0;
	virtual prim_type get_type() = 0;
};

struct Expr : Statement{
	virtual void emit_source() = 0;
	virtual void emit_target() = 0;
	virtual void check(Context*) = 0;
	virtual prim_type _check(Context*) = 0;
	virtual void context_register(Context*);
};

struct Top{
	std::vector<Definition*> defs;
	
	Top();
	virtual void emit_source();
	virtual void emit_target();
	virtual void check(Context*);
};

struct Statements{
	std::vector<Statement*> stmts;

	Statements();
	virtual void emit_source();
	virtual void emit_target();
	virtual void check(Context*);
};

struct Var_def : Definition{
	std::string type;
	std::string name;

	Var_def();
	virtual void emit_source();
	virtual void emit_target();
	virtual void check(Context*);
	virtual void context_register(Context*);
	virtual std::string& get_name();
	virtual prim_type get_type();
};

struct Func_def : Definition{
	Var_def * ret_var;
	std::vector<Var_def*> args;
	Statements * stmts;

	Func_def();
	virtual void emit_source();
	virtual void emit_target();
	virtual void check(Context*);
	virtual void context_register(Context*);
	virtual std::string& get_name();
	virtual prim_type get_type();
};

struct Factor_const_num : Expr{
	unsigned int value;

	Factor_const_num();
	virtual void emit_source();
	virtual void emit_target();
	virtual void check(Context*);
	virtual prim_type _check(Context*);
};

struct Factor_const_str : Expr{
	std::string value;

	Factor_const_str();
	virtual void emit_source();
	virtual void emit_target();
	virtual void check(Context*);
	virtual prim_type _check(Context*);
};

struct Factor_var : Expr{
	std::string name;

	Factor_var();
	virtual void emit_source();
	virtual void emit_target();
	virtual void check(Context*);
	virtual prim_type _check(Context*);
};

struct Factor_call : Expr{
	std::string name;
	std::vector<std::string> args;

	Factor_call();
	virtual void emit_source();
	virtual void emit_target();
	virtual void check(Context*);
	virtual prim_type _check(Context*);
};

struct Binary_op : Expr{
	Token op;
	Expr * left;
	Expr * right;

	Binary_op();
	virtual void emit_source();
	virtual void emit_target();
	virtual void check(Context*);
	virtual prim_type _check(Context*);
};

struct If_block : Statement{
	Expr * condition;
	Statements * stmts_true, * stmts_false;

	If_block();
	virtual void emit_source();
	virtual void emit_target();
	virtual void context_register(Context*);
	virtual void check(Context*);
};

struct While_block : Statement{
	Expr * condition;
	Statements * stmts;

	While_block();
	virtual void emit_source();
	virtual void emit_target();
	virtual void context_register(Context*);
	virtual void check(Context*);
};

#endif
