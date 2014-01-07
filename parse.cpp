#include "lex.hpp"
#include "parse.hpp"
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <list>
#include <map>
#include <string>
#include <cstdio>
#include <cstdlib>

#define error(s) do { fprintf(stderr, "error: %s\n", s); exit(1); } while (0)

extern Token tokens[];
extern int tokenN;
static int current;
Top top;
static OperatorManager & opManager = top.opManager;

const Type Type::Int("int", llvm::Type::getInt64Ty(llvm::getGlobalContext()));
const Operator Operator::Assign("=", 10, Operator::BINARY, false);
const Operator Operator::Add("+", 20, Operator::BINARY, true);
const Operator Operator::Pos("+", 40, Operator::RIGHT_UNARY, true);
const Operator Operator::Mul("*", 30, Operator::BINARY, true);
FuncDef FuncDef::externPutchar(Type::Int, "putchar", true);

void TypeManager::regi(const char * s, const Type & type) {
	if (map.count(s)) {
		error("duplicate type registor");
	}
	map[s] = &type;
};

void OperatorManager::init() {
	memset(binOrLeftUnaryManager, 0, sizeof(binOrLeftUnaryManager));
	memset(rightUnaryManager, 0, sizeof(rightUnaryManager));
	regi(EQU, Operator::Assign);
	regi(PLUS, Operator::Add);
	regi(PLUS, Operator::Pos);
	regi(STAR, Operator::Mul);
};

void OperatorManager::Trie::insert(const std::list<const Type *> & types, Callback callback) {
	std::list<const Type *>::const_iterator iter = types.begin();
	Trie * now = this;
	while (iter != types.end()) {
		if (now->children.count(*iter) == 0){
			now->children[*iter] = new Trie();
		}
		now = now->children[*iter];
		++iter;
	}
	if (now->callback) {
		error("duplicate operator overload");
	}
	now->callback = callback;
}

void OperatorManager::regi(int tok, const Operator & op) {
	if (leftAssoManager[op.prec] != -1) {
		if (leftAssoManager[op.prec] != op.leftAsso) {
			error("inconsistent left associate");
		}
	} else {
		leftAssoManager[op.prec] = op.leftAsso;
	}
	switch (op.type) {
	case Operator::BINARY:
	case Operator::LEFT_UNARY:
		if (opManager.binOrLeftUnaryManager[tok]) {
			error("duplicate operator registor");
		}
		opManager.binOrLeftUnaryManager[tok] = &op;
		break;
	case Operator::RIGHT_UNARY:
		if (opManager.rightUnaryManager[tok]) {
			error("duplicate operator registor");
		}
		opManager.rightUnaryManager[tok] = &op;
		break;
	default:
		error("unexpected operator registor");
	}
};

void OperatorManager::overload(const Operator & op, const std::list<const Type *> & types, Callback callback) {
	map[&op].insert(types, callback);
};

static void eat(int token) {
	if (tokens[current] == token) {
		current++;
		return;
	}
	error("token unexpected");
};

bool isConst(int token) {
	return token == CONSTANT_INT;
};

Expr * parseExpr(FuncDef * env);
Statement * parseReturn(FuncDef * env) {
	eat(KWD_RETURN);
	return new Return(parseExpr(env));
};

CallNode * parseCallNode(FuncDef * env) {
	CallNode * ret;
	if (!top.funcManager.count(*((std::string *)tokens[current].data))) {
		error("function not find");
	}
	FuncDef * func = top.funcManager[*((std::string *)tokens[current].data)];
	ret = new CallNode(func);
	eat(IDENTIFIER);
	eat(LPAREN);
	int n = func->arguments.size();
	while (n--) {
		Expr * temp = parseExpr(env);
		ret->arguments.push_back(temp);
		if (tokens[current] == COMMA) {
			eat(COMMA);
		} else {
			break;
		}
	}
	eat(RPAREN);
	if (n != 0) {
		error("number of arguments unexpected");
	}
	return ret;
};

VarNode * parseVarNode(FuncDef * env) {
	std::string & str = *(std::string *)tokens[current].data;
	eat(IDENTIFIER);
	if (env->varManager.count(str)) {
		return new VarNode(env->varManager.find(str)->second);
	}
	if (top.varManager.count(str)) {
		return new VarNode(top.varManager[str]);
	}
	error("variable not find");
};

ConstantNumNode * parseConstInt() {
	ConstantNumNode * ret = new ConstantNumNode(*((unsigned long long *)tokens[current].data));
	eat(CONSTANT_INT);
	return ret;
};

FactorNode * parseFactor(FuncDef * env) {
	switch (tokens[current]) {
	case IDENTIFIER:
		if (tokens[current + 1] == LPAREN) { // CallNode
			return parseCallNode(env);
		}
		return parseVarNode(env);
	case CONSTANT_INT:
		return parseConstInt();
	default:
		error("invalid factor");
	}
};

Expr * parseExpr(FuncDef * env) {
	static const Operator root("", 0, Operator::RIGHT_UNARY, true);
	std::list<OpNode *> stack;
	OpNode t(&root);
	stack.push_back(&t);
	bool filled = false;
	while (1) {
		int tok = tokens[current];
		OpNode * now = stack.back();
		if (filled) { // BINARY or left-unary
			if (opManager.binOrLeftUnaryManager[tok]) {
				int prec = opManager.binOrLeftUnaryManager[tok]->prec;
				OpNode * newNode = new OpNode(opManager.binOrLeftUnaryManager[tok]);
				while (now->op->prec > prec || (now->op->prec == prec && opManager.leftAssoManager[prec])) {
					stack.pop_back();
					now = stack.back();
				}
				newNode->left = now->right;
				now->right = newNode;
				stack.push_back(newNode);
				current++;
				if (newNode->op->type == Operator::BINARY) {
					filled = false;
				} else {
					filled = true;
				}
			} else {
				// paren check
				return stack.front()->right;
			}
		} else { // factor or right-unary
			if (tok == IDENTIFIER || isConst(tok)) {
				now->right = parseFactor(env);
				filled = true;
			} else if (tok == LPAREN) {
				eat(LPAREN);
				now->right = parseExpr(env);
				eat(RPAREN);
				filled = true;
			} else if (opManager.rightUnaryManager[tok]) {
				OpNode * temp = new OpNode(opManager.rightUnaryManager[tok]);
				stack.push_back(temp);
				now->right = temp;
				current++;
				filled = false;
			} else {
				if (stack.size() > 1) {
					error("rvalue expected");
				} else {
					break;
				}
			}
		}
	}
	return NULL;
};

VarDef * parseVar(FuncDef * env) {
	Identifier & type = *(std::string *)tokens[current].data;
	Identifier & name = *(std::string *)tokens[current + 1].data;
	eat(IDENTIFIER);
	eat(IDENTIFIER);

	if (!top.typeManager.count(type)) {
		error("no such type");
	}
	if (env) {
		if (env->varManager.count(name)) {
			error("variable duplicated definition");
		}
		return env->varManager[name] = new VarDef(top.typeManager[type], name);
	}
	if (top.varManager.count(name)) {
		error("variable duplicated definition");
	}
	return top.varManager[name] = new VarDef(top.typeManager[type], name);
};

FuncDef * parseFunc() {
	Identifier & funcType = *(std::string *)tokens[current].data;
	Identifier & funcName = *(std::string *)tokens[current + 1].data;
	eat(IDENTIFIER);
	eat(IDENTIFIER);
	if (!top.typeManager.count(funcType)) {
		error("no such type");
	}
	if (top.funcManager.count(funcName)) {
		error("function duplicated definition");
	}
	FuncDef * ret = new FuncDef(top.typeManager[funcType], funcName);
	eat(LPAREN);
	if (tokens[current] != RPAREN) {
		while (1) {
			Identifier & type = *(std::string *)tokens[current].data;
			Identifier & name = *(std::string *)tokens[current + 1].data;
			eat(IDENTIFIER);
			eat(IDENTIFIER);
			if (!top.typeManager.count(type)) {
				error("no such type");
			}
			if (ret->varManager.count(name)) {
				error("duplicated definition");
			}
			ret->arguments.push_back(ret->varManager[name] = new VarDef(top.typeManager[type], name));
			if (tokens[current] != RPAREN) {
				eat(COMMA);
			} else {
				break;
			}
		}
	}
	eat(RPAREN);
	eat(LBRACE);
	VarDef * temp;
	while (1) {
		switch (tokens[current]) {
		case KWD_RETURN:
			ret->stmtList.push_back(parseReturn(ret));
			break;
		case IDENTIFIER:
			if (tokens[current + 1] != IDENTIFIER) {
		default:
				ret->stmtList.push_back(parseExpr(ret));
				break;
			}
			temp = parseVar(ret);
			ret->varManager[temp->name] = temp;
			break;
		case RBRACE:
			goto out;
		case RPAREN:
			error("RPAREN unexpected");
			break;
		}
	}
out:
	eat(RBRACE);
	return top.funcManager[funcName] = ret;
};

Top * parseTop() {
	while (current < tokenN) {
		switch (tokens[current]) {
		case IDENTIFIER:
			if (tokens[current + 1] != IDENTIFIER) {
				error("definition expected");
			}
			if (tokens[current + 2] == LPAREN) {
				parseFunc();
			} else {
				parseVar(NULL);
			}
			break;
		default:
			error("token unexpected");
		}
	}
	return &top;
};

extern void codeGen();

void parseInit() {
	current = 0;
	top.typeManager.init();
	top.opManager.init();
	FuncDef::externPutchar.arguments.push_back(new VarDef(Type::Int, "ch"));
	top.funcManager["putchar"] = &FuncDef::externPutchar;
	parseTop();
	codeGen();
	//top.dump();
};
