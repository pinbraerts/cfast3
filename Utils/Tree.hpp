#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>

#include "VectorNode.hpp"

template<class T>
class Tree {
public:
    // Typedefs
    using node_type = VectorNode<T>;
    using pointer   = typename node_type::pointer;

private:
    std::vector<node_type> _pool;

public:
    // Constructors
    Tree() = default;
    Tree(const Tree&) = default;
    Tree(Tree&&) = default;

    // Node flow
    template<class... Args>
    pointer CreateNode(Args&&... args) {
        _pool.emplace_back(std::forward<Args>(args)...);
        return pointer(_pool.size() - 1);
    }

    void DeleteNode(pointer ptr) {
        if (ptr.offset() != _pool.size() - 1)
            throw std::runtime_error("Tree can delete only last created node");
        _pool.pop_back();
    }

    node_type* get(pointer ptr) {
        return &_pool[ptr.offset()];
    }
};

template<class T, class A, class I>
std::vector<T, A> MoveItems(std::vector<T, A>& vec, I first, I last) {
    if(first == vec.begin() && last == vec.end()) {
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
