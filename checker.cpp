#include <cstdio>

#include "misc.hpp"
#include "common.hpp"
#include "checker.hpp"

Context::Context(Context * _father) : father(_father) {};

Context * global = new Context(NULL);

void Top::check(Context * cur){
	for (unsigned int i = 0, e = defs.size(); i < e; i++){
		defs[i]->context_register(global);
	}
	for (unsigned int i = 0, e = defs.size(); i < e; i++){
		defs[i]->check(global);
	}
};

void Statements::check(Context * cur){
	for (unsigned int i = 0, e = stmts.size(); i < e; i++){
		stmts[i]->context_register(global);
		stmts[i]->check(global);
	}
};

void Expr::context_register(Context * cur){};
void If_block::context_register(Context * cur){};
void While_block::context_register(Context * cur){};

std::string & Var_def::get_name(){
	return name;
};

prim_type Var_def::get_type(){
	if (type == "int"){
		return type_int;
	} else if (type == "string"){
		return type_str;
	}
	return type_invalid;
};

std::string & Func_def::get_name(){
	return ret_var->name;
};

prim_type Func_def::get_type(){
	return ret_var->get_type();
};

void Var_def::context_register(Context * cur){
	if (type != "int" && type != "string"){
		error("type error");
	}
	if (cur->str_tbl.count(get_name()) && cur->str_tbl.find(get_name())->second != this){
		error("duplicated definitions");
	} else {
		cur->str_tbl.insert(std::make_pair(get_name(), this));
	}
};

void Func_def::context_register(Context * cur){
	if (cur->str_tbl.count(get_name()) && cur->str_tbl.find(get_name())->second != this){
		error("duplicated definitions");
	} else {
		cur->str_tbl.insert(std::make_pair(get_name(), this));
	}
};

void Var_def::check(Context * cur){
};

void Func_def::check(Context * cur){
	Context * sub = new Context(cur);
	if (ret_var->type != "void"){
		ret_var->context_register(sub);
	}
	for (unsigned int i = 0, e = args.size(); i < e; i++){
		args[i]->context_register(sub);
	}
	stmts->check(sub);
};

prim_type Factor_const_num::_check(Context * cur){
	return type_int;
};

void Factor_const_num::check(Context * cur){
	_check(cur);
};

prim_type Factor_const_str::_check(Context * cur){
	return type_str;
};

void Factor_const_str::check(Context * cur){
	_check(cur);
};

prim_type Factor_var::_check(Context * cur){
	while (cur != NULL){
		if (!cur->str_tbl.count(name)){
			cur = cur->father;
			continue;
		}
		return cur->str_tbl[name]->get_type();
	}
	error("var undefined");
	return type_invalid;
};

void Factor_var::check(Context * cur){
	_check(cur);
};

prim_type Factor_call::_check(Context * cur){
	while (cur != NULL){
		if (!cur->str_tbl.count(name)){
			cur = cur->father;
			continue;
		}
		Func_def * F = (Func_def*)(cur->str_tbl[name]);
		if (args.size() != F->args.size()){
			error("number of arguments error");
		}
		for (unsigned int i = 0, e = args.size(); i < e; i++){
			if (cur->str_tbl[args[i]]->get_type() != F->args[i]->get_type()){
				error("type check error");
			}
		}
		return cur->str_tbl[name]->get_type();
	}
	return type_invalid;
};

void Factor_call::check(Context * cur){
	_check(cur);
};

prim_type Binary_op::_check(Context * cur){
	if (left->_check(cur) != right->_check(cur)){
		error("type check error");
	}
	return left->_check(cur);
};

void Binary_op::check(Context * cur){
	_check(cur);
};

void If_block::check(Context * cur){
	Context * sub = new Context(cur);
	stmts_true->check(sub);
	stmts_false->check(sub);
};

void While_block::check(Context * cur){
	Context * sub = new Context(cur);
	stmts->check(sub);
};
