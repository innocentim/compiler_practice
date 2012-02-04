#include <cstdio>
#include <cstdlib>

#include "common.hpp"
#include "token.hpp"
#include "parser.hpp"

std::map<std::string, Type> type_map;
Operator * operator_map[tok_invalid + 1] = {NULL};
std::vector<token_type> tokens;

Top * top;

void op_register(Token tok, const std::string & str, unsigned int left_unary, unsigned int right_unary, unsigned int eye, bool left_associative){
	operator_map[tok] = new Operator(tok, str, left_unary, right_unary, eye, left_associative);
};

void init(){
	op_register(tok_punc_plus, "+", 10, 0, 2, true);
	op_register(tok_punc_minus, "-", 10, 0, 2, true);
	op_register(tok_punc_star, "*", 10, 10, 3, true);
	op_register(tok_punc_slash, "/", 0, 0, 3, true);
	op_register(tok_punc_equ, "=", 0, 0, 1, false);
	type_map.insert(std::pair<std::string, Type>("int", type_int));
	type_map.insert(std::pair<std::string, Type>("string", type_str));
	type_map.insert(std::pair<std::string, Type>("void", type_void));
};

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
	top->emit_target();
	return 0;
};
