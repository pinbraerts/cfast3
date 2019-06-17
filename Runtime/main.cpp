#include <iostream>

#include "Binder.hpp"

using namespace cf;
using namespace rt;

int main() {
	//Scope global;

	//Type* cint = global.Add<Type>("Int");
	//Type* cdouble = global.Add<Type>("Double");

	//Type* _X = global.Add<Type>("X");
	//_X->Add<Method>("a", cint);
	//_X->Add<Method>("b", cdouble);

	//std::cout << std::boolalpha;
	//std::cout << _X->Find("a")->type->name << std::endl;
	//std::cout << bool(_X->Find("c")) << std::endl;


#if 0
	SyntaxTree tree;
	std::ifstream input("example.out", std::ios::binary);
	Lexer l = Lexer::from_file("example.fc");
	tree.load_binary(input, l.source.begin().ptr());
	std::cout << tree << std::endl;
#else
	Lexer l = Lexer::from_file("..//Analysis//example.fc");
	SyntaxTree tree;
	Parser p(tree, l);
	Parser::Error e = p.parse();
	if (!e.is_ok())
		std::cerr << e.message << std::endl;
	std::cout << tree << std::endl;
	std::ofstream output("..//Analysis//example.out", std::ios::binary);
	tree.save_binary(output, l.source.begin().ptr());
#endif

	BoundTree bound;
	Scope global;
	Binder b(tree, bound, global);
	b.Bind();

	std::cout << bound << std::endl;

	return 0;
}
