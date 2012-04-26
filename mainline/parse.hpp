#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include <string>
#include <list>
#include <map>
#include <cassert>

typedef std::string Identifier;
class Type;
class Definition;
class Expr;
class Operator;

class Top {
public:
    std::list<Definition *> defList;

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
    std::list<Expr *> exprList;
    std::map<std::string, VarDef *> varManager;

    FuncDef(Type * type, const Identifier & name) : Definition(type, name) {};
    virtual void dump() const;
};

class Expr {
public:
    Type * type;

    Expr() : type(NULL) {};
    virtual void dump() const = 0;
};

class OpNode : public Expr {
public:
    const Operator * op;
    OpNode * left;
    OpNode * right;

    OpNode(const Operator * op) : Expr(), op(op), left(NULL), right(NULL) {};
    virtual void dump() const;
};

extern const Operator opFactor;
class FactorNode : public OpNode {
public:
    FactorNode() : OpNode(&opFactor) {};
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

class Return : public Expr {
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
