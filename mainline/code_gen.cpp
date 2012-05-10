#include "parse.hpp"
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Instructions.h>
#include <vector>
#include <cstdio>

#define error(s) do { fprintf(stderr, "error: %s\n", s); exit(1); } while (0)

using namespace llvm;

Module module("compiler_toy", getGlobalContext());

BasicBlock * lastBlock(const FuncDef * func) {
    return &func->value->back();
};

Module * Top::code_gen() {
    std::list<Definition *>::const_iterator it;
    for (it = defList.begin(); it != defList.end(); ++it){
        (*it)->code_gen(0);
    }
    return &module;
};

Value * VarDef::code_gen(const FuncDef * env) {
    Value * ret;
    if (env) {
        ret = new AllocaInst(type.value, name, lastBlock(env));
    } else {
        GlobalVariable * temp;
        module.getGlobalList().push_back(temp = new GlobalVariable(type.value, false, llvm::GlobalVariable::CommonLinkage, 0, name));
        ret = temp;
    }
    return ret;
};

Value * FuncDef::code_gen(const FuncDef * env) {
    value = Function::Create(FunctionType::get(type.value, /*?*/false), Function::ExternalLinkage, name, &module);
    BasicBlock::Create(getGlobalContext(), "init", value);
    std::list<VarDef *>::const_iterator iter;
    for (iter = arguments.begin(); iter != arguments.end(); ++iter) {
        Argument * temp = new Argument((*iter)->type.value);
        value->getArgumentList().push_back(temp);
        new StoreInst(temp, new AllocaInst((*iter)->type.value, (*iter)->name, lastBlock(this)), lastBlock(this));
    }
    return value;
};

Value * Return::code_gen(const FuncDef * env) {
    return NULL;
};

Value * CallNode::code_gen(const FuncDef * env) {
    return NULL;
};

Value * VarNode::code_gen(const FuncDef * env) {
    return NULL;
};

Value * ConstantNumNode::code_gen(const FuncDef * env) {
    return NULL;
};

Value * Expr::code_gen(const FuncDef * env) {
    return NULL;
};

void code_gen(Top & top) {
    //FuncDef * temp = new FuncDef(Type::intType, "", add_code_gen); // add
    //temp->arguments.push_back(new VarDef(Type::intType, "a"));
    //temp->arguments.push_back(new VarDef(Type::intType, "b"));
    //top.opManager.override(Operator::opAdd, temp);
    //top.opManager._map[&Operator::opAdd].next(Type::intType)->next(Type::intType)->data->code_gen();
    top.code_gen()->dump();
};
