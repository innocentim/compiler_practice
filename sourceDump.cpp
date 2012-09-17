#include <cstdio>
#include "parse.hpp"

static int indent = 0;

void printIden() {
	for (int i = indent; i > 0; i--) {
		printf("	");
	}
}

void Top::dump() const {
	{
		std::map<Identifier, VarDef *>::const_iterator iter;
		for (iter = varManager.begin(); iter != varManager.end(); ++iter) {
			iter->second->dump();
			printf("\n");
		}
	}
	{
		std::map<Identifier, FuncDef *>::const_iterator iter;
		for (iter = funcManager.begin(); iter != funcManager.end(); ++iter) {
			iter->second->dump();
			printf("\n");
		}
	}
};

void VarDef::dump() const {
	printf("%s %s", type.name.c_str(), name.c_str());
};

void FuncDef::dump() const {
	printf("%s %s(", type.name.c_str(), name.c_str());
	if (arguments.size() > 0) {
		std::list<VarDef *>::const_iterator iter = arguments.begin();
		printf("%s %s", (*iter)->type.name.c_str(), (*iter)->name.c_str());
		++iter;
		for (; iter != arguments.end(); ++iter) {
			printf(", %s %s", (*iter)->type.name.c_str(), (*iter)->name.c_str());
		}
	}
	printf(") {\n");
	indent++;
	{
		std::map<std::string, VarDef *>::const_iterator iter;
		for (iter = varManager.begin(); iter != varManager.end(); ++iter) {
			printIden();
			(*iter).second->dump();
			printf("\n");
		}
	}
	{
		std::list<Statement *>::const_iterator iter;
		for (iter = stmtList.begin(); iter != stmtList.end(); ++iter) {
			printIden();
			(*iter)->dump();
			printf("\n");
		}
	}
	indent--;
	printf("}\n");
};

void OpNode::dump() const {
	printf("(");
	if (left) {
		left->dump();
		printf(" ");
	}
	op->dump();
	if (right) {
		printf(" ");
		right->dump();
	}
	printf(")");
};

void ConstantNumNode::dump() const {
	printf("%lld", num);
};

void VarNode::dump() const {
	printf("%s", var->name.c_str());
};

void CallNode::dump() const {
	printf("%s(", func->name.c_str());
	if (arguments.size()) {
		std::list<Expr *>::const_iterator iter = arguments.begin();
		(*iter)->dump();
		++iter;
		for (; iter != arguments.end(); ++iter) {
			printf(", ");
			(*iter)->dump();
		}
	}
	printf(")");
};

void Operator::dump() const {
	printf("%s", name.c_str());
};

void Return::dump() const {
	printf("return");
	if (retExpr) {
		printf(" ");
		retExpr->dump();
	}
};
