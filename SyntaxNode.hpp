#ifndef CFAST_SYNTAX_NODE_HPP
#define CFAST_SYNTAX_NODE_HPP

#include "Token.hpp"

namespace cf {

struct SyntaxNode: Token {
	TreePtr parent;
	size_t first_no_space = 0;
	priority_t priority = 0;
	Children children;

	SyntaxNode(TreePtr _parent, const Token& tok) noexcept: Token(tok), parent(_parent) {}
	SyntaxNode(TreePtr _parent, Token::Type t) noexcept : Token(t), parent(_parent) {}
	SyntaxNode(TreePtr _parent, Token::Type t, priority_t _priority) noexcept : Token(t), parent(_parent), priority(_priority) {}
	SyntaxNode() noexcept : Token(Token::Error), parent(0) {} // for vector
	// Node(TreePtr _parent, Token::Type t, Lexer::Iter beg) : Token(t), parent(_parent), children(beg, beg) {}
	// Node(TreePtr _parent, Token::Type t, Lexer::Iter beg, Lexer::Iter en) : Token(t), parent(_parent), children(beg, en) {}

	auto working() noexcept {
		return slice(children, first_no_space);
	}

	SyntaxNode& operator=(const Token& other) noexcept {
		Token::operator=(other);
		return *this;
	}

	void save_binary(std::ostream& stream) {
		Token::save_binary(stream);
		write(stream, first_no_space);
		write(stream, priority);
		write(stream, children);
	}
	void load_binary(std::istream& stream) {
		Token::load_binary(stream);
		read(stream, first_no_space);
		read(stream, priority);
		read(stream, children);
	}
};

std::ostream& operator<<(std::ostream& stream, const SyntaxNode& node) {
	return node.is_empty() ?
		stream << Token::type2str(node.type) :
		stream << (const Token&)node;
}

} // namespace cf

#endif // !CFAST_SYNTAX_NODE_HPP
