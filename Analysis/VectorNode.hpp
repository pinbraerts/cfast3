#ifndef CFAST_VECTOR_NODE_HPP
#define CFAST_VECTOR_NODE_HPP

#include <vector>

struct EmplaceTag { };

template<class T, template<class> class A>
struct VectorNode {
public:
    using Item = T;
    using pointer = typename A<VectorNode>::pointer;
    using Children = std::vector<pointer>;
    
    Item item;
    Children children;
    
    VectorNode() = default;
    VectorNode(const VectorNode&) = default;
    VectorNode(VectorNode&& n) = default;
    
    VectorNode(const Item& i): item(i) { }
    VectorNode(Item&& i): item(std::move(i)) { }
    
    VectorNode& operator=(const VectorNode&) = default;
    VectorNode& operator=(VectorNode&&) = default;
    
    VectorNode& operator=(const Item& other) {
        if(&other == &item) return *this;
        item = other;
        return *this;
    }
    VectorNode& operator=(Item&& other) {
        if(&other == &item) return *this;
        item = std::move(other);
        return *this;
    }
    
    template<class... Args>
    VectorNode(EmplaceTag, Args&&... args): item(std::forward<Args>(args)...) { }
    
    operator Item&() {
        return item;
    }
    operator const Item&() const {
        return item;
    }
};

#endif // !CFAST_VECTOR_NODE_HPP