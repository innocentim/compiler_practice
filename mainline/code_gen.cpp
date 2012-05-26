#include "parse.hpp"
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Instructions.h>
#include <llvm/Constants.h>
#include <vector>
#include <cstdio>

#define error(s) do { fprintf(stderr, "error: %s\n", s); exit(1); } while (0)

using namespace llvm;

Module module("compiler_toy", getGlobalContext());
extern Top top;

bool is_var(Value * val) {
    return AllocaInst::classof(val) || GlobalVariable::classof(val);
};

BasicBlock * last_block(const FuncDef * func) {
    return &func->value->back();
};

Module * Top::code_gen() {
    {
        std::map<Identifier, VarDef *>::const_iterator iter;
        for (iter = varManager.begin(); iter != varManager.end(); ++iter) {
            iter->second->code_gen(NULL);
        }
    }
    {
        std::map<Identifier, FuncDef *>::const_iterator iter;
        for (iter = funcManager.begin(); iter != funcManager.end(); ++iter) {
            if (!iter->second->value) {
                iter->second->code_gen(NULL);
            }
        }
    }
    return &module;
};

Value * VarDef::code_gen(const FuncDef * env) {
    if (env) {
        if (!value) {
            value = new AllocaInst(type.value, name, last_block(env));
        }
    } else {
        GlobalVariable * temp;
        module.getGlobalList().push_back(temp = new GlobalVariable(type.value, false, GlobalVariable::CommonLinkage, Constant::getNullValue(type.value), name));
        value = temp;
    }
    return value;
};

Function * FuncDef::code_gen(const FuncDef * env) {
    if (declare) {
        std::vector<llvm::Type *> temp;
        std::list<VarDef *>::const_iterator argIter;
        for (argIter = arguments.begin(); argIter != arguments.end(); ++argIter) {
            temp.push_back((*argIter)->type.value);
            //Argument * temp = new Argument((*argIter)->type.value);
            //value->getArgumentList().push_back(temp);
            //(*argIter)->value = temp;
        }
        value = Function::Create(FunctionType::get(type.value, temp, false), Function::ExternalLinkage, name, &module);
        return value;
    }
    value = Function::Create(FunctionType::get(type.value, /*?*/false), Function::ExternalLinkage, name, &module);
    BasicBlock::Create(getGlobalContext(), "", value);
    std::map<Identifier, VarDef *>::const_iterator localIter;
    for (localIter = varManager.begin(); localIter != varManager.end(); ++localIter) {
        VarDef * temp = localIter->second;
        temp->code_gen(this);
    }
    std::list<VarDef *>::const_iterator argIter;
    for (argIter = arguments.begin(); argIter != arguments.end(); ++argIter) {
        Argument * temp2 = new Argument((*argIter)->type.value);
        value->getArgumentList().push_back(temp2);
        new StoreInst(temp2, (*argIter)->code_gen(this), last_block(this));
    }
    std::list<Statement *>::const_iterator stmtIter;
    for (stmtIter = stmtList.begin(); stmtIter != stmtList.end(); ++stmtIter) {
        (*stmtIter)->code_gen(this);
    }
    
    return value;
};

Value * OpNode::code_gen(const FuncDef * env) {
    Value * leftRet = left->code_gen(env);
    Value * rightRet = right->code_gen(env);
    OperatorManager::Trie * current = top.opManager[*op];
    current = current->next(*left->type);
    current = current->next(*right->type);
    if (!current) {
        error("no callback for this operator");
    }
    std::list<Value *> temp;
    temp.push_back(leftRet);
    temp.push_back(rightRet);
    return current->callback(env, temp, type);
};

Value * ConstantNumNode::code_gen(const FuncDef * env) {
    type = &::Type::Int;
    return ConstantInt::get(llvm::Type::getInt64Ty(getGlobalContext()), num, true);
};

Value * VarNode::code_gen(const FuncDef * env) {
    type = &var->type;
    return var->value;
};

Value * CallNode::code_gen(const FuncDef * env) {
    type = &func->type;
    std::vector<Value *> temp;
    std::list<Expr *>::const_iterator iter;
    std::list<VarDef *>::const_iterator defIter;
    for (iter = arguments.begin(), defIter = func->arguments.begin(); iter != arguments.end(); ++iter, ++defIter) {
        Value * val = (*iter)->code_gen(env);
        if ((*iter)->type != &(*defIter)->type) {
            error("type unexpected");
        }
        if (is_var(val)) {
            val = new LoadInst(val, "LoadForCall", last_block(env));
        }
        temp.push_back(val);
    }
    if (!func->value) {
        func->code_gen(NULL);
    }
    return CallInst::Create(func->value, temp, func->name, last_block(env));
};

Value * Return::code_gen(const FuncDef * env) {
    Value * val = retExpr->code_gen(env);
    if (is_var(val)) {
        val = new LoadInst(retExpr->code_gen(env), "LoadForReturn", last_block(env));
    }
    return ReturnInst::Create(getGlobalContext(), val, last_block(env));
};

Value * pos_call_back(const FuncDef *env, const std::list<Value *> & args, const ::Type *& type) {
    type = &::Type::Int;
    std::list<Value *>::const_iterator iter = args.begin();
    Value * right = *iter;
    if (is_var(right)) {
        right = new LoadInst(right, "Load", last_block(env));
    }
    return right;
};

Value * assign_call_back(const FuncDef *env, const std::list<Value *> & args, const ::Type *& type) {
    type = &::Type::Int;
    std::list<Value *>::const_iterator iter = args.begin();
    Value * left = *iter;
    Value * right = *++iter;
    if (!is_var(left)) {
        error("lvalue expected");
    }
    if (is_var(right)) {
        right = new LoadInst(right, "Load", last_block(env));
    }
    new StoreInst(right, left, last_block(env));
    return right;
};

Value * add_call_back(const FuncDef *env, const std::list<Value *> & args, const ::Type *& type) {
    type = &::Type::Int;
    std::list<Value *>::const_iterator iter = args.begin();
    Value * left = *iter;
    Value * right = *++iter;
    if (is_var(left)) {
        left = new LoadInst(left, "Load", last_block(env));
    }
    if (is_var(right)) {
        right = new LoadInst(right, "Load", last_block(env));
    }
    return BinaryOperator::Create(Instruction::Add, left, right, "", last_block(env));
};

Value * mul_call_back(const FuncDef *env, const std::list<Value *> & args, const ::Type *& type) {
    type = &::Type::Int;
    std::list<Value *>::const_iterator iter = args.begin();
    Value * left = *iter;
    Value * right = *++iter;
    if (is_var(left)) {
        left = new LoadInst(left, "Load", last_block(env));
    }
    if (is_var(right)) {
        right = new LoadInst(right, "Load", last_block(env));
    }
    return BinaryOperator::Create(Instruction::Mul, left, right, "", last_block(env));
};

void code_gen() {
    module.setTargetTriple("x86_64-linux-gnu");
    std::list<const ::Type *> types;
    types.push_back(&::Type::Int);
    top.opManager.overload(Operator::Pos, types, pos_call_back);
    types.push_back(&::Type::Int);
    top.opManager.overload(Operator::Assign, types, assign_call_back);
    top.opManager.overload(Operator::Add, types, add_call_back);
    top.opManager.overload(Operator::Mul, types, mul_call_back);

    FILE * back = stderr;
    freopen("out.ll", "w", stderr);
    top.code_gen()->dump();
    stderr = back;
    system("llc -o out.s out.ll");
    system("as -o out.o out.s");
    system("ld -o a.out -dynamic-linker /lib64/ld-linux-x86-64.so.2 /usr/lib/x86_64-linux-gnu/crt1.o /usr/lib/gcc/x86_64-linux-gnu/4.6/crtbegin.o /usr/lib/x86_64-linux-gnu/crti.o /usr/lib/gcc/x86_64-linux-gnu/4.6/crtend.o /usr/lib/x86_64-linux-gnu/crtn.o out.o -lc");
};
