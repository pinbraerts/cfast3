#include <iostream>
#include <vector>
#include <algorithm>

#include "PoolAllocator.hpp"
#include "VectorNode.hpp"

template<class T> void DeleteTree(T&);

template<class T,
    template<class> class A = std::allocator,
    class N = VectorNode<T, A>>
class Tree: A<N> {
public:
    using Node = N;
    using Allocator = A<N>;
    using Item = typename Node::Item;
    using Children = typename Node::Children;
    using pointer = typename Allocator::pointer;
    using reference = typename Allocator::reference;

private:
    pointer _root = 0;

public:
    pointer root() {
        return _root;
    }
    
    bool empty() const {
        return _root == 0;
    }
    
    template<class... Args>
    pointer CreateNode(Args&&... args) {
        pointer p = Allocator::allocate(1);
        Allocator::construct(p, EmplaceTag{}, std::forward<Args>(args)...);
        if(empty())
            _root = p;
        return p;
    }
    
    void DeleteNode(pointer p) {
        if(p == _root)
            _root = nullptr;
        Allocator::destroy(p);
        Allocator::deallocate(p, 1);
    }
    
    static constexpr reference get(pointer p) {
        return *p;
    }
    
    ~Tree() {
        DeleteTree(*this);
    }
};

template<class T>
class Tree<T, PoolAllocator, VectorNode<T, PoolAllocator>>: PoolAllocator<VectorNode<T, PoolAllocator>> {
public:
    using Node = VectorNode<T, PoolAllocator>;
    using Allocator = PoolAllocator<VectorNode<T, PoolAllocator>>;
    using Item = typename Node::Item;
    using Children = typename Node::Children;
    using pointer = typename Allocator::pointer;
    using reference = typename Allocator::reference;
    
    constexpr static pointer root() {
        return 0;
    }
    
    bool empty() const {
        return Allocator::_pool.empty();
    }
    
    template<class... Args>
    pointer CreateNode(Args&&... args) {
        pointer p = Allocator::allocate(1);
        Allocator::construct(p, EmplaceTag{}, std::forward<Args>(args)...);
        return p;
    }
    
    void DeleteNode(pointer p) {
        Allocator::destroy(p);
        Allocator::deallocate(p, 1);
    }
    
    using Allocator::get;
    
    ~Tree() = default;
};

template<class T>
class TreeWalker {
public:
    using Tree = T;
    using Node = typename Tree::Node;
    using Item = typename Tree::Item;
    using Children = typename Tree::Children;
    using pointer = typename Tree::pointer;
    using reference = typename Tree::reference;

private:
    Tree& _tree;
    std::vector<pointer> _parents;

public:    
    reference get(pointer p) {
        return _tree.get(p);
    }
    pointer current_pointer() {
        return _parents.back();
    }
    reference current() {
        return get(current_pointer());
    }
    reference root() {
        return get(_parents.front());
    }
    Tree& tree() {
        return _tree;
    }

    TreeWalker(Tree& tree): _tree(tree) {
        if(!tree.empty())
            _parents.push_back(tree.root());
    }
    TreeWalker(TreeWalker&&) = default;
    TreeWalker(const TreeWalker&) = default;
    
    size_t depth() const {
        return _parents.size();
    }
    bool empty() const {
        return _parents.empty();
    }
    
    void Select(pointer p) {
        _parents.push_back(p);
    }
    void PushChild(pointer p) {
        current().children.push_back(p);
    }
    template<class... Args>
    pointer EmplaceChild(Args&&... args) {
        pointer i = _tree.CreateNode(std::forward<Args>(args)...);
        if(!empty())
            PushChild(i);
        return i;
    }
    
    template<class... Args>
    pointer EmplaceChildAndSelect(Args&&... args) {
        pointer i = EmplaceChild(std::forward<Args>(args)...);
        Select(i);
        return i;
    }
    
    void CutChildren() {
        if(empty() || current().children.empty())
            return;
        for(pointer p: current().children) {
            Select(p);
            CutChildren();
            GoUp();
            _tree.DeleteNode(p); // keep nullptr in children...
        }
        current().children.clear(); // ...then clear them all
    }
    void GoUp() {
        _parents.pop_back();
    }
    void GoToRoot() {
        _parents.resize(1);
    }
    bool TryGoUp() {
        if(depth() <= 1) // if used in while, _parents always contains at least one element, so it's allowed to push children (or none if it had non before)
            return false;
        GoUp();
        return true;
    }
};

#ifdef __cplusplus
#if __cplusplus == 201703L

template<class T> TreeWalker(T&) -> TreeWalker<T>;

#endif // __cplusplus == 201703L
#endif // __cplusplus

template<class T>
void DeleteTree(T& t) {
    if(!t.empty()) {
        TreeWalker<T>(t).CutChildren();
        t.DeleteNode(t.root());
    }
}

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

template<class N>
void PrintItem(std::ostream& s, const N& item) {
    s << item;
}

template<class T, class F=decltype(&PrintItem<typename T::Item>)>
struct TreePrinter {
    using Tree = T;
    using Node = typename Tree::Node;
    using Item = typename Tree::Item;
    using pointer = typename Tree::pointer;
    
private:
    Tree& _tree;
    std::ostream& _stream;
    F f;
    
    void _print(pointer p, size_t depth) {
        Node& x = _tree.get(p);
        std::string prefix(depth * 2, ' ');
        _stream << prefix;
        f(_stream, x.item);
        if(x.children.empty()) {
            _stream << std::endl;
            return;
        }
        _stream << ' ' << '{' << std::endl;
        for(pointer i: x.children) {
            _print(i, depth + 1);
        }
        _stream << prefix << '}' << std::endl;
    }
    
public:
    TreePrinter(Tree& tree, std::ostream& stream, F p = &PrintItem<Item>): _tree(tree), _stream(stream), f(p) {}

    void print() {
        _print(_tree.root(), 0);
    }
};

void TestTree() {
    Tree<int, std::allocator> t;
    TreeWalker<decltype(t)> w(t);
    
    std::string delim(10, '=');
    auto log = [&] (std::string msg) {
        std::cout << delim << ' ' << msg << ' ' << delim << std::endl;
        TreePrinter<decltype(t)>(t, std::cout).print();
        std::cout << std::endl;
    };
    
    w.EmplaceChildAndSelect(0);
        w.EmplaceChildAndSelect(1);
            w.EmplaceChild(2);
            w.EmplaceChild(3);
        w.GoUp();
        
        w.EmplaceChildAndSelect(4);
            w.EmplaceChild(5);
            w.EmplaceChildAndSelect(6);
                w.EmplaceChild(7);
            w.GoUp();
    
    log("Initialized tree");
    
    auto p = t.CreateNode(8);
    auto& n = t.get(p);
    auto& c = w.current().children;
    n.children = MoveItems(c, c.begin(), c.end());
    
    w.PushChild(p);
    
    log("Captured item");
}