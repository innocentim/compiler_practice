#include <cstdio>
#include <cstdlib>

#include "common.hpp"
#include "token.hpp"
#include "parser.hpp"
#include "checker.hpp"

void init(){
	init_punc();
	init_operator();
};

Top * top;

int main(int args, char *argv[]){
	FILE * fin;
	if (args < 2 || !(fin = fopen(argv[1], "r"))){
		exit(1);
	}
	init();
	get_tokens(fin);

//	for (unsigned int i = 0; i < tokens.size(); i++){
//		printf("%d -> %d -> %s\n", i, tokens[i].tok, tokens[i].str.c_str());
//	}
//	printf("\n");

	top = new Top();

//	top->emit_source();
	
	top->check(global);

	top->emit_target();

	return 0;
};

