#include "common.hpp"
#include "misc.hpp"
#include "parser.hpp"

#include <llvm/Function.h>
//#include <llvm/Analysis/Verifier.h>
//#include <llvm/ExecutionEngine/GenericValue.h>
//#include <llvm/ExecutionEngine/JIT.h>
//#include <llvm/DerivedTypes.h>
#include <llvm/Support/IRBuilder.h>
//#include <llvm/Support/raw_ostream.h>

#include <cstdio>
#include <map>

void Top::emit_target(){
};

void Statements::emit_target(){
};

void Var_def::emit_target(){
};

void Func_def::emit_target(){
};

void Factor_const_num::emit_target(){
};

void Factor_const_str::emit_target(){
};

void Factor_var::emit_target(){
};

void Factor_call::emit_target(){
};

void Binary_op::emit_target(){
};

void If_block::emit_target(){
};

void While_block::emit_target(){
};

