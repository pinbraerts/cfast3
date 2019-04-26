#ifndef CFAST_SOURCE_HPP
#define CFAST_SOURCE_HPP

#include "includes.hpp"

namespace cf {

template<class Iter>
struct WeakSpan {
private:
	Iter _begin = 0, _end = 0;

public:
	WeakSpan() = default;
	template<class I>
	WeakSpan(I& other) {
		set(other);
	}
	template<class I>
	WeakSpan(I first, I last) {
		set(first, last);
	}

	size_t size() const {
		return _end - _begin;
	}

	bool is_empty() const {
		return _begin >= _end;
	}

	Iter& begin(Iter other) {
		return _begin = other;
	}
	Iter& begin() {
		return _begin;
	}
	const Iter& begin() const {
		return _begin;
	}

	Iter& end() {
		return _end;
	}
	const Iter& end() const {
		return _end;
	}
	Iter& end(Iter other) {
		return _end = other;
	}

	template<class I>
	WeakSpan& set(I first, I last) {
		_begin = first;
		_end = last;
		return *this;
	}
	template<class I>
	WeakSpan& set(I& span) {
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

	bool operator!=(const WeakSpan& other) const {
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

template<class T, class U = size_t>
class WeakSlice {
	U _start_offset = 0, _end_offset = 0;

	T& _val;

public:
	WeakSlice(T& val, U start_offset = 0, U end_offset = 0) :
		_val(val),
		_start_offset(start_offset),
		_end_offset(end_offset) {}

	WeakSlice(T& val, WeakSpan<U> span) :
		_val(val),
		_start_offset(span.begin()),
		_end_offset(span.end()) {}

	auto begin() {
		return std::advance(_val.begin(), _start_offset);
	}
	auto end() {
		return std::advance(_val.end(), -_end_offset);
	}

	auto cbegin() const {
		return std::advance(_val.cbegin(), _start_offset);
	}
	auto cend() const {
		return std::advance(_val.cend(), -_end_offset);
	}
};

template<class T, class U = size_t> WeakSlice(T&, U, U) -> WeakSlice<T, U>;
template<class T, class U> WeakSlice(T&, WeakSpan<U>) -> WeakSlice<T, U>;

struct TextPosition {
private:
	const char_type* _ptr = nullptr;

public:
	size_t line = 1, position = 1;

	TextPosition(char_type* ptr = nullptr): _ptr(ptr) {}

	TextPosition& operator++() {
		if (*_ptr == line_terminator) {
			++line;
			position = 1;
		}
		else ++position;
		++_ptr;
		return *this;
	}
	TextPosition& operator+=(size_t s) {
		for (size_t i = 0; i < s; ++i)
			operator++();
		return *this;
	}
	TextPosition operator+(size_t s) {
		return TextPosition(*this) += s;
	}
	size_t operator-(const TextPosition& other) const {
		return _ptr - other._ptr;
	}
	TextPosition& operator*() {
		return *this;
	}
	TextPosition& operator=(const char_type* ptr) {
		_ptr = ptr;
		line = 1;
		position = 1;
		return *this;
	}
	bool operator<(const TextPosition& other) const {
		return _ptr < other._ptr;
	}
	bool operator<=(const TextPosition& other) const {
		return _ptr < other._ptr;
	}
	bool operator>=(const TextPosition& other) const {
		return _ptr >= other._ptr;
	}
	bool operator!=(const TextPosition& other) const {
		return _ptr != other._ptr;
	}

	const char_type& chr() const {
		return *_ptr;
	}
	const char_type& chr() {
		return *_ptr;
	}
	const char_type* const& ptr() const {
		return _ptr;
	}
	const char_type*& ptr() {
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
