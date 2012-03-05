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
static Instruction::BinaryOps op_map[SLASHSLASH];

static Type * getType(const Identifier & type, bool void_permit){
	if (type == "int"){
		return Type::getInt64Ty(getGlobalContext());
	} else if (type == "string"){
		return Type::getInt8PtrTy(getGlobalContext());
	} else if (type == "void"){
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

Value * FactorVar::codeGen(CGContext * context){
	CGContext * cur = context;
	while (cur != NULL){
		if (cur->table.count(str) == 0){
			cur = cur->father;
			continue;
		}
		type = cur->defTable[str]->type;
		return new LoadInst(cur->table[str], "", false, &context->func->getBasicBlockList().back());
	}
	error("variable undefined");
	return NULL;
};

Value * FactorNum::codeGen(CGContext * context){
	type = "int";
	return ConstantInt::get(Type::getInt64Ty(getGlobalContext()), value, true);
};

Value * FactorStr::codeGen(CGContext * context){
	type = "string";
	Constant * temp = ConstantDataArray::getString(getGlobalContext(), str, true);
	Constant * s = new GlobalVariable(*module, temp->getType(), true, GlobalValue::PrivateLinkage, temp, ".str");
	std::vector<Value*> idxList;
	idxList.push_back(ConstantInt::get(Type::getInt64Ty(getGlobalContext()), 0, true));
	idxList.push_back(ConstantInt::get(Type::getInt64Ty(getGlobalContext()), 0, true));
	Value * ret = ConstantExpr::getGetElementPtr(s, idxList, true);
	return ret;
};

Value * FactorCall::codeGen(CGContext * context){
	CGContext * cur = context;
	while (cur != NULL){
		if (cur->table.count(name) == 0){
			cur = cur->father;
			continue;
		}
		std::vector<Value*> v_args;
		ExprList::const_iterator it, e;
		for (it = args.begin(), e = args.end(); it != e; ++it){
			v_args.push_back((**it).codeGen(context));
		}
		type = cur->defTable[name]->type;
		return CallInst::Create(cur->table[name], v_args, "", &context->func->getBasicBlockList().back());
	}
	error("variable undefined");
	return NULL;
};

Value * BinaryOp::codeGen(CGContext * context){
	Value * l = left.codeGen(context);
	Value * r = right.codeGen(context);
	if (left.type != right.type){
		error("type not match");
	}
	type = left.type;
	return BinaryOperator::Create(op_map[op], l, r, "", &context->func->getBasicBlockList().back());
};

Value * Assignment::codeGen(CGContext * context){
	CGContext * cur = context;
	while (cur != NULL){
		if (cur->table.count(lvalue) == 0){
			cur = cur->father;
			continue;
		}
		Value * l = cur->table[lvalue];
		Value * r = rvalue.codeGen(context);
		if (cur->defTable[lvalue]->type != rvalue.type){
			error("type not match");
		}
		new StoreInst(r, l, false, &context->func->getBasicBlockList().back());
		type = rvalue.type;
		return r;
	}
	error("variable undefined");
	return NULL;
};

Value * VarDef::codeGen(CGContext * context){
	if (context->table.count(name) != 0){
		error("duplicated definition");
	}
	Value * ret;
	context->defTable[name] = this;
	if (context->father != NULL){
		ret = new AllocaInst(getType(type, false), name, &context->func->getBasicBlockList().back());
		context->table[name] = ret;
		if (assign != NULL){
			Assignment temp(name, *assign);
			temp.codeGen(context);
		}
	} else {
		Constant * temp = ConstantInt::get(getType(type, false), 0, true);
		GlobalVariable * ret_temp = new GlobalVariable(*module, getType(type, false), false, GlobalValue::ExternalLinkage, temp, name);
		context->table[name] = ret_temp;
		if (assign != NULL){
			Value * rvalue = assign->codeGen(context);
			if (Constant::classof(rvalue)){
				ret_temp->setInitializer((Constant*)rvalue);
			} else {
				error("initializer element is not a compile-time constant");
			}
		}
		ret = ret_temp;
	}
	return ret;
};

Value * FuncDef::codeGen(CGContext * context){
	if (context->table.count(name) != 0){
		error("duplicated definition");
	}
	std::vector<Type*> argTypes;
	VarList::const_iterator it, e;
	for (it = args.begin(), e = args.end(); it != e; ++it){
		argTypes.push_back(getType((**it).type, false));
	}
	FunctionType * ft = FunctionType::get(getType(type, true), argTypes, false);
	Function * f = Function::Create(ft, GlobalValue::ExternalLinkage, name, module);
	context->defTable[name] = this;
	context->table[name] = f;
	CGContext * cur = new CGContext(context, f);
	BasicBlock::Create(getGlobalContext(), "", f, NULL);
	Function::ArgumentListType::iterator ait;
	for (it = args.begin(), ait = f->getArgumentList().begin(), e = args.end(); it != e; ++it, ++ait){
		new StoreInst(ait, (**it).codeGen(cur), false, &f->getBasicBlockList().back());
	}
	body.codeGen(cur);
	ReturnInst::Create(getGlobalContext(), &f->getBasicBlockList().back());
	return NULL;
};

Value * ExprStmt::codeGen(CGContext * context){
	return expr.codeGen(context);
};

Value * Return::codeGen(CGContext * context){
	return ReturnInst::Create(getGlobalContext(), ret->codeGen(context), &context->func->getBasicBlockList().back());
};

void initCodeGen(){
	module->setTargetTriple("x86_64-unknown-linux-gnu");
	op_map[PLUS] = Instruction::Add;
	op_map[MINUS] = Instruction::Sub;
	op_map[STAR] = Instruction::Mul;
	op_map[SLASH] = Instruction::SDiv;
};

void codeDump(){
	module->dump();
};
