#ifndef __PARSER_HPP__
#define __PARSER_HPP__

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

class Type {
public:
    Identifier name;
    Type(const Identifier & name) : name(name) {};
};

class TypeManager {
    std::map<Identifier, const Type *> _map;
public:
    const Type intType;
    TypeManager() : intType("int") {
    };
    void init() {
        regi("int", intType);
    };

    void regi(const char * s, const Type & type);
    int count(const Identifier & iden) {
        return _map.count(iden);
    };
    const Type * operator[] (const Identifier & type) {
        return _map[type];
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
    OperatorType type;
    const int prec;

    Operator(const std::string & name, int prec, OperatorType type) : name(name), prec(prec), type(type) {
    };
    virtual void dump() const;
};

class OperatorManager {
    class Trie {
    };
public:
    const Operator * binOrLeftUnaryManager[256];
    const Operator * rightUnaryManager[256];
    bool leftAssoManager[100];

    OperatorManager();
    void init();

    void regi(int tok, const Operator & op, bool leftAsso);
};

class Top {
public:
    std::list<Definition *> defList;
    TypeManager typeManager;
    OperatorManager opManager;
    std::map<std::string, FuncDef *> funcManager;
    std::map<std::string, VarDef *> varManager;

    virtual std::map<std::string, VarDef *> & getVarManager() {
        return varManager;
    };
    virtual const std::map<std::string, VarDef *> & getVarManager() const {
        return varManager;
    };
    virtual void dump() const;
};

class Definition {
public:
    const Type * type;
    Identifier name;

    Definition(const Type * type, const Identifier & name) : type(type), name(name) {};
    virtual void dump() const = 0;
};

class VarDef : public Definition {
public:
    VarDef(const Type * type, const Identifier & name) : Definition(type, name) {};
    virtual void dump() const;
};

class FuncDef : public Definition {
public:
    std::list<VarDef *> arguments;
    std::map<std::string, VarDef *> varManager;
    std::list<Statement *> stmtList;

    FuncDef(const Type * type, const Identifier & name) : Definition(type, name) {};
    virtual void dump() const;
};

class Statement {
public:
    Type * type;

    Statement() : type(NULL) {};
    virtual void dump() const = 0;
};

class Expr : public Statement {
public:
    const Operator * op;
    Expr * left;
    Expr * right;

    Expr(const Operator * op) : Statement(), op(op), left(NULL), right(NULL) {};
    virtual void dump() const;
};

class FactorNode : public Expr {
public:
    FactorNode() : Expr(&Operator::opFactor) {};
    virtual void dump() const = 0;
};

class ConstantNode : public FactorNode {
public:
    ConstantNode() {};
    virtual void dump() const = 0;
};

class ConstantNumNode : public ConstantNode {
public:
    long long value;

    ConstantNumNode(long long value) : ConstantNode(), value(value) {};
    virtual void dump() const;
};

class VarNode : public FactorNode {
public:
    const VarDef * var;

    VarNode(const VarDef * var) : var(var) {};
    virtual void dump() const;
};

class CallNode : public FactorNode {
public:
    FuncDef * func;
    std::list<Expr *> arguments;

    CallNode(FuncDef * func) : func(func) {};
    virtual void dump() const;
};

class Return : public Statement {
public:
    const Expr * value;
    
    Return(const Expr * value) : value(value) {};
    virtual void dump() const;
};

#endif
