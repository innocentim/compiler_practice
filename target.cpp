#include "common.hpp"
#include <llvm/DerivedTypes.h>
#include <llvm/Instructions.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>

static llvm::Type * type_map[type_invalid + 1] = {NULL};
static Type type_ref_map[type_invalid + 1];
static llvm::Instruction::BinaryOps op_map[tok_invalid + 1];
static llvm::Module * module;

void init_target();
llvm::Module * Top::emit_target(){
	init_target();
	module = new llvm::Module("compiler_practice", llvm::getGlobalContext());
	for (unsigned int i = 0, e = vars.size(); i < e; i++){
		vars[i]->emit_target(NULL);
	}
	for (unsigned int i = 0, e = funcs.size(); i < e; i++){
		funcs[i]->emit_target();
	}
	return module;
};

void Statements::emit_target(llvm::Function * father){
	llvm::BasicBlock * block = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", father);
	for (unsigned int i = 0, e = vars.size(); i < e; i++){
		vars[i]->emit_target(block);
	}
	for (unsigned int i = 0, e = funcs.size(); i < e; i++){
		funcs[i]->emit_target();
	}
	for (unsigned int i = 0, e = stmts.size(); i < e; i++){
		stmts[i]->emit_target(block);
	}
};

void Var_def::emit_target(llvm::BasicBlock * father){
	if (father == NULL){
		llvm_bind = new llvm::GlobalVariable(*module, type_map[type], false, llvm::GlobalValue::CommonLinkage, NULL, name);
	} else {
		llvm_bind = new llvm::AllocaInst(type_map[type], name, father);
	}
};

void Func_def::emit_target(){
	std::vector<llvm::Type*> args_type;
	if (ret_var->type != type_void){
		args_type.push_back(type_map[type_ref_map[ret_var->type]]);
	}
	for (unsigned int i = 0, e = args.size(); i < e; i++){
		args_type.push_back(type_map[args[i]->type]);
	}
	llvm::FunctionType * ft = llvm::FunctionType::get(llvm::Type::getVoidTy(llvm::getGlobalContext()), args_type, false);
	llvm_bind = llvm::Function::Create(ft, llvm::Function::ExternalLinkage , ret_var->name, module);
	stmts->emit_target(llvm_bind);
};

llvm::Value * Factor_const_num::emit_target(llvm::BasicBlock * father){
	return llvm::ConstantInt::get(type_map[type_int], value);
};

llvm::Value * Factor_const_str::emit_target(llvm::BasicBlock * father){
	return NULL;
};

llvm::Value * Factor_var::emit_target(llvm::BasicBlock * father){
	return bind->llvm_bind;
};

llvm::Value * Factor_call::emit_target(llvm::BasicBlock * father){
	return NULL;
};

llvm::Value * Binary_op::emit_target(llvm::BasicBlock * father){
	llvm::Value * l = left->emit_target(father);
	llvm::Value * r = right->emit_target(father);
	llvm::Value * ret = llvm::BinaryOperator::Create(op_map[op], l, r, "tmp", father);
	return ret;
};

llvm::Value * If_block::emit_target(llvm::BasicBlock * father){
	return NULL;
};

llvm::Value * While_block::emit_target(llvm::BasicBlock * father){
	return NULL;
};

void init_target(){
	type_map[type_void] = llvm::Type::getVoidTy(llvm::getGlobalContext());
	type_map[type_int] = llvm::Type::getInt32Ty(llvm::getGlobalContext());
	type_map[type_str] = llvm::Type::getInt8PtrTy(llvm::getGlobalContext());
	type_map[type_int_ref] = llvm::Type::getInt32PtrTy(llvm::getGlobalContext());
	type_map[type_str_ref] = llvm::Type::getInt8PtrTy(llvm::getGlobalContext());
	type_ref_map[type_void] = type_invalid;
	type_ref_map[type_int] = type_int_ref;
	type_ref_map[type_str] = type_str_ref;
	op_map[tok_punc_plus] = llvm::BinaryOperator::Add;
	op_map[tok_punc_minus] = llvm::BinaryOperator::Sub;
	op_map[tok_punc_star] = llvm::BinaryOperator::Mul;
	op_map[tok_punc_slash] = llvm::BinaryOperator::UDiv;
	//op_map[tok_punc_equ] = llvm::BinaryOperator::
};
