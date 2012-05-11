#include "parse.hpp"
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Instructions.h>
#include <vector>
#include <cstdio>

#define error(s) do { fprintf(stderr, "error: %s\n", s); exit(1); } while (0)

using namespace llvm;

Module module("compiler_toy", getGlobalContext());
extern Top top;

BasicBlock * lastBlock(const FuncDef * func) {
    return &func->value->back();
};

Module * Top::code_gen() {
    std::list<Definition *>::const_iterator it;
    for (it = defList.begin(); it != defList.end(); ++it){
        (*it)->code_gen(NULL);
    }
    return &module;
};

Value * VarDef::code_gen(const FuncDef * env) {
    Value * ret;
    if (env) {
        ret = new AllocaInst(type.value, name, lastBlock(env));
    } else {
        GlobalVariable * temp;
        module.getGlobalList().push_back(temp = new GlobalVariable(type.value, false, GlobalVariable::CommonLinkage, 0, name));
        ret = temp;
    }
    return ret;
};

Function * FuncDef::code_gen(const FuncDef * env) {
    value = Function::Create(FunctionType::get(type.value, /*?*/false), Function::ExternalLinkage, name, &module);
    BasicBlock::Create(getGlobalContext(), "init", value);
    std::list<VarDef *>::const_iterator argIter;
    for (argIter = arguments.begin(); argIter != arguments.end(); ++argIter) {
        Argument * temp = new Argument((*argIter)->type.value);
        value->getArgumentList().push_back(temp);
        new StoreInst(temp, new AllocaInst((*argIter)->type.value, (*argIter)->name, lastBlock(this)), lastBlock(this));
    }
    //std::list<Statement *>::const_iterator stmtIter;
    //for (stmtIter = stmtList.begin(); stmtIter != stmtList.end(); ++stmtIter) {
    //    Instruction * temp;
    //    if ((temp = (*stmtIter)->code_gen(this))) {
    //        lastBlock(this)->getInstList().push_back(temp);
    //    }
    //}
    return value;
};

Instruction * OpNode::code_gen(const FuncDef * env) {
    if (!this) {
        return NULL;
    }
    const Instruction * leftRet = left->code_gen(env);
    const Instruction * rightRet = right->code_gen(env);
    OperatorManager::Trie * current = top.opManager[*op];
    current = current->next(*left->type);
    current = current->next(*right->type);
    if (!current) {
        error("no code_gen for this operator");
    }
    std::list<const Instruction *> temp;
    temp.push_back(leftRet);
    temp.push_back(rightRet);
    return current->callBack(temp);
};

Instruction * ConstantNumNode::code_gen(const FuncDef * env) {
    if (!this) {
        return NULL;
    }
    return NULL;
};

Instruction * VarNode::code_gen(const FuncDef * env) {
    if (!this) {
        return NULL;
    }
    return NULL;
};

Instruction * CallNode::code_gen(const FuncDef * env) {
    if (!this) {
        return NULL;
    }
    return NULL;
};

Instruction * Return::code_gen(const FuncDef * env) {
    if (!this) {
        return NULL;
    }
    return NULL;
};

Instruction * pos_call_back(const std::list<const Instruction *> & args) {
    return NULL;
};

Instruction * assign_call_back(const std::list<const Instruction *> & args) {
    return NULL;
};

Instruction * add_call_back(const std::list<const Instruction *> & args) {
    return NULL;
};

Instruction * mul_call_back(const std::list<const Instruction *> & args) {
    return NULL;
};

void code_gen() {
    std::list<const ::Type *> types;
    types.push_back(&::Type::Int);
    top.opManager.overload(Operator::Pos, types, pos_call_back);
    types.push_back(&::Type::Int);
    top.opManager.overload(Operator::Assign, types, assign_call_back);
    top.opManager.overload(Operator::Add, types, add_call_back);
    top.opManager.overload(Operator::Mul, types, mul_call_back);

    top.code_gen()->dump();
};
