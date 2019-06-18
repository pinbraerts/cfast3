#ifndef CFAST_TOKEN_HPP
#define CFAST_TOKEN_HPP

#include "TextPosition.hpp"

namespace cf {

struct Token: Source {
	enum Type {
		End,
		Space,
		Line,
		Operator,
		String,
		Quote,
		OpenBrace,
		CloseBrace,
		Container,
		Error,
	} type;

	Token(Type t) noexcept: type(t) {}
	Token(Type t, TextPosition first, TextPosition last) noexcept: type(t), Source(first, last) {}

	static Token eof() noexcept {
		return Token(Type::End);
	}

	constexpr static const char* TypeToString(Type type) {
		switch (type) {
		case Type::OpenBrace: case Type::CloseBrace:
			return "Brace";
		case Type::End: return "End";
		case Type::Line: return "Line";
		case Type::Operator:
			return "Operator";
		case Type::Quote: return "Quote";
		case Type::Space: return "Space";
		case Type::String: return "String";
		case Type::Container: return "Container";
		case Type::Error: return "Error";
		default: return "Unknown";
		}
	}

	void SaveBinary(std::ostream& stream) {
		Write(stream, *this);
	}
	void LoadBinary(std::istream& stream) {
		Read(stream, *this);
	}
};

std::ostream& operator<<(std::ostream& stream, const Token& token) {
	return (stream << Token::TypeToString(token.type) << " \'").write(token.begin().ptr(), token.Size()) << '\'';
}

} // namespace cf

#endif // !CFAST_TOKEN_HPP
