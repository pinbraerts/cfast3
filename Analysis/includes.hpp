#ifndef CFAST_INCLUDES_HPP
#define CFAST_INCLUDES_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <limits>
#include <set>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

using namespace std::literals;

namespace cf {

using char_type = char;
constexpr char_type line_terminator = '\n';

using priority_t = size_t;

struct TextPosition;

template<class Iter>
struct WeakSpan;

template<class Iter, class U>
class WeakSlice;

using Source = WeakSpan<TextPosition>;

struct Token;
struct Lexer;

using TreePtr = size_t;
struct Node;
template<class T>
struct Tree;

template<class T>
struct TreePrinter;

struct SyntaxNode;
using Children = std::vector<TreePtr>;
using SyntaxTree = Tree<SyntaxNode>;

struct Traits;
struct Parser;

template<class T>
void Write(std::ostream& stream, const T& x, size_t n = 1) {
	stream.write(reinterpret_cast<const char*>(&x), sizeof(x) * n);
}
template<class T>
void Read(std::istream& stream, T& x, size_t n = 1) {
	stream.read(reinterpret_cast<char*>(&x), sizeof(x) * n);
}

template<class T>
void Write(std::ostream& stream, const std::vector<T>& vec) {
	Write(stream, vec.size());
	if (vec.empty()) return;
	if constexpr (std::is_integral_v<T> || std::is_pod_v<T>) {
		Write(stream, vec.front(), vec.size());
	}
	else {
		for (auto& item : vec)
			item.SaveBinary(stream);
	}
}

template<class T>
void Read(std::istream& stream, std::vector<T>& vec) {
	vec.clear();
	size_t s;
	Read(stream, s);
	if (s == 0) return;
	vec.reserve(s);
	vec.resize(s);
	if constexpr (std::is_integral_v<T> || std::is_pod_v<T>) {
		Read(stream, vec.front(), vec.size());
	}
	else {
		for (auto& item : vec)
			item.LoadBinary(stream);
	}
}

} // namespace cf

#endif // !CFAST_INCLUDES_HPP
