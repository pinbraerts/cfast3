#ifndef INT_PTR_HPP
#define INT_PTR_HPP

template<class T, class S = size_t>
class int_ptr {
public:
    // Typedefs
    using size_type       = S;
    using pointer         = int_ptr;
    using element_type    = T;
    using difference_type = size_type;

private:
    size_type _offset;

public:
    // Constructors
    int_ptr() : _offset(0) { }
    int_ptr(nullptr_t) : _offset(0) { }
    int_ptr(size_type offset) : _offset(offset) { }
    int_ptr(int integer) : _offset(integer) { }

    // Boolean operators
    template<class U>
    std::enable_if_t<std::is_convertible_v<T*, U*>, bool> operator==(int_ptr<U> ptr) const {
        return _offset == ptr._offset;
    }
    template<class U>
    std::enable_if_t<std::is_convertible_v<T*, U*>, bool> operator!=(int_ptr<U> ptr) const {
        return _offset != ptr._offset;
    }
    template<class U>
    std::enable_if_t<std::is_convertible_v<T*, U*>, bool> operator<=(int_ptr<U> ptr) const {
        return _offset <= ptr._offset;
    }
    template<class U>
    std::enable_if_t<std::is_convertible_v<T*, U*>, bool> operator>=(int_ptr<U> ptr) const {
        return _offset >= ptr._offset;
    }
    template<class U>
    std::enable_if_t<std::is_convertible_v<T*, U*>, bool> operator<(int_ptr<U> ptr) const {
        return _offset < ptr._offset;
    }
    template<class U>
    std::enable_if_t<std::is_convertible_v<T*, U*>, bool> operator>(int_ptr<U> ptr) const {
        return _offset > ptr._offset;
    }

    // Arithmetic operators
    int_ptr operator-(size_type offset) const {
        return int_ptr(_offset - offset);
    }
    int_ptr operator+(size_type offset) const {
        return int_ptr(_offset + offset);
    }
    template<class U>
    std::enable_if_t<std::is_convertible_v<T*, U*>, difference_type> operator-(int_ptr<U> ptr) const {
        return _offset - ptr._offset;
    }

    // Assignment operators
    template<class U>
    std::enable_if_t<std::is_convertible_v<T*, U*>, int_ptr&> operator=(int_ptr<U> ptr) {
        _offset = ptr._offset;
        return *this;
    }
    int_ptr& operator+=(size_type offset) {
        _offset += offset;
        return *this;
    }
    int_ptr& operator-=(size_type offset) {
        _offset -= offset;
        return *this;
    }

    // Cast operators
    explicit operator size_type&() {
        return _offset;
    }
    explicit operator const size_type& () const {
        return _offset;
    }

    // Properties
    size_type offset() const {
        return _offset;
    }
};

#endif // !INT_PTR_HPP
