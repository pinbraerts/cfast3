#ifndef CFAST_OBJECT_HPP
#define CFAST_OBJECT_HPP

#include <algorithm>
#include <string_view>
#include <vector>

using namespace std::string_literals;

namespace cf::rt {

using byte = unsigned char;

struct Type;

struct Head {
	Type* type;

	Head(Type* _type = nullptr): type(_type) {}
};

struct Method: Head {
	std::string name;

	Method(
		std::string _name = "",
		Type* _type = nullptr
	) :	Head(_type),
		name(std::move(_name)) {}

	static Method NotFound() {
		return Method();
	}

	virtual ~Method() = default;
};

struct Scope {
	std::vector<std::unique_ptr<Method>> methods;

	Method* Resolve(std::string_view name) {
		auto iter = std::find_if(methods.begin(), methods.end(), [name](const std::unique_ptr<Method>& method) { return method->name == name; });
		if (iter < methods.end()) {
			return iter->get();
		}
		else return nullptr;
	}

	template<class T, class... Args>
	T* Declare(Args&& ... args) {
		return (T*)methods.emplace_back(std::make_unique<T>(std::forward<Args>(args)...)).get();
	}
};

struct Type: Method, Scope {
	Type(
		std::string _name = "",
		Type* meta = nullptr
	) :	Method(_name, meta) {}	
};

template<class T>
struct Literal : Method {
	T value;

	Literal(
		std::string _name,
		T _value,
		Type* type = nullptr // ctype<T>()
	) :	Method(_name, type),
		value(_value) {}
};



struct Flow {
	std::vector<Scope*> scope_stack;
	size_t count_constants = 0;

	Scope* current() {
		return scope_stack.back();
	}
	Scope* global() {
		return scope_stack.front();
	}

	template<class... Args>
	Type* DeclareType(Args&& ... args) {
		((std::cout << "Declare Type: ") << ... << args) << std::endl;
		return current()->Declare<Type>(std::forward<Args>(args)...);
	}
	template<class... Args>
	void DeclareTypeAndEnter(Args&& ... args) {
		scope_stack.push_back(DeclareType(std::forward<Args>(args)...));
	}
	void ExitScope() {
		scope_stack.pop_back();
	}

	// TODO add methods of calling function

	template<class... Args>
	Method* DeclareMethod(Args&& ... args) {
		((std::cout << "Declare Method: ") << ... << args) << std::endl;
		return current()->Declare<Type>(std::forward<Args>(args)...);
	}

	template<class T>
	Method* DeclareLiteral(T x) {
		std::cout << "Declare Literal: " << x << std::endl;
		return current()->Declare<Literal<T>>("lit"s + std::to_string(count_constants++), x);
	}

	Method* Resolve(std::string_view name) {
		Method* res = nullptr;
		for (auto i = scope_stack.rbegin(); i < scope_stack.rend() && res == nullptr; ++i) {
			std::cout << "Resolving " << name << " in next scope " << std::endl;
			res = (*i)->Resolve(name);
		}
		return res;
	}
};

} // namespace cf::rt

#endif // !CFAST_OBJECT_HPP
