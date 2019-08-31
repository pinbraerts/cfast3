#ifndef CFAST_BUFFER_HPP
#define CFAST_BUFFER_HPP

#include "defines.hpp"

namespace cfast {

template<class C = char>
struct TextPosition {
    using char_type = C;
    
    size_t line, position;
    
    TextPosition(
        size_t _line,
        size_t _position
    ) : line(_line),
        position(_position) { }
};

template<class C = char, class T = TextPosition<C>>
struct Buffer: std::basic_string<C> {
public:
    using base        = std::basic_string<C>;
    using typename base::size_type;
    using typename base::pointer;
    using char_type   = C;
    using description = T;

    using base::size;
    using base::empty;
    using base::operator[];
    using base::operator+=;
    using base::append;
    using base::data;

private:
    std::vector<size_type> _lines;

    void newline(size_type i) {
        _lines.push_back(i);
    }

    void scan() {
        for (size_type pos = 0; pos < size(); ++pos) {
            if (operator[](pos) == '\n')
                newline(pos);
        }
    }

public:
    // Constructors
    Buffer(const base& str): base(str) {
        scan();
    }
    Buffer(base&& str) : base(std::move(str)) {
        scan();
    }
    Buffer(std::basic_istream<char_type>& input) {
        size_type pos = 0;
        while (input) {
            input.ignore(std::numeric_limits<std::streamsize>::max(), char_type('\n'));
            pos += input.gcount();
            newline(pos);
        }
        input.clear();
        input.seekg(0, std::ios::beg);
        base::reserve(pos);
        base::append(std::istreambuf_iterator<char_type>(input), {});
    }
    Buffer(const Buffer&) = default;
    Buffer(Buffer&&) = default;

    // Assignment operators
    Buffer& operator=(const base& str) {
        if (static_cast<base*>(this) == &str)
            return *this;
        operator=(str);
        scan();
        return *this;
    }
    Buffer& operator=(base&& str) {
        if (static_cast<base*>(this) == &str)
            return *this;
        operator=(std::move(str));
        scan();
        return *this;
    }
    Buffer& operator=(const Buffer&) = default;
    Buffer& operator=(Buffer&&) = default;

    // Factory
    static Buffer FromFile(string_view<char_type> path) {
        std::basic_ifstream<char_type> input(path);
        return Buffer(input);
    }

    description get_description(size_type i) {
        auto it = std::lower_bound(_lines.begin(), _lines.end(), i);
        if (it <= _lines.begin())
            return description(1, i + 1);
        --it;
        return description(it - _lines.begin() + 2, i + 1 - *it);
    }

    const std::vector<size_t>& lines() const {
        return _lines;
    }

    pointer get(size_type i) {
        return &operator[](i);
    }
    
    template<class U>
    string_view<char_type> span(U&& x) {
        return string_view<char_type>(get(x.begin()), get(x.end()));
    }

    void clear() noexcept {
        base::clear();
        _lines.clear();
    }
};

} // namespace cfast

#endif // !CFAST_BUFFER_HPP