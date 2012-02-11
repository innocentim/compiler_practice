#include "AST.hpp"
#include "code_gen.hpp"
#include "parser.hpp"
#include <llvm/Constants.h>
#include <llvm/Type.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Instruction.h>
#include <llvm/Instructions.h>
#include <cstdio>
#include <cstdlib>

#define error(str) fprintf(stderr, "error: ");fprintf(stderr, str);fprintf(stderr, "\n");exit(1)

using namespace llvm;

Module * module = new Module("compiler_practice", getGlobalContext());
static Instruction::BinaryOps op_map[SLASHSLASH + 1];

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
	CGContext * cur = context;
	while (cur != NULL){
		if (cur->table.count(name.str) == 0){
			cur = cur->father;
			continue;
		}
		std::vector<Value*> v_args;
		ExprList::const_iterator it, e;
		for (it = args.begin(), e = args.end(); it != e; ++it){
			v_args.push_back((**it).codeGen(context));
		}
		return CallInst::Create(cur->table[name.str], v_args, "", &context->func->getBasicBlockList().back());
	}
	error("variable undefined");
	return NULL;
};

Value * BinaryOp::codeGen(CGContext * context){
	Value * l = left.codeGen(context);
	Value * r = right.codeGen(context);
	if (l->getType() == Type::getVoidTy(getGlobalContext()) || r->getType() == Type::getVoidTy(getGlobalContext())){
		error("a procedure can't be rvalue");
	}
	if (l->getType() != r->getType()){
		error("type not match");
	}
	return BinaryOperator::Create(op_map[op], l, r, "", &context->func->getBasicBlockList().back());
};

Value * Assignment::codeGen(CGContext * context){
	CGContext * cur = context;
	while (cur != NULL){
		if (cur->table.count(lvalue.str) == 0){
			cur = cur->father;
			continue;
		}
		Value * l = cur->table[lvalue.str];
		Value * r = rvalue.codeGen(context);
		if (l->getType() != r->getType()->getPointerTo()){
			error("type not match");
		}
		return new StoreInst(r, l, false, &context->func->getBasicBlockList().back());
	}
	error("variable undefined");
	return NULL;
};

Value * VarDef::codeGen(CGContext * context){
	if (context->table.count(name.str) != 0){
		error("duplicated definition");
	}
	Value * ret;
	if (context->father != NULL){
		ret = new AllocaInst(getType(type, false), name.str, &context->func->getBasicBlockList().back());
	} else {
	Constant * temp = ConstantInt::get(Type::getInt64Ty(getGlobalContext()), 0, true);
		ret = new GlobalVariable(*module, getType(type, false), false, GlobalValue::CommonLinkage, temp, name.str);
	}
	context->table[name.str] = ret;
	if (assign != NULL){
		Assignment temp(name, *assign);
		temp.codeGen(context);
	}
	return ret;
};

Value * FuncDef::codeGen(CGContext * context){
	if (context->table.count(name.str) != 0){
		error("duplicated definition");
	}
	std::vector<Type*> argTypes;
	VarList::const_iterator it, e;
	for (it = args.begin(), e = args.end(); it != e; ++it){
		argTypes.push_back(getType((**it).type, false));
	}
	FunctionType * ft = FunctionType::get(getType(type, true), argTypes, false);
	Function * f = Function::Create(ft, GlobalValue::ExternalLinkage, name.str, module);
	context->table[name.str] = f;
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
	op_map[PLUS] = Instruction::Add;
	op_map[MINUS] = Instruction::Sub;
	op_map[STAR] = Instruction::Mul;
	op_map[SLASH] = Instruction::SDiv;
};

void codeDump(){
	module->dump();
};
