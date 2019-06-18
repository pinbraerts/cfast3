#ifndef CFAST_LEXER_HPP
#define CFAST_LEXER_HPP

#include "LexerTraits.hpp"

namespace cf {

struct Lexer {
private:
	std::unique_ptr<char_type[]> _buffer;
	LexerTraits traits;

	Lexer(char_type* buffer, size_t size) noexcept: current(buffer), source(buffer, buffer + size), _buffer(buffer) {}

public:
	TextPosition current;
	Source source;

	Token Next() noexcept {
		if (current >= source.end())
			return Token::eof();

		Token x(traits.GetType(current.chr()), current, current);
		x.end(++current);
		Token temp = x;

		while (current < source.end() && x.type == traits.GetType(current.chr())) {
			temp.end(++current);
			switch (traits.Match(temp)) {
			case LexerTraits::Combination:
				x = temp;
				break;
			case LexerTraits::Start:
				break;
			case LexerTraits::Nothing:
			default:
				current = x.end();
				return x;
			}
		}

		return x;
	}

	static Lexer FromFile(std::string path) {
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

	static Lexer FromString(std::string str) {
		if (str.empty()) return Lexer(nullptr, 0);
		char_type* buffer = new char_type[str.size() + 1];
		str.copy(buffer, str.size());
		buffer[str.size()] = '\0';
		return Lexer(buffer, str.size());
	}

	struct Iter {
		Iter(Lexer& l) noexcept: lex(l), tok(lex.Next()) {}
		Iter(Lexer& l, Token t) noexcept: lex(l), tok(t) {}

		Lexer& lex;
		Token tok;

		Iter& operator++() noexcept {
			tok = lex.Next();
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
