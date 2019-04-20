#ifndef CFAST_TOKEN_HPP
#define CFAST_TOKEN_HPP

#include "Source.hpp"

namespace cf {

struct Token: Source {
	enum Type {
		End,
		Space,
		Line,
		Operator,
		SingleOperator,
		String,
		Quote,
		OpenBrace,
		CloseBrace,
		Container,
		Error,
	} type;

	Token(Type t): type(t) {}
	Token(Type t, TextPosition first, TextPosition last) : type(t), Source(first, last) {}

	static Token eof() {
		return Token(Type::End);
	}

	constexpr static Type get_type(char_type chr) {
		switch (chr) {
		case ' ': case '\t':
			return Type::Space;

		case '+': case '-':
		case '*': case '/': case '%':
		case '<': case '>':
		case '&': case '|':
		case '~': case '^':
		case '!': case '=':
			return Type::Operator;

		case '.': case ',':
		case ':': case ';':
		case '@': case '$':
		case '#': case '?':
		case '\\':
			return Type::SingleOperator;

		case '\'': case '"': case '`':
			return Type::Quote;

		case '(': case '{': case '[':
			return Type::OpenBrace;

		case ')': case '}': case ']':
			return Type::CloseBrace;

		case '\n': case '\r':
			return Type::Line;

		case '\0': return Type::End;
		default: return Type::String;
		}
	}

	constexpr static const char* type2str(Type type) {
		switch (type) {
		case Type::OpenBrace: case Type::CloseBrace:
			return "Brace";
		case Type::End: return "End";
		case Type::Line: return "Line";
		case Type::SingleOperator: case Type::Operator:
			return "Operator";
		case Type::Quote: return "Quote";
		case Type::Space: return "Space";
		case Type::String: return "String";
		case Type::Container: return "Container";
		case Type::Error: return "Error";
		default: return "Unknown";
		}
	}

	void save_binary(std::ostream& stream) {
		write(stream, *this);
	}
	void read_binary(std::istream& stream) {
		read(stream, *this);
	}
};

std::ostream& operator<<(std::ostream& stream, const Token& token) {
	return (stream << Token::type2str(token.type) << " \'").write(token.c_str(), token.size()) << '\'';
}

} // namespace cf

#endif // !CFAST_TOKEN_HPP
