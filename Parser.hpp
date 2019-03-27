#ifndef CFAST_PARSER_HPP
#define CFAST_PARSER_HPP

#include "includes.hpp"
#include "Error.hpp"

class Parser: public ErrorProcessor {
public:
	std::istream& stream;

	explicit Parser(std::istream& _stream) noexcept : stream(_stream) {}

	operator std::istream&() {
		return stream;
	}
	operator const std::istream&() const {
		return stream;
	}

	template<class T>
	Parser& operator>>(T&& rule) {
		check();
		auto f = rule_move(std::move(rule));
		return static_cast<Parser&>(f(*this));
	}
};

#endif // !CFAST_PARSER_HPP
