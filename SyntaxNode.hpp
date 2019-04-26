#ifndef CFAST_SYNTAX_NODE_HPP
#define CFAST_SYNTAX_NODE_HPP

#include "Token.hpp"

namespace cf {

struct SyntaxNode: Token {
	TreePtr parent;
	Working working_area; // except spaces
	Children children;

	SyntaxNode(TreePtr _parent, const Token& tok) : Token(tok), parent(_parent) {}
	SyntaxNode(TreePtr _parent, Token::Type t) : Token(t), parent(_parent) {}
	SyntaxNode() : Token(Token::Error) {} // for vector
	// Node(TreePtr _parent, Token::Type t, Lexer::Iter beg) : Token(t), parent(_parent), children(beg, beg) {}
	// Node(TreePtr _parent, Token::Type t, Lexer::Iter beg, Lexer::Iter en) : Token(t), parent(_parent), children(beg, en) {}

	bool is_root() const {
		return parent == 0;
	}

	auto working() {
		return WeakSlice(children, working_area);
	}

	SyntaxNode& operator=(const Token& other) {
		Token::operator=(other);
		return *this;
	}

	void save_binary(std::ostream& stream) {
		Token::save_binary(stream);
		write(stream, working_area);
		write(stream, children);
	}
	void load_binary(std::istream& stream) {
		Token::load_binary(stream);
		read(stream, working_area);
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
