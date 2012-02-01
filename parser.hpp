#ifndef __PARSER_H__
#define __PARSER_H__

#include <vector> 
#include <string>
#include "common.hpp"
#include "token.hpp"

struct Operator{
	Token tok;
	std::string str;
	unsigned int left_unary;
	unsigned int right_unary;
	unsigned int eye;
	bool left_associative;

	Operator(Token, const std::string&, unsigned int, unsigned int, unsigned int, bool);
};

void init_operator();

extern Operator * operator_map[];

#endif
