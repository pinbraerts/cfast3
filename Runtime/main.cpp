#include <iostream>

#include "Binder.hpp"

using namespace cf;
using namespace rt;

struct Operation {
	enum Code {
		Call,
		Push,
	} code;
};

int main() {
#if 0
	Scope global;

	Type* cint = global.Declare<Type>("Int");
	Type* cdouble = global.Declare<Type>("Double");

	Type* _X = global.Declare<Type>("X");
	_X->Declare<Method>("a", cint);
	_X->Declare<Method>("b", cdouble);

	std::cout << std::boolalpha;
	std::cout << _X->Find("a")->type->name << std::endl;
	std::cout << bool(_X->Find("c")) << std::endl;
#else
	Lexer l = Lexer::FromFile("..//Analysis//example.fc");
	SyntaxTree tree;
	Parser p(tree, l);
	Parser::Error e = p.Parse();
	if (!e.is_ok())
		std::cerr << e.message << std::endl;
	std::cout << tree << std::endl;
	std::ofstream output("..//Analysis//example.out", std::ios::binary);
	tree.SaveBinary(output, l.source.begin().ptr());

	Scope global;
	BoundTree b;
	Binder(tree, b, global).Bind();
	std::cout << b << std::endl;

#endif
	return 0;
}
