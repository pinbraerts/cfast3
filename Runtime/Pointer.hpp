#ifndef CFAST_POINTER_HPP
#define CFAST_POINTER_HPP

#include <tuple>

template<class... T> constexpr size_t index_impl = 0;
template<class T, class F, class... U> constexpr size_t index_impl<T, F, U...> = index_impl<T, U...> +1;
template<class T, class... U> constexpr size_t index_impl<T, T, U...> = 0;

template<class... Args>
struct types_list {
	template<size_t i> using get_t = std::tuple_element_t<i, std::tuple<Args...>>;

	template<class T> static constexpr size_t index = index_impl<T, Args...>;

	static constexpr size_t size = sizeof...(Args);

	using sequence = std::index_sequence_for<Args...>;
};

template<class Ret, class... Args> using f_type = Ret(*)(Args...);

template<class T> struct basic_ptr {
protected:
	T* _ptr;

public:
	basic_ptr(nullptr_t = nullptr): _ptr(nullptr) {}
	basic_ptr(T* ptr): _ptr(ptr) {}
	basic_ptr(const basic_ptr& other): _ptr(other._ptr) {}
	basic_ptr(basic_ptr&& other): _ptr(std::exchange(other._ptr, nullptr)) {}

	template<class U>
	basic_ptr(U* pointer): _ptr(pointer) {}
	template<class U>
	basic_ptr(const basic_ptr<U>& other) : _ptr(other._ptr) {}
	template<class U>
	basic_ptr(basic_ptr<U>&& other) : _ptr(std::exchange(other._ptr, nullptr)) {}

	basic_ptr& operator=(const basic_ptr& other) {
		if (this == &other) return *this;
		if (_ptr == other._ptr) return *this;
		reset();
		_ptr = other._ptr;
		return *this;
	}
	basic_ptr& operator=(basic_ptr&& other) {
		if (this == &other) return *this;
		reset();
		_ptr = std::exchange(other._ptr, nullptr);
		return *this;
	}
	basic_ptr& operator=(T* pointer) {
		if (_ptr == pointer) return *this;
		reset();
		_ptr = pointer;
		return *this;
	}

	void reset() {
		if (_ptr != nullptr) {
			delete _ptr;
			_ptr = nullptr;
		}
	}
	template<class U>
	void swap(const basic_ptr<U>& other) {
		std::swap(_ptr, other._ptr);
	}

	operator T*() {
		return _ptr;
	}
	operator const T*() const {
		return _ptr;
	}

	T* operator->() {
		return _ptr;
	}
	const T* operator->() const {
		return _ptr;
	}

	T& operator*() {
		return *_ptr;
	}
	const T& operator*() const {
		return *_ptr;
	}

	T& operator[](size_t i) {
		return _ptr[i];
	}
	const T& operator[](size_t i) const {
		return _ptr[i];
	}

	operator bool() const {
		return _ptr != nullptr;
	}

	basic_ptr& operator++() {
		++_ptr;
		return *this;
	}
	basic_ptr& operator--() {
		--_ptr;
		return *this;
	}

	template<class U>
	bool operator==(const basic_ptr<U>& other) const {
		return _ptr == other._ptr;
	}
	template<class U>
	bool operator!=(const basic_ptr<U>& other) const {
		return _ptr != other._ptr;
	}
	template<class U>
	bool operator>(const basic_ptr<U>& other) const {
		return _ptr > other._ptr;
	}
	template<class U>
	bool operator<(const basic_ptr<U>& other) const {
		return _ptr < other._ptr;
	}
	template<class U>
	bool operator>=(const basic_ptr<U>& other) const {
		return _ptr >= other._ptr;
	}
	template<class U>
	bool operator<=(const basic_ptr<U>& other) const {
		return _ptr <= other._ptr;
	}
	template<class U>
	size_t operator-(const basic_ptr<U>& other) const {
		return _ptr - other._ptr;
	}
};

template<class T> struct weak: basic_ptr<T> {
	using base = basic_ptr<T>;
	using base::_ptr;
	using base::reset;
	using base::base;
	using base::operator=;
};

template<class T> struct weak<T[]>: basic_ptr<T> {
	using base = basic_ptr<T>;
	using base::_ptr;
	using base::reset;
	using base::base;
	using base::operator=;

	template<size_t N>
	weak(T(&arr)[N]) : _ptr(&arr) {}

	weak(nullptr_t = nullptr) : _ptr(nullptr) {}

	void reset() {
		if (_ptr != nullptr) {
			delete[] _ptr;
			_ptr = nullptr;
		}
	}
};

template<class T> struct owner: weak<T> {
	using base = weak<T>;
	using base::_ptr;
	using base::reset;
	using base::base;

	template<class U>
	owner(const owner<U>& other) = delete;

	template<class U>
	owner(owner<U>&& other): weak<T>(std::exchange(other._ptr, nullptr)) { }

	template<class... Args>
	static owner make(Args&&... args) {
		return owner(new T(std::forward<Args>(args)...));
	}

	template<class U>
	owner& operator=(const owner<U>& other) = delete;

	template<class U>
	owner& operator=(owner<U>&& other) {
		if (this == &other) return *this;
		reset();
		_ptr = std::exchange(other._ptr, nullptr);
		return *this;
	}

	template<class U>
	owner& operator=(U* ptr) {
		if (_ptr == ptr) return *this;
		reset();
		_ptr = ptr;
		return *this;
	}

	~owner() {
		reset();
	}
};

struct countable {
	size_t num_refs = 0;
};

template<class T> struct counted: weak<T> {
	using base = weak<T>;
	using base::_ptr;

	template<class... Args>
	static counted make(Args&&... args) {
		return counted(new T(std::forward<Args>(args)...));
	}

	template<class U>
	counted(const counted<U>& other): weak<T>(other._ptr) {
		inc_ref();
	}
	template<class U>
	counted(counted<U>&& other): weak<T>(std::exchange(other._ptr, nullptr)) { }

	counted(nullptr_t = nullptr): weak<T>(nullptr) { }
	counted(T* ptr): weak<T>(ptr) {}
	counted(const counted& other) : weak<T>(other._ptr) {
		inc_ref();
	}
	counted(counted&& other) noexcept : weak<T>(std::exchange(other._ptr, nullptr)) { }

	template<class U>
	counted& operator=(const counted<U>& other) {
		if (this == &other) return *this;
		if (_ptr == other._ptr) return *this;
		reset();
		_ptr = other._ptr;
		inc_ref();
		return *this;
	}

	template<class U>
	counted& operator=(counted<U>&& other) {
		if (this == &other) return *this;
		if (_ptr == other._ptr) return *this;
		reset();
		_ptr = std::exchange(other._ptr, nullptr);
		return *this;
	}

	template<class U>
	counted& operator=(U* other) {
		if (_ptr == other) return *this;
		reset();
		_ptr = other;
		return *this;
	}

	counted& operator=(counted&& other) {
		if (this == &other) return *this;
		if (_ptr == other._ptr) return *this;
		reset();
		_ptr = std::exchange(other._ptr, nullptr);
		return *this;
	}

	counted& operator=(counted& other) {
		if (this == &other) return *this;
		if (_ptr == other._ptr) return *this;
		reset();
		_ptr = other._ptr;
		inc_ref();
		return *this;
	}

	counted& operator=(T* ptr) {
		if (_ptr == ptr) return *this;
		reset();
		_ptr = ptr;
		return *this;
	}

	void inc_ref() {
		if(_ptr != nullptr)
			++_ptr->num_refs;
	}
	void reset() {
		if (_ptr != nullptr && _ptr->num_refs != 0) {
			--_ptr->num_refs;
			if (_ptr->num_refs == 0) {
				delete _ptr;
				_ptr = nullptr;
			}
		}
	}

	~counted() {
		reset();
	}
};

#endif // !CFAST_POINTER_HPP
