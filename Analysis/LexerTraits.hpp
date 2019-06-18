#ifndef CFAST_LEXER_TRAITS_HPP
#define CFAST_LEXER_TRAITS_HPP

#include "Token.hpp"

namespace cf {

struct LexerTraits {
	Token::Type get_type(char_type chr) noexcept {
		switch (chr) {
		case ' ': case '\t':
			return Token::Space;

		case '+': case '-':
		case '*': case '/': case '%':
		case '<': case '>':
		case '&': case '|':
		case '~': case '^':
		case '!': case '=':
		case ':':
			return Token::Operator;

		case '.': case ',':
		case ';':
		case '@': case '$':
		case '#': case '?':
		case '\\':
			return Token::Operator;

		case '\'': case '"': case '`':
			return Token::Quote;

		case '(': case '{': case '[':
			return Token::OpenBrace;

		case ')': case '}': case ']':
			return Token::CloseBrace;

		case '\n': case '\r':
			return Token::Line;

		case '\0': return Token::End;
		default: return Token::String;
		}
	}

	std::set<std::string_view> possible_combinations {
		"::", "->",
		"--", "++",
		"<<", ">>",
		"<=>", "<=", ">=", "==", "!=",
		"&&", "||",
		"+=", "-=", "*=", "/=", "%=", ">>=", "<<=", "&=", "|=", "^="
	};

	enum Result {
		Nothing,
		Start,
		Combination
	};

	Result Match(const Token& tok) {
		if (tok.type == Token::String)
			return Combination;
		std::string s (tok.view());
		auto iter = possible_combinations.lower_bound(s);
		if (iter == possible_combinations.end()) return Nothing;
		std::string s2 (*iter);
		if (s2.find_first_of(s) == 0) {
			return s2.size() == s.size() ? Combination : Start;
		}
		return Nothing;
	}
};

} // namespace cf

#endif // !CFAST_LEXER_TRAITS_HPP
