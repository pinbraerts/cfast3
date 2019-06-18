#ifndef CFAST_BINDER_HPP
#define CFAST_BINDER_HPP

#include "BoundTree.hpp"
#include "BoundTraits.hpp"

namespace cf::rt {

struct Binder {
	BoundTraits traits;
	const SyntaxTree& syntax;
	BoundTree& tree;
	Scope& global;

	Binder(
		const SyntaxTree& _syntax,
		BoundTree& _tree,
		Scope& _global
	) :	syntax(_syntax),
		tree(_tree),
		global(_global) {}

	void BindSingle(TreePtr p) {
		const SyntaxNode& node = syntax[p];
		switch (node.type) {
		case Token::Quote:
			break;
		case Token::String:
			// TODO check for digit, think how to create and name temporary objects
			tree.push_child(global.Find(std::string(node.view())));
			break;
		default: // skip
			break;
		}
	}

	Method* Find(std::string s) {
		Method* m = global.Find(s);
		if (m == nullptr)
			return global.Declare<Method>(s);
		return m;
	}
	Method* Find(const SyntaxNode& node) {
		return Find(std::string(node.view()));
	}
	Method* Find(TreePtr p) {
		return Find(std::string(syntax[p].view()));
	}
	Method* FindOperator(const SyntaxNode& node) {
		return Find("operator"s + std::string(node.view()));
	}
	Method* FindOperator(TreePtr p) {
		return Find("operator"s + std::string(syntax[p].view()));
	}

	template<bool direction>
	auto children_begin(const Children& c) {
		if constexpr (direction) {
			return c.begin();
		}
		else return c.rbegin();
	}
	template<bool direction>
	auto children_end(const Children& c) {
		if constexpr (direction) {
			return c.end();
		}
		else return c.rend();
	}

	template<bool direction>
	void BindBinaryOperator(TreePtr p) {
		const auto& node = syntax[p];
		const auto& c = node.children;
		if (c.empty()) throw std::runtime_error("no no no");
		
		auto i = children_begin<direction>(c), e = children_end<direction>(c);
		BindUnaryOperator<direction>(p, i, e);
		for (++i; i < e; ++i) {
			if (syntax[*i].priority != 0) { // binary operator
				Method* op = FindOperator(*i);
				++i;
				BindUnaryOperator<direction>(p, i, e);
				if constexpr (direction) {
					auto ce = tree.last().children.end();
					tree.insert_capture(ce - 2, ce, op);
				}
				else {
					auto cb = tree.last().children.begin();
					tree.insert_capture(cb, cb + 2, op);
				}
			}
		}
		if constexpr (!direction) {
			auto& c = tree[*tree.last().children.rbegin()].children;
			std::reverse(c.begin(), c.end());
		}
	}

	template<bool direction, class Iter>
	void BindUnaryOperator(TreePtr p, Iter first, Iter last) {
		const auto& node = syntax[p];
		const auto& c = node.children;
		if (c.empty()) throw std::runtime_error("no no no");

		Iter i;
		for (i = first; i < last && syntax[*i].priority == node.priority; ++i);
		if (i >= last) throw std::runtime_error("no no no");

		Bind(*i);
		for (; i > first;) {
			--i;
			Method* op = FindOperator(*i);
			if constexpr(direction)
				tree.insert_capture(tree.last().children.end() - 1, tree.last().children.end(), op);
			else
				tree.insert_capture(tree.last().children.begin(), tree.last().children.begin() + 1, op);
		}
	}

	void BindMultiplie(TreePtr p) {
		const auto& node = syntax[p];
		const auto& c = node.children;
		if (c.empty()) throw std::runtime_error("no no no");

		auto i = c.begin(), e = c.end();
		if (i >= e) return;

		if (syntax[*i].priority == node.priority)
			throw std::runtime_error("leading multiplie");
		Bind(*i);
		++i;
		
		if (i >= e) return;
		if (syntax[*i].priority != node.priority)
			throw std::runtime_error("no no no"); // TODO think about function call
		Method* op = FindOperator(*i);
		tree.select(tree.insert_capture(tree.last().children.end() - 1, tree.last().children.end(), op));
		for (++i; i < e; ++i) {
			if (syntax[*i].priority == node.priority)
				throw std::runtime_error("empty space");
			Bind(*i);
			++i;
			if (i >= e) break;;
			if (syntax[*i].priority != node.priority)
				throw std::runtime_error("no no no"); // TODO think about function call
		}
		tree.up();
	}

	void BindOperator(TreePtr p) {
		const auto& node = syntax[p];
		if (traits.Is(node, traits.binary)) {
			if (traits.Is(node, traits.ltr)) {
				BindBinaryOperator<true>(p);
			}
			else BindBinaryOperator<false>(p);
		}
		else if (traits.Is(node, traits.unary)) {
			if (traits.Is(node, traits.ltr)) {
				BindUnaryOperator<true>(p, node.children.begin(), node.children.end());
			}
			else BindUnaryOperator<false>(p, node.children.rbegin(), node.children.rend());
		}
		else if (traits.Is(node, traits.multiplie)) {
			BindMultiplie(p);
		}
	}

	void Bind(TreePtr p = 0) {
		const SyntaxNode& node = syntax[p];
		if (node.type == Token::End ||
			node.type == Token::Space ||
			node.type == Token::CloseBrace ||
			node.type == Token::OpenBrace) return;
		else if (node.children.empty() || (node.children.size() == 1 && syntax[node.children[0]].type == Token::Space)) {
			tree.push_child(Find(node));
		}
		else if(node.priority == 0) { // TODO check if it's a braces or string literal or root element
			tree.push_child_and_select(Find("Braces"));
			for (TreePtr i : node.children)
				Bind(i);
			tree.up();
		}
		else if (node.priority < 100) { // operator
			BindOperator(p);
		}
	}
};

} // namespace cf::rt

#endif // !CFAST_BINDER_HPP
