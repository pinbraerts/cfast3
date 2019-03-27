#ifndef CFAST_FUNCTION_HPP
#define CFAST_FUNCTION_HPP

template<class T> struct fn;

template<class R, class... Args>
class fn<R(Args...)> {
	using invoke_t = R(*)(void*, Args&&...);
	using destroy_t = void(*)(void*);
	using construct_t = void(*)(void*, void*);

	template<class T>
	static R invoke(T* t, Args&&... args) {
		return (*t)(std::forward<Args>(args)...);
	}
	template<class T>
	static void construct(T* t1, T* t2) {
		new (t1) T(*t2);
	}
	template<class T>
	static void destroy(T* t) {
		t->~T();
	}

	invoke_t invoke_f;
	destroy_t destroy_f;
	construct_t construct_f;
	std::unique_ptr<char[]> data;
	size_t size;

public:
	fn() :
		invoke_f(nullptr),
		destroy_f(nullptr),
		construct_f(nullptr),
		data(nullptr),
		size(0) {}

	template<class F>
	fn(F&& f) :
		invoke_f(reinterpret_cast<invoke_t>(invoke<F>)),
		destroy_f(reinterpret_cast<destroy_t>(destroy<F>)),
		construct_f(reinterpret_cast<construct_t>(construct<F>)),
		data(new char[sizeof(F)]),
		size(sizeof(F)) {}

	R operator()(Args&&... args) {
		return invoke_f(data.get(), std::forward<Args>(args)...);
	}

	~fn() {
		if (data != nullptr)
			destroy_f(data.get());
	}
};

#endif // !CFAST_FUNCTION_HPP
