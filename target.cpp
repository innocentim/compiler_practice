#include "common.hpp"
#include "misc.hpp"
#include "parser.hpp"

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>

using namespace llvm;

Module * Top::emit_target(){
	return NULL;
};

BasicBlock * Statements::emit_target(){
	return NULL;
};

Value * Var_def::emit_target(){
	return NULL;
};

Function * Func_def::emit_target(){
	return NULL;
};

Constant * Factor_const_num::emit_target(){
	return NULL;
};

Constant * Factor_const_str::emit_target(){
	return NULL;
};

User * Factor_var::emit_target(){
	return NULL;
};

User * Factor_call::emit_target(){
	return NULL;
};

User * Binary_op::emit_target(){
	return NULL;
};

Value * If_block::emit_target(){
	return NULL;
};

Value * While_block::emit_target(){
	return NULL;
};
