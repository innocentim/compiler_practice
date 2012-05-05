#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include <string>
#include <list>
#include <map>
#include <cassert>

typedef std::string Identifier;
class Type;
class Definition;
class FuncDef;
class VarDef;
class Statement;
class Operator;

class Top {
public:
    std::list<Definition *> defList;
    std::map<std::string, Type *> typeManager;
    std::map<std::string, FuncDef *> funcManager;
    std::map<std::string, VarDef *> varManager;

    virtual void dump() const;
};

class Definition {
public:
    Type * type;
    Identifier name;

    Definition(Type * type, const Identifier & name) : type(type), name(name) {};
    virtual void dump() const = 0;
};

class VarDef : public Definition {
public:
    VarDef(Type * type, const Identifier & name) : Definition(type, name) {};
    virtual void dump() const;
};

class FuncDef : public Definition {
public:
    std::list<VarDef *> arguments;
    std::list<Statement *> stmtList;
    std::map<std::string, VarDef *> varManager;

    FuncDef(Type * type, const Identifier & name) : Definition(type, name) {};
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

extern const Operator opFactor;
class FactorNode : public Expr {
public:
    FactorNode() : Expr(&opFactor) {};
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

class Type {
public:
    Identifier name;
};

class Operator {
    std::string name;
public:
    enum OperatorType{
        binary,
        left_unary,
        right_unary,
        factor,
    };
    OperatorType type;
    const int prec;

    Operator(const std::string & name, int prec, OperatorType type) : name(name), prec(prec), type(type) {};
    virtual void dump() const;
};

#endif
