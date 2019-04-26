#ifndef CFAST_INCLUDES_HPP
#define CFAST_INCLUDES_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <limits>
#include <set>
#include <vector>
#include <algorithm>

using namespace std::literals;

namespace cf {

using char_type = char;
constexpr char_type line_terminator = '\n';

struct TextPosition;

template<class Iter>
struct WeakSpan;

template<class Iter>
class WeakSlice;

using Source = WeakSpan<TextPosition>;

struct Token;
struct Lexer;

using TreePtr = size_t;

template<class T>
struct Tree;

struct SyntaxNode;
using Children = std::vector<TreePtr>;
using SyntaxTree = Tree<SyntaxNode>;
using Working = WeakSpan<TreePtr>;

struct Parser;

template<class T>
void write(std::ostream& stream, const T& x, size_t n = 1) {
	stream.write(reinterpret_cast<const char*>(&x), sizeof(x) * n);
}
template<class T>
void read(std::istream& stream, T& x, size_t n = 1) {
	stream.read(reinterpret_cast<char*>(&x), sizeof(x) * n);
}

template<class T>
void write(std::ostream& stream, const std::vector<T>& vec) {
	write(stream, vec.size());
	if (vec.empty()) return;
	if constexpr (std::is_integral_v<T> || std::is_pod_v<T>) {
		write(stream, vec.front(), vec.size());
	}
	else {
		for (auto& item : vec)
			item.save_binary(stream);
	}
}

template<class T>
void read(std::istream& stream, std::vector<T>& vec) {
	vec.clear();
	size_t s;
	read(stream, s);
	if (s == 0) return;
	vec.reserve(s);
	vec.resize(s);
	if constexpr (std::is_integral_v<T> || std::is_pod_v<T>) {
		read(stream, vec.front(), vec.size());
	}
	else {
		for (auto& item : vec)
			item.load_binary(stream);
	}
}

} // namespace cf

#endif // !CFAST_INCLUDES_HPP
