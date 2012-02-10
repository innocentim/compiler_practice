#include "AST.hpp"

llvm::Value * Identifier::codeGen(){
	return NULL;
};

llvm::Value * FactorNum::codeGen(){
	return NULL;
};

llvm::Value * FactorStr::codeGen(){
	return NULL;
};

llvm::Value * FactorCall::codeGen(){
	return NULL;
};

llvm::Value * BinaryOp::codeGen(){
	return NULL;
};

llvm::Value * Assignment::codeGen(){
	return NULL;
};

llvm::Value * VarDef::codeGen(){
	return NULL;
};

llvm::Value * FuncDef::codeGen(){
	return NULL;
};

llvm::Value * ExprStmt::codeGen(){
	return NULL;
};

