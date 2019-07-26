#ifndef CFAST_LEXER_HPP
#define CFAST_LEXER_HPP

#include <string_view>

#include "Buffer.hpp"
#include "Token.hpp"
#include "TokenTraits.hpp"

template<class C,
    class B = Buffer<C>,
    class L = TokenTraits<B>,
    class T = Token<L>>
struct Lexer {
public:
    using Buffer = B;
    using char_type = typename Buffer::char_type;
    using pointer = typename Buffer::pointer;
    using Raw = typename Buffer::Raw;
    using Index = typename Buffer::Index;
    using Position = typename Buffer::Position;
    using Token = T;
    using Traits = L;
    using Type = typename Traits::Type;
    using View = std::basic_string_view<char_type>;
    
private:
    Buffer& _buffer;
    Index _current;
    Traits _traits;
    
public:
    Lexer(
        Buffer& buffer,
        Index current = 0,
        Traits traits = Traits { }
    ) : _buffer(buffer),
        _current(current),
        _traits(std::move(traits)) { }
    
    char_type& chr() {
        return *_buffer.get(_current);
    }
    
    View view(const Token& t) {
        return View(_buffer.get(t.begin()), t.size());
    }
    
    MatchResult Match(const Token& t) {
        return _traits.Match(t.type(), view(t));
    }
    
    Token Next() noexcept {
        if (_current >= _buffer.size())
            return Token();

        Token x(_traits.GetType(chr()), _current, _current);
        x.end(++_current);
        Token temp = x;

        while (_current < _buffer.size() && x.type() == _traits.GetType(chr())) {
            temp.end(++_current);
            switch (Match(temp)) {
            case MatchResult::Combination:
                x = temp;
                break;
            case MatchResult::Start:
                break;
            case MatchResult::Nothing:
            default:
                _current = x.end();
                return x;
            }
        }

        return x;
    }
};

void TestLexer() {
    auto b = Buffer<char>::FromFile("Lexer.hpp");
    Lexer<char> l(b);
    using L = Lexer<char>;
    L::Token t = l.Next();
    for(; t.type() != L::Traits::Type::End; t = l.Next()) {
        std::cout << ToString(t.type()) << ' ' << '\'' << l.view(t) << '\'' << std::endl;
    }
}

#endif // !CFAST_LEXER_HPP