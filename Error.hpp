#ifndef CFAST_ERROR_HPP
#define CFAST_ERROR_HPP

#include <sstream>

enum class Code {
	ok, stop, fatal, eof
};

struct Error {
	Code code;
	std::string message;

	Error(Code _code, const std::string& _message) : code(_code), message(_message) {}
	Error(Code _code, std::string&& _message) : code(_code), message(std::move(_message)) {}

	int retcode() const {
		return 1 + (size_t)code;
	}
};

struct ErrorProcessor {
	Code code = Code::ok;
	std::ostringstream message;

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

	[[noreturn]] void raise() const {
		throw Error(code, message.str());
	}
	void check() const {
		if (operator!())
			raise();
	}

	ErrorProcessor& operator<<(Code _code) noexcept {
		code = _code;
		if (code == Code::eof)
			message.str("Reached the end of file");
		return *this;
	}
	ErrorProcessor& operator<<(bool condition) noexcept {
		if (!condition)
			code = Code::stop;
		return *this;
	}
	template<class T>
	ErrorProcessor& operator<<(T&& x) {
		if (operator!())
			message << std::move(x);
		return *this;
	}
	template<class T>
	ErrorProcessor& operator<<(const T& x) {
		if (operator!())
			message << x;
		return *this;
	}
};

#endif // !CFAST_ERROR_HPP
