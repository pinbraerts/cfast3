#ifndef CFAST_RULE_HPP
#define CFAST_RULE_HPP

#include "includes.hpp"

struct Rule {
	virtual Parser& apply(Parser& p) {
		return p;
	}

	virtual ~Rule() {}
};

struct RulePtr : std::unique_ptr<Rule> {
	RulePtr(char c);
	RulePtr(bool b);

	template<class T>
	RulePtr(std::unique_ptr<T>&& other) : unique_ptr(std::move(other)) {}

	RulePtr(RulePtr&) = delete;

	template<class T>
	RulePtr(T&& t);

	template<class T>
	RulePtr(T& t);
};

#endif // !CFAST_RULE_HPP
