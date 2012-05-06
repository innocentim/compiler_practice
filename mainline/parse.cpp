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

Expr * parse_expr(Environment & env);
Statement * parse_return(Environment & env) {
    eat(kwd_return);
    return new Return(parse_expr(env));
};

CallNode * parse_call_node(Environment & env) {
    CallNode * ret;
    if (!top.funcManager.count(*((std::string *)tokens[current].data))) {
        error("function not find");
    }
    ret = new CallNode(top.funcManager[*((std::string *)tokens[current].data)]);
    eat(identifier);
    eat(lparen);
    std::list<VarDef *>::const_iterator iter, e;
    for (iter = ret->func->arguments.begin(), e = ret->func->arguments.end(); iter != e; ++iter) {
        Expr * temp = parse_expr(env);
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

VarNode * parse_var_node(Environment & env) {
    std::string & str = *(std::string *)tokens[current].data;
    eat(identifier);
    if (env.getVarManager().count(str)) {
        return new VarNode(env.getVarManager().find(str)->second);
    }
    if (top.varManager.count(str)) {
        return new VarNode(top.varManager[str]);
    }
    error("variable not find");
};

ConstantNumNode * parse_const_int() {
    ConstantNumNode * ret = new ConstantNumNode(*((long long *)tokens[current].data));
    eat(constant_int);
    return ret;
};

FactorNode * parse_factor(Environment & env) {
    switch (tokens[current]) {
    case identifier:
        if (tokens[current + 1] == lparen) { // CallNode
            return parse_call_node(env);
        }
        return parse_var_node(env);
    case constant_int:
        return parse_const_int();
    default:
        error("invalid factor");
    }
};

Expr * parse_expr(Environment & env) {
    std::list<Expr *> stack;
    Expr _t(&opRoot);
    stack.push_back(&_t);
    bool filled = false;
    while (1) {
        int tok = tokens[current];
        Expr * now = stack.back();
        if (filled) { // binary or left-unary
            if (binOrLeftUnaryManager[tok]) {
                int prec = binOrLeftUnaryManager[tok]->prec;
                Expr * newNode = new Expr(binOrLeftUnaryManager[tok]);
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
                stack.push_back(now->right = parse_factor(env));
                filled = true;
            } else if (rightUnaryManager[tok]) {
                stack.push_back(now->right = new Expr(rightUnaryManager[tok]));
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

VarDef * parse_var(Environment & env) {
    Identifier & type = *(std::string *)tokens[current].data;
    Identifier & name = *(std::string *)tokens[current + 1].data;
    eat(identifier);
    eat(identifier);

    if (!top.typeManager.count(type)) {
        error("no such type");
    }
    if (env.getVarManager().count(name)) {
        error("variable duplicated definition");
    }
    return env.getVarManager()[name] = new VarDef(top.typeManager[type], name);
};

FuncDef * parse_func() {
    Identifier & funcType = *(std::string *)tokens[current].data;
    Identifier & funcName = *(std::string *)tokens[current + 1].data;
    FuncDef * ret = new FuncDef(top.typeManager[funcType], funcName);
    eat(identifier);
    eat(identifier);
    if (!top.typeManager.count(funcType)) {
        error("no such type");
    }
    if (top.funcManager.count(funcName)) {
        error("function duplicated definition");
    }
    eat(lparen);
    if (tokens[current] != rparen) {
        while (1) {
            Identifier & type = *(std::string *)tokens[current].data;
            Identifier & name = *(std::string *)tokens[current + 1].data;
            eat(identifier);
            eat(identifier);
            if (!top.typeManager.count(type)) {
                error("no such type");
            }
            if (ret->block.varManager.count(name)) {
                error("duplicated definition");
            }
            ret->arguments.push_back(ret->block.varManager[name] = new VarDef(top.typeManager[type], name));
            if (tokens[current] != rparen) {
                eat(comma);
            } else {
                break;
            }
        }
    }
    eat(rparen);
    eat(lbrace);
    while (1) {
        switch (tokens[current]) {
        case kwd_return:
            ret->block.stmtList.push_back(parse_return(ret->block));
            break;
        case identifier:
            if (tokens[current + 1] != identifier) {
        default:
                ret->block.stmtList.push_back(parse_expr(ret->block));
                break;
            }
            top.defList.push_back(parse_var(ret->block));
            break;
        case rbrace:
            goto out;
        }
    }
out:
    eat(rbrace);
    return top.funcManager[funcName] = ret;
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
                temp = parse_var(top);
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

    intType.name = "int";
    top.typeManager["int"] = &intType;

    leftAssoManager[10] = false; // =
    leftAssoManager[20] = true; // + -
    leftAssoManager[30] = true; // * /
    leftAssoManager[40] = false; // a++
    binOrLeftUnaryManager[equ] = &opAssign;
    binOrLeftUnaryManager[plus] = &opAdd;
    rightUnaryManager[plus] = &opPos;
    binOrLeftUnaryManager[star] = &opMul;

    parse_top();
};
