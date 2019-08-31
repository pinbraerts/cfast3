#ifndef CFAST_SCOPED_NODE_ITERATOR_HPP
#define CFAST_SCOPED_NODE_ITERATOR_HPP

#include "ScopedNode.hpp"

namespace cfast {

template<class T>
class ScopedNode<T>::iterator {
private:
    std::unique_ptr<ScopedNode> _node;
    std::vector<size_t> _positions;

    size_t& last_position() {
        return _positions.back();
    }

    void descend() {
        if (_positions.empty())
            _positions.push_back(0);
        while (!(*_node)->children.empty()) {
            if (last_position() >= (*_node)->children.size())
                break;
            _node->SelectChild(last_position());
            _positions.push_back(0);
        }
    }

    bool ascend() {
        if (_positions.empty())
            return false;
        if ((*_node)->children.empty()) {
            if (_node->TryGoUp())
                _positions.pop_back();
            else return false;
        }
        while (
            !_positions.empty() &&
            last_position() >= (*_node)->children.size()
            ) {
            _positions.pop_back();
            if (!_node->TryGoUp())
                return false;
        }
        return !_positions.empty();
    }

public:
    iterator() :
        _node(nullptr) { }

    iterator(ScopedNode& node) :
        _node(std::make_unique<ScopedNode>(node.tree(), node.current_pointer())) {
        descend();
    }

    iterator(tree_type& tree, pointer ptr) :
        _node(std::make_unique<ScopedNode>(tree, ptr)) {
        descend();
    }

    ScopedNode& operator*() {
        return *_node;
    }
    iterator& operator++() {
        if (!ascend()) {
            _node = nullptr;
            return *this;
        }
        ++last_position();
        descend();
        return *this;
    }
    bool operator!=(const iterator& other) {
        return _node != other._node;
    }
};

template<class T>
typename ScopedNode<T>::iterator ScopedNode<T>::begin() {
    return iterator(*this);
}

template<class T>
typename ScopedNode<T>::iterator ScopedNode<T>::end() {
    return iterator();
}

} // namespace cfast

#endif // !CFAST_SCOPED_NODE_ITERATOR_HPP
