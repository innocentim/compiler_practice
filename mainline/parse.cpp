#include "lex.hpp"
#include "parse.hpp"
#include <llvm/Type.h>
#include <llvm/DerivedTypes.h>
#include <list>
#include <map>
#include <string>
#include <cstdio>
#include <cstdlib>

#define error(s) do { fprintf(stderr, "error: %s\n", s); exit(1); } while (0)

extern Token tokens[];
extern int token_n;
static int current;
Top top;
static OperatorManager & opManager = top.opManager;

const Type Type::Int("int", llvm::Type::getInt64Ty(llvm::getGlobalContext()));
const Operator Operator::Assign("=", 10, Operator::binary, false);
const Operator Operator::Add("+", 20, Operator::binary, true);
const Operator Operator::Pos("+", 40, Operator::right_unary, true);
const Operator Operator::Mul("*", 30, Operator::binary, true);
FuncDef FuncDef::externPutchar(Type::Int, "putchar", true);

void TypeManager::regi(const char * s, const Type & type) {
    if (_map.count(s)) {
        error("duplicate type registor");
    }
    _map[s] = &type;
};

void OperatorManager::init() {
    memset(binOrLeftUnaryManager, 0, sizeof(binOrLeftUnaryManager));
    memset(rightUnaryManager, 0, sizeof(rightUnaryManager));
    regi(equ, Operator::Assign);
    regi(plus, Operator::Add);
    regi(plus, Operator::Pos);
    regi(star, Operator::Mul);
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
    case Operator::binary:
    case Operator::left_unary:
        if (opManager.binOrLeftUnaryManager[tok]) {
            error("duplicate operator registor");
        }
        opManager.binOrLeftUnaryManager[tok] = &op;
        break;
    case Operator::right_unary:
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
    _map[&op].insert(types, callback);
};

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

Expr * parse_expr(FuncDef * env);
Statement * parse_return(FuncDef * env) {
    eat(kwd_return);
    return new Return(parse_expr(env));
};

CallNode * parse_call_node(FuncDef * env) {
    CallNode * ret;
    if (!top.funcManager.count(*((std::string *)tokens[current].data))) {
        error("function not find");
    }
    FuncDef * func = top.funcManager[*((std::string *)tokens[current].data)];
    ret = new CallNode(func);
    eat(identifier);
    eat(lparen);
    int n = func->arguments.size();
    while (n--) {
        Expr * temp = parse_expr(env);
        ret->arguments.push_back(temp);
        if (tokens[current] == comma) {
            eat(comma);
        } else {
            break;
        }
    }
    eat(rparen);
    if (n != -1) {
        error("number of arguments unexpected");
    }
    return ret;
};

VarNode * parse_var_node(FuncDef * env) {
    std::string & str = *(std::string *)tokens[current].data;
    eat(identifier);
    if (env->varManager.count(str)) {
        return new VarNode(env->varManager.find(str)->second);
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

FactorNode * parse_factor(FuncDef * env) {
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

Expr * parse_expr(FuncDef * env) {
    static const Operator root("", 0, Operator::right_unary, true);
    std::list<OpNode *> stack;
    OpNode _t(&root);
    stack.push_back(&_t);
    bool filled = false;
    while (1) {
        int tok = tokens[current];
        OpNode * now = stack.back();
        if (filled) { // binary or left-unary
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
                if (newNode->op->type == Operator::binary) {
                    filled = false;
                } else {
                    filled = true;
                }
            } else {
                // paren check
                return stack.front()->right;
            }
        } else { // factor or right-unary
            if (tok == identifier || is_const(tok)) {
                now->right = parse_factor(env);
                filled = true;
            } else if (tok == lparen) {
                eat(lparen);
                now->right = parse_expr(env);
                eat(rparen);
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

VarDef * parse_var(FuncDef * env) {
    Identifier & type = *(std::string *)tokens[current].data;
    Identifier & name = *(std::string *)tokens[current + 1].data;
    eat(identifier);
    eat(identifier);

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
            if (ret->varManager.count(name)) {
                error("duplicated definition");
            }
            ret->arguments.push_back(ret->varManager[name] = new VarDef(top.typeManager[type], name));
            if (tokens[current] != rparen) {
                eat(comma);
            } else {
                break;
            }
        }
    }
    eat(rparen);
    eat(lbrace);
    VarDef * temp;
    while (1) {
        switch (tokens[current]) {
        case kwd_return:
            ret->stmtList.push_back(parse_return(ret));
            break;
        case identifier:
            if (tokens[current + 1] != identifier) {
        default:
                ret->stmtList.push_back(parse_expr(ret));
                break;
            }
            temp = parse_var(ret);
            ret->varManager[temp->name] = temp;
            break;
        case rbrace:
            goto out;
        case rparen:
            error("rparen unexpected");
            break;
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
            if (tokens[current + 2] == lparen) {
                parse_func();
            } else {
                parse_var(NULL);
            }
            break;
        default:
            error("token unexpected");
        }
    }
    return &top;
};

extern void code_gen();

void parse_init() {
    current = 0;
    top.typeManager.init();
    top.opManager.init();
    FuncDef::externPutchar.arguments.push_back(new VarDef(Type::Int, "ch"));
    top.funcManager["putchar"] = &FuncDef::externPutchar;
    parse_top();
    code_gen();
};
