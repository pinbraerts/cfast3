#include <iostream>

#include "CBinding.hpp"

double f(float x) {
	std::cout << x << std::endl;
	return x + 5;
}

int main() {
	PObject global = new Root();

	PObject x = global->c_wrap<float>(4.5f);
	x->name = "x";
	global->children.push_back(x);

	PObject _f = global->c_wrap(f);
	_f->name = "f";
	global->children.push_back(_f);

	_f = global->find_all("f");
	x = global->find_all("x");
	std::cout << _f->klass->name << ' ' << x->klass->name << std::endl;

	PObject res = _f->klass->invoke(global, _f, new Object(nullptr, nullptr, std::vector<PObject>{ x }));
	std::cout << res->klass->name << ' ' << global->c_unwrap<double>(res) << std::endl;

	return 0;
}
