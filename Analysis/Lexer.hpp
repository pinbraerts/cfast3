#ifndef CFAST_LEXER_HPP
#define CFAST_LEXER_HPP

#include "../Utils/Buffer.hpp"
#include "Token.hpp"
#include "TokenTraits.hpp"

namespace cfast {

template<class C,
    class L = TokenTraits<C>,
    class T = Token<L>>
struct Lexer {
public:
    // Typedefs
    using char_type   = C;
    using Buffer      = Buffer<char_type>;
    using description = typename Buffer::description;
    using pointer     = typename Buffer::pointer;
    using Token       = T;
    using Traits      = L;
    using Type        = typename Traits::Type;
    
private:
    Buffer& _buffer;
    size_t _current;
    Traits _traits;
    
public:
    // Constructor
    Lexer(
        Buffer& buffer,
        size_t current = 0,
        Traits traits = Traits { }
    ) : _buffer(buffer),
        _current(current),
        _traits(std::move(traits)) { }
    
    // Properties
    char_type& chr() {
        return *_buffer.get(_current);
    }
    Buffer& buffer() {
        return _buffer;
    }
    
    MatchResult Match(const Token& t) {
        return _traits.Match(t.type, _buffer.span(t));
    }
    
    Token Next() noexcept {
        if (_current >= _buffer.size())
            return Token();

        Token x(_traits.GetType(chr()), _current, _current);
        x.end(++_current);
        Token temp = x;

        while (_current < _buffer.size() && x.type == _traits.GetType(chr())) {
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

} // namespace cfast

#endif // !CFAST_LEXER_HPP