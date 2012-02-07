#include "common.hpp"
#include "parser.hpp"
#include "misc.hpp"
#include "lexer.hpp"
#include <vector>
#include <cstdlib>
#include <cstdio>

extern Operator * operator_map[];
extern std::vector<token_type> tokens;
extern std::map<std::string, Type> type_map;
static unsigned int current;
static Context * context = NULL;

void context_push(){
	context = new Context(context);
};

void context_pop(){
	context = context->father;
};

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
};

Var_def::Var_def(){
	if (type_map.count(lookahead(0).str) > 0){
		type_str = lookahead(0).str;
		type = type_map[lookahead(0).str];
	}
	eat(tok_identifier);
	name = lookahead(0).str;
	eat(tok_identifier);
	if (name != "void"){
		if (context->var_tbl.count(get_name())){
			error("duplicated var definition");
		}
		context->var_tbl.insert(std::pair<std::string, Var_def*>(get_name(), this));
	}
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

Expr * parse_factor();
Factor_call::Factor_call(){
	name = lookahead(0).str;
	eat(tok_identifier);
	eat(tok_punc_lparen);
	if (lookahead(0).tok != tok_punc_rparen){
		while (1){
			Expr * temp;
			if ((temp = parse_factor()) == NULL){
				error("factor expected");
			}
			args.push_back(temp);
			if (lookahead(0).tok == tok_punc_rparen){
				break;
			}
			eat(tok_punc_comma);
		}
	}
	eat(tok_punc_rparen);
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
	case tok_const_str:
		return new Factor_const_str();
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
		left->left->get_type();
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
			ret->get_type();
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
	Context * con_temp = context;
	context_push();
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
	if (context->func_tbl.count(get_name())){
		error("duplicated function definition");
	}
	Context * con_temp2 = context;
	context = con_temp;
	context->func_tbl.insert(std::pair<std::string, Func_def*>(get_name(), this));
	context = con_temp2;
	eat(tok_punc_lbrace);
	stmts = new Statements();
	eat(tok_punc_rbrace);
	context_pop();
};

Top::Top(){
	context_push();
	while (1){
		switch (lookahead(0).tok){
		case tok_identifier:
			if (lookahead(1).tok == tok_identifier){
				if (lookahead(2).tok == tok_punc_lparen){
					funcs.push_back(new Func_def());
				} else {
					vars.push_back(new Var_def());
				}
			} else {
				error("definition expected!");
			}
			break;
		default:
			goto _out;
		}
	}
_out:
	context_pop();
};

Statements::Statements(){
	while (1){
		switch (lookahead(0).tok){
		case tok_identifier:
			if (lookahead(1).tok == tok_identifier){
				if (lookahead(2).tok == tok_punc_lparen){
					funcs.push_back(new Func_def());
				} else {
					vars.push_back(new Var_def());
				}
			} else {
		case tok_const_num:
		case tok_const_str:
				stmts.push_back(parse_expr());
			}
			break;
		case tok_kw_if:
			stmts.push_back(new If_block());
			break;
		case tok_kw_while:
			stmts.push_back(new While_block());
			break;
		default:
			goto _out;
		}
	}
_out:
	return;
};

Type Binary_op::get_type(){
	Type ret = left->get_type();
	if (ret != right->get_type()){
		error("type check error");
	}
	return ret;
};

Type Var_def::get_type(){
	return type;
};

Type Func_def::get_type(){
	if (ret_var == NULL){
		return type_void;
	} else {
		return ret_var->get_type();
	}
};

Type Factor_const_num::get_type(){
	return type_int;
};

Type Factor_const_str::get_type(){
	return type_str;
};

Type Factor_var::get_type(){
	Context * cur = context;
	while (cur != NULL){
		if (!cur->var_tbl.count(name)){
			cur = cur->father;
			continue;
		}
		_bind = cur->var_tbl[name];
		return cur->var_tbl[name]->get_type();
	}
	error("var undefined");
	return type_invalid;
};

Type Factor_call::get_type(){
	Context * cur = context;
	while (cur != NULL){
		if (!cur->func_tbl.count(name)){
			cur = cur->father;
			continue;
		}

		Func_def * F = cur->func_tbl[name];
		if (args.size() != F->args.size()){
			error("number of arguments error");
		}
		for (unsigned int i = 0, e = args.size(); i < e; i++){
			if (args[i]->get_type() != F->args[i]->get_type()){
				error("type check error");
			}
		}
		_bind = cur->func_tbl[name];
		return cur->func_tbl[name]->get_type();
	}
	error("function can't bind");
	return type_invalid;
};
