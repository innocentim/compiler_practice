#include "AST.hpp"
#include "parser.hpp"
#include <cstdio>
#include <cstdlib>

#define STRING_END -2
#define error(str) fprintf(stderr, "error: ");fprintf(stderr, str);fprintf(stderr, "\n");exit(1)

class dict_tree;
static dict_tree * punc_map;
const char * punc_dump[1024];

class dict_tree{
public:
	dict_tree * out[256];
	int data;

	dict_tree(){
		data = 0;
		for (unsigned int i = 0; i < 256; i++){
			out[i] = NULL;
		}
	};

	void set(const char * str, int _data){
		if (*str == '\0'){
			data = _data;
		} else {
			if (out[*(unsigned char *)str] == NULL){
				out[*(unsigned char *)str] = new dict_tree();
			}
			out[*(unsigned char *)str]->set(str + 1, _data);
		}
	};
};

int get_escaped_char(int end){
	int now;
	now = getchar();
	if (now == end){
		return STRING_END;
	}
	if (now == '\\'){
		now = getchar();
		switch (now){
		case '\'':
			return '\'';
		case '\"':
			return '\"';
		case 'n':
			return '\n';
		case 'r':
			return '\r';
		case 't':
			return '\t';
		default:
			error("unknown escape sequence");
		}
	}
	return now;
};

void punc_regi(const char * s, int token){
	punc_map->set(s, token);
	punc_dump[token] = s;
};

void initPunc(){
	punc_map = new dict_tree();
	punc_regi("+", PLUS);
	punc_regi("-", MINUS);
	punc_regi("*", STAR);
	punc_regi("/", SLASH);
	punc_regi("=", EQU);
	punc_regi("{", LBRACE);
	punc_regi("}", RBRACE);
	punc_regi("(", LPAREN);
	punc_regi(")", RPAREN);
	punc_regi(",", COMMA);
	punc_regi("//", SLASHSLASH);
};

int yylex(){
	static int last = ' ';
	while (isspace(last) || last == ';'){
		last = getchar();
	}
	if (isalpha(last) || last == '_'){ // identifier: [a-zA-Z_][a-zA-Z0-9_]*
		std::string & str = *(yylval.str = new std::string());
		str = last;
		while (isalnum(last = getchar()) || last == '_'){
			str += last;
		}
		if (str == "if"){
			delete yylval.str;
			return IF;
		} else if (str == "while"){
			delete yylval.str;
			return WHILE;
		} else if (str == "return"){
			delete yylval.str;
			return RETURN;
		}
		return IDENTIFIER;
	}else if (isdigit(last) || last == '\''){ // number: ([0-9]+)|('[^']')
		std::string & str = *(yylval.str = new std::string());
//		if (isdigit(last)){
			str = last;
			while (isdigit(last = getchar())){
				str += last;
			}
//		}else{
//			last = get_escaped_char('\'');
//			if (last == STRING_END){
//				error("empty character constant");
//			}
//			int last_temp;
//			last_temp = last;
//			last = get_escaped_char('\'');
//			if (last != STRING_END){
//				do{
//					last_temp = last;
//					last = get_escaped_char('\'');
//					if (last == EOF){
//						error("missing terminating \' character");
//					}
//				}while (last != STRING_END);
//			}
//			char temp[4];
//			sprintf(temp, "%d", last_temp);
//			str = std::string(temp);
//			last = getchar();
//		}
		return NUM;
	}else if (last == '\"'){ // string: "[^"]*"
		std::string & str = *(yylval.str = new std::string());
		while (1){
			last = get_escaped_char('\"');
			if (last == EOF){
				error("missing terminating \" character");
			}
			if (last == STRING_END){
				break;
			}
			str += last;
		}
		last = getchar();
		return STR;
	}else if (last == EOF){
		return 0;
	}
	dict_tree * now = punc_map;
	while (now->out[last]){
		now = now->out[last];
		last = getchar();
	}
	if (now->data != SLASHSLASH){
		yylval.token = now->data;
		return now->data;
	}
	while (last != '\n' && last != '\r'){
		last = getchar();
	}
	return yylex();
};
