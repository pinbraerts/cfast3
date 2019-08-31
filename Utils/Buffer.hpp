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

template<class C = char, class T = TextPosition<C>>
struct Buffer {
public:
    using char_type   = C;
    using description = T;
    using pointer     = char_type *;

private:
    std::unique_ptr<C[]> _data;
    size_t _size;
    std::vector<size_t> _lines;

    Buffer(
        pointer&& data,
        size_t sz
    ) : _data(std::move(data)),
        _size(sz) { }

    void newline(size_t i) {
        _lines.push_back(i);
    }

public:
    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&) = default;
    Buffer(std::basic_istream<char_type>& input) {
        size_t pos = 0;
        while (input) {
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
    Buffer(const std::basic_string<char_type>& str) {
        for (size_t pos = 0; pos < str.size(); ++pos) {
            if (str[pos] != '\n')
                continue;
            newline(pos);
        }
        _data = std::unique_ptr<char_type[]>(new char_type[str.size() + 1]);
        str.copy(_data.get(), str.size() + 1);
        _size = str.size();
    }

    static Buffer FromFile(std::basic_string<char_type> path) {
        std::basic_ifstream<char_type> input(path);
        return Buffer(input);
    }

    size_t size() const {
        return _size;
    }

    const std::vector<size_t>& lines() const {
        return _lines;
    }

    pointer get(size_t i) {
        return data() + i;
    }

    pointer data() {
        return _data.get();
    }

    description GetDescription(size_t i) {
        auto it = std::lower_bound(
            _lines.begin(), _lines.end(), i
        );
        if (it <= _lines.begin())
            return description(get(i), 1, i + 1);
        --it;
        return description(
            get(i),
            it - _lines.begin() + 2,
            i + 1 - *it
        );
    }

#if __cplusplus == 201703L

    template<class T>
    std::basic_string_view<char_type> span(T&& x) {
        return std::basic_string_view<char_type>(get(x.begin()), get(x.end()));
    }

#else // ^^^ C++17 | previous versions vvv

    template<class T>
    std::basic_string<char_type> span(T&& x) {
        return std::basic_string<char_type>(get(x.begin()), get(x.end()));
    }

#endif // C++17
};

#endif // !CFAST_BUFFER_HPP