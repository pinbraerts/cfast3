#ifndef CFAST_VECTOR_NODE_HPP
#define CFAST_VECTOR_NODE_HPP

#include <vector>

#include "int_ptr.hpp"

template<class T, template<class...> class ptr = int_ptr>
struct VectorNode {
    using item_type = T;
    using pointer   = ptr<VectorNode>;

    item_type item;
    std::vector<pointer> children;

    VectorNode() = default;
    VectorNode(const VectorNode&) = default;
    VectorNode(VectorNode&&) = default;

    VectorNode(const item_type& _item) : item(_item) { }
    VectorNode(item_type&& _item) : item(std::move(_item)) { }

    template<class... Args>
    VectorNode(Args&& ... args) : item(std::forward<Args>(args)...) { }
};

#endif // !CFAST_VECTOR_NODE_HPP