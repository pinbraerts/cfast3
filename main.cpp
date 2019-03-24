#include "ImplicitRules.hpp"
#include "BlockRule.hpp"

RulePtr str_literal(
	std::string& dest,
	RulePtr&& start = '"',
	RulePtr&& end = '"'
) {
	return block(std::move(start), std::move(end), true, [&](Parser& p) -> ErrorProcessor& {
		char c = p.stream.get();
		if (c == '\\')
			c = p.stream.get();
		if (c == EOF)
			return p << Code::eof;
		dest += (char)c;
		return p;
	});
}

int main() {
	std::ifstream input("input.txt");
	try {
		Parser(input) >> block("{,}", [index = 0](Parser& p) mutable -> Parser& {
			std::string name, value;
			p >> str_literal(name) >> ':' >> str_literal(value, '\'', '\'');
			std::cout << index << ' ' << name << " = " << value << std::endl;
			++index;
			return p;
		});
	}
	catch (const Error& e) {
		std::cerr << e.message << std::endl;
		return e.retcode();
	}
	return 0;
}
