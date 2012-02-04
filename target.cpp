#include "common.hpp"

#include <llvm/DerivedTypes.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Operator.h>
#include <map>
#include <cstdio>

llvm::Module * global_module;
llvm::Type * type_map_l[type_invalid + 1] = {NULL};
prim_type type_ref_map_l[type_invalid + 1];


void init_target();
llvm::Module * Top::emit_target(){
	init_target();
	for (unsigned int i = 0, e = defs.size(); i < e; i++){
		defs[i]->emit_target();
	}
	global_module->dump();
	return global_module;
};

llvm::BasicBlock * Statements::emit_target(){
	return NULL;
};

llvm::Value * Var_def::emit_target(){
	return NULL;
};

llvm::Function * Func_def::emit_target(){
	std::vector<llvm::Type*> args_type;
	if (ret_var->type != type_void){
		args_type.push_back(type_map_l[type_ref_map_l[ret_var->type]]);
	}
	for (unsigned int i = 0, e = args.size(); i < e; i++){
		args_type.push_back(type_map_l[args[i]->type]);
	}
	llvm::FunctionType * FT = llvm::FunctionType::get(llvm::Type::getVoidTy(llvm::getGlobalContext()), args_type, false);
	llvm::Function * F = llvm::Function::Create(FT, llvm::Function::CommonLinkage , ret_var->name, global_module);
	return F;
};

llvm::Constant * Factor_const_num::emit_target(){
	return NULL;
};

llvm::Constant * Factor_const_str::emit_target(){
	return NULL;
};

llvm::User * Factor_var::emit_target(){
	return NULL;
};

llvm::User * Factor_call::emit_target(){
	return NULL;
};

llvm::Operator * Binary_op::emit_target(){
	return NULL;
};

llvm::Value * If_block::emit_target(){
	return NULL;
};

llvm::Value * While_block::emit_target(){
	return NULL;
};

void init_target(){
	global_module = new llvm::Module("", llvm::getGlobalContext());
	type_map_l[type_void] = llvm::Type::getVoidTy(llvm::getGlobalContext());
	type_map_l[type_int] = llvm::Type::getInt64Ty(llvm::getGlobalContext());
	type_map_l[type_str] = llvm::Type::getInt64Ty(llvm::getGlobalContext());
	type_map_l[type_int_ref] = llvm::Type::getInt64Ty(llvm::getGlobalContext());
	type_map_l[type_str_ref] = llvm::Type::getInt64Ty(llvm::getGlobalContext());
	type_ref_map_l[type_void] = type_invalid;
	type_ref_map_l[type_int] = type_int_ref;
	type_ref_map_l[type_str] = type_str_ref;
};
