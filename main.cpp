#include "Parser.hpp"
using namespace cf;

struct A {
	TreePtr parent;
	WeakSpan<A, TreePtr> children;
	int v;

	A(TreePtr p, int x) : parent(p), v(x), children(0, 0) {}
};

std::ostream& operator<<(std::ostream& stream, const A& a) {
	return stream << a.v;
}

void tree_test() {
	Tree<A> t;
	char cmd;
	int v;
	do {
		std::cin >> cmd;
		switch (cmd) {
		case 'c':
			std::cin >> v;
			t.push_child(v);
			break;
		case 's':
			std::cin >> v;
			t.push_sibling(v);
			break;
		case 'p':
			std::cin >> v;
			t.push_parent(v);
			break;
		case 'u':
			t.up();
			break;
		case 'n':
			t.next();
			break;
		case 'b':
			t.prev();
			break;
		case 'q': case 'e':
			cmd = '\0';
			break;
		}
		std::cout << t << std::endl;
	} while (cmd);
}

int main() {
	//tree_test();
	//return 0;

	Lexer l = Lexer::from_file("example.fc");
	Tree<SyntaxNode> tree;
	Parser p(tree, l);
	Parser::Error e = p.parse();
	if (!e.is_ok())
		std::cerr << e.message << std::endl;
	std::cout << tree << std::endl;
	return 0;
}
