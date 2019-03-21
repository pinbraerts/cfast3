#ifndef CFAST_PARSER_HPP
#define CFAST_PARSER_HPP

#include "Rule.hpp"

class Parser {
public:
	enum Code {
		ok, stop, fatal, eof
	} code = Code::ok;
	std::ostringstream message;

	std::istream& stream;

	explicit Parser(std::istream& _stream) noexcept : stream(_stream) {}

	explicit operator bool() {
		bool res = code == Code::ok;
		message.str("");
		message.clear();
		code = Code::ok;
		return res;
	}
	bool operator!() const {
		return code != Code::ok;
	}

	struct Error {
		Code code;
		std::string message;

		Error(Code _code, const std::string& _message) : code(_code), message(_message) {}
		Error(Code _code, std::string&& _message) : code(_code), message(std::move(_message)) {}

		int retcode() const {
			return 1 + code;
		}
	};

	[[noreturn]] void raise() const {
		throw Error(code, message.str());
	}
	void check() const {
		if (operator!())
			raise();
	}

	Parser& operator<<(Code _code) noexcept {
		code = _code;
		if (code == Code::eof)
			message.str("Reached the end of file");
		return *this;
	}
	Parser& operator<<(bool condition) noexcept {
		if (!condition)
			code = Code::stop;
		return *this;
	}
	template<class T>
	Parser& operator<<(T&& x) {
		if (operator!())
			message << std::move(x);
		return *this;
	}
	template<class T>
	Parser& operator<<(const T& x) {
		if (operator!())
			message << x;
		return *this;
	}
	template<> Parser& operator<<(RulePtr&&) = delete;
	template<> Parser& operator<<(const RulePtr&) = delete;

	Parser& operator>>(Rule& rule) {
		check();
		return rule.apply(*this);
	}
	Parser& operator>>(Rule&& rule) {
		check();
		return rule.apply(*this);
	}
	Parser& operator>>(RulePtr&& ptr) {
		check();
		return ptr->apply(*this);
	}
	Parser& operator>>(RulePtr& ptr) {
		check();
		return ptr->apply(*this);
	}
};

#endif // !CFAST_PARSER_HPP
