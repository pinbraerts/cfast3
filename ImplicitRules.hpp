#ifndef CFAST_IMPLICIT_RULES_HPP
#define CFAST_IMPLICIT_RULES_HPP

#include "Parser.hpp"

struct CharRule : Rule {
	char c;

	CharRule(char _c) noexcept : c(_c) {}

	ErrorProcessor& apply(Parser& p) override {
		if (!isspace(unsigned char(c))) {
			p.stream >> std::ws;
		}
		const int x = p.stream.peek();
		if (x == EOF)
			return p << Code::eof;
		else if (x == c) {
			p.stream.get();
			return p;
		}
		return p << false << "Expected character: '"s << c << '\'';
	}
};

template<class T>
struct FuncRule : Rule {
	T t;

	FuncRule(T&& _t) noexcept : t(_t) {}

	ErrorProcessor& apply(Parser& p) override {
		return t(p);
	}
};

template<class T>
struct ReaderRule : Rule {
	T& dest;

	ReaderRule(T& _dest) : dest(_dest) {}

	ErrorProcessor& apply(Parser& p) override {
		p.stream >> dest;
		return p << (p.stream.operator bool() && !dest.empty());
	}
};

struct BoolRule : Rule {
	bool val;

	BoolRule(bool _val): val(_val) {}

	ErrorProcessor& apply(Parser& p) override {
		return p << val << "Bool Rule";
	}
};

RulePtr::RulePtr(char c) : unique_ptr(std::make_unique<CharRule>(c)) {}
RulePtr::RulePtr(bool b) : unique_ptr(std::make_unique<BoolRule>(b)) {}
template<class T> RulePtr::RulePtr(T&& t) : unique_ptr(std::make_unique<FuncRule<T>>(std::move(t))) {}
template<> RulePtr::RulePtr(RulePtr&& other) : unique_ptr(std::move(other)) {}
template<class T> RulePtr::RulePtr(T& t) : unique_ptr(std::make_unique<ReaderRule<T>>(t)) {}

#endif // !CFAST_IMPLICIT_RULES_HPP
