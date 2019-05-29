#ifndef CFAST_OBJECT_HPP
#define CFAST_OBJECT_HPP

#include <algorithm>
#include <string_view>

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
private:
	std::string_view _m_name = std::string_view();

	std::string_view copy_name(const std::string_view& _name) {
		if (!_name.empty()) {
			char* nm = new char[_name.size()];
			std::copy(_name.begin(), _name.end(), nm);
			return std::string_view(nm, _name.size());
		}
		return std::string_view();
	}

	void release_name() {
		if (!_m_name.empty()) {
			delete _m_name.data();
			_m_name = std::string_view();
		}
	}

public:
	PClass type;
	WObject context = nullptr;
	std::vector<PObject> children;

	void* raw_data = nullptr;

	Object(PClass _klass, WObject _context, std::string_view _name = std::string_view(), void* _raw_data = nullptr) :
		type(std::move(_klass)),
		context(_context),
		_m_name(copy_name(_name)),
		raw_data(_raw_data) { }

	Object(PClass _klass, WObject _context, std::vector<PObject> _children, std::string_view _name = std::string_view(), void* _raw_data = nullptr) :
		type(std::move(_klass)),
		context(_context),
		_m_name(copy_name(_name)),
		children(std::move(_children)),
		raw_data(_raw_data) { }

	Object() = default;

	void name(std::string_view _name) {
		release_name();
		_m_name = copy_name(_name);
	}
	const std::string_view& name() {
		return _m_name;
	}

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
			if (s->_m_name == name)
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
			raw_data = nullptr;
		}
		release_name();
	}
};


#endif // !CFAST_OBJECT_HPP
