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

struct Node;

using Children = WeakSpan<Node, TreePtr>;
struct Parser;

} // namespace cf

#endif // !CFAST_INCLUDES_HPP
