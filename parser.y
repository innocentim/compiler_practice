%{
	#include "AST.hpp"
	#include <cstdio>

	extern int yylex();
	extern Stmts * top;
	void yyerror(const char * s) {
		fprintf(stderr, "%s\n", s);
	};
%}

%union {
	Identifier * ident;
	Expr * expr;
	Definition * def;
	Stmt * stmt;
	VarList * varlist;
	ExprList * exprlist;
	Stmts * stmts;
	VarDef * vardef;
	std::string * str;
	int token;
}

%token <str> IDENTIFIER NUM STR
%token <token> IF WHILE ELSE RETURN
%token <token> PLUS MINUS STAR SLASH EQU
%token <token> LBRACE RBRACE LPAREN RPAREN COMMA SLASHSLASH
/*EQUEQU MOD EXCLAIM LESS LESSEQU GREATER GREATEREQU EXCLAIMEQU */

%type <ident> ident
%type <expr> expr
%type <varlist> func_def_args
%type <exprlist> call_args
%type <stmts> stmts top defs
%type <stmt> stmt return
%type <def> def func_def
%type <vardef> var_def

%right EQU
%left PLUS MINUS
%left STAR SLASH

%start top

%%

top : defs { top = $1; }
	;

defs : def { $$ = new Stmts(); $$->stmts.push_back($1); }
	 | defs def { $1->stmts.push_back($2); }

def : var_def { $$ = $1; }
	| func_def { $$ = $1; }

ident : IDENTIFIER { $$ = new Identifier(*$1); }

var_def : ident ident { $$ = new VarDef(*$1, *$2); }
		| ident ident EQU expr { $$ = new VarDef(*$1, *$2, $4); }
		;

func_def : ident ident LPAREN func_def_args RPAREN LBRACE stmts RBRACE
		   { $$ = new FuncDef(*$1, *$2, *$4, *$7); }
		 ;

func_def_args : { $$ = new VarList(); }
			  | var_def { $$ = new VarList(); $$->push_back($1); }
			  | func_def_args COMMA var_def { $1->push_back($3); }
			  ;

stmts : { $$ = new Stmts(); }
	  | stmt { $$ = new Stmts(); $$->stmts.push_back($1); }
	  | stmts stmt { $1->stmts.push_back($2); }
	  | defs { $$ = $1; }
	  ;

stmt : expr { $$ = new ExprStmt(*$1); }
	 | return { $$ = $1; }
	 ;

return : RETURN expr { $$ = new Return($2); }

expr : ident EQU expr { $$ = new Assignment(*$1, *$3); }
	 | ident LPAREN call_args RPAREN { $$ = new FactorCall(*$1, *$3); }
	 | ident { $$ = new FactorVar(*$1); }
	 | NUM { $$ = new FactorNum(atol($1->c_str())); } 
	 | STR { $$ = new FactorStr(*$1); }
	 | expr PLUS expr { $$ = new BinaryOp(*$1, $2, *$3); }
	 | expr MINUS expr { $$ = new BinaryOp(*$1, $2, *$3); }
	 | expr STAR expr { $$ = new BinaryOp(*$1, $2, *$3); }
	 | expr SLASH expr { $$ = new BinaryOp(*$1, $2, *$3); }
	 | LPAREN expr RPAREN { $$ = $2; }
	 ;

call_args : { $$ = new ExprList(); }
		  | expr { $$ = new ExprList(); $$->push_back($1); }
		  | call_args COMMA expr { $1->push_back($3); }
		  ;

