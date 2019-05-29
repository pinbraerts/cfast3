#ifndef CFAST_CLASS_HPP
#define CFAST_CLASS_HPP

#include <vector>

#include "Object.hpp"

struct Class : Object {
	using invoke_t = PObject(*)(PObject, PObject, PObject);

	std::vector<PClass> parents;
	invoke_t _invoke;
	size_t size;

	Class(WObject _context, std::string _name, size_t _size, invoke_t invoker = nullptr, void* _raw_data = nullptr) :
		Object(nullptr, _context, std::move(_name), std::move(_raw_data)),
		size(_size), _invoke(invoker) { }

	Class(std::vector<PClass> _parents, WObject _context, std::string _name, size_t _size, invoke_t invoker = nullptr, void* _raw_data = nullptr) :
		Object(nullptr, _context, std::move(_name), std::move(_raw_data)),
		parents(std::move(_parents)), _invoke(invoker), size(_size) { }

	bool is_invokeable() const {
		return _invoke != nullptr;
	}

	PObject invoke(PObject context, PObject self, PObject arguments) {
		if (_invoke == nullptr) return nullptr; // TODO return error "Object is not invokeable"

		size_t i = 1;
		if (i < children.size()) {
			auto& income = self, &expected = children[i];
			if (income->type != expected->type) return nullptr; // TODO check cast, return error "Argument class mismatch"
			if (!income->name().empty() && income->name() != expected->name()) return nullptr; // TODO return error "Argument name mismatch"
		}

		for (++i; i < children.size() && i <= arguments->children.size(); ++i) {
			auto& income = arguments->children[i - 1], &expected = children[i];
			if (income->type != expected->type) return nullptr; // TODO check cast, return error "Argument class mismatch"
			if (!income->name().empty() && income->name() != expected->name()) return nullptr; // TODO return error "Argument name mismatch"
		}

		// default arguments
		for (; i < children.size(); ++i) {
			if (children[i]->raw_data == nullptr) return nullptr; // TODO return error "More arguments expected"
			arguments->children.push_back(children[i]);
		}

		if (i <= arguments->children.size()) return nullptr; // TODO return error "Less arguments expected"

		return _invoke(context, self, arguments);
	}
};

#endif // !CFAST_CLASS_HPP
