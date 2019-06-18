#include <iostream>

#include "Binder.hpp"

using namespace cf;
using namespace rt;

int main() {
#if 0
	Scope global;

	Type* cint = global.Declare<Type>("Int");
	Type* cdouble = global.Declare<Type>("Double");

	Type* _X = global.Declare<Type>("X");
	_X->Declare<Method>("a", cint);
	_X->Declare<Method>("b", cdouble);

	std::cout << std::boolalpha;
	std::cout << _X->Resolve("a")->type->name << std::endl;
	std::cout << bool(_X->Resolve("c")) << std::endl;
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
	Flow flow;
	flow.scope_stack.push_back(&global);
	BoundTree b;
	Binder(tree, b, flow).Bind();
	std::cout << b << std::endl;

#endif
	return 0;
}
