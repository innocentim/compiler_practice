#ifndef __TOKEN_H__
#define __TOKEN_H__

#include "common.hpp"
#include <vector>
#include <string>
#include <cstdlib>

struct token_type{
	Token tok;
	std::string str;
};
void get_tokens(FILE *);
#endif
