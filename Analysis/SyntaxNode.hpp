#ifndef CFAST_SYNTAX_NODE_HPP
#define CFAST_SYNTAX_NODE_HPP

#include "Token.hpp"
#include "Tree.hpp"

namespace cf {

struct SyntaxNode: Node, Token {
	priority_t priority = 0;

	SyntaxNode(TreePtr _parent, const Token& tok) noexcept: Token(tok), Node(_parent) {}
	SyntaxNode(TreePtr _parent, const Token& tok, priority_t _priority) noexcept : Token(tok), Node(_parent), priority(_priority) {}
	SyntaxNode(TreePtr _parent, Token::Type t) noexcept : Token(t), Node(_parent) {}
	SyntaxNode(TreePtr _parent, Token::Type t, priority_t _priority) noexcept : Token(t), Node(_parent), priority(_priority) {}
	SyntaxNode() noexcept : Token(Token::Error), Node(0) {} // for vector
	// Node(TreePtr _parent, Token::Type t, Lexer::Iter beg) : Token(t), parent(_parent), children(beg, beg) {}
	// Node(TreePtr _parent, Token::Type t, Lexer::Iter beg, Lexer::Iter en) : Token(t), parent(_parent), children(beg, en) {}

	SyntaxNode& operator=(const Token& other) noexcept {
		Token::operator=(other);
		return *this;
	}

	void SaveBinary(std::ostream& stream) {
		Node::SaveBinary(stream);
		Token::SaveBinary(stream);
		Write(stream, priority);
	}
	void LoadBinary(std::istream& stream) {
		Node::LoadBinary(stream);
		Token::LoadBinary(stream);
		Read(stream, priority);
	}
};

std::ostream& operator<<(std::ostream& stream, const SyntaxNode& node) {
	return node.IsEmpty() ?
		stream << Token::TypeToString(node.type) :
		stream << (const Token&)node;
}

} // namespace cf

#endif // !CFAST_SYNTAX_NODE_HPP
