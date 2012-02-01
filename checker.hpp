#ifndef __CHECKER_H__
#define __CHECKER_H__

#include <map>
#include <string>
#include "common.hpp"

struct Context{
	Context * father;
	std::map<std::string, Definition*> str_tbl;

	Context(Context*);
};

struct Top;

extern Context * global;

#endif

