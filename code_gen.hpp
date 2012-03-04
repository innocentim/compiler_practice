#ifndef __CODEGEN_H__
#define __CODEGEN_H__

#include "AST.hpp"
#include <llvm/Function.h>
#include <string>
#include <map>

class CGContext{
public:
    CGContext * father;
    llvm::Function * func;
    std::map<std::string, llvm::Value*> table;
    std::map<std::string, Definition*> defTable;
    CGContext(CGContext * father, llvm::Function * func) : father(father), func(func) {};
};

#endif
