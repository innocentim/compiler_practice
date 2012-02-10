#include "AST.hpp"
#include "code_gen.hpp"
#include <llvm/Constants.h>
#include <llvm/Type.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Instructions.h>
#include <cstdio>
#include <cstdlib>

#define error(str) fprintf(stderr, "error: ");fprintf(stderr, str);fprintf(stderr, "\n");exit(1)

using namespace llvm;

Module * module = new Module("compiler_practice", getGlobalContext());

static Type * getType(const Identifier & type, bool void_permit){
	if (type.str == "int"){
		return Type::getInt64Ty(getGlobalContext());
	} else if (type.str == "string"){
		return Type::getInt8PtrTy(getGlobalContext());
	} else if (type.str == "void"){
 		if (void_permit){
			return Type::getVoidTy(getGlobalContext());
		}
		error("the type of variable can't be void");
	}
	error("type undefined");
	return NULL;
};

Value * Stmts::codeGen(CGContext * context){
	StmtList::const_iterator it, e;
	for (it = stmts.begin(), e = stmts.end(); it != e; ++it){
		(**it).codeGen(context);
	}
	return NULL;
};

Value * Identifier::codeGen(CGContext * context){
	CGContext * cur = context;
	while (cur != NULL){
		if (cur->table.count(str) == 0){
			cur = cur->father;
			continue;
		}
		return new LoadInst(cur->table[str], "", false, &context->func->getBasicBlockList().back());
	}
	error("variable undefined");
	return NULL;
};

Value * FactorNum::codeGen(CGContext * context){
	return ConstantInt::get(Type::getInt64Ty(getGlobalContext()), value, true);
};

Value * FactorStr::codeGen(CGContext * context){
	return NULL;
};

Value * FactorCall::codeGen(CGContext * context){
	return NULL;
};

Value * BinaryOp::codeGen(CGContext * context){
	return NULL;
};

Value * Assignment::codeGen(CGContext * context){
	return NULL;
};

Value * VarDef::codeGen(CGContext * context){
	if (context->table.count(name.str) != 0){
		error("duplicated variable definition");
	}
	Value * ret;
	if (context->father != NULL){
		ret = new AllocaInst(getType(type, false), name.str, &context->func->getBasicBlockList().back());
	} else {
		ret = new GlobalVariable(*module, getType(type, false), false, GlobalValue::CommonLinkage, NULL, name.str);
	}
	context->table[name.str] = ret;
	if (assign != NULL){
		Assignment temp(name, *assign);
		temp.codeGen(context);
	}
	return ret;
};

Value * FuncDef::codeGen(CGContext * context){
	std::vector<Type*> argTypes;
	VarList::const_iterator it, e;
	for (it = args.begin(), e = args.end(); it != e; ++it){
		argTypes.push_back(getType((**it).type, false));
	}
	FunctionType * ft = FunctionType::get(getType(type, true), argTypes, false);
	Function * f = Function::Create(ft, GlobalValue::ExternalLinkage, name.str, module);
	CGContext * cur = new CGContext(context, f);
	BasicBlock::Create(getGlobalContext(), "", f, NULL);
	for (it = args.begin(), e = args.end(); it != e; ++it){
		(**it).codeGen(cur);
	}
	body.codeGen(cur);
	ReturnInst::Create(getGlobalContext(), &f->getBasicBlockList().back());
	return NULL;
};

Value * ExprStmt::codeGen(CGContext * context){
	return expr.codeGen(context);
};

Value * Return::codeGen(CGContext * context){
	return ReturnInst::Create(getGlobalContext(), &context->func->getBasicBlockList().back());
};

void initCodeGen(){
	module->setTargetTriple("x86-unknown-linux-gnu");
};

void codeDump(){
	module->dump();
};
