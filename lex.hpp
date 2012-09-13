#ifndef __LEX_HPP__
#define __LEX_HPP__

enum tokenType {
	IDENTIFIER,
	CONSTANT_INT,
	KWD_RETURN,
	LPAREN, // (
	RPAREN, // )
	LBRACE, // {
	RBRACE, // }
	EQU, // =
	PLUS, // +
	STAR, // *
	COMMA, // ,
	SLASHSLASH, // //
	_EOF
};

class Token {
	int token;
public:
	void * data;
	Token(int token = -1, void * data = 0) : token(token), data(data) {};
	operator int() {
		return token;
	};
};
#endif
