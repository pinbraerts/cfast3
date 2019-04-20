#ifndef CFAST_INCLUDES_HPP
#define CFAST_INCLUDES_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <limits>
#include <set>
#include <vector>

using namespace std::literals;

namespace cf {

using char_type = char;
constexpr char_type line_terminator = '\n';

struct TextPosition;

template<class T, class Iter>
struct WeakSpan;

using Source = WeakSpan<char_type, TextPosition>;

struct Token;
struct Lexer;

using TreePtr = size_t;

template<class T>
struct Tree;

struct SyntaxNode;
using Children = WeakSpan<SyntaxNode, TreePtr>;
using SyntaxTree = Tree<SyntaxNode>;

struct Parser;

template<class T>
void write(std::ostream& stream, const T& x, size_t n = 1) {
	stream.write(reinterpret_cast<const char*>(&x), sizeof(x) * n);
}
template<class T>
void read(std::istream& stream, T& x, size_t n = 1) {
	stream.read(reinterpret_cast<char*>(&x), sizeof(x) * n);
}

} // namespace cf

#endif // !CFAST_INCLUDES_HPP
