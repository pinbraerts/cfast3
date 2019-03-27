#ifndef CFAST_RULE_HPP
#define CFAST_RULE_HPP

#include "Parser.hpp"
#include "includes.hpp"

template<class T>
auto rule_move(T&& t) {
	return std::move(t);
}

template<class T>
inline auto rule_cast(T t) {
	static_assert("Type is not supported!");
}

template<>
inline auto rule_cast(char c) {
	return [c](Parser& p) -> ErrorProcessor& {
		if (!isspace(c)) {
			p.stream >> std::ws;
		}
		int x = p.stream.peek();
		if (x == c) {
			p.stream.get();
			return p;
		}
		if (x == EOF)
			return p << Code::eof;
		return p << false << "Char mismatch, expected: '" << c << "', found: '" << (char)x << '\'';
	};
}

template<>
inline auto rule_cast(bool b) {
	return [b](Parser& p) -> ErrorProcessor& {
		return p << b << "Bool rule";
	};
}

template<class T> auto rule_move(T& t) {
	return [&t](Parser& p) -> ErrorProcessor& {
		p.stream >> t;
		return p;
	};
}
template<> auto rule_move(char&& c) {
	return rule_cast(c);
}
template<> auto rule_move(bool&& b) {
	return rule_cast(b);
}

#endif // !CFAST_RULE_HPP
