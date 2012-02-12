#include "AST.hpp"
#include <cstdio>

static int iden = 0;
extern char * punc_dump[];

void print_iden(int iden){
	while (iden--){
		printf("    ");
	}
};

void Stmts::emitSource() const {
	StmtList::const_iterator it, e;
	for (it = stmts.begin(), e = stmts.end(); it != e; ++it){
		print_iden(iden);
		(**it).emitSource();
		printf("\n");
	}
};

void FactorVar::emitSource() const {
	printf("%s", str.c_str());
};

void FactorNum::emitSource() const {
	printf("%lld", value);
};

void FactorStr::emitSource() const {
	printf("\"%s\"", str.c_str());
};

void FactorCall::emitSource() const {
	printf("%s", name.c_str());
	printf("(");
	ExprList::const_iterator it, e;
	if (args.size() > 0) {
		it = args.begin();
		(**it).emitSource();
		++it;
		for (e = args.end(); it != e; ++it){
			printf(", ");
			(**it).emitSource();
		}
	}
	printf(")");
};

void BinaryOp::emitSource() const {
	printf("(");
	left.emitSource();
	printf(" %s ", punc_dump[op]);
	right.emitSource();
	printf(")");
};

void Assignment::emitSource() const {
	printf("(");
	printf("%s", lvalue.c_str());
	printf(" = ");
	rvalue.emitSource();
	printf(")");
};

void VarDef::emitSource() const {
	printf("%s", type.c_str());
	printf(" ");
	printf("%s", name.c_str());
	if (assign != NULL){
		printf(" = ");
		assign->emitSource();
	}
};

void FuncDef::emitSource() const {
	printf("%s", type.c_str());
	printf(" ");
	printf("%s", name.c_str());
	printf("(");
	VarList::const_iterator it, e;
	if (args.size() > 0){
		it = args.begin();
		(**it).emitSource();
		++it;
		for (e = args.end(); it != e; ++it){
			printf(", ");
			(**it).emitSource();
		}
	}
	printf(") {\n");
	iden++;
	body.emitSource();
	iden--;
	print_iden(iden);
	printf("}");
};

void ExprStmt::emitSource() const {
	expr.emitSource();
};

void Return::emitSource() const {
	printf("return ");
	ret->emitSource();
};

