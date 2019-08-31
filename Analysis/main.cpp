#include <iostream>

#include "Lexer.hpp"

using namespace cfast;

void TestLexer() {
    auto b = Buffer<char>::FromFile("Lexer.hpp");
    Lexer<char> l(b);
    using L = Lexer<char>;
    L::Token t = l.Next();
    for(; t.type() != L::Traits::Type::End; t = l.Next()) {
        std::cout << ToString(t.type()) << ' ' << '\'' << l.buffer().span(t) << '\'' << std::endl;
    }
}

int main() {
    TestLexer();
    //TestParser();
    return 0;
}