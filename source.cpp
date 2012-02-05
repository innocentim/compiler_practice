#include "common.hpp"
#include "misc.hpp"
#include "parser.hpp"
#include <cstdio>

extern token_type * operator_map[];
static unsigned int iden;

void Top::emit_source(){
	iden = 0;
	for (unsigned int i = 0, e = vars.size(); i < e; i++){
		vars[i]->emit_source();
		printf("\n");
	}
	for (unsigned int i = 0, e = funcs.size(); i < e; i++){
		funcs[i]->emit_source();
		printf("\n");
	}
};

void Statements::emit_source(){
	iden++;
	for (unsigned int i = 0, e = vars.size(); i < e; i++){
		print_iden(iden);
		vars[i]->emit_source();
		printf("\n");
	}
	for (unsigned int i = 0, e = funcs.size(); i < e; i++){
		funcs[i]->emit_source();
		printf("\n");
	}
	for (unsigned int i = 0, e = stmts.size(); i < e; i++){
		print_iden(iden);
		stmts[i]->emit_source();
		printf("\n");
	}
	iden--;
};

void Var_def::emit_source(){
	printf("%s %s", type_str.c_str(), name.c_str());
};

void Factor_const_num::emit_source(){
	printf("%u", value);
};

void Factor_const_str::emit_source(){
	printf("\"%s\"", value.c_str());
};

void Factor_var::emit_source(){
	printf("%s", name.c_str());
};

void Factor_call::emit_source(){
	printf("%s(", name.c_str());
	if (args.size() > 0){
		for (unsigned int i = 0, e = args.size() - 1; i < e; i++){
			printf("%s, ", args[i].c_str());
		}
		printf("%s", args.back().c_str());
	}
	printf(")");
};

void Binary_op::emit_source(){
	printf("(");
	left->emit_source();
	printf(" %s ", operator_map[op]->str.c_str());
	right->emit_source();
	printf(")");
};

void If_block::emit_source(){
	printf("if (");
	condition->emit_source();
	printf("){\n");
	stmts_true->emit_source();
	print_iden(iden);
	printf("}");
	if (stmts_false){
		printf(" else {\n");
		stmts_false->emit_source();
		printf("}");
	}
};

void While_block::emit_source(){
	printf("while (");
	condition->emit_source();
	printf("){\n");
	stmts->emit_source();
	print_iden(iden);
	printf("}");
};

void Func_def::emit_source(){
	ret_var->emit_source();
	printf("(");
	if (args.size() > 0){
		for (unsigned int i = 0, e = args.size() - 1; i < e; i++){
			args[i]->emit_source();
			printf(", ");
		}
		args.back()->emit_source();
	}
	printf("){\n");
	stmts->emit_source();
	printf("};\n");
};
