#include "AST.hpp"
#include <cstdio>
#include <cassert>

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

void Identifier::emitSource() const {
	printf("%s", str.c_str());
};

void FactorNum::emitSource() const {
	printf("%lld", value);
};

void FactorStr::emitSource() const {
	printf("\"%s\"", str.c_str());
};

void FactorCall::emitSource() const {
	name.emitSource();
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
	lvalue.emitSource();
	printf(" = ");
	rvalue.emitSource();
};

void VarDef::emitSource() const {
	type.emitSource();
	printf(" ");
	name.emitSource();
	if (assign != NULL){
		printf(" = ");
		assign->emitSource();
	}
};

void FuncDef::emitSource() const {
	type.emitSource();
	printf(" ");
	name.emitSource();
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
	printf("}");
};

void ExprStmt::emitSource() const {
	expr.emitSource();
};

