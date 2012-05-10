#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <string>
#include <list>
#include <map>
#include <cassert>
#include <cstring>

typedef std::string Identifier;
class Type;
class Definition;
class FuncDef;
class VarDef;
class Statement;
class Operator;
class Expr;

class Type {
public:
    llvm::Type * value;
    static const Type intType;
    const Identifier name;
    Type(const Identifier & name, llvm::Type * value) : value(value), name(name) {};
};

class TypeManager {
    std::map<Identifier, const Type *> _map;
public:
    TypeManager() {};
    void init() {
        regi("int", Type::intType);
    };

    void regi(const char * s, const Type & type);
    int count(const Identifier & iden) {
        return _map.count(iden);
    };
    const Type & operator[] (const Identifier & type) {
        return *_map[type];
    };
};

class Operator {
    std::string name;
public:
    static const Operator opRoot;
    static const Operator opFactor;
    static const Operator opAssign;
    static const Operator opAdd;
    static const Operator opPos;
    static const Operator opMul;
    enum OperatorType{
        binary,
        left_unary,
        right_unary,
        factor,
    };
    const int prec;
    const OperatorType type;
    const bool leftAsso;

    Operator(const std::string & name, int prec, OperatorType type, bool leftAsso) : name(name), prec(prec), type(type), leftAsso(leftAsso) {};
    virtual void dump() const;
};

class OperatorManager {
    class Trie {
        std::map<const Type *, Trie *> children;
    public:
        const FuncDef * data;
        Trie() : data(NULL) {};
        Trie *& next(const Type & type) {
            if (!children.count(&type)) {
                children[&type] = 0;
            }
            return children[&type];
        }
    };
public:
    std::map<const Operator *, Trie> _map;
    const Operator * binOrLeftUnaryManager[256];
    const Operator * rightUnaryManager[256];
    bool leftAssoManager[100];

    OperatorManager();
    void init();

    void regi(int tok, const Operator & op);
    void override(const Operator & op, const FuncDef * func);
};

class Top {
public:
    std::list<Definition *> defList;
    TypeManager typeManager;
    OperatorManager opManager;
    std::map<std::string, FuncDef *> funcManager;
    std::map<std::string, VarDef *> varManager;

    virtual void dump() const;
    virtual llvm::Module * code_gen();
};

class Definition {
public:
    const Type & type;
    const Identifier name;

    Definition(const Type & type, const Identifier & name) : type(type), name(name) {};
    virtual void dump() const = 0;
    virtual llvm::Value * code_gen(const FuncDef * env) = 0;
};

class VarDef : public Definition {
public:
    VarDef(const Type & type, const Identifier & name) : Definition(type, name) {};
    virtual void dump() const;
    virtual llvm::Value * code_gen(const FuncDef * env);
};

class FuncDef : public Definition {
    //void (*_callBack)(const FuncDef * func);
public:
    llvm::Function * value;
    std::list<VarDef *> arguments;
    std::map<std::string, VarDef *> varManager;
    std::list<Statement *> stmtList;

    FuncDef(const Type & type, const Identifier & name/*, void (*_callBack)(const FuncDef * func) = NULL*/) : Definition(type, name)/*, _callBack(_callBack)*/ {};
    virtual void dump() const;
    virtual llvm::Value * code_gen(const FuncDef * env);
};

class Statement {
public:
    Type * type;

    Statement() : type(NULL) {};
    virtual void dump() const = 0;
    virtual llvm::Value * code_gen(const FuncDef * env) = 0;
};

class Return : public Statement {
public:
    const Expr * retExpr;
    
    Return(const Expr * retExpr) : retExpr(retExpr) {};
    virtual void dump() const;
    virtual llvm::Value * code_gen(const FuncDef * env);
};

class Expr : public Statement {
public:
    const Operator * op;
    Expr * left;
    Expr * right;

    Expr(const Operator * op) : Statement(), op(op), left(NULL), right(NULL) {};
    virtual void dump() const;
    virtual llvm::Value * code_gen(const FuncDef * env);
};

class FactorNode : public Expr {
public:
    FactorNode() : Expr(&Operator::opFactor) {};
    virtual void dump() const = 0;
    virtual llvm::Value * code_gen(const FuncDef * env) = 0;
};

class ConstantNode : public FactorNode {
public:
    ConstantNode() {};
    virtual void dump() const = 0;
    virtual llvm::Value * code_gen(const FuncDef * env) = 0;
};

class ConstantNumNode : public ConstantNode {
public:
    long long num;

    ConstantNumNode(long long num) : ConstantNode(), num(num) {};
    virtual void dump() const;
    virtual llvm::Value * code_gen(const FuncDef * env);
};

class VarNode : public FactorNode {
public:
    const VarDef * var;

    VarNode(const VarDef * var) : var(var) {};
    virtual void dump() const;
    virtual llvm::Value * code_gen(const FuncDef * env);
};

class CallNode : public FactorNode {
public:
    FuncDef * func;
    std::list<Expr *> arguments;

    CallNode(FuncDef * func) : func(func) {};
    virtual void dump() const;
    virtual llvm::Value * code_gen(const FuncDef * env);
};

#endif
