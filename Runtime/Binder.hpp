#ifndef CFAST_BINDER_HPP
#define CFAST_BINDER_HPP

#include "BoundTree.hpp"
#include "BoundTraits.hpp"

namespace cf::rt {

struct Binder {
	BoundTraits traits;
	const SyntaxTree& syntax;
	BoundTree& tree;
	Flow& flow;

	Binder(
		const SyntaxTree& _syntax,
		BoundTree& _tree,
		Flow& _flow
	) :	syntax(_syntax),
		tree(_tree),
		flow(_flow) {}

	Method* Find(std::string_view s) {
		Method* m = flow.Resolve(std::string(s));
		if (m == nullptr)
			return flow.DeclareMethod(std::string(s));
		return m;
	}
	//Method* Find(const SyntaxNode& node) {
	//	return Find(std::string_view(node));
	//}
	Method* Find(TreePtr p) {
		return Find(syntax[p]);
	}
	Method* FindOperator(const SyntaxNode& node) {
		return Find("operator"s + std::string(node));
	}
	Method* FindOperator(TreePtr p) {
		return Find("operator"s + std::string(syntax[p]));
	}

	template<bool direction>
	auto ChildrenBegin(const Children& c) {
		if constexpr (direction) {
			return c.begin();
		}
		else return c.rbegin();
	}
	template<bool direction>
	auto ChildrenEnd(const Children& c) {
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
		
		auto i = ChildrenBegin<direction>(c), e = ChildrenEnd<direction>(c);
		BindUnaryOperator<direction>(p, i, e);
		for (++i; i < e; ++i) {
			if (syntax[*i].priority != 0) { // binary operator
				Method* op = FindOperator(*i);
				++i;
				BindUnaryOperator<direction>(p, i, e);
				if constexpr (direction) {
					auto ce = tree.last().children.end();
					tree.InsertAndCapture(ce - 2, ce, op);
				}
				else {
					auto cb = tree.last().children.begin();
					tree.InsertAndCapture(cb, cb + 2, op);
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
				tree.InsertAndCapture(tree.last().children.end() - 1, tree.last().children.end(), op);
			else
				tree.InsertAndCapture(tree.last().children.begin(), tree.last().children.begin() + 1, op);
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
		tree.Select(tree.InsertAndCapture(tree.last().children.end() - 1, tree.last().children.end(), op));
		for (++i; i < e; ++i) {
			if (syntax[*i].priority == node.priority)
				throw std::runtime_error("empty space");
			Bind(*i);
			++i;
			if (i >= e) break;;
			if (syntax[*i].priority != node.priority)
				throw std::runtime_error("no no no"); // TODO think about function call
		}
		tree.GoUp();
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

	void BindQuote(TreePtr p) {
		const auto& node = syntax[p];
		size_t first = syntax[node.children[0]].type == Token::Space;
		std::string string_literal =
			node.children.size() == 3 + first ?
				std::string(syntax[node.children[1 + first]]) :
				"";
		tree.PushChild(flow.DeclareLiteral(string_literal)); // TODO use another method
	}

	void BindBraces(TreePtr p) {
		const SyntaxNode& node = syntax[p];
		size_t first = syntax[node.children[0]].type == Token::Space;
		std::string br = std::string(syntax[node.children[first]]) + std::string(syntax[*node.children.rbegin()]);
		Method* method = Find("operator"s + br);

		// TODO match function call with previous symbol
		// TODO {} can lead to flow.DeclareTypeAndEnter(...)
		// TODO operator() [ operator,[ ... ] ] -> operator() [ ... ]

		tree.PushChildAndSelect(method);
		for (TreePtr i : node.children)
			Bind(i);
		tree.GoUp();
	}

	void Bind(TreePtr p = 0) {
		const SyntaxNode& node = syntax[p];
		if(p == 0) {
			tree.PushChildAndSelect(nullptr);
			for (TreePtr i : node.children)
				Bind(i);
			tree.GoUp();
			return;
		}
		if (node.type == Token::End ||
			node.type == Token::Space ||
			node.type == Token::CloseBrace ||
			node.type == Token::OpenBrace) return;
		else if (node.children.empty() || (node.children.size() == 1 && syntax[node.children[0]].type == Token::Space)) {
			if (isdigit(node.begin().chr()))
				tree.PushChild(flow.DeclareLiteral(std::stod(std::string(node))));
			else
				tree.PushChild(Find(node));
		}
		else if(node.priority == 0) { // TODO check if it's a braces or string literal
			switch (node.type) {
			case Token::Quote:
				BindQuote(p);
				break;
			case Token::Container:
				BindBraces(p);
				break;
			}
		}
		else if (node.priority < 100) { // operator
			BindOperator(p);
		}
	}
};

} // namespace cf::rt

#endif // !CFAST_BINDER_HPP
