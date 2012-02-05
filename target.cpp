#include "common.hpp"
#include <llvm/DerivedTypes.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>

static llvm::Type * type_map[type_invalid + 1] = {NULL};
static Type type_ref_map[type_invalid + 1];
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
	llvm::BasicBlock * block = llvm::BasicBlock::Create(llvm::getGlobalContext());
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
		new llvm::GlobalVariable(type_map[type], false, llvm::GlobalValue::CommonLinkage, NULL, name);
	} else {
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
	stmts->emit_target(llvm::Function::Create(ft, llvm::Function::ExternalLinkage , ret_var->name));
};

void Factor_const_num::emit_target(llvm::BasicBlock * father){
};

void Factor_const_str::emit_target(llvm::BasicBlock * father){
};

void Factor_var::emit_target(llvm::BasicBlock * father){
};

void Factor_call::emit_target(llvm::BasicBlock * father){
};

void Binary_op::emit_target(llvm::BasicBlock * father){
};

void If_block::emit_target(llvm::BasicBlock * father){
};

void While_block::emit_target(llvm::BasicBlock * father){
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
};
