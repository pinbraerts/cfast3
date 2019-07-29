#ifndef CFAST_ALLOCATOR_HPP
#define CFAST_ALLOCATOR_HPP

#include <vector>

template<class T>
class PoolAllocator {
public:
    using size_type = size_t;
    using difference_type = size_t;
    using pointer = size_t;
    using const_pointer = size_t;
    using reference = T&;
    using const_reference = const T&;
    using value_type = T;
    
protected:
    std::vector<T> _pool;
    size_t _last = 0;
    
public:
    pointer allocate(size_type n) {
        _last += n;
        _pool.reserve(_last);
        return _last - 1;
    }
    
    void deallocate(pointer p, size_type n) {
        _pool.resize(_pool.size() - n);
        _last -= n;
    }
    
    void destroy(pointer p) {
        T t = std::move(_pool[p]);
    }

    template<class... Args>
    void construct(pointer p, Args&&... args) {
        _pool.emplace_back(std::forward<Args>(args)...);
    }
    
    const_pointer address(const_reference r) const {
        return &r - _pool.data();
    }
    pointer address(reference r) {
        return &r - _pool.data();
    }
    
    reference get(pointer p) {
        return _pool[p];
    }
    
    const_reference get(const_pointer p) const {
        return _pool[p];
    }
    
    static constexpr size_type max_size() {
        return ~(size_t)0;
    }
};

#endif // !CFAST_ALLOCATOR_HPP