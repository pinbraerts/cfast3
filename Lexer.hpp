#ifndef CFAST_LEXER_HPP
#define CFAST_LEXER_HPP

#include "Token.hpp"

namespace cf {

struct Lexer {
private:
	std::unique_ptr<char_type[]> _buffer;

	Lexer(char_type* buffer, size_t size) noexcept: current(buffer), source(buffer, buffer + size), _buffer(buffer) {}

public:
	TextPosition current;
	Source source;

	Token next() noexcept {
		if (current >= source.end())
			return Token::eof();

		Token x(Token::get_type(current.chr()), current, current);
		++current;
		if (x.type == Token::SingleOperator || x.type == Token::CloseBrace ||
			x.type == Token::OpenBrace || x.type == Token::End) {
			x.end(current);
			return x;
		}

		for (; current < source.end() && Token::get_type(current.chr()) == x.type; ++current);
		x.end(current);
		return x;
	}

	static Lexer from_file(std::string path) {
		std::ifstream input(path);
		if (!input) return Lexer(nullptr, 0);
		input.ignore(std::numeric_limits<std::streamsize>::max());
		size_t sz = input.gcount();
		input.clear();
		input.seekg(0, std::ios::beg);
		char_type* buffer = new char_type[sz + 1];
		input.read(buffer, sz);
		buffer[sz] = '\0';
		return Lexer(buffer, sz);
	}

	static Lexer from_string(std::string str) {
		if (str.empty()) return Lexer(nullptr, 0);
		char_type* buffer = new char_type[str.size() + 1];
		str.copy(buffer, str.size());
		buffer[str.size()] = '\0';
		return Lexer(buffer, str.size());
	}

	struct Iter {
		Iter(Lexer& l) noexcept: lex(l), tok(lex.next()) {}
		Iter(Lexer& l, Token t) noexcept: lex(l), tok(t) {}

		Lexer& lex;
		Token tok;

		Iter& operator++() noexcept {
			tok = lex.next();
			return *this;
		}
		Token* operator->() noexcept {
			return &tok;
		}
		bool operator<(const Iter& other) const noexcept {
			return tok.type != Token::End;
		}
		bool operator!=(const Iter& other) const noexcept {
			return tok.type != Token::End;
		}
		Token& operator*() noexcept {
			return tok;
		}
	};

	Iter begin() noexcept {
		return Iter(*this);
	}
	Iter end() noexcept {
		return Iter(*this, Token::eof());
	}
};

} // namespace cf

#endif // !CFAST_LEXER_HPP
