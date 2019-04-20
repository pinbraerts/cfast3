#include <iomanip>

#include "Lexer.hpp"
#include "Tree.hpp"

namespace cf {

struct Node: Token {
	TreePtr parent;
	Children children = Children(0, 0);

	Node(TreePtr _parent, const Token& tok) : Token(tok), parent(_parent) {}
	Node(TreePtr _parent, Token::Type t) : Token(t), parent(_parent) {}
	//Node(TreePtr _parent, Token::Type t, Lexer::Iter beg) : Token(t), parent(_parent), children(beg, beg) {}
	//Node(TreePtr _parent, Token::Type t, Lexer::Iter beg, Lexer::Iter en) : Token(t), parent(_parent), children(beg, en) {}

	bool is_root() const {
		return parent == 0;
	}

	Node& operator=(Token& other) {
		Token::operator=(other);
		return *this;
	}
};

std::ostream& operator<<(std::ostream& stream, const Node& node) {
	return node.is_empty() ?
		stream << Token::type2str(node.type) :
		stream << (const Token&)node;
}

struct Parser {
	Tree<Node>& tree;
	Lexer::Iter iter;

	Parser(Tree<Node>& _tree, Lexer& lexer): tree(_tree), iter(lexer.begin()) {
		tree.push_child(Token::Container); // root
	}

	struct Error {
		std::string message;

		Error(std::string msg): message(std::move(msg)) {}
		Error() = default;

		static Error ok() {
			return Error();
		}

		bool is_ok() const {
			return message.empty();
		}
	};

	void skip_spaces() {
		for (; iter->type != Token::End && iter->type == Token::Space; ++iter)
			tree.push_child(*iter);
	}

	void skip_spaces_and_lines() {
		for (; iter->type != Token::End && (iter->type == Token::Space || iter->type == Token::Line); ++iter)
			tree.push_child(*iter);
	}

	void deep_spaces() {
		TreePtr pchild = tree.last().children.end() - 1;
		Node& child = tree.pool[pchild];
		child.children.begin() = pchild;
		child.children.end() = pchild + 1;
		tree.swap_children(tree.last_position(), pchild);
		child.children.end() -= 1;
	}

	void parseQuote() {
		Source opening = tree.push_child(*iter); // opening
		deep_spaces();
		TreePtr body = tree | tree.push_child(Token::String); // body
		tree.push_child(Token::Quote); // closure

		tree.select(body); // === tree.down(), tree.next()
		tree.last().begin() = iter->end() + 1;
		for (++iter; iter->type != Token::End &&
			(iter->type != Token::Quote || *iter != opening); ++iter)
			if (iter->type == Token::SingleOperator && iter->begin().chr() == '\\')
				++iter;
		if (iter->type != Token::Quote)
			throw Error("quote is not closed");

		tree.last().end() = iter->begin();
		tree.next();
		tree.last() = *iter;

		++iter;
		tree.up();
	}
	
	void parseBody() {
		std::cout << tree << std::endl;
		skip_spaces_and_lines();

		switch (iter->type) {
		case Token::End: throw Error::ok();
		case Token::Line:
			break;
		case Token::Operator:
			break;
		case Token::SingleOperator:
			break;
		case Token::String:
			break;
		case Token::Quote:
			parseQuote();
			break;
		case Token::OpenBrace:
			parseBraces();
			break;
		case Token::CloseBrace:
			return;
		case Token::Space: case Token::Container: default:
			throw Error("unexpected token type: "s + Token::type2str(iter->type));
		case Token::Error:
			throw Error(std::string(&iter->begin().chr(), &iter->end().chr()));
		}
		tree.up();
	}

	Error parse(void(Parser::*f)() = &Parser::parseBody) {
		try {
			(this->*f)();
		}
		catch (const Error& e) {
			return e;
		}
		return Error::ok();
	}

	void parseBraces() {
		Source opening = tree.push_child(*iter); // opening
		deep_spaces();
		TreePtr body = tree | tree.push_child(Token::Container); // body
		tree.push_child(Token::CloseBrace); // closure

		tree.select(body); // === tree.down(), tree.next()
		++iter;
		parseBody();

		tree.next(); // select closure
		skip_spaces();
		if (iter->type != Token::CloseBrace) // add open/close check
			throw Error("braces is not closed");
		tree.last() = *iter;
		++iter;
		tree.up();
	}
};

} // namespace cf

using namespace cf;

struct A {
	TreePtr parent;
	WeakSpan<A, TreePtr> children;
	int v;

	A(TreePtr p, int x) : parent(p), v(x), children(0, 0) {}
};

std::ostream& operator<<(std::ostream& stream, const A& a) {
	return stream << a.v;
}

void tree_test() {
	Tree<A> t;
	char cmd;
	int v;
	do {
		std::cin >> cmd;
		switch (cmd) {
		case 'c':
			std::cin >> v;
			t.push_child(v);
			break;
		case 's':
			std::cin >> v;
			t.push_sibling(v);
			break;
		case 'p':
			std::cin >> v;
			t.push_parent(v);
			break;
		case 'u':
			t.up();
			break;
		case 'n':
			t.next();
			break;
		case 'b':
			t.prev();
			break;
		case 'q': case 'e':
			cmd = '\0';
			break;
		}
		std::cout << t << std::endl;
	} while (cmd);
}

int main() {
	//tree_test();
	//return 0;

	Lexer l = Lexer::from_file("example.fc");
	Tree<Node> tree;
	Parser p(tree, l);
	Parser::Error e = p.parse();
	if (!e.is_ok())
		std::cerr << e.message << std::endl;
	std::cout << tree << std::endl;
	return 0;
}
