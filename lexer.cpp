#include "misc.hpp"
#include "lexer.hpp"
#include <cstdio>

#define STRING_END -2

struct dict_tree;
extern std::vector<token_type> tokens;
static dict_tree * punc_map;

struct dict_tree{
	dict_tree * out[256];
	Token data;

	dict_tree(){
		data = tok_invalid;
		for (unsigned int i = 0; i < 256; i++){
			out[i] = NULL;
		}
	};

	void set(const char * str, Token _data){
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

int get_escaped_char(FILE * fin, int end){
	int now;
	now = fgetc(fin);
	if (now == end){
		return STRING_END;
	}
	if (now == '\\'){
		now = fgetc(fin);
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

void init_punc(){
	punc_map = new dict_tree();
	punc_map->set("+", tok_punc_plus);
	punc_map->set("-", tok_punc_minus);
	punc_map->set("*", tok_punc_star);
	punc_map->set("/", tok_punc_slash);
	punc_map->set("=", tok_punc_equ);
	punc_map->set("==", tok_punc_equequ);
	punc_map->set("%", tok_punc_mod);
	punc_map->set("~", tok_punc_tilde);
	punc_map->set("&", tok_punc_amp);
	punc_map->set("&&", tok_punc_ampamp);
	punc_map->set("|", tok_punc_pipe);
	punc_map->set("||", tok_punc_pipepipe);
	punc_map->set("^", tok_punc_caret);
	punc_map->set("!", tok_punc_exclaim);
	punc_map->set("<", tok_punc_less);
	punc_map->set("<=", tok_punc_lessequ);
	punc_map->set(">", tok_punc_greater);
	punc_map->set(">=", tok_punc_greaterequ);
	punc_map->set("!=", tok_punc_exclaimequ);
	punc_map->set("{", tok_punc_lbrace);
	punc_map->set("}", tok_punc_rbrace);
	punc_map->set("(", tok_punc_lparen);
	punc_map->set(")", tok_punc_rparen);
	punc_map->set("[", tok_punc_lsquare);
	punc_map->set("]", tok_punc_rsquare);
	punc_map->set(".", tok_punc_dot);
	punc_map->set(",", tok_punc_comma);
	punc_map->set("//", tok_punc_slashslash);
	punc_map->set("/*", tok_punc_slashstar);
};

void get_tokens(FILE * fin){
	init_punc();
	int last = ' ';
	while (1){
		tokens.push_back(token_type());

		Token & cur_token = tokens.back().tok;
		std::string & cur_token_str = tokens.back().str;

		while (isspace(last) || last == ';'){
			last = fgetc(fin);
		}
		if (isalpha(last) || last == '_'){ // identifier: [a-zA-Z_][a-zA-Z0-9_]*
			cur_token_str = last;
			while (isalnum(last = fgetc(fin)) || last == '_'){
				cur_token_str += last;
			}
			if (cur_token_str == "if"){
				cur_token = tok_kw_if;
			} else if (cur_token_str == "while"){
				cur_token = tok_kw_while;
			} else {
				cur_token = tok_identifier;
			}
		}else if (isdigit(last) || last == '\''){ // number: ([0-9]+)|('[^']')
			if (isdigit(last)){
				cur_token_str = last;
				while (isdigit(last = fgetc(fin))){
					cur_token_str += last;
				}
			}else{
				last = get_escaped_char(fin, '\'');
				if (last == STRING_END){
					error("empty character constant");
				}
				int last_temp;
				last_temp = last;
				last = get_escaped_char(fin, '\'');
				if (last != STRING_END){
					warn("multi-character character constant");
					do{
						last_temp = last;
						last = get_escaped_char(fin, '\'');
						if (last == EOF){
							error("missing terminating \' character");
						}
					}while (last != STRING_END);
				}
				char temp[4];
				sprintf(temp, "%d", last_temp);
				cur_token_str = std::string(temp);
				last = fgetc(fin);
			}
			cur_token = tok_const_num;
		}else if (last == '\"'){ // string: "[^"]*"
			cur_token_str = "";
			while (1){
				last = get_escaped_char(fin, '\"');
				if (last == EOF){
					error("missing terminating \" character");
				}
				if (last == STRING_END){
					break;
				}
				cur_token_str += last;
			}
			cur_token = tok_const_str;
			last = fgetc(fin);
		}else if (last == EOF){
			cur_token = tok_eof;
			break;
		}else{
			dict_tree * now = punc_map;
			while (now->out[last]){
				now = now->out[last];
				last = fgetc(fin);
			}
			switch (now->data){
			case tok_punc_slashslash:
				while (last != '\n' && last != '\r'){
					last = fgetc(fin);
				}
				tokens.pop_back();
				break;
			case tok_punc_slashstar:
				do {
					while (last != '*'){
						last = fgetc(fin);
					}
					last = fgetc(fin);
				} while (last != '/');
				last = fgetc(fin);
				tokens.pop_back();
				break;
			default:
				cur_token = now->data;
			}
		}
	}
};
