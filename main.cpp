#include "AST.hpp"
#include "code_gen.hpp"
#include "parser.hpp"
#include <cstdio>
#include <cstdlib>

extern void initPunc();
extern void codeDump();
extern void initCodeGen();
extern int yyparse();
extern int yylex();
extern int yydebug;
Stmts * top;

int main(int args, char *argv[]){
    if (args < 2 || !(freopen(argv[1], "r", stdin))){
        exit(1);
    }
    initPunc();
    initCodeGen();
//    yydebug = 1;
    yyparse();
//    top->emitSource();
    top->codeGen(new CGContext(NULL, NULL));
    codeDump();
    return 0;
};
