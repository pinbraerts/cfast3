#ifndef CFAST_SCOPED_NODE_HPP
#define CFAST_SCOPED_NODE_HPP

#include "Tree.hpp"

namespace cfast {

template<class T>
class ScopedNode {
public:
    // Typedefs
    using tree_type = T;
    using node_type = typename tree_type::node_type;
    using pointer   = typename tree_type::pointer;

private:
    tree_type& _tree;
    std::vector<pointer> _stack;

public:
    // Constructors
    ScopedNode(const ScopedNode&) = default;
    ScopedNode(ScopedNode&&) = default;
    ScopedNode(tree_type& tree) : _tree(tree) { }
    ScopedNode(tree_type& tree, pointer ptr) : _tree(tree), _stack{ ptr } { }

    // Assignment operators
    ScopedNode& operator=(const ScopedNode& other) {
        if (this == &other) return *this;
        if (&_tree != &other._tree)
            throw std::runtime_error("ScopedNodes should have same tree");
        _stack = other._stack;
        return *this;
    }
    ScopedNode& operator=(ScopedNode&& other) {
        if (this == &other) return *this;
        if (&_tree != &other._tree)
            throw std::runtime_error("ScopedNodes should have same tree");
        _stack = std::move(other._stack);
        return *this;
    }

    // Node access
    node_type* get(pointer ptr) {
        return _tree.get(ptr);
    }
    const node_type* get(pointer ptr) const {
        return _tree.get(ptr);
    }
    node_type* get() {
        return _tree.get(_stack.back());
    }
    const node_type* get() const {
        return _tree.get(_stack.back());
    }
    node_type& operator*() {
        return *get();
    }
    const node_type& operator*() const {
        return *get();
    }
    operator node_type* () {
        return get();
    }
    operator const node_type* () const {
        return get();
    }
    node_type* operator->() {
        return get();
    }
    const node_type* operator->() const {
        return get();
    }

    // Node flow
    template<class... Args>
    pointer Create(Args... args) {
        return _tree.CreateNode(std::forward<Args>(args)...);
    }

    void Select(pointer ptr) {
        _stack.push_back(ptr);
    }

    void SelectChild(size_t offset) {
        Select(get()->children[offset]);
    }

    void Push(pointer ptr) {
        return get()->children.push_back(ptr);
    }

    void GoUp() {
        _stack.pop_back();
    }

    bool TryGoUp() {
        if (depth() == 1)
            return false;
        GoUp();
        return true;
    }

    // Combinations
    template<class... Args>
    void CreatePush(Args... args) {
        pointer ptr = Create(std::forward<Args>(args)...);
        Push(ptr);
    }

    template<class... Args>
    void CreateSelect(Args... args) {
        pointer ptr = Create(std::forward<Args>(args)...);
        Select(ptr);
    }

    void PushSelect(pointer ptr) {
        Push(ptr);
        Select(ptr);
    }

    template<class... Args>
    void CreatePushSelect(Args... args) {
        pointer ptr = Create(std::forward<Args>(args)...);
        Push(ptr);
        Select(ptr);
    }

    // Properties
    size_t depth() const {
        return _stack.size();
    }
    pointer current_pointer() const {
        return _stack.back();
    }
    tree_type& tree() {
        return _tree;
    }

    class iterator;

    iterator begin();
    iterator end();
};

template<class C, class T>
std::basic_ostream<C>& operator<<(std::basic_ostream<C>& stream, const ScopedNode<T>& node) {
    return stream << node->item;
}

#if __cplusplus == 201703L // C++17

template<class T> ScopedNode(T&)->ScopedNode<T>;

#endif // C++17

} // namespace cfast;

#include "ScopedNode.Iterator.hpp"

#endif // !CFAST_SCOPED_NODE_HPP
