#ifndef CFAST_OBJECT_HPP
#define CFAST_OBJECT_HPP

#include <algorithm>

#include "Pointer.hpp"

struct Object;
struct Root;
struct Class;
struct Function;

using PObject = ptr<Object>;
using WObject = Object * ;

using PClass = ptr<Class>;
using WClass = Class * ;

struct Object: Collectible<Object> {
	PClass klass;
	WObject context = nullptr;
	std::string name;
	std::vector<PObject> children;

	void* raw_data = nullptr;

	Object(PClass _klass, WObject _context, std::string _name = "", void* _raw_data = nullptr) :
		klass(std::move(_klass)),
		context(_context),
		name(std::move(_name)),
		raw_data(_raw_data) { }

	Object(PClass _klass, WObject _context, std::vector<PObject> _children, std::string _name = "", void* _raw_data = nullptr) :
		klass(std::move(_klass)),
		context(_context),
		name(std::move(_name)),
		children(std::move(_children)),
		raw_data(_raw_data) { }

	Object() = default;

	bool is_root() const {
		return context == nullptr;
	}
	Root* obtain_root() {
		WObject ptr;
		for (ptr = this; !ptr->is_root(); ptr = ptr->context);
		return (Root*)ptr; // unsafe cast
	}

	PObject find_local(std::string name) {
		for (auto s : children) {
			if (std::strncmp(name.c_str(), s->name.c_str(), std::min(name.size(), s->name.size())) == 0)
				return s;
		}
		return nullptr;
	}

	PObject find_all(std::string name) {
		if (PObject res = find_local(name))
			return res;

		if (is_root()) return nullptr;

		return context->find_all(name);
	}

	template<class T>
	PObject c_wrap(const T& x) {
		PObject res = obtain_root()->c_wrap<T>(x);
		if (res != nullptr)
			res->context = this;
		return res;
	}

	template<class T>
	PObject c_wrap(T&& x) {
		PObject res = obtain_root()->c_wrap<T>(x);
		if (res != nullptr)
			res->context = this;
		return res;
	}

	template<class T>
	T c_unwrap(PObject obj) {
		return obtain_root()->c_unwrap<T>(obj);
	}

	~Object() {
		if (raw_data != nullptr) {
			delete raw_data;
		}
	}
};



#endif // !CFAST_OBJECT_HPP
