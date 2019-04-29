#ifndef CFAST_TRAITS_HPP
#define CFAST_TRAITS_HPP

#include "includes.hpp"
#include "Token.hpp"

namespace cf {

struct Traits {
	// https://en.cppreference.com/w/cpp/language/operator_precedence
	std::map<std::string_view, priority_t> priority_map {
		{ "::", 1 },
		{ ".", 2 }, { "->", 2 }, { "--", 2 }, { "++", 2 },
		{ "!", 3 }, { "~", 3 },
		{ "*", 4 }, { "/", 4 }, { "%", 4 },
		{ "+", 5 }, { "-", 5 },
		{ "<<", 6 }, { ">>", 6 },
		{ "<=>", 7 },
		{ "<", 8 }, { "<=", 8 }, { ">", 8 }, { ">=", 8 },
		{ "==", 9 }, { "!=", 9 },
		{ "&", 10 },
		{ "^", 11 },
		{ "|", 12 },
		{ "&&", 13 },
		{ "||", 14 },
		{ ",", 15 },
		{ "=", 16 }, { "+=", 16 }, { "-=", 16 },
		{ "*=", 16 }, { "/=", 16 }, { "%=", 16 },
		{ ">>=", 16 }, { "<<=", 16 },
		{ "&=", 16 }, { "|=", 16 }, { "^=", 16 }
	};

	std::set<std::string_view> assignment_set {
		"=", "+=", "-=", "*=", "/=", "%=", ">>=", "<<=", "&=", "^=", "|="
	};

	std::set<std::string_view> unary_set {
		"++", "--", "!", "~"
	};

	priority_t get_priority(const Token& src) const {
		if (auto iter = priority_map.find(src.view()); iter != priority_map.end())
			return iter->second;
		else return 0;
	}

	bool is_assignment(const Token& src) const {
		return assignment_set.find(src.view()) != assignment_set.end();
	}

	bool is_unary(const Token& src) const {
		return unary_set.find(src.view()) != unary_set.end();
	}
};

} // namespace cf

#endif // !CFAST_TRAITS_HPP
