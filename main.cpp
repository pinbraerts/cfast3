#include "Parser.hpp"

using namespace cf;

int main() {
#if 1
	SyntaxTree tree;
	std::ifstream input("example.out", std::ios::binary);
	Lexer l = Lexer::from_file("example.fc");
	tree.load_binary(input, l.source.begin().ptr());
	std::cout << tree << std::endl;
	return 0;
#else
	Lexer l = Lexer::from_file("example.fc");
	SyntaxTree tree;
	Parser p(tree, l);
	Parser::Error e = p.parse();
	if (!e.is_ok())
		std::cerr << e.message << std::endl;
	std::cout << tree << std::endl;
	std::ofstream output("example.out", std::ios::binary);
	tree.save_binary(output, l.source.begin().ptr());
	return 0;
#endif
}
