#include "AST.hpp"
#include "parser.hpp"
#include <cstdio>
#include <cstdlib>

extern void init_punc();
extern void yyparse();
extern int yylex();
extern int yydebug;
Stmts * top;

int main(int args, char *argv[]){
	if (args < 2 || !(freopen(argv[1], "r", stdin))){
		exit(1);
	}
	stdout = stderr;
	init_punc();
//	int token;
//	while ((token = yylex()) != 0){
//		printf("%d\n", token);
//	}
//	freopen(argv[1], "r", stdin);
	yydebug = 1;
	yyparse();
	top->emitSource();
//	top->emit_target()->dump();
	return 0;
};
