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
			return global.Add<Method>(s);
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
	void BindOperator(TreePtr p) {
		const auto& node = syntax[p];
		const auto& c = node.children;
		if (c.empty()) throw std::runtime_error("no no no");
		
		auto i = children_begin<direction>(c), e = children_end<direction>(c);
		if (syntax[*i].priority == node.priority) { // unary operator
			Method* op = FindOperator(*i);
			++i;
			tree.push_child_and_select(op);
			Bind(*i);
			tree.up();
		}
		else Bind(*i);
		for (++i; i < e; ++i) {
			if (syntax[*i].priority != 0) { // binary operator
				Method* op = FindOperator(*i);
				++i;
				Bind(*i);
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
	}

	void BindOperator(TreePtr p, bool direction) {
		if (direction) {
			BindOperator<true>(p);
		}
		else BindOperator<false>(p);
	}

	void Bind(TreePtr p = 0) {
		const SyntaxNode& node = syntax[p];
		if (node.type == Token::End || node.type == Token::Space) return;
		else if (node.children.empty() || (node.children.size() == 1 && syntax[node.children[0]].type == Token::Space)) {
			tree.push_child(Find(node));
		}
		else if(node.priority == 0) {
			tree.push_child_and_select(nullptr);
			for (TreePtr i : node.children)
				Bind(i);
			tree.up();
		}
		else if (node.priority < 100) { // operator
			BindOperator(p, traits.ltr(node.priority));
		}
		else { // brace
			// todo
		}
	}
};

} // namespace cf::rt

#endif // !CFAST_BINDER_HPP
