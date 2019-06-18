#ifndef CFAST_TEXT_POSITION_HPP
#define CFAST_TEXT_POSITION_HPP

#include "WeakSpan.hpp"

namespace cf {

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

	void SaveBinary(std::ostream& stream) {
		Write(stream, *this);
	}
	void LoadBinary(std::istream& stream) {
		Read(stream, *this);
	}
};

} // namespace cf

#endif // !CFAST_TEXT_POSITION_HPP
