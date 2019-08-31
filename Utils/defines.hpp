#ifndef CFAST_DEFINES_HPP
#define CFAST_DEFINES_HPP

#ifndef __cplusplus
#error C++ compiler required.
#endif // !__cplusplus

#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <memory>

#include "int_ptr.hpp"

#if __cplusplus == 201703L

#include <string_view>

#endif // C++17

namespace cfast {

#if __cplusplus == 201703L

template<class T>
using string_view = std::basic_string_view<T>;

#else // ^^^ C++17 | previous versions vvv

template<class T>
using string_view = std::basic_string<T>;

#endif // C++17

template<class T, class A, class I>
std::vector<T, A> MoveItems(std::vector<T, A>& vec, I first, I last) {
    if (first == vec.begin() && last == vec.end()) {
        return std::move(vec);
    }

    std::vector<T, A> res;

    std::copy(
        std::make_move_iterator(first),
        std::make_move_iterator(last),
        std::back_inserter(res)
    );
    vec.erase(first, last);
    return res;
}

}; // namespace cfast

#endif // !CFAST_DEFINES_HPP
