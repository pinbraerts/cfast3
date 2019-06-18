#ifndef CFAST_PARSER_HPP
#define CFAST_PARSER_HPP

#include "SyntaxNode.hpp"
#include "Lexer.hpp"
#include "Tree.hpp"
#include "ParserTraits.hpp"

namespace cf {
	
struct Parser {
	SyntaxTree& tree;
	Lexer::Iter iter;
	ParserTraits traits;
	bool eat_lines = false;

	Parser(SyntaxTree& _tree, Lexer& lexer): tree(_tree), iter(lexer.begin()) {
		tree.push_child(Token::Container); // root
	}

	struct Error {
		std::string message;

		Error(std::string msg) noexcept: message(std::move(msg)) {}
		Error() = default;

		static Error ok() noexcept {
			return Error();
		}

		bool is_ok() const noexcept {
			return message.empty();
		}
	};

	void eat_spaces() {
		for (; iter->type != Token::End &&
			(
				iter->type == Token::Space ||
				(eat_lines && iter->type == Token::Line)
			); ++iter) {
			tree.push_child(*iter);
		}
		// Use node.working() instead
		// if (tree.last().children.empty())
			// tree.push_child(Token::Space);
		eat_lines = false;
	}

	void parseQuote() {
		tree.last() = Token(Token::Quote);
		const Source opening = tree.push_child(*iter); // opening
		const TreePtr body = tree | tree.push_child(Token::String); // body

		tree.select(body); // === tree.down(), tree.next()
		tree.last().begin() = iter->end();
		for (++iter; iter->type != Token::End &&
			(iter->type != Token::Quote || *iter != opening); ++iter)
			if (iter->type == Token::Operator && iter->begin().chr() == '\\')
				++iter;
		if (iter->type != Token::Quote)
			throw Error("quote is not closed");

		tree.last().end() = iter->begin();

		tree.up();
		tree.push_child_and_select(Token::Quote); // closure
		tree.last() = *iter;
		tree.up();
		tree.up();

		++iter;
	}

	void parseBody() {
		while (true) {
			eat_lines = true;
			tree.push_child_and_select(Token::Space);
			eat_spaces();

			switch (iter->type) {
			case Token::End:
				tree.last() = Token::End;
				while (tree.last().parent != 0)
					tree.move_up();
				throw Error::ok();
			case Token::Operator:
			{
				TreePtr container = tree.last().parent;
				tree.last() = *iter;
				priority_t p = traits.get_priority(*iter);
				tree.last().priority = p;

				if (p != tree[container].priority) {
					for (; p > tree[container].priority && container; container = tree[container].parent)
						tree.move_up();
					if (p != tree[container].priority) {

						auto& ch = tree[container].children;
						auto i = ch.end();
						do --i;
						while (ch.begin() < i && (tree[*i].priority <= p));
						if (tree[*i].priority > p)
							++i;

						tree.select(tree.insert_capture(i, Token::Container, p));
					}
					else tree.up();
				}
				else tree.up();
				++iter;
			}
			break;
			case Token::CloseBrace:
				return;
			case Token::String:
				parseString();
				break;
			case Token::Quote:
				parseQuote();
				break;
			case Token::OpenBrace:
				parseBraces();
				break;
			case Token::Space:
			case Token::Container: default:
				throw Error("unexpected token type: "s + Token::type2str(iter->type));
			case Token::Error:
				throw Error(std::string(iter->begin().ptr(), iter->end().ptr()));
			}
		}
	}

	Error parse(void(Parser::*f)() = &Parser::parseBody) {
		if (f == nullptr)
			return Error("Cannot call null parser");
		try {
			(this->*f)();
		}
		catch (const Error& e) {
			return e;
		}
		catch (const std::exception& e) {
			return Error(e.what());
		}
		return Error::ok();
	}

	void parseString() {
		tree.last() = *iter;
		++iter;
		tree.up();
	}
	void parseBraces() {
		tree.last() = Token(Token::Container);
		tree.last().priority = 100;
		const TreePtr container = tree.last_position();
		tree.push_child(*iter, 100); // opening

		++iter;
		//tree.push_child_and_select(Token::Container);
		parseBody();
		//tree.up();
		if (iter->type != Token::CloseBrace) // add open/close check
			throw Error("braces is not closed");

		tree.last() = *iter;
		while (tree.last().parent != container && tree.last().parent != 0)
			tree.move_up();
		++iter;
		tree.up();
		tree.last().priority = 0;
		tree.up();
	}
};

} // namespace cf

#endif // !CFAST_PARSER_HPP