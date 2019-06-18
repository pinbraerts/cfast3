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
	Scope global;

	Type* cint = global.Declare<Type>("Int");
	Type* cdouble = global.Declare<Type>("Double");

	Type* _X = global.Declare<Type>("X");
	_X->Declare<Method>("a", cint);
	_X->Declare<Method>("b", cdouble);

	std::cout << std::boolalpha;
	std::cout << _X->Find("a")->type->name << std::endl;
	std::cout << bool(_X->Find("c")) << std::endl;
	return 0;
}
