#ifndef CFAST_SYNTAX_HPP
#define CFAST_SYNTAX_HPP

#include "Token.hpp"

namespace cfast {

template<class R>
class Syntax: public Token<R> {
public:
    using base       = Token<R>;
    using Traits     = R;
    using typename base::Type;
    using TokenType  = typename Traits::TokenType;
    using token_type = typename Traits::token_type;
    using Priority   = typename Traits::Priority;

    using base::base;
    using base::type;
    using base::begin;
    using base::end;
    
    Priority priority;

    Syntax(): base(), priority(0) { }
    Syntax(Type t): base(t), priority(0) { }
    Syntax(Type t, Priority p): base(t), priority(p) { }
    
    Syntax(token_type t): base((Type)(size_t)t.type, t.begin(), t.end()), priority(0) { }
    Syntax(token_type t, Priority p): base((Type)(size_t)t.type, t.begin(), t.end()), priority(p) { }

    operator base&() {
        return *this;
    }
    operator const base&() const {
        return *this;
    }
};

} // namespace cfast

#endif // !CFAST_SYNTAX_HPP