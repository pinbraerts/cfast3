#ifndef CFAST_BUFFER_HPP
#define CFAST_BUFFER_HPP

#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <memory>
#include <sstream>

template<class C = char>
struct TextPosition {
    using char_type = C;
    
    char_type* chr;
    size_t line, position;
    
    TextPosition(
        char_type* _c,
        size_t _line,
        size_t _position
    ) : chr(_c),
        line(_line),
        position(_position) { }
};

template<class C = char,
    class P = std::unique_ptr<C[]>,
    class D = C*,
    class I = size_t,
    class T = TextPosition<C>>
struct Buffer {
public:
    using char_type = C;
    using pointer = P;
    using Raw = D;
    using Index = I;
    using Position = T;

private:
    pointer _data;
    size_t _size;
    std::vector<Index> _lines;
    
    Buffer(
        pointer&& data,
        size_t sz
    ) : _data(std::move(data)),
        _size(sz) { }
    
public:
    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&) = default;
    Buffer(std::basic_istream<char_type>& input) {
        size_t pos = 0;
        while(input) {
            input.ignore(std::numeric_limits<std::streamsize>::max(), char_type('\n'));
            pos += input.gcount();
            newline(pos);
        }
        input.clear();
        input.seekg(0, std::ios::beg);
        _data = std::unique_ptr<char_type[]>(new char_type[pos + 1]);
        input.read(data(), pos);
        _data[pos] = '\0';
        _size = pos;
    }

    static Buffer FromFile(std::basic_string<char_type> path) {
        std::basic_ifstream<char_type> input(path);
        return Buffer(input);
    }
    
    static Buffer FromString(std::basic_string<char_type> str) {
        std::basic_istringstream<char_type> input;
        input.str(str);
        return Buffer(input);
    }

    void newline(Index i) {
        _lines.push_back(i);
    }
    
    size_t size() const {
        return _size;
    }
    
    const std::vector<Index>& lines() const {
        return _lines;
    }

    Raw get(Index i) {
        return data() + i;
    }
    
    Raw data() {
        return _data.get();
    }
    
    Position GetPosition(Index i) {
        auto it = std::lower_bound(
            _lines.begin(), _lines.end(), i
        );
        if(it >= _lines.end())
            return Position(get(i), 1, i + 1);
        return Position(
            get(i),
            it - _lines.begin() + 1,
            i - *it + 1
        );
    }
};

void TestBuffer() {
    auto b = Buffer<char>::FromFile("Buffer.hpp");
    std::cout << b.data() << std::endl;
    for(size_t i: b.lines())
        std::cout << i << ' ';
}

#endif // !CFAST_BUFFER_HPP