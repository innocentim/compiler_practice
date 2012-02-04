#ifndef __PARSER_H__
#define __PARSER_H__

#include "common.hpp"
#include "token.hpp"
#include <map>
#include <string>

struct Operator{
	Token tok;
	std::string str;
	unsigned int left_unary;
	unsigned int right_unary;
	unsigned int eye;
	bool left_associative;

	Operator(Token _tok, const std::string & _str, unsigned int _left_unary, unsigned int _right_unary, unsigned int _eye, bool _left_associative) : tok(_tok), str(_str), left_unary(_left_unary), right_unary(_right_unary), eye(_eye), left_associative(_left_associative){};
};

struct Context{
	Context * father;
	std::map<std::string, Definition*> str_tbl;

	Context(Context * _father) : father(_father) {};
};

#endif
