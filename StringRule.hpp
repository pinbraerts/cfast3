#ifndef CFAST_STRING_RULE_HPP
#define CFAST_STRING_RULE_HPP

#include "BlockRule.hpp"

template<class Start, class End>
inline auto str_literal(
	std::string& dest,
	Start&& start,
	End&& end
) {
	return block(rule_move(start), rule_move(end), rule_cast(true), [&](Parser& p) -> ErrorProcessor& {
		char c = p.stream.get();
		if (c == '\\')
			c = p.stream.get();
		if (c == EOF)
			return p << Code::eof;
		dest += (char)c;
		return p;
	});
}

template<class Start>
inline auto str_literal(
	std::string& dest,
	Start&& start
) {
	return str_literal(dest, std::ref(rule_move(start)), std::ref(rule_move(start)));
}

inline auto str_literal(std::string& dest) {
	return str_literal(dest, rule_cast('"'), rule_cast('"'));
}

template<> inline auto rule_move(std::string& s) {
	return str_literal(s);
}

#endif // !CFAST_STRING_RULE_HPP
