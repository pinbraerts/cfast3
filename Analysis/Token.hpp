#ifndef CFAST_TOKEN_HPP
#define CFAST_TOKEN_HPP

template<class T>
struct Token {
public:
    using Index = typename T::Index;
    using Type = typename T::Type;
    
private:
    Type _type;
    Index _begin, _end;
    
public:
    Token(): Token(Type::End) { }
    Token(Type t): _type(t), _begin(0), _end(0) { }
    Token(Type t, Index b): _type(t), _begin(b), _end(b) { }
    Token(Type t, Index b, Index e): _type(t), _begin(b), _end(e) { }
    Token(const Token&) = default;
    Token(Token&&) = default;
    
    Token& operator=(const Token&) = default;
    Token& operator=(Token&&) = default;

    Index begin() const {
        return _begin;
    }
    void begin(Index v) {
        _begin = v;
    }
    
    Index end() const {
        return _end;
    }
    void end(Index v) {
        _end = v;
    }
    
    Type type() const {
        return _type;
    }
    void type(Type t) {
        _type = t;
    }
    
    size_t size() const {
        return _end - _begin;
    }
    bool empty() const {
        return _end <= _begin;
    }
};

#endif // !CFAST_TOKEN_HPP
