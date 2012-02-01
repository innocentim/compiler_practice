#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <vector>
#include <string>
#include "common.hpp"


struct token_type{
	Token tok;
	std::string str;
	
	void print();
};

void init_punc();

void get_tokens(const char *);

extern std::vector<token_type> tokens;

#endif
