#include <iostream>

#include "Object.hpp"

struct X {
	int a;
	double b;
};

int main() {
	CppContext ctx;

	PType _X = ctx.CppType(X);
	_X->AddMember(PMember::make("a", sizeof(int)));
	_X->AddMember(PMember::make("b", sizeof(double)));

	counted<Object> x = new (_X) Object(_X);

	int* a = (int*)x->GetMember("a");
	std::cout << a << ' ' << &(((X*)x->body())->a) << std::endl;

	double* b = (double*)x->GetMember("b");
	std::cout << b << ' ' << &(((X*)x->body())->b) << std::endl;

	return 0;
}
