#ifndef CFAST_LEXER_HPP
#define CFAST_LEXER_HPP

#include "Token.hpp"

namespace cf {

struct LexerTraits {
	Token::Type get_type(char_type chr) {
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

	bool joinable(char_type chr) {
		switch (chr) {
		case ' ': case '\t':
			return true;

		case '+': case '-':
		case '*': case '/': case '%':
		case '<': case '>':
		case '&': case '|':
		case '~': case '^':
		case '!': case '=':
		case ':':
			return true;

		case '.': case ',':
		case ';':
		case '@': case '$':
		case '#': case '?':
		case '\\':
			return false;

		case '\'': case '"': case '`':
			return true;

		case '(': case '{': case '[':
			return false;

		case ')': case '}': case ']':
			return false;

		case '\n': case '\r':
			return false;

		case '\0': return false;
		default: return true;
		}
	}
};

struct Lexer {
private:
	std::unique_ptr<char_type[]> _buffer;
	LexerTraits traits;

	Lexer(char_type* buffer, size_t size) noexcept: current(buffer), source(buffer, buffer + size), _buffer(buffer) {}

public:
	TextPosition current;
	Source source;

	Token next() noexcept {
		if (current >= source.end())
			return Token::eof();

		Token x(traits.get_type(current.chr()), current, current);
		++current;
		if (!traits.joinable(current.chr())) {
			x.end(current);
			return x;
		}

		for (; current < source.end() && traits.get_type(current.chr()) == x.type; ++current);
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
