#ifndef CFAST_PARSER_HPP
#define CFAST_PARSER_HPP

#include "SyntaxNode.hpp"
#include "Lexer.hpp"
#include "Tree.hpp"

namespace cf {

struct Parser {
	Tree<SyntaxNode>& tree;
	Lexer::Iter iter;

	Parser(Tree<SyntaxNode>& _tree, Lexer& lexer): tree(_tree), iter(lexer.begin()) {
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
		SyntaxNode& child = tree.pool[pchild];
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

#endif // !CFAST_PARSER_HPP
