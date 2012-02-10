#include "AST.hpp"
#include "parser.hpp"
#include <cstdio>
#include <cstdlib>

extern void init_punc();
extern void yyparse();
StmtList * top;

int main(int args, char *argv[]){
	if (args < 2 || !(freopen(argv[1], "r", stdin))){
		exit(1);
	}
	init_punc();
	yyparse();
//	top->emit_source();
//	top->emit_target()->dump();
	return 0;
};
