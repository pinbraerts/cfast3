#ifndef CFAST_TRAITS_HPP
#define CFAST_TRAITS_HPP

#include "Token.hpp"

namespace cf {

struct ParserTraits {
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
		{ "&=", 16 }, { "|=", 16 }, { "^=", 16 },
		{ ";", 17 }
	};

	std::set<std::string_view> assignment_set {
		"=", "+=", "-=", "*=", "/=", "%=", ">>=", "<<=", "&=", "^=", "|="
	};

	std::set<std::string_view> unary_set {
		"++", "--", "!", "~"
	};

	priority_t GetPriority(const Token& src) const {
		if (auto iter = priority_map.find(src); iter != priority_map.end())
			return iter->second;
		else return 0;
	}

	bool IsAssignment(const Token& src) const {
		return assignment_set.find(src) != assignment_set.end();
	}

	bool IsUnary(const Token& src) const {
		return unary_set.find(src) != unary_set.end();
	}
};

} // namespace cf

#endif // !CFAST_TRAITS_HPP

