#pragma once
#include "graph.h"
struct labels {
	enum label_type {
		loop_begin,
		loop_end,
		for_suf,
		for_begin,
		if_end,
		else_end,
		user,
		switch_end,
		case_end,
		fail
	};
	labels & operator=(labels const&) = default;
	labels(labels const&) = default;
	labels() = default;
	using label = std::pair<int, label_type >;
	std::vector<label> vec_label;
	auto begin() {
		return vec_label.rbegin();
	}
	auto end() {
		return vec_label.rend();
	}
	auto begin() const{
		return vec_label.crbegin();
	}
	auto end() const{
		return vec_label.crend();
	}
	void push(label_type ty, int i) {
		vec_label.emplace_back(i, ty);
	}
	label pop() {
		auto x = vec_label.back();
		vec_label.pop_back();
		return x;
	}
	label operator[](int i) const {
		return vec_label[i];
	}
	auto top_type( ) const{
		return vec_label.back().second;
	}
	label find_nearest(label_type ty) const {
		for (auto it=vec_label.rbegin();it!=vec_label.rend();it++) {
			if (it->second == ty)return *it;
		}
		return label{ -1,fail };
	}
};
struct vars {
	std::vector<int> now_pos;
	std::vector<type_token> now_type;
	vars & operator=(vars const&rhs) {
		now_pos = rhs.now_pos;
		now_type = rhs.now_type;
		return *this;
	}
	vars(vars const& rhs) {
		now_pos = rhs.now_pos;
		now_type = rhs.now_type;
	}
	vars() = default;
	void push_id(int now) {
		now_pos.push_back(now);
		now_type.push_back(type_token::identifier);
	}
	void push_str(int now) {
		now_pos.push_back(now);
		now_type.push_back(type_token::string_literal);
	}
	void push_char(int now) {
		now_pos.push_back(now);
		now_type.push_back(type_token::char_literal);
	}
	void push_int(int now) {
		now_pos.push_back(now);
		now_type.push_back(type_token::int_literal);
	}
	void push_type ( int now) {
		now_pos.push_back(now);
		now_type.push_back(type_token::type);
	}
	void push_float(int now) {
		now_pos.push_back(now);
		now_type.push_back(type_token::double_literal);
	}
	type_token now() { return now_type.back(); }
	int pop() {
		int n = now_pos.back();
		now_pos.pop_back();
		now_type.pop_back();
		return n;
	}
	int top() { return now_pos.back();}
	size_t size( ) { return now_pos.size(); }

	void debug(lex_data const& data) {
		std::cout << " --";
		for (int i = 0; i<now_pos.size(); i++) {
			if( now_type[i] ==type_token::type)
				std::cout << "[" << get_type_name(now_pos[i], data) << "] ";
			else 
				std::cout << "{" << get_name_of_now(now_pos[i], data) << "} ";
		}
		std::cout << std::endl;
	}
};
enum class quat_op {
	label ,
	jmp,
	btrue ,
	bfalse, 
	newvar ,
	func ,
	funcparam ,
	funcend, 
	structdef,
	structend,
	cblock,
	cend,
	push ,
	call,
	callend,
	ret,
	retval,
	arrayval, structval , 
	add, sub,mul, div, mod ,
	shl, shr, 
	e, ne, ge, le, l,g,
	bxor,bor,band,lor,land, 
	inc ,dec, bnot, lnot, pos, neg, 
	assign,
	initlst,initlstend,initlstitem
};
namespace assign_type {
	static constexpr int normal = 1,
		add = 2,
		sub = 3,
		mul = 4,
		div = 5,
		mod = 6,
		shl = 7,
		shr = 8,
		and = 9,
		or = 10,
		xor = 11,
		init = 12;
}


struct parser {
	using quat_info = std::array<int, 3>;
	using quat_type = std::pair<quat_op, quat_info >;
	lex_data data;

	grammar & grm;
	int quat_top;
	vars stack;
	labels label_stack;
	std::map<int, std::pair<int, int> > quat_history;
	std::vector<quat_type> quats;
	std::stack<vars> rec_stack_vars;
	std::stack<labels> rec_stack_labels;
	void add_quat(quat_op, int, int, int);
	void return_to(int);
	void new_history(int);
	int push_tmp_var();
	int alloc_tmp_var();
	int push_label(labels::label_type ty);
	void push_label(int i, labels::label_type ty);
	int pop_label();
	int tmp_var_cnt;
	int	label_cnt;
	int input_data_cnt;
	int action_called_cnt;
	void binocular_operator(std::string const& op_name, quat_op op,
		std::initializer_list<type_token> && ban);
	void check_contain(std::string const& op_name,std::initializer_list<type_token> && lst);
	void check_not(std::string const& op_name, type_token x);
	void monocular_operator(std::string const& op_name, quat_op op,
		std::initializer_list<type_token> && ban);
	void assign_operator(int ass_type);
	void check_find_label(labels::label const& lb, std::string const& msg);
	parser(lex_data const& d, grammar &g);
	void show_quats();
	void parse() {
		grm.parse_ll1(data);
	}

};