#ifndef CFAST_POINTER_HPP
#define CFAST_POINTER_HPP

template<class T>
class Collectible {
private:
	mutable size_t ptrs;

	T& self() {
		return static_cast<T&>(*this);
	}
	const T& self() const {
		return static_cast<T&>(*this);
	}

public:
	void inc_ptrs() const {
		++ptrs;
	}
	void dec_ptrs() {
		--ptrs;
		if (ptrs == 0) {
			self().~T();
			delete this;
		}
	}

	Collectible() : ptrs(0) {}
};

template<class T>
class ptr {
public:
	T* _ptr;

	ptr(nullptr_t = nullptr) : _ptr(nullptr) {}
	ptr(T* _raw) : _ptr(_raw) {
		if (_ptr != nullptr)
			_ptr->inc_ptrs();
	}
	ptr(const ptr& other) : _ptr(other._ptr) {
		if (_ptr != nullptr)
			_ptr->inc_ptrs();
	}
	ptr(ptr&& other) : _ptr(std::exchange(other._ptr, nullptr)) {}

	ptr& operator=(const ptr& other) {
		reset();
		_ptr = other._ptr;
		if (_ptr != nullptr)
			_ptr->inc_ptrs();
		return *this;
	}
	ptr& operator=(ptr&& other) {
		reset();
		_ptr = std::exchange(other._ptr, nullptr);
		return *this;
	}

	void reset() {
		if (_ptr != nullptr)
			_ptr->dec_ptrs();
		_ptr = nullptr;
	}
	void swap(const ptr& other) {
		std::swap(_ptr, other._ptr);
	}

	~ptr() {
		if (_ptr != nullptr)
			_ptr->dec_ptrs();
	}

	template<class U>
	operator ptr<U>() {
		return (U*)_ptr;
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

	operator bool() const {
		return _ptr != nullptr;
	}

	template<class U>
	bool operator==(const ptr<U>& other) const {
		return _ptr == other._ptr;
	}
	template<class U>
	bool operator!=(const ptr<U>& other) const {
		return _ptr != other._ptr;
	}
	template<class U>
	bool operator>(const ptr<U>& other) const {
		return _ptr > other._ptr;
	}
	template<class U>
	bool operator<(const ptr<U>& other) const {
		return _ptr < other._ptr;
	}
	template<class U>
	bool operator>=(const ptr<U>& other) const {
		return _ptr >= other._ptr;
	}
	template<class U>
	bool operator<=(const ptr<U>& other) const {
		return _ptr <= other._ptr;
	}
	template<class U>
	size_t operator-(const ptr<U>& other) const {
		return _ptr - other._ptr;
	}
};


#endif // !CFAST_POINTER_HPP
