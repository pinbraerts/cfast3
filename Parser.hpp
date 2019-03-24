#ifndef CFAST_PARSER_HPP
#define CFAST_PARSER_HPP

#include "Rule.hpp"
#include "Error.hpp"

class Parser: public ErrorProcessor {
public:
	std::istream& stream;

	explicit Parser(std::istream& _stream) noexcept : stream(_stream) {}

	Parser& operator>>(Rule& rule) {
		check();
		return static_cast<Parser&>(rule.apply(*this));
	}
	Parser& operator>>(Rule&& rule) {
		check();
		return static_cast<Parser&>(rule.apply(*this));
	}
	Parser& operator>>(RulePtr&& ptr) {
		check();
		return static_cast<Parser&>(ptr->apply(*this));
	}
	Parser& operator>>(RulePtr& ptr) {
		check();
		return static_cast<Parser&>(ptr->apply(*this));
	}
};

#endif // !CFAST_PARSER_HPP
