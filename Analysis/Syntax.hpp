#ifndef CFAST_SYNTAX_HPP
#define CFAST_SYNTAX_HPP

#include "Token.hpp"

namespace cfast {

template<class R>
class Syntax: public Token<R> {
public:
    using base      = Token<R>;
    using Type      = typename base::Type;
    using Traits    = R;
    using TokenType = typename Traits::TokenType;
    using Token     = typename Traits::Token;
    using Priority  = typename Traits::Priority;

    using base::base;
    using base::type;
    using base::begin;
    using base::end;
    
    Priority priority;

    Syntax(): base(), priority(0) { }
    Syntax(Type t): base(t), priority(0) { }
    Syntax(Type t, Priority p): base(t), priority(p) { }
    
    Syntax(Token t): base((Type)(size_t)t.type, t.begin(), t.end()), priority(0) { }
    Syntax(Token t, Priority p): base((Type)(size_t)t.type, t.begin(), t.end()), priority(p) { }
    
    operator base&() {
        return *this;
    }
    operator const base&() const {
        return *this;
    }
};

} // namespace cfast

#endif // !CFAST_SYNTAX_HPP