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

	constexpr static const char* type2str(Type type) {
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

	void save_binary(std::ostream& stream) {
		write(stream, *this);
	}
	void load_binary(std::istream& stream) {
		read(stream, *this);
	}

	std::string_view view() const noexcept {
		return std::string_view(begin().ptr(), size());
	}
};

std::ostream& operator<<(std::ostream& stream, const Token& token) {
	return (stream << Token::type2str(token.type) << " \'").write(token.begin().ptr(), token.size()) << '\'';
}

} // namespace cf

#endif // !CFAST_TOKEN_HPP
