#ifndef CFAST_TYPES_HPP
#define CFAST_TYPES_HPP

template<size_t N, class T, class... Rest>
struct TypesItem {
    using type = typename TypesItem<N - 1, Rest...>::type;
};

template<class T, class... Rest>
struct TypesItem<0, T, Rest...> {
    using type = T;
};

template<class T, class... Args>
struct TypesIndex {};

template<class T, class R, class... Rest>
struct TypesIndex<T, R, Rest...> {
    static constexpr size_t value = TypesIndex<T, Rest...>::value + 1;
};

template<class T>
struct TypesIndex<T> {
    static constexpr size_t value = 0;
};

template<class T, class... Rest>
struct TypesIndex<T, T, Rest...> {
    static constexpr size_t value = 0;
};

template<class... Args>
struct Types {
    static constexpr size_t Size = sizeof...(Args);
    
    template<size_t N> using At = typename TypesItem<N, Args...>::type;
    
    template<class T>
    static constexpr size_t Index = TypesIndex<T, Args...>::value;
};

#endif // !CFAST_TYPES_HPP
