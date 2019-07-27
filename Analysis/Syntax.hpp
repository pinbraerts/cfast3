#ifndef CFAST_SYNTAX_HPP
#define CFAST_SYNTAX_HPP

#include "Token.hpp"

template<class R>
class Syntax: public Token<R> {
public:
    using base = Token<R>;
    using base::base;
    using base::type;
    using base::begin;
    using base::end;
    using Type = typename base::Type;
    using Index = typename base::Index;
    using TokenType = typename R::TokenType;
    using Token = typename R::Token;
    using Priority = typename R::Priority;
    
    Priority priority;

    Syntax(): base(), priority(0) { }
    Syntax(Type t): base(t), priority(0) { }
    Syntax(Type t, Priority p): base(t), priority(p) { }
    
    Syntax(Token t): base((Type)(size_t)t.type(), t.begin(), t.end()), priority(0) { }
    Syntax(Token t, Priority p): base((Type)(size_t)t.type(), t.begin(), t.end()), priority(p) { }
    
    /*using base::operator=;*/
    
    /*Syntax& operator=(Token t) {
        type((Type)(size_t)t.type());
        begin(t.begin());
        end(t.end());
        priority = 0;
        return *this;
    }*/
    
    operator base&() {
        return *this;
    }
    operator const base&() const {
        return *this;
    }
};

#endif // !CFAST_SYNTAX_HPP