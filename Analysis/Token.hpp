#ifndef CFAST_TOKEN_HPP
#define CFAST_TOKEN_HPP

namespace cfast {

template<class T>
struct Token {
public:
    using Type  = typename T::Type;
    
private:
    size_t _begin, _end;
    
public:
    Type type;

    Token(): Token(Type::End) { }
    Token(Type t): type(t), _begin(0), _end(0) { }
    Token(Type t, size_t b): type(t), _begin(b), _end(b) { }
    Token(Type t, size_t b, size_t e): type(t), _begin(b), _end(e) { }
    Token(const Token&) = default;
    Token(Token&&) = default;
    
    Token& operator=(const Token&) = default;
    Token& operator=(Token&&) = default;

    size_t begin() const {
        return _begin;
    }
    void begin(size_t v) {
        _begin = v;
    }
    
    size_t end() const {
        return _end;
    }
    void end(size_t v) {
        _end = v;
    }
    
    size_t size() const {
        return _end - _begin;
    }
    bool empty() const {
        return _end <= _begin;
    }
};

} // namespace cfast

#endif // !CFAST_TOKEN_HPP
