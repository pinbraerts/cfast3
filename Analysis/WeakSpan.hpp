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
		Set(other);
	}
	template<class I>
	WeakSpan(I first, I last) noexcept {
		Set(first, last);
	}

	size_t Size() const noexcept {
		return _end - _begin;
	}

	bool IsEmpty() const noexcept {
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
	WeakSpan& Set(I first, I last) noexcept {
		_begin = first;
		_end = last;
		return *this;
	}
	template<class I>
	WeakSpan& Set(I& span) noexcept {
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
		if (other.Size() != Size()) return false;
		if (IsEmpty()) return true;

		for (Iter i = begin(), j = other.begin(); i < end() && j < other.end(); ++i, ++j)
			if constexpr (std::is_same_v<Iter, TextPosition>) {
				if (i.chr() != j.chr()) return false;
			}
			else if (*i != *j) return false;

		return true;
	}

	bool operator!=(const WeakSpan& other) const noexcept {
		if (&other == this) return false;
		if (other.Size() != Size()) return true;
		if (IsEmpty()) return false;

		for (Iter i = begin(), j = other.begin(); i < end() && j < other.end(); ++i, ++j)
			if constexpr (std::is_same_v<Iter, TextPosition>) {
				if (i.chr() == j.chr()) return false;
			}
			else if (*i == *j) return false;

		return true;
	}

	operator std::string_view() const {
		return std::string_view(_begin.ptr(), Size());
	}

	//operator std::string() const {
	//	return std::string(_begin.ptr(), Size());
	//}

	void SaveBinary(std::ostream& stream) {
		Write(stream, *this);
	}
	void LoadBinary(std::istream& stream) {
		Read(stream, *this);
	}
};

template<class Iter> WeakSpan(Iter, Iter) -> WeakSpan<Iter>;

template<class T, class Diff = size_t>
auto Slice(T& t, Diff s) noexcept {
	auto it = t.begin();
	std::advance(it, s);
	return WeakSpan(it, t.end());
}
template<class T, class Diff = size_t>
auto Slice(const T& t, Diff s) {
	auto it = t.cbegin();
	std::advance(it, s);
	return WeakSpan(it, t.end());
}

template<class T, class Diff = size_t>
auto Slice(T& t, Diff s, Diff e) {
	auto it = t.begin(), it2 = t.begin();
	std::advance(it, s); std::advance(it2, e);
	return WeakSpan(it, it2);
}
template<class T, class Diff = size_t>
auto Slice(const T& t, Diff s, Diff e) {
	auto it = t.cbegin(), it2 = t.cbegin();
	std::advance(it, s); std::advance(it2, e);
	return WeakSpan(it, it2);
}

} // namespace cf

#endif // !CFAST_SOURCE_HPP
