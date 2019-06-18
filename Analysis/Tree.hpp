#ifndef CFAST_TREE_HPP
#define CFAST_TREE_HPP

#include "includes.hpp"

namespace cf {

struct Node {
	TreePtr parent;
	std::vector<TreePtr> children;

	Node(TreePtr _parent) noexcept: parent(_parent) {}

	void SaveBinary(std::ostream& stream) {
		Write(stream, parent);
		Write(stream, children);
	}
	void LoadBinary(std::istream& stream) {
		Read(stream, parent);
		Read(stream, children);
	}
};

template<class T>
struct Tree {
	using Pool = std::vector<T>;
	Pool pool;
	TreePtr _last = 0;

	T& get(size_t index) {
		return pool[index];
	}
	const T& get(size_t index) const {
		return pool[index];
	}
	T& operator[](size_t index) {
		return get(index);
	}
	const T& operator[](size_t index) const {
		return get(index);
	}

	T& root() {
		return pool.front();
	}
	T& last() {
		return pool[_last];
	}
	const T& root() const {
		return pool.front();
	}
	const T& last() const {
		return pool[_last];
	}

	template<class... Args>
	T& PushChild(Args&&... args) {
		const bool empty = IsEmpty();
		T& val = pool.emplace_back(_last, std::forward<Args>(args)...);
		if (!empty)
			last().children.push_back(ptr(val));
		return val;
	}
	template<class... Args>
	T& PushChildAndSelect(Args&&... args) {
		const bool empty = IsEmpty();
		T& val = pool.emplace_back(_last, std::forward<Args>(args)...);
		const TreePtr _ptr = ptr(val);
		if(!empty)
			last().children.push_back(_ptr);
		Select(_ptr);
		return val;
	}
	template<class Iter, class... Args>
	T& InsertAndCapture(Iter first, Iter last, Args&&... args) {
		TreePtr pp = get(*first).parent;
		T& res = pool.emplace_back(pp, std::forward<Args>(args)...);
		T& p = get(pp);

		res.children.insert(
			res.children.end(),
			std::move_iterator(first),
			std::move_iterator(last)
		);
		p.children.erase(first, last);
		p.children.emplace_back(ptr(res));

		for (TreePtr child: res.children) {
			get(child).parent = ptr(res);
		}

		return res;
	}
	template<class Iter, class... Args>
	T& InsertAndCapture(Iter first, Args&& ... args) {
		TreePtr pp = get(*first).parent;
		T& res = pool.emplace_back(pp, std::forward<Args>(args)...);
		T& p = get(pp);
		return InsertAndCapture(first, p.children.end(), std::forward<Args>(args)...);
	}
	template<class... Args>
	T& MoveUp() {
		T& parent = get(last().parent);
		T& pparent = get(last().parent = parent.parent);
		pparent.children.push_back(LastPosition());
		parent.children.erase(std::remove(parent.children.begin(), parent.children.end(), LastPosition()));
		return last();
	}
	void SwapChildren(TreePtr a, TreePtr b) {
		SyntaxNode& x = pool[a], &y = pool[b];
		for (TreePtr i: x.children)
			pool[i].parent = b;
		for (TreePtr i: y.children)
			pool[i].parent = a;
		std::swap(x.children, y.children);
	}
	void SwapItems(TreePtr a, TreePtr b) {
		SwapChildren(a, b);
		std::swap(pool[a], pool[b]);
	}

	TreePtr operator|(const T& x) const {
		return ptr(x);
	}

	bool IsEmpty() const noexcept {
		return pool.empty();
	}

	void GoUp() {
		_last = last().parent;
	}
	size_t ptr(const T& x) const {
		return &x - &root();
	}
	void Select(const T& x) {
		_last = ptr(x);
	}
	void Select(TreePtr x) noexcept {
		_last = x;
	}
	TreePtr LastPosition() noexcept {
		return _last;
	}

	void SaveBinary(std::ostream& stream, const char* buffer) {
		size_t offs = (size_t)buffer;
		Write(stream, pool.size());
		for (auto node : pool) {
			node.begin().ptr() -= offs;
			node.end().ptr() -= offs;
			node.SaveBinary(stream);
		}
	}

	void LoadBinary(std::istream& stream, const char* buffer) {
		size_t offs = size_t(buffer);
		_last = 0;
		Read(stream, pool);
		for (auto& node : pool) {
			node.begin().ptr() += offs;
			node.end().ptr() += offs;
		}
	}
};

template<class T>
struct TreePrinter {
	const Tree<T>& tree;
	std::ostream& stream;
	size_t depth;

	TreePrinter(const Tree<T>& _tree, std::ostream& _stream) noexcept: tree(_tree), depth(0), stream(_stream) {}

	void Print(const T& t) {
		if (&tree.last() == &t) stream << std::string((depth <= 1 ? 0 : depth - 1) * 4, ' ') << ">>> ";
		else stream << std::string(depth * 4, ' ');
		stream << t;
		if (t.children.empty()) {
			stream << std::endl;
			return;
		}
		stream << " [" << std::endl;
		++depth;
		for (TreePtr i: t.children)
			Print(tree[i]);
		--depth;
		stream << std::string(depth * 4, ' ') << "]" << std::endl;
	}
};

template<class T>
std::ostream& operator<<(std::ostream& stream, const Tree<T>& a) {
	if (a.IsEmpty()) return stream;
	TreePrinter<T>(a, stream).Print(a.root());
	return stream;
}

} // namespace cf

#endif // !CFAST_TREE_HPP