#include <string>
#include <cstdio>
#include <cstdlib>
#include <list>
#include <map>
#include "lex.hpp"
#include "parse.hpp"

#define error(s) do { fprintf(stderr, "%s\n", s); exit(1); } while (0)

extern Token tokens[];
extern int token_n;
static int current;
static Top top;
static FuncDef * funcNow;

std::map<std::string, Type *> typeManager;
std::map<std::string, FuncDef *> funcManager;
std::map<std::string, VarDef *> varManager;
const Operator * binOrLeftUnaryManager[256] = { 0 };
const Operator * rightUnaryManager[256] = { 0 };
bool leftAssoManager[100] = { 0 };
const Operator opRoot("", 0, Operator::right_unary);
const Operator opFactor("", 99, Operator::factor);
const Operator opAssign("=", 10, Operator::binary);
const Operator opAdd("+", 20, Operator::binary);
const Operator opPos("+", 40, Operator::right_unary);
const Operator opMul("*", 30, Operator::binary);
Type intType;

static void eat(int token) {
    if (tokens[current] == token) {
        current++;
        return;
    }
    error("token unexpected");
};

bool is_const(int token) {
    return token == constant_int;
};

Expr * parse_expr();
Expr * parse_return() {
    eat(kwd_return);
    return new Return(parse_expr());
};

CallNode * parse_call_node() {
    CallNode * ret;
    if (!funcManager.count(*((std::string *)tokens[current].data))) {
        error("function not find");
    }
    ret = new CallNode(funcManager[*((std::string *)tokens[current].data)]);
    eat(identifier);
    eat(lparen);
    std::list<VarDef *>::const_iterator iter, e;
    for (iter = ret->func->arguments.begin(), e = ret->func->arguments.end(); iter != e; ++iter) {
        Expr * temp = parse_expr();
        ret->arguments.push_back(temp);
        if (tokens[current] == comma) {
            eat(comma);
        } else {
            break;
        }
    }
    eat(rparen);
    return ret;
};

VarNode * parse_var_node() {
    std::string & str = *(std::string *)tokens[current].data;
    eat(identifier);
    if (funcNow->varManager.count(str)) {
        return new VarNode(funcNow->varManager.find(str)->second);
    }
    if (varManager.count(str)) {
        return new VarNode(varManager[str]);
    }
    error("variable not find");
};

ConstantNumNode * parse_const_int() {
    ConstantNumNode * ret = new ConstantNumNode(*((long long *)tokens[current].data));
    eat(constant_int);
    return ret;
};

FactorNode * parse_factor() {
    switch (tokens[current]) {
    case identifier:
        if (tokens[current + 1] == lparen) { // CallNode
            return parse_call_node();
        }
        return parse_var_node();
    case constant_int:
        return parse_const_int();
    default:
        error("invalid factor");
    }
};

Expr * parse_expr() {
    std::list<OpNode *> stack;
    OpNode _t(&opRoot);
    stack.push_back(&_t);
    bool filled = false;
    while (1) {
        int tok = tokens[current];
        OpNode * now = stack.back();
        if (filled) { // binary or left-unary
            if (binOrLeftUnaryManager[tok]) {
                int prec = binOrLeftUnaryManager[tok]->prec;
                OpNode * newNode = new OpNode(binOrLeftUnaryManager[tok]);
                while (now->op->prec > prec || (now->op->prec == prec && leftAssoManager[prec])) {
                    stack.pop_back();
                    now = stack.back();
                }
                newNode->left = now->right;
                now->right = newNode;
                stack.push_back(newNode);
                current++;
                if (newNode->op->type == Operator::binary) {
                    filled = false;
                } else {
                    filled = true;
                }
            } else {
                return stack.front()->right;
            }
        } else { // factor or right-unary
            if (tok == identifier || is_const(tok)) {
                stack.push_back(now->right = parse_factor());
                filled = true;
            } else if (rightUnaryManager[tok]) {
                stack.push_back(now->right = new OpNode(rightUnaryManager[tok]));
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

VarDef * parse_var() {
    Identifier & type = *(std::string *)tokens[current].data;
    Identifier & name = *(std::string *)tokens[current + 1].data;
    eat(identifier);
    eat(identifier);

    if (!typeManager.count(type)) {
        error("no such type");
    }
    std::map<std::string, VarDef *> & manager = funcNow ? funcNow->varManager : varManager;
    if (manager.count(name)) {
        error("variable duplicated definition");
    }
    return manager[name] = new VarDef(typeManager[type], name);
};

FuncDef * parse_func() {
    Identifier & funcType = *(std::string *)tokens[current].data;
    Identifier & funcName = *(std::string *)tokens[current + 1].data;
    FuncDef * ret = new FuncDef(typeManager[funcType], funcName);
    eat(identifier);
    eat(identifier);
    if (!typeManager.count(funcType)) {
        error("no such type");
    }
    if (funcManager.count(funcName)) {
        error("function duplicated definition");
    }
    eat(lparen);
    if (tokens[current] != rparen) {
        while (1) {
            Identifier & type = *(std::string *)tokens[current].data;
            Identifier & name = *(std::string *)tokens[current + 1].data;
            eat(identifier);
            eat(identifier);
            if (!typeManager.count(type)) {
                error("no such type");
            }
            if (ret->varManager.count(name)) {
                error("duplicated definition");
            }
            ret->arguments.push_back(ret->varManager[name] = new VarDef(typeManager[type], name));
            if (tokens[current] != rparen) {
                eat(comma);
            } else {
                break;
            }
        }
    }
    eat(rparen);
    eat(lbrace);
    funcNow = ret;
    while (1) {
        switch (tokens[current]) {
        case kwd_return:
            ret->exprList.push_back(parse_return());
            break;
        case identifier:
            if (tokens[current + 1] != identifier) {
        default:
                ret->exprList.push_back(parse_expr());
                break;
            }
            top.defList.push_back(parse_var());
            break;
        case rbrace:
            goto out;
        }
    }
out:
    eat(rbrace);
    funcNow = NULL;
    return funcManager[funcName] = ret;
};

Top * parse_top() {
    while (current < token_n) {
        switch (tokens[current]) {
        case identifier:
            if (tokens[current + 1] != identifier) {
                error("definition expected");
            }
            Definition * temp;
            if (tokens[current + 2] == lparen) {
                temp = parse_func();
            } else {
                temp = parse_var();
            }
            top.defList.push_back(temp);
            temp->dump();
            printf("\n");
            break;
        default:
            error("token unexpected");
        }
    }
    return &top;
};

void parse_init() {
    current = 0;
    funcNow = NULL;

    intType.name = "int";
    typeManager["int"] = &intType;

    leftAssoManager[10] = false; // =
    leftAssoManager[20] = true; // + -
    leftAssoManager[30] = true; // * /
    leftAssoManager[40] = false; // a++, which should be larger than ++a
    binOrLeftUnaryManager[equ] = &opAssign;
    binOrLeftUnaryManager[plus] = &opAdd;
    rightUnaryManager[plus] = &opPos;
    binOrLeftUnaryManager[star] = &opMul;

    parse_top();
};
