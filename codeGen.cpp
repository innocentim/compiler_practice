#include "parse.hpp"
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Instructions.h>
#include <llvm/Constants.h>
#include <vector>
#include <cstdio>
#include <string>

#define error(s) do { fprintf(stderr, "error: %s\n", s); exit(1); } while (0)

using namespace llvm;

Module module("compiler_toy", getGlobalContext());
extern Top top;

bool isVar(Value * val) {
	return AllocaInst::classof(val) || GlobalVariable::classof(val);
};

BasicBlock * lastBlock(const FuncDef * func) {
	return &func->value->back();
};

Module * Top::codeGen() {
	for (std::map<Identifier, VarDef *>::const_iterator iter = varManager.begin(); iter != varManager.end(); ++iter) {
		iter->second->codeGen(NULL);
	}
	for (std::map<Identifier, FuncDef *>::const_iterator iter = funcManager.begin(); iter != funcManager.end(); ++iter) {
		if (!iter->second->value) {
			iter->second->codeGen(NULL);
		}
	}
	return &module;
};

Value * VarDef::codeGen(const FuncDef * env) {
	if (env) {
		if (!value) {
			value = new AllocaInst(type.value, name, lastBlock(env));
		}
	} else {
		GlobalVariable * temp;
		module.getGlobalList().push_back(temp = new GlobalVariable(type.value, false, GlobalVariable::CommonLinkage, Constant::getNullValue(type.value), name));
		value = temp;
	}
	return value;
};

Function * FuncDef::codeGen(const FuncDef * env) {
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
		temp->codeGen(this);
	}
	std::list<VarDef *>::const_iterator argIter;
	for (argIter = arguments.begin(); argIter != arguments.end(); ++argIter) {
		Argument * temp2 = new Argument((*argIter)->type.value);
		value->getArgumentList().push_back(temp2);
		new StoreInst(temp2, (*argIter)->codeGen(this), lastBlock(this));
	}
	std::list<Statement *>::const_iterator stmtIter;
	for (stmtIter = stmtList.begin(); stmtIter != stmtList.end(); ++stmtIter) {
		(*stmtIter)->codeGen(this);
	}
	
	return value;
};

Value * OpNode::codeGen(const FuncDef * env) {
	Value * leftRet = left->codeGen(env);
	Value * rightRet = right->codeGen(env);
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

Value * ConstantNumNode::codeGen(const FuncDef * env) {
	type = &::Type::Int;
	return ConstantInt::get(llvm::Type::getInt64Ty(getGlobalContext()), num, true);
};

Value * VarNode::codeGen(const FuncDef * env) {
	type = &var->type;
	return var->value;
};

Value * CallNode::codeGen(const FuncDef * env) {
	type = &func->type;
	std::vector<Value *> temp;
	std::list<Expr *>::const_iterator iter;
	std::list<VarDef *>::const_iterator defIter;
	for (iter = arguments.begin(), defIter = func->arguments.begin(); iter != arguments.end(); ++iter, ++defIter) {
		Value * val = (*iter)->codeGen(env);
		if ((*iter)->type != &(*defIter)->type) {
			error("type unexpected");
		}
		if (isVar(val)) {
			val = new LoadInst(val, "LoadForCall", lastBlock(env));
		}
		temp.push_back(val);
	}
	if (!func->value) {
		func->codeGen(NULL);
	}
	return CallInst::Create(func->value, temp, func->name, lastBlock(env));
};

Value * Return::codeGen(const FuncDef * env) {
	Value * val = retExpr->codeGen(env);
	if (isVar(val)) {
		val = new LoadInst(retExpr->codeGen(env), "LoadForReturn", lastBlock(env));
	}
	return ReturnInst::Create(getGlobalContext(), val, lastBlock(env));
};

Value * pos_call_back(const FuncDef *env, const std::list<Value *> & args, const ::Type *& type) {
	type = &::Type::Int;
	std::list<Value *>::const_iterator iter = args.begin();
	Value * right = *iter;
	if (isVar(right)) {
		right = new LoadInst(right, "Load", lastBlock(env));
	}
	return right;
};

Value * assign_call_back(const FuncDef *env, const std::list<Value *> & args, const ::Type *& type) {
	type = &::Type::Int;
	std::list<Value *>::const_iterator iter = args.begin();
	Value * left = *iter;
	Value * right = *++iter;
	if (!isVar(left)) {
		error("lvalue expected");
	}
	if (isVar(right)) {
		right = new LoadInst(right, "Load", lastBlock(env));
	}
	new StoreInst(right, left, lastBlock(env));
	return right;
};

Value * add_call_back(const FuncDef *env, const std::list<Value *> & args, const ::Type *& type) {
	type = &::Type::Int;
	std::list<Value *>::const_iterator iter = args.begin();
	Value * left = *iter;
	Value * right = *++iter;
	if (isVar(left)) {
		left = new LoadInst(left, "Load", lastBlock(env));
	}
	if (isVar(right)) {
		right = new LoadInst(right, "Load", lastBlock(env));
	}
	return BinaryOperator::Create(Instruction::Add, left, right, "", lastBlock(env));
};

Value * mul_call_back(const FuncDef *env, const std::list<Value *> & args, const ::Type *& type) {
	type = &::Type::Int;
	std::list<Value *>::const_iterator iter = args.begin();
	Value * left = *iter;
	Value * right = *++iter;
	if (isVar(left)) {
		left = new LoadInst(left, "Load", lastBlock(env));
	}
	if (isVar(right)) {
		right = new LoadInst(right, "Load", lastBlock(env));
	}
	return BinaryOperator::Create(Instruction::Mul, left, right, "", lastBlock(env));
};

void codeGen() {
	//const char * triple = "x86_64-unknown-linux-gnu";
	//module.setTargetTriple(triple);
	std::list<const ::Type *> types;
	types.push_back(&::Type::Int);
	top.opManager.overload(Operator::Pos, types, pos_call_back);
	types.push_back(&::Type::Int);
	top.opManager.overload(Operator::Assign, types, assign_call_back);
	top.opManager.overload(Operator::Add, types, add_call_back);
	top.opManager.overload(Operator::Mul, types, mul_call_back);

	freopen("out.ll", "w", stderr);
	top.codeGen()->dump();
	system("llc -o out.s out.ll");
	system("as -o out.o out.s");
	system("ld -o a.out -dynamic-linker /lib64/ld-linux-x86-64.so.2 /usr/lib/crt1.o /usr/lib/crti.o /usr/lib/crtn.o out.o -lc");
};
