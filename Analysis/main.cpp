#include <iostream>
#include <iomanip>

#include "Parser.hpp"

using namespace cfast;

void TestLexer() {
    auto b = Buffer<char>::FromFile("Lexer.hpp");
    Lexer<char> l(b);
    using L = Lexer<char>;
    L::Token t = l.Next();
    for(; t.type != L::Traits::Type::End; t = l.Next()) {
        std::cout << ToString(t.type) << ' ' << '\'' << l.buffer().span(t) << '\'' << std::endl;
    }
}

void TestParser() {
    auto b = Buffer<char>::FromFile("Parser.hpp");
    Lexer<char> l(b);
    Parser<decltype(l)>::Tree t;
    Parser<decltype(l)> p(l, t);
    auto res = p.Parse();
    if (!res.empty()) {
        std::cerr << res << std::endl;
        return;
    }

    for (auto& node : p._walker) {
        std::cout << std::setw(node.depth()) << ' '
            << ToString(node->item.type) << ' '
            << node->item.priority << ' '
            << b.span(node->item)
            << std::endl;
    }
}

int main() {
    TestLexer();
    TestParser();
    return 0;
}