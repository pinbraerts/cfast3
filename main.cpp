#include "Parser.hpp"

using namespace cf;

int main() {
	Lexer l = Lexer::from_file("example.fc");
	SyntaxTree tree;
	Parser p(tree, l);
	Parser::Error e = p.parse();
	if (!e.is_ok())
		std::cerr << e.message << std::endl;
	std::cout << tree << std::endl;
	return 0;
}
