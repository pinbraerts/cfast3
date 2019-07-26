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

private:
    Priority _priority;
    
    constexpr static Priority DefaultPriority(Type t) {
        switch(t) {
        case Type::OpenBrace: return 100;
        case Type::ContainerBrace: return 100;
        default: return 0;
        }
    }

public:
    Syntax(): base(), _priority(DefaultPriority(type())) { }
    Syntax(Type t): base(t), _priority(DefaultPriority(t)) { }
    Syntax(Type t, Priority p): base(t), _priority(p) { }
    
    Syntax(Token t): base((Type)(size_t)t.type(), t.begin(), t.end()), _priority(DefaultPriority(type())) { }
    Syntax(Token t, Priority p): base((Type)(size_t)t.type(), t.begin(), t.end()), _priority(p) { }
    
    using base::operator=;
    
    Syntax& operator=(Token t) {
        type((Type)(size_t)t.type());
        begin(t.begin());
        end(t.end());
        priority(DefaultPriority(type()));
        return *this;
    }
    
    Priority priority() const {
        return _priority;
    }
    void priority(Priority p) {
        _priority = p;
    }
    
    operator base&() {
        return *this;
    }
    operator const base&() const {
        return *this;
    }
};

#endif // !CFAST_SYNTAX_HPP