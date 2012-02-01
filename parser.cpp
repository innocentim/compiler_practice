#include <cstdlib>
#include <cstdio>

#include "misc.hpp"
#include "common.hpp"
#include "token.hpp"
#include "parser.hpp"

unsigned int current = 0;
Operator * operator_map[tok_invalid + 1];

token_type lookahead(unsigned int i){
	if (current + i < tokens.size()){
		return tokens[current + i];
	}
	return tokens.back();
};

void eat(Token tok){
	if (tokens[current].tok == tok){
		current++;
	}else{
		error("expected");
	}
}

Var_def::Var_def(){
	type = lookahead(0).str;
	eat(tok_identifier);
	name = lookahead(0).str;
	eat(tok_identifier);
};

Factor_const_num::Factor_const_num(){
	value = strtod(lookahead(0).str.c_str(), 0);
	eat(tok_const_num);
};

Factor_const_str::Factor_const_str(){
	value = lookahead(0).str;
	eat(tok_const_str);
}

Factor_var::Factor_var(){
	name = lookahead(0).str;
	eat(tok_identifier);
};

Factor_call::Factor_call(){
	name = lookahead(0).str;
	eat(tok_identifier);
	eat(tok_punc_lparen);
	if (lookahead(0).tok != tok_punc_rparen){
		while (1){
			args.push_back(lookahead(0).str);
			eat(tok_identifier);
			if (lookahead(0).tok == tok_punc_rparen){
				break;
			}
			eat(tok_punc_comma);
		}
	}
	eat(tok_punc_rparen);
};

Binary_op::Binary_op(){
	op = tok_invalid;
	left = NULL;
	right = NULL;
};

Expr * parse_expr();
Expr * parse_factor(){
	Expr * ret;
	switch (lookahead(0).tok){
	case tok_identifier:
		if (lookahead(1).tok == tok_punc_lparen){
			return new Factor_call();
		}
		return new Factor_var();
	case tok_punc_lparen:
		eat(tok_punc_lparen);
		ret = parse_expr();
		eat(tok_punc_rparen);
		return ret;
	case tok_const_num:
		return new Factor_const_num();
//	case tok_const_str:
//		return new Factor_const_str();
	default:
		return NULL;
	}
};

Expr * parse_expr(){
	Binary_op * left = new Binary_op();
	if ((left->left = parse_factor()) == NULL){
		return NULL;
	}
	if (operator_map[lookahead(0).tok] == NULL){
		return left->left;
	}
	left->op = lookahead(0).tok;
	eat(left->op);
	if ((left->right = parse_factor()) == NULL){
		error("right value expected");
	}
	Binary_op * ret = left;
	Binary_op * father = NULL;
	while (1){
		Binary_op * _new = new Binary_op();
		if (operator_map[lookahead(0).tok] == NULL){
			return ret;
		}
		_new->op = lookahead(0).tok;
		eat(_new->op);
		if ((_new->right = parse_factor()) == NULL){
			error("right value expected");
		}
		int temp = operator_map[left->op]->eye - operator_map[_new->op]->eye;
		if (temp > 0 || (temp == 0 && operator_map[_new->op]->left_associative)){
			_new->left = left;
			if (father){
				father->right = _new;
			}
			left = _new;
		} else {
			father = left;
			_new->left = left->right;
			left->right = _new;
			left = _new;
		}
	}
};

If_block::If_block(){
	eat(tok_kw_if);
	eat(tok_punc_lparen);
	condition = parse_expr();
	eat(tok_punc_rparen);
	eat(tok_punc_lbrace);
	stmts_true = new Statements();
	eat(tok_punc_rbrace);
	if (lookahead(0).tok == tok_kw_else){
		eat(tok_kw_else);
		eat(tok_punc_lbrace);
		stmts_false = new Statements();
		eat(tok_punc_rbrace);
	}else{
		stmts_false = NULL;
	}
};

While_block::While_block(){
	eat(tok_kw_while);
	eat(tok_punc_lparen);
	condition = parse_expr();
	eat(tok_punc_rparen);
	eat(tok_punc_lbrace);
	stmts = new Statements();
	eat(tok_punc_rbrace);
};

Func_def::Func_def(){
	ret_var = new Var_def();
	eat(tok_punc_lparen);
	if (lookahead(0).tok != tok_punc_rparen){
		while (1){
			args.push_back(new Var_def());
			if (lookahead(0).tok == tok_punc_rparen){
				break;
			}
			eat(tok_punc_comma);
		}
	}
	eat(tok_punc_rparen);
	eat(tok_punc_lbrace);
	stmts = new Statements();
	eat(tok_punc_rbrace);
};

Top::Top(){
	Definition * temp;
	while (1){
		switch (lookahead(0).tok){
		case tok_identifier:
			if (lookahead(1).tok == tok_identifier){
				if (lookahead(2).tok == tok_punc_lparen){
					temp = new Func_def();
				} else {
					temp = new Var_def();
				}
			}
			break;
		default:
			temp = NULL;
			break;
		}
		if (temp == NULL){
			break;
		}
		defs.push_back(temp);
	}
};

Statements::Statements(){
	Statement * temp;
	while (1){
		switch (lookahead(0).tok){
		case tok_identifier:
			if (lookahead(1).tok == tok_identifier){
				temp = new Var_def();
			} else {
				temp = parse_expr();
			}
			break;
		case tok_kw_if:
			temp = new If_block();
			break;
		case tok_kw_while:
			temp = new While_block();
			break;
		default:
			temp = NULL;
			break;
		}
		if (temp == NULL){
			break;
		}
		stmts.push_back(temp);
	}
};

Operator::Operator(Token _tok, const std::string & _str, unsigned int _left_unary, unsigned int _right_unary, unsigned int _eye, bool _left_associative) : tok(_tok), str(_str), left_unary(_left_unary), right_unary(_right_unary), eye(_eye), left_associative(_left_associative){};

void op_register(Token tok, const std::string & str, unsigned int left_unary, unsigned int right_unary, unsigned int eye, bool left_associative){
	operator_map[tok] = new Operator(tok, str, left_unary, right_unary, eye, left_associative);
};

void init_operator(){
	for (unsigned int i = 0; i <tok_invalid + 1; i++){
		operator_map[i] = NULL;
	}
	op_register(tok_punc_plus, "+", 10, 0, 2, true);
	op_register(tok_punc_minus, "-", 10, 0, 2, true);
	op_register(tok_punc_star, "*", 10, 10, 3, true);
	op_register(tok_punc_slash, "/", 0, 0, 3, true);
	op_register(tok_punc_equ, "=", 0, 0, 1, false);
};
