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
%type <stmts> top defs stmts
%type <stmt> stmt def func_def return
%type <vardef> var_def

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

stmts : stmt { $$ = new Stmts(); $$->stmts.push_back($1); }
	  | stmts stmt { $1->stmts.push_back($2); }
	  ;

stmt : def
	 | expr { $$ = new ExprStmt(*$1); }
	 | return { $$ = $1; }
	 ;

return : RETURN { $$ = new Return(); }

expr : ident EQU expr { $$ = new Assignment(*$1, *$3); }
	 | ident LPAREN call_args RPAREN { $$ = new FactorCall(*$1, *$3); }
	 | NUM { $$ = new FactorNum(atol($1->c_str())); } 
	 | STR { $$ = new FactorStr(*$1); }
	 | ident { $$ = $1; }
	 | expr PLUS expr { $$ = new BinaryOp(*$1, $2, *$3); }
	 | expr MINUS expr { $$ = new BinaryOp(*$1, $2, *$3); }
	 | expr STAR expr { $$ = new BinaryOp(*$1, $2, *$3); }
	 | expr SLASH expr { $$ = new BinaryOp(*$1, $2, *$3); }
	 | LPAREN expr RPAREN { $$ = $2; }
	 ;

call_args : { $$ = new ExprList(); }
		  | expr { $$ = new ExprList(); $$->push_back($1); }
		  | call_args COMMA ident { $1->push_back($3); }
		  ;

