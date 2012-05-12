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
    static const Type Int;
    const Identifier name;
    Type(const Identifier & name, llvm::Type * value) : value(value), name(name) {};
};

class TypeManager {
    std::map<Identifier, const Type *> _map;
public:
    TypeManager() {};
    void init() {
        regi("int", Type::Int);
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
    static const Operator Assign;
    static const Operator Add;
    static const Operator Pos;
    static const Operator Mul;
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
public:
    typedef llvm::Value * (*Callback)(const FuncDef * env, const std::list<llvm::Value *> & args, const Type *& type);
    class Trie {
        std::map<const Type *, Trie *> children;
    public:
        Callback callback;

        Trie() : callback(NULL) {};
        void insert(const std::list<const Type *> & types, Callback callback);
        Trie * next(const Type & type) {
            if (children.count(&type)) {
                return children[&type];
            }
            return NULL;
        };
    };
private:
    std::map<const Operator *, Trie> _map;
public:
    const Operator * binOrLeftUnaryManager[256];
    const Operator * rightUnaryManager[256];
    bool leftAssoManager[100];

    OperatorManager() {};
    void init();
    void regi(int tok, const Operator & op);
    void overload(const Operator & op, const std::list<const Type *> & types, Callback callback);
    Trie * operator[](const Operator & op) {
        if (_map.count(&op)) {
            return &_map[&op];
        }
        return NULL;
    };
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
    llvm::Value * value;
    VarDef(const Type & type, const Identifier & name) : Definition(type, name) {};
    virtual void dump() const;
    virtual llvm::Value * code_gen(const FuncDef * env);
};

class FuncDef : public Definition {
public:
    llvm::Function * value;
    std::list<VarDef *> arguments;
    std::list<VarDef *> localVar;
    std::map<std::string, VarDef *> varManager;
    std::list<Statement *> stmtList;

    FuncDef(const Type & type, const Identifier & name) : Definition(type, name) {};
    virtual void dump() const;
    virtual llvm::Function * code_gen(const FuncDef * env);
};

class Statement {
public:
    Statement() {};
    virtual void dump() const = 0;
    virtual llvm::Value * code_gen(const FuncDef * env) = 0;
};

class Expr : public Statement {
public:
    const Type * type;
    Expr * left;
    Expr * right;

    Expr() : Statement(), type(NULL), left(NULL), right(NULL) {};
    virtual void dump() const = 0;
    virtual llvm::Value * code_gen(const FuncDef * env) = 0;
};

class FactorNode : public Expr {
public:
    FactorNode() {};
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

    ConstantNumNode(long long num) : num(num) {};
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

class OpNode : public CallNode {
public:
    const Operator * op;
    OpNode(const Operator * op) : CallNode(NULL), op(op) {};
    virtual void dump() const;
    virtual llvm::Value * code_gen(const FuncDef * env);
};

class Return : public Statement {
public:
    Expr * retExpr;
    
    Return(Expr * retExpr) : retExpr(retExpr) {};
    virtual void dump() const;
    virtual llvm::Value * code_gen(const FuncDef * env);
};

#endif
