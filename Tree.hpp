#ifndef CFAST_TREE_HPP
#define CFAST_TREE_HPP

#include "includes.hpp"

namespace cf {

template<class T>
struct Tree {
	using Pool = std::vector<T>;
	Pool pool;
	TreePtr _last = 0;

	T& operator[](size_t index) {
		return pool[index];
	}
	const T& operator[](size_t index) const {
		return pool[index];
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
		if (!pool.empty()) {
			if (last().children.is_empty()) {
				last().children.begin() = pool.size();
				last().children.end() = pool.size() + 1;
			}
			else last().children.end() += 1;
		}
		return pool.emplace_back(_last, std::forward<Args>(args)...);
	}
	template<class... Args>
	T& push_child_and_move(Args&&... args) {
		if (!pool.empty()) {
			if (last().children.is_empty()) {
				last().children.begin() = pool.size();
				last().children.end() = pool.size() + 1;
			}
			else last().children.end() += 1;
		}
		T& ret = pool.emplace_back(_last, std::forward<Args>(args)...);
		_last = pool.size() - 1;
		return ret;
	}
	template<class... Args>
	T& push_sibling(Args&&... args) {
		up();
		return push_child_and_move(std::forward<Args>(args)...);
	}
	template<class... Args>
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
	}
	void swap_children(TreePtr a, TreePtr b) {
		SyntaxNode& x = pool[a], &y = pool[b];
		for (TreePtr i = x.children.begin(); i < x.children.end(); ++i)
			pool[i].parent = b;
		for (TreePtr i = y.children.begin(); i < y.children.end(); ++i)
			pool[i].parent = a;
		std::swap(x.children, y.children);
	}
	void swap(TreePtr a, TreePtr b) {
		swap_children(a, b);
		std::swap(pool[a], pool[b]);
	}

	TreePtr operator|(const T& x) const {
		return offs(x);
	}

	bool is_empty() const {
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
	size_t offs(const T& x) const {
		return &x - &root();
	}
	void select(const T& x) {
		_last = offs(x);
	}
	void select(TreePtr x) {
		_last = x;
	}
	TreePtr last_position() {
		return _last;
	}

	void insert_subtree(TreePtr position, Tree& tree) {
		TreePtr beg = pool.size();
		pool.insert(pool.back(), tree.pool.begin() + 1, tree.pool.end());
		if (pool[position].children.end() != beg) {
			pool[position].children.begin() = beg;
		}
		pool[position].children.end() = pool.size();
	}

	void save_binary(std::ostream& stream, const char* buffer) {
		size_t offs = (size_t)buffer;
		write(stream, pool.size());
		for (auto node : pool) {
			node.begin().ptr() -= offs;
			node.end().ptr() -= offs;
			write(stream, node);
		}
	}

	void load_binary(std::istream& stream, const char* buffer) {
		size_t offs = (size_t)buffer;
		_last = 0;
		pool.clear();
		size_t s;
		read(stream, s);
		pool.reserve(s);
		pool.resize(s);
		read(stream, root(), s);
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

	TreePrinter(const Tree<T>& _tree, std::ostream& _stream) : tree(_tree), depth(0), stream(_stream) {}

	void print(const T& t) {
		if (&tree.last() == &t) stream << std::string((depth <= 1 ? 0 : depth - 1) * 4, ' ') << ">>> ";
		else stream << std::string(depth * 4, ' ');
		stream << t;
		if (t.children.is_empty()) {
			stream << std::endl;
			return;
		}
		stream << " [" << std::endl;
		++depth;
		for (TreePtr i = t.children.begin(); i < t.children.end(); ++i)
			print(tree[i]);
		--depth;
		stream << std::string(depth * 4, ' ') << "]" << std::endl;
	}
};

template<class T>
std::ostream& operator<<(std::ostream& stream, const Tree<T>& a) {
	if (a.is_empty()) return stream;
	TreePrinter(a, stream).print(a.root());
	return stream;
}

} // namespace cf

#endif // !CFAST_TREE_HPP
