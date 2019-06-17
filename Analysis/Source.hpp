#ifndef CFAST_SOURCE_HPP
#define CFAST_SOURCE_HPP

#include "includes.hpp"

namespace cf {

template<class Iter>
struct WeakSpan {
private:
	Iter _begin = Iter(), _end = Iter();

public:
	WeakSpan() = default;
	template<class I>
	WeakSpan(I& other) noexcept {
		set(other);
	}
	template<class I>
	WeakSpan(I first, I last) noexcept {
		set(first, last);
	}

	size_t size() const noexcept {
		return _end - _begin;
	}

	bool is_empty() const noexcept {
		return _begin >= _end;
	}

	Iter& begin(Iter other) {
		return _begin = other;
	}
	Iter& begin() noexcept {
		return _begin;
	}
	const Iter& begin() const noexcept {
		return _begin;
	}

	Iter& end() noexcept {
		return _end;
	}
	const Iter& end() const noexcept {
		return _end;
	}
	Iter& end(Iter other) noexcept {
		return _end = other;
	}

	template<class I>
	WeakSpan& set(I first, I last) noexcept {
		_begin = first;
		_end = last;
		return *this;
	}
	template<class I>
	WeakSpan& set(I& span) noexcept {
		if constexpr (std::is_same_v<I, std::string>) {
			_begin = span.c_str();
			_end = _begin + span.size();
		}
		else {
			_begin = span.begin();
			_end = span.end();
		}
		return *this;
	}
	template<class I>
	WeakSpan& operator=(I& other) {
		return set(other);
	}

	bool operator==(const WeakSpan& other) const {
		if (&other == this) return true;
		if (other.size() != size()) return false;
		if (is_empty()) return true;

		for (Iter i = begin(), j = other.begin(); i < end() && j < other.end(); ++i, ++j)
			if constexpr (std::is_same_v<Iter, TextPosition>) {
				if (i.chr() != j.chr()) return false;
			}
			else if (*i != *j) return false;

		return true;
	}

	bool operator!=(const WeakSpan& other) const noexcept {
		if (&other == this) return false;
		if (other.size() != size()) return true;
		if (is_empty()) return false;

		for (Iter i = begin(), j = other.begin(); i < end() && j < other.end(); ++i, ++j)
			if constexpr (std::is_same_v<Iter, TextPosition>) {
				if (i.chr() == j.chr()) return false;
			}
			else if (*i == *j) return false;

		return true;
	}

	void save_binary(std::ostream& stream) {
		write(stream, *this);
	}
	void load_binary(std::istream& stream) {
		read(stream, *this);
	}
};

template<class Iter> WeakSpan(Iter, Iter) -> WeakSpan<Iter>;

template<class T, class Diff = size_t>
auto slice(T& t, Diff s) noexcept {
	auto it = t.begin();
	std::advance(it, s);
	return WeakSpan(it, t.end());
}
template<class T, class Diff = size_t>
auto slice(const T& t, Diff s) {
	auto it = t.cbegin();
	std::advance(it, s);
	return WeakSpan(it, t.end());
}

template<class T, class Diff = size_t>
auto slice(T& t, Diff s, Diff e) {
	auto it = t.begin(), it2 = t.begin();
	std::advance(it, s); std::advance(it2, e);
	return WeakSpan(it, it2);
}
template<class T, class Diff = size_t>
auto slice(const T& t, Diff s, Diff e) {
	auto it = t.cbegin(), it2 = t.cbegin();
	std::advance(it, s); std::advance(it2, e);
	return WeakSpan(it, it2);
}

struct TextPosition {
private:
	const char_type* _ptr = nullptr;

public:
	size_t line = 1, position = 1;

	TextPosition(char_type* ptr = nullptr) noexcept: _ptr(ptr) {}

	const char& operator[](size_t index) const noexcept {
		return _ptr[index];
	}
	TextPosition& operator++() noexcept {
		if (*_ptr == line_terminator) {
			++line;
			position = 1;
		}
		else ++position;
		++_ptr;
		return *this;
	}
	TextPosition& operator+=(size_t s) noexcept {
		for (size_t i = 0; i < s; ++i)
			operator++();
		return *this;
	}
	TextPosition operator+(size_t s) const noexcept {
		return TextPosition(*this) += s;
	}
	const char* operator-(size_t s) const noexcept {
		return _ptr - s;
	}
	size_t operator-(const TextPosition& other) const noexcept {
		return _ptr - other._ptr;
	}
	TextPosition& operator*() noexcept {
		return *this;
	}
	TextPosition& operator=(const char_type* ptr) noexcept {
		_ptr = ptr;
		line = 1;
		position = 1;
		return *this;
	}
	bool operator<(const TextPosition& other) const noexcept {
		return _ptr < other._ptr;
	}
	bool operator<=(const TextPosition& other) const noexcept {
		return _ptr <= other._ptr;
	}
	bool operator>=(const TextPosition& other) const noexcept {
		return _ptr >= other._ptr;
	}
	bool operator!=(const TextPosition& other) const noexcept {
		return _ptr != other._ptr;
	}

	const char_type& chr() const noexcept {
		return *_ptr;
	}
	const char_type& chr() noexcept {
		return *_ptr;
	}
	const char_type* const& ptr() const noexcept {
		return _ptr;
	}
	const char_type*& ptr() noexcept {
		return _ptr;
	}

	void save_binary(std::ostream& stream) {
		write(stream, *this);
	}
	void load_binary(std::istream& stream) {
		read(stream, *this);
	}
};

} // namespace cf

#endif // !CFAST_SOURCE_HPP
