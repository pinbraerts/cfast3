#ifndef CFAST_TREE_HPP
#define CFAST_TREE_HPP

#include "includes.hpp"

namespace cf {

struct Node {
	TreePtr parent;
	std::vector<TreePtr> children;

	Node(TreePtr _parent) noexcept: parent(_parent) {}

	void save_binary(std::ostream& stream) {
		write(stream, parent);
		write(stream, children);
	}
	void load_binary(std::istream& stream) {
		read(stream, parent);
		read(stream, children);
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
	T& push_child(Args&&... args) {
		const bool empty = is_empty();
		T& val = pool.emplace_back(_last, std::forward<Args>(args)...);
		if (!empty)
			last().children.push_back(ptr(val));
		return val;
	}
	template<class... Args>
	T& push_child_and_select(Args&&... args) {
		const bool empty = is_empty();
		T& val = pool.emplace_back(_last, std::forward<Args>(args)...);
		const TreePtr _ptr = ptr(val);
		if(!empty)
			last().children.push_back(_ptr);
		select(_ptr);
		return val;
	}
	template<class Iter, class... Args>
	T& insert_capture(Iter first, Iter last, Args&&... args) {
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
	T& insert_capture(Iter first, Args&& ... args) {
		TreePtr pp = get(*first).parent;
		T& res = pool.emplace_back(pp, std::forward<Args>(args)...);
		T& p = get(pp);
		return insert_capture(first, p.children.end(), std::forward<Args>(args)...);
	}
	template<class... Args>
	T& move_up() {
		T& parent = get(last().parent);
		T& pparent = get(last().parent = parent.parent);
		pparent.children.push_back(last_position());
		parent.children.erase(std::remove(parent.children.begin(), parent.children.end(), last_position()));
		return last();
	}
	/*template<class... Args>
	T& push_parent(Args... args) {
		TreePtr parent = last().parent;
		last().parent = _last;
		for (TreePtr i = last().children.begin(); i < last().children.end(); ++i)
			pool[i].parent = pool.size();
		pool.emplace_back(std::move(last()));
		T& ins = *new (&last()) T(parent, std::forward<Args>(args)...);
		ins.children.begin() = pool.size() - 1;
		ins.children.end() = pool.size();
		return ins;
	}*/
	void swap_children(TreePtr a, TreePtr b) {
		SyntaxNode& x = pool[a], &y = pool[b];
		for (TreePtr i: x.children)
			pool[i].parent = b;
		for (TreePtr i: y.children)
			pool[i].parent = a;
		std::swap(x.children, y.children);
	}
	void swap(TreePtr a, TreePtr b) {
		swap_children(a, b);
		std::swap(pool[a], pool[b]);
	}

	TreePtr operator|(const T& x) const {
		return ptr(x);
	}

	bool is_empty() const noexcept {
		return pool.empty();
	}

	void up() {
		_last = last().parent;
	}
	void next() {
		++_last;
	}
	void prev() {
		--_last;
	}
	void down() {
		_last = last().children.begin();
	}
	size_t ptr(const T& x) const {
		return &x - &root();
	}
	void select(const T& x) {
		_last = ptr(x);
	}
	void select(TreePtr x) noexcept {
		_last = x;
	}
	TreePtr last_position() noexcept {
		return _last;
	}

	void save_binary(std::ostream& stream, const char* buffer) {
		size_t offs = (size_t)buffer;
		write(stream, pool.size());
		for (auto node : pool) {
			node.begin().ptr() -= offs;
			node.end().ptr() -= offs;
			node.save_binary(stream);
		}
	}

	void load_binary(std::istream& stream, const char* buffer) {
		size_t offs = size_t(buffer);
		_last = 0;
		read(stream, pool);
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

	void print(const T& t) {
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
			print(tree[i]);
		--depth;
		stream << std::string(depth * 4, ' ') << "]" << std::endl;
	}
};

template<class T>
std::ostream& operator<<(std::ostream& stream, const Tree<T>& a) {
	if (a.is_empty()) return stream;
	TreePrinter<T>(a, stream).print(a.root());
	return stream;
}

} // namespace cf

#endif // !CFAST_TREE_HPP