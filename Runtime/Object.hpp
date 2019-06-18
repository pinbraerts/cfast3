#ifndef CFAST_OBJECT_HPP
#define CFAST_OBJECT_HPP

#include <algorithm>
#include <string_view>
#include <variant>
#include <vector>
#include <unordered_set>

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
};

struct Scope {
	std::vector<std::unique_ptr<Method>> methods;

	Method* Find(std::string name) {
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

} // namespace cf::rt

#endif // !CFAST_OBJECT_HPP
