#ifndef CFAST_C_HPP
#define CFAST_C_HPP

#include <map>
#include <sstream>

#include "Class.hpp"

template<class Ret, class traits, class F, size_t... i>
PObject invoke_c_function_impl(PObject context, PObject self, PObject arguments, std::index_sequence<i...>) {
	F f = *(F*)self->raw_data;
	if (f == nullptr) return nullptr;
	return context->c_wrap<Ret>(f(context->c_unwrap<traits::get_t<i>>(arguments->children[i])...));
}

template<class Ret, class... Args>
PObject invoke_c_function(PObject context, PObject self, PObject arguments) {
	using traits = types_list<Args...>;
	using F = f_type<Ret, Args...>;
	return invoke_c_function_impl<Ret, traits, F>(context, self, arguments, traits::sequence{});
}

struct Root: Object {
	using traits = types_list<int, float, double>;
	static constexpr const char* names[] = { "Int", "Float", "Double" };

	std::map<std::vector<size_t>, WClass> function_classes;

	template<size_t... i> void push_impl(std::index_sequence<i...>) {
		children = std::vector<PObject> {
			new Class(this, names[i], sizeof(traits::get_t<i>))...
		};
	}

	Root() {
		push_impl(traits::sequence{});
	}

	template<class T>
	PObject c_wrap(const T& x) {
		if constexpr (std::is_pointer_v<T> && std::is_function_v<std::remove_pointer_t<T>>) {
			return c_wrap_f(x);
		}
		else if constexpr (std::is_function_v<T>) {
			return c_wrap_f(&x);
		}
		else if constexpr (traits::index<T> < traits::size) {
			return new Object(children[traits::index<T>], nullptr, "", new T(x));
		}
		else if constexpr (std::is_same_v<T, PObject>) {
			return x;
		}
		else return nullptr; // TODO return error "No such wrapper"
	}

	template<class First, class... Args> static void make_argument_names_impl(std::ostringstream& ss) {
		// TODO add check
		ss << names[traits::index<First>];
		if constexpr (sizeof...(Args) > 0) {
			ss << ", ";
			return make_argument_names_impl<Args...>(ss);
		}
	}
	template<class... Args> static void make_argument_names(std::ostringstream& ss) {
		if constexpr (sizeof...(Args) > 0) {
			return make_argument_names_impl<Args...>(ss);
		}
	}

	template<class Ret, class... Args>
	static std::string make_function_name() {
		std::ostringstream ss;
		// TODO add check
		ss << names[traits::index<Ret>] << '(';
		make_argument_names<Args...>(ss);
		ss << ')';
		return ss.str();
	}

	template<class Ret, class... Args>
	static std::vector<PClass> make_arguments() {
		// TODO add check
		return std::vector<PClass> {
			(PClass)children[traits::index<Ret>],
				(PClass)children[traits::index<Args>]...
		};
	}

	template<class Ret, class... Args>
	PClass get_function_class() {
		std::vector<size_t> expected{
			traits::index<Ret>,
			traits::index<Args>...
		};
		auto it = function_classes.lower_bound(expected);
		if (it != function_classes.end() && it->first == expected) {
			return it->second;
		}
		else {
			it = function_classes.insert(it, { expected, new Class(this, make_function_name<Ret, Args...>(), sizeof(f_type<Ret, Args...>), invoke_c_function<Ret, Args...>) });
			children.push_back(it->second);
			return it->second;
		}
	}

	template<class Ret, class... Args>
	PObject c_wrap_f(f_type<Ret, Args...> f) {
		return new Object(get_function_class<Ret, Args...>(), this, "", new f_type<Ret, Args...>(f));
	}

	template<class T>
	T c_unwrap(PObject x) {
		if constexpr (std::is_pointer_v<T> && std::is_function_v<std::remove_pointer_t<T>>) {
			return *(T*)c_unwrap_f(&x, T{});
		}
		else if constexpr (traits::index<T> < traits::size) {
			if (x->type != children[traits::index<T>]) return T(); // TODO cast or return error of type mismatch
			return *(T*)x->raw_data;
		}
		else if constexpr (std::is_same_v<T, PObject>) {
			return x;
		}
		else return T(); // TODO return error "No such wrapper"
	}

	template<class Ret, class... Args>
	void* c_unwrap_f(PObject x, f_type<Ret, Args...>) {
		if (x->type != get_function_class<Ret, Args...>()) return nullptr; // TODO cast or return error of type mismatch
		return x->raw_data;
	}
};

#endif // !CFAST_C_HPP
